#include "ntv2mbcontroller.h"
#include <sstream>

#if defined(AJALinux)
#include <stdlib.h>
#endif

using namespace std;

CNTV2MBController::CNTV2MBController(CNTV2Card &device) : CNTV2MailBox(device)
{
}

bool CNTV2MBController::SetMBNetworkConfiguration (eSFP port, string ipaddr, string netmask, string gateway)
{
    // If no MB do nothing, this is for Cochrane
    uint32_t features = getFeatures();
    if (features & SAREK_MB_PRESENT)
    {
        sprintf((char*)txBuf,"cmd=%d,port=%d,ipaddr=%s,subnet=%s,gateway=%s",
                      (int)MB_CMD_SET_NET,(int)port,ipaddr.c_str(),netmask.c_str(),gateway.c_str());

        bool rv = sendMsg(1000);
        if (!rv)
        {
            return false;
        }

        string response;
        getResponse(response);
        vector<string> msg;
        splitResponse(response, msg);
        if (msg.size() >=1)
        {
            string status;
            rv = getString(msg[0],"status",status);
            if (rv && (status == "OK"))
            {
                return true;
            }
            else if (rv && (status == "FAIL"))
            {
                if (msg.size() >= 3)
                {
                    rv = getString(msg[2],"error",mError);
                    return false;
                }
            }
         }

         mError = "Invalid response from MB";
         return false;
    }
    else
         return true;
}

bool CNTV2MBController::JoinIGMPGroup(eSFP port, NTV2Channel channel, string ipaddr)
{
    // If no MB do nothing, this is for Cochrane
    uint32_t features = getFeatures();
    if (features & SAREK_MB_PRESENT)
    {
        sprintf((char*)txBuf,"cmd=%d,port=%d,ipaddr=%s,channel=%d",(int)MB_CMD_START_IGMP,(int)port,ipaddr.c_str(),int(channel));
        bool rv = sendMsg(250);
        if (!rv)
        {
            return false;
        }

        string response;
        getResponse(response);
        vector<string> msg;
        splitResponse(response, msg);
        if (msg.size() >=1)
        {
            string status;
            rv = getString(msg[0],"status",status);
            if (rv && (status == "OK"))
            {
                return true;
            }
            else if (rv && (status == "FAIL"))
            {
                if (msg.size() >= 3)
                {
                    rv = getString(msg[2],"error",mError);
                    return false;
                }
            }
        }

        mError = "Invalid response from MB";
        return false;
    }
    else
        return true;
}

bool CNTV2MBController::LeaveIGMPGroup(eSFP port, NTV2Channel channel, string ipaddr)
{
    // If no MB do nothing, this is for Cochrane
    uint32_t features = getFeatures();
    if (features & SAREK_MB_PRESENT)
    {
        sprintf((char*)txBuf,"cmd=%d,port=%d,ipaddr=%s,channel=%d",(int)MB_CMD_STOP_IGMP,(int)port,ipaddr.c_str(),(int)channel);
        bool rv = sendMsg(250);
        if (!rv)
        {
            return false;
        }

        string response;
        getResponse(response);
        vector<string> msg;
        splitResponse(response, msg);
        if (msg.size() >=1)
        {
            string status;
            rv = getString(msg[0],"status",status);
            if (rv && (status == "OK"))
            {
                return true;
            }
            else if (rv && (status == "FAIL"))
            {
                if (msg.size() >= 3)
                {
                    rv = getString(msg[2],"error",mError);
                    return false;
                }
            }
        }

        mError = "Invalid response from MB";
        return false;
    }
    else
        return true;
}

bool CNTV2MBController::GetRemoteMAC(std::string remote_IPAddress, string & MACaddress)
{
    // If no MB do nothing, this is for Cochrane
    uint32_t features = getFeatures();
    if (features & SAREK_MB_PRESENT)
    {
        eArpState as = GetRemoteMACFromArpTable(remote_IPAddress,MACaddress);
        switch (as)
        {
        case ARP_VALID:
            return true;
        case ARP_ERROR:
            return false;
            break;
        default:
            break;
        }

        int count = 60;
        do
        {
            SendArpRequest(remote_IPAddress);
            mDevice.WaitForOutputVerticalInterrupt();
            as = GetRemoteMACFromArpTable(remote_IPAddress,MACaddress);
            switch (as)
            {
            case ARP_VALID:
                return true;
            case ARP_ERROR:
                return false;
                break;
            case ARP_INCOMPLETE:
                return false;
            default:
            case ARP_NOT_FOUND:
               break;
            }

        } while (--count);

        return false;
    }
    else
        return true;
}

