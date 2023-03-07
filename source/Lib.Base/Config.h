#pragma once
#ifndef XXXXX_CONFIG_H
#define XXXXX_CONFIG_H

#include <string>
#include "../FrameConsumer/Lib.FrameConsumer/IFrameConsumer.h"
#include <map>
typedef struct FileSelection
{
	bool bSelect;
	char szVName[MAX_PATH];
	char szAName[MAX_PATH];
	FileSelection()
	{
		memset(szVName, 0, MAX_PATH);
		memset(szAName, 0, MAX_PATH);
		szVName[0] = '\0';
		szAName[0] = '\0';
	}
};
class IConfig
{
public:
	int m_mapDisk[4] = { -1 };
	UINT64 m_mapFrames[4] = { 0 };
	FileSelection m_file_selection[EM_ConsumerChannel_MAX];
	static IConfig *		 getInstance();
	static void				 releaseInstance();
	void LoadSetting(LPCWSTR lpVideoFormat);
	int getPoolCount() { return m_poolCount; }
	FrameConsumerType getConsumerType() { return m_emType; };
	FPTVideoFormat getVideoFormat() { return m_videoFormat; }
	float getInterval();
	void setVideoFormat(FPTVideoFormat _format)	{		m_videoFormat = _format;			}

	const char* getStrVideoFormat();
	std::wstring getStrType() { return m_strType; }
	int getPixFormat() { return m_pixFormat; }
	
	bool is8k() {
		switch (m_videoFormat)
		{
		case    FP_FORMAT_8kp_2400:
		case FP_FORMAT_8kp_2500:
		case FP_FORMAT_8kp_2997:
		case FP_FORMAT_8kp_3000:
		case FP_FORMAT_8kp_5000:
		case FP_FORMAT_8kp_5994:
		case FP_FORMAT_8kp_6000:
			return true;
			break;
		default:
			return false;
		}
	}
	void setWriteFile(bool _b) { m_bWriteFile = _b; };
	bool needWriteFile() { return m_bWriteFile; }
	void UpdateSec(int nIdx,char*pvideoCh, char*pAudioCH)
	{
		strcpy_s(m_file_selection[nIdx].szVName, MAX_PATH, pvideoCh);	
		strcpy_s(m_file_selection[nIdx].szAName, MAX_PATH, pAudioCH);
	}
	bool useGestDisk() { return m_bUseGestDisk; }
	void set3gDS1(int ds2ch, int ds1ch);
	bool get3gDS1(int channel,  int& ds1ch);
	int getMaxConsumerChannelCount() { return m_maxConsumerChannelCount ; }
protected:
	IConfig();
	void load();
	virtual ~IConfig() = default;
private:
	int m_maxConsumerChannelCount = 8;
	std::wstring m_strType;
	FPTVideoFormat m_videoFormat;
	FrameConsumerType m_emType;
	int m_pixFormat;
	bool m_bWriteFile = false;
	bool m_bUseGestDisk = false;
	int m_poolCount = 200;
	using ds1Channel = int;
	using ds2Channel = int;
	std::map<ds2Channel, ds1Channel> m_3gDSMap;
};
#endif