#include "Config.h"
//*
 #include "locker.h"
#include "../lib.Import/AudioSampleHeader.h"

#include <windows.h>
#include <shlwapi.h>
#include <atltime.h>

inline void ReadConfigString(
	_In_opt_ LPCWSTR lpAppName,
	_In_opt_ LPCWSTR lpKeyName,
	_In_opt_ LPCWSTR lpDefault,
	_Out_writes_to_opt_(nSize, return +1) LPWSTR lpReturnedString,
	_In_     DWORD nSize,
	_In_opt_ LPCWSTR lpFileName)
{
	static WCHAR strConfig[MAX_PATH] = { '\0' };
	if (strConfig[0] == '\0')
	{
		WCHAR strModuleFileName[MAX_PATH] = {};
		GetModuleFileNameW(GetModuleHandleA("PlayYUV_GUI.exe"), strModuleFileName, MAX_PATH);
		PathRemoveFileSpecW(strModuleFileName);

		swprintf_s(strConfig, L"%s\\Config.ini", strModuleFileName);
	}
	::GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, strConfig);
}

#define GetPrivateProfileStringW  ReadConfigString

inline int ReadConfigInt(
	_In_     LPCWSTR lpAppName,
	_In_     LPCWSTR lpKeyName,
	_In_     INT nDefault,
	_In_opt_ LPCWSTR lpFileName)
{
	WCHAR lpReturnedString[MAX_PATH] = {};
	WCHAR lpDefault[MAX_PATH] = {};
	swprintf_s(lpDefault, L"%d", nDefault);


	GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpReturnedString, MAX_PATH, lpFileName);

	return _wtoi(lpReturnedString);
	//int nValue = GetPrivateProfileInt(_T("GENERAL_CONFIG"), _T("Card_Type"), 0, strConfig);
}

#define GetPrivateProfileInt  ReadConfigInt

static IConfig *mSInstance = nullptr;
static Locker	g_lock;

IConfig::IConfig()
{
	//load
	load();
}

void IConfig::load()
{
	TCHAR strModuleFileName[MAX_PATH] = {};
	GetModuleFileName(GetModuleHandle(_T("PlayYUV_GUI.exe")), strModuleFileName, MAX_PATH);
	PathRemoveFileSpec(strModuleFileName);
	CString strConfig;
	strConfig.Format(_T("%s\\Config.ini"), strModuleFileName);
	
	m_maxConsumerChannelCount=GetPrivateProfileInt(_T("GENERAL_CONFIG"), _T("maxChannel"), m_maxConsumerChannelCount, strConfig);

	if (m_maxConsumerChannelCount > EM_ConsumerChannel_MAX)
		m_maxConsumerChannelCount = EM_ConsumerChannel_MAX;
	int nValue = GetPrivateProfileInt(_T("GENERAL_CONFIG"), _T("Card_Type"), 0, strConfig);

	switch (nValue)
	{
	case 1:
		m_emType = FCT_BLACKMAGIC;
		m_strType = _T("BlackMagic");
		break;
	case 2:
		m_emType = FCT_DELTACAST;
		m_strType = _T("DeltaCast");
		break;
	default:
		m_strType = _T("AJA");
		m_emType = FCT_AJA;
		break;
	}
	m_videoFormat = FPTVideoFormat(GetPrivateProfileInt(_T("GENERAL_CONFIG"), _T("Video_Format"), FP_FORMAT_1080i_5000, strConfig));
	if (m_videoFormat >= FP_FORMAT_MAX || m_videoFormat <= FP_FORMAT_UNKNOWN)
		m_videoFormat = FP_FORMAT_1080i_5000;
	m_poolCount = GetPrivateProfileInt(_T("GENERAL_CONFIG"), _T("poolCount"), 200, strConfig);


	m_pixFormat = GetPrivateProfileInt(_T("GENERAL_CONFIG"), _T("pixFormat"), 1, strConfig);// 0:10bit 1:8bit
	
	m_bUseGestDisk = GetPrivateProfileInt(_T("GestDisk"), _T("useGestDisk"), 0, strConfig);
	if (m_bUseGestDisk)
	{
		m_mapDisk[0] = GetPrivateProfileInt(_T("GestDisk"), _T("disk0"), -1, strConfig);;
		m_mapDisk[1] = GetPrivateProfileInt(_T("GestDisk"), _T("disk1"), -1, strConfig);;
		m_mapDisk[2] = GetPrivateProfileInt(_T("GestDisk"), _T("disk2"), -1, strConfig);;
		m_mapDisk[3] = GetPrivateProfileInt(_T("GestDisk"), _T("disk3"), -1, strConfig);;


		m_mapFrames[0] = GetPrivateProfileInt(_T("GestDisk"), _T("totalframes0"), 0, strConfig);;
		m_mapFrames[1] = GetPrivateProfileInt(_T("GestDisk"), _T("totalframes1"), 0, strConfig);;
		m_mapFrames[2] = GetPrivateProfileInt(_T("GestDisk"), _T("totalframes2"), 0, strConfig);;
		m_mapFrames[3] = GetPrivateProfileInt(_T("GestDisk"), _T("totalframes3"), 0, strConfig);;
	}
}

