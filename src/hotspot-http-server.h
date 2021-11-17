#ifndef HOTSPOT_HTTP_SERVER_H
#define HOTSPOT_HTTP_SERVER_H

#include "Particle.h"
#include "softap_http.h"

// This is for hosting http during listen mode
void myPages(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved)
{
  if (!strcmp(url, "/helloworld")) 
  {
       
      int addr = 10;
      uint16_t value = 12345;
      EEPROM.put(addr, value);
      
      // send the response code 200, the mime type "text/html"
      cb(cbArg, 0, 200, "text/html", nullptr);
      // send the page content
      result->write("<h2>hello world!</h2>");
      return;
  }
  if (!strcmp(url, "/index")) 
  {
      // send the response code 200, the mime type "text/html"
      cb(cbArg, 0, 200, "text/html", nullptr);
      // send the page content

      uint16_t objec;
      int address = 10;
      EEPROM.get(address, objec);

      Serial.printf("value: %d", int(objec));
      result->write("<h2>default!!!</h2>");
      return;
  }
  
  cb(cbArg, 0, 404, "text/plain", nullptr);
  return;
}

#endif