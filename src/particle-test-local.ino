/*
 * Project particle-test-local
 * Description:
 * Author:
 * Date:
 */


#include "Particle.h"
#include "softap_http.h"


// enables simultaneous execution of application and system thread
SYSTEM_THREAD(ENABLED);

// Use primary serial over USB interface for logging output (9600)
// Choose logging level here (ERROR, WARN, INFO)
SerialLogHandler logHandler(LOG_LEVEL_INFO, { // Logging level for all messages
    { "app.hackerpet", LOG_LEVEL_ERROR }, // Logging level for library messages
    { "app", LOG_LEVEL_INFO } // Logging level for application messages
});


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

STARTUP(softap_set_application_page_handler(myPages, nullptr));




long last_print = 0;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  if (millis() - last_print > 3000)
  {

      last_print = millis();
      Log.info("running loop... %i", last_print);

  }

}