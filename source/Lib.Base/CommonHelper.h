#pragma once
#include <vector>
#include <windows.h>
using namespace std;

#define RGB24_480_SIZE (480*270*3)
#define RGB24_960_SIZE (960*540*3)

string			GetAppPath();
bool			isValidString(unsigned char * _str, int _len);
void			buildStrforGUID(char * _str, int size, unsigned char * _guid, int _srcSize);
string			getIDString(char * id, unsigned long size);
int				YUV444ToRGB888(unsigned char *src_buffer, int w, int h, unsigned char *des_buffer, bool bBGR /*true:BGR,false:RGB */);
int				RGB888ToYUV444(unsigned char *src_buffer, int w, int h, unsigned char *des_buffer, bool bBGR /*true:BGR,false:RGB */);
void			YUV2RGB(unsigned char* bufYUV, unsigned char* bufRGB, int nbElem);//UYVY2BGR24
pair<int, int>	ParseVersion(string versionString);
bool			checkStroageCompatibility(string _newV, string _oldV);
void			UYVY_To_YUV444(BYTE* _pDst, BYTE* _pSrc, int _width, int _height);
void			YUV422Planar_To_YUV444(BYTE* _pDst, BYTE* _pSrcY, BYTE* _pSrcU, BYTE* _pSrcV, int _width, int _height);
DWORD			round(DWORD _value, int _base = 4);
void		    YUV444_To_UYVY(BYTE* _pDst, BYTE* _pSrc, int _width, int _height);
void			YUV444_To_YUV422Planner(BYTE* _pDst, BYTE* _pSrc, int _width, int _height);
void			UYVY_To_YUV422Planner(BYTE* _pDst, BYTE* _pSrc, int _width, int _height);
bool			CopyToClipboard(const char* pszData, const int nDataLen);

wstring			getWString(const char * fmt, ...);
wstring			getWString(const wchar_t * fmt, ...);
wstring			getWString(const string& fmt);
string			getAString(const wchar_t * fmt, ...);
string			getAString(const char * fmt, ...);
string			getAString(const wstring& fmt);

struct  SATAInfo
{
	UINT32 index;
	string deviceID;
	UINT32 partitions;
	UINT32 scsiBus;
	string serialNumber;
	string name;
	UINT64 size;//in bytes
	UINT32 windowsDiskID;
	SATAInfo()
	{
		index = 0;
		deviceID = "";
		partitions = 0;
		scsiBus = 0;
		serialNumber = "";
		name = "";
		size = 0;
		windowsDiskID = 0;
	}
};
/*
return:
0: succeed , else failed
*/
int  GetSATAMap(vector<SATAInfo> &_sataTable);
