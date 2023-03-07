/**
    @file		ntv2configts2022.h
    @brief		Declares the CNTV2ConfigTs2022 class.
	@copyright	(C) 2014-2016 AJA Video Systems, Inc.	Proprietary and confidential information.
**/

#ifndef NTV2_2022CONFIGTS_H
#define NTV2_2022CONFIGTS_H

#include "ntv2card.h"
#include "ntv2enums.h"
#include "ntv2registers2022.h"
#include "ntv2mbcontroller.h"
#include "ntv2tshelper.h"
#include <string.h>


// Encoder part numbers
#define     ENCODE_TS_TIMER                 0
#define     ENCODE_TS_J2K_ENCODER           1
#define     ENCODE_TS_MPEG_J2K_ENCAP        2
#define     ENCODE_TS_AES_ENCAP             3
#define     ENCODE_TS_MPEG_AES_ENCAP        4
#define     ENCODE_TS_MPEG_ANC_ENCAP        6

// Decoder part numbers
#define     DECODE_TS_MPEG_J2K_DECAP        0
#define     DECODE_TS_J2K_DECODER           1
#define     DECODE_TS_MPEG_AES_DECAP        2
#define     DECODE_TS_AES_DECAP             3
#define     DECODE_TS_MPEG_ANC_DECAP        5


#define     PES_HDR_LOOKUP                  0x0
#define     PES_HDR_LEN                     0xc0
#define     PTS_OFFSET                      0xc1
#define     J2K_TS_OFFSET                   0xc2
#define     AUF1_OFFSET                     0xc3
#define     AUF2_OFFSET                     0xc4
#define     J2K_TS_LOAD                     0xd0
#define     TS_GEN_TC                       0xd1
#define     HOST_EN                         0xe0
#define     INTERLACED_VIDEO                0xe1
#define     PAYLOAD_PARAMS                  0xe2
#define     LOOPBACK                        0xe3
#define     PAT_TABLE_LOOKUP                0x100
#define     PAT_PMT_PERIOD                  0x1f0
#define     PMT_TABLE_LOOKUP                0x200
#define     ADAPTATION_LOOKUP               0x300
#define     ADAPTATION_HDR_LENGTH           0x3f0


/**
    @brief	The CNTV2ConfigTs2022 class is the interface to Kona-IP SMPTE 2022 J2K encoder and TS chips
**/

class AJAExport CNTV2ConfigTs2022 : public CNTV2MBController
{
public:
    CNTV2ConfigTs2022 (CNTV2Card & device);

    // Set/Get NTV2 video format for TS and J2K setup
    bool    SetEncodeVideoFormat(const NTV2Channel channel, const NTV2VideoFormat format);
    bool    GetEncodeVideoFormat(const NTV2Channel channel, NTV2VideoFormat & format);

    // Set/Get ULL mode for J2K setup
    bool    SetEncodeJ2KUllMode(const NTV2Channel channel, const uint32_t ull);
    bool    GetEncodeJ2KUllMode(const NTV2Channel channel, uint32_t & ull);

    // Set/Get bit depth for J2K setup
    bool    SetEncodeJ2KBitDepth(const NTV2Channel channel, const uint32_t bitDepth);
    bool    GetEncodeJ2KBitDepth(const NTV2Channel channel, uint32_t & bitDepth);

    // Set/Get chroma sub sampling for J2K setup
    bool    SetEncodeJ2KChromaSubsamp(const NTV2Channel channel, const uint32_t subSamp);
    bool    GetEncodeJ2KChromaSubsamp(const NTV2Channel channel, uint32_t & subSamp);

    // Set/Get code block size for J2K setup
    bool    SetEncodeJ2KCodeBlockSize(const NTV2Channel channel, const uint32_t codeBlockSize);
    bool    GetEncodeJ2KCodeBlockSize(const NTV2Channel channel, uint32_t & codeBlockSize);

    // Set/Get encode rate in MBPS for J2K setup
    bool    SetEncodeJ2KMbps(const NTV2Channel channel, const uint32_t mbps);
    bool    GetEncodeJ2KMbps(const NTV2Channel channel, uint32_t & mbps);

    // Set/Get encode rate in MBPS for J2K setup
    bool    SetDecodePid(const NTV2Channel channel, const uint32_t pid);
    bool    GetDecodePid(const NTV2Channel channel, uint32_t & pid);

