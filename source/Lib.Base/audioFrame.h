#pragma once
#include <windows.h>
#include <memory>
#include <atomic>
using namespace std;

class AudioFrame
{
	UINT64             m_tag;
	short*             m_raw;
	UINT64             m_nFrameID;
	unsigned long      m_totalSize;
	unsigned long      m_dataSize;
	UINT32             m_monoCnt;
	UINT32             m_sampleCnt;
	static void  get2AudioCnl(int sample, long* srcBuffer, long* dstBuffer, int cn1ID, int cn2ID);
	void reAlloc(UINT32 size);

public:
	AudioFrame();
	~AudioFrame();
	short*			getRaw() const;
	void			setToMute() const;
	void			SetFrameID(UINT64  nFrameID);
	void			SetMonoCnt(UINT32 cnt);
	void			SetSampleCnt(UINT32  sampleCnt);
	UINT32			GetMonoCnt() const;
	UINT64			GetFrameID() const;
	unsigned long	getBufferTotalSize() const;
	unsigned long	getSampleCount() const;
	bool			isMute() const;
	unsigned long	getDataSize() const;
	void			setBufferSize(UINT32 size);
	void			trimStero(int steroID);
	UINT64			getTag() const;
	void			setTag(UINT64 _tag);
};

typedef std::shared_ptr<AudioFrame> pAframe;
