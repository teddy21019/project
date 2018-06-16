char notes[] = "baabBbBAGbbbcCcCBAGbbccccbaggggagagabbb ";
unsigned long beats[] = {1,1,1,3,1,1,1,2,5,
1,1,1,3,1,1,1,2,2,3,
1,2,2,1,1,1,1,1,1,4,
1,1,2,2,1,1,1,2,1,1,3}; 
int length = sizeof(notes); 
int tempo = 100;


void buzz() {
  if(canBuzz)
  {
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } else {
        Press();
        if(reading)
        {
          reading = 2;
          break;
         }
          
        playNote(buzzerPin,notes[i], beats[i] * tempo);
      } 
      delay(tempo/10); 
    }
  }
}

void playNote(int OutputPin, char note, unsigned long duration) {
  char names[] = { 'g','a','b','c', 'd', 'e', 'f', 'G', 'A', 'B', 'C', };
  int tones[] = { 784, 880, 988, 1046, 1175, 1319, 1397, 1568, 1760, 1976, 2093 };
  // 播放音符對應的頻率
  for (int i = 0; i < sizeof(names); i++) {
    if (names[i] == note) {
      tone(OutputPin,tones[i], duration);
      delay(duration);
      noTone(OutputPin);
    }
  }
}

void Press()
{
  reading =  digitalRead(buttonPin);
//          Serial.println("okok");
}
