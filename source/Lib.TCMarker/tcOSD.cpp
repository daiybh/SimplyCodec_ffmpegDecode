#include "tcOSD.h"

#include <stdio.h>
#include <tchar.h>
#include <PathCch.h>
#include <wincodecsdk.h>

#pragma comment(lib, "WindowsCodecs.lib")
#pragma comment(lib, "Pathcch.lib")

#define MAX_FILENAME_SIZE	250

#define W2160C	100
#define W2160S	80
#define H2160	94
#define OSD3840OFFSET	1800				// the OSD start at line 1800 in 3840

#define W1080C	50
#define W1080S	40
#define H1080	94
#define OSD1080OFFSET	900				// the OSD start at line 900 in 1080

#define W720C	50
#define W720S	40
#define H720	94
#define OSD720OFFSET	600				// the OSD start at line 600 in 720

#define W_SD_C	36
#define W_SD_S	28
#define H_SD	47
#define OSD_SD_OFFSET_PAL	500
#define OSD_SD_OFFSET_NTSC	400

TcOSD::TcOSD()
{
	CoInitialize(NULL);	// Initialize COM on this thread
	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	m_ScalePercent = 150;
	for (int i = 0; i < WorkType_MAX; i++)
	{
		m_bInitSuc[i] = false;
		m_bLoaded[i] = false;
		m_bAllocaMem[i] = false;
	}
}
void TcOSD::allocationMem(WorkType workType)
{
	if (m_bAllocaMem[workType])
		return;

	if (workType == WorkType_SD)
	{
		for (int i = 0; i < 10; i++)
			m_charSD[i] = new BYTE[W_SD_C * H_SD * 2];
		m_charSD[10] = new BYTE[W_SD_S * H_SD * 2];
		m_charSD[11] = new BYTE[W_SD_S * H_SD * 2];
	}
	else if (workType == WorkType_720)
	{
		for (int i = 0; i < 10; i++)
			m_char720[i] = new BYTE[m_char_Num_Width * m_char_Height * 2];
		m_char720[10] = new BYTE[m_char_S_Width * m_char_Height * 2];
		m_char720[11] = new BYTE[m_char_S_Width * m_char_Height * 2];
	}
	else if (workType == WorkType_1080)
	{
		for (int i = 0; i < 10; i++)
			m_char1080[i] = new BYTE[m_char_Num_Width * m_char_Height * 2];
		m_char1080[10] = new BYTE[m_char_S_Width * m_char_Height * 2];
		m_char1080[11] = new BYTE[m_char_S_Width * m_char_Height * 2];
	}
	else if (workType == WorkType_2160)
	{
		for (int i = 0; i < 10; i++)
			m_char2160[i] = new BYTE[W2160C * H2160 * 2];
		m_char2160[10] = new BYTE[W2160S * H2160 * 2];
		m_char2160[11] = new BYTE[W2160S * H2160 * 2];
	}
	m_bAllocaMem[workType] = true;
}

TcOSD::~TcOSD()
{
	for (int i = 0; i < 12; i++)
		if (m_charSD[i])free(m_charSD[i]);

	for (int i = 0; i < 12; i++)
		if (m_char720[i])free(m_char720[i]);

	for (int i = 0; i < 12; i++)
		if (m_char1080[i])free(m_char1080[i]);

	for (int i = 0; i < 12; i++)
		if (m_char2160[i])free(m_char2160[i]);
}

void TcOSD::cleanUp(IWICBitmapDecoder * _pDecoder, IWICImagingFactory * _pFactory, IWICStream * _pStream, IWICBitmapFrameDecode * _pBitmapFrame)
{
	if (_pBitmapFrame != nullptr)
		_pBitmapFrame->Release();
	if (_pStream != nullptr)
		_pStream->Release();
	if (_pFactory != nullptr)
		_pFactory->Release();
	if (_pDecoder != nullptr)
		_pDecoder->Release();
}

