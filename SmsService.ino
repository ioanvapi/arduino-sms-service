
/*
return code for SendSMS:

        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module didn't answer in timeout
        -3 - GSM module has answered "ERROR" string

        OK ret val:
        -----------
        0 - SMS was not sent
        1 - SMS was sent

*/

#include <SPI.h>
#include <Ethernet.h>
#include "SIM900.h"
#include "sms.h"
#include <SoftwareSerial.h>

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
SMSGSM sms_gsm;

// GSM module initialised
boolean gsm_started = false;

void setup() 
{
  // Start Serial
  Serial.begin(9600);
  initGSM();
  initEthernet();
  
  // Give name and ID to device
  rest.set_id("001");
  rest.set_name("arduino_device");
  
  //register REST functions
  rest.function("sendsms", sendSMS);
  
  // this one is for testing purpose
  rest.function("proba", proba);
}

void loop() 
{
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
}

// invoked when sens sms request comes
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
  
  if (gsm_started)
  {
    Serial.println(F("# sending SMS ..."));
    char p[phone.length() + 1];  // +1 extra for '\0'
    char m[msg.length() + 1];
    phone.toCharArray(p, phone.length() + 1);
    msg.toCharArray(m, msg.length() + 1);
    char code = sms_gsm.SendSMS(p, m);
    Serial.print(F("# sending SMS done with response code: "));
    Serial.println(code);
    return code;
  }
  else 
  {
    return 0;  
  }  
}


// function registered for testing purpose
int proba(String args)
{
  return args.length();
}


// initialise the Ethernet card
void initEthernet() 
{  
  Ethernet.begin(MAC, IP, DNS, GW, MASK);
  server.begin();
  Serial.print(F("\n# RESET: server is at: "));
  Serial.println(Ethernet.localIP());
}

// initialise the GSM module
void initGSM() {
  if (gsm.begin(19200)) {
    Serial.println(F("\n GSM=READY"));
    gsm_started = true;
  } 
  else {
    Serial.println(F("\nGSM=IDLE"));
  }
}


