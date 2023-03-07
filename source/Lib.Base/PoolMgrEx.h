#pragma once
#include "CapturePoolMgr.h"
#include <bitset>
#include "../FrameConsumer/Lib.FrameConsumer/IFrameConsumer.h"

using ChannelMask = std::bitset<EM_ConsumerChannel_MAX>;

class PoolMgrEx
{
public:
	PoolMgrEx();
	~PoolMgrEx();
	CapturePoolMgr				m_pool[EM_ConsumerChannel_MAX];
public:
	static PoolMgrEx * GetInstance();
	void initialize(FPTVideoFormat _fpFormat, int nPixFormat, ChannelMask mask, int _size);
	CapturePoolMgr* getPoolMgr(int nCnl);
	void getNew(int nCnl, VideoFrame* & _elem, FPTVideoFormat _fpFormat);
	void getNew(int nCnl, AudioFrame* & _elem);
	void release(int nCnl, AudioFrame* _elem);
	void release(int nCnl, VideoFrame* _elem);
};