BYTE * TcOSD::pngToYuvBuf(const wchar_t * _fileName, UINT & _stride)
{
	IWICBitmapDecoder		* pDecoder = nullptr;
	IWICImagingFactory		* pFactory = nullptr;
	IWICStream				* pStream = nullptr;
	IWICBitmapFrameDecode	* pBitmapFrame = nullptr;
	UINT					  width, height;

	CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_ALL, IID_IWICBitmapDecoder, (void**)&pDecoder);
	CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
	pFactory->CreateStream(&pStream);

	if (pStream->InitializeFromFilename(_fileName, GENERIC_READ) != S_OK)
	{
		cleanUp(pDecoder, pFactory, pStream, pBitmapFrame);
		return nullptr;
	}

	pDecoder->Initialize(pStream, WICDecodeMetadataCacheOnDemand);

	if (pDecoder->GetFrame(0, &pBitmapFrame) != S_OK)
	{
		cleanUp(pDecoder, pFactory, pStream, pBitmapFrame);
		return nullptr;
	}

	GUID pixelFormat = { 0 };
	if (pBitmapFrame->GetPixelFormat(&pixelFormat) != S_OK)
	{
		cleanUp(pDecoder, pFactory, pStream, pBitmapFrame);
		return nullptr;
	}

	if ((pixelFormat != GUID_WICPixelFormat24bppBGR) && (pixelFormat != GUID_WICPixelFormat32bppBGRA))
	{
		cleanUp(pDecoder, pFactory, pStream, pBitmapFrame);
		return nullptr;
	}

	pBitmapFrame->GetSize(&width, &height);

	UINT size = width * height * 4;
	_stride = (width * 3 + 2) & ~2;

	BYTE * pBuffer = new BYTE[size];

	if (pixelFormat == GUID_WICPixelFormat32bppBGRA)
	{
		_stride = (width * 4 + 3) & ~3;
		BYTE * pBuffer2 = new BYTE[size];
		pBitmapFrame->CopyPixels(0, _stride, size, pBuffer2);

		BYTE * pSource = pBuffer2;
		BYTE * pDest = pBuffer;

		for (int i = width * height; i; i--)
		{
			pDest[0] = pSource[0];
			pDest[1] = pSource[1];
			pDest[2] = pSource[2];
			pDest += 3;
			pSource += 4; // skip alpha
		}
		free(pBuffer2);
		_stride = (width * 3 + 2) & ~2;
	}
	else
		pBitmapFrame->CopyPixels(0, _stride, size, pBuffer);

	cleanUp(pDecoder, pFactory, pStream, pBitmapFrame);
	return pBuffer;
}
void yuvScale(int srcW, int srcH, int destW, int destH)
{
}
int TcOSD::loadFont(WorkType workType, int _scalePercent)
{
	m_ScalePercent = _scalePercent;
	switch (workType)
	{
	case WorkType_SD:
		m_char_S_Width = W_SD_S;
		m_char_Num_Width = W_SD_C;
		m_char_Height = H_SD;
		m_osdOffset = OSD_SD_OFFSET_PAL;
		break;
	case WorkType_720:
		m_char_S_Width = W720S;
		m_char_Num_Width = W720C;
		m_char_Height = H720;
		m_osdOffset = OSD720OFFSET;
		if (m_ScalePercent != 100)
		{
			m_char_S_Width = m_char_S_Width * m_ScalePercent / 100;
			m_char_Num_Width = m_char_Num_Width * m_ScalePercent / 100;
			m_char_Height = m_char_Height * m_ScalePercent / 100;
			m_char_Num_Width = 76;
			m_char_Height -= 2;
		}
		m_osdOffset = 720 / 2;
		break;
	case WorkType_1080:
	case WorkType_2160:
	case WorkType_MAX:
	default:
	{
		m_char_S_Width = W1080S;
		m_char_Num_Width = W1080C;
		m_char_Height = H1080;
		m_osdOffset = OSD1080OFFSET;

		if (m_ScalePercent != 100)
		{
			m_char_S_Width = m_char_S_Width * m_ScalePercent / 100;
			m_char_Num_Width = m_char_Num_Width * m_ScalePercent / 100;
			m_char_Height = m_char_Height * m_ScalePercent / 100;
			m_char_Num_Width = 76;
			m_char_Height -= 2;
		}
		m_osdOffset = 1080 / 2;
		break;
	}
	}

	if (m_bLoaded[workType])
		return 0;
	allocationMem(workType);
	m_bLoaded[workType] = true;
	UINT stride;
	BYTE * pBuffer;

	HMODULE hModule = GetModuleHandleW(NULL);
	TCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);
	PathCchRemoveFileSpec(path, MAX_PATH);

	TCHAR pngPath[MAX_PATH];
