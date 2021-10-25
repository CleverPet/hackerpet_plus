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

String get_selected_option(float time_zone_offset, float compare)
{
    if (time_zone_offset == compare)
    {
        return " selected";
    }
    else
    {
        return "";
    }
}

String get_time_zone_string(float time_zone_offset)
{

    String time_zone_string = 
    "<form method=\"post\" action=\"http://cleverpet.local\">\n"
    "Select Timezone: <select name=\"select_timezone\" onchange=\"this.form.submit()\">\n"
	"<option timeZoneIndex=\"1\" gmtAdjustment=\"GMT-12:00\" useDaylightTime=\"0\" value=\"-12\"" + get_selected_option(time_zone_offset, -12) + ">(GMT-12:00) International Date Line West</option>\n"
	"<option timeZoneIndex=\"2\" gmtAdjustment=\"GMT-11:00\" useDaylightTime=\"0\" value=\"-11\"" + get_selected_option(time_zone_offset, -11) + ">(GMT-11:00) Midway Island, Samoa</option>\n"
	"<option timeZoneIndex=\"3\" gmtAdjustment=\"GMT-10:00\" useDaylightTime=\"0\" value=\"-10\"" + get_selected_option(time_zone_offset, -10) + ">(GMT-10:00) Hawaii</option>\n"
	"<option timeZoneIndex=\"4\" gmtAdjustment=\"GMT-09:00\" useDaylightTime=\"1\" value=\"-9\"" + get_selected_option(time_zone_offset, -9) + ">(GMT-09:00) Alaska</option>\n"
	"<option timeZoneIndex=\"5\" gmtAdjustment=\"GMT-08:00\" useDaylightTime=\"1\" value=\"-8\"" + get_selected_option(time_zone_offset, -8) + ">(GMT-08:00) Pacific Time (US & Canada)</option>\n"
	//"<option timeZoneIndex=\"6\" gmtAdjustment=\"GMT-08:00\" useDaylightTime=\"1\" value=\"-8\">(GMT-08:00) Tijuana, Baja California</option>\n"
	//"<option timeZoneIndex=\"7\" gmtAdjustment=\"GMT-07:00\" useDaylightTime=\"0\" value=\"-7\">(GMT-07:00) Arizona</option>\n"
	//"<option timeZoneIndex=\"8\" gmtAdjustment=\"GMT-07:00\" useDaylightTime=\"1\" value=\"-7\">(GMT-07:00) Chihuahua, La Paz, Mazatlan</option>\n"
	"<option timeZoneIndex=\"9\" gmtAdjustment=\"GMT-07:00\" useDaylightTime=\"1\" value=\"-7\"" + get_selected_option(time_zone_offset, -7) + ">(GMT-07:00) Mountain Time (US & Canada)</option>\n"
	//"<option timeZoneIndex=\"10\" gmtAdjustment=\"GMT-06:00\" useDaylightTime=\"0\" value=\"-6\">(GMT-06:00) Central America</option>\n"
	"<option timeZoneIndex=\"11\" gmtAdjustment=\"GMT-06:00\" useDaylightTime=\"1\" value=\"-6\"" + get_selected_option(time_zone_offset, -6) + ">(GMT-06:00) Central Time (US & Canada)</option>\n"
	//"<option timeZoneIndex=\"12\" gmtAdjustment=\"GMT-06:00\" useDaylightTime=\"1\" value=\"-6\">(GMT-06:00) Guadalajara, Mexico City, Monterrey</option>\n"
	//"<option timeZoneIndex=\"13\" gmtAdjustment=\"GMT-06:00\" useDaylightTime=\"0\" value=\"-6\">(GMT-06:00) Saskatchewan</option>\n"
	//"<option timeZoneIndex=\"14\" gmtAdjustment=\"GMT-05:00\" useDaylightTime=\"0\" value=\"-5\">(GMT-05:00) Bogota, Lima, Quito, Rio Branco</option>\n"
	"<option timeZoneIndex=\"15\" gmtAdjustment=\"GMT-05:00\" useDaylightTime=\"1\" value=\"-5\"" + get_selected_option(time_zone_offset, -5) + ">(GMT-05:00) Eastern Time (US & Canada)</option>\n"
	//"<option timeZoneIndex=\"16\" gmtAdjustment=\"GMT-05:00\" useDaylightTime=\"1\" value=\"-5\">(GMT-05:00) Indiana (East)</option>\n"
	"<option timeZoneIndex=\"17\" gmtAdjustment=\"GMT-04:00\" useDaylightTime=\"1\" value=\"-4\"" + get_selected_option(time_zone_offset, -4) + ">(GMT-04:00) Atlantic Time (Canada)</option>\n"
	//"<option timeZoneIndex=\"18\" gmtAdjustment=\"GMT-04:00\" useDaylightTime=\"0\" value=\"-4\">(GMT-04:00) Caracas, La Paz</option>\n"
	//"<option timeZoneIndex=\"19\" gmtAdjustment=\"GMT-04:00\" useDaylightTime=\"0\" value=\"-4\">(GMT-04:00) Manaus</option>\n"
	//"<option timeZoneIndex=\"20\" gmtAdjustment=\"GMT-04:00\" useDaylightTime=\"1\" value=\"-4\">(GMT-04:00) Santiago</option>\n"
	"<option timeZoneIndex=\"21\" gmtAdjustment=\"GMT-03:30\" useDaylightTime=\"1\" value=\"-3.5\"" + get_selected_option(time_zone_offset, -3.5) + ">(GMT-03:30) Newfoundland</option>\n"
	//"<option timeZoneIndex=\"22\" gmtAdjustment=\"GMT-03:00\" useDaylightTime=\"1\" value=\"-3\">(GMT-03:00) Brasilia</option>\n"
	"<option timeZoneIndex=\"23\" gmtAdjustment=\"GMT-03:00\" useDaylightTime=\"0\" value=\"-3\"" + get_selected_option(time_zone_offset, -3) + ">(GMT-03:00) Buenos Aires, Georgetown</option>\n"
	//"<option timeZoneIndex=\"24\" gmtAdjustment=\"GMT-03:00\" useDaylightTime=\"1\" value=\"-3\">(GMT-03:00) Greenland</option>\n"
	//"<option timeZoneIndex=\"25\" gmtAdjustment=\"GMT-03:00\" useDaylightTime=\"1\" value=\"-3\">(GMT-03:00) Montevideo</option>\n"
	"<option timeZoneIndex=\"26\" gmtAdjustment=\"GMT-02:00\" useDaylightTime=\"1\" value=\"-2\"" + get_selected_option(time_zone_offset, -2) + ">(GMT-02:00) Mid-Atlantic</option>\n"
	"<option timeZoneIndex=\"27\" gmtAdjustment=\"GMT-01:00\" useDaylightTime=\"0\" value=\"-1\"" + get_selected_option(time_zone_offset, -1) + ">(GMT-01:00) Cape Verde Is.</option>\n"
	//"<option timeZoneIndex=\"28\" gmtAdjustment=\"GMT-01:00\" useDaylightTime=\"1\" value=\"-1\">(GMT-01:00) Azores</option>\n"
	//"<option timeZoneIndex=\"29\" gmtAdjustment=\"GMT+00:00\" useDaylightTime=\"0\" value=\"0\">(GMT+00:00) Casablanca, Monrovia, Reykjavik</option>\n"
	"<option timeZoneIndex=\"30\" gmtAdjustment=\"GMT+00:00\" useDaylightTime=\"1\" value=\"0\"" + get_selected_option(time_zone_offset, 0) + ">(GMT+00:00) Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London</option>\n"
	"<option timeZoneIndex=\"31\" gmtAdjustment=\"GMT+01:00\" useDaylightTime=\"1\" value=\"1\"" + get_selected_option(time_zone_offset, 1) + ">(GMT+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna</option>\n"
	//"<option timeZoneIndex=\"32\" gmtAdjustment=\"GMT+01:00\" useDaylightTime=\"1\" value=\"1\">(GMT+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague</option>\n"
	//"<option timeZoneIndex=\"33\" gmtAdjustment=\"GMT+01:00\" useDaylightTime=\"1\" value=\"1\">(GMT+01:00) Brussels, Copenhagen, Madrid, Paris</option>\n"
	//"<option timeZoneIndex=\"34\" gmtAdjustment=\"GMT+01:00\" useDaylightTime=\"1\" value=\"1\">(GMT+01:00) Sarajevo, Skopje, Warsaw, Zagreb</option>\n"
	//"<option timeZoneIndex=\"35\" gmtAdjustment=\"GMT+01:00\" useDaylightTime=\"1\" value=\"1\">(GMT+01:00) West Central Africa</option>\n"
	//"<option timeZoneIndex=\"36\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Amman</option>\n"
	"<option timeZoneIndex=\"37\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\"" + get_selected_option(time_zone_offset, 2) + ">(GMT+02:00) Athens, Bucharest, Istanbul</option>\n"
	//"<option timeZoneIndex=\"38\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Beirut</option>\n"
	//"<option timeZoneIndex=\"39\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Cairo</option>\n"
	//"<option timeZoneIndex=\"40\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"0\" value=\"2\">(GMT+02:00) Harare, Pretoria</option>\n"
	//"<option timeZoneIndex=\"41\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius</option>\n"
	//"<option timeZoneIndex=\"42\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Jerusalem</option>\n"
	//"<option timeZoneIndex=\"43\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Minsk</option>\n"
	//"<option timeZoneIndex=\"44\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\">(GMT+02:00) Windhoek</option>\n"
	//"<option timeZoneIndex=\"45\" gmtAdjustment=\"GMT+03:00\" useDaylightTime=\"0\" value=\"3\">(GMT+03:00) Kuwait, Riyadh, Baghdad</option>\n"
	"<option timeZoneIndex=\"46\" gmtAdjustment=\"GMT+03:00\" useDaylightTime=\"1\" value=\"3\"" + get_selected_option(time_zone_offset, 3) + ">(GMT+03:00) Moscow, St. Petersburg, Volgograd</option>\n"
	//"<option timeZoneIndex=\"47\" gmtAdjustment=\"GMT+03:00\" useDaylightTime=\"0\" value=\"3\">(GMT+03:00) Nairobi</option>\n"
	//"<option timeZoneIndex=\"48\" gmtAdjustment=\"GMT+03:00\" useDaylightTime=\"0\" value=\"3\">(GMT+03:00) Tbilisi</option>\n"
	"<option timeZoneIndex=\"49\" gmtAdjustment=\"GMT+03:30\" useDaylightTime=\"1\" value=\"3.5\"" + get_selected_option(time_zone_offset, 3.5) + ">(GMT+03:30) Tehran</option>\n"
	"<option timeZoneIndex=\"50\" gmtAdjustment=\"GMT+04:00\" useDaylightTime=\"0\" value=\"4\"" + get_selected_option(time_zone_offset, 4) + ">(GMT+04:00) Abu Dhabi, Muscat</option>\n"
	//"<option timeZoneIndex=\"51\" gmtAdjustment=\"GMT+04:00\" useDaylightTime=\"1\" value=\"4\">(GMT+04:00) Baku</option>\n"
	//"<option timeZoneIndex=\"52\" gmtAdjustment=\"GMT+04:00\" useDaylightTime=\"1\" value=\"4\">(GMT+04:00) Yerevan</option>\n"
	"<option timeZoneIndex=\"53\" gmtAdjustment=\"GMT+04:30\" useDaylightTime=\"0\" value=\"4.5\"" + get_selected_option(time_zone_offset, 4.5) + ">(GMT+04:30) Kabul</option>\n"
	//"<option timeZoneIndex=\"54\" gmtAdjustment=\"GMT+05:00\" useDaylightTime=\"1\" value=\"5\">(GMT+05:00) Yekaterinburg</option>\n"
	"<option timeZoneIndex=\"55\" gmtAdjustment=\"GMT+05:00\" useDaylightTime=\"0\" value=\"5\"" + get_selected_option(time_zone_offset, 5) + ">(GMT+05:00) Islamabad, Karachi, Tashkent</option>\n"
	//"<option timeZoneIndex=\"56\" gmtAdjustment=\"GMT+05:30\" useDaylightTime=\"0\" value=\"5.5\">(GMT+05:30) Sri Jayawardenapura</option>\n"
	"<option timeZoneIndex=\"57\" gmtAdjustment=\"GMT+05:30\" useDaylightTime=\"0\" value=\"5.5\"" + get_selected_option(time_zone_offset, 5.5) + ">(GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi</option>\n"
	"<option timeZoneIndex=\"58\" gmtAdjustment=\"GMT+05:45\" useDaylightTime=\"0\" value=\"5.75\"" + get_selected_option(time_zone_offset, 5.75) + ">(GMT+05:45) Kathmandu</option>\n"
	"<option timeZoneIndex=\"59\" gmtAdjustment=\"GMT+06:00\" useDaylightTime=\"1\" value=\"6\"" + get_selected_option(time_zone_offset, 6) + ">(GMT+06:00) Almaty, Novosibirsk</option>\n"
	//"<option timeZoneIndex=\"60\" gmtAdjustment=\"GMT+06:00\" useDaylightTime=\"0\" value=\"6\">(GMT+06:00) Astana, Dhaka</option>\n"
	"<option timeZoneIndex=\"61\" gmtAdjustment=\"GMT+06:30\" useDaylightTime=\"0\" value=\"6.5\"" + get_selected_option(time_zone_offset, 6.5) + ">(GMT+06:30) Yangon (Rangoon)</option>\n"
	"<option timeZoneIndex=\"62\" gmtAdjustment=\"GMT+07:00\" useDaylightTime=\"0\" value=\"7\"" + get_selected_option(time_zone_offset, 7) + ">(GMT+07:00) Bangkok, Hanoi, Jakarta</option>\n"
	//"<option timeZoneIndex=\"63\" gmtAdjustment=\"GMT+07:00\" useDaylightTime=\"1\" value=\"7\">(GMT+07:00) Krasnoyarsk</option>\n"
	"<option timeZoneIndex=\"64\" gmtAdjustment=\"GMT+08:00\" useDaylightTime=\"0\" value=\"8\"" + get_selected_option(time_zone_offset, 8) + ">(GMT+08:00) Beijing, Chongqing, Hong Kong, Urumqi</option>\n"
	//"<option timeZoneIndex=\"65\" gmtAdjustment=\"GMT+08:00\" useDaylightTime=\"0\" value=\"8\">(GMT+08:00) Kuala Lumpur, Singapore</option>\n"
	//"<option timeZoneIndex=\"66\" gmtAdjustment=\"GMT+08:00\" useDaylightTime=\"0\" value=\"8\">(GMT+08:00) Irkutsk, Ulaan Bataar</option>\n"
	//"<option timeZoneIndex=\"67\" gmtAdjustment=\"GMT+08:00\" useDaylightTime=\"0\" value=\"8\">(GMT+08:00) Perth</option>\n"
	//"<option timeZoneIndex=\"68\" gmtAdjustment=\"GMT+08:00\" useDaylightTime=\"0\" value=\"8\">(GMT+08:00) Taipei</option>\n"
	"<option timeZoneIndex=\"69\" gmtAdjustment=\"GMT+09:00\" useDaylightTime=\"0\" value=\"9\"" + get_selected_option(time_zone_offset, 9) + ">(GMT+09:00) Osaka, Sapporo, Tokyo</option>\n"
	//"<option timeZoneIndex=\"70\" gmtAdjustment=\"GMT+09:00\" useDaylightTime=\"0\" value=\"9\">(GMT+09:00) Seoul</option>\n"
	//"<option timeZoneIndex=\"71\" gmtAdjustment=\"GMT+09:00\" useDaylightTime=\"1\" value=\"9\">(GMT+09:00) Yakutsk</option>\n"
	"<option timeZoneIndex=\"72\" gmtAdjustment=\"GMT+09:30\" useDaylightTime=\"0\" value=\"9.5\"" + get_selected_option(time_zone_offset, 9.5) + ">(GMT+09:30) Adelaide</option>\n"
	//"<option timeZoneIndex=\"73\" gmtAdjustment=\"GMT+09:30\" useDaylightTime=\"0\" value=\"9.5\">(GMT+09:30) Darwin</option>\n"
	//"<option timeZoneIndex=\"74\" gmtAdjustment=\"GMT+10:00\" useDaylightTime=\"0\" value=\"10\">(GMT+10:00) Brisbane</option>\n"
	"<option timeZoneIndex=\"75\" gmtAdjustment=\"GMT+10:00\" useDaylightTime=\"1\" value=\"10\"" + get_selected_option(time_zone_offset, 10) + ">(GMT+10:00) Canberra, Melbourne, Sydney</option>\n"
	//"<option timeZoneIndex=\"76\" gmtAdjustment=\"GMT+10:00\" useDaylightTime=\"1\" value=\"10\">(GMT+10:00) Hobart</option>\n"
	//"<option timeZoneIndex=\"77\" gmtAdjustment=\"GMT+10:00\" useDaylightTime=\"0\" value=\"10\">(GMT+10:00) Guam, Port Moresby</option>\n"
	//"<option timeZoneIndex=\"78\" gmtAdjustment=\"GMT+10:00\" useDaylightTime=\"1\" value=\"10\">(GMT+10:00) Vladivostok</option>\n"
	"<option timeZoneIndex=\"79\" gmtAdjustment=\"GMT+11:00\" useDaylightTime=\"1\" value=\"11\"" + get_selected_option(time_zone_offset, 11) + ">(GMT+11:00) Magadan, Solomon Is., New Caledonia</option>\n"
	"<option timeZoneIndex=\"80\" gmtAdjustment=\"GMT+12:00\" useDaylightTime=\"1\" value=\"12\"" + get_selected_option(time_zone_offset, 12) + ">(GMT+12:00) Auckland, Wellington</option>\n"
	//"<option timeZoneIndex=\"81\" gmtAdjustment=\"GMT+12:00\" useDaylightTime=\"0\" value=\"12\">(GMT+12:00) Fiji, Kamchatka, Marshall Is.</option>\n"
	"<option timeZoneIndex=\"82\" gmtAdjustment=\"GMT+13:00\" useDaylightTime=\"0\" value=\"13\"" + get_selected_option(time_zone_offset, 13) + ">(GMT+13:00) Nuku'alofa</option>\n"
    "</select><br>\n"
    "</form>\n";				

    return time_zone_string;
}


