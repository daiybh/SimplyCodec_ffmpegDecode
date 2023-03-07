/**
	@file		ntv2ccplayer.cpp
	@brief		Implementation of NTV2CCPlayer class.
	@copyright	Copyright (C) 2012-2016 AJA Video Systems, Inc.  All rights reserved.
**/

#include "ntv2ccplayer.h"
#include "ntv2devicefeatures.h"
#include "ntv2debug.h"
#include "ntv2democommon.h"
#include "ntv2rp188.h"
#include "ntv2transcode.h"
#include "ajastuff/common/types.h"
#include "ajastuff/common/ajarefptr.h"
#include "ajastuff/system/systemtime.h"
#include "ajastuff/system/process.h"
#include "ajastuff/common/testpatterngen.h"
#include "ajastuff/common/videotypes.h"
#include "ajaanc/includes/ancillarylist.h"
#include "ajaanc/includes/ancillarydata_cea608_line21.h"
#include "ajaanc/includes/ancillarydata_cea608_vanc.h"
#include "ajaanc/includes/ancillarydata_cea708.h"
#include "ajacc/includes/ntv2captionrenderer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <deque>


using namespace std;


//#define	MEASURE_ACCURACY	1		//	Enables a feedback mechanism to precisely generate captions at the desired rate
static const bool		kTallVANC		(true);		//	Used only with VANC
static const bool		kTallerVANC		(false);	//	Used only with VANC
static const uint32_t	kAppSignature	(AJA_FOURCC ('C','C','P','L'));


/**
	@brief	Some default text to use if standard input isn't used.
**/
static const string gBuiltInCaptions ("IN CONGRESS, July 4, 1776.\n"
	"The unanimous Declaration of the thirteen united States of America.\n"
	"When in the Course of human events, it becomes necessary for one people to dissolve the political bands which have connected them with another, "
	"and to assume among the powers of the earth, the separate and equal station to which the Laws of Nature and of Nature's God entitle them, a decent "
	"respect to the opinions of mankind requires that they should declare the causes which impel them to the separation.\n"
	"We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, "
	"that among these are Life, Liberty and the pursuit of Happiness. That to secure these rights, Governments are instituted among Men, deriving their "
	"just powers from the consent of the governed, That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, "
	"and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and "
	"Happiness. Prudence, indeed, will dictate that Governments long established should not be changed for light and transient causes; and accordingly all experience hath shewn, "
	"that mankind are more disposed to suffer, while evils are sufferable, than to right themselves by abolishing the forms to which they are accustomed. "
	"But when a long train of abuses and usurpations, pursuing invariably the same Object evinces a design to reduce them under absolute Despotism, it is their right, "
	"it is their duty, to throw off such Government, and to provide new Guards for their future security. Such has been the patient sufferance of these Colonies; and such is now "
	"the necessity which constrains them to alter their former Systems of Government. The history of the present King of Great Britain is a history of repeated injuries and "
	"usurpations, all having in direct object the establishment of an absolute Tyranny over these States. To prove this, let Facts be submitted to a candid world.\n"
	"He has refused his Assent to Laws, the most wholesome and necessary for the public good.\n"
	"He has forbidden his Governors to pass Laws of immediate and pressing importance, unless suspended in their operation till his Assent should be obtained; "
	"and when so suspended, he has utterly neglected to attend to them.\n"
	"He has refused to pass other Laws for the accommodation of large districts of people, unless those people would relinquish the right of Representation in the Legislature, "
	"a right inestimable to them and formidable to tyrants only.\n"
	"He has called together legislative bodies at places unusual, uncomfortable, and distant from the depository of their public Records, for the sole purpose of fatiguing them "
	"into compliance with his measures. \n"
	"He has dissolved Representative Houses repeatedly, for opposing with manly firmness his invasions on the rights of the people.\n"
	"He has refused for a long time, after such dissolutions, to cause others to be elected; whereby the Legislative powers, incapable of Annihilation, have returned to the People "
	"at large for their exercise; the State remaining in the mean time exposed to all the dangers of invasion from without, and convulsions within.\n"
	"He has endeavoured to prevent the population of these States; for that purpose obstructing the Laws for Naturalization of Foreigners; refusing to pass others to encourage "
	"their migrations hither, and raising the conditions of new Appropriations of Lands.\n"
	"He has obstructed the Administration of Justice, by refusing his Assent to Laws for establishing Judiciary powers.\n"
	"He has made Judges dependent on his Will alone, for the tenure of their offices, and the amount and payment of their salaries.\n"
	"He has erected a multitude of New Offices, and sent hither swarms of Officers to harrass our people, and eat out their substance.\n"
	"He has kept among us, in times of peace, Standing Armies without the Consent of our legislatures.\n"
	"He has affected to render the Military independent of and superior to the Civil power.\n"
	"He has combined with others to subject us to a jurisdiction foreign to our constitution, and unacknowledged by our laws; "
	"giving his Assent to their Acts of pretended Legislation:\n"
	"For Quartering large bodies of armed troops among us:\n"
	"For protecting them, by a mock Trial, from punishment for any Murders which they should commit on the Inhabitants of these States:\n"
	"For cutting off our Trade with all parts of the world:\n"
	"For imposing Taxes on us without our Consent:\n"
	"For depriving us in many cases, of the benefits of Trial by Jury:\n"
	"For transporting us beyond Seas to be tried for pretended offences.\n"
	"For abolishing the free System of English Laws in a neighbouring Province, establishing therein an Arbitrary government, and enlarging its Boundaries so as to render it at "
	"once an example and fit instrument for introducing the same absolute rule into these Colonies:\n"
	"For taking away our Charters, abolishing our most valuable Laws, and altering fundamentally the Forms of our Governments:\n"
	"For suspending our own Legislatures, and declaring themselves invested with power to legislate for us in all cases whatsoever.\n"
	"He has abdicated Government here, by declaring us out of his Protection and waging War against us.\n"
	"He has plundered our seas, ravaged our Coasts, burnt our towns, and destroyed the lives of our people.\n"
	"He is at this time transporting large Armies of foreign Mercenaries to compleat the works of death, desolation and tyranny, already begun with circumstances of Cruelty & "
	"perfidy scarcely paralleled in the most barbarous ages, and totally unworthy the Head of a civilized nation.\n"
	"He has constrained our fellow Citizens taken Captive on the high Seas to bear Arms against their Country, to become the executioners of their friends and Brethren, "
	"or to fall themselves by their Hands.\n"
	"He has excited domestic insurrections amongst us, and has endeavoured to bring on the inhabitants of our frontiers, the merciless Indian Savages, whose known rule of warfare, "
	"is an undistinguished destruction of all ages, sexes and conditions.\n"
	"In every stage of these Oppressions We have Petitioned for Redress in the most humble terms: Our repeated Petitions have been answered only by repeated injury. "
	"A Prince whose character is thus marked by every act which may define a Tyrant, is unfit to be the ruler of a free people.\n"
	"Nor have We been wanting in attentions to our Brittish brethren. We have warned them from time to time of attempts by their legislature to extend an unwarrantable "
	"jurisdiction over us. We have reminded them of the circumstances of our emigration and settlement here. We have appealed to their native justice and magnanimity, "
	"and we have conjured them by the ties of our common kindred to disavow these usurpations, which, would inevitably interrupt our connections and correspondence. "
	"They too have been deaf to the voice of justice and of consanguinity. We must, therefore, acquiesce in the necessity, which denounces our Separation, and hold them, "
	"as we hold the rest of mankind, Enemies in War, in Peace Friends.\n"
	"We, therefore, the Representatives of the united States of America, in General Congress, Assembled, appealing to the Supreme Judge of the world for the rectitude of "
	"our intentions, do, in the Name, and by Authority of the good People of these Colonies, solemnly publish and declare, That these United Colonies are, and of Right ought "
	"to be Free and Independent States; that they are Absolved from all Allegiance to the British Crown, and that all political connection between them and the State of Great "
	"Britain, is and ought to be totally dissolved; and that as Free and Independent States, they have full Power to levy War, conclude Peace, contract Alliances, establish "
	"Commerce, and to do all other Acts and Things which Independent States may of right do. And for the support of this Declaration, with a firm reliance on the protection of "
	"divine Providence, we mutually pledge to each other our Lives, our Fortunes and our sacred Honor.");

/**
	@brief	The global built-in caption data input stream.
**/
static istringstream	gBuiltInStream (gBuiltInCaptions);


