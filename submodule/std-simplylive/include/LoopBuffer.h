#pragma once
#include <cstdint>
#include "locker.h"

class LoopBuffer
{
public:
	LoopBuffer() = default;

	~LoopBuffer()
	{
		unInit();
	}

	bool init(int32_t iSize)
	{
		if (m_bBufInited)
		{
			reset();
			return true;
		}

		if (iSize <= 0)
			return false;
		m_iBufLen = iSize;
		m_pBuffer = new uint8_t[m_iBufLen];
		memset(m_pBuffer, 0, m_iBufLen);
		reset();
		m_bBufInited = true;
		return true;
	}

	int32_t read(uint8_t* pBuf, int32_t iReadSize = -1, bool bAll = false)
	{
		LockHolder lock(m_lock);

		int32_t iRDis = GetReadDistance();
		if (bAll)
			iReadSize = iRDis;

		if (iRDis < iReadSize)
			return 0;

		copyData(iReadSize, pBuf, true);
		m_i64ReadedCnt += iReadSize;
		return iReadSize;
	}

	int32_t write(uint8_t* pBuf, int32_t iWriteSize)
	{
		LockHolder lock(m_lock);
		int32_t    iWDis = GetWriteDistance();
		if (iWDis < iWriteSize)
			return -1;

		copyData(iWriteSize, pBuf);
		m_i64WritedCnt += iWriteSize;
		return iWriteSize;
	}

	bool unInit()
	{
		reset();
		if (m_pBuffer != nullptr)
		{
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
		}
		m_bBufInited = false;
		return true;
	}

	bool reset()
	{
		LockHolder lock(m_lock);
		m_iWPointer = m_iRPointer = 0;
		m_i64ReadedCnt = m_i64WritedCnt = 0;
		return true;
	}

	bool hasInited() const
	{
		return m_bBufInited;
	}

	uint64_t getWritedCnt() const
	{
		return m_i64WritedCnt;
	}

	uint64_t getReadedCnt() const
	{
		return m_i64ReadedCnt;
	}

	uint64_t getSizeOfDataList() const
	{
		return m_i64WritedCnt - m_i64ReadedCnt;
	}

	int32_t getFreeSize()
	{
		LockHolder lock(m_lock);
		return GetWriteDistance();
	}

	int32_t getDataSize()
	{
		LockHolder lock(m_lock);
		return GetReadDistance();
	}

	void moveReaderPos(int32_t iLen)
	{
		LockHolder lock(m_lock);
		int32_t    iStart = m_iRPointer;
		int32_t    iRemain = m_iBufLen - iStart;
		if (iRemain >= iLen)
		{
			m_iRPointer = iStart + iLen;
			if (m_iRPointer < 0)
				m_iRPointer = m_iRPointer + m_iBufLen;
		}
		else
		{
			int32_t iHead = iLen - iRemain;
			m_iRPointer = iHead;
		}
	}

private:
	uint8_t* m_pBuffer = nullptr;
	int32_t  m_iBufLen = 1;
	int32_t  m_iWPointer = 0;
	int32_t  m_iRPointer = 0;
	int64_t  m_i64WritedCnt = 0;
	int64_t  m_i64ReadedCnt = 0;
	bool     m_bBufInited = false;
	Locker   m_lock;

	int32_t GetReadDistance() const
	{
		int32_t iDis = (m_iWPointer + m_iBufLen - m_iRPointer) % m_iBufLen;
		return iDis;
	}

	int32_t GetWriteDistance() const
	{
		int32_t iDis = (((m_iRPointer + m_iBufLen) - m_iWPointer) - 1) % m_iBufLen;
		return iDis;
	}

	bool copyData(int32_t iLen, uint8_t* pBuf, bool bRead = false)
	{
		int32_t iRemain = 0;
		int32_t iStart = 0;
		if (bRead)
		{
			iStart = m_iRPointer;
			iRemain = m_iBufLen - iStart;
			if (iRemain >= iLen)
			{
				memcpy(pBuf, m_pBuffer + iStart, iLen);
				m_iRPointer = iStart + iLen;
			}
			else
			{
				memcpy(pBuf, m_pBuffer + iStart, iRemain);

				int32_t iHead = iLen - iRemain;
				memcpy(pBuf + iRemain, m_pBuffer, iHead);
				m_iRPointer = iHead;
			}
		}
		else
		{
			iStart = m_iWPointer;
			iRemain = m_iBufLen - iStart;
			if (iRemain >= iLen)
			{
				memcpy(m_pBuffer + iStart, pBuf, iLen);
				m_iWPointer = iStart + iLen;
			}
			else
			{
				memcpy(m_pBuffer + iStart, pBuf, iRemain);

				int32_t iHead = iLen - iRemain;
				memcpy(m_pBuffer, pBuf + iRemain, iHead);
				m_iWPointer = iHead;
			}
		}
		return true;
	}
};
