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



int mgschwan_serve_webinterface() {
    int c = 0, last_c = 0, last_last_c = 0;
    int new_game_selected = -1;

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
            // check URL for game to set, or none (keep playing current game)
            String route = thing.substring(thing.indexOf("GET") + 5);
            route = route.substring(0, route.indexOf(" "));

            String content = "";
            content += "<br>";

            // return the id from this function at the end; or, return what? -1? to indicate no new choice?
            if (route.equalsIgnoreCase("game-0"))
            {
                new_game_selected = 0;
                content += "selected game 0!<br>";
            }
            else if (route.equalsIgnoreCase("game-1"))
            {
                new_game_selected = 1;
                content += "selected game 1!<br>";                   
            }
            else if (route.equalsIgnoreCase("game-2"))
            {
                new_game_selected = 2;
                content += "selected game 2!<br>";
            }

            // print list of games and URL to go to


            //      <a href="https://www.w3schools.com">Visit W3Schools.com!</a>
            content += "<br>";
            content += "select game:<br>";
            content += "<a href=\"http://cleverpet.local/game-0\">Eating the Food</a><br>";
            content += "<a href=\"http://cleverpet.local/game-1\">Exploring the Touchpads</a><br>";
            content += "<a href=\"http://cleverpet.local/game-2\">Engaging Consistently</a><br>";

            webclient.println("HTTP/1.0 200 OK");
            webclient.println("Content-type: text/html");
            webclient.print("Content-length: ");
            webclient.println(content.length());

            webclient.print(content);
            webclient.println();
            
            //webclient.write(bin2c_index_html, sizeof(bin2c_index_html));

            webclient.println("</body>");

        }

        if (false)  // testing only!
        {
            
            // extract URL from thing
            // everything after GET and before next space

            String route = thing.substring(thing.indexOf("GET") + 5);
            route = route.substring(0, route.indexOf(" "));

            String content = "";
            content += "\n\nhello! <br>";
            content += "route: (";
            content += route;
            content += ") <br>";

            if (route.equalsIgnoreCase("set-12345"))
            {
                content += "Setting Variable to 12345 <br>";

                int addr = 10;
                uint16_t value = 12345;
                EEPROM.put(addr, value);
      
            }

            else if (route.equalsIgnoreCase("set-11"))
            {
                content += "Setting Variable to 11 <br>";

                int addr = 10;
                uint16_t value = 11;
                EEPROM.put(addr, value);
            }

            else if (route.equalsIgnoreCase("get"))
            {
                content += "Getting Variable <br>";
                uint16_t objec;
                int address = 10;
                EEPROM.get(address, objec);
                content += "value: ";
                content += String(int(objec));
                content += "<br>";
            }

            webclient.println("HTTP/1.0 200 OK");
            webclient.println("Content-type: text/html");
            webclient.print("Content-length: ");
            webclient.println(content.length());

            webclient.print(content);
            webclient.println();
            
            //webclient.write(bin2c_index_html, sizeof(bin2c_index_html));

            webclient.println("</body>");
        }

        delay (1); //That is a hack to allow the browser to receive the data
        webclient.stop();
    }

    return new_game_selected;
}

void mgschwan_MDNS_loop() {
    mgschwan_mdns.processQueries();
}

