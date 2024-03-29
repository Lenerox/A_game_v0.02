/*_________________Gra na podstawie_________________:
https://github.com/DIYTechBros/ButtonSmack/blob/master/Smak_a_button.ino

Dodano:
-tryb jednoosobowy
-zróżnicowane poziomy trudności
-możliwość sprawdzenia działania przycisków (konieczny "serial monitor" i ustawienie "while" (odczytywanie wartosci wejsc) na true)

BUTTON MAP [LED]
button_values1
00 [3]  01 [2]    02      03    04
587     920       N       N     N

10 [4]  11 [5]    12 [6]  13    14
427     340       270     N     N

20 [9]  21 [8]    22 [7]  23    24
110     180       215     N     N

button_values2 
00    01    02    03    04
N     N     920   427   340

10    11    12    13    14
N     N     N     215   270

20    21    22    23    24
N     N     N     180   110
*/
#include <stdio.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

long randNumber;
int pin_light = 0;
int button_values1[] = {920,587,427,340,270,215,180,110};
int button_values2[] = {920,427,340,270,215,180,110};
int analogValue1 = 0;
int analogValue = 0;
int pin_p1 = A7;
int pin_p2 = A6;
int p0_leds[15] = {2,3,4,5,6,7,8,9,A1,A2,A3,A4,A5,A0,12};
int p0_size = 0;
int p1_leds[8] = {2,3,4,5,6,7,8,9};
int p2_leds[7] = {A1,A2,A3,A4,A5,A0,12};
int leds_cnt = 6;
int p1_score = 0;
int p2_score = 0;
int difSet = 0; //poziom trudności: 0- led świecą aż do naciśnięcia; 1- led gasną przed zapaleniem kolejnego; 2- 1+ ujemne punkty za nietrafienie; 3- 2+ ujemne punkkty za nie zgaszenie led; 4- 3+ ujemne punkty
int step_counter = 0;
int btn_tol = 23;
int action_speed = 200;
int action_speed_min = 50;
int limCounter1=0;
int limCounter2=0;
char buf[10];
bool step_action = false;
bool twoPlayers = false; //jeden gracz - false ,dwóch - true
bool bt1_press=false;
bool bt2_press=false;
bool bt01_press[8]={false,false,false,false,false,false,false,false};
bool bt02_press[8]={false,false,false,false,false,false,false,false};

void setup() {
  Serial.begin(9600);
  P.begin(1);
  P.displayClear();
  randomSeed(analogRead(A1));
  
  //inicjalizacja przycisków
  pinMode(pin_p1, INPUT);
  pinMode(pin_p2, INPUT);

  //inicjalizacja led
  p0_size = sizeof(p0_leds)/sizeof(int);
  for (int i = 0; p0_size > i; i++) 
  {
    pinMode(p0_leds[i], OUTPUT);
    digitalWrite(p0_leds[i], 0);
  }
  while(false)//odczytywanie wartosci wejsc
  {
    p1_score=analogRead(pin_p1);
    sprintf(buf, "%d /", p1_score);
    Serial.write(buf);
    delay(500);
  }

  //oczekiwanie na wybór trybu gry
  analogValue=analogRead(pin_p1);
  delay(5);
  analogValue1=analogRead(pin_p2);
  while(analogValue<50 and analogValue1<50)
  {
    step_counter++;
    if (step_counter>59)
    {
      step_counter=0;
      digitalWrite(p0_leds[pin_light], !digitalRead(p0_leds[pin_light]));
      pin_light++;
    }
    if(p0_size==pin_light)
      pin_light = 0;
    analogValue=analogRead(pin_p1);
    delay(5);
    analogValue1=analogRead(pin_p2);
  }
  pin_light = 0;
 
 //gaszenie led
  for (int i = 0; p0_size > i; i++) 
  {
    digitalWrite(p0_leds[i], 0);
  }

  //ustalanie wybranego trybu gry
  gameSet();

  //rozpoczęcie gry
  if (twoPlayers == true)
    P.displayText( "1 VS 1", PA_CENTER, P.getSpeed(), 3000, PA_SCROLL_UP);
  else
    P.displayText( "1 GRACZ", PA_CENTER, P.getSpeed(), 3000, PA_SCROLL_LEFT);
    animDisp();
}

