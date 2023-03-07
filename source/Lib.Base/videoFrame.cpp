#include "videoFrame.h"

VideoFrame::VideoFrame()
{
}

VideoFrame::~VideoFrame()
{
	if (m_pFrame)
		_aligned_free(m_pFrame);
}

unsigned long VideoFrame::getWidth() const
{
	return m_videoWidth;
}

unsigned long VideoFrame::getHeight() const
{
	return m_videoHeight;
}

unsigned char* VideoFrame::getRaw() const
{
	return m_pFrame;
}

unsigned long VideoFrame::getRawSize() const
{
	return m_usedSize;
}

int VideoFrame::setBufferSize(unsigned long _size)
{
	if (_size > m_totalSize)
	{
		if (m_pFrame != nullptr)
			_aligned_free(m_pFrame);
		m_pFrame = static_cast <unsigned char *> (_aligned_malloc(_size, 1024));
		m_totalSize = _size;
	}
	m_usedSize = _size;
	return 0;
}

void VideoFrame::setWidth(uint32_t _videoWidth)
{
	m_videoWidth = _videoWidth;
}

void VideoFrame::setHight(uint32_t _videoHeight)
{
	m_videoHeight = _videoHeight;
}

int VideoFrame::setToBlack(bool _flagOnly)
{
	if (_flagOnly)
		m_isBlackFrame = true;
	else
	{
		unsigned long size = getRawSize() / 4;
		unsigned long* dest = (unsigned long *)getRaw();

		for (unsigned long i = 0; i < size; i++)
			*dest++ = 0x10801080;
	}

	return 0;
}

void VideoFrame::resetBlackFlag()
{
	m_isBlackFrame = false;
}

bool VideoFrame::isBlackFrame() const
{
	return m_isBlackFrame;
}

void VideoFrame::SetFrameID(UINT64  nFrameID)
{
	m_nFrameID = nFrameID;
}

UINT64 VideoFrame::GetFrameID() const
{
	return m_nFrameID;
}

void VideoFrame::setFormat(const FPTVideoFormat fpFormat)
{
	m_fpFormat = fpFormat;
}

FPTVideoFormat VideoFrame::getFormat() const
{
	return m_fpFormat;
}