#ifdef DEBUG
#else
#endif // DEBUG
	BYTE **pTemp = m_char1080;
	if (workType == WorkType_SD)
	{
		_stprintf(pngPath, L"%s\\PNG\\%s", path, L"SD.PNG");
		if ((pBuffer = pngToYuvBuf(pngPath, stride)) != nullptr)
		{
			for (int i = 0; i < 10; i++)
				block2yuv(&pBuffer[(i * (W_SD_C + 2) + 1) * 3 + stride], m_charSD[i], W_SD_C, H_SD, stride);
			block2yuv(&pBuffer[(10 * (W_SD_C + 2) + 1) * 3 + stride], m_charSD[10], W_SD_S, H_SD, stride);
			block2yuv(&pBuffer[(10 * (W_SD_C + 2) + W_SD_S + 2 + 1) * 3 + stride], m_charSD[11], W_SD_S, H_SD, stride);
			free(pBuffer);
			m_bInitSuc[WorkType_SD] = true;
		}
		return 0;
	}
	else if (workType == WorkType_720)
	{
		_stprintf(pngPath, L"%s\\PNG\\%s", path, (m_ScalePercent != 100) ? L"720_150.PNG" : L"720.PNG");

		pTemp = m_char720;
	}
	else
	{
		_stprintf(pngPath, L"%s\\PNG\\%s", path, (m_ScalePercent != 100) ? L"1080_150.PNG" : L"1080.PNG");
	}
	if ((pBuffer = pngToYuvBuf(pngPath, stride)) != nullptr)
	{
		for (int i = 0; i < 10; i++)
			block2yuv(&pBuffer[(i * (m_char_Num_Width + 2) + 1) * 3 + stride * 1], pTemp[i], m_char_Num_Width, m_char_Height, stride);
		block2yuv(&pBuffer[(10 * (m_char_Num_Width + 2) + 1) * 3 + stride * 1], pTemp[10], m_char_S_Width, m_char_Height, stride);
		block2yuv(&pBuffer[(10 * (m_char_Num_Width + 2) + m_char_S_Width + 2 + 1) * 3 + stride * 1], pTemp[11], m_char_S_Width, m_char_Height, stride);
		free(pBuffer);
		if (workType == WorkType_720)
		{
			m_bInitSuc[WorkType_720] = true;
		}
		else
		{
			m_bInitSuc[WorkType_1080] = true;
			m_bInitSuc[WorkType_2160] = true;
		}
	}

	return 0;
}

void TcOSD::block2yuv(BYTE * _pIn, BYTE * _pOut, int _width, int _height, int _stride)
{
	for (int h = 0; h < _height; h++, _pIn += (_stride - _width * 3))
		for (int w = 0; w < _width / 2; w++, _pIn += 6, _pOut += 4)
			rgb2yuyv(_pIn[2], _pIn[1], _pIn[0], _pIn[5], _pIn[4], _pIn[3], _pOut[1], _pOut[0], _pOut[3], _pOut[2]);
}

