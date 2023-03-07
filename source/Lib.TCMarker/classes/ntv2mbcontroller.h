#ifndef NTV2MBCONTROLLER_H
#define NTV2MBCONTROLLER_H

#include "ntv2card.h"
#include "ntv2mailbox.h"
#include <iostream>
#include <vector>

enum eMBCmd
{
    MB_CMD_SET_NET      = 0,
    MB_CMD_START_IGMP   = 1,
    MB_CMD_STOP_IGMP    = 2,
    MB_CMD_GET_MAC_FROM_ARP_TABLE = 3,
    MB_CMD_SEND_ARP_REQ = 4,
    MB_CMD_UNKNOWN      = 5
};

enum eSFP
{
    SFP_TOP,
    SFP_BOTTOM
};

enum eArpState
{
    ARP_ERROR,
    ARP_VALID,
    ARP_INCOMPLETE,
    ARP_NOT_FOUND
};

class AJAExport CNTV2MBController : public CNTV2MailBox
{
public:
    CNTV2MBController(CNTV2Card & device);

    // all these methods block until response received or timeout
    bool SetMBNetworkConfiguration (eSFP port, std::string ipaddr, std::string netmask,std::string gateway);
    bool JoinIGMPGroup( eSFP port, NTV2Channel channel, std::string ipaddr);
    bool LeaveIGMPGroup(eSFP port, NTV2Channel channel,std::string ipaddr);
    bool GetRemoteMAC(std::string remote_IPAddress, std::string & MACaddress);

protected:
    eArpState GetRemoteMACFromArpTable(std::string remote_IPAddress, std::string & MACaddress);
    bool SendArpRequest(std::string remote_IPAddress);

    void splitResponse(const std::string response, std::vector<std::string> & results);
    bool getDecimal(const std::string & resp, const std::string & parm, uint32_t & result);
    bool getHex(const std::string & resp, const std::string & parm, uint32_t &result);
    bool getString(const std::string & resp, const std::string & parm, std::string & result);

private:
    uint32_t getFeatures();

};

#endif // NTV2MBCONTROLLER_H
