// LoopBuffer.h: interface for the CLoopBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOPBUFFER_H__7AA9E6EC_A58F_4865_82A8_1DFEF85E4F59__INCLUDED_)
#define AFX_LOOPBUFFER_H__7AA9E6EC_A58F_4865_82A8_1DFEF85E4F59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>

class CLoopBuffer
{
public:
	CLoopBuffer();
	virtual ~CLoopBuffer();

public:
	BOOL	InitBuffer(int iSize);
	void	RestBufferPos();
	int		ReadBuffer(LPBYTE pBuf, int iReadSize = -1, BOOL bAll = FALSE);
	int		WriteBuffer(LPBYTE pBuf, int iWriteSize);
	BOOL	UnInitBuffer();
	BOOL	ResetBuffer();
	BOOL	CanGetAudioData(int nSize);
	INT64   GetWritedCnt() { return m_i64WritedCnt; }
	INT64   GetReadedCnt() { return m_i64ReadedCnt; }
	int	GetSizeOfDataList() { return m_i64WritedCnt - m_i64ReadedCnt; }
private:
	LPBYTE  m_pBuffer = nullptr;
	int		m_iBufLen = 0;
	int		m_iWPointer = 0;
	int		m_iRPointer = 0;
	INT64	m_i64WritedCnt=0;
	INT64	m_i64ReadedCnt = 0;

	int		GetReadDistance();
	int		GetWriteDistance();
	BOOL	Memcopy(int iLen, LPBYTE pBuf, BOOL bRead = FALSE);
	CRITICAL_SECTION CriticalSection;
};

#endif // !defined(AFX_LOOPBUFFER_H__7AA9E6EC_A58F_4865_82A8_1DFEF85E4F59__INCLUDED_)
