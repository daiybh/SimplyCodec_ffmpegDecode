/////////////////////////////////////////////////////////////////////////////
// ntv2konaflashprogram.h
//
// Copyright (C) 2010 AJA Video Systems, Inc.  Proprietary and Confidential information.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef NTV2KONAFLASHPROGRAM_H
#define NTV2KONAFLASHPROGRAM_H

#include "ntv2card.h"
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "ntv2debug.h"
#include "ntv2mcsfile.h"

#if defined(AJALinux) || defined(AJAMac)
#include <netinet/in.h>	// htons and friends
#endif

#define MAXBITFILE_HEADERSIZE 256
#define MAXMCSINFOSIZE 256
#define MCS_STEPS      6

typedef enum 
{
	MAIN_FLASHBLOCK,
	FAILSAFE_FLASHBLOCK,
	AUTO_FLASHBLOCK,
	SOC1_FLASHBLOCK,
	SOC2_FLASHBLOCK,
	MAC_FLASHBLOCK,
	MCS_INFO_BLOCK
}FlashBlockID;

typedef enum
{
	BANK_0,
	BANK_1,
	BANK_2,
	BANK_3
}BankSelect;

struct MacAddr
{
	uint8_t mac[6];
};


class AJAExport CNTV2KonaFlashProgram : public CNTV2Card
{
public:
	CNTV2KonaFlashProgram();

	CNTV2KonaFlashProgram (const UWord boardNumber, const bool displayErrorMessage = false,
		const UWord ulBoardType = DEVICETYPE_NTV2);	//	 | BOARDTYPE_AJAXENA2);

	virtual ~CNTV2KonaFlashProgram();

public:
	virtual bool	SetBoard (UWord boardNumber, NTV2DeviceType boardType = DEVICETYPE_NTV2, uint32_t index = 0);
	bool			ReadHeader (FlashBlockID flashBlock);
	bool			ReadInfoString();
	void			SetBitFile (const char *bitFileName, FlashBlockID blockNumber = AUTO_FLASHBLOCK);
	bool			SetMCSFile (const char *sMCSFileName);
    void			Program (bool verify = true);
    bool            ProgramFromMCS(bool verify);
    bool            ProgramSOC(bool verify = true);
	void			ProgramCustom ( const char *sCustomFileName, const uint32_t addr);
	void			EraseBlock ();
	void			EraseBlock (FlashBlockID blockNumber);
	bool			EraseChip (UWord chip = 0);
	bool			CreateSRecord ();
	bool			CreateEDIDIntelRecord ();
	void			SetQuietMode ()		{_bQuiet = true;}
	bool			VerifyFlash (FlashBlockID flashBlockNumber);
	bool			SetBankSelect (BankSelect bankNumber);
	bool			SetFlashBlockIDBank(FlashBlockID blockID);
	uint32_t			ReadBankSelect ();

	std::string & GetDesignName()
	{
		size_t index = _designName.find("HW_TIMEOUT");
		if(index != std::string::npos)
		{
			_designName.erase(index - 1, _designName.length()-1);
		}
		index = _designName.find("UserID");
		if(index != std::string::npos)
		{
			_designName.erase(index - 1, _designName.length() - 1);
		}
		return _designName;
	}
	std::string & GetPartName()
	{
		return _partName;
	}
	std::string & GetDate()
	{
		return _date;
	}
	std::string & GetTime()
	{
		return _time;
	}
	uint32_t GetNumBytes()
	{
		return _numBytes;
	}
	std::string & GetMCSInfo()
	{
		return _mcsInfo;
	}

	void ParsePartitionFromFileLines(uint32_t address, uint16_t & partitionOffset);
	bool CreateBankRecord(BankSelect bankID);

	bool ProgramMACAddresses(MacAddr * mac1, MacAddr * mac2);
	bool ReadMACAddresses(MacAddr & mac1, MacAddr & mac2);
	void DisplayData(uint32_t address, uint32_t len);
	bool ProgramInfoFromString(std::string infoString);

    int32_t  NextMcsStep() {return ++_mcsStep;}

