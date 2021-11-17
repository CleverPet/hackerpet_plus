#include <MDNS.h>
using namespace mdns;
#include "http-server-util.h"


// this was enabled before:
//TCPServer webserver = TCPServer(80);
//TCPClient webclient;

MDNS mgschwan_mdns;


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


bool mgschwan_setupMDNS() {
    bool success = false;

    success = mgschwan_mdns.setHostname("cleverpet");
    Log.info("MDNS: Set hostname %d",success);

    if (success) {
      success = mgschwan_mdns.addService("tcp", "http", 80, "Cleverpet Interface");
    } 

    Log.info("MDNS: Add service %d",success);

    if (success) {
        success = mgschwan_mdns.begin(true);
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

void mgschwan_setupNetwork()
{
    // these are *probably* not necessary for serving http?
        // server.begin();
        //webSocket.begin();
        //webSocket.onEvent(webSocketEvent);
    
    // we've modified this to only set up port 80 for http
    mgschwan_setupMDNS();
}


void mgschwan_MDNS_loop() {
    mgschwan_mdns.processQueries();
}

