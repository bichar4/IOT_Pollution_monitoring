#define CO2_TXPIN 10
#define CO2_RXPIN 11

#include <SoftwareSerial.h>
#include "MHZ19.h"
#include "SDS011.h"


MHZ19 mhz(&Serial1);

//SDS011 my_sds;
float pm10=0, pm25=0;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
float floatFromPC = 0.0;

boolean newData = false;

float SensorData[] = {50,50,50,50};

int number1 = 150;
int number2 = 150;
int number3 = 150;
int number4 = 150;

SoftwareSerial ESPSerial(8,9);//RX,TX
int error;
void setup() {
  // put your setup code here, to run once:
 Serial3.begin(115200);
 Serial2.begin(9600); // for pm sensor 
 Serial1.begin(9600); // for co2 sensor 

 Serial.begin(9600);

}

void loop() {
  
  
  getCO2Reading();
  getTemperature();
  error = PMread();
  
//  while(0)     
//  {
//   error = PMread();
////  if (error) {
////    Serial.println("error in PM");  
////  }
//  Serial.println("P2.5: " + String(number3));
//  Serial.println("P10:  " + String(number2));
//  delay(100);
//  }
  // put your main code here, to run repeatedly:
 SensorData[0] = number1;
 SensorData[1] = number2;
 SensorData[2] = number3;
 SensorData[3] = number4;
 
 sendTheData();
 Serial.println("data sent");
 displayTheData();
 Serial.println(" ");
 delay(1000);
}


void sendTheData()
{
   Serial3.print("<");
   for(int i=0; i<sizeof(SensorData) / sizeof(SensorData[0]); i++)
   {
      Serial3.print(SensorData[i]);
      Serial3.print(",");
   }
   Serial3.print(">");
}


void displayTheData()
{
   Serial.print("<");
   for(int i=0; i<sizeof(SensorData) / sizeof(SensorData[0]); i++)
   {
      Serial.print(SensorData[i]);
      Serial.print(",");
   }
   Serial.print(">");
}


void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    integerFromPC = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    floatFromPC = atof(strtokIndx);     // convert this part to a float

}

void getCO2Reading(){

  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
   number1 = mhz.getCO2();
   Serial.print(F("CO2: "));
   Serial.println(number1);
  }
  else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
  }

  
  delay(100);
}

void getTemperature(){
  
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  {
   number4 = mhz.getTemperature();
   Serial.print(F("Temperature: "));
   Serial.println(number4);
  }
  else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
  }
 
  
  delay(100);
}

int PMread() {
  byte buffer;
  int value;
  int len = 0;
  int pm10_serial = 0;
  int pm25_serial = 0;
  int checksum_is;
  int checksum_ok = 0;
  int error = 1;
  while ((Serial2.available() > 0) && (Serial2.available() >= (10-len))) {
    buffer = Serial2.read();
    value = int(buffer);
    switch (len) {
      case (0): if (value != 170) { len = -1; }; break;
      case (1): if (value != 192) { len = -1; }; break;
      case (2): pm25_serial = value; checksum_is = value; break;
      case (3): pm25_serial += (value << 8); checksum_is += value; break;
      case (4): pm10_serial = value; checksum_is += value; break;
      case (5): pm10_serial += (value << 8); checksum_is += value; break;
      case (6): checksum_is += value; break;
      case (7): checksum_is += value; break;
      case (8): if (value == (checksum_is % 256)) { checksum_ok = 1; } else { len = -1; }; break;
      case (9): if (value != 171) { len = -1; }; break;
    }
    len++;
    if (len == 10 && checksum_ok == 1) {
       number2=(float)pm10_serial/10.0;
       number3=(float)pm25_serial/10.0;
      len = 0; checksum_ok = 0; pm10_serial = 0.0; pm25_serial = 0.0; checksum_is = 0;
      error = 0;
      
    }
    yield();
  }
  return error;
}