void TcOSD::rgb2yuv(BYTE _R, BYTE  _G, BYTE  _B, BYTE & _Y, BYTE & _U, BYTE & _V)
{
	double Y = 0.2126 * (double)_R + 0.7152 * (double)_G + 0.02722 * (double)_B;
	double U = -0.09991 * (double)_R - 0.33609 * (double)_G + 0.436 * (double)_B + 128;
	double V = 0.615 * (double)_R - 0.55861 * (double)_G - 0.05639 * (double)_B + 128;

	_Y = (Y > 0xF0) ? 0xF0 : (Y < 0x10) ? 0x10 : (unsigned char)Y;
	_U = (U > 0xFF) ? 0xFF : (U < 0) ? 0 : (unsigned char)U;
	_V = (V > 0xFF) ? 0xFF : (V < 0) ? 0 : (unsigned char)V;
}

void TcOSD::rgb2yuyv(BYTE _R1, BYTE _G1, BYTE _B1, BYTE _R2, BYTE _G2, BYTE _B2, BYTE & _Y1, BYTE & _U, BYTE & _Y2, BYTE & _V)
{
	unsigned char U1, V1, U2, V2;

	rgb2yuv(_R1, _G1, _B1, _Y1, U1, V1);
	rgb2yuv(_R2, _G2, _B2, _Y2, U2, V2);

	_U = (BYTE)(((UINT32)U1 + (UINT32)U2) / 2);
	_V = (BYTE)(((UINT32)V1 + (UINT32)V2) / 2);
}

#define buildLine2160( d, index, width)	\
		memcpy(d, m_char2160[index] + (width * 2 * i), width * 2);		\
		pC += (width / 2);

#define buildLine1080( d, index, width)	\
		memcpy(d, m_char1080[index] + (width * 2 * i), width * 2);		\
		pC += (width / 2);pC-=2//pC += (10);

#define buildLine720( d, index, width)	\
		memcpy(d, m_char720[index] + (width * 2 * i), width * 2);		\
		pC += (width / 2);

#define buildLineSD( d, index, width)	\
		memcpy(d, m_charSD[index] + (width * 2 * i), width * 2);		\
		pC += (width / 2);

void TcOSD::incrustSD(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop, bool pal, bool b404)
{
	if (!m_bInitSuc[WorkType_SD])
		return;
	int offsetHeight = pal ? OSD_SD_OFFSET_PAL : OSD_SD_OFFSET_NTSC;
	if (b404)
	{
		offsetHeight = 404 - 80;
	}

	UINT32 * pF = (UINT32 *)&_frame[720 - 380 + (720 * 2 * (offsetHeight))];				// 528 pixels, the middle = (1280-528)/2 *2 since we have 2 bytes per pixels

	int h1 = (_h / 10) % 10, h2 = (_h % 10) & 0xF;
	int m1 = (_m / 10) % 10, m2 = (_m % 10) & 0xF;
	int s1 = (_s / 10) % 10, s2 = (_s % 10) & 0xF;
	int f1 = (_f / 10) % 10, f2 = (_f % 10) & 0xF;

	for (int i = 0; i < H_SD; i++, pF += 720 / 2)
	{
		UINT32 * pC = pF;
		*pC++ = 0x10801080;	*pC++ = 0x10801080;
		// hh:mm:ss;ff or hh:mm:ss:ff
		buildLineSD(pC, h1, W_SD_C);	buildLineSD(pC, h2, W_SD_C);		// hh
		buildLineSD(pC, 10, W_SD_S);										// :
		buildLineSD(pC, m1, W_SD_C);	buildLineSD(pC, m2, W_SD_C);		// mm
		buildLineSD(pC, 10, W_SD_S);										// :
		buildLineSD(pC, s1, W_SD_C);	buildLineSD(pC, s2, W_SD_C);		// ss
		buildLineSD(pC, _isDrop ? 11 : 10, W_SD_S);						// ; or :
		buildLineSD(pC, f1, W_SD_C);	buildLineSD(pC, f2, W_SD_C);		// ff

		*pC++ = 0x10801080;	*pC++ = 0x10801080;
	}
}

