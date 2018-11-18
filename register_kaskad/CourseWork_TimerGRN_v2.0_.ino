#include <EnableInterrupt.h>  // подключаем библиотеки

// обьявляем переменнные
byte num[] = {0b01101111, // "0"
  0b00101000, // "1"
  0b01011101, // "2"
  0b01111100, // "3"
  0b00111010, // "4"
  0b01110110, // "5"
  0b01110111, // "6"
  0b00101100, // "7"
  0b01111111, // "8"
  0b01111110}; // "9"
int pinA = 8; // Пины прерываний энкодера
int pinB = 10; // Пины прерываний энкодера
int lastButton;
int mode = 11;


// Изменяемые переменные
int latch = 6; // Пин сдвигового регистра 12 st, защёлка
int clok = 7; // Пин сдвигового регистра 11 sh, тактируемый сигнал
int data = 5; // Пин сдвигового регистра 14 ds, данные
 int buzzer = 12; // пищалка
int Nkoder_output = 9; // Клавиша энкодера
volatile int count = 0; // Счетчик оборотов
volatile int actualcount = 0; // Временная переменная определяющая изменение основного счетчика

volatile int state = 0; // Статус одного шага - от 0 до 4 в одну сторону, от 0 до -4 - в другую

volatile int pinAValue = 0; // Переменные хранящие состояние пина, для экономии времени
volatile int pinBValue = 0; // Переменные хранящие состояние пина, для экономии времени

void setup()
{
  pinMode(latch, OUTPUT);
  pinMode(clok, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(Nkoder_output, INPUT);
  pinMode(mode, INPUT);
  
  pinMode(pinA, INPUT);           
  pinMode(pinB, INPUT);          

  enableInterrupt(pinA, A, CHANGE);
  enableInterrupt(pinB, B, CHANGE);

  randomSeed(analogRead(3));
  Serial.begin(9600);
}

void timerStart()
{
  outputNum(0); 
  interrupts();
  while(1){
    if(digitalRead(mode) == HIGH)
      return;
    enableInterrupt(pinA, A, CHANGE);
    enableInterrupt(pinB, B, CHANGE);
    Serial.println("timerStart()");
    if(digitalRead(Nkoder_output) == LOW)
      timer();
    if(digitalRead(mode) == HIGH)
      return;
  }
}

void randomNumStart()
{
  Serial.println("randomNumStart()");
  //disableInterrupt(pinA);
  //disableInterrupt(pinB);  
  randomNum();
   
}

void timer()
{
  Serial.println(count);
  disableInterrupt(pinA);
  disableInterrupt(pinB);
  if(count == 0) return;
  int i = count;
  int time;
  do
  {
    Serial.print("time: ");
    time = millis();
    Serial.println(time);
    Serial.println(i);
    i--;
    outputNum(i);
    //noInterrupts();
    if(digitalRead(mode) == HIGH) return;
    delay(990);
    //for(int a = 0; a <= 500; a++){for(int b = 0; b <= 500; b++){lastButton = micros();}}
    //interrupts();
    if(digitalRead(mode) == HIGH) return;
      
  }while(i != 0);
  endClockTime();
  enableInterrupt(pinA, A, CHANGE);
  enableInterrupt(pinB, B, CHANGE);
}

void randomNum()
{
  disableInterrupt(pinA);
  disableInterrupt(pinB);
  outputNum(0); 
  while(1)
  {
    if(digitalRead(mode) == LOW)
    return;
    if(digitalRead(Nkoder_output) == LOW)
    {
      // noInterrupts();
      //cli();
      
      Serial.println("randomNum");
      int number_Rand = random(99);
      Serial.println(number_Rand);
      outputNumRand(number_Rand);
      //interrupts();
      //sei();
     
    }
  }
   enableInterrupt(pinA, A, CHANGE);
   enableInterrupt(pinB, B, CHANGE);
}

void outputNumRand(int number)
{
  if(digitalRead(mode) == LOW)
    return;
  digitalWrite(latch, LOW);
  int num_St = number/10;
  int num_Ml = number - num_St*10;
  flipping(num_St,num_Ml, number); 
  shiftOut(data, clok, LSBFIRST, num[num_St]);
  shiftOut(data, clok, LSBFIRST, num[num_Ml]);
  digitalWrite(latch, HIGH);
  if(digitalRead(mode) == LOW)
    return;
}

int flipping(int stN, int mlN, int Num)
{
  int clk = 0;
  for(int i = 0; i <= stN; i++)
  {
    if(digitalRead(mode) == LOW)
       return;
    for(int j = 0; j <= 9; j++)
    {
      digitalWrite(latch, LOW);
      shiftOut(data, clok, LSBFIRST, num[i]);
      shiftOut(data, clok, LSBFIRST, num[j]);
      digitalWrite(latch, HIGH);
      //sei();
      delay(30 + (clk*100/Num)*2);
      //cli();
      if(clk == Num){
        endClockRandNum();
        return 0;
      } 
       clk++; 
      if(digitalRead(mode) == LOW)
         return; 
    }
  }
}

void outputNum(int number)
{
  digitalWrite(latch, LOW);
  int num_St = number/10;
  int num_Ml = number - num_St*10;
  shiftOut(data, clok, LSBFIRST, num[num_St]);
  shiftOut(data, clok, LSBFIRST, num[num_Ml]);
  digitalWrite(latch, HIGH);
}

void endClockTime()
{
  for(int i = 0; i < 5; i++)
  {
    digitalWrite(buzzer, HIGH);
    delay(400);
    digitalWrite(buzzer, LOW);
    delay(40);
  }
}

void endClockRandNum()
{
  for(int i = 0, j = 10; i < 7; i++, j--)
  {
    digitalWrite(buzzer, HIGH);
    delay(10 + i*10);
    digitalWrite(buzzer, LOW);
    delay(j);
  }
}

void A()
{
  pinAValue = digitalRead(pinA);
  pinBValue = digitalRead(pinB);

  cli();
  if (state == 0  && !pinAValue &&  pinBValue || state == 2  && pinAValue && !pinBValue) {
    state += 1; 
  }
  if (state == -1 && !pinAValue && !pinBValue || state == -3 && pinAValue &&  pinBValue) {
    state -= 1;
  }
  setCount(state);
  sei();

  if (pinAValue && pinBValue && state != 0) state = 0; // Если что-то пошло не так, возвращаем статус в исходное состояние
}
void B()
{
  pinAValue = digitalRead(pinA);
  pinBValue = digitalRead(pinB);

  cli();
  if (state == 1 && !pinAValue && !pinBValue || state == 3 && pinAValue && pinBValue)
  {
    state += 1; // Если выполняется условие, наращиваем переменную state
  }
  if (state == 0 && pinAValue && !pinBValue || state == -2 && !pinAValue && pinBValue)
  {
    state -= 1;
  }
  setCount(state);
  sei();
  
  if (pinAValue && pinBValue && state != 0)
    state = 0; 
}

void setCount(int state) {         
  if (state == 4 || state == -4)
  {  
    count += (int)(state / 4);
    if(count < 0)
       count = 99;
    else if(count > 99)
        count = 0;     
    outputNum(count);
  }
}


void loop()
{
  //outputNum(0);
  switch(digitalRead(mode)){
    case LOW: {timerStart(); break;}
    case HIGH: {randomNumStart();break;}
  }
  
  //timerStart();

}
