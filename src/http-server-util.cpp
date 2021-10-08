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



String get_css_string()
{
    String css_str = ".inline {"
                     "display: inline;"
                     "}"
                     ""
                    ".link-button {"
                    "background: none;"
                    "border: none;"
                    "color: blue;"
                    "text-decoration: underline;"
                    "cursor: pointer;"
                    "font-size: 1em;"
                    "font-family: serif;"
                    "}"
                    ".link-button:focus {"
                    "outline: none;"
                    "}"
                    ".link-button:active {"
                    "color:red;"
                    "}";

    return css_str;

}

String get_post_link_string(String text, String name, String value)
{
    String post_link_str = "<form method=\"post\" action=\"http://cleverpet.local\" class=\"inline\">"
                           "<input type=\"hidden\" name=\"" + name + "\" value=\"" + value + "\">"
                           "<button type=\"submit\" name=\"submit_param\" value=\"submit_value\" class=\"link-button\">" + text + ""
                           "</button>"
                           "</form>";
    return post_link_str;
}

String get_link_for_game(int game_index, String game_index_str, String game_name, int current_game, int overrideable_next_game)
{
    String content_str;
    
    // get_link_for_game(1, "1", "Exploring the Touchpads", current_game, overrideable_next_game);
    
    if (current_game == game_index)
    {
        content_str = "[<b>playing</b>] " + get_post_link_string(game_name, "game", game_index_str) + "<br>";
    }
    else if (overrideable_next_game == game_index)
    {
        content_str = "[<b>queued</b>] " + get_post_link_string(game_name, "game", game_index_str) + "<br>";
    }
    else
    {
        content_str = "[-------] " + get_post_link_string(game_name, "game", game_index_str) + "<br>";
    }

    return content_str;
}

String get_script_html()
{
    String content_str = "<script>"
                         "function updateAllEvents() { \n"
                         "  var xhttp = new XMLHttpRequest();\n"
                         "  var status_element = document.getElementById(\"api-hub-status\");\n"
                         "  xhttp.open(\"GET\", \"http://cleverpet.local/local-api\", true);\n"
                         "  xhttp.send();\n"
                         "  xhttp.onload = function () {"
                         "      console.log('DONE', xhttp.readyState);" // readyState will be 4
                         "      console.log(xhttp.responseText);"
                         "      var data = JSON.parse(xhttp.responseText);"
                         "      status_element.innerHTML = data.status;\n"
                         "  };"
                         "}\n"
                         "\n"
                         "setInterval(updateAllEvents, 2000);\n"
                         "</script>";
    return content_str;
}

String get_async_html()
{
    String content_str = "<div class=\"event\" id=\"temp-event\">"
                         "HUB STATUS:<BR>"
                         "<strong class=\"api-msg\" id=\"api-hub-status\">init-status</strong><br />"
                         "</div>";

    return content_str;
}