#if defined (MEASURE_ACCURACY)

	/**
		@brief	I'm a simple object that measures time between successive Sample calls as a moving average.
		@note	I am not thread-safe.
	**/
	class Speedometer
	{
		public:
			/**
				@brief	Constructs me to accommodate a given number of measurement samples.
				@param[in]	inNumSamples	Specifies the maximum number of measurements I will accommodate.
											Defaults to 20.
			**/
			Speedometer (const size_t inNumSamples = 20)
				:	mMaxNumSamples	(inNumSamples)
			{
				mTimespanSamples.push_back (10);	//	Initial seed of 10 millisec
				mWhenLastSampleCall = AJATime::GetSystemMilliseconds ();
			}

			/**
				@brief	Enqueues the elapsed time since this method was last called,
						discarding the oldest measurement (if my queue is full).
			**/
			virtual void	Sample (void)
			{
				if (mTimespanSamples.size () >= mMaxNumSamples)
					mTimespanSamples.pop_front ();
				const uint64_t	now	(AJATime::GetSystemMilliseconds ());
				mTimespanSamples.push_back (now - mWhenLastSampleCall);
				mWhenLastSampleCall = now;
			}

			/**
				@brief	Returns the average time between Sample calls, in milliseconds.
			**/
			virtual double	GetAvgMilliSecsBetweenSamples (void) const
			{
				uint64_t	sum	(0.0);
				for (SampleQueueConstIter iter (mTimespanSamples.begin ()); iter != mTimespanSamples.end (); ++iter)
					sum += *iter;
				return double (sum) / double (mTimespanSamples.size ());
			}

			/**
				@brief	Returns the average number of Sample calls per second.
			**/
			virtual double	GetAvgSamplesPerSecond (void) const
			{
				uint64_t	sum	(0.0);
				for (SampleQueueConstIter iter (mTimespanSamples.begin ()); iter != mTimespanSamples.end (); ++iter)
					sum += *iter;
				return double (mTimespanSamples.size ()) * 1000.0 / double (sum);
			}

			/**
				@brief	Returns the average number of Sample calls per minute.
			**/
			virtual inline double	GetAvgSamplesPerMinute (void) const		{return GetAvgSamplesPerSecond () * 60.0;}

			/**
				@brief	Returns the number of measurements currently in my queue.
			**/
			virtual inline size_t	GetSampleCount (void) const				{return mTimespanSamples.size ();}

		private:
			typedef std::deque <uint64_t>		SampleQueue;
			typedef	SampleQueue::const_iterator	SampleQueueConstIter;

			SampleQueue		mTimespanSamples;		///	My queue of timespan samples
			const size_t	mMaxNumSamples;			///	My maximum queue size
			uint64_t		mWhenLastSampleCall;	///	The last time Sample was called

	};	//	Speedometer


	/**
		@brief	Streams the given Speedometer instance to the given output stream in a human-readable format.
		@param		inOutStr	Specifies the output stream that is to receive the Speedometer in a human-readable format.
		@param[in]	inObj		Specifies the Speedometer instance to be streamed.
		@return	A reference to the given output stream.
	**/
	static ostream & operator << (ostream & inOutStr, const Speedometer & inObj)
	{
		const double	samplesPerSecond	(inObj.GetAvgSamplesPerSecond ());
		const double	samplesPerMinute	(samplesPerSecond * 60.0);
		return inOutStr << samplesPerMinute << " per min";
	}

	static uint64_t gWhenLastDisplay (0);		///	Used to periodically show timing information

#endif	//	MEASURE_ACCURACY


//////////////////////////////////////////////
//	Caption Source
//////////////////////////////////////////////

/**
	@brief	I read text from an input stream and I "package" it into "sentences" that are up
			to 32 characters long, suitable for captioning. Clients specify the rate at which
			I generate captions, and the input stream that I read from, then repeatedly call
			my GetNextCaptionRow function to obtain each successive "sentence". Upon reaching
			end-of-file, I enter a "finished" state and thereafter return empty strings.
	@note	The current design doesn't handle Unicode characters.
**/
class CaptionSource;
typedef AJARefPtr <CaptionSource>	CaptionSourcePtr;


/**
	@brief	I am an ordered sequence of CaptionSources.
**/
typedef deque <CaptionSourcePtr>			CaptionSourceList;
typedef CaptionSourceList::const_iterator	CaptionSourceListConstIter;


class CaptionSource
{
	//	Instance Methods
	public:
		/**
			@brief	My only constructor.
			@param[in]	inCharsPerMinute		Specifies the maximum rate at which I will emit characters.
			@param[in]	pInInputStream			Specifies a valid, non-NULL pointer to the input stream to use.
			@param[in]	inDeleteInputStream		If true, I will delete the input stream object when I'm destroyed;
												otherwise it's the caller's responsibility.
		**/
		explicit CaptionSource (const double inCharsPerMinute, istream * pInInputStream, const bool inDeleteInputStream = false)
			:	mpInputStream		(pInInputStream),
				mCharsPerMinute		(inCharsPerMinute),
				mMaxRowCharWidth	(32),
				mFinished			(false),
				mDeleteInputStream	(inDeleteInputStream),
				mIsTextMode			(false),
				mMilliSecsPerChar	(0.0)
		{
			NTV2_ASSERT (mpInputStream);							//	Must be non-NULL pointer
			NTV2_ASSERT (mCharsPerMinute > 0.0);					//	Must be greater than zero
			*mpInputStream >> std::noskipws;				//	Include whitespace when reading from this stream
			mMilliSecsPerChar = 60000.0 / mCharsPerMinute;	//	Milliseconds of delay (sleep time) between emitting successive characters
		}


		/**
			@brief	My destructor. If was told at construction time that I was to be responsible
					for deleting the input stream, then I'll delete it here.
		**/
		virtual ~CaptionSource ()
		{
			if (mDeleteInputStream && mpInputStream)
			{
				delete mpInputStream;
				mpInputStream = NULL;
			}
			#if defined (MEASURE_ACCURACY)
				cerr << "## DEBUG:  Final chars: " << mCharSpeed << endl;
			#endif	//	MEASURE_ACCURACY
		}


		/**
			@brief		Returns the next character read from my input stream. If my text input stream has reached
						EOF, I set my "finished" flag and return zero.
			@return		A string containing a single UTF8-encoded unicode character.
		**/
		virtual string GetNextCaptionCharacter (void)
		{
			static double	milliSecsPerChar	(mMilliSecsPerChar);
			string			resultChar;
			unsigned char	rawBytes [5]	= {0, 0, 0, 0, 0};

			AJATime::Sleep (static_cast <int32_t> (milliSecsPerChar));

			*mpInputStream >> rawBytes [0];
			if (mpInputStream->eof ())
				SetFinished ();

			if (rawBytes [0] < 0x80)		//	1-byte code
			{
				if (rawBytes [0])
					resultChar = string (1, rawBytes [0]);
			}
			else if (rawBytes [0] < 0xC0)	//	invalid
				cerr << "## WARNING:  GetNextCaptionCharacter:  Invalid UTF8 byte value read from input stream:  0x" << CNTV2CaptionLogConfig::gHexUC [rawBytes [0]] << endl;
			else if (rawBytes [0] < 0xE0)	//	2-byte code
			{
				if (IsFinished ())
					cerr << "## WARNING:  GetNextCaptionCharacter:  EOF on input stream before reading byte 2 of 2-byte UTF8 character" << endl;
				else
				{
					resultChar = string (1, rawBytes [0]);
					*mpInputStream >> rawBytes [1];
					if (mpInputStream->eof ())
						SetFinished ();
					resultChar += string (1, rawBytes [1]);
					NTV2_ASSERT (resultChar.length () == 2);
				}
			}
			else if (rawBytes [0] < 0xF0)	//	3-byte code
			{
				resultChar = string (1, rawBytes [0]);
				for (unsigned ndx (1);  ndx <= 2;  ndx++)
				{
					if (IsFinished ())
					{
						cerr << "## WARNING:  GetNextCaptionCharacter:  EOF on input stream before reading byte " << (ndx + 1) << " of 3-byte UTF8 character" << endl;
						resultChar = "";
						break;
					}
					*mpInputStream >> rawBytes [ndx];
					if (mpInputStream->eof ())
						SetFinished ();
					resultChar += string (1, rawBytes [ndx]);
				}
				if (!IsFinished ())	NTV2_ASSERT (resultChar.length () == 3);
			}
			else if (rawBytes [0] < 0xF8)	//	4-byte code
			{
				resultChar = string (1, rawBytes [0]);
				for (unsigned ndx (1);  ndx <= 3;  ndx++)
				{
					if (IsFinished ())
					{
						cerr << "## WARNING:  GetNextCaptionCharacter:  EOF on input stream before reading byte " << (ndx + 1) << " of 4-byte UTF8 character" << endl;
						resultChar = "";
						break;
					}
					*mpInputStream >> rawBytes [ndx];
					if (mpInputStream->eof ())
						SetFinished ();
					resultChar += string (1, rawBytes [ndx]);
				}
				if (!IsFinished ())	NTV2_ASSERT (resultChar.length () == 4);
			}
			else
				cerr << "## WARNING:  GetNextCaptionCharacter:  UTF8 byte value not handled:  0x" << CNTV2CaptionLogConfig::gHexUC [rawBytes [0]] << endl;

			#if defined (MEASURE_ACCURACY)
				if (!resultChar.empty ())
					mCharSpeed.Sample ();

				if (AJATime::GetSystemMilliseconds () - gWhenLastDisplay > 1000)
				{
					//	Once per second, see how far off I am from the target character rate, and adjust the sleep time as needed...
					const double	avg		(mCharSpeed.GetAvgMilliSecsBetweenSamples ());
					const double	diff	(avg - mMilliSecsPerChar);
					milliSecsPerChar -= diff;
					if (milliSecsPerChar < 0.0)
						milliSecsPerChar = mMilliSecsPerChar;
					cerr << "## DEBUG:  chars: " << mCharSpeed << "     " << milliSecsPerChar << "    " << avg << " - " << mMilliSecsPerChar << " = " << diff << "diff" << endl;
					gWhenLastDisplay = AJATime::GetSystemMilliseconds ();
				}
			#endif	//	MEASURE_ACCURACY

			return resultChar;

		}	//	GetNextCaptionCharacter


