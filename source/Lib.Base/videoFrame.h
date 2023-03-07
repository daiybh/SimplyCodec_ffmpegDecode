#pragma once
#include <windows.h>
#include <memory>
#include "videoFormat.h"

using namespace std;
class VideoFrame
{
	UINT64			m_nFrameID = 0;
	FPTVideoFormat       m_fpFormat = FP_FORMAT_1080i_5000;
	bool			     m_isBlackFrame = false;
	unsigned char *		 m_pFrame = nullptr;
	int32_t				 m_totalSize = 0;
	int32_t				 m_usedSize = 0;
	uint32_t			 m_videoWidth = 0;
	uint32_t			 m_videoHeight = 0;
	uint32_t             m_rowBytes = 0;
public:
	VideoFrame();
	virtual ~VideoFrame();
	int32_t getRowBytes() { return m_rowBytes; }
	void setRowBytes(uint32_t rowBytes) { m_rowBytes = rowBytes; }
	unsigned long			getWidth() const;
	unsigned long			getHeight() const;
	unsigned char*			getRaw() const;
	unsigned long			getRawSize() const;
	int						setBufferSize(unsigned long _size);
	void					setWidth(uint32_t _videoWidth);
	void					setHight(uint32_t _videoHeight);
	int						setToBlack(bool _flagOnly = false);
	void					resetBlackFlag();
	bool					isBlackFrame() const;
	void SetFrameID(UINT64  nFrameID);
	UINT64 GetFrameID() const;
	void setFormat(const FPTVideoFormat fpFormat);
	FPTVideoFormat getFormat() const;
};

typedef shared_ptr<VideoFrame> pVFrame;
