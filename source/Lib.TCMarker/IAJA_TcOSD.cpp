#include "IAJA_TcOSD.h"
#include "tcOSD.h"
#include "../lib.Base/locker.h"

static IAJA_TcOSD *mSInstance = nullptr; 
static Locker	g_lock;
IAJA_TcOSD * IAJA_TcOSD::getInstance()
{
	if (mSInstance != nullptr)
		return mSInstance;

	LockHolder lock(g_lock);
	if (mSInstance == nullptr)
		mSInstance = new AJA_TcOSD();
	return mSInstance;
}

IAJA_TcOSD * IAJA_TcOSD::createInstance()
{
	return new AJA_TcOSD();
}