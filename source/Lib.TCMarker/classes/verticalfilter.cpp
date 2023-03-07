// VerticalFilter.cpp : implementation file
// 
// Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
//

#include "verticalfilter.h"
#include <memory.h>


inline RGBAlphaPixel VerticalFilterPixel(RGBAlphaPixel *pTop,
	                                     RGBAlphaPixel *pCurrent,
										 RGBAlphaPixel *pBottom);
inline RGBAlphaPixel FieldInterpolatePixel(RGBAlphaPixel * pTop, 
										   RGBAlphaPixel * pBottom);

void VerticalFilterLine(RGBAlphaPixel * topLine, 
  					    RGBAlphaPixel * midLine, 
						RGBAlphaPixel * bottomLine,
						RGBAlphaPixel * destLine,
						LWord numPixels )
{
	for ( LWord pixelCount = 0; pixelCount < numPixels; pixelCount++ )
	{
		*destLine = VerticalFilterPixel(topLine,midLine,bottomLine);
		topLine++;
		midLine++;
		bottomLine++;
		destLine++;
	}
}

inline RGBAlphaPixel VerticalFilterPixel(RGBAlphaPixel * pTop, 
										 RGBAlphaPixel * pCurrent, 
										 RGBAlphaPixel * pBottom)
{
	RGBAlphaPixel destPixel;

	destPixel.Red = (pTop->Red>>2) + (pCurrent->Red>>1) + (pBottom->Red>>2);
	destPixel.Green = (pTop->Green>>2) + (pCurrent->Green>>1) + (pBottom->Green>>2);
	destPixel.Blue = (pTop->Blue>>2) + (pCurrent->Blue>>1) + (pBottom->Blue>>2);
	destPixel.Alpha = (pTop->Alpha>>2) + (pCurrent->Alpha>>1) + (pBottom->Alpha>>2);

	return destPixel;
}


void FieldInterpolateLine(RGBAlphaPixel * topLine, 
						  RGBAlphaPixel * bottomLine,
						  RGBAlphaPixel * destLine,
						  LWord numPixels )
{
	for ( LWord pixelCount = 0; pixelCount < numPixels; pixelCount++ )
	{
		*destLine = FieldInterpolatePixel(topLine,bottomLine);
		topLine++;
		bottomLine++;
		destLine++;
	}
}

inline RGBAlphaPixel FieldInterpolatePixel(RGBAlphaPixel * pTop, 
										   RGBAlphaPixel * pBottom)
{
	RGBAlphaPixel destPixel;

	destPixel.Red   = (pTop->Red + pBottom->Red)>>1;
	destPixel.Green = (pTop->Green + pBottom->Green)>>1;
	destPixel.Blue  = (pTop->Blue + pBottom->Blue)>>1;
	destPixel.Alpha = (pTop->Alpha + pBottom->Alpha)>>1;

	return destPixel;
}
