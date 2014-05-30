
#include "Arduino.h"
#include <UIPEthernet.h>

// number of functions that can be defined in main app
#define NUMBER_FUNCTIONS 4

class SmsREST
{

public:
  SmsREST();
  void set_id(String device_id);
  void set_name(String device_name);
  void handle(EthernetClient client);
  void function(String function_name, int (*f)(String));

private:
  String id;
  String name;   		

  void handle_request(EthernetClient client);
  void send_http_headers(EthernetClient client);
  void send_end_response(EthernetClient client);	
  int freeRAM();
  String decode(String s);
  unsigned char h2int(char c);

  // Functions array
  uint8_t functions_number;	
  int (*functions[NUMBER_FUNCTIONS])(String);
  String functions_names[NUMBER_FUNCTIONS];
};

