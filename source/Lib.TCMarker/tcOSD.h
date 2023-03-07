#pragma once
#include <Windows.h>
#include <wincodec.h>
#include "../lib.base/videoFormat.h"

enum WorkType
{
	WorkType_SD = 0,
	WorkType_720,
	WorkType_1080,
	WorkType_2160,
	WorkType_MAX
};

class TcOSD
{

	int m_char_Num_Width;
	int m_char_S_Width;
	int m_char_Height;
	int m_osdOffset = 0;
	WorkType  m_currentWorkType=WorkType_1080;
	int    m_ScalePercent=100;
	bool   m_bAllocaMem[WorkType_MAX] = { false };
	BYTE * m_char2160[12]={nullptr};
	BYTE * m_char1080[12] = { nullptr };
	BYTE * m_char720[12] = { nullptr };
	BYTE * m_charSD[12] = { nullptr };
	bool	m_bInitSuc[WorkType_MAX] = { false };
	BYTE * pngToYuvBuf(const wchar_t * _fileName, UINT & _stride);
	void   cleanUp(IWICBitmapDecoder * _pDecoder, IWICImagingFactory * _pFactory, IWICStream * _pStream, IWICBitmapFrameDecode * _pBitmapFrame);
	void   rgb2yuv(BYTE _R, BYTE _G, BYTE _B, BYTE & _Y, BYTE & _U, BYTE & _V);
	void   rgb2yuyv(BYTE _R1, BYTE _G1, BYTE _B1, BYTE _R2, BYTE _G2, BYTE _B2, BYTE & _Y1, BYTE & _U, BYTE & _Y2, BYTE & _V);
	void   block2yuv(BYTE * _pIn, BYTE * _pOut, int _width, int _height, int _stride);
	bool	m_bLoaded[WorkType_MAX] = { false };
	void allocationMem(WorkType workType);
public:
	TcOSD();
	~TcOSD();

	int loadFont(WorkType workType, int _scalePercent=100);
	void incrustSD(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop, bool pal, bool b404);
	void incrust720(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop);
	void incrust1080(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop);
	void incrust2160(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop);
};
class AJATimeCodeBurn;
class CVITC;
#include "IAJA_TcOSD.h"
class AJA_TcOSD:public IAJA_TcOSD
{
	AJATimeCodeBurn *m_ajaTimeCodeBurn = nullptr;
	CVITC				  *m_vitc = nullptr;
public:
	AJA_TcOSD();
	 bool RenderTimeCodeFont(FPTVideoFormat _fpFormat, int _pixFormat);
	/**
	*	Burns a timecode in with simple font 00:00:00;00.
	*
	*
	*	@param[in]	pBaseVideoAddress	Base address of Raster
	*	@param[in]	pTimeCodeString		Something like "00:00:00:00"
	*	@param[in]	percentY		    Percent down the screen. if 0, will make it 80
	*/
	 bool BurnTimeCode(char * pBaseVideoAddress, const char * pTimeCodeString, const int percentY);
	 bool BurnTimeCode(char * pBaseVideoAddress, const int percentY);
};