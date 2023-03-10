/**
	@file		timecode.h
	@copyright	Copyright (C) 2010-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares the AJATimeCode class.
**/

#ifndef AJA_TIMECODE_H
#define AJA_TIMECODE_H
#include "ajastuff/common/timebase.h"

/** \class AJATimeCode timecode.h
 *  \brief Utility class for timecodes.
 *
 *   This is a storage and conversion class for timecodes.
 */
class AJA_EXPORT AJATimeCode
{
public:
	AJATimeCode();
	AJATimeCode(uint32_t frame);
	AJATimeCode(const wchar_t *pString, const AJATimeBase& timeBase, bool bDropFrame, bool bStdTc=false);
	AJATimeCode(const char *pString,    const AJATimeBase& timeBase, bool bDropFrame, bool bStdTc=false);
	AJATimeCode(const wchar_t *pString, const AJATimeBase& timeBase);
	AJATimeCode(const char *pString,	const AJATimeBase& timeBase);
	virtual	~AJATimeCode();
	
	/**
	 *	Query string showing timecode for current frame count given the passed parameters.
	 *
	 *	@param[out]	pString						buffer in which to place string.
	 *	@param[in]	timeBase  					frame rate from which to calculate string.
	 *	@param[in]	bDropFrame					drop frame value for string.
	 */
	void				QueryString(wchar_t *pString, const AJATimeBase& timeBase, bool bDropFrame);
	/**
	 *	Query string showing timecode for current frame count given the passed parameters.
	 *
	 *	@param[out]	pString						buffer in which to place string.
	 *	@param[in]	timeBase  					frame rate from which to calculate string.
	 *	@param[in]	bDropFrame					drop frame value for string.
	 */
	void				QueryString(char *pString, const AJATimeBase& timeBase, bool bDropFrame);
	
	/**
	 *	Query SMPTE string showing timecode for current frame count given the passed parameters.
	 *
	 *	@param[out]	pString						buffer in which to place string.
	 *	@param[in]	timeBase  					frame rate from which to calculate string.
	 *	@param[in]	bDropFrame					drop frame value for string.
	 */
	void				QuerySMPTEString(char *pString, const AJATimeBase& timeBase, bool bDropFrame);
	
	/**
	 *	Query SMPTE string byte count.
	 *
	 *	@return		number of bytes in SMPTE timecode string.
	 */
	static int          QuerySMPTEStringSize(void);
	
	/**
	 * Query frame number.
	 *
	 *	@return		frame number.
	 */
	uint32_t			QueryFrame(void) const;
	
	
	/**
	 *	Query HFR divide-by-two flag.
	 *
	 *	@return	bStdTc	 Return true when using standard TC notation for HFR (e.g 01:00:00:59 -> 01:00:00:29*), set to true by default
	 */
	bool                QueryStdTimecodeForHfr() { return m_stdTimecodeForHfr; }
	
	/**
	 *	Query hmsf values showing timecode for current frame count given the passed parameters.
	 *
	 *	@param[out]	h						    place in which to put hours value.
	 *	@param[out]	m						    place in which to put minutes value.
	 *	@param[out]	s						    place in which to put seconds value.
	 *	@param[out]	f						    place in which to put frames value.
	 *	@param[in]	timeBase  					frame rate from which to calculate string.
	 *	@param[in]	bDropFrame					drop frame value for string.
	 */
	void				QueryHmsf(uint32_t &h, uint32_t &m, uint32_t &s, uint32_t &f, const AJATimeBase& timeBase, bool bDropFrame) const;
	
	/**
	 *	Set current frame number.
	 *
	 *	@param[in]	frame    					new frame number.
	 */
	void				Set(uint32_t frame);
	
	/**
	 *	Set current frame number.
	 *
	 *	@param[in]	pString    					xx:xx:xx:xx style string representing new frame number.
 	 *	@param[in]	timeBase  					frame rate associated with pString.
	 */
	void				Set(const wchar_t *pString, const AJATimeBase& timeBase);
	
	/**
	 *	Set current frame number.
	 *
	 *	@param[in]	pString    					xx:xx:xx:xx style string representing new frame number.
 	 *	@param[in]	timeBase  					frame rate associated with pString.
	 */
	void				Set(const char *pString, const AJATimeBase& timeBase);
	
	/**
	 *	Set current frame number.  A variant which may have junk in the string.
	 *
	 *	@param[in]	pString    					xx:xx:xx:xx style string representing new frame number.
 	 *	@param[in]	timeBase  					frame rate associated with pString.
	 *  @param[in]  bDrop                       true if drop frame
	 */
	void				SetWithCleanup(const char *pString, const AJATimeBase& timeBase,bool bDrop);
	
