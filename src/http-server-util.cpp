#include <MDNS.h>
using namespace mdns;
#include "http-server-util.h"


// Do we need these? probably not?
// int mgschwan_broadcastPort = 4888;
// TCPServer server = TCPServer(mgschwan_broadcastPort+1);
// TCPClient client;

TCPServer webserver = TCPServer(80);
TCPClient webclient;

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


void mgschwan_setupNetwork()
{
    // these are *probably* not necessary for serving http?
        // server.begin();
        //webSocket.begin();
        //webSocket.onEvent(webSocketEvent);
    
    // we've modified this to only set up port 80 for http
    mgschwan_setupMDNS();
}



void mgschwan_serve_webinterface() {
    int c = 0, last_c = 0, last_last_c = 0;

    webclient = webserver.available();
    bool request_finished = false;
    if (webclient.connected()) {
        String thing = "";
        while (webclient.available())
        {
            c = webclient.read();
            last_last_c = last_c;
            last_c = c;

            thing += char(c);


            /* Request finished. We are assuming it is a GET request since 
             * we are only serving a single html file
             * The browser will probably try to access the favicon as well
             * but he has to deal with it
             */
            if (c == '\n' && (c == last_c || c == last_last_c) )
            {
                request_finished = true;
            }
        }

        if (request_finished)
        {

            String content = "";
            content += "\nhello he world 2xxxxxxx ";
            content += "\nth:";
            content += thing;
            //content += "\nhello again!";

            webclient.println("HTTP/1.0 200 OK");
            webclient.println("Content-type: text/html");
            webclient.print("Content-length: ");
            webclient.println(content.length());
            //webclient.println(sizeof(content));
            //webclient.println(sizeof("\nhello hello world\n\nthing: " + thing));
            //webclient.println(sizeof(bin2c_index_html));
            
            webclient.print(content);
            webclient.println();
            
            //webclient.println("");
            //webclient.println("hello hello world\n\n");
            
            //webclient.println("thing: " + thing);
            //webclient.println("");
            //webclient.write(bin2c_index_html, sizeof(bin2c_index_html));

            webclient.println("</body>");
        }

        delay (1); //That is a hack to allow the browser to receive the data
        webclient.stop();
    }
}

void mgschwan_MDNS_loop() {
    mgschwan_mdns.processQueries();
}

