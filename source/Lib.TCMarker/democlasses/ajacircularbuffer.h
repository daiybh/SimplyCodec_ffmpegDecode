////////////////////////////////////////////////////////////
//
//	Copyright (C) 2005 AJA Video Systems, Inc.  
//	Proprietary and Confidential information.
//
// This implements a Producer/Consumer algorithm so
// we can be filling up a circular buffer on one end
// and emptying it on the other and not have to worry
// about synchronization
////////////////////////////////////////////////////////////
#ifndef CAJACircularBuffer_H
#define CAJACircularBuffer_H

typedef struct {
	uint32_t*			videoBuffer;
	uint32_t			videoBufferSize;
	uint32_t*			audioBuffer;
	uint32_t			audioBufferSize;
	uint32_t			audioRecordSize;
	uint32_t			frameIndex;
	uint32_t			flags;			// user flags

} AJAAVDataBuffer;

#define AJA_DEFAULT_CIRCBUFFER_SIZE 18

#if defined AJA_WINDOWS

#include <windows.h>
#include <vector>
#include <string>
typedef std::vector<HANDLE> AJABufferMutexList;
typedef std::vector<std::string> AJAStringList;

// Wait with timeout and check to see if abortFlag is set..
// else continue waiting.
#define WAIT_OR_DIE(handle,abortFlag,timeOut,gotoplace)\
do {					\
	DWORD waitStatus = WaitForSingleObject(handle,timeOut); \
    if (waitStatus == WAIT_TIMEOUT)							\
		if ( abortFlag )									\
			if ( *abortFlag)								\
				goto gotoplace;								\
    if (waitStatus == WAIT_ABANDONED )						\
			goto gotoplace;									\
   if (waitStatus == WAIT_OBJECT_0 )						\
		break;												\
 } while(1) 

#define WAIT_TIMEOUT_MS  100

template<typename T>
class CAJACircularBuffer {
public:
	CAJACircularBuffer()
		: _head(0),
		_tail(0),
		_fillIndex(0),
		_emptyIndex(0),
		_notFullEvent(NULL),
		_notEmptyEvent(NULL),
		_dataBufferMutex(NULL),
		_circBufferCount(0),
		_pAbortFlag(NULL)
	{
		_dataBufferMutex = CreateMutex(NULL,FALSE,NULL);
		_notFullEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		_notEmptyEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	}

	virtual ~CAJACircularBuffer()
	{
		CloseHandle(_dataBufferMutex);
		CloseHandle(_notFullEvent);
		CloseHandle(_notEmptyEvent);
		AJABufferMutexList::iterator mutIter;
		for ( mutIter = _xenaBufferMutexList.begin(); mutIter != _xenaBufferMutexList.end(); mutIter++ )
		{
			CloseHandle(*mutIter);
		}

	}
	
	void Init(int* abortFlag=NULL)
	{
		_pAbortFlag = abortFlag;
		_xenaBufferList.clear();
		_xenaBufferMutexList.clear();
		_head = 0;
		_tail = 0;
		_fillIndex = 0;
		_emptyIndex = 0;
		_circBufferCount = 0;

	}

	bool Add(T buffer)
	{
		_xenaBufferList.push_back(buffer);
		HANDLE mutex = CreateMutex(NULL,FALSE,NULL);
		if ( mutex == NULL )
			return false;

		_xenaBufferMutexList.push_back(mutex);
		return true;
	}

	T StartProduceNextBuffer()
	{
		while (1)
		{

			WAIT_OR_DIE(_dataBufferMutex,_pAbortFlag,WAIT_TIMEOUT_MS,done);	
			if ( _circBufferCount == _xenaBufferList.size())
			{
				ReleaseMutex(_dataBufferMutex);
				WAIT_OR_DIE(_notFullEvent,_pAbortFlag,WAIT_TIMEOUT_MS,done);
				continue;
			}
			break;
		}
		WAIT_OR_DIE(_xenaBufferMutexList[_head],_pAbortFlag,WAIT_TIMEOUT_MS,done);
		_fillIndex = _head;
		_head = (_head+1)%((unsigned int)(_xenaBufferList.size()));
		_circBufferCount++;
		ReleaseMutex(_dataBufferMutex);

		return _xenaBufferList[_fillIndex];

done:
		return NULL;
	}

	void EndProduceNextBuffer()
	{
		ReleaseMutex(_xenaBufferMutexList[_fillIndex]);
		PulseEvent(_notEmptyEvent);

	}

	T StartConsumeNextBuffer()
	{
		while (1)
		{
			WAIT_OR_DIE(_dataBufferMutex,_pAbortFlag,WAIT_TIMEOUT_MS,done);	
			if ( _circBufferCount == 0)
			{
				ReleaseMutex(_dataBufferMutex);
				WAIT_OR_DIE(_notEmptyEvent,_pAbortFlag,WAIT_TIMEOUT_MS,done);
				continue;
			}
			break;
		}
		WAIT_OR_DIE(_xenaBufferMutexList[_tail],_pAbortFlag,WAIT_TIMEOUT_MS,done);
		_emptyIndex = _tail;
		_tail = (_tail+1)%((unsigned int)_xenaBufferList.size());
		_circBufferCount--;
		ReleaseMutex(_dataBufferMutex);

		return _xenaBufferList[_emptyIndex];

done:
		return NULL;
	}

