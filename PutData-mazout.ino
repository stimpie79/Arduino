/*
  Read Utrasonic distance and send to remote url with GET parameters
  using an Arduino MKR WiFi 1010 and ultrasonic sensor JSN-SR04T (pins 6-7).
*/

#include <WiFiNINA.h>          // wifi
#include <ArduinoLowPower.h>   // sleep between measurements
#include <utility/wifi_drv.h>  // rgb led usage built into WiFiNINA - https://github.com/arduino-libraries/WiFiNINA/issues/24
#include <NewPing.h>           // to ping multiple times
//#include <SPI.h>             // replaced by newping

// setDebug enables led and serial output  (*bug: has to be enabled to run from powerbank?)
  int setDebug = 1; 
  unsigned long runnr = 0;

// sleep time between data entries in milliseconds
  const int interval = 30*60*1000;   // 30 minutes
  unsigned long startTime;           // keep the interval at 30 minutes by subtracting time to process code
  unsigned long endTime;             // keep the interval at 30 minutes by subtracting time to process code

// configure the RGB LEDs
  int green = 25;      // MKR1010 internal pin
  int red   = 26;      // MKR1010 internal pin
  int blue  = 27;      // MKR1010 internal pin
  
// definitions for WiFi
  const char ssid[] = "<<SSID>>";    // network SSID (name)
  const char pass[] = "<<PASSWD>>";    // network password (use for WPA, or use as key for WEP)
  int status = WL_IDLE_STATUS;
  WiFiClient client;

// website to post to
  const char server[] = "www.<<DOMAIN>>.be"; 
  char getStmt[256];

//definitions for the ultrasonic sensor
  #define TRIGGER_PIN  6      
  #define ECHO_PIN     7 
  #define MAX_DISTANCE 300    // calibration parameter
  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
  float duration, distance;
  const int iterations = 10;  // measure x times for better accuracy


void setup() {
   Serial.begin(9600);

  // configure the rgb output led
  WiFiDrv::pinMode(red,   OUTPUT);  
  WiFiDrv::pinMode(green, OUTPUT);  
  WiFiDrv::pinMode(blue,  OUTPUT);  

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    setDebug = 1;
    Serial.println("Communication with WiFi module failed!");
    fadeRed();
    while (true);
  }

  if (setDebug) {
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("Please upgrade the firmware");
    }
    ledGreen();
  }

  delay(15*1000);  // wait after reset to have a chance to upload a new sketch (not possible in sleep mode)

}



void loop() {
  startTime = millis();
  runnr++;
  if (setDebug) {ledOrange();}
  
  // check the wifi connection and reconnect if necessary
  connectWifi();
  delay(500); // to make sure the connection is started
  
  // Determine distance from duration of pings (distance/2 because of echo)
  // Use 343 metres per second as speed of sound
  duration = sonar.ping_median(iterations);
  distance = (duration / 2) * 0.0343;          //distance = sonar.convert_cm(duration);
  
  // GET url with parameters will be parsed by php file
  sprintf(getStmt,"""GET /Arduino/PutData.php?sensor=mazout&comment=%d&distance=%5.2f HTTP/1.0""",runnr,distance);

  // post the result to the server
  if (client.connect(server, 80)) {

    Serial.print("Connected to server: ");
    Serial.println(server);
    Serial.println(getStmt);
    if (setDebug) { ledGreen(); }
    
      
    // Make a HTTP request:
    client.println(getStmt);
    client.println("Host: www.<<DOMAIN>>.be");
    client.println("Connection: close");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println();
  }
  
  // if the server is disconnected, stop the client:
  if (!client.connected()) {
    client.stop();
  }

  if (setDebug) { fadeOut(); }
  Serial.println("Next execution in 30 minutes.");
  Serial.println("-----------------------------");

  // Powersaving settings 
  WiFi.disconnect();
  WiFi.end();                                    // disconnect and stop wifi
  status = WL_IDLE_STATUS; 
  endTime=millis();                              // time to process 1 run subtracted from interval to keep it at +- 30 minutes
  LowPower.sleep(interval-(endTime-startTime));  // turns everything off before starting the loop() cycle again.

} 


void connectWifi() {
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);
  if (setDebug) { ledBlue(); }
    
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.println(".");
    status = WiFi.begin(ssid, pass);
    delay(50);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
  Serial.println("--------------------------------------");  
}


void printWifiStatus() {
  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void ledOrange(){
  if (setDebug) {
    WiFiDrv::analogWrite(red,   255);
    WiFiDrv::analogWrite(green, 255);
    WiFiDrv::analogWrite(blue,    0);
  }
}

void fadeRed(){
  if (setDebug) {
    WiFiDrv::analogWrite(green,  0);
    WiFiDrv::analogWrite(blue,   0);
  
    for (int i = 0; i <= 255; i++) {
      WiFiDrv::analogWrite(red, i);
      delay(5);
    }
  }
}

void ledRed(){
  if (setDebug) {
    WiFiDrv::analogWrite(red,   255);
    WiFiDrv::analogWrite(green,   0);
    WiFiDrv::analogWrite(blue,    0);
  }
}

void ledGreen(){
  if (setDebug) {
    WiFiDrv::analogWrite(red,     0);
    WiFiDrv::analogWrite(green, 255);
    WiFiDrv::analogWrite(blue,    0);
  }
}

void ledBlue(){
  if (setDebug) {
    WiFiDrv::analogWrite(red,     0);
    WiFiDrv::analogWrite(green,   0);
    WiFiDrv::analogWrite(blue,  255);
  }
}

void ledWhite(){
  if (setDebug) {
    WiFiDrv::analogWrite(red,   255);
    WiFiDrv::analogWrite(green, 255);
    WiFiDrv::analogWrite(blue,  255);
  }
}
void fadeOut(){
  if (setDebug) {
    for (int i = 255; i >= 0; i--) {
      WiFiDrv::analogWrite(red,   0);
      WiFiDrv::analogWrite(green, 0);
      WiFiDrv::analogWrite(blue,  i);
      delay(5);
    }
  }
} 