		/**
			@brief	Returns the next "word" read from my input stream. If my text input stream has reached
					EOF, I set my "finished" flag and return an empty string. If the returned word is longer
					than my maximum allowable word length, the string is truncated to that maximum length.
			@param[out]	outLineBreak	Returns true if a newline or line break was encountered in the resulting
										caption word;  otherwise returns false.
		**/
		virtual string GetNextCaptionWord (bool & outLineBreak)
		{
			std::locale	loc;
			string		resultWord;
			string		nextChar	(GetNextCaptionCharacter ());

			outLineBreak = false;
			while (!nextChar.empty ())
			{
				if (IsSpaceChar (nextChar) || IsControlChar (nextChar))
				{
					outLineBreak = (nextChar.at (0) == '\n' || nextChar.at (0) == '\r');
					break;	//	Control chars & whitespace break words (HT, LF, CR, etc.) -- but don't include them in the word
				}

				resultWord += nextChar;
				if (IsWordBreakCharacter (nextChar))
					break;	//	Some punctuation breaks word -- include in the word, too

				nextChar = GetNextCaptionCharacter ();
			}	//	loop til break

			return resultWord;

		}	//	GetNextCaptionWord


		/**
			@brief	Returns the next "sentence" read from my input stream. I build the "sentence" by reading "words"
					using my GetNextCaptionWord function. I always try to return a sentence as close to my designated
					maximum sentence length as possible. In Text Mode, there's no limit to the length of the row text.
		**/
		virtual string GetNextCaptionRow (const bool inBreakLinesOnNewLineChars = false)
		{
			string	resultRow;

			//	Build resultRow one word at a time (except Text Mode)...
			while (true)
			{
				if (mIsTextMode)
				{
					//	Special case for Text Mode...
					string	nextChar	(GetNextCaptionCharacter ());
					while (!nextChar.empty ())
					{
						if (nextChar.at (0) == '\n' || nextChar.at (0) == '\r')
							return resultRow;	//	LF and CR will break the row -- but don't include LF/CR in the row text

						resultRow += nextChar;
						nextChar = GetNextCaptionCharacter ();
					}	//	loop til break
					return resultRow;
				}	//	if Text Mode


				//	Any text left over from the last time?
				if (!mLeftoverRowText.empty ())
				{
					//	Use whatever text was left over from the last time...
					if (mLeftoverRowText.length () > mMaxRowCharWidth)
					{
						//	Not all of it will fit. Use whatever will fit and save the rest for later...
						resultRow = mLeftoverRowText.substr (0, mMaxRowCharWidth);
						mLeftoverRowText.erase (0, mMaxRowCharWidth);
						break;	//	Done
					}
					else
					{
						resultRow = mLeftoverRowText;
						mLeftoverRowText.clear ();
					}
				}	//	if any leftover text

				if (resultRow.length () == mMaxRowCharWidth)	//	Completely full?
					break;										//	If so, send it!

				//	ResultRow has room for more.
				//	Get the next word from the caption source...
				bool	lineBreak	(false);
				string	nextWord	(GetNextCaptionWord (lineBreak));
				if (nextWord.empty () && IsFinished ())
					break;

				if (nextWord.empty () && !lineBreak)
					continue;						//	Nothing to add at this time

				//	If there's anything to send, and the word is maxRowWidth or longer, send resultRow now, and save the rest...
				if (!resultRow.empty () && nextWord.length () >= mMaxRowCharWidth)
				{
					mLeftoverRowText = nextWord;	//	Save the word for next time
					break;							//	Send resultRow now
				}

				//	If the word is longer than maxRowWidth...
				if (nextWord.length () >= mMaxRowCharWidth)
				{
					resultRow = nextWord.substr (0, mMaxRowCharWidth);	//	Send whatever will fit
					nextWord.erase (0, mMaxRowCharWidth);
					mLeftoverRowText = nextWord;						//	Save the rest for next time
					break;												//	Send resultRow now
				}

				//	Add the word only if it will fit...
				const size_t	newLength	((resultRow.length () ? resultRow.length () + 1 : 0) + nextWord.length ());
				if (newLength > mMaxRowCharWidth)
				{
					//	Wrap this word onto next line...
					mLeftoverRowText = nextWord;	//	Save the word for next time
					break;							//	Send resultRow now
				}

				if (resultRow.empty ())
					resultRow = nextWord;
				else
					resultRow += " " + nextWord;
				if (inBreakLinesOnNewLineChars && lineBreak)
					break;
			}	//	loop til I have a row to return

			return resultRow;

		}	//	GetNextCaptionRow


		virtual bool IsFinished (void) const	{return mFinished;}		///< @brief	Returns true if I'm finished -- i.e., if I've delivered my last word.

		virtual void SetFinished (void)			{mFinished = true;}		///< @brief	Sets my "finished" flag.


		/**
			@brief	Sets my text mode. If I'm in text mode, I don't do any word breaking or line/row truncation.
			@param[in]	inIsTextMode	Specify true if I'm supplying caption data to a text channel (Tx1, Tx2, Tx3 or Tx4).
										Specify false if I'm supplying caption data to a normal caption channel (CC1, CC2, CC3, or CC4).
		**/
		virtual void SetTextMode (const bool inIsTextMode)	{mIsTextMode = inIsTextMode;}


	//	Private Instance Methods
	private:
		inline			CaptionSource (const CaptionSource & inCaptionSource)				: mDeleteInputStream (false)	{(void) inCaptionSource;}	//	Hidden
		virtual inline	CaptionSource & operator = (const CaptionSource & inCaptionSource)	{(void) inCaptionSource; return *this;}						//	Hidden


	//	Instance Data
	private:
		istream *		mpInputStream;		///< @brief	My current input file stream.
		double			mCharsPerMinute;	///< @brief	My character emission rate, in characters per minute.
		string			mLeftoverRowText;	///< @brief	My row text accumulator.
		size_t			mMaxRowCharWidth;	///< @brief	My maximum row width, in characters.
		bool			mFinished;			///< @brief	True if I've delivered everything I can.
		const bool		mDeleteInputStream;	///< @brief	True if I'm responsible for deleting the input stream.
		bool			mIsTextMode;		///< @brief	True if I'm generating Text Mode caption data instead of normal Captions.
		double			mMilliSecsPerChar;	///< @brief	Delay between emitting successive characters, in milliseconds.
		#if defined (MEASURE_ACCURACY)
			Speedometer		mCharSpeed;		///< @brief	This is used to measure/adjust the rate at which I generate captions.
		#endif	//	MEASURE_ACCURACY


	//	Private Class Methods
	private:
		static bool		IsWordBreakCharacter (const string & inUTF8Char)
		{
			static const string	wordBreakCharacters	("!#$%&()*+,./:;<=>?@[\\]^_`{|}~. \t");
			if (inUTF8Char.length () != 1)
				return false;
			return wordBreakCharacters.find (inUTF8Char) != string::npos;
		}

		static bool		IsSpaceChar (const string & inUTF8Char)
		{
			//	tab, newline, vertical tab, form feed, carriage return, and space
			if (inUTF8Char.length () != 1)
				return false;
			const unsigned char	c	(inUTF8Char [0]);
			if (c == 0x09 || c == 0x0A || c == 0x0B || c == 0x0C || c == 0x0D || c == 0x20)
				return true;
			return false;
		}

		static bool		IsControlChar (const string & inUTF8Char)
		{
			//	ASCII characters octal codes 000 through 037, and 177 (DEL)
			if (inUTF8Char.length () != 1)
				return false;
			const unsigned char	c	(inUTF8Char [0]);
			if ((c > 0x00 && c < ' ') || c == 0x7F)
				return true;
			return false;
		}

};	//	CaptionSource


/**
	@brief		Creates and returns a list of CaptionSource instances to play from a given list
				of paths to text files (which might be empty).
	@param[in]	inFilesToPlay		Specifies a list of paths to text files to be played.
									If empty, use the built-in caption stream.
	@param[in]	inCharsPerMinute	Specifies the desired rate at which caption characters get produced.
	@return		A list of newly-created CaptionSource instances.
**/
static CaptionSourceList GetCaptionSources (const StringList & inFilesToPlay, const double inCharsPerMinute)
{
	CaptionSourceList	result;
	static istream *	pStdIn			(&cin);
	static size_t		nFilesToPlay	(9999);

	if (nFilesToPlay == 9999)
		nFilesToPlay = inFilesToPlay.size ();

	if (nFilesToPlay == 0)
	{
		if (gBuiltInStream.eof ())
		{
			//	The built-in caption stream has already been used.
			//	This can happen when running in "loop" mode, so reset it, so it can be used again...
			gBuiltInStream.clear ();	//	Clear EOF
			gBuiltInStream.seekg (0);	//	Rewind
		}
		result.push_back (CaptionSourcePtr (new CaptionSource (inCharsPerMinute, &gBuiltInStream)));
	}
	else
	{
		for (StringListConstIter pFilePath (inFilesToPlay.begin ());  pFilePath != inFilesToPlay.end ();  ++pFilePath)
		{
			if (*pFilePath == "-")
			{
				if (pStdIn)
				{
					result.push_back (CaptionSourcePtr (new CaptionSource (inCharsPerMinute, pStdIn)));	//	Add stdin once
					pStdIn = NULL;	//	Standard input can only be read from once
				}
				else
					cerr << "## WARNING:  Standard input ('-') can only be specified once" << endl;
			}	//	'-' means "read from standard input"
			else
			{
				ifstream * pFileStream (new ifstream (pFilePath->c_str ()));
				NTV2_ASSERT (pFileStream);
				if (pFileStream->is_open ())
					result.push_back (CaptionSourcePtr (new CaptionSource (inCharsPerMinute, pFileStream, true)));
				else
					cerr << "## WARNING:  Cannot play '" << pFilePath->c_str () << "'" << endl;
			}	//	else not '-'

		}	//	for each file path
	}	//	else file list is not empty

	return result;

}	//	GetCaptionSources



