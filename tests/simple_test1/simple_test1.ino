/*
 Built on Arduino Uno + Wiznet Ethernet shield. 
 Tested with browser and a POSTMAN chrome plugin
 
 End of line = '13 10' = '\r \n'
 End of request = empty line = '13 10' = '\r \n'
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


void setup() 
{
  Serial.begin(9600);

  Ethernet.begin(mac, ip, domain, gateway, mask);
  server.begin();
  Serial.print(F("## server is at "));
  Serial.println(Ethernet.localIP());
}  


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) 
  {
    Serial.println(F("## new client"));

    // an http request ends with a blank line
    boolean after_end_of_line = false;

    while (client.connected() && client.available()) 
    {
      char c = client.read();      

      Serial.write(c);  

      if (c == '\n' && after_end_of_line) {        
        // send a standard http response header
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.print("Hello world from Arduino !!!");                        
        client.println("</html>");
        break;
      }

      if (c == '\n') {
        after_end_of_line = true;
      } 
      else if (c != '\r') {
        after_end_of_line = false;
      }      
    }

    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println(F("## client disonnected"));
  }
}













