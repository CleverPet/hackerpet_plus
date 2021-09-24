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



int mgschwan_serve_webinterface(int current_game, int next_game) {
    int c = 0, last_c = 0, last_last_c = 0;
    int new_game_selected = -1;
    
    int overrideable_next_game = next_game;

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

        if (request_finished)  //request_finished)
        {
            // this is where we will look for POST request first. Need to check if GET or POST


            bool req_get = thing.substring(0, 3).equalsIgnoreCase("GET");
            bool req_post = thing.substring(0, 4).equalsIgnoreCase("POST");

            if (req_get)
            {
                Log.info("!!! SERVER IS SERVING GET REQUEST !!!");
            }
            else if (req_post)
            {
                Log.info("!!! SERVER IS SERVING POST REQUEST !!!");
            }
            else
            {
                Log.info("!!! SERVER IS SERVING UNKNOWN REQUEST !!!");
            }

            if (req_post)
            {
                // get and process new game value
                // this is the section where we will set new_game_selected, not below
            }

            if (req_get)
            {
                // check URL for game to set, or none (keep playing current game)
                String route = thing.substring(thing.indexOf("GET") + 5);
                route = route.substring(0, route.indexOf(" "));

                String content = "";
                content += "<!DOCTYPE html>";
                content += "<html>";
                content += "<body>";

                // enable for debugging full GET request:
                //content += "<br><br>";
                //content += thing;
                //content += "<br><br>";

                // return the id from this function at the end; or, return what? -1? to indicate no new choice?

                //  TODO THIS STUFF MOVES TO POST ABOVE!!!
                if (route.equalsIgnoreCase("game-0"))
                {
                    new_game_selected = 0;
                    //content += "selected game 0!<br>";
                }
                else if (route.equalsIgnoreCase("game-1"))
                {
                    new_game_selected = 1;
                    //content += "selected game 1!<br>";                   
                }
                else if (route.equalsIgnoreCase("game-2"))
                {
                    new_game_selected = 2;
                    //content += "selected game 2!<br>";
                }

                if (new_game_selected >= 0)
                {
                    overrideable_next_game = new_game_selected;
                }
                // print list of games and URL to go to
                
                content += "<br>";
                content += "select game:<br><br>";
                if (current_game == 0)
                {
                    content += "[<b>playing</b>] <a href=\"http://cleverpet.local/game-0\">Eating the Food</a><br>";
                }
                else if (overrideable_next_game == 0)
                {
                    content += "[<b>queued </b>] <a href=\"http://cleverpet.local/game-0\">Eating the Food</a><br>";
                }
                else
                {
                    content += "[-------] <a href=\"http://cleverpet.local/game-0\">Eating the Food</a><br>";
                }

                if (current_game == 1)
                {
                    content += "[<b>playing</b>] <a href=\"http://cleverpet.local/game-1\">Exploring the Touchpads</a><br>";
                }
                else if (overrideable_next_game == 1)
                {
                    content += "[<b>queued </b>] <a href=\"http://cleverpet.local/game-1\">Exploring the Touchpads</a><br>";
                }
                else
                {
                    content += "[-------] <a href=\"http://cleverpet.local/game-1\">Exploring the Touchpads</a><br>";
                }

                if (current_game == 2)
                {
                    content += "[<b>playing</b>] <a href=\"http://cleverpet.local/game-2\">Engaging Consistently</a><br>";
                }
                else if (overrideable_next_game == 2)
                {
                    content += "[<b>queued </b>] <a href=\"http://cleverpet.local/game-2\">Engaging Consistently</a><br>";
                }
                else
                {
                    content += "[-------] <a href=\"http://cleverpet.local/game-2\">Engaging Consistently</a><br>";
                }

                content += "</body>";
                content += "</html>";
                
                webclient.println("HTTP/1.0 200 OK");
                webclient.println("Content-type: text/html");
                webclient.print("Content-length: ");
                webclient.println(content.length());
                webclient.println("");
                webclient.print(content);
                webclient.println();
            }
        }

        delay (1); //That is a hack to allow the browser to receive the data
        webclient.stop();
    }

    return new_game_selected;
}

void mgschwan_MDNS_loop() {
    mgschwan_mdns.processQueries();
}

