#pragma once
#include "audioFrame.h"
#include "videoFrame.h"
#include "ObjectPoolEx.h"

class CapturePoolMgr
{
	ObjectPoolEx<VideoFrame>                m_captureVideoPoolHD;
	ObjectPoolEx<AudioFrame>                m_captureAudioPool;
	int m_nBufferSize = 0;
public:
	CapturePoolMgr();
	~CapturePoolMgr();
	void initialize(FPTVideoFormat _fpFormat, int nPixFormat, int _size);
	void getNew(VideoFrame* & _elem, FPTVideoFormat _fpFormat);
	void getNew(AudioFrame* & _elem);
	void release(AudioFrame* _elem);
	void release(VideoFrame* _elem); 
	bool waitDataEvent(int nTimeout) {
	return	m_captureVideoPoolHD.waitDataEvent(nTimeout);
	}
	uint32_t getVideoPoolSize(FPTVideoFormat _fpFormat);
	uint32_t getAudioPoolSize();
};
