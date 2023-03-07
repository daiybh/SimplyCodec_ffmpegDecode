#include "audioFrame.h"
#include <malloc.h>

AudioFrame::AudioFrame()
	:m_totalSize(1920 * 16 * 4) //16 audio channels , and simple bit depth is 4 BYTES
{
	m_raw = (short*)malloc(m_totalSize);
	m_dataSize = 0;
	m_monoCnt = 16;
	m_sampleCnt = 1920;
}
void AudioFrame::reAlloc(UINT32 size)
{
	if (m_raw)
		free(m_raw);

	m_raw = (short*)malloc(size);
	m_totalSize = size;
}
AudioFrame::~AudioFrame()
{
	free(m_raw);
}

short* AudioFrame::getRaw() const
{
	return m_raw;
}

bool AudioFrame::isMute() const
{
	BYTE * pBuf = (BYTE*)m_raw;
	for (int i = 0; i < m_dataSize; i++)
	{
		if (pBuf[i])
			return false;
	}
	return true;
}

unsigned long AudioFrame::getDataSize() const
{
	return m_dataSize;
}

void AudioFrame::setBufferSize(UINT32 size)
{
	m_dataSize = size;
	m_sampleCnt = m_dataSize / (m_monoCnt * 4);
	if (m_dataSize > m_totalSize)
	{
		reAlloc(size);
	}
}

void AudioFrame::trimStero(int steroID)
{
	get2AudioCnl(m_dataSize / (16 * 4), (long*)m_raw, (long*)m_raw, steroID * 2, steroID * 2 + 1);
	m_dataSize = m_dataSize / 8;
}

UINT64 AudioFrame::getTag() const
{
	return  m_tag;
}

void AudioFrame::setTag(UINT64 _tag)
{
	m_tag = _tag;
}

void AudioFrame::setToMute() const
{
	memset(m_raw, 0, m_totalSize);
}

void AudioFrame::SetFrameID(UINT64  nFrameID)
{
	m_nFrameID = nFrameID;
}

void AudioFrame::SetMonoCnt(UINT32 cnt)
{
	m_monoCnt = cnt;
	int size = cnt * m_sampleCnt * 4;
	if (size != m_dataSize)
		setBufferSize(size);
}

void AudioFrame::SetSampleCnt(UINT32 sampleCnt)
{
	m_sampleCnt = sampleCnt;
	int size = sampleCnt * 4 * m_monoCnt;
	if (size != m_dataSize)
		setBufferSize(size);
}

UINT32 AudioFrame::GetMonoCnt() const
{
	return m_monoCnt;
}

UINT64 AudioFrame::GetFrameID() const
{
	return m_nFrameID;
}

unsigned long AudioFrame::getBufferTotalSize() const
{
	return m_totalSize;
}

unsigned long AudioFrame::getSampleCount() const
{
	return m_sampleCnt;
}

void  AudioFrame::get2AudioCnl(int sample, long* srcBuffer, long* dstBuffer, int cn1ID, int cn2ID)
{
	for (int i = 0; i < sample; i++)
	{
		*dstBuffer++ = srcBuffer[cn1ID];
		*dstBuffer++ = srcBuffer[cn2ID];
		srcBuffer += 16;
	}
}