void loop() {
  if (bt1_press==false and bt2_press==false)
    step_counter++;
  step_action = false;
  if (step_counter > action_speed) 
  {
    step_counter = 0;
    step_action = true;  
    action_speed = action_speed - round(action_speed/50);
    if (action_speed < action_speed_min)
        action_speed = action_speed_min;
  }

  if (twoPlayers == true)//2 graczy
  {
    if (step_action) 
    {
      int pin_light = random(0,6);
      for (int i = 0; leds_cnt > i; i++)
      { 
        bt01_press[i]= false;
        bt02_press[i]= false;
      }
      if (difSet>0)//gaszenie led z poprzedniego losowania
      {
        for (int i = 0; leds_cnt > i; i++) 
        {
          //odejmowanie punktów za niezgaszenie led przed kolejnym losowaniem
          if (digitalRead(p1_leds[i]) == 1 and difSet>2 and !(i==pin_light))
            p1_score--;
          if (digitalRead(p2_leds[i]) == 1 and difSet>2 and !(i==pin_light))
            p2_score--;
          digitalWrite(p1_leds[i], LOW);
          digitalWrite(p2_leds[i], LOW);
        }
      }
      digitalWrite(p1_leds[pin_light], HIGH);
      digitalWrite(p2_leds[pin_light], HIGH);
    }
    game_2();
    if (difSet<4 and p1_score<0)
      p1_score=0;
    if (difSet<4 and p2_score<0)
      p2_score=0;
    sprintf(buf, "%d:%d", p1_score, p2_score);
  }
  else //1 gracz
  {
    if (step_action) 
    {
      int pin_light = random(0,  p0_size);
      for (int i = 0; 8 > i; i++)
      { 
        bt01_press[i]= false;
        bt02_press[i]= false;
      }
      if (difSet>0)
      {
        for (int i = 0; p0_size > i; i++) 
        {
          if (digitalRead(p0_leds[i]) == 1 and difSet>2 and !(i==pin_light))
            p1_score--;
          digitalWrite(p0_leds[i], LOW);
        }
      }
      digitalWrite(p0_leds[pin_light], HIGH);
    }
    game_1();
    if (difSet<4 and p1_score<0)
      p1_score=0;
    sprintf(buf, "%d", p1_score);
  }

  //zabezpieczenie przed długim trzymaniem przycisku
  if (limCounter1>round(action_speed_min/3))
  {
    limCounter1=0;
    bt1_press=false;
  }
  if (limCounter2>round(action_speed_min/3))
  {
    limCounter2=0;
    bt2_press=false;
  }

  //wyświetlanie punktów
  P.displayText(buf, PA_CENTER, 0, 0, PA_NO_EFFECT);
  P.displayAnimate();

  //endgame
  if (p1_score>99)
  {
    P.displayText("<<WIN<<", PA_CENTER, 0, 0, PA_SCROLL_LEFT);
    p1_score=0;
    p2_score=0;
    animDisp();
    difSet++;
    action_speed=action_speed + action_speed_min + action_speed_min;
    action_speed_min=action_speed_min-5;
    if (action_speed_min<10)
      action_speed_min=10;
    animDisp(); 
  }
  if (p2_score>99)
  {
    P.displayText(">>WIN>>", PA_CENTER, 0, 0, PA_SCROLL_RIGHT);
    p1_score=0;
    p2_score=0;
    animDisp();   
    difSet++;
    action_speed=action_speed + action_speed_min + action_speed_min;
    action_speed_min=action_speed_min-5;
    if (action_speed_min<10)
      action_speed_min=10;
    animDisp(); 
  }
}