void TcOSD::incrust720(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop)
{
	if (!m_bInitSuc[WorkType_720])
		return;
	UINT32 * pF = (UINT32 *)&_frame[1280 - 528 + (1280 * 2 * m_osdOffset)];				// 528 pixels, the middle = (1280-528)/2 *2 since we have 2 bytes per pixels

	int h1 = (_h / 10) % 10, h2 = (_h % 10) & 0xF;
	int m1 = (_m / 10) % 10, m2 = (_m % 10) & 0xF;
	int s1 = (_s / 10) % 10, s2 = (_s % 10) & 0xF;
	int f1 = (_f / 10) % 10, f2 = (_f % 10) & 0xF;

	for (int i = 0; i < m_char_Height; i++, pF += 1280 / 2)
	{
		UINT32 * pC = pF;
		*pC++ = 0x10801080;	*pC++ = 0x10801080;
		// hh:mm:ss;ff or hh:mm:ss:ff
		buildLine720(pC, h1, m_char_Num_Width);	buildLine720(pC, h2, m_char_Num_Width);		// hh
		buildLine720(pC, 10, m_char_S_Width);										// :
		buildLine720(pC, m1, m_char_Num_Width);	buildLine720(pC, m2, m_char_Num_Width);		// mm
		buildLine720(pC, 10, m_char_S_Width);										// :
		buildLine720(pC, s1, m_char_Num_Width);	buildLine720(pC, s2, m_char_Num_Width);		// ss
		buildLine720(pC, _isDrop ? 11 : 10, m_char_S_Width);						// ; or :
		buildLine720(pC, f1, m_char_Num_Width);	buildLine720(pC, f2, m_char_Num_Width);		// ff

		*pC++ = 0x10801080;	*pC++ = 0x10801080;
	}
}

void TcOSD::incrust1080(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop)
{
	if (!m_bInitSuc[WorkType_1080])
		return;
	UINT32 * pF = (UINT32 *)&_frame[1920 - 528 + (1920 * 2 * m_osdOffset)];				// 528 pixels, the middle = (1920-528)/2 *2 since we have 2 bytes per pixels
	int h1 = (_h / 10) % 10, h2 = (_h % 10) & 0xF;
	int m1 = (_m / 10) % 10, m2 = (_m % 10) & 0xF;
	int s1 = (_s / 10) % 10, s2 = (_s % 10) & 0xF;
	int f1 = (_f / 10) % 10, f2 = (_f % 10) & 0xF;

	for (int i = 0; i < m_char_Height; i++, pF += 1920 / 2)
	{
		UINT32 * pC = pF;
		*pC++ = 0x10801080;	*pC++ = 0x10801080;

		// hh:mm:ss;ff or hh:mm:ss:ff
		buildLine1080(pC, h1, m_char_Num_Width);	buildLine1080(pC, h2, m_char_Num_Width);		// hh
		buildLine1080(pC, 10, m_char_S_Width);										// :
		buildLine1080(pC, m1, m_char_Num_Width);	buildLine1080(pC, m2, m_char_Num_Width);		// mm
		buildLine1080(pC, 10, m_char_S_Width);										// :
		buildLine1080(pC, s1, m_char_Num_Width);	buildLine1080(pC, s2, m_char_Num_Width);		// ss
		buildLine1080(pC, _isDrop ? 11 : 10, m_char_S_Width);						// ; or :
		buildLine1080(pC, f1, m_char_Num_Width);	buildLine1080(pC, f2, m_char_Num_Width);		// ff
		/**/
		*pC++ = 0x10801080;	*pC++ = 0x10801080;
	}
}

