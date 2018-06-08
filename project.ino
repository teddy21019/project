#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set parameters to connect to firebase
#define FIREBASE_HOST "light-test-7d9fa.firebaseio.com"
#define FIREBASE_AUTH "mhMewzfKimcL7TTpjoEFRKltxjDScIgkCiNrXNWz"
#define WIFI_SSID "qwer"
#define WIFI_PASSWORD "qqqqqqqq"

int buttonPin = D6;     //must add D before pin number or else compile error


void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);       //WiFi is Defined in esp8266Wifi.h
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  pinMode(D0, OUTPUT);
  pinMode(buttonPin, INPUT); 
  pinMode(LED_BUILTIN, HIGH);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect arduino to firebase database

}

// Global variables
int interval = 100;           // initialize interval for flash
bool light ;                  // whether the light should blink

//Set Debounce
//    To prevent elders from continuously pressing button
//    Set time interval to 5 mins.
//    Can be alterd

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 5000;       // time until next time it can press
int lastButtonState = LOW;
bool canPress = true;
int dbounce=0;  //debonce for internet

void loop() {
  //get data from firebase
  if (Firebase.getBool("alter") == true && dbounce >= 6 ) // in case the database hasn't change its state
  {
    light = Firebase.getBool("light");
    interval = Firebase.getInt("flash");
    Firebase.setBool("alter", false);
    dbounce = 0;
  }

  if (light == true) {
    digitalWrite(D0, HIGH);
    delay(interval);
    digitalWrite(D0, LOW);
    delay(interval);
  }
  else if (light == false) {
    digitalWrite(D0, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }
  dbounce++;

  //send data to firebase while button pressed
 
  int reading = digitalRead(buttonPin);
      Serial.print(reading);
  if (canPress && reading == HIGH) {
    lastButtonState = reading;
    canPress = !canPress;
//    Serial.println(canPress);
    lastDebounceTime = millis();
    
    // do things
    DynamicJsonBuffer jsonBuffer;                             // create json object to push data to firebase
    JsonObject& clickEvent = jsonBuffer.createObject();
    clickEvent[".sv"] = "timestamp";                          // save system time
    Firebase.push("clicked", clickEvent);
    Serial.println("sended");
    }else if(!canPress){                                      // if one is unable to press
      if (millis() - lastDebounceTime >= debounceDelay) {     // if it is down to the reset time
        canPress = !canPress;                                 // toggle the state of canPress
//            Serial.println(canPress);

      } 
  }
      

}