eArpState CNTV2MBController::GetRemoteMACFromArpTable(std::string remote_IPAddress, string & MACaddress)
{
    // If no MB do nothing, this is for Cochrane
    uint32_t features = getFeatures();
    if (features & SAREK_MB_PRESENT)
    {
        sprintf((char*)txBuf,"cmd=%d,ipaddr=%s",(int)MB_CMD_GET_MAC_FROM_ARP_TABLE,remote_IPAddress.c_str());
        bool rv = sendMsg(250);
        if (!rv)
        {
            return ARP_ERROR;
        }

        string response;
        getResponse(response);
        vector<string> msg;
        splitResponse(response, msg);
        if (msg.size() >=1)
        {
            string status;
            rv = getString(msg[0],"status",status);
            if (rv && (status == "OK"))
            {
                if (msg.size() != 3)
                {
                    mError = "Invalid response size from MB";
                    return ARP_ERROR;
                }

                rv = getString(msg[2],"MAC",MACaddress);
                if (rv == false)
                {
                    mError = "MAC Address not found in response from MB";
                    return ARP_ERROR;
                }
                return ARP_VALID;
            }
            else if (rv && (status == "FAIL"))
            {
                if (msg.size() >= 4)
                {
                    uint32_t state;
                    rv = getString(msg[2],"error",mError);
                    rv = getDecimal(msg[3],"state",state);
                    return (eArpState)state;
                }
            }
         }

         mError = "Invalid response from MB";
         return ARP_ERROR;
    }
    else
         return ARP_VALID;
}

bool CNTV2MBController::SendArpRequest(std::string remote_IPAddress)
{
    // If no MB do nothing, this is for Cochrane
    uint32_t features = getFeatures();
    if (features & SAREK_MB_PRESENT)
    {
        sprintf((char*)txBuf,"cmd=%d,ipaddr=%s",(int)MB_CMD_SEND_ARP_REQ,remote_IPAddress.c_str());
        bool rv = sendMsg(250);
        if (!rv)
        {
            return ARP_ERROR;
        }

        string response;
        getResponse(response);
        vector<string> msg;
        splitResponse(response, msg);
        if (msg.size() >=1)
        {
            string status;
            rv = getString(msg[0],"status",status);
            if (rv && (status == "OK"))
            {
                if (msg.size() != 2)
                {
                    mError = "Invalid response size from MB";
                    return false;
                }
                return true;
            }
            else if (rv && (status == "FAIL"))
            {
                if (msg.size() >= 4)
                {
                    rv = getString(msg[2],"error",mError);
                    return false;
                }
            }
        }

        mError = "Invalid response from MB";
        return false;
    }
    else
        return true;
}

void CNTV2MBController::splitResponse(std::string response, std::vector<std::string> & results)
{
    std::istringstream ss(response);
    std::string token;

    while(std::getline(ss, token, ','))
    {
        results.push_back(token);
    }
}

bool CNTV2MBController::getDecimal(const std::string & resp, const std::string & parm, uint32_t & result)
{
    string val;
    bool rv = getString(resp,parm,val);
    if (rv)
    {
        result = atoi(val.c_str());
        return true;
    }
    return false;
}

bool CNTV2MBController::getHex(const std::string & resp, const std::string & parm, uint32_t & result)
{
    string val;
    bool rv = getString(resp,parm,val);
    if (rv)
    {
        result = strtoul(val.c_str(),NULL,16);
        return true;
    }
    return false;
}

bool CNTV2MBController::getString(const std::string & resp, const std::string & parm, std::string & result)
{
    string match = parm + "=";

    std::string::size_type i = resp.find(match);

    if (i != std::string::npos && i == 0)
    {
        result = resp;
        result.erase(i, match.length());
        return true;
    }
    return false;   // not found
}

uint32_t CNTV2MBController::getFeatures()
{
    uint32_t val;
    ReadAddress((SAREK_REGS + kRegSarekFwCfg)*4, &val);
    return val;
}