static NTV2CCPlayer *	gApp	(NULL);


NTV2CCPlayer::NTV2CCPlayer (const CCPlayerConfig & inConfigData)
	:	mConfig					(inConfigData),
		mPlayThread				(NULL),
		mLock					(new AJALock (CNTV2DemoCommon::GetGlobalMutexName ())),
		mCurrentFrame			(0),
		mDroppedFrameTally		(0),
		mDeviceID				(DEVICE_ID_NOTFOUND),
		mDeviceSpecifier		(inConfigData.fDeviceSpecifier),
		mOutputChannel			(inConfigData.fOutputChannel),
		mEmitStats				(inConfigData.fEmitStats),
		mVideoFormat			(inConfigData.fVideoFormat),
		mPixelFormat			(inConfigData.fPixelFormat),
		mFrameRate				(NTV2_FRAMERATE_UNKNOWN),
		mSavedTaskMode			(NTV2_DISABLE_TASKS),
		mTallVANC				(kTallVANC),
		mTallerVANC				(kTallerVANC),
		mPlayerQuit				(false),
		mCaptionGeneratorQuit	(false),
		mpVideoBuffer(NULL)
{
	::memset (mGeneratorThreads, 0, sizeof (mGeneratorThreads));

	NTV2_ASSERT (!inConfigData.fChannelGenerators.empty ());
	NTV2_ASSERT (!(!mTallVANC && mTallerVANC));		//	taller VANC true and tall VANC false doesn't make sense
	gApp = this;

}	//	constructor


NTV2CCPlayer::~NTV2CCPlayer (void)
{
	//	Stop my playout and producer threads, then destroy them...
	Quit ();

	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
	{
		mDevice.UnsubscribeOutputVerticalEvent (NTV2Channel (mOutputChannel + 3));
		mDevice.UnsubscribeOutputVerticalEvent (NTV2Channel (mOutputChannel + 2));
		mDevice.UnsubscribeOutputVerticalEvent (NTV2Channel (mOutputChannel + 1));
	}
	mDevice.UnsubscribeOutputVerticalEvent (mOutputChannel);

	if (mpVideoBuffer)
	{
		delete [] mpVideoBuffer;
		mpVideoBuffer = NULL;
	}

	if (!mConfig.fDoMultiFormat)
	{
		mDevice.SetEveryFrameServices (mSavedTaskMode);															//	Restore prior service level
		mDevice.ReleaseStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ()));	//	Release the device
	}

}	//	destructor


void NTV2CCPlayer::Quit (const bool inQuitImmediately)
{
	//	Kill the caption generators first...
	mCaptionGeneratorQuit = true;
	for (unsigned ccChannel (NTV2_CC608_CC1);  ccChannel < NTV2_CC608_XDS;  ccChannel++)
	{
		AJAThread *	pThread	(mGeneratorThreads [ccChannel]);
		if (pThread)
		{
			while (pThread->Active ())
				AJATime::Sleep (10);
			delete pThread;
			mGeneratorThreads [ccChannel] = NULL;
		}
	}

	if (m608Encoder)
	{
		if (inQuitImmediately)
		{
			//cerr << "## DEBUG:  Quit immediate -- flushing" << endl;
			m608Encoder->Flush ();					//	Immediately flush all queued messages
		}
		else
		{
			//cerr << "## DEBUG:  Quit non-immediate -- waiting for queue to drain" << endl;
			while (m608Encoder->GetQueuedByteCount (NTV2_CC608_Field1) || m608Encoder->GetQueuedByteCount (NTV2_CC608_Field2))
				AJATime::Sleep (10);				//	Wait for PlayThread encoder's message queues to drain as PlayThread continues
			//	This Sleep isn't necessary, but if not done, the last captions (or Erase) won't get a chance to be seen on downstream
			//	devices (because the AJA device will get released, and the retail services will change the device configuration).
			AJATime::Sleep (2000);
		}
	}

	//cerr << "## DEBUG:  Signal playout thread to terminate" << endl;
	mPlayerQuit = true;
	if (mPlayThread)
	{
		while (mPlayThread->Active ())
			AJATime::Sleep (10);
		delete mPlayThread;
		mPlayThread = NULL;
	}

}	//	Quit


bool NTV2CCPlayer::DeviceAncExtractorIsAvailable (void)
{
	UWord	majorVersion (0),	minorVersion (0),	pointVersion (0),	buildNumber (0);
	mDevice.GetDriverVersionComponents (majorVersion, minorVersion, pointVersion, buildNumber);
	//	Device Anc extraction requires driver version 12.3 minimum  (or 0.0.0.0 for internal development)...
	if ((majorVersion >= 12 && minorVersion >= 3) || (majorVersion == 0 && minorVersion == 0 && pointVersion == 0 && buildNumber == 0))
		//	The device must also support it...
		if (::NTV2DeviceCanDoCustomAnc (mDeviceID))
			//	And perhaps even do firmware version/date checks??
			return true;
	return false;
}


AJAStatus NTV2CCPlayer::Init (void)
{
	AJAStatus	status	(AJA_STATUS_SUCCESS);

	//	Any AJA devices out there?
	if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, mDevice))
		{cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return AJA_STATUS_OPEN;}

	if (!mDevice.IsDeviceReady ())
		{cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not ready" << endl;  return AJA_STATUS_INITIALIZE;}

	if (!mConfig.fDoMultiFormat)
	{
		if (!mDevice.AcquireStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ())))
		{
			cerr << "## ERROR:  Unable to acquire device because another app owns it" << endl;
			return AJA_STATUS_BUSY;		//	Some other app owns the device
		}
		mDevice.GetEveryFrameServices (mSavedTaskMode);	//	Save the current task mode
	}

	mDevice.SetEveryFrameServices (NTV2_OEM_TASKS);		//	Use the OEM service level


	//	Store the device ID in a member because it will be used frequently...
	mDeviceID = mDevice.GetDeviceID ();

	if (::NTV2DeviceCanDoMultiFormat (mDeviceID) && mConfig.fDoMultiFormat)
		mDevice.SetMultiFormatMode (true);
	else if (::NTV2DeviceCanDoMultiFormat (mDeviceID))
		mDevice.SetMultiFormatMode (false);

	if (!mConfig.fForceVanc)						//	if user didn't use --vanc option...
		if (!DeviceAncExtractorIsAvailable ())		//	and anc extractor isn't available...
			mConfig.fForceVanc = true;				//	then force Vanc anyway
	if (!mConfig.fForceVanc)
		mTallVANC = mTallerVANC = false;

	//	Set up the device video config...
	status = SetUpOutputVideo ();
	if (AJA_FAILURE (status))
		return status;

	//	Set up my background pattern buffer...
	status = SetUpBackgroundPatternBuffer ();
	if (AJA_FAILURE (status))
		return status;

	//	Set up device signal routing...
	return RouteOutputSignal ();

}	//	Init


AJAStatus NTV2CCPlayer::SetUpBackgroundPatternBuffer (void)
{
	//	Allocate and clear the host video buffer memory...
	mpVideoBuffer = new uint8_t [mVideoBufferSize];
	::memset (mpVideoBuffer, 0, mVideoBufferSize);

	//	Generate the test pattern...
	AJATestPatternBuffer		testPatternBuffer;
	AJATestPatternGen			testPatternGen;
	const NTV2FormatDescriptor	formatDesc	(::GetFormatDescriptor (mVideoFormat, mPixelFormat, mTallVANC, mTallerVANC));

	if (!testPatternGen.DrawTestPattern (AJA_TestPatt_FlatField,  formatDesc.GetRasterWidth (),  formatDesc.GetVisibleRasterHeight (),
										CNTV2DemoCommon::GetAJAPixelFormat (mPixelFormat), testPatternBuffer))
	{
		cerr << "## ERROR:  DrawTestPattern failed, formatDesc: " << formatDesc << endl;
		return AJA_STATUS_FAIL;
	}

	//	Stuff the gray pattern into my video buffer...
	::memcpy (formatDesc.GetTopVisibleRowAddress (mpVideoBuffer), &testPatternBuffer[0], testPatternBuffer.size ());

	//	Set the VANC area, if any, to legal black...
	if (formatDesc.IsVANC () && !::SetRasterLinesBlack (mPixelFormat, mpVideoBuffer, formatDesc.GetBytesPerRow (), formatDesc.firstActiveLine))
	{
		cerr << "## ERROR:  Cannot set video buffer's VANC area to legal black" << endl;
		return AJA_STATUS_FAIL;
	}

	//	Add info to the display...
	const string	strVideoFormat	(CNTV2DemoCommon::StripFormatString (::NTV2VideoFormatToString (mVideoFormat)));
	{ostringstream	oss;	oss << setw (32) << left << string ("CCPlayer ") + strVideoFormat + string (formatDesc.IsVANC() ? " VANC" : "");
	CNTV2CaptionRenderer::BurnString (oss.str (), NTV2Line21Attributes (NTV2_CC608_White, NTV2_CC608_Cyan),
										formatDesc.GetTopVisibleRowAddress (mpVideoBuffer),
										formatDesc.GetVisibleRasterDimensions (), mPixelFormat, formatDesc.GetBytesPerRow (),  1, 1);	}	//	row 1, col 1
	{ostringstream	oss;	oss << formatDesc.GetRasterWidth() << "Wx" << formatDesc.GetFullRasterHeight() << "H  " << ::NTV2FrameBufferFormatToString (mPixelFormat, true) << string (20, ' ');
	CNTV2CaptionRenderer::BurnString (oss.str (), NTV2Line21Attributes (NTV2_CC608_White, NTV2_CC608_Cyan),
										formatDesc.GetTopVisibleRowAddress (mpVideoBuffer),
										formatDesc.GetVisibleRasterDimensions (), mPixelFormat, formatDesc.GetBytesPerRow (),  2, 1);	}	//	row 2, col 1
	return AJA_STATUS_SUCCESS;

}	//	SetUpBackgroundPatternBuffer