int mgschwan_serve_webinterface(int current_game, int next_game, String display_error_msg) {
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
            
            // detect if this is an API request
            // look for: /local-api
            if (thing.indexOf("local-api") > -1)
            {
                // this is an API request (asynchronous webpage update)
                Log.info("!!! SERVER IS SERVING API REQUEST !!!");
                Log.info("API request string:");
                Log.print(thing);

                String return_str = "{\"status\":\"" + display_error_msg + "\"}";
                webclient.println(return_str);  // println?
            }
            else
            {
                // this is a regular POST or GET request
                
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
                    
                    //Log.info("POST request string:");
                    //Log.print(thing);
                    
                    String new_game_str = thing.substring(thing.indexOf("game=") + 5, thing.indexOf("game=") + 6);
                    String new_game_str_2 = thing.substring(thing.indexOf("game=") + 5, thing.indexOf("game=") + 7);
                    
                    if (new_game_str_2.equalsIgnoreCase("10"))
                    {
                        new_game_selected = 10;
                        Log.info("POST: selected game 10!");
                    }
                    else if (new_game_str_2.equalsIgnoreCase("11"))
                    {
                        new_game_selected = 11;
                        Log.info("POST: selected game 11!");
                    }
                    else if (new_game_str.equalsIgnoreCase("0"))
                    {
                        new_game_selected = 0;
                        Log.info("POST: selected game 0!");
                    }
                    else if (new_game_str.equalsIgnoreCase("1"))
                    {
                        new_game_selected = 1;
                        Log.info("POST: selected game 1!");          
                    }
                    else if (new_game_str.equalsIgnoreCase("2"))
                    {
                        new_game_selected = 2;
                        Log.info("POST: selected game 2!");
                    }
                    else if (new_game_str.equalsIgnoreCase("3"))
                    {
                        new_game_selected = 3;
                        Log.info("POST: selected game 3!");
                    }
                    else if (new_game_str.equalsIgnoreCase("4"))
                    {
                        new_game_selected = 4;
                        Log.info("POST: selected game 4!");
                    }
                    else if (new_game_str.equalsIgnoreCase("5"))
                    {
                        new_game_selected = 5;
                        Log.info("POST: selected game 5!");
                    }
                    else if (new_game_str.equalsIgnoreCase("6"))
                    {
                        new_game_selected = 6;
                        Log.info("POST: selected game 6!");
                    }
                    else if (new_game_str.equalsIgnoreCase("7"))
                    {
                        new_game_selected = 7;
                        Log.info("POST: selected game 7!");
                    }
                    else if (new_game_str.equalsIgnoreCase("8"))
                    {
                        new_game_selected = 8;
                        Log.info("POST: selected game 8!");
                    }
                    else if (new_game_str.equalsIgnoreCase("9"))
                    {
                        new_game_selected = 9;
                        Log.info("POST: selected game 9!");
                    }


                    if (new_game_selected >= 0)
                    {
                        overrideable_next_game = new_game_selected;
                    }
                    
                
                }

                if (req_get || req_post)
                {
                    // check URL for game to set, or none (keep playing current game)
                    //String route = thing.substring(thing.indexOf("GET") + 5);
                    //route = route.substring(0, route.indexOf(" "));

                    String content = "";
                    content += "<!DOCTYPE html>";
                    content += "<html>";
                    content += "<head><style>";
                    content += get_css_string();
                    content += "</style>";
                    content += get_script_html();
                    content += "</head>";
                    content += "<body>";

                    // enable for debugging full GET request:
                    //content += "<br><br>";
                    //content += thing;
                    //content += "<br><br>";

                    // return the id from this function at the end; or, return what? -1? to indicate no new choice?

                    // print list of games and URL to go to
                    
                    content += "<br>";
                    content += "select game:<br><br>";

                    content += get_link_for_game(0, "0", "Eating the Food", current_game, overrideable_next_game);
                    content += get_link_for_game(1, "1", "Exploring the Touchpads", current_game, overrideable_next_game);
                    content += get_link_for_game(2, "2", "Engaging Consistently", current_game, overrideable_next_game);
                    content += get_link_for_game(3, "3", "Avoiding Unlit Touchpads", current_game, overrideable_next_game);
                    content += get_link_for_game(4, "4", "Learning the Lights", current_game, overrideable_next_game);
                    content += get_link_for_game(5, "5", "Mastering the Lights", current_game, overrideable_next_game);
                    content += get_link_for_game(6, "6", "Responding Quickly", current_game, overrideable_next_game);
                    content += get_link_for_game(7, "7", "Learning Brightness", current_game, overrideable_next_game);
                    content += get_link_for_game(8, "8", "Learning Double Sequences", current_game, overrideable_next_game);
                    content += get_link_for_game(9, "9", "Learning Longer Sequences", current_game, overrideable_next_game);
                    content += get_link_for_game(10, "10", "Matching Two Colors", current_game, overrideable_next_game);
                    content += get_link_for_game(11, "11", "Matching More Colors", current_game, overrideable_next_game);

                    content += "<br>";
                    //content += display_error_msg;
                    //content += "<br>";
                    content += get_async_html();
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
        }

        delay (1); //That is a hack to allow the browser to receive the data
        webclient.stop();
    }

    return new_game_selected;
}

void mgschwan_MDNS_loop() {
    mgschwan_mdns.processQueries();
}

