/*
 Built on Arduino Nano + Wiznet W5100 Ethernet shield and Ethernet library. 
 Tested in a browser and a POSTMAN chrome plugin
 
 End of line = '13 10' = '\r \n'
 End of request = empty line = '13 10' = '\r \n'
 
 Note: It works with both GET and POST requests.
 */


#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(10, 220, 10 , 171);
IPAddress mask(255, 255, 0 , 0);
IPAddress gateway(10, 220, 0 , 1);
IPAddress domain(10, 220, 10 , 1);

EthernetServer server(80);
EthernetClient client;

void setup() 
{
  Serial.begin(9600);

  Ethernet.begin(mac, ip, domain, gateway, mask);
  server.begin();
  Serial.print(F("## server is at "));
  Serial.println(Ethernet.localIP());
}  


void loop() {

  if (client = server.available()) 
  {
    Serial.println(F("\n## new client"));

    while (client.available()) 
    {
      Serial.write((char) client.read());  
    }         

    send_http_response(client); 
    
    delay(5);
    client.stop();
    Serial.println(F("\n## client disconnected"));
  }
}


void send_http_response(EthernetClient client)
{ 
  // send a standard http response
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println();
  client.println(F("<!DOCTYPE HTML>"));
  client.println(F("<html>"));
  client.print(F("Hello world from Arduino !!!"));
  client.println(F("</html>"));
  client.println();
}