AJAStatus NTV2CCPlayer::SetUpOutputVideo (void)
{
	//	Preflight checks...
	if (!::NTV2DeviceCanDoVideoFormat (mDeviceID, mVideoFormat))
	{
		cerr << "## ERROR:  Device cannot accommodate video format '" << ::NTV2VideoFormatToString (mVideoFormat) << "'" << endl;
		return AJA_STATUS_UNSUPPORTED;
	}
	if ((mOutputChannel == NTV2_CHANNEL1) && (!::NTV2DeviceCanDoFrameStore1Display (mDeviceID)))
	{
		cerr << "## NOTE:  Device cannot playout thru FrameStore 1 -- using channel 2 instead" << endl;
		mOutputChannel = NTV2_CHANNEL2;
	}
	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat) && mOutputChannel != NTV2_CHANNEL1 && mOutputChannel != NTV2_CHANNEL5)
	{
		cerr << "## ERROR:  Quad-frame formats require output channel 1 or 5, not '" << ::NTV2ChannelToString (mOutputChannel, true) << "'" << endl;
		return AJA_STATUS_BAD_PARAM;
	}
	if (NTV2_IS_625_FORMAT (mVideoFormat))
	{
		cerr << "## ERROR:  Sorry, 'ntv2ccplayer' does not support 625 format" << endl;
		return AJA_STATUS_UNSUPPORTED;
	}
	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat) && mConfig.fForceVanc)
	{
		cerr << "## ERROR:  Cannot use VANC with quad-frame format" << endl;
		return AJA_STATUS_UNSUPPORTED;
	}
	if (NTV2_IS_SD_VIDEO_FORMAT (mVideoFormat) && !mConfig.fSuppressLine21 && mPixelFormat != NTV2_FBF_8BIT_YCBCR && mPixelFormat != NTV2_FBF_10BIT_YCBCR)
	{
		cerr << "## ERROR:  Cannot replace line 21 in '" << ::NTV2FrameBufferFormatToString (mPixelFormat) << "' frame buffer -- only '2vuy' or 'v210' allowed" << endl;
		return AJA_STATUS_UNSUPPORTED;
	}
	if (UWord (mOutputChannel) >= ::NTV2DeviceGetNumFrameStores (mDeviceID))
	{
		cerr	<< "## ERROR:  Cannot use channel '" << mOutputChannel+1 << "' -- device only supports channel 1"
				<< (::NTV2DeviceGetNumFrameStores (mDeviceID) > 1  ?  string (" thru ") + string (1, uint8_t (::NTV2DeviceGetNumFrameStores (mDeviceID)+'0'))  :  "") << endl;
		return AJA_STATUS_UNSUPPORTED;
	}

	//	Enable the required frame store(s)...
	mDevice.EnableChannel (mOutputChannel);
	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
	{
		mDevice.EnableChannel (NTV2Channel (mOutputChannel + 1));
		mDevice.EnableChannel (NTV2Channel (mOutputChannel + 2));
		mDevice.EnableChannel (NTV2Channel (mOutputChannel + 3));
		mDevice.SetMultiFormatMode (true);
	}

	//	Set video format/standard...
	mDevice.SetVideoFormat (mVideoFormat, AJA_RETAIL_DEFAULT, false, mOutputChannel);

	if (!::NTV2DeviceCanDoFrameBufferFormat (mDeviceID, mPixelFormat))
	{
		cerr << "## ERROR:  Device cannot accommodate pixel format '" << ::NTV2FrameBufferFormatToString (mPixelFormat) << "'" << endl;
		return AJA_STATUS_UNSUPPORTED;
	}
	mDevice.SetFrameBufferFormat (mOutputChannel, mPixelFormat);
	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
	{
		mDevice.SetFrameBufferFormat (NTV2Channel (mOutputChannel + 1), mPixelFormat);
		mDevice.SetFrameBufferFormat (NTV2Channel (mOutputChannel + 2), mPixelFormat);
		mDevice.SetFrameBufferFormat (NTV2Channel (mOutputChannel + 3), mPixelFormat);
	}

	//	Enable VANC only if device has no Anc insertion capability, or if --vanc specified...
	if (mTallVANC)
	{
		mDevice.SetEnableVANCData (mTallVANC, mTallerVANC, mOutputChannel);
		if (::Is8BitFrameBufferFormat (mPixelFormat))
		{
			mDevice.SetVANCShiftMode (mOutputChannel, NTV2_VANCDATA_8BITSHIFT_ENABLE);	//	8-bit FBFs require VANC bit shift
			if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
			{
				mDevice.SetVANCShiftMode (NTV2Channel (mOutputChannel + 1), NTV2_VANCDATA_8BITSHIFT_ENABLE);
				mDevice.SetVANCShiftMode (NTV2Channel (mOutputChannel + 2), NTV2_VANCDATA_8BITSHIFT_ENABLE);
				mDevice.SetVANCShiftMode (NTV2Channel (mOutputChannel + 3), NTV2_VANCDATA_8BITSHIFT_ENABLE);
			}
		}
	}

	//	Create our caption encoders...
	if (!CNTV2CaptionEncoder608::Create (m608Encoder))
		return AJA_STATUS_MEMORY;
	if (!CNTV2CaptionEncoder708::Create (m708Encoder))
		return AJA_STATUS_MEMORY;

	mDevice.SetReference (NTV2_REFERENCE_FREERUN);	//	Use free-run for playout

	//	Subscribe to the output interrupt...
	mDevice.SubscribeOutputVerticalEvent (mOutputChannel);
	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
	{
		mDevice.SubscribeOutputVerticalEvent (NTV2Channel (mOutputChannel + 1));
		mDevice.SubscribeOutputVerticalEvent (NTV2Channel (mOutputChannel + 2));
		mDevice.SubscribeOutputVerticalEvent (NTV2Channel (mOutputChannel + 3));
	}

	mVideoBufferSize = ::GetVideoWriteSize (mVideoFormat, mPixelFormat, mTallVANC, mTallerVANC);
	mDevice.GetFrameRate (mFrameRate);

	cerr	<< "## NOTE:  Generating '" << ::NTV2VideoFormatToString (mVideoFormat) << "' using " << (mConfig.fForceVanc ? "VANC" : "device Anc inserter")
			<< " on '" << mDevice.GetDisplayName () << "' output " << ::NTV2ChannelToString (mOutputChannel) << " and " << ::NTV2FrameBufferFormatToString (mPixelFormat) << endl;

	return AJA_STATUS_SUCCESS;

}	//	SetUpOutputVideo


