#include "HX711.h" 
 
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 3

#define LED1 12
#define LED2 11
#define LED3 10 
#define LED4 9 
#define LED5 8 

#define BUTTON 7
#define BUZZER 6

HX711 scale;

float calibration_factor = -215000;

float waterTot;
float waterGoal;

int start = 0;
int flag = 0;
int flagBuzzer=0;

int numTones = 7;
//int tones[] = {261, 277, 294, 311, 330, 349, 370, 392};
int tones[] = {261, 294, 330, 392, 330, 294, 261};

void setup() {
  Serial.begin(9600);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();       // 영점 조절

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  Serial.println("안녕하세요. 물 챙김이 비서입니다!");
  Serial.println("오늘 하루도 수분 가득한 하루 보내세요!\n");
  Serial.println("*** 하루 물 섭취 목표량 계산을 시작합니다 ***");
  calWaterGoal();
}

void loop() {
  scale.set_scale(calibration_factor);
  float weight = scale.get_units(10);

  if (abs(weight) >= 0.02)
  {
    start = 1;
    flag = 1;
  }
  else
  {
    flag = 0;
  }
  
  if (start == 1 && flag == 0)
  {
    waterTot += 0.2;
    start = 0;
  }

  Serial.print(" // water = ");
  Serial.print(abs(weight), 2);
  Serial.print(" kg "); 
  Serial.print(" // total water = ");
  Serial.print(waterTot);
  Serial.print(" kg");
  Serial.println();

  // 시리얼 모니터에 r 입력하면 몸무게와 체중 재입력 -> waterGoal 다시 계산
  if(Serial.read() == 'r')
  {
    Serial.println("*** 하루 물 섭취 목표량을 리셋합니다 ***");
    calWaterGoal();
  } 

  waterTotLED(waterTot, waterGoal);   // 누적된 물의 양에 따라 LED 켜기
  
  // 만약 리셋 버튼 누르면 waterTot 초기화
  if(digitalRead(BUTTON) == HIGH)
  {
    //waterTot = 0;
    //Serial.println("정상");

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
  }
}

// 하루 섭취 목표량 계산하기 
void calWaterGoal()
{ 
  waterGoal = 0;
  
  Serial.print("5초 이내로 키를 입력하세요."); delay(5000);
  float height = Serial.parseFloat();
  Serial.print("\t\t >> "); Serial.println(height);
  
  Serial.print("5초 이내로 몸무게를 입력하세요."); delay(5000);
  float weight = Serial.parseFloat();
  Serial.print("\t\t >> "); Serial.println(weight);

  waterGoal = (height + weight)/100;
  
  Serial.print("하루 목표 섭취량은 \t\t\t >> ");
  Serial.print(waterGoal); 
  Serial.println("kg");
} 

// 누적된 물의 양에 따라 LED 켜기 
void waterTotLED(float waterTot, float waterGoal) {
  float ratio = waterTot / waterGoal;
  
  if(ratio >= 0.2)
    digitalWrite(LED1, HIGH);
  if(ratio >= 0.4)
    digitalWrite(LED2, HIGH);
  if(ratio >= 0.6)
    digitalWrite(LED3, HIGH);
  if(ratio >= 0.8)
    digitalWrite(LED4, HIGH);
  if(ratio >= 1)
    digitalWrite(LED5, HIGH);

  Serial.print("ratio = ");
  Serial.print(ratio);
  Serial.print(" / flagBuzzer = ");
  Serial.print(flagBuzzer);
  Serial.print(" // ");

  // 목표량 달성 시 부저 울리기
  if(ratio >= 0.8 && ratio < 1)
    flagBuzzer = 0;
    
  if(ratio >= 1 and flagBuzzer == 0)
  {
    for(int i = 0; i < 7; i++) {
      tone(BUZZER, tones[i]);
      delay(300);
    }
    noTone(BUZZER);
    flagBuzzer = 1;
  }
}  
