#ifndef HTTP_SERVER_UTIL_H
#define HTTP_SERVER_UTIL_H

#include <Particle.h>

#include <MDNS.h>
using namespace mdns;

using namespace std;

//Calculate a broadcast address from the local IP and netmask
IPAddress mgschwan_getBroadcastAddress(); 

bool mgschwan_setupMDNS(MDNS * mgschwan_mdns, bool begin_only);

void mgschwan_setupNetwork(MDNS * mgschwan_mdns, bool begin_only);

int mgschwan_serve_webinterface(int current_game, int next_game, String display_error_msg, float &time_zone_offset, int time_zone_address, bool & dst_on, int dst_address, int & hub_mode, int hub_mode_address);

void mgschwan_MDNS_loop(MDNS * mgschwan_mdns);

String int_to_string(int the_int);

String float_to_string(float the_float);

#endif