AJAStatus NTV2CCPlayer::RouteOutputSignal (void)
{
	const NTV2Standard		outputStandard	(::GetNTV2StandardFromVideoFormat (mVideoFormat));
	const UWord				numVideoOutputs	(::NTV2DeviceGetNumVideoOutputs (mDeviceID));
	bool					isRGB			(::IsRGBFormat (mPixelFormat));
	NTV2OutputCrosspointID	cscVidOutXpt	(::GetCSCOutputXptFromChannel (mOutputChannel,  false/*isKey*/,  true/*isRGB*/));
	NTV2OutputCrosspointID	fsVidOutXpt		(::GetFrameBufferOutputXptFromChannel (mOutputChannel,  false/*isRGB*/,  false/*is425*/));

	//	If device has no RGB conversion capability for the desired channel, use YUV instead
	if (UWord (mOutputChannel) > ::NTV2DeviceGetNumCSCs (mDeviceID))
		isRGB = false;

	if (isRGB && mConfig.fForceVanc)
	{
		//	FOR NOW:	Always output YCbCr, do colorspace conversion
		//	TODO:		Output RGB over-the-wire if device/vidFormat/pixFormat can do it
		//	At this point, either the device has no Anc insertion capabilities, or the driver is too old to talk to an Anc inserter,
		//	or the '--vanc' option was specified on the command line. Since CCPlayer at this time only outputs YCbCr,
		//	RGB must be routed through a CSC to the output, which corrupts the data in the VANC area.
		cerr << "## ERROR:  Routing '" << ::NTV2FrameBufferFormatToString (mPixelFormat, true) << "' thru CSC will clobber VANC data " << endl;
		return AJA_STATUS_UNSUPPORTED;
	}	//	if RGB FBF

	if (!mConfig.fDoMultiFormat)
		mDevice.ClearRouting ();

	if (NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
	{
		NTV2_ASSERT (mOutputChannel == NTV2_CHANNEL1 || mOutputChannel == NTV2_CHANNEL5);	//	In quad mode, channel must be 1 or 5!
		if (isRGB)
		{
			//	For RGB, the frame buffer outputs feed the CSC inputs, and the CSC outputs feed the SDIOut inputs...
			for (unsigned ch (0);  ch < 4;  ch++)
			{
				mDevice.Connect (::GetCSCInputXptFromChannel (NTV2Channel (mOutputChannel + ch)), ::GetFrameBufferOutputXptFromChannel (NTV2Channel (mOutputChannel + ch), true));
				mDevice.Connect (::GetSDIOutputInputXpt (NTV2Channel (mOutputChannel + ch)), ::GetCSCOutputXptFromChannel (NTV2Channel (mOutputChannel + ch)));
				if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID))
					mDevice.SetSDITransmitEnable (NTV2Channel (mOutputChannel + ch), true);
			}
		}
		else
		{
			//	For YCbCr, the frame buffer outputs feed the SDI outputs directly
			for (unsigned ch (0);  ch < 4;  ch++)
			{
				mDevice.Connect (::GetSDIOutputInputXpt (NTV2Channel (mOutputChannel + ch)), ::GetFrameBufferOutputXptFromChannel (NTV2Channel (mOutputChannel + ch)));
				if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID))
					mDevice.SetSDITransmitEnable (NTV2Channel (mOutputChannel + ch), true);
			}
		}
	}	//	if quad/UHD
	else
	{
		if (isRGB)
			mDevice.Connect (::GetCSCInputXptFromChannel (mOutputChannel, false/*isKeyInput*/),  fsVidOutXpt);

		if (mConfig.fDoMultiFormat)
		{
			//	Multiformat --- route the one SDI output to the CSC video output (RGB) or FrameStore output (YUV)...
			if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID))
				mDevice.SetSDITransmitEnable (mOutputChannel, true);

			mDevice.Connect (::GetSDIOutputInputXpt (mOutputChannel, false/*isDS2*/),  isRGB ? cscVidOutXpt : fsVidOutXpt);
			mDevice.SetSDIOutputStandard (mOutputChannel, outputStandard);
		}
		else
		{
			//	Not multiformat:  Route all possible SDI outputs to CSC video output (RGB) or FrameStore output (YUV)...
			mDevice.ClearRouting ();

			for (NTV2Channel chan (NTV2_CHANNEL1);  ULWord (chan) < numVideoOutputs;  chan = NTV2Channel (chan + 1))
			{
				if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID))
					mDevice.SetSDITransmitEnable (chan, true);		//	Make it an output

				mDevice.Connect (::GetSDIOutputInputXpt (chan, false/*isDS2*/),  isRGB ? cscVidOutXpt : fsVidOutXpt);
				mDevice.SetSDIOutputStandard (chan, outputStandard);
			}	//	for each output spigot

			if (::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtAnalogOut1))
				mDevice.Connect (::GetOutputDestInputXpt (NTV2_OUTPUTDESTINATION_ANALOG),  isRGB ? cscVidOutXpt : fsVidOutXpt);

			if (::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtHDMIOut1)
				|| ::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtHDMIOut1v2)
				|| ::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtHDMIOut1v3))
					mDevice.Connect (::GetOutputDestInputXpt (NTV2_OUTPUTDESTINATION_HDMI),  isRGB ? cscVidOutXpt : fsVidOutXpt);
		}
	}	//	else non-quad

	return AJA_STATUS_SUCCESS;

}	//	RouteOutputSignal


AJAStatus NTV2CCPlayer::Run ()
{
	//	Start the threads...
	StartPlayoutThread ();
	StartCaptionGeneratorThreads ();

	return AJA_STATUS_SUCCESS;

}	//	Run


//////////////////////////////////////////////
//	Caption generator thread
//////////////////////////////////////////////


void NTV2CCPlayer::StartCaptionGeneratorThreads ()
{
	//	Create and start the caption generator threads, one per caption channel...
	for (CaptionChanGenMapConstIter iter (mConfig.fChannelGenerators.begin ());  iter != mConfig.fChannelGenerators.end ();  ++iter)
	{
		const CCGeneratorConfig &	ccGeneratorConfig	(iter->second);
		const size_t				ccChannel			(ccGeneratorConfig.fCaptionChannel);
		AJAThread *					pThread				(new AJAThread ());
		if (pThread)
		{
			mGeneratorThreads [ccChannel] = pThread;
			pThread->Attach (CaptionGeneratorThreadStatic, (void *) ccChannel);	//	The generator thread needs to know its caption channel
			pThread->Start ();
		}
	}

}	//	StartCaptionGeneratorThreads


void NTV2CCPlayer::CaptionGeneratorThreadStatic (AJAThread * pThread, void * pContext)
{
	(void) pThread;
	const size_t	ccChannel	((size_t) pContext);

	//	Grab the NTV2CCPlayer instance pointer from the gApp global, call its GenerateCaptions method,
	//	and pass it the given caption channel (passed in via the pContext parameter)...
	if (gApp)
		gApp->GenerateCaptions (NTV2Line21Channel (ccChannel));

}	//	CaptionGeneratorThreadStatic


void NTV2CCPlayer::GenerateCaptions (const NTV2Line21Channel inCCChannel)
{
	CaptionChanGenMapConstIter	iter	(mConfig.fChannelGenerators.find (inCCChannel));
	NTV2_ASSERT (iter != mConfig.fChannelGenerators.end ());

	const CCGeneratorConfig &	ccGeneratorConfig	(iter->second);
	const NTV2Line21Mode		captionMode			(ccGeneratorConfig.fCaptionMode);		//	My caption mode (paint-on, pop-on, roll-up, etc.)
	const double				charsPerMinute		(ccGeneratorConfig.fCharsPerMinute);	//	Desired caption rate (in characters per minute)
	const bool					newlinesMakeNewRows	(ccGeneratorConfig.fNewLinesAreNewRows);//	Newline characters cause row breaks?
	const AtEndAction			endAction			(ccGeneratorConfig.fEndAction);			//	What to do after last file is done playing
	StringList					filesToPlay			(ccGeneratorConfig.fFilesToPlay);		//	List of text files to play
	unsigned					linesWanted			(3);									//	Desired number of lines to Enqueue in one shot (min 1, max 4)
	bool						quitThisGenerator	(false);								//	Set true to exit this function/thread
	const UWord					paintPopTopRow		(9);									//	PaintOn/PopOn only:	top display row
	const UWord					paintPopMaxNumRows	(15 - paintPopTopRow + 1);				//	PaintOn/PopOn only:	number of rows to fill to bottom
	UWord						lineTally			(0);									//	PaintOn/PopOn only:	used to calculate display row

	//cerr << "## DEBUG:  Starting " << NTV2Line21ChannelToStr (inCCChannel) << " generator thread" << endl;
	if (IsLine21TextChannel (inCCChannel) || !IsLine21RollUpMode (captionMode))
		linesWanted = 1;
	while (!mCaptionGeneratorQuit)
	{
		CaptionSourceList	captionSources	(::GetCaptionSources (filesToPlay, charsPerMinute));
		while (!mCaptionGeneratorQuit && !captionSources.empty ())
		{
			CaptionSourcePtr	captionSource (captionSources.front ());
			captionSources.pop_front ();

			captionSource->SetTextMode (IsLine21TextChannel (inCCChannel));		//	Set CaptionSource to Text Mode if caption channel is TX1/TX2/TX3/TX4

			while (!mCaptionGeneratorQuit && !captionSource->IsFinished ())
			{
				//	Enqueue another message only if the encoder has less than 200 messages queued up (to prevent runaway memory consumption)...
				if (m608Encoder->GetQueuedMessageCount () < 200)
				{
					string	str	(captionSource->GetNextCaptionRow (newlinesMakeNewRows));

					if (IsLine21CaptionChannel (inCCChannel) && !IsLine21RollUpMode (captionMode) && linesWanted > 1)
						for (UWord lines (linesWanted - 1);  lines;  lines--)
							str += "\n" + captionSource->GetNextCaptionRow ();

					if (!mEmitStats && !str.empty ())
						cout << str << endl;	//	Echo caption lines (if not emitting stats)

					//	For now, only the 608 encoder generates caption data.
					//	Someday we'll generate captions using 708-specific features (e.g., windowing, etc.).

					//	Convert the UTF-8 string into a string containing CEA-608 byte sequences, which is
					//	what the Enqueue*Message functions accept (except for EnqueueTextMessage)...
					if (!IsLine21TextChannel (inCCChannel))
						str = CUtf8Helpers::Utf8ToCEA608String (str, inCCChannel);
					switch (captionMode)
					{
						case NTV2_CC608_CapModePopOn:
							m608Encoder->EnqueuePopOnMessage (str, inCCChannel, lineTally % paintPopMaxNumRows + paintPopTopRow, 1);
							break;
						case NTV2_CC608_CapModeRollUp2:
						case NTV2_CC608_CapModeRollUp3:
						case NTV2_CC608_CapModeRollUp4:
							m608Encoder->EnqueueRollUpMessage (str, captionMode, inCCChannel);
							break;
						case NTV2_CC608_CapModePaintOn:
							m608Encoder->EnqueuePaintOnMessage (str, lineTally % paintPopMaxNumRows == 0, inCCChannel, lineTally % paintPopMaxNumRows + paintPopTopRow, 1);
							break;
						default:
							NTV2_ASSERT (IsLine21TextChannel (inCCChannel));
							m608Encoder->EnqueueTextMessage (str, lineTally == 0, inCCChannel);
							break;
					}	//	switch on caption mode
					lineTally++;
				}	//	if encoder has < 200 messages queued
				else
					AJATime::Sleep (1000);
			}	//	loop til captionSource is finished (or mCaptionGeneratorQuit)
		}	//	for each captionSource

		switch (endAction)
		{
			case AtEndAction_Quit:		//cerr << "## DEBUG:  " << ::NTV2Line21ChannelToStr (inCCChannel) << " generator signaling 'main' to terminate..." << endl;
										filesToPlay.clear ();			//	Clear to-do list if we loop again
										::SignalHandler (SIG_AJA_STOP);	//	Signal 'main' to terminate
										quitThisGenerator = true;		//	Instantly terminates me
										break;

			case AtEndAction_Repeat:	//cerr << "## DEBUG:  " << ::NTV2Line21ChannelToStr (inCCChannel) << " generator repeating..." << endl;
										break;

			case AtEndAction_Idle:		//cerr << "## DEBUG:  " << ::NTV2Line21ChannelToStr (inCCChannel) << " generator entering idle mode..." << endl;
										filesToPlay.clear ();			//	Clear to-do list if we loop again
										quitThisGenerator = true;		//	Instantly terminates me
										break;

			default:					NTV2_ASSERT (false && "bad end action");
										break;
		}
		if (quitThisGenerator)
			break;
	}	//	loop til mCaptionGeneratorQuit

	//	Let's be nice, and inject an EDM (Erase Displayed Memory) control message.
	//	This will prevent frozen, on-screen captions from remaining in/on downstream decoders/monitors...
	m608Encoder->Erase (inCCChannel);
	//cerr << "## DEBUG:  " << ::NTV2Line21ChannelToStr (inCCChannel) << " generator thread exit" << endl;

}	//	GenerateCaptions


