#include "../lib/MDNS/src/MDNS.h"

using namespace mdns;
#include "http-server-util.h"


// this was enabled before:
//TCPServer webserver = TCPServer(80);
//TCPClient webclient;


IPAddress mgschwan_getBroadcastAddress() {
 
        IPAddress localIP;
        IPAddress broadcastIP;
        IPAddress netmask;

        localIP = WiFi.localIP();
        netmask = WiFi.subnetMask();

        for (int idx = 0; idx < 4; idx++) {
            broadcastIP[idx] = localIP[idx] | ~netmask[idx];
        }

        return broadcastIP;
}


bool mgschwan_setupMDNS(MDNS * mgschwan_mdns, bool begin_only) {
    bool success = false;

    if (!begin_only)
    {
        success = mgschwan_mdns->setHostname("cleverpet");
        Log.info("MDNS: Set hostname %d",success);

        if (success) {
        success = mgschwan_mdns->addService("tcp", "http", 80, "Cleverpet Interface");
        } 

        Log.info("MDNS: Add service %d",success);
    }
    if (success || begin_only) {
        success = mgschwan_mdns->begin(true);
    }

    Log.info("MDNS: Begin %d",success);

    return success;
}


String int_to_string(int the_int)
{
    char the_str[30];
    sprintf(the_str,"%d", the_int);
    String the_str2 = String(the_str);
    return the_str2;
}

String float_to_string(float the_float)
{
    char the_str[30];
    sprintf(the_str,"%f", the_float);
    String the_str2 = String(the_str);
    return the_str2;
}


void mgschwan_setupNetwork(MDNS * mgschwan_mdns, bool begin_only)
{
    // these are *probably* not necessary for serving http?
        // server.begin();
        //webSocket.begin();
        //webSocket.onEvent(webSocketEvent);
    
    // we've modified this to only set up port 80 for http
    mgschwan_setupMDNS(mgschwan_mdns, begin_only);
}


void mgschwan_MDNS_loop(MDNS * mgschwan_mdns) {
    mgschwan_mdns->processQueries();
}

