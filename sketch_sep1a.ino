#include <stdio.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

long randNumber;
int button_values1[] = {920,587,427,340,270,215,180,110};
int button_values2[] = {920,427,340,270,215,180,110};
int analogValue0 = 0;
int analogValue1 = 0;
//int analogValue2 = 0;
int analogValue = 0;
int pin_p1 = A0;
int pin_p2 = A6;
int leds_cnt = 6;
int p0_leds[15] = {2,3,4,5,6,7,8,9,A1,A2,A3,A4,A5,0,1};
int p1_leds[6] = {2,3,4,5,8,9};
int p2_leds[6] = {A2,A3,A4,A5,0,1};
int p1_score = 0;
int p2_score = 0;
int pin_light = 0;
char buf[10];
bool numPlayer = false; //jeden gracz - false ,dwóch - true
int difSet = 0; //poziom trudności: 0- led świecą aż do naciśnięcia; 1- led gasną przed zapaleniem kolejnego; 2- 1+ ujemne punkty za nietrafienie; 3- 2+ ujemne punkkty za nie zgaszenie led; można to ogarnąć też inaczej:)
int p0_size = 0;
//int step_counter = 0;
int btn_tol = 25;
//int leds_cnt = 8;
//int p1_leds[8] = {2,3,4,5,6,7,8,9};
//int p2_leds[8] = {A1,A2,A3,A4,A5,0,1,1};
//int action_speed = 10;
//int action_speed_min = 5;

void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(A1));
  
  pinMode(pin_p1, INPUT);
  pinMode(pin_p2, INPUT);

  p0_size = sizeof(p0_leds)/sizeof(int);
  for (int i = 0; p0_size > i; i++) 
  {
    pinMode(p0_leds[i], OUTPUT);
    digitalWrite(p0_leds[i], 0);
  }
  
  //oczekiwanie na wybór trybu gry
  analogValue=analogRead(pin_p1);
  analogValue=analogValue+analogRead(pin_p2);
  while(analogValue<50 )
  {
    digitalWrite(p0_leds[pin_light], !digitalRead(p0_leds[pin_light]));
    delay(100);
    pin_light++;
    if(p0_size==pin_light)
      pin_light = 0;
    
    if (pin_light%2==1)
      analogValue=analogRead(pin_p1);
    else
      analogValue=analogRead(pin_p2);
  }
  pin_light = 0;
 
  

    

P.begin(1);

  P.displayText( "ABcdE", PA_LEFT, 1, 3000, PA_NO_EFFECT);
  //P.displayText(const char *pText, textPosition_t align, uint16_t speed, uint16_t pause, textEffect_t effectIn)
  P.displayAnimate();
  delay(500);  
  delay(500);  
  P.displayText( "ABcdE", PA_CENTER, 1, 3000, PA_NO_EFFECT);
  //P.displayText(const char *pText, textPosition_t align, uint16_t speed, uint16_t pause, textEffect_t effectIn)
  P.displayAnimate();
  delay(500);  
  delay(500);  
}

void loop() {

   /* step_counter++;
    bool step_action = false;
    if (step_counter > action_speed) {
      step_counter = 0;
      step_action = true;  
      action_speed = action_speed - round(action_speed/50);
      if (action_speed < action_speed_min) {
        action_speed = action_speed_min;
      }
     // Serial.println(action_speed);
    }*/
  //Serial.println(step_counter);
 /*   if (step_action) {
      int pin_light = random(0,6);
      //digitalWrite(p1_leds[pin_light], HIGH);
      for (int i = 0; leds_cnt - 1 > i; i++) {
        digitalWrite(p1_leds[i], LOW);
        digitalWrite(p2_leds[i], LOW);
      }
      digitalWrite(p1_leds[7], LOW);
      digitalWrite(p2_leds[pin_light], HIGH);
      
    }*/



analogValue0 = 0;
analogValue1 = 0;
analogValue = analogRead(pin_p2);
 if (analogValue > 50)
 {
 analogValue0 = analogValue;
 delay(5);  
 analogValue1 = analogRead(pin_p2);
 analogValue = (analogValue0+analogValue1)/2;
for (int i = 0; leds_cnt > i; i++) {
      if ( analogValue < button_values2[i] + btn_tol and analogValue > button_values2[i] - btn_tol ){
          if(digitalRead(p2_leds[i]) == HIGH){
            digitalWrite(p2_leds[i], LOW);
            p2_score++;
            //Serial.println(p2_score);
          }
        
        }
      }
 }
//Serial.println(analogValue);
 
    sprintf(buf, "%d - %d", p1_score, p2_score);
    P.displayClear();
    P.displayText(buf, PA_CENTER, 0, 0, PA_NO_EFFECT);

    P.displayAnimate();

 
  delay(150);  
}