//////////////////////////////////////////////
//	Playout thread
//////////////////////////////////////////////


void NTV2CCPlayer::StartPlayoutThread ()
{
	//	Create and start the playout thread...
	NTV2_ASSERT (mPlayThread == NULL);
	mPlayThread = new AJAThread ();
	mPlayThread->Attach (PlayThreadStatic, this);
	mPlayThread->SetPriority (AJA_ThreadPriority_High);
	mPlayThread->Start ();

}	//	StartPlayoutThread


//	The playout thread function
void NTV2CCPlayer::PlayThreadStatic (AJAThread * pThread, void * pContext)		//	static
{
	(void) pThread;
	//	Grab the NTV2CCPlayer instance pointer from the pContext parameter,
	//	then call its PlayoutFrames method...
	NTV2CCPlayer *	pApp	(reinterpret_cast <NTV2CCPlayer *> (pContext));
	pApp->PlayoutFrames ();

}	//	PlayThreadStatic


void NTV2CCPlayer::PlayoutFrames (void)
{
	static const NTV2Line21Attributes		kBlueOnWhite	(NTV2_CC608_Blue,  NTV2_CC608_White,   NTV2_CC608_Opaque);
	static const NTV2Line21Attributes		kRedOnYellow	(NTV2_CC608_Red,   NTV2_CC608_Yellow,  NTV2_CC608_Opaque);
	static const AJAAncillaryDataLocation	kCEA708Location	(AJAAncillaryDataLink_A,  AJAAncillaryDataVideoStream_Y,  AJAAncillaryDataSpace_VANC,  9);
	static UByte				pF1AncBuffer [1024];
	static UByte				pF2AncBuffer [1024];
	static const uint32_t		AUDIOBYTES_MAX_48K	(201 * 1024);	//	Max audio bytes per frame (16 chls x 4 bytes x 67 msec/fr x 48000 Hz)
	static const double			gAmplitudes [16]	= {	0.10,			0.15,			0.20,			0.25,			0.30,			0.35,			0.40,
														0.45,			0.50,			0.55,			0.60,			0.65,			0.70,			0.75,
														0.80,			0.85};
	static const double			gFrequencies [16]	= {	150.00000000,	200.00000000,	266.66666667,	355.55555556,	474.07407407,	632.09876543,
														842.79835391,	1123.73113855,	1498.30818473,	1997.74424630,	2663.65899507,	3551.54532676,
														4735.39376902,	6313.85835869,	8418.47781159,	11224.63708211};
	const NTV2Standard			standard			(::GetNTV2StandardFromVideoFormat (mVideoFormat));
	const NTV2FormatDescriptor	formatDesc			(::GetFormatDescriptor (standard, mPixelFormat, mTallVANC, Is2KFormat (mVideoFormat), mTallerVANC));
	const ULWord				bytesPerRow			(formatDesc.GetBytesPerRow ());
	const ULWord				vancLineNum			(CNTV2SMPTEAncData::GetVancLineOffset (formatDesc, ::GetSmpteLineNumber (standard),
																							CNTV2SMPTEAncData::GetCaptionAncLineNumber (mVideoFormat)));
	CNTV2Line21Captioner		F1Line21Encoder;	//	Used to encode Field 1 analog (line 21) waveform
	CNTV2Line21Captioner		F2Line21Encoder;	//	Used to encode Field 2 analog (line 21) waveform
	CaptionData					captionData;		//	Current frame's 608 caption bytes (Fields 1 and 2)
	ULWord						acOptionFlags		(0);
	ULWord						currentSample		(0);
	NTV2AudioSystem				audioSystem			(NTV2_AUDIOSYSTEM_INVALID);
	ULWord						numAudioChannels	(0);
	Bouncer						colBouncer			(32 - 11 /*upperLimit*/, 0 /*lowerLimit*/, 0 /*startAt*/);
	NTV2_POINTER				pAudioBuffer		(0);
	AUTOCIRCULATE_STATUS		acStatus;
	AUTOCIRCULATE_TRANSFER		xferInfo;

	if (!mConfig.fSuppressAudio)
	{
		//	Audio setup...
		audioSystem = (::NTV2DeviceGetNumAudioSystems (mDeviceID) > 1)  ?  ::NTV2ChannelToAudioSystem (mOutputChannel)  :  NTV2_AUDIOSYSTEM_1;
		mDevice.SetNumberAudioChannels (::NTV2DeviceGetMaxAudioChannels (mDeviceID), audioSystem);	//	Use all possible audio channels
		mDevice.GetNumberAudioChannels (numAudioChannels, audioSystem);								//	Get actual number of audio channels in use
		mDevice.SetAudioRate (NTV2_AUDIO_48K, audioSystem);											//	48kHz sample rate
		mDevice.SetAudioBufferSize (NTV2_AUDIO_BUFFER_BIG, audioSystem);							//	Use 4MB output buffer
		mDevice.SetSDIOutputAudioSystem (mOutputChannel, audioSystem);								//	Set output DS1 audio embedders to use designated audio system
		mDevice.SetSDIOutputDS2AudioSystem (mOutputChannel, audioSystem);							//	Set output DS2 audio embedders to use designated audio system
		mDevice.SetAudioLoopBack (NTV2_AUDIO_LOOPBACK_OFF, audioSystem);							//	Disable loopback, since not E-E mode
		pAudioBuffer.Allocate (AUDIOBYTES_MAX_48K);													//	Allocate audio buffer (large enough for one frame's audio)
		pAudioBuffer.Fill (UByte (0));																//	Zero audio buffer
		if (!mConfig.fDoMultiFormat)
		{
			for (UWord chan (0);  chan < ::NTV2DeviceGetNumVideoOutputs (mDeviceID);  chan++)
			{
				mDevice.SetSDIOutputAudioSystem (NTV2Channel (chan), audioSystem);
				mDevice.SetSDIOutputDS2AudioSystem (NTV2Channel (chan), audioSystem);
			}
			if (::NTV2DeviceGetNumHDMIAudioOutputChannels (mDeviceID) == 8)
			{
				mDevice.SetHDMIOutAudioChannels (NTV2_HDMIAudio8Channels);
				mDevice.SetHDMIOutAudioSource8Channel (NTV2_AudioChannel1_8, audioSystem);
			}
		}	//	if not multiformat
	}	//	if audio not suppressed

	//	Set up the transfer buffers...
	xferInfo.SetVideoBuffer (reinterpret_cast <ULWord *> (mpVideoBuffer), mVideoBufferSize);
	if (!mConfig.fForceVanc)
		xferInfo.SetAncBuffers (reinterpret_cast <ULWord *> (pF1AncBuffer), sizeof (pF1AncBuffer),
								reinterpret_cast <ULWord *> (pF2AncBuffer), sizeof (pF2AncBuffer));
	if (mConfig.fSuppressTimecode)
		xferInfo.acOutputTimeCodes.Set (NULL, 0);

	//	Set up playout AutoCirculate and start it...
	if (!mConfig.fForceVanc)
		acOptionFlags |= AUTOCIRCULATE_WITH_ANC;
	if (!mConfig.fSuppressTimecode)
		acOptionFlags |= AUTOCIRCULATE_WITH_RP188;
	if (!mConfig.fSuppressTimecode  &&  !mConfig.fDoMultiFormat  &&  ::NTV2DeviceCanDoLTCOutN (mDeviceID, 0))
		acOptionFlags |= AUTOCIRCULATE_WITH_LTC;		//	Emit analog LTC if we "own" the device
	mDevice.AutoCirculateStop (mOutputChannel);			//	Maybe some other app left this A/C channel running
	{
		AJAAutoLock	autoLock (mLock);	//	Avoid AutoCirculate buffer collisions
		mDevice.AutoCirculateInitForOutput (mOutputChannel,  7,  audioSystem,  acOptionFlags);
	}
	mDevice.AutoCirculateStart (mOutputChannel);

	while (!mPlayerQuit)
	{
		mDevice.AutoCirculateGetStatus (mOutputChannel, acStatus);
		mDroppedFrameTally = acStatus.acFramesDropped;
		if (acStatus.CanAcceptMoreOutputFrames ())			//	Room for another frame on the device?
		{
			m608Encoder->GetNextCaptionData (captionData);	//	Pop queued captions from 608 encoder waiting to be transmitted
			m708Encoder->Set608CaptionData (captionData);	//	Set the 708 encoder's 608 caption data (for both F1 and F2)
			if (m708Encoder->MakeSMPTE334AncPacket (mFrameRate, NTV2_CC608_Field1))		//	Generate F1's SMPTE-334 Anc data packet
			{
				bool	doVanc	(mConfig.fForceVanc);	//	True if --vanc option set, or Anc functions fail
				if (!doVanc)
				{
					uint32_t					pktSizeInBytes	(0);
					AJAAncillaryData_Cea708		pkt;
					pkt.SetFromSMPTE334 (m708Encoder->GetSMPTE334Data (), m708Encoder->GetSMPTE334Size (), kCEA708Location);
					pkt.Calculate8BitChecksum ();
					doVanc = AJA_FAILURE (pkt.GenerateTransmitData (pF1AncBuffer, sizeof (pF1AncBuffer), pktSizeInBytes));
				}
				if (doVanc)
					m708Encoder->InsertSMPTE334AncPacketInVideoFrame (mpVideoBuffer, mVideoFormat, mPixelFormat, vancLineNum);	//	Embed into FB VANC area
			}

			if (!IsProgressivePicture (mVideoFormat) && m708Encoder->MakeSMPTE334AncPacket (mFrameRate, NTV2_CC608_Field2))		//	Generate F2's SMPTE-334 Anc data packet (interlace only)
			{
				bool	doVanc	(mConfig.fForceVanc);	//	True if --vanc option set, or Anc functions fail
				if (!doVanc)
				{
					uint32_t					pktSizeInBytes	(0);
					AJAAncillaryData_Cea708		pkt;
					pkt.SetFromSMPTE334 (m708Encoder->GetSMPTE334Data (), m708Encoder->GetSMPTE334Size (), kCEA708Location);
					pkt.Calculate8BitChecksum ();
					doVanc = AJA_FAILURE (pkt.GenerateTransmitData (pF2AncBuffer, sizeof (pF2AncBuffer), pktSizeInBytes));
				}
				if (doVanc)
					m708Encoder->InsertSMPTE334AncPacketInVideoFrame (mpVideoBuffer, mVideoFormat, mPixelFormat, vancLineNum);	//	Embed into FB VANC area
			}

			if (NTV2_IS_SD_VIDEO_FORMAT (mVideoFormat) && !mConfig.fSuppressLine21)
			{
				const ULWord	kLine21F1RowNum		(mTallVANC && mTallerVANC  ?  29  :  (mTallVANC  ?  23  :  1));
				const ULWord	kLine21F2RowNum		(kLine21F1RowNum + 1);
				UByte *			pF1EncodedYUV8Line	(F1Line21Encoder.EncodeLine (captionData.f1_char1, captionData.f1_char2));
				UByte *			pF2EncodedYUV8Line	(F2Line21Encoder.EncodeLine (captionData.f2_char1, captionData.f2_char2));
				UByte *			pF1Line21InBuffer	(mpVideoBuffer + (kLine21F1RowNum * bytesPerRow));
				UByte *			pF2Line21InBuffer	(mpVideoBuffer + (kLine21F2RowNum * bytesPerRow));

				if (mPixelFormat == NTV2_FBF_8BIT_YCBCR)
					::memcpy (pF1Line21InBuffer, pF1EncodedYUV8Line, bytesPerRow);		//	Replace line 21 with the line handed to us by EncodeLine
				else if (mPixelFormat == NTV2_FBF_10BIT_YCBCR)
					::ConvertLine_2vuy_to_v210 (pF1EncodedYUV8Line, reinterpret_cast <ULWord *> (pF1Line21InBuffer), 720);	//	Convert to 10-bit YUV in-place

				if (mPixelFormat == NTV2_FBF_8BIT_YCBCR)
					::memcpy (pF2Line21InBuffer, pF2EncodedYUV8Line, bytesPerRow);		//	Replace line 21 with the line handed to us by EncodeLine
				else if (mPixelFormat == NTV2_FBF_10BIT_YCBCR)
					::ConvertLine_2vuy_to_v210 (pF2EncodedYUV8Line, reinterpret_cast <ULWord *> (pF2Line21InBuffer), 720);	//	Convert to 10-bit YUV in-place
			}	//	if SD and --noline21 wasn't specified

			if (!mConfig.fSuppressTimecode)
			{
				const TimecodeFormat	tcFormat	(CNTV2DemoCommon::NTV2FrameRate2TimecodeFormat (mFrameRate));
				const CRP188			rp188		(acStatus.acFramesProcessed, tcFormat);
				char					tcString[]	= {"                                "};
				const UWord				colShift	(acStatus.acFramesProcessed % 10 == 0  ?  colBouncer.Next()  :  colBouncer.Value());
				bool					tcOK		(false);
				NTV2_RP188				tc;
				rp188.GetRP188Reg  (tc);
				if (!NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat) && !mConfig.fDoMultiFormat)
					tcOK = xferInfo.SetAllOutputTimeCodes (tc);
				else
				{
					//	Be more selective as to which output spigots get the generated timecode...
					NTV2TimeCodes	timecodes;
					for (int num (0);  num < 4;  num++)
					{
						timecodes [::NTV2ChannelToTimecodeIndex (NTV2Channel (mOutputChannel + num), false)] = tc;
						timecodes [::NTV2ChannelToTimecodeIndex (NTV2Channel (mOutputChannel + num), true)] = tc;
						if (acOptionFlags & AUTOCIRCULATE_WITH_LTC)
						{
							timecodes [NTV2_TCINDEX_LTC1] = tc;
							if (::NTV2DeviceCanDoLTCOutN (mDeviceID, 1))
								timecodes [NTV2_TCINDEX_LTC2] = tc;
						}
						if (!NTV2_IS_QUAD_FRAME_FORMAT (mVideoFormat))
							break;
					}
					tcOK = xferInfo.SetOutputTimeCodes (timecodes);
				}
				::memcpy (tcString + colShift, rp188.GetRP188CString (), 11);
				CNTV2CaptionRenderer::BurnString (tcString, tcOK ? kBlueOnWhite : kRedOnYellow, formatDesc.GetTopVisibleRowAddress (mpVideoBuffer),
													formatDesc.GetVisibleRasterDimensions (), mPixelFormat, bytesPerRow, 3, 1);
			}

			if (!mConfig.fSuppressAudio && !pAudioBuffer.IsNULL ())
				xferInfo.SetAudioBuffer (reinterpret_cast <ULWord *> (pAudioBuffer.GetHostPointer ()),
										::AddAudioTone (reinterpret_cast <ULWord *> (pAudioBuffer.GetHostPointer ()),	//	audio buffer to fill
														currentSample,					//	sample for continuing the waveform
														::GetAudioSamplesPerFrame (mFrameRate, NTV2_AUDIO_48K, acStatus.acFramesProcessed),	//	# samples to generate
														48000.0,						//	sample rate [Hz]
														gAmplitudes,					//	per-channel amplitudes
														gFrequencies,					//	per-channel tone frequencies [Hz]
														31,								//	bits per sample
														false,							//	don't byte swap
														numAudioChannels));				//	number of audio channels
			mDevice.AutoCirculateTransfer (mOutputChannel, xferInfo);
		}	//	if room for another output frame on device
		else
			mDevice.WaitForOutputVerticalInterrupt (mOutputChannel);
	}	//	loop til quit signaled

	//	Stop AutoCirculate...
	mDevice.AutoCirculateStop (mOutputChannel);
	//cerr << "## DEBUG:  Player thread exit" << endl;

}	//	PlayoutFrames


void NTV2CCPlayer::GetStatus (	size_t & outMessagesQueued,	size_t & outBytesQueued,
								size_t & outTotMsgsEnq,		size_t & outTotBytesEnq,
								size_t & outTotMsgsDeq,		size_t & outTotBytesDeq,
								size_t & outMaxQueDepth,	size_t & outDroppedFrames) const
{
	if (m608Encoder)
	{
		outMessagesQueued	= m608Encoder->GetQueuedMessageCount ();
		outBytesQueued		= m608Encoder->GetQueuedByteCount ();
		outTotMsgsEnq		= m608Encoder->GetEnqueueMessageTally ();
		outTotBytesEnq		= m608Encoder->GetEnqueueByteTally ();
		outTotMsgsDeq		= m608Encoder->GetDequeueMessageTally ();
		outTotBytesDeq		= m608Encoder->GetDequeueByteTally ();
		outMaxQueDepth		= m608Encoder->GetHighestQueueDepth ();
	}
	else
		outMessagesQueued = outBytesQueued = outTotMsgsEnq = outTotBytesEnq = outTotMsgsDeq = outTotBytesDeq = outMaxQueDepth = 0;
	outDroppedFrames = size_t (mDroppedFrameTally);

}	//	GetStatus
