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
volatile int latch = 6; // Пин сдвигового регистра 12 st, защёлка
volatile int clok = 7; // Пин сдвигового регистра 11 sh, тактируемый сигнал
volatile int data = 5; // Пин сдвигового регистра 14 ds, данные
volatile int buzzer = 12; // пищалка
volatile int Nkoder_output = 9; // Клавиша энкодера
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

  enableInterrupt(mode, randomNumStart, FALLING);
  enableInterrupt(mode, timerStart, RISING);

  enableInterrupt(8, A, CHANGE);
  enableInterrupt(10, B, CHANGE);

  randomSeed(analogRead(3));
  Serial.begin(9600);
}

void timerStart()
{
  Serial.println("timerStart()");
  while(1)
  {
    if(digitalRead(Nkoder_output) == LOW)
    {
      timer();
    }
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
  Serial.println(actualcount);
  //disableInterrupt(pinA);
  //disableInterrupt(pinB);
  if(actualcount == 0) return;
  volatile int i = actualcount;
  do
  {
    Serial.println(i);
    i--;
    outputNum(i);
    cli();
    for(int a = 0; a <= 500; a++){for(int b = 0; b <= 500; b++){lastButton = micros();}}
    sei();
  }while(i != 0);
  endClock();
}

void randomNum()
{
  while(1)
  {
    if(digitalRead(Nkoder_output) == LOW)
    {
      Serial.println("randomNum");
      int number_Rand = random(99);
      Serial.println(number_Rand);
      outputNumRand(number_Rand);
    }
  }
}

void outputNumRand(int number)
{
  digitalWrite(latch, LOW);
  int num_St = number/10;
  int num_Ml = number - num_St*10;
  flipping(num_St,num_Ml, number); 
  shiftOut(data, clok, LSBFIRST, num[num_St]);
  shiftOut(data, clok, LSBFIRST, num[num_Ml]);
  digitalWrite(latch, HIGH);
}

int flipping(int stN, int mlN, int Num)
{
  int clk = 0;
  cli();
  for(int i = 0; i <= stN; i++)
  {
    for(int j = 0; j <= 9; j++)
    {
      digitalWrite(latch, LOW);
      shiftOut(data, clok, LSBFIRST, num[i]);
      shiftOut(data, clok, LSBFIRST, num[j]);
      digitalWrite(latch, HIGH);
      delay(50 + (clk*100/Num)*4);
      if(clk == Num){
        endClock();
        return 0;
      } 
       clk++;  
    }
  }
  sei();
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

void endClock()
{
  for(int i = 0; i < 7; i++)
  {
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    delay(300);
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
    outputNum(count);
  }
}


void loop()
{
  if (actualcount != count) {     // Чтобы не загружать ненужным выводом в Serial, выводим состояние
    actualcount = count;          // счетчика только в момент изменения
    Serial.println(actualcount);
  }
  
}
