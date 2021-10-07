#include <Particle.h>

using namespace std;

//Calculate a broadcast address from the local IP and netmask
IPAddress mgschwan_getBroadcastAddress(); 

bool mgschwan_setupMDNS();

void mgschwan_setupNetwork();

int mgschwan_serve_webinterface(int current_game, int next_game, String display_error_msg);

void mgschwan_MDNS_loop();
