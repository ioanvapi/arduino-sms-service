#include "SmsREST.h"


SmsREST::SmsREST() 
{				
}

void SmsREST::set_id(String device_id)
{
  id = device_id;
}

void SmsREST::set_name(String device_name)
{  
  name = device_name;
}



void SmsREST::handle(EthernetClient client) 
{
  if (client.available()) 
  {
    // Handle request
    handle_request(client);

    // Give the web browser time to receive the data
    delay(5);

    client.stop();  	   	

    Serial.print(F("\n#Free RAM: "));
    Serial.println(freeRAM());
    Serial.println();
  }
}

void SmsREST::handle_request(EthernetClient client) 
{
  String buffer = "";
  boolean http_method_selected = false;
  boolean is_GET_request = false;
  boolean is_POST_request = false;
  boolean function_selected = false;
  boolean params_selected = false;	
  boolean command_sent = false;		
  String arguments = "";	
  uint8_t function_index = 0;

  // Check if there is data available to read
  while (client.available()) 
  {
    char c = client.read();
    delay(1);
    buffer = buffer + c;

    Serial.print(c);

    // Check for end of request
    if (c == '\n' || c == '/') 
    {      	
      // Reset buffer
      buffer = "";
    }  

    // get HTTP request method (GET or POST)
    if (c == ' ' && !http_method_selected) 
    {		
      buffer.trim();	

      if (buffer.equals("GET")) 
      {			
        is_GET_request = true;
        http_method_selected = true;
      }
      else if (buffer.equals("POST")) 
      {			
        is_POST_request = true;
        http_method_selected = true;
      }
      //Serial.println("\n# HTTP method: " + buffer);
    }

    // get function name
    if (http_method_selected && ((c == ' ') || c == '?') && !function_selected) 
    {
      if (c == ' ') 
      {
        buffer.trim();
      }
      else 
      {
        buffer = buffer.substring(0, buffer.length()-1);		
      }			

      //Serial.println("\n# Buffer before function: " + buffer);

      // Check if function name is in array
      for (int i = 0; i < functions_number; i++)
      {						
        if(buffer.equals(functions_names[i])) 
        {				
          Serial.println("\n# Function: " + functions_names[i]);

          function_selected = true;									
          function_index = i;
          break;
        }
      }

      //Serial.println(F("# Buffer reset"));     
      buffer = "";
    }	

    // HTTP GET request -> get function arguments
    if (is_GET_request && function_selected && (c == ' ') && !params_selected) 
    {		
      Serial.println("\n# Arguments: " + buffer);

      arguments = buffer;
      params_selected = true;

      //Serial.println(F("# Buffer reset"));     
      buffer = "";
    }

    // HTTP POST request -> get function arguments
    if (is_POST_request && function_selected && (client.available() == 0) && !params_selected) 
    {		
      Serial.println("\n# Arguments: " + buffer);

      arguments = decode(buffer);
      params_selected = true;

      //Serial.println(F("# Buffer reset"));     
      buffer = "";
    }


    // FINALLY !!! -> execute function and send http response 
    if (function_selected && params_selected && !command_sent) 
    {		
      send_http_headers(client);
      Serial.println(F("# Headers sent."));
      // Execute function
      int result = functions[function_index](arguments);
      Serial.println(F("# function invoked"));			   
      // Send feedback to client
      client.print(F("{\"return_value\": "));
      client.print(result);
      client.print(F(", \"message\": \""));
      client.print(F("Function "));
      client.print(functions_names[function_index]);
      client.print(F(" has been executed\", "));	
      Serial.println(F("# before end response"));			   
      // End of message
      send_end_response(client);
      Serial.println(F("# after end response"));			   
      // End here
      command_sent = true;   
    }

  }

}



void SmsREST::send_http_headers(EthernetClient client)
{
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.println();  
}

void SmsREST::send_end_response(EthernetClient client)
{
  client.print(F("\"id\": \""));
  client.print(id);
  client.print(F("\", \"name\": \""));
  client.print(name);
  client.println(F("\", \"connected\": true}"));
}



void SmsREST::function(String function_name, int (*f)(String))
{
  functions_names[functions_number] = function_name;
  functions[functions_number] = f;
  functions_number++;
}

int SmsREST::freeRAM () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


// decodes + to space and '%hh' to decimal char code
String SmsREST::decode(String s)
{
  String decoded = "";
  char c, x;

  for (int i=0 ; i < s.length(); i++) {
    c = s.charAt(i);
    if (c == '+')
    {
      c = ' ';
    }
    if (c == '%' && ((i + 2) < s.length())) 
    {
      x = h2int(s.charAt(++i)) << 4;
      c = x | h2int(s.charAt(++i));
    }
    decoded += c;
  }

  return decoded;
}

// convert a single hex digit character to its integer value
unsigned char SmsREST::h2int(char c)
{
  if (c >= '0' && c <='9')
  {
    return((unsigned char)c - '0');
  }
  if (c >= 'a' && c <='f')
  {
    return((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <='F')
  {
    return((unsigned char)c - 'A' + 10);
  }
  return(0);
} 