String get_css_string()
{
    String css_str = ".inline {\n"
                     "display: inline;\n"
                     "}\n"
                     "\n"
                    ".link-button {\n"
                    "background: none;\n"
                    "border: none;\n"
                    "color: blue;\n"
                    "text-decoration: underline;\n"
                    "cursor: pointer;\n"
                    "font-size: 1em;\n"
                    "font-family: serif;\n"
                    "}\n"
                    ".link-button:focus {\n"
                    "outline: none;\n"
                    "}\n"
                    ".link-button:active {\n"
                    "color:red;\n"
                    "}\n";

    return css_str;

}

 
String get_post_link_string(String text, String name, String value)
{
    String post_link_str = "<form method=\"post\" action=\"http://cleverpet.local\" class=\"inline\">\n"
                           "<input type=\"hidden\" name=\"" + name + "\" value=\"" + value + "\">\n"
                           "<button type=\"submit\" name=\"submit_param\" value=\"submit_value\" class=\"link-button\">" + text + ""
                           "</button>\n"
                           "</form>\n";
    return post_link_str;
}

String get_link_for_game(int game_index, String game_index_str, String game_name, int current_game, int overrideable_next_game)
{

    /*
    String content_str = "<div class=\"event\" id=\"temp-event\">"
                         "HUB STATUS:<BR>"
                         "<strong class=\"api-msg\" id=\"api-hub-status\">init-status</strong><br />"
                         "</div>";
    */
    
    // get_link_for_game(1, "1", "Exploring the Touchpads", current_game, overrideable_next_game);
    String placeholder;  // on reload or post to page

    if(current_game == game_index)
    {
        placeholder = "playing";
    }
    else if (overrideable_next_game == game_index)
    {
        placeholder = "queued";
    }
    else
    {
        placeholder = "-------";
    }

    String content_str = "<div class=\"event\" id=\"temp-game-status-" + game_index_str + "\">\n"
                         "[<b><strong class=\"api-msg\" id=\"game-status-" + game_index_str + "\">" + placeholder + "</strong></b>] " + get_post_link_string(game_name, "game", game_index_str) + "<br />\n"
                         "</div>\n";
    // old:
    /*    
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
    */

    return content_str;
}

String get_script_html()
{

    // now have also: game-id-queued, game-id-playing

    String num_games = "12";
    
    String content_str = "<script>\n"
                         "function updateAllEvents() { \n"
                         "  var xhttp = new XMLHttpRequest();\n"
                         "  var status_element = document.getElementById(\"api-hub-status\");\n"
                         "  var time_element = document.getElementById(\"api-time\");\n"
                         "  xhttp.open(\"GET\", \"http://cleverpet.local/local-api\", true);\n"
                         "  xhttp.send();\n"
                         "  xhttp.onload = function () {\n"
                         "      console.log('DONE', xhttp.readyState);\n" // readyState will be 4
                         "      console.log(xhttp.responseText);\n"
                         "      var data = JSON.parse(xhttp.responseText);\n"
                         "      status_element.innerHTML = data.status;\n"
                         "      time_element.innerHTML = data.time;\n"
                         "      for(let i=0; i < " + num_games + "; i++)\n"
                         "      {\n"
                         "           console.log(\"game-status-\" + i.toString());\n"
                         "           var game_element = document.getElementById(\"game-status-\" + i.toString());\n"
                         "           if (parseInt(data.game_id_playing)==i)\n"  // playing same game overrides the element if also queued same
                         "           {\n"
                         "              console.log(\"playing found\");\n"
                         "              game_element.innerHTML=\"playing\";\n"
                         "           }\n"
                         "           else\n"
                         "           {\n"
                         "              if (parseInt(data.game_id_queued)==i)\n"
                         "              {\n"
                         "                  console.log(\"queued found\");\n"
                         "                  game_element.innerHTML=\"queued\";\n"
                         "              }\n"
                         "              else\n"
                         "              {\n"
                         "                  game_element.innerHTML=\"-------\";\n"
                         "              }\n"
                         "           }\n"
                         "      }\n"
                         "  };\n"
                         "}\n"
                         "\n"
                         "setInterval(updateAllEvents, 2000);\n"
                         "</script>\n";
    return content_str;
}

