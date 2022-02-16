#include "html-manager.h"


HtmlManager::HtmlManager()
{

}

String HtmlManager::get_css_string()
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

String HtmlManager::get_script_html()
{

    // now have also: game-id-queued, game-id-playing

    String num_games = "12";
    
    String content_str = "<script>\n"
                         "function updateAllEvents() { \n"
                         "  var xhttp = new XMLHttpRequest();\n"
                         "  var status_element = document.getElementById(\"api-hub-status\");\n"
                         "  var time_element = document.getElementById(\"api-time\");\n"
                         "  var hub_state_element = document.getElementById(\"api-hub-state\");\n"
                         "  xhttp.open(\"GET\", \"http://cleverpet.local/local-api\", true);\n"
                         "  xhttp.send();\n"
                         "  xhttp.onload = function () {\n"
                         "      console.log('DONE', xhttp.readyState);\n" // readyState will be 4
                         "      console.log(xhttp.responseText);\n"
                         "      var data = JSON.parse(xhttp.responseText);\n"
                         "      status_element.innerHTML = data.status;\n"
                         "      time_element.innerHTML = data.time;\n"
                         "      hub_state_element.innerHTML = data.hub_state;\n"
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

String HtmlManager::get_link_for_game(int game_index, String game_index_str, String game_name, int current_game, int overrideable_next_game)
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
                         "[<b><strong class=\"api-msg\" id=\"game-status-" + game_index_str + "\">" + placeholder + "</strong></b>] " + _get_post_link_string(game_name, "game", game_index_str) + "<br />\n"
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

String HtmlManager::get_async_html()
{
    String content_str = "<div class=\"event\" id=\"temp-event\">\n"
                         "HUB STATUS:<BR>\n"
                         "<strong class=\"api-msg\" id=\"api-hub-status\">init-status</strong><br />\n"
                         "</div>\n";

    return content_str;
}

String HtmlManager::get_time_zone_string(float time_zone_offset)
{

    String time_zone_string = 
    "<form method=\"post\" action=\"http://cleverpet.local\">\n"
    "Select Timezone: <select name=\"select_timezone\" onchange=\"this.form.submit()\">\n"
	"<option timeZoneIndex=\"1\" gmtAdjustment=\"GMT-12:00\" useDaylightTime=\"0\" value=\"-12\"" + _get_selected_option(time_zone_offset, -12) + ">(GMT-12:00) International Date Line West</option>\n"
	"<option timeZoneIndex=\"2\" gmtAdjustment=\"GMT-11:00\" useDaylightTime=\"0\" value=\"-11\"" + _get_selected_option(time_zone_offset, -11) + ">(GMT-11:00) Midway Island, Samoa</option>\n"
	"<option timeZoneIndex=\"3\" gmtAdjustment=\"GMT-10:00\" useDaylightTime=\"0\" value=\"-10\"" + _get_selected_option(time_zone_offset, -10) + ">(GMT-10:00) Hawaii</option>\n"
	"<option timeZoneIndex=\"4\" gmtAdjustment=\"GMT-09:00\" useDaylightTime=\"1\" value=\"-9\"" + _get_selected_option(time_zone_offset, -9) + ">(GMT-09:00) Alaska</option>\n"
	"<option timeZoneIndex=\"5\" gmtAdjustment=\"GMT-08:00\" useDaylightTime=\"1\" value=\"-8\"" + _get_selected_option(time_zone_offset, -8) + ">(GMT-08:00) Pacific Time (US & Canada)</option>\n"
	"<option timeZoneIndex=\"9\" gmtAdjustment=\"GMT-07:00\" useDaylightTime=\"1\" value=\"-7\"" + _get_selected_option(time_zone_offset, -7) + ">(GMT-07:00) Mountain Time (US & Canada)</option>\n"
	"<option timeZoneIndex=\"11\" gmtAdjustment=\"GMT-06:00\" useDaylightTime=\"1\" value=\"-6\"" + _get_selected_option(time_zone_offset, -6) + ">(GMT-06:00) Central Time (US & Canada)</option>\n"
	"<option timeZoneIndex=\"15\" gmtAdjustment=\"GMT-05:00\" useDaylightTime=\"1\" value=\"-5\"" + _get_selected_option(time_zone_offset, -5) + ">(GMT-05:00) Eastern Time (US & Canada)</option>\n"
	"<option timeZoneIndex=\"17\" gmtAdjustment=\"GMT-04:00\" useDaylightTime=\"1\" value=\"-4\"" + _get_selected_option(time_zone_offset, -4) + ">(GMT-04:00) Atlantic Time (Canada)</option>\n"
	"<option timeZoneIndex=\"21\" gmtAdjustment=\"GMT-03:30\" useDaylightTime=\"1\" value=\"-3.5\"" + _get_selected_option(time_zone_offset, -3.5) + ">(GMT-03:30) Newfoundland</option>\n"
	"<option timeZoneIndex=\"23\" gmtAdjustment=\"GMT-03:00\" useDaylightTime=\"0\" value=\"-3\"" + _get_selected_option(time_zone_offset, -3) + ">(GMT-03:00) Buenos Aires, Georgetown</option>\n"
	"<option timeZoneIndex=\"26\" gmtAdjustment=\"GMT-02:00\" useDaylightTime=\"1\" value=\"-2\"" + _get_selected_option(time_zone_offset, -2) + ">(GMT-02:00) Mid-Atlantic</option>\n"
	"<option timeZoneIndex=\"27\" gmtAdjustment=\"GMT-01:00\" useDaylightTime=\"0\" value=\"-1\"" + _get_selected_option(time_zone_offset, -1) + ">(GMT-01:00) Cape Verde Is.</option>\n"
	"<option timeZoneIndex=\"30\" gmtAdjustment=\"GMT+00:00\" useDaylightTime=\"1\" value=\"0\"" + _get_selected_option(time_zone_offset, 0) + ">(GMT+00:00) Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London</option>\n"
	"<option timeZoneIndex=\"31\" gmtAdjustment=\"GMT+01:00\" useDaylightTime=\"1\" value=\"1\"" + _get_selected_option(time_zone_offset, 1) + ">(GMT+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna</option>\n"
	"<option timeZoneIndex=\"37\" gmtAdjustment=\"GMT+02:00\" useDaylightTime=\"1\" value=\"2\"" + _get_selected_option(time_zone_offset, 2) + ">(GMT+02:00) Athens, Bucharest, Istanbul</option>\n"
	"<option timeZoneIndex=\"46\" gmtAdjustment=\"GMT+03:00\" useDaylightTime=\"1\" value=\"3\"" + _get_selected_option(time_zone_offset, 3) + ">(GMT+03:00) Moscow, St. Petersburg, Volgograd</option>\n"
	"<option timeZoneIndex=\"49\" gmtAdjustment=\"GMT+03:30\" useDaylightTime=\"1\" value=\"3.5\"" + _get_selected_option(time_zone_offset, 3.5) + ">(GMT+03:30) Tehran</option>\n"
	"<option timeZoneIndex=\"50\" gmtAdjustment=\"GMT+04:00\" useDaylightTime=\"0\" value=\"4\"" + _get_selected_option(time_zone_offset, 4) + ">(GMT+04:00) Abu Dhabi, Muscat</option>\n"
	"<option timeZoneIndex=\"53\" gmtAdjustment=\"GMT+04:30\" useDaylightTime=\"0\" value=\"4.5\"" + _get_selected_option(time_zone_offset, 4.5) + ">(GMT+04:30) Kabul</option>\n"
	"<option timeZoneIndex=\"55\" gmtAdjustment=\"GMT+05:00\" useDaylightTime=\"0\" value=\"5\"" + _get_selected_option(time_zone_offset, 5) + ">(GMT+05:00) Islamabad, Karachi, Tashkent</option>\n"
	"<option timeZoneIndex=\"57\" gmtAdjustment=\"GMT+05:30\" useDaylightTime=\"0\" value=\"5.5\"" + _get_selected_option(time_zone_offset, 5.5) + ">(GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi</option>\n"
	"<option timeZoneIndex=\"58\" gmtAdjustment=\"GMT+05:45\" useDaylightTime=\"0\" value=\"5.75\"" + _get_selected_option(time_zone_offset, 5.75) + ">(GMT+05:45) Kathmandu</option>\n"
	"<option timeZoneIndex=\"59\" gmtAdjustment=\"GMT+06:00\" useDaylightTime=\"1\" value=\"6\"" + _get_selected_option(time_zone_offset, 6) + ">(GMT+06:00) Almaty, Novosibirsk</option>\n"
	"<option timeZoneIndex=\"61\" gmtAdjustment=\"GMT+06:30\" useDaylightTime=\"0\" value=\"6.5\"" + _get_selected_option(time_zone_offset, 6.5) + ">(GMT+06:30) Yangon (Rangoon)</option>\n"
	"<option timeZoneIndex=\"62\" gmtAdjustment=\"GMT+07:00\" useDaylightTime=\"0\" value=\"7\"" + _get_selected_option(time_zone_offset, 7) + ">(GMT+07:00) Bangkok, Hanoi, Jakarta</option>\n"
	"<option timeZoneIndex=\"64\" gmtAdjustment=\"GMT+08:00\" useDaylightTime=\"0\" value=\"8\"" + _get_selected_option(time_zone_offset, 8) + ">(GMT+08:00) Beijing, Chongqing, Hong Kong, Urumqi</option>\n"
	"<option timeZoneIndex=\"69\" gmtAdjustment=\"GMT+09:00\" useDaylightTime=\"0\" value=\"9\"" + _get_selected_option(time_zone_offset, 9) + ">(GMT+09:00) Osaka, Sapporo, Tokyo</option>\n"
	"<option timeZoneIndex=\"72\" gmtAdjustment=\"GMT+09:30\" useDaylightTime=\"0\" value=\"9.5\"" + _get_selected_option(time_zone_offset, 9.5) + ">(GMT+09:30) Adelaide</option>\n"
	"<option timeZoneIndex=\"75\" gmtAdjustment=\"GMT+10:00\" useDaylightTime=\"1\" value=\"10\"" + _get_selected_option(time_zone_offset, 10) + ">(GMT+10:00) Canberra, Melbourne, Sydney</option>\n"
	"<option timeZoneIndex=\"79\" gmtAdjustment=\"GMT+11:00\" useDaylightTime=\"1\" value=\"11\"" + _get_selected_option(time_zone_offset, 11) + ">(GMT+11:00) Magadan, Solomon Is., New Caledonia</option>\n"
	"<option timeZoneIndex=\"80\" gmtAdjustment=\"GMT+12:00\" useDaylightTime=\"1\" value=\"12\"" + _get_selected_option(time_zone_offset, 12) + ">(GMT+12:00) Auckland, Wellington</option>\n"
	"<option timeZoneIndex=\"82\" gmtAdjustment=\"GMT+13:00\" useDaylightTime=\"0\" value=\"13\"" + _get_selected_option(time_zone_offset, 13) + ">(GMT+13:00) Nuku'alofa</option>\n"
    "</select><br>\n"
    "</form>\n";				

    return time_zone_string;
}

String HtmlManager::get_scheduler_html(int hub_mode, String weekday_from, String weekday_to, String weekend_from, String weekend_to)
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

    // sending data:
    // https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/send
    
    String content_str = "<form method=\"post\" action=\"http://cleverpet.local\">\n"
                         //"Hub Mode (debug: hub mode is: " + int_to_string(hub_mode) + ")<br>\n"
                         "<label for=\"0\">Stay Off</label> <input type=\"radio\" id=\"html\" name=\"hub_mode\" value=\"0\" onchange=\"this.form.submit()\"" + checked_str_stay_off + ">\n"
                         "<label for=\"1\">Stay On</label> <input type=\"radio\" id=\"html\" name=\"hub_mode\" value=\"1\" onchange=\"this.form.submit()\"" + checked_str_stay_on + ">\n"
                         "<label for=\"2\">Scheduled</label> <input type=\"radio\" id=\"html\" name=\"hub_mode\" value=\"2\" onchange=\"this.form.submit()\"" + checked_str_stay_sch + ">\n"
                         "<br><br>\n"
                         "<script>\n"
                         "function sch_change() { \n"
                         "      var xhttp = new XMLHttpRequest();\n"
                         "      xhttp.open(\"POST\", \"http://cleverpet.local/local-api/scheduler\", true);\n"
                         "      xhttp.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\n"
                         "      var weekday_from = document.getElementById(\"weekday_from\");\n"
                         "      var weekday_to = document.getElementById(\"weekday_to\");\n"
                         "      var weekend_from = document.getElementById(\"weekend_from\");\n"
                         "      var weekend_to = document.getElementById(\"weekend_to\");\n"
                         "      xhttp.send(\"weekday_from=\"+weekday_from.value+\"&weekday_to=\"+weekday_to.value+\"&weekend_from=\"+weekend_from.value+\"&weekend_to=\"+weekend_to.value);\n"
                         
                         "      xhttp.onload = function () {\n"
                         "           console.log(xhttp.responseText);\n"
                         // TODO ! set values?
                         "      };\n"
                         "      \n"
                         "}\n"
                         "</script>\n"
                         "If scheduled:<br>\n"
                         "<b>    Weekdays</b> | From: <input type=\"time\" id=\"weekday_from\" name=\"weekday_from\" onchange=\"sch_change()\" value=\"" + weekday_from + "\"> To: <input type=\"time\" id=\"weekday_to\" name=\"weekday_to\" onchange=\"sch_change()\" value=\"" + weekday_to + "\"><br>\n"
                         "<b>    Weekends</b> | From: <input type=\"time\" id=\"weekend_from\" name=\"weekend_from\" onchange=\"sch_change()\" value=\"" + weekend_from + "\"> To: <input type=\"time\" id=\"weekend_to\" name=\"weekend_to\" onchange=\"sch_change()\" value=\"" + weekend_to + "\"><br>\n"
                         "</form>\n";

    return content_str;
}

String HtmlManager::_get_selected_option(float time_zone_offset, float compare)
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

String HtmlManager::_get_post_link_string(String text, String name, String value)
{
    String post_link_str = "<form method=\"post\" action=\"http://cleverpet.local\" class=\"inline\">\n"
                           "<input type=\"hidden\" name=\"" + name + "\" value=\"" + value + "\">\n"
                           "<button type=\"submit\" name=\"submit_param\" value=\"submit_value\" class=\"link-button\">" + text + ""
                           "</button>\n"
                           "</form>\n";
    return post_link_str;
}