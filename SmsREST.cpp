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
  boolean arguments_selected = false;	
  boolean command_sent = false;		
  String arguments = "";	
  uint8_t function_index = -1;

  // Check if there is data available to read
  while (client.available()) 
  {
    char c = client.read();

    buffer += c;

    Serial.print(c);

    // reset buffer when something important should come next
    // read line by line and discard previous line 
    // '/' is marker for function name
    if (c == '\n' || c == '/') 
    {      	
      buffer = "";
    }  

    // get HTTP request method (GET or POST)
    // first space marks
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

      buffer = "";
      continue;  // to read the next char
    }

    // determine function name and GET parameters (if GET detected)
    if (http_method_selected && (c == ' ') && !function_selected) 
    {
      buffer.trim();
      String fname = buffer;
      int idx = buffer.indexOf('?');

      // GET has params in query line
      if (is_GET_request && idx != -1)
      {
        fname = buffer.substring(0, idx);

        if (idx < buffer.length() - 1)
        {
          arguments = decode(buffer.substring(idx + 1));
        }        
      }              

      // Check if function name is in array or registered functions
      for (int i = 0; i < functions_number; i++)
      {						
        if(fname.equals(functions_names[i])) 
        {				
          function_index = i;
          function_selected = true;
          break;
        }
      }

      if (is_GET_request)
      {
        arguments_selected = true;  
      }  

      Serial.println("\n# Function: " + buffer);
      Serial.println("# Arguments: " + arguments);
      buffer = "";
      continue;
    }	

    // HTTP POST request -> get function arguments
    if (is_POST_request && function_selected && (client.available() == 0) && !arguments_selected) 
    {		
      Serial.println("\n# POST Arguments: " + buffer);

      arguments = decode(buffer);
      arguments_selected = true;

      buffer = "";
    }
  }

  // prepare and send http response
  if (!function_selected)   // send error response
  {
    send_http_headers(client);
    send_error_response(client, "Invalid function");
    send_end_response(client);
  }
  else 
  {		
    send_http_headers(client);
    // Execute function
    int result = functions[function_index](arguments);
    Serial.println(F("# function invoked"));			       
    String fname = functions_names[function_index];
    send_func_exec_response(client, fname, result);
    send_end_response(client);
  }

}



void SmsREST::send_http_headers(EthernetClient client)
{
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.println();  
}


void SmsREST::send_func_exec_response(EthernetClient client, String fname, int result)
{  
  client.print(F("{\"return_value\": "));
  client.print(result);
  client.print(F(", \"message\": \""));
  client.print(F("Function "));
  client.print(fname);
  client.print(F(" has been executed\", "));	  
}  

void SmsREST::send_error_response(EthernetClient client, String error)
{  
  client.print(F("{\"error\": \""));
  client.print(error);
  client.print(F("\","));
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



