#include "CapturePoolMgr.h"
#include "locker.h"
#include "videoFormat.h"
#include "..\lib.Import\AudioSampleHeader.h"
using namespace std;

CapturePoolMgr::CapturePoolMgr()
{
}

CapturePoolMgr::~CapturePoolMgr()
{
}

void CapturePoolMgr::initialize(FPTVideoFormat _fpFormat, int nPixFormat, int _size)
{
	m_captureVideoPoolHD.initialize<VideoFrame>(_size);

	int nWidth = FFMPEG_AUDIOSAMPLE::getFrameWidth(_fpFormat);
	int nHeight= FFMPEG_AUDIOSAMPLE::getFrameHeight(_fpFormat);
	m_nBufferSize = 5529600;// 5693440;;//10bits
	int rowBytes = nWidth * 2;
	if (nPixFormat == 1) m_nBufferSize = nWidth * nHeight * 2;
	else if(nPixFormat==0)
	{
		rowBytes = ((nWidth + 47) / 48) * 128;
		m_nBufferSize = rowBytes* nHeight;
	}

	for (int i = 0; i < _size; i++)
	{
		VideoFrame* frame = nullptr;
		m_captureVideoPoolHD.getNew(frame);
		frame->setBufferSize(m_nBufferSize);
		frame->setWidth(nWidth);
		frame->setHight(nHeight);
		frame->setRowBytes(rowBytes);
		frame->setFormat(_fpFormat);
		m_captureVideoPoolHD.release(frame);
	}
	m_captureAudioPool.initialize<AudioFrame>(_size);
}

void CapturePoolMgr::getNew(VideoFrame* & _elem, FPTVideoFormat fpFormat)
{
	m_captureVideoPoolHD.getNew(_elem);

	if (_elem)
	{

		_elem->setFormat(fpFormat);

		int nWidth = FFMPEG_AUDIOSAMPLE::getFrameWidth(fpFormat);
		int nHeight = FFMPEG_AUDIOSAMPLE::getFrameHeight(fpFormat);
		m_nBufferSize = max(nWidth*nHeight * 2, m_nBufferSize);
		_elem->setBufferSize(m_nBufferSize);
		_elem->setWidth(nWidth);
		_elem->setHight(nHeight);
		//_elem->resetAncInfo();
	}
}


void CapturePoolMgr::getNew(AudioFrame* & _elem)
{
	m_captureAudioPool.getNew(_elem);
}

void CapturePoolMgr::release(AudioFrame* _elem)
{
	//WriteLogA(L"C:\\logs\\playerYUV\\releaseFrame.log", LOGLEVEL::Error, "need release audio one 0x%x", _elem);
	if (_elem != nullptr)
		m_captureAudioPool.release(_elem);
}

void CapturePoolMgr::release(VideoFrame* _elem)
{
	if (_elem == nullptr)
		return;
	m_captureVideoPoolHD.release(_elem);
	
}

uint32_t CapturePoolMgr::getVideoPoolSize(FPTVideoFormat _fpFormat)
{
	return  m_captureVideoPoolHD.size();// : m_captureVideoComp.size();
}

uint32_t CapturePoolMgr::getAudioPoolSize()
{
	return m_captureAudioPool.size();
}