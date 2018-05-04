#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "light-test-f2c8e.firebaseio.com"
#define FIREBASE_AUTH "pM6onwRR2jlWHn6zLUv9qqOS8eVNnkdblrdX9iOy"
#define WIFI_SSID "qwer"
#define WIFI_PASSWORD "qqqqqqqq"

int buttonPin = D6;


void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

int interval = 100;
bool light ;

//Set Debounce
//    To prevent elders from continuously pressing button
//    Set time interval to 5 mins.
//    Can be alterd


unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 5000; //5 sec 3e5;   //5mins
int lastButtonState = LOW;
bool canPress = true;
int dbounce=0;  //debonce for internet

void loop() {
  //get data from firebase
//  Serial.println(dbounce);
  if (Firebase.getBool("alter") == true && dbounce >= 6 ) // in case the database hasn't change its state
  {
//    Serial.println("changed");
    light = Firebase.getBool("light");
    interval = Firebase.getInt("flash");
    Firebase.setBool("alter", false);
    dbounce = 0;
//    Serial.println(interval);
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
//  Serial.println(light);
  dbounce++;

  //send data to firebase;
//  Serial.print("canPress:");
//  Serial.println(canPress);
//  Serial.print("State:");
//  Serial.println(reading);
 
  int reading = digitalRead(buttonPin);
  Serial.print("Reading=");
  Serial.println(reading);
  if (canPress && reading == HIGH) {
    Serial.println("clicked");
    lastButtonState = reading;
    canPress = !canPress;
    lastDebounceTime = millis();
    // do things
    DynamicJsonBuffer jsonBuffer;
    JsonObject& clickEvent = jsonBuffer.createObject();
    clickEvent[".sv"] = "timestamp";
    Firebase.push("clicked", clickEvent);
  }else if(!canPress){
      Serial.print("Current time");
      Serial.println(millis());
      Serial.print("Last Debounce Time:");
      Serial.println(lastDebounceTime);

  if (millis() - lastDebounceTime >= debounceDelay) {
    Serial.println("click enabled");
    canPress = !canPress;
  } 
  }
      

}
