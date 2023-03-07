#pragma once
#include "../lib.base/videoFormat.h"

class IAJA_TcOSD
{
protected:
	IAJA_TcOSD() = default;
	virtual ~IAJA_TcOSD() = default;

public:
	static IAJA_TcOSD *getInstance();
	static IAJA_TcOSD * createInstance();
	virtual bool RenderTimeCodeFont(FPTVideoFormat _fpFormat, int _pixFormat)=0;
	/**
	*	Burns a timecode in with simple font 00:00:00;00.
	*
	*
	*	@param[in]	pBaseVideoAddress	Base address of Raster
	*	@param[in]	pTimeCodeString		Something like "00:00:00:00"
	*	@param[in]	percentY		    Percent down the screen. if 0, will make it 80
	*/
	virtual bool BurnTimeCode(char * pBaseVideoAddress, const char * pTimeCodeString, const int percentY)=0;
	virtual bool BurnTimeCode(char * pBaseVideoAddress, const int percentY)=0;
};