	bool ParseHeader(char* headerAddress);
	bool WaitForFlashNOTBusy();
	bool ProgramFlashValue(uint32_t address, uint32_t value);
	bool FastProgramFlash256(uint32_t address, uint32_t* buffer);
	bool EraseSector(uint32_t sectorAddress);
	bool CheckFlashErased(FlashBlockID flashBlockNumber);
	bool CheckFlashErased();
	uint32_t ReadDeviceID();
	bool SetDeviceProperties();
	void DetermingFlashTypeAndBlockNumberFromFileName(const char* bitFileName);
	void SRecordOutput (const char *pSRecord);

	uint32_t GetSectorAddressForSector(FlashBlockID flashBlockNumber,uint32_t sectorNumber)
	{
		return GetBaseAddressForProgramming(flashBlockNumber)+(sectorNumber*_sectorSize);
	}

	uint32_t GetBaseAddressForProgramming(FlashBlockID flashBlockNumber)
	{
		switch ( flashBlockNumber )
		{
		default:
		case MAIN_FLASHBLOCK:
			return _mainOffset;
			break;
		case FAILSAFE_FLASHBLOCK:
			return _failSafeOffset;
			break;
		case SOC1_FLASHBLOCK:
			return _soc1Offset;
			break;
		case SOC2_FLASHBLOCK:
			return _soc2Offset;
			break;
		case MAC_FLASHBLOCK:
			return _macOffset;
			break;
		case MCS_INFO_BLOCK:
			return _mcsInfoOffset;
			break;
		}

	}

	uint32_t GetNumberOfSectors(FlashBlockID flashBlockNumber)
	{
		switch ( flashBlockNumber )
		{
		default:
		case MAIN_FLASHBLOCK:
			return _numSectorsMain;
			break;
		case FAILSAFE_FLASHBLOCK:
			return _numSectorsFailSafe;
			break;
		case SOC1_FLASHBLOCK:
			return _numSectorsSOC1;
			break;
		case SOC2_FLASHBLOCK:
			return _numSectorsSOC2;
			break;
		case MAC_FLASHBLOCK:
			return 1;
			break;
		case MCS_INFO_BLOCK:
			return 1;
			break;
		}

	}
	bool VerifySOCPartition(FlashBlockID flashID, uint32_t FlashBlockOffset);

	uint8_t*	  _bitFileBuffer;
	uint8_t*	  _customFileBuffer;
	uint32_t		  _bitFileSize;
	std::string       _bitFileName;
	std::string       _date;
	std::string       _time;
	std::string       _designName;
	std::string       _partName;
	std::string		  _mcsInfo;
	uint32_t			  _spiDeviceID;
	uint32_t			  _flashSize;
	uint32_t			  _bankSize;
	uint32_t			  _sectorSize;
	uint32_t			  _mainOffset;
	uint32_t			  _failSafeOffset;
	uint32_t			  _macOffset;
    uint32_t            _mcsInfoOffset;
    uint32_t			  _soc1Offset;
    uint32_t			  _soc2Offset;
	uint32_t			  _numSectorsMain;
	uint32_t			  _numSectorsSOC1;
	uint32_t			  _numSectorsSOC2;
	uint32_t			  _numSectorsFailSafe;
	uint32_t			  _numBytes;
	FlashBlockID	  _flashID;
	uint32_t			  _deviceID;
	bool			  _bQuiet;
    int32_t               _mcsStep;
    CNTV2MCSfile	_mcsFile;
	std::vector<uint8_t> _partitionBuffer;

	typedef enum {
		READID_COMMAND=0x9F,
		WRITEENABLE_COMMAND=0x06,
		WRITEDISABLE_COMMAND=0x04,
		READSTATUS_COMMAND=0x05,
		WRITESTATUS_COMMAND=0x01,
		READFAST_COMMAND=0x0B,
		PAGEPROGRAM_COMMAND=0x02,
		SECTORERASE_COMMAND=0xD8,
		CHIPERASE_COMMAND=0xC7,
		BANKSELECT_COMMMAND=0x17,
		READBANKSELECT_COMMAND=0x16
	} _FLASH_STUFF ;

};


#endif