IConfig * IConfig::getInstance()
{
	if (mSInstance != nullptr)
		return mSInstance;

	LockHolder lock(g_lock);
	if (mSInstance == nullptr)
		mSInstance = new IConfig();
	return mSInstance;
}

void IConfig::releaseInstance()
{
	LockHolder lock(g_lock);
	if (mSInstance)
	{
		delete mSInstance;
		mSInstance = nullptr;
	}
}
void IConfig::LoadSetting(LPCWSTR lpVideoFormat)
{
	char strModuleFileName[MAX_PATH] = {};
	GetModuleFileNameA(GetModuleHandleA("PlayYUV_GUI.exe"), strModuleFileName, MAX_PATH);
	PathRemoveFileSpecA(strModuleFileName);
	char strSetting[MAX_PATH];
	//m_strVideoFormat = IConfig::getInstance()->getStrVideoFormat();
	USES_CONVERSION;
	sprintf_s(strSetting, "%s\\%s-SETTING.dat", strModuleFileName, W2A(lpVideoFormat));

	FILE *fp = nullptr;
	fopen_s(&fp, strSetting, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		int nSize = sizeof(FileSelection)*EM_ConsumerChannel_MAX;
		int nFileSize = ftell(fp);
		if (nFileSize == nSize)
		{
			rewind(fp);
			int nSize = sizeof(FileSelection);
			int m_nChannels = EM_ConsumerChannel_MAX;
			for (int i = 0; i < m_nChannels; i++)
				fread((void*)(&m_file_selection[i]), nSize, 1, fp);
		}
		fclose(fp);
	}
}
float IConfig::getInterval() 
{
	int den = FFMPEG_AUDIOSAMPLE::getFrameRateDen(m_videoFormat);
	int num = FFMPEG_AUDIOSAMPLE::getFrameRateNum(m_videoFormat);
	if (den == 0 || num == 0)return 40.0;
	return ((float)den)*1000.0 / ((float)num);	
}
/**/
const char* IConfig::getStrVideoFormat()
{
	return FFMPEG_AUDIOSAMPLE::getVideoFormatString(m_videoFormat);
}

void IConfig::set3gDS1(int ds2ch, int ds1ch) {
	m_3gDSMap.emplace(ds2ch, ds1ch);
}

bool IConfig::get3gDS1(int channel, int& ds1ch) {
	if (m_3gDSMap.size() == 0)return false;

	auto pFind = m_3gDSMap.find(channel);
	//channel as ds2
	if (pFind != m_3gDSMap.end())
	{
		ds1ch = pFind->second;
		if (ds1ch == -1)return false;
		return true;
	}
	return false;
}