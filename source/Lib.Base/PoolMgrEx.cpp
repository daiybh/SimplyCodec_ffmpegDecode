#include "PoolMgrEx.h"



PoolMgrEx::PoolMgrEx()
{
}


PoolMgrEx::~PoolMgrEx()
{
}


PoolMgrEx * PoolMgrEx::GetInstance()
{
	static PoolMgrEx *gSInstance = nullptr;
	static Locker g_lock;
	LockHolder lock(g_lock);
	if (gSInstance == nullptr)
	{
		gSInstance = new PoolMgrEx();
	}
	return gSInstance;
}

void PoolMgrEx::initialize(FPTVideoFormat _fpFormat,int nPixFormat, ChannelMask mask, int _size)
{
	for (int _cnl = 0; _cnl < EM_ConsumerChannel_MAX; ++_cnl)
	{
		if (mask[_cnl])
			m_pool[_cnl].initialize(_fpFormat,nPixFormat, _size);
	}
}

CapturePoolMgr* PoolMgrEx::getPoolMgr(int nCnl)
{
	return &m_pool[nCnl];
}

void PoolMgrEx::getNew(int nCnl, VideoFrame* & _elem, FPTVideoFormat _fpFormat)
{
	m_pool[nCnl].getNew(_elem,_fpFormat);
}

void PoolMgrEx::getNew(int nCnl, AudioFrame* & _elem)
{
	m_pool[nCnl].getNew(_elem);
}

void PoolMgrEx::release(int nCnl, AudioFrame* _elem)
{
	m_pool[nCnl].release(_elem);
}

void PoolMgrEx::release(int nCnl, VideoFrame* _elem)
{
	m_pool[nCnl].release(_elem);
}
