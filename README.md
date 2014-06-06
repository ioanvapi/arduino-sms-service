# arduino-sms-service - UNDER CONSTRUCTION :)


## Overview
Sms REST Service made of Arduino Nano, Ethernet Wiznet W5100 shield and EFCom SIM900 GSM shield.

I used a similar approach as the suggestet REST API but I removed the code that is not involved in sending a SMS.

The service supports both GET and POST methods and responds with a status in json format.

The GSM library uses digital pins 2 and 3 or Arduino for software serial communication with GSM shield.
GSM shield uses the digital pin(D6) of Arduino to power on and reset(D5) its gsm module. 
That's why I had to make a change in GSM.h file of library. I changed the pins definition for GSM_ON and GSM_RESET to be:
 - #define GSM_ON              6 // EFCOM module ON
 - #define GSM_RESET           5 // EFCOM module RESET

##Additional information
- [EFCom Pro GPRS/GSM Module Shield](http://www.elecfreaks.com/wiki/index.php?title=EFCom_Pro_GPRS/GSM_Module)
- [EFCom GPRS/GSM Module Shield](http://www.elecfreaks.com/wiki/index.php?title=EFCom_GPRS/GSM_Shield)
- [GSM-GPRS-GPS-Shield library](https://github.com/MarcoMartines/GSM-GPRS-GPS-Shield) 
- [Arduino REST API](https://github.com/marcoschwartz/aREST)
- [Arduino Nano](http://arduino.cc/en/Main/ArduinoBoardNano)
  