String get_async_html()
{
    String content_str = "<div class=\"event\" id=\"temp-event\">\n"
                         "HUB STATUS:<BR>\n"
                         "<strong class=\"api-msg\" id=\"api-hub-status\">init-status</strong><br />\n"
                         "</div>\n";

    return content_str;
}

String get_scheduler_html(int hub_mode)
{


    /*
        "<form method=\"post\" action=\"http://cleverpet.local\">\n"
        "Apply Daylight Savings: <select name=\"select_dst\" onchange=\"this.form.submit()\"><option value=\"1\"" + dst_option_1_sel + ">Yes</option><option value=\"0\"" + dst_option_2_sel + ">No</option></select><br>\n"
        "</form>\n";

    */

    // TODO javascript that sets min of "to" times to +1 hour past the "from" time
    // TODO javascript: schedule required if scheduled set? how to handle no scheduled value? or submit stay off / stay on? what if not set and required is set?

    //   <label for="dewey">Dewey</label>
    // checked instead of selected
    

    // TODO use hub mode from .h file declarations instead of hardcoded 0, 1, 2 here!

    String checked_str_stay_off = "";
    String checked_str_stay_on = "";
    String checked_str_stay_sch = "";

    if (hub_mode == 0)
    {
        checked_str_stay_off = "checked";
    }
    else if (hub_mode == 1)
    {
        checked_str_stay_on = "checked";
    }
    else if (hub_mode == 2)
    {
        checked_str_stay_sch = "checked";
    }

    String content_str = "<form method=\"post\" action=\"http://cleverpet.local\">\n"
                         //"Hub Mode (debug: hub mode is: " + int_to_string(hub_mode) + ")<br>\n"
                         "<label for=\"0\">Stay Off</label> <input type=\"radio\" id=\"html\" name=\"hub_mode\" value=\"0\" onchange=\"this.form.submit()\"" + checked_str_stay_off + ">\n"
                         "<label for=\"1\">Stay On</label> <input type=\"radio\" id=\"html\" name=\"hub_mode\" value=\"1\" onchange=\"this.form.submit()\"" + checked_str_stay_on + ">\n"
                         "<label for=\"2\">Scheduled</label> <input type=\"radio\" id=\"html\" name=\"hub_mode\" value=\"2\" onchange=\"this.form.submit()\"" + checked_str_stay_sch + ">\n"
                         "<br><br>\n"
                         "If scheduled:<br>\n"
                         "<b>    Weekdays</b> | From: <input type=\"time\" id=\"weekday_from\" name=\"weekday_from\"> To: <input type=\"time\" id=\"weekday_to\" name=\"weekday_to\"><br>\n"
                         "<b>    Weekends</b> | From: <input type=\"time\" id=\"weekend_from\" name=\"weekend_from\"> To: <input type=\"time\" id=\"weekend_to\" name=\"weekend_to\"><br>\n"
                         "</form>\n";

    return content_str;
}