	void EndConsumeNextBuffer()
	{
		ReleaseMutex(_xenaBufferMutexList[_emptyIndex]);
		PulseEvent(_notFullEvent);
	}

	bool IsEmpty(void)
	{
		return _circBufferCount == 0;
	}
	
	unsigned int GetCircBufferCount()
	{
		return _circBufferCount;
	}

protected:


	std::vector<T>							_xenaBufferList;
	AJABufferMutexList						_xenaBufferMutexList;

	unsigned int							_head;
	unsigned int							_tail;
	unsigned int							_circBufferCount;
	HANDLE									_notFullEvent;
	HANDLE                                  _notEmptyEvent;
	HANDLE									_dataBufferMutex;

	unsigned int							_fillIndex;
	unsigned int							_emptyIndex;

	const int*								_pAbortFlag;
};


#else 

#include <pthread.h>
#include <vector>
#include <string>

typedef std::vector<pthread_mutex_t*> AJABufferMutexList;

template<typename T> 
class CAJACircularBuffer {
public:
	CAJACircularBuffer()
		: _head(0),
		_circBufferCount(0),
		_tail(0),
		_fillIndex(0),
		_emptyIndex(0),
		_pAbortFlag(NULL)
	{
		pthread_mutex_init(&_dataBufferMutex,NULL);
		pthread_cond_init(&_notFullEvent,NULL);
		pthread_cond_init(&_notEmptyEvent,NULL);

	}

	virtual ~CAJACircularBuffer()
	{

		pthread_mutex_destroy(&_dataBufferMutex);
		pthread_cond_destroy(&_notFullEvent);
		pthread_cond_destroy(&_notEmptyEvent);
		AJABufferMutexList::iterator mutIter;
		for ( mutIter = _ajaBufferMutexList.begin(); mutIter != _ajaBufferMutexList.end(); mutIter++ )
		{
			pthread_mutex_destroy(*mutIter);
		}

	}

	void Init(int* abortFlag=NULL)
	{
		_pAbortFlag = abortFlag;
		_ntv4BufferList.clear();
		_ajaBufferMutexList.clear();
		_head = 0;
		_tail = 0;
		_fillIndex = 0;
		_emptyIndex = 0;
		_circBufferCount = 0;

	}

	bool Add(T buffer)
	{
		uint32_t numBuffers = _ajaBufferMutexList.size();
		_ntv4BufferList.push_back(buffer);
		pthread_mutex_init(&_fbMutexArray[numBuffers],NULL);
		_ajaBufferMutexList.push_back(&_fbMutexArray[numBuffers]);

		return true;
	}

	T StartProduceNextBuffer()
	{
		pthread_mutex_lock(&_dataBufferMutex);
		while ( _circBufferCount >= _ntv4BufferList.size())
		{
			//printf("pwaiting\n");
			pthread_cond_wait(&_notFullEvent,&_dataBufferMutex);
		}
		pthread_mutex_lock(_ajaBufferMutexList[_head]);
		_fillIndex = _head;
		_head = (_head+1)%((unsigned int)(_ntv4BufferList.size()));
		_circBufferCount++;
		pthread_mutex_unlock(&_dataBufferMutex);

		return _ntv4BufferList[_fillIndex];


	}

	void EndProduceNextBuffer()
	{
		pthread_cond_signal(&_notEmptyEvent);
		pthread_mutex_unlock(_ajaBufferMutexList[_fillIndex]);

	}

	T StartConsumeNextBuffer()
	{
		pthread_mutex_lock(&_dataBufferMutex);	
		while ( _circBufferCount <= 0)
		{
			//printf("cwaiting\n");
			pthread_cond_wait(&_notEmptyEvent,&_dataBufferMutex);
		}
		pthread_mutex_lock(_ajaBufferMutexList[_tail]);
		_emptyIndex = _tail;
		_tail = (_tail+1)%((unsigned int)_ntv4BufferList.size());
		_circBufferCount--;
		pthread_mutex_unlock(&_dataBufferMutex);

		return _ntv4BufferList[_emptyIndex];

	}

	void EndConsumeNextBuffer()
	{
		pthread_cond_signal(&_notFullEvent);
		pthread_mutex_unlock(_ajaBufferMutexList[_emptyIndex]);
	}

	bool IsEmpty(void)
	{
		return _circBufferCount == 0;
	}

	unsigned int GetCircBufferCount()
	{
		return _circBufferCount;
	}

protected:


	std::vector<T>							_ntv4BufferList;
	AJABufferMutexList						_ajaBufferMutexList;

	unsigned int							_head;
	unsigned int							_tail;
	unsigned int							_circBufferCount;
	pthread_cond_t							_notFullEvent;
	pthread_cond_t                          _notEmptyEvent;
	pthread_mutex_t							_dataBufferMutex;
	pthread_mutex_t							_fbMutexArray[32];
	unsigned int							_fillIndex;
	unsigned int							_emptyIndex;

	const int*								_pAbortFlag;
};


#endif



#endif
