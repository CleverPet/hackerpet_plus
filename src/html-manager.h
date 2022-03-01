#ifndef HTML_MANAGER_H
#define HTML_MANAGER_H

#include "Particle.h"


class HtmlManager
{
    public:
        HtmlManager();
        String get_css_string();
        String get_script_html();
        String get_link_for_game(int game_index, String game_index_str, String game_name, int current_game, int overrideable_next_game);
        String get_async_html();
        String get_time_zone_string(float time_zone_offset);
        String get_scheduler_html(int hub_mode, String weekday_from, String weekday_to, String weekend_from, String weekend_to);
        String get_kibbles_html(int kibbles_limit, int kibbles_eaten_today);
    private:
        String _get_selected_option(float time_zone_offset, float compare);
        String _get_post_link_string(String text, String name, String value);

};

#endif