
#include <SPI.h>
#include <Ethernet.h>
#include "SmsREST.h"

byte MAC[] = { 
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
		
IPAddress DNS(10, 220, 10, 1);
IPAddress GW(10, 220, 0, 1);
IPAddress MASK(255, 255, 0, 0);  
IPAddress IP(10, 220, 10, 171);       //is-giro-01

//IPAddress IP(192, 168, 1, 171);     
//IPAddress MASK(255, 255, 255, 0);   
//IPAddress GW(192, 168, 1, 1);       
//IPAddress DNS(192, 168, 1, 1);      

EthernetServer server(80);

SmsREST rest = SmsREST();

void setup() 
{
  // Start Serial
  Serial.begin(9600);

  // Give name and ID to device
  rest.set_id("001");
  rest.set_name("arduino_device");
  rest.function("sendsms", sendSMS);
  rest.function("proba", proba);

  ethInit();
}

void loop() 
{
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
}

// initialise the Ethernet card
void ethInit() 
{  
  Ethernet.begin(MAC, IP, DNS, GW, MASK);
  server.begin();
  Serial.print(F("\n# RESET: server is at: "));
  Serial.println(Ethernet.localIP());
}

int sendSMS(String args) 
{
  Serial.print(F("# sendSMS() invoked with args: '"));
  Serial.print(args);
  Serial.println("'");

  byte idx = args.indexOf('&');
  String phone = args.substring(6, idx);
  String msg = args.substring(idx + 5);
  Serial.println("# Phone: " + phone);
  Serial.println("# Msg: " + msg);
  return 1;  
}


// function registered for testing purpose
int proba(String args)
{
  return args.length();
}




