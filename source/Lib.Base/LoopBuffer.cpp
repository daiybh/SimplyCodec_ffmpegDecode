// LoopBuffer.cpp: implementation of the CLoopBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "LoopBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoopBuffer::CLoopBuffer()
{
	m_pBuffer = NULL;
	m_iBufLen = 1;
	RestBufferPos();
	InitializeCriticalSectionAndSpinCount(&CriticalSection, 4000);		
}

CLoopBuffer::~CLoopBuffer()
{
	LeaveCriticalSection(&CriticalSection);	
	DeleteCriticalSection(&CriticalSection);
	UnInitBuffer();
}

BOOL CLoopBuffer::InitBuffer(int iSize)
{
	if (iSize <= 0)
		return FALSE;

	m_iBufLen = iSize;
	m_pBuffer = new BYTE[m_iBufLen];
	memset(m_pBuffer, 0, m_iBufLen);
	RestBufferPos();
	return TRUE;
}

void CLoopBuffer::RestBufferPos()
{
	m_iWPointer = 0;
	m_iRPointer = 0;
	m_i64ReadedCnt = 0;
	m_i64WritedCnt = 0;
}

int	CLoopBuffer::ReadBuffer(LPBYTE pBuf, int iReadSize, BOOL bAll)
{
	EnterCriticalSection(&CriticalSection); 

	int iRDis = GetReadDistance();
	if (bAll)
	{
		iReadSize = iRDis;
	}

	if (iRDis < iReadSize)
	{
		LeaveCriticalSection(&CriticalSection);	
		return 0;
	}	
	
	Memcopy(iReadSize, pBuf, TRUE);
	m_i64ReadedCnt += iReadSize;
	LeaveCriticalSection(&CriticalSection);	
	return iReadSize;
}

int	CLoopBuffer::WriteBuffer(LPBYTE pBuf, int iWriteSize)
{
	EnterCriticalSection(&CriticalSection); 
	int iWDis = GetWriteDistance();
	if (iWDis < iWriteSize)
	{
		LeaveCriticalSection(&CriticalSection);	
		return -1;
	}
	
	Memcopy(iWriteSize, pBuf);
	m_i64WritedCnt+= iWriteSize;
	LeaveCriticalSection(&CriticalSection);	
	return iWriteSize;
}

BOOL CLoopBuffer::UnInitBuffer()
{
	RestBufferPos();
	if (m_pBuffer != NULL)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	return TRUE;
}

BOOL CLoopBuffer::ResetBuffer()
{
	EnterCriticalSection(&CriticalSection); 
	m_iWPointer = m_iRPointer = 0;
	m_i64ReadedCnt = m_i64WritedCnt =0;
	LeaveCriticalSection(&CriticalSection);	
	return TRUE;
}

int	CLoopBuffer::GetReadDistance()
{
	int iDis = (m_iWPointer + m_iBufLen - m_iRPointer) %m_iBufLen;
	return iDis;
}

int	CLoopBuffer::GetWriteDistance()
{
	int iDis = (((m_iRPointer + m_iBufLen) - m_iWPointer) - 1) %m_iBufLen;
	return iDis;
}

BOOL CLoopBuffer::Memcopy(int iLen, LPBYTE pBuf, BOOL bRead)
{
	int iRemain = 0;
	int iStart = 0;
	if (bRead)
	{		
		iStart = m_iRPointer;		
		iRemain = m_iBufLen - iStart;
		if (iRemain >= iLen)
		{
			memcpy(pBuf, m_pBuffer+iStart, iLen);			
			m_iRPointer = iStart + iLen;			
		}
		else
		{		
			memcpy(pBuf, m_pBuffer+iStart, iRemain);
			
			int iHead = iLen - iRemain;
			memcpy(pBuf+iRemain, m_pBuffer, iHead);
			m_iRPointer = iHead;			
		}
	}
	else
	{
		iStart = m_iWPointer;		
		iRemain = m_iBufLen - iStart;
		if (iRemain >= iLen)
		{
			memcpy(m_pBuffer+iStart, pBuf, iLen);			
			m_iWPointer = iStart + iLen;		
		}
		else
		{		
			memcpy(m_pBuffer+iStart, pBuf, iRemain);
			
			int iHead = iLen - iRemain;
			memcpy(m_pBuffer, pBuf+iRemain, iHead);			
			m_iWPointer = iHead;			
		}
	}
	return TRUE;
}

BOOL CLoopBuffer::CanGetAudioData(int nSize)
{
	EnterCriticalSection(&CriticalSection); 
	int iRemain = GetReadDistance();
	if (iRemain < nSize)
	{
		LeaveCriticalSection(&CriticalSection);	
		return FALSE;
	}

	LeaveCriticalSection(&CriticalSection);	
	return TRUE;
}