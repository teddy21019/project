#include <Firebase.h>
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <Timer.h>
#include <Stepper.h>


#define FIREBASE_HOST "light-test-7d9fa.firebaseio.com"
#define FIREBASE_AUTH "mhMewzfKimcL7TTpjoEFRKltxjDScIgkCiNrXNWz"
#define WIFI_SSID "qwer"
#define WIFI_PASSWORD "qqqqqqqq"

int buttonPin = D1;
int buzzerPin = D2;
const float roatationRatio = 5.68892f;

bool openBoxFromServer = false;
bool canPress = false; 
bool canDetect = false;
bool canBuzz = false;
bool dbounceNet = true;
bool turnMotor = false;

int lastAngle = 0;
//enum med = {"off", "day", "niggt"};
int angle[3] = {0, 120, -120};

Timer t;
Stepper myStepper(roatationRatio*120, D8, D6, D7, D5);
int timerBuzzId, timerMotor, timerDetectId, timerNetworkDebounceID;

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

  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Set timer intiallization
  timerBuzzId = t.pulse(buzzerPin, 2e3, HIGH);
  myStepper.setSpeed(15);


  }

void loop() {
  openBoxFromServer = Firebase.getBool("open");
  int medicineToEat = Firebase.getInt("C8763");
  int reading =  digitalRead(buttonPin);
//  Serial.print("dbouncenet="); Serial.println(dbounceNet);
//  Serial.print("openBoxFromServer");Serial.println(openBoxFromServer);
  
  if (openBoxFromServer == true && dbounceNet == true){
//    Serial.println("canPress");
    t.stop(timerDetectId);
    canBuzz = true;
    canPress = true;
    canDetect = true;
  }
  Serial.println(canDetect);
  //servo motor part
  if (reading == HIGH) {
    if (canPress) {
      Serial.println("pressed");
      canBuzz = false;
      canPress = false;
      canDetect = false;
      turnMotor = true;
      dbounceNet=false;
      timerDetectId = t.after(0.25 * 60 * 1000, detectTimerCallBack, (void*)0);
      t.after(50 ,dbounceNetFunc, (void*)0);           //in case that the state hasn't been changed on server
      alterFireBaseClose();
      
    } else{
      if (canDetect) {
        uploadToFireBase();
        canDetect = false;
        timerDetectId = t.after(0.25 * 60 * 1000, detectTimerCallBack, (void*)0);
      }
    }
  }
  //buzzer
  if (canBuzz) {
//    buzz();
  }
  
//  Serial.println(turnMotor);
  if(turnMotor){
    int a  = angle[medicineToEat]-lastAngle;
    Serial.print("Into trun");
    Serial.println(angle[medicineToEat]);
    myStepper.step(a*roatationRatio);
    delay(1);
    lastAngle = angle[medicineToEat];
    
    turnMotor = false;
  }
  t.update();
  
}
void detectTimerCallBack(void *a) {
  canDetect = true;
}



void uploadToFireBase() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& clickEvent = jsonBuffer.createObject();
  clickEvent[".sv"] = "timestamp";
  Firebase.push("clicked", clickEvent);
}

void alterFireBaseClose() {
  Firebase.setBool("open", false);
}

void dbounceNetFunc(void *a){
  Serial.println("called db func");
  dbounceNet = true;
}