	/**
	 *	Set current frame number.
	 *
	 *	@param[in]	pString    					xx:xx:xx:xx style string representing new frame number.
 	 *	@param[in]	timeBase  					frame rate associated with pString.
 	 *	@param[in]	bDropFrame  				true if forcing dropframe, false otherwise.
	 */
	void				Set(const wchar_t *pString, const AJATimeBase& timeBase, bool bDropFrame);
	
	/**
	 *	Set current frame number.
	 *
	 *	@param[in]	pString    					xx:xx:xx:xx style string representing new frame number.
 	 *	@param[in]	timeBase  					frame rate associated with pString.
 	 *	@param[in]	bDropFrame  				true if forcing dropframe, false otherwise.
	 */
	void				Set(const char *pString, const AJATimeBase& timeBase, bool bDropFrame);
	
	/**
	 *	Set current frame number.
	 *
	 *	@param[in]	h    					    hours value.
	 *	@param[in]	m    					    minutes value.
	 *	@param[in]	s    					    seconds value.
	 *	@param[in]	f    					    frames value.
 	 *	@param[in]	timeBase  					frame rate associated with hmsf.
 	 *	@param[in]	bDropFrame  				true if forcing dropframe, false otherwise.
	 */
	void				SetHmsf(uint32_t h, uint32_t m, uint32_t s, uint32_t f, const AJATimeBase& timeBase, bool bDropFrame);
	
	/**
	 *	Set timecode via a SMPTE string.
	 *
	 *	@param[in]	pBufr    					pointer to string.
	 *	@param[in]	timeBase                    time base associated with string.
	 */
	void                SetSMPTEString(const char *pBufr, const AJATimeBase& timeBase);
	
	/**
	 *	Set timecode via RP188 bytes.
	 *
	 *	@param[in]	dbb
	 *	@param[in]	low
 	 *	@param[in]	high
	 */
	void                SetRP188(uint32_t dbb, uint32_t low, uint32_t high, const AJATimeBase& timeBase);
	
	/**
	 *	Get RP188 bytes from a frame number, timebase, and drop frame.
	 *
	 *	@param[in]	pDbb		If non-NULL, points to the variable to receive the DBB component.
	 *	@param[in]	pLow		If non-NULL, points to the variable to receive the low byte component.
 	 *	@param[in]	pHigh		If non-NULL, points to the variable to receive the high byte component.
 	 *	@param[in]	timeBase	Specifies the AJATimeBase to use.
 	 *	@param[in]	bDrop		Specify true if forcing drop-frame;  otherwise false.
	 */
	void                QueryRP188(uint32_t *pDbb, uint32_t *pLow, uint32_t *pHigh, const AJATimeBase& timeBase, bool bDrop);
	
	/**
	 *	Set HFR divide-by-two flag.
	 *
	 *	@param[in]	bStdTc    Set true when using standard TC notation for HFR (e.g 01:00:00:59 -> 01:00:00:29*), set to true by default
	 */
	void                SetStdTimecodeForHfr(bool bStdTc) {m_stdTimecodeForHfr = bStdTc;}

	
	/**
	 *	Query string showing timecode for current frame count given the passed parameters.
	 *
	 *	@param[out]	bool					    'true' if drop frame style timecode, false otherwise.
	 *	@param[in]	pString                     character string with timecode
	 */
	static bool			QueryIsDropFrame(const wchar_t *pString);
	static bool			QueryIsDropFrame(const char    *pString);
	
	/**
	 *	Query string size needed for query string commands
	 *
	 *	@param[out]	int					    number of bytes need to store string
	 */
	static int 			QueryStringSize(void);
	
	/**
	 *	Query if rp188 data is drop frame or not
	 *
	 *	@param[out]	bool					    'true' if RP188 data is drop frame, false otherwise.
	 *	@param[in]	dbb
	 *	@param[in]	low
 	 *	@param[in]	high
	 */
	static bool			QueryIsRP188DropFrame(uint32_t dbb,uint32_t low,uint32_t high);
	
	AJATimeCode&		operator=(const AJATimeCode  &val);
	AJATimeCode&		operator+=(const AJATimeCode &val);
	AJATimeCode&		operator-=(const AJATimeCode &val);
	AJATimeCode&		operator+=(const int32_t val);
	AJATimeCode&		operator-=(const int32_t val);
	const AJATimeCode	operator+(const AJATimeCode &val) const;
	const AJATimeCode	operator+(const int32_t val) const;
	const AJATimeCode	operator-(const AJATimeCode &val) const;
	const AJATimeCode	operator-(const int32_t val) const;
	bool				operator==(const AJATimeCode &val) const;
	bool				operator<(const AJATimeCode &val) const;
	bool				operator<(const int32_t val) const;
	bool				operator>(const AJATimeCode &val) const;
	bool				operator>(const int32_t val) const;
	bool				operator!=(const AJATimeCode &val) const;
	
	uint32_t			m_frame;
	bool				m_stdTimecodeForHfr;
protected:
private:
};

#endif	// AJA_TIMECODE_H