int mgschwan_serve_webinterface(int current_game, int next_game, String display_error_msg, float &time_zone_offset, int time_zone_address, bool & dst_on, int dst_address, int & hub_mode, int hub_mode_address) {
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

                String next_game_str = int_to_string(next_game);
                String current_game_str = int_to_string(current_game);

                String return_str = "{"
                                    "\"status\":\"" + display_error_msg + "\","
                                    "\"game_id_queued\":\"" + next_game_str + "\","
                                    "\"game_id_playing\":\"" + current_game_str + "\","
                                    "\"time\":\"" + Time.timeStr() + "\""
                                    "}";
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
                    //Log.info("-----------*********************************---------");
                    //Log.print(thing + "\n");
                    //Log.info("-----------*********************************---------");

                    int timezone_index = thing.indexOf("select_timezone=");
                    int game_index = thing.indexOf("game=");
                    int dst_index = thing.indexOf("select_dst=");
                    int hub_mode_index = thing.indexOf("hub_mode=");

                    // TODO handle dst stuff
                    if (dst_index > -1)
                    {
                        Log.info("This is a DST post request.");

                        String tmp_2 = thing.substring(dst_index + 11);
                        //tmp_2 = tmp_2.substring(0, tmp_2.indexOf("&"));
                        Log.print(tmp_2 + "\n");
                        
                        // TODO NEED TO return TIME_ZONE_OFFSET
                        dst_on = bool(tmp_2.toInt());
                        if(dst_on)
                        {
                            Time.beginDST();
                        }
                        else
                        {
                            Time.endDST();
                        }
                        EEPROM.put(dst_address, dst_on);
                    }
                    else if (timezone_index > -1)
                    {
                        Log.info("This is a TIMEZONE post request.");

                        String tmp_2 = thing.substring(timezone_index + 16);
                        //tmp_2 = tmp_2.substring(0, tmp_2.indexOf("&"));
                        Log.print(tmp_2 + "\n");
                        
                        // TODO NEED TO return TIME_ZONE_OFFSET
                        float tz_offset = tmp_2.toFloat();
                        Time.zone(tz_offset);              
                        time_zone_offset = tz_offset;
                        EEPROM.put(time_zone_address, time_zone_offset);
                    }
                    else if (hub_mode_index > -1)
                    {
                        Log.info("This is a HUB MODE / SCHEDULER post request.");
                        //Log.print("*******\n");
                        //Log.print(thing + "\n");
                        //Log.print("*-------------*\n");
                        String tmp_2 = thing.substring(hub_mode_index + 9);
                        tmp_2 = tmp_2.substring(0, tmp_2.indexOf("&"));
                        Log.print(tmp_2 + "\n");
                        hub_mode = tmp_2.toInt();
                        EEPROM.put(hub_mode_address, hub_mode);
                    }
                    else if (game_index > -1)
                    {
                        Log.info("This is a GAME post request.");

                        String new_game_str = thing.substring(game_index + 5, game_index + 6);
                        String new_game_str_2 = thing.substring(game_index + 5, game_index + 7);
                        
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
                
                }

                if (req_get || req_post)
                {
                    // check URL for game to set, or none (keep playing current game)
                    //String route = thing.substring(thing.indexOf("GET") + 5);
                    //route = route.substring(0, route.indexOf(" "));

                    String content = "";
                    content += "<!DOCTYPE html>\n";
                    content += "<html>\n";
                    content += "<head>\n<style>\n";
                    content += get_css_string();
                    content += "</style>\n";
                    content += get_script_html();
                    content += "</head>\n";
                    content += "<body>\n";

                    // enable for debugging full GET request:
                    //content += "<br><br>";
                    //content += thing;
                    //content += "<br><br>";

                    // return the id from this function at the end; or, return what? -1? to indicate no new choice?

                    // print list of games and URL to go to
                    
                    content += "<br>\n";
                    content += "select game:<br><br>\n";

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

                    content += "<br>\n";
                    //content += display_error_msg;
                    //content += "<br>";
                    content += get_async_html();
                    content += "<br>\n";
                    
                    //    "<form method=\"post\" action=\"http://cleverpet.local\">\n"
                    //"Select Timezone: <select name=\"select_timezone\" onchange=\"this.form.submit()\">\n"
                    
                    String dst_option_1_sel = "";
                    String dst_option_2_sel = "";
                    if (dst_on)
                    {
                        dst_option_1_sel = " selected";
                    }
                    else
                    {
                        dst_option_2_sel = " selected";
                    }

                    content += "<form method=\"post\" action=\"http://cleverpet.local\">\n"
                               "Apply Daylight Savings: <select name=\"select_dst\" onchange=\"this.form.submit()\"><option value=\"1\"" + dst_option_1_sel + ">Yes</option><option value=\"0\"" + dst_option_2_sel + ">No</option></select><br>\n"
                               "</form>\n";

                    String time_zone_str = get_time_zone_string(time_zone_offset);
                    Log.info("time zone str length: " + int_to_string(time_zone_str.length()));

                    // TODO DEBUG THIS FURTHER:
                    String content_2 = "";
                    
                    content_2 += "<br>\n";
                    
                    content_2 += "Current Date/Time:<br>\n";
                    content_2 += "<b><strong class=\"api-msg\" id=\"api-time\">" + Time.timeStr() + "</strong><br />\n" + "</b>";

                    content_2 += "<br>\n";
                    
                    String content_3 = "";
                    
                    // add scheduler to content_3

                    content_3 += get_scheduler_html(hub_mode);

                    content_3 += "</body>\n";
                    content_3 += "</html>";
                    //Log.info("content length: " + int_to_string(content.length()));
                    //Log.info("content_2 length: " + int_to_string(content_2.length()));
                    webclient.println("HTTP/1.0 200 OK");
                    webclient.println("Content-type: text/html");
                    webclient.print("Content-length: ");
                    webclient.println(content.length() + time_zone_str.length() + content_2.length() + content_3.length());
                    webclient.println("");
                    webclient.print(content);
                    webclient.print(time_zone_str);
                    webclient.print(content_2);
                    webclient.print(content_3);
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

