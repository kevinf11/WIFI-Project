#include <SoftwareSerial.h>
#include <DHT.h>
//arduino RX pin=2  arduino TX pin=3
SoftwareSerial espSerial =  SoftwareSerial(2,3);
#define DHTPIN 5//Connect the data pin to pin 5 of the Arduino
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
String apiKey = "YGPX9IUK6XKNAIZS";//API KEY WRITE of ThingSpeak
String ssid="Cementerio de Moscas";//Wifi network SSID
String password ="MoonifyYT123456";//Wifi network password

//Time Response Function
void tiempoRespuesta(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        Serial.print(c);
      }
    }
}

boolean thingSpeakWrite(float value1, float value2){
  String cmd = "AT+CIPSTART=\"TCP\",\"";//Conexión TCP
  cmd += "184.106.153.149";//api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);//Send serial data to ESP8266
  Serial.println(cmd);
  if(espSerial.find("Error")){
     Serial.println("AT+CIPSTART error");
    return false;
  }
  //Prepare the GET String to send
  String getStr = "GET /update?api_key=YGPX9IUK6XKNAIZS";
  getStr += apiKey;
  getStr +="&field1=";
  //Convert from floating to String
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr += "\r\n\r\n";
  //Send chain length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  Serial.println(cmd);
  delay(100);
  if(espSerial.find(">")){//If ok send the GET string
    espSerial.print(getStr);
    Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    //Let the user know that data is not being sent
    Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}

void setup() {

  Serial.begin(9600);
  dht.begin();
  espSerial.begin(9600);
  //Enable these lines to reset the module
  //espSerial.println("AT+RST");
  //tiempoRespuesta(1000);
  //Enable these lines to configure the module with 9600 speed
  //espSerial.println("AT+UART_CUR=9600,8,1,0,0");
  //tiempoRespuesta(1000);
  //Configure the ESP8266 as cliente
  espSerial.println("AT+CWMODE=1");
  tiempoRespuesta(1000);
  //Configure the SSID and password for WIFI connection
  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");
  tiempoRespuesta(5000);
  Serial.println("Setup completed");
}
void loop() {
  //Sensor reading failed
   float t = dht.readTemperature();
   float h = dht.readHumidity();
        if (isnan(t) || isnan(h)) {
        Serial.println("");
      }
      else {
        //Print the values on the serial monitor
          Serial.println("Temp="+String(t)+"°C");
          Serial.println("Humedad="+String(h)+"%");
          //Write the values to ThingSpeak
          thingSpeakWrite(t,h);
      }
  //Thingspeak needs at least 15 seconds to update
  delay(16000);
}