void TcOSD::incrust2160(BYTE * _frame, BYTE _h, BYTE _m, BYTE _s, BYTE _f, bool _isDrop)
{
	if (!m_bInitSuc[WorkType_2160])
		return;
	UINT32 * pF = (UINT32 *)&_frame[3840 - 528 + (3840 * 2 * 2 * OSD1080OFFSET)];
	int h1 = (_h / 10) % 10, h2 = (_h % 10) & 0xF;
	int m1 = (_m / 10) % 10, m2 = (_m % 10) & 0xF;
	int s1 = (_s / 10) % 10, s2 = (_s % 10) & 0xF;
	int f1 = (_f / 10) % 10, f2 = (_f % 10) & 0xF;

	for (int i = 0; i < H1080; i++, pF += 3840 / 2)
	{
		UINT32 * pC = pF;
		//	*pC++ = 0x10801080;	*pC++ = 0x10801080;
			// hh:mm:ss;ff or hh:mm:ss:ff
		buildLine1080(pC, h1, W1080C);	buildLine1080(pC, h2, W1080C);		// hh
		buildLine1080(pC, 10, W1080S);										// :
		buildLine1080(pC, m1, W1080C);	buildLine1080(pC, m2, W1080C);		// mm
		buildLine1080(pC, 10, W1080S);										// :
		buildLine1080(pC, s1, W1080C);	buildLine1080(pC, s2, W1080C);		// ss
		buildLine1080(pC, _isDrop ? 11 : 10, W1080S);						// ; or :
		buildLine1080(pC, f1, W1080C);	buildLine1080(pC, f2, W1080C);		// ff
		//*pC++ = 0x10801080;	*pC++ = 0x10801080;
	}
}
int mainXX()
{
	printf("\nmain\n");
	TcOSD tc;

	tc.loadFont(WorkType_1080, 150);

	const char *pSorce = "D:\\1080.yuv";
	FILE *pFileSource, *pFileDeta;
	fopen_s(&pFileSource, pSorce, "rb");
	fopen_s(&pFileDeta, "d:\\png\\timecode_150_1920x1080.yuv", "wb");
	BYTE* pBuffer = new BYTE[1920 * 1080 * 2];
	//	BrunTimeCode btc;
	//	btc.loadChar(1920, 1080);
	int x = 0;
	char ff[255];
	while (1920 * 1080 * 2 == fread(pBuffer, 1, 1920 * 1080 * 2, pFileSource))
	{
		sprintf_s(ff, "%d", x);
		//btc.BurnTimeCode(pBuffer, ff, 10);
		tc.incrust1080(pBuffer, 12 + x, 34 + x, x, 78 + x, false);
		fwrite(pBuffer, 1920 * 1080 * 2, 1, pFileDeta);
		x++;
	}
	fclose(pFileDeta);
	fclose(pFileSource);
	return 0;
}
#define MSWindows 1
#include "timecodeburn.h"
#include "vitc.h"

AJA_TcOSD::AJA_TcOSD()
{
}

bool AJA_TcOSD::RenderTimeCodeFont(FPTVideoFormat _fpFormat, int _pixFormat)
{
	if (m_ajaTimeCodeBurn == nullptr) {
		m_ajaTimeCodeBurn = new AJATimeCodeBurn();
	}
	if (m_vitc == nullptr)m_vitc = new CVITC;
	m_vitc->setFormat(_fpFormat);
	return m_ajaTimeCodeBurn->RenderTimeCodeFont(_fpFormat, _pixFormat);
}

bool AJA_TcOSD::BurnTimeCode(char * pBaseVideoAddress, const char * pTimeCodeString, const int percentY)
{
	if (m_ajaTimeCodeBurn == nullptr)m_ajaTimeCodeBurn = new AJATimeCodeBurn();
	return m_ajaTimeCodeBurn->BurnTimeCode(pBaseVideoAddress, pTimeCodeString, percentY);
}

bool AJA_TcOSD::BurnTimeCode(char* pBaseVideoAddress, const int percentY)
{
	if (m_ajaTimeCodeBurn == nullptr)m_ajaTimeCodeBurn = new AJATimeCodeBurn();
	m_vitc->inc();
	return m_ajaTimeCodeBurn->BurnTimeCode(pBaseVideoAddress, m_vitc->getString(), percentY);
}