void game_1()
{
  analogValue1 = 0;
  analogValue = analogRead(pin_p2);
  if (analogValue > 50 and bt2_press==false)
  {
    analogValue1 = analogRead(pin_p2);
    if (analogValue>analogValue1)
      analogValue1=analogValue; 
    analogValue = (analogValue+analogValue1)/2;
    for (int i = 0; 7 > i; i++) 
    {
      if ( analogValue < button_values2[i] + btn_tol and analogValue > button_values2[i] - btn_tol and bt02_press[i]==false and bt2_press==false)
      {
        bt02_press[i]=true;
        bt2_press=true;
        if(digitalRead(p2_leds[i]) == HIGH) 
        {
          p1_score++;
          digitalWrite(p2_leds[i], LOW);
        }
        else if (difSet>1)
        {
          p1_score--;
        }
      }
    }
  }
  else
    limCounter2++;
  delay(5); 
  analogValue = analogRead(pin_p1);
  if (analogValue > 50 and bt1_press==false)
  {
    //delay(5);  
    analogValue1 = analogRead(pin_p1);
    if (analogValue>analogValue1)
      analogValue1=analogValue;
    analogValue = (analogValue+analogValue1)/2;
    for (int i = 0; 8 > i; i++) 
    {
      if ( analogValue < button_values1[i] + btn_tol and analogValue > button_values1[i] - btn_tol and bt01_press[i]==false and bt1_press==false)
      {
        bt01_press[i]=true;
        bt1_press=true;
        if(digitalRead(p1_leds[i]) == HIGH) 
        {
          digitalWrite(p1_leds[i], LOW);
          p1_score++;
          /* while (analogValue < button_values1[i] + btn_tol and analogValue > button_values1[i] - btn_tol)
          {
            analogValue = analogRead(pin_p1);
            delay(10); 
          } */
        }
        else if (difSet>1 and step_counter>20)
        {
          p1_score--;
        }
      }
    }
  }
  else
    limCounter1++;
}
void game_2()
{
  //gracz nr 2
  analogValue1 = 0;
  analogValue = analogRead(pin_p2);
  if (analogValue > 50 and bt2_press==false)//sprawdzanie czy naciśnięto przycisk
  {
    analogValue1 = analogRead(pin_p2);
    if (analogValue>analogValue1)
      analogValue1=analogValue;
    analogValue = (analogValue + analogValue1)/2;//obliczanie średniej z 2 pomiarów w celu pewniejszego ustalenia naciśniętego przycisku
    for (int i = 0; leds_cnt > i; i++) 
    {
      if ( analogValue < button_values2[i] + btn_tol and analogValue > button_values2[i] - btn_tol and bt02_press[i]==false and bt2_press==false)//ustalanie który przycisk został naciśnięty
      {
        //for (int j = 0; leds_cnt > j; j++)
        //  bt02_press[j]= false;
        bt02_press[i]=true;
        bt2_press=true;
        if(digitalRead(p2_leds[i]) == HIGH)
        {
          digitalWrite(p2_leds[i], LOW);
          p2_score++;
        }
        else if (difSet>1)
          p2_score--;
      }
    }
  }
  else
    limCounter2++;
  delay(5); 
    sprintf(buf, "%d:%d - ", p1_score, p2_score);
  Serial.print(buf);
  sprintf(buf, "%d",   analogValue);
  Serial.println(buf);
  //gracz nr 1
  analogValue1 = 0;
  analogValue = analogRead(pin_p1);
  if (analogValue > 50 and bt1_press==false)
  {
    //delay(5);  
    analogValue1 = analogRead(pin_p1);
    if (analogValue>analogValue1)
      analogValue1=analogValue; 
    analogValue = (analogValue+analogValue1)/2;
    for (int i = 0; leds_cnt > i; i++) 
    {
      if ( analogValue < button_values1[i] + btn_tol and analogValue > button_values1[i] - btn_tol and bt01_press[i]==false and bt1_press==false)
      {
        bt01_press[i]=true;
        bt1_press=true;
        if(digitalRead(p1_leds[i]) == HIGH)
        {
          digitalWrite(p1_leds[i], LOW);
          p1_score++;
        }
        else if (difSet>1)
          p1_score--;
      }
    }
  }
  else
    limCounter1++;
}
void gameSet()//ustalanie wybranego trybu gry
{
  //1 gracz
  if ( analogValue < button_values1[0] + btn_tol and analogValue > button_values1[0] - btn_tol )//bt01
  {
    difSet = 1;
  }
  else if ( analogValue1 < button_values2[0] + btn_tol and analogValue1 > button_values2[0] - btn_tol )//bt02
  {
    difSet = 2;
  } 
  else if ( analogValue1 < button_values2[1] + btn_tol and analogValue1 > button_values2[1] - btn_tol )//bt03
  {
    difSet = 3;
  } 
  else if ( analogValue1 < button_values2[2] + btn_tol and analogValue1 > button_values2[2] - btn_tol )//bt04
  {
    difSet = 4;
  } 
  //2 graczy 
  else if ( analogValue < button_values1[2] + btn_tol and analogValue > button_values1[2] - btn_tol )//bt10
  {
    difSet = 0;
    but_value();
  }
  else if ( analogValue < button_values1[3] + btn_tol and analogValue > button_values1[3] - btn_tol )//bt11
  {
    difSet = 1;
    but_value();
  }
  else if ( analogValue < button_values1[4] + btn_tol and analogValue > button_values1[4] - btn_tol )//bt12
  {
    difSet = 2;
    but_value();
  } 
  else if ( analogValue1 < button_values2[4] + btn_tol and analogValue1 > button_values2[4] - btn_tol )//bt13
  {
    difSet = 3;
    but_value();
  } 
  else if ( analogValue1 < button_values2[3] + btn_tol and analogValue1 > button_values2[3] - btn_tol )//bt14
  {
    difSet = 4;
    but_value();
  } 
}

void but_value()//ograniczenie rozmiaru tablic do "6"
{
  twoPlayers = true;
  button_values1[4]=button_values1[6];
  button_values1[5]=button_values1[7];
  p1_leds[4]=p1_leds[6];
  p1_leds[5]=p1_leds[7];
  for (int i = 0; leds_cnt>i;i++)
  {
    button_values2[i]=button_values2[i+1];
    p2_leds[i]=p2_leds[i+1];
  }
}

void animDisp()//animacja wyświetlacza
{
  P.displayAnimate();
  for (int i =0; i<200;i++)
    {
      delay(15);P.displayAnimate();
    }
}