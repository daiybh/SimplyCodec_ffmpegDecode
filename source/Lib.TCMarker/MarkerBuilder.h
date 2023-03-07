#pragma once
#include <stdio.h>
#include <windows.h>
#include <intsafe.h>
#include "MarkerBuilder.h"
bool decodePNG2(unsigned char*&pUYVY, int &nSize, unsigned int &w, unsigned int &h, char*filename);
class NewMark
{
	unsigned char *pMark = nullptr;
	int nMarkSize = 0;
	unsigned int m_nwidth = 0;
	unsigned int m_nheight = 0;
public:
	bool  loadPng(int _cnl,int _nW) {
		char filePath[255];
		char modulename[255];
		GetModuleFileNameA(nullptr, (LPSTR)modulename,255);
		char*pp = strrchr(modulename, '\\');
		pp++;
		*pp = '\0';
		sprintf_s(filePath, "%s\\PNG\\frameLabel%c.png", modulename, 'A' + _cnl);
		
		if (!decodePNG2(pMark, nMarkSize, m_nwidth, m_nheight, filePath))
			return false;
		if(_nW != m_nwidth)
		{
			unsigned char *pTemp = new unsigned char[_nW*m_nheight*2];
			for(int i=0;i<m_nheight;i++)
			{
				memcpy(pTemp + i*_nW * 2, pMark + i*m_nwidth * 2,min( _nW,m_nwidth)*2);
			}
			delete[]pMark;
			pMark = pTemp;
		}
		nMarkSize = _nW * m_nheight * 2;
		return true;
	}

	void putMark(unsigned char*pDestUYVY)
	{
		memcpy(pDestUYVY, pMark, nMarkSize);
	}
};
class Build_Marker
{
	int m_nWidth = 0;
	int m_nHeight = 0;
	int fieldInx[2] = { 0, 0 };
	int totalBLocks = 66;//1080/2/8-1=66
	int halfBlocks = 33;
	NewMark m_newMark ;
	bool m_bNewMark = false;
public:
	Build_Marker(int _cnl,int _nWidth, int _nHeight)
	{
		reset(_cnl, _nWidth, _nHeight);
	}
	void reset(int _cnl, int _nWidth, int _nHeight)
	{
		m_nWidth = _nWidth;
		m_nHeight = _nHeight;
		totalBLocks = _nHeight / 2 / 8 - 1;
		halfBlocks = totalBLocks / 2;
		m_bNewMark = (m_newMark.loadPng(_cnl,_nWidth));
	}
	void Mark2OnBlock(BYTE *pUYVY, int blockInx, int offsetX, bool bWhite)
	{
		int heightOffset = blockInx * 8 * 2; //8 pixels height and it's interlace

		short* pRootBlock = (short*)(pUYVY + heightOffset * m_nWidth * 2 + offsetX * 2);

		for (int j = 0; j < 8; j++)  //height
		{
			for (int i = 0; i < 8; i++)  //width
			{
				pRootBlock[i] = bWhite ? 0xF080 : 0x1080;
			}
			pRootBlock += m_nWidth * 2;//interlace
		}
	}

	void Mark2OnField(BYTE* pUYVY, int offsetX, bool fieldA)
	{
		int inx = fieldA ? fieldInx[0] : fieldInx[1];

		// 	int blockMarkArry[totalBLocks];
		// 	memset(blockMarkArry, 0, totalBLocks*sizeof(int));

		//fill the black line firstly

		int gap;
		if (inx < halfBlocks)
			gap = 0;
		else
			gap = inx - halfBlocks + 1;

		for (int i = 0; i < totalBLocks; i++)
		{
			bool bWriteWhite = false;
			if (i >= inx && i < (min(totalBLocks, inx + 33)))
				bWriteWhite = true;
			if (i < gap)
				bWriteWhite = true;

			Mark2OnBlock(pUYVY, i, offsetX, bWriteWhite);
		}
		if (fieldA)
			fieldInx[0] = (++fieldInx[0]) % totalBLocks;
		else
			fieldInx[1] = (++fieldInx[1]) % totalBLocks;
	}

	void writeMarker2(BYTE* pUYVY, UINT64 nFrameNum, int offsetX)
	{
		Mark2OnField(pUYVY, offsetX, true);
		Mark2OnField(pUYVY + m_nWidth * 2, offsetX, false);
	}
	void writeMarker(byte*pUYVY, UINT64 nFrameNum)
	{
		UINT64 nFieldA = nFrameNum * 2;
		UINT64 nFieldB = nFieldA + 1;
		{
			byte*pPos1 = pUYVY;
			byte*pPos2 = pUYVY + m_nWidth * 2;
			int nWidth = 512 / 8;
			for (int j = 0; j < 8; j++)
			{
				int *pPosA = (int *)pPos1;
				int *pPosB = (int *)pPos2;

				for (int i = 0; i < nWidth; i++)
				{
					bool bWriteWhiteA = false;
					bool bWriteWhiteB = false;

					bWriteWhiteA = (nFieldA >> (nWidth - 1 - i)) & 1;
					bWriteWhiteB = (nFieldB >> (nWidth - 1 - i)) & 1;
					for (int ii = 0; ii < 8; ii++)
					{
						*pPosA++ = (bWriteWhiteA) ? 0xF080f080 : 0x10801080;
						*pPosB++ = (bWriteWhiteB) ? 0xF080f080 : 0x10801080;
					}
				}
				pPos1 += m_nWidth * 4;
				pPos2 += m_nWidth * 4;
			}
		}
	}
	void buildMarker(BYTE* pUYVY, UINT64 nFrameNum, int playerPos)
	{
		if (m_bNewMark)
			m_newMark.putMark(pUYVY);
		else
			Build_Marker::writeMarker(pUYVY, nFrameNum);
		Build_Marker::writeMarker2(pUYVY, nFrameNum, 16 * (100 - playerPos * 5));
	}
};
