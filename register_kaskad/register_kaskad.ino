int latch = 6; // 12 st
int clok = 7; // 11 sh
int data = 5; // 14 ds
int buzzer = 12;
int Nkoder_output = 9;

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
  
void setup() {
  // put your setup code here, to run once:
  pinMode(latch, OUTPUT);
  pinMode(clok, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(Nkoder_output, INPUT);
  randomSeed(analogRead(3));
  Serial.begin(9600);
  
}

void timer99()
{
  while(digitalRead(Nkoder_output) == HIGH){
    digitalRead(Nkoder_output);
  }
  for(int i = 9; i >= 0; i--)
  {
    for(int j = 9; j >= 0; j--)
    {
      digitalWrite(latch, LOW);
      shiftOut(data, clok, LSBFIRST, num[i]);
      shiftOut(data, clok, LSBFIRST, num[j]);
      digitalWrite(latch, HIGH);
      if(digitalRead(Nkoder_output) == LOW){
        return;
      }
      delay(1000);
    }
    clock_Buzzer();
  }
  endClock();    
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

void clock_Buzzer()
{
  digitalWrite(buzzer, HIGH);
    delay(60);
    digitalWrite(buzzer, LOW);
    delay(60);
}

void return_num()
{
  shiftOut(data, clok, LSBFIRST, num[0]);
  shiftOut(data, clok, LSBFIRST, num[0]);
}

void randomNum()
{
  int number_Rand = random(99);
  Serial.println(number_Rand);
  outputNum(number_Rand);
}

void outputNum(int number)
{
  digitalWrite(latch, LOW);
  //int number = 89;
  //Serial.println(number);
  //delay(1000);
  int num_St = number/10;
  //Serial.println(num_St);
  //delay(1000);
  int num_Ml = number - num_St*10;
  //Serial.println(num_Ml);
  shiftOut(data, clok, LSBFIRST, num[num_St]);
  shiftOut(data, clok, LSBFIRST, num[num_Ml]);
  digitalWrite(latch, HIGH);
  delay(1000);
  
}


void loop()
{
  //return_num();
  //outputNum();
  //timer99();
  //endClock();
  randomNum();
}
