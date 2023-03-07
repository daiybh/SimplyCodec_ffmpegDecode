#pragma once

struct tagFrameInfo //Used for import and RTMP IN
{
	unsigned char *pData = nullptr;
	int nDataSize;
	int nBufferSize = 1920 * 1080 * 2;
	tagFrameInfo(int _nBufferSize)
	{
		if(nBufferSize<_nBufferSize)
			nBufferSize = _nBufferSize;
		pData = new unsigned char[_nBufferSize];
		nDataSize = 0;
	}
	~tagFrameInfo()
	{
		if (pData)
			delete[] pData;
	}
};