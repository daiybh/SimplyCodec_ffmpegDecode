#ifndef CNTV2MAILBOX_H
#define CNTV2MAILBOX_H

#include "ntv2card.h"
#include "ntv2enums.h"
#include "ntv2registers2022.h"
#include <stdint.h>

#define MB_WRDATA  0x00
#define MB_RDDATA  0x08
#define MB_STATUS  0x10
#define MB_ERROR   0x14
#define MB_SIT     0x18    // send interrupt threshold
#define MB_RIT     0x18    // receive interrupt threshold
#define MB_IS      0x20    // interrupt status register
#define MB_IE      0x24    // interrupt enable register
#define MB_IP      0x28    // interrupt pending register

#define MBS_RX_EMPTY    BIT(0)
#define MBS_TX_FULL     BIT(1)
#define MBS_SEND_LT_TA  BIT(2)    // send level <= SIT
#define MBS_RCV_GT_TA   BIT(3)    // receive level > RIT

#define MBE_RX_EMPTY    BIT(1)
#define MBE_TX_FULL     BIT(2)

#define MB_RX_INT       BIT(1)

#define FIFO_SIZE       1024    // 32-bit words
#define MB_TIMEOUT      50      // milliseconds

#define SEQNUM_MIN      1
#define SEQNUM_MAX      500

class AJAExport CNTV2MailBox
{
    friend class CNTV2MBController;

public:
    CNTV2MailBox(CNTV2Card & device);
    ~CNTV2MailBox();

    bool    sendMsg(char * msg, uint32_t timeout); // returns response
    bool    sendMsg(uint32_t timeout);

    void    getError(std::string & error);
    void    getResponse(std::string & response);

    bool    WriteAddress (ULWord address, ULWord value,  ULWord mask = 0xFFFFFFFF, ULWord shift = 0x0);
    bool    ReadAddress  (const ULWord inAddress, ULWord & outValue, const ULWord inMask = 0xFFFFFFFF, const ULWord inShift = 0x0);
    inline bool    ReadAddress  (ULWord address, ULWord *pOutValue, ULWord mask = 0xFFFFFFFF, ULWord shift = 0x0)			{return pOutValue ? ReadAddress (address, *pOutValue, mask, shift) : false;}

    bool    WriteChannelAddress (ULWord address, ULWord value,  ULWord mask = 0xFFFFFFFF, ULWord shift = 0x0);
    bool    ReadChannelAddress  (const ULWord inAddress, ULWord & outValue, const ULWord inMask = 0xFFFFFFFF, const ULWord inShift = 0x0);
    inline bool    ReadChannelAddress  (ULWord address, ULWord *pOutValue, ULWord mask = 0xFFFFFFFF, ULWord shift = 0x0)	{return pOutValue ? ReadChannelAddress (address, *pOutValue, mask, shift) : false;}

    void    SetChannel(ULWord channelAddress, ULWord channelNumber, ULWord channelPS);

    bool    AcquireMailbox();
    void    ReleaseMailbox();

protected:
    bool    rcvMsg(uint32_t timeout);

    bool    writeMB(uint32_t val,  uint32_t timeout = MB_TIMEOUT);
    bool    readMB(uint32_t & val, uint32_t timeout = MB_TIMEOUT);

    bool    waitSOM(uint32_t timeout);
    bool    waitRxReady(uint32_t timeout);
    bool    waitTxReady(uint32_t timeout);

    bool    rxReady();

    uint32_t getStatus();

    CNTV2Card  &mDevice;

    std::string mError;

private:
    void        startTimer();
    uint64_t    getElapsedTime();
    int64_t     getSystemCounter();
    int64_t     getSystemFrequency();
    uint64_t    getSystemMilliseconds();


    uint32_t    nextSeqNum() {if (++_seqNum > SEQNUM_MAX) return SEQNUM_MIN; else return _seqNum;}
    uint32_t    currentSeqNum() {return _seqNum;}
    uint32_t    getFeatures();

    uint32_t    chanAddress;
    uint32_t    chanNumber;
    uint32_t    chanPS;

    uint32_t    baddr;             // base address
    uint32_t    txBuf[FIFO_SIZE+1];
    uint32_t    rxBuf[FIFO_SIZE+1];

    uint64_t    _startTime;
    uint32_t    _seqNum;
};

#endif // CNTV2MAILBOX_H