    // Set/Get stream type for J2K setup
    bool    SetStreamType(const NTV2Channel channel, const J2KStreamType streamType);
    bool    GetStreamType(const NTV2Channel channel, J2KStreamType & streamType);

    // Setup all of the TS encode parts and the J2K Encoder
    bool    SetupForEncode();

    // Setup all of the TS decode parts and the J2K decoder
    bool    SetupForDecode();

    // Setup the J2K encoder
    bool    SetupJ2KEncoder(const NTV2Channel channel);

    // Setup the J2K decoder
    bool    SetupJ2KDecoder();

    // Setup the TS encode parts
    bool    SetupTsForEncode(const NTV2Channel channel);

    // Setup the TS decode parts
    bool    SetupTsForDecode();

    // If method returns false call this to get details
    std::string getLastError();


 private:
    // These routines are used to setup all the individual TS parts for encode and decode

    // Setup individual TS encode parts
    bool    SetupEncodeTsTimer(const NTV2Channel channel);
    bool    SetupEncodeTsJ2KEncoder(const NTV2Channel channel);
    bool    SetupEncodeTsMpegJ2kEncap(const NTV2Channel channel);
    bool    SetupEncodeTsAesEncap(const NTV2Channel channel);
    bool    SetupEncodeTsMpegAesEncap(const NTV2Channel channel);
    bool    SetupEncodeTsMpegAncEncap();

    // Setup individual TS decode parts
    bool    SetupDecodeTsMpegJ2kDecap();
    bool    SetupDecodeTsJ2KDecoder();
    bool    SetupDecodeTsMpegAesDecap();
    bool    SetupDecodeTsAesDecap();
    bool    SetupDecodeTsMpegAncDecap();
    
    // Routines to talk to the J2K part
    bool                J2kCanAcceptCmd(const NTV2Channel channel);
    void                J2kSetParam(const NTV2Channel channel, uint32_t config, uint32_t param, uint32_t value);
    void                J2kSetMode(const NTV2Channel channel, uint32_t tier, uint32_t mode);
    uint32_t            GetFeatures();

    bool                GenerateTransactionTableForMpegJ2kEncap(const NTV2Channel channel);
    uint32_t            GetIpxJ2KAddr(const NTV2Channel channel);
    uint32_t            GetIpxTsAddr(const NTV2Channel channel);

    CNTV2TsHelper       _tsHelper;
    bool                _is2022_6;
    bool                _is2022_2;

    int32_t             _transactionTable[1024][2];
    int32_t             _transactionCount;

};	//	CNTV2ConfigTs2022


struct tsSetupReg
{
    uint32_t reg;
    uint32_t value;
};

static const tsSetupReg tsAesEncapTable[] =
{
    {0x000, 0x02},
    {0x008, 0x1},
};
#define numTsAesEncapEntries (sizeof(tsAesEncapTable) / sizeof(tsSetupReg))

static const tsSetupReg tsMpegAesEncapTable[] =
{
    {0x000, 0x47},
    {0x001, 0x41},
    {0x002, 0x2},
    {0x003, 0x10},
    {0x004, 0x0},
    {0x005, 0x0},
    {0x006, 0x1},
    {0x007, 0xbd},
    {0x008, 0x0},
    {0x009, 0x0},
    {0x00a, 0x80},
    {0x00b, 0x80},
    {0x00c, 0x5},
    {0x00d, 0x21},
    {0x00e, 0x0},
    {0x00f, 0x1},
    {0x010, 0x0},
    {0x011, 0x1},
    {0x012, 0x0},
    {0x013, 0x0},
    {0x014, 0x0},
    {0x015, 0x10},

    {0x0c0, 0x16},
    {0x0c1, 0xd},
    {0x0c3, 0x1000012},
    {0x0c4, 0x1000c08},

    {0x300, 0x47},               // adaptation header
    {0x301, 0x1},
    {0x302, 0x2},
    {0x303, 0x30},
    {0x304, 0x0},
    {0x305, 0x0},

    {0x3f0, 0x6},

    {0x0e2, 0x102},
    {0x0e0, 0x1},

};
#define numTsMpegAesEncapEntries (sizeof(tsMpegAesEncapTable) / sizeof(tsSetupReg))



#endif // NTV2_2022CONFIGTS_H
