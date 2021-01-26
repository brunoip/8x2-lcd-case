
// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>    
#include <DHT.h>
           
#include "RTClib.h"

// Definimos el pin donde se conecta el sensor
#define DHTPIN A3
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

byte progress_bar_start_full[8] = {
  B00000,
  B01111,
  B11000,
  B10011,
  B10011,
  B11000,
  B01111,
};

byte progress_bar_start_empty[8] = {
  B00000,
  B01111,
  B11000,
  B10000,
  B10000,
  B11000,
  B01111,
};

byte progress_bar_middle_full[8] = {
  B00000,
  B11111,
  B00000,
  B11011,
  B11011,
  B00000,
  B11111,
};

byte progress_bar_middle_half[8] = {
  B00000,
  B11111,
  B00000,
  B11000,
  B11000,
  B00000,
  B11111,
};

byte progress_bar_middle_empty[8] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};

byte progress_bar_end_full[8] = {
  B00000,
  B11110,
  B00011,
  B11001,
  B11001,
  B00011,
  B11110,
};

byte progress_bar_end_empty[8] = {
  B00000,
  B11110,
  B00011,
  B00001,
  B00001,
  B00011,
  B11110,
};

byte progress_bar_test[8] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};

int mode=0;
int page=0;

bool needReleasePage=false;
bool needReleaseMode=false;
long waitTime=250;

//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2, buttonPin1=8, buttonPin2=9, buttonPin3=10;
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4, buttonPin1=14, buttonPin2=16;
int buttonState1 = 0,buttonState2 = 0;

const int buzzer = 15; //buzzer to arduino pin 16

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
RTC_DS1307 RTC;  
DateTime now;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buzzer, OUTPUT);

  lcd.createChar(0, progress_bar_start_full); 
  lcd.createChar(1, progress_bar_start_empty); 
  lcd.createChar(2, progress_bar_middle_empty);
  lcd.createChar(3, progress_bar_middle_half);  
  lcd.createChar(4, progress_bar_middle_full); 
  lcd.createChar(5, progress_bar_end_full); 
  lcd.createChar(6, progress_bar_test); 
  lcd.createChar(7, progress_bar_end_empty); 

  // set up the LCD's number of columns and rows:
  lcd.begin(8, 2);

  // Comenzamos el sensor DHT
  dht.begin();

  Serial.begin(115200);   

  beep();

  Wire.begin();
  RTC.begin();
  delay(5); 
  if (! RTC.isrunning())
  {
    // Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void beep(){
  tone(buzzer, 1000);
  delay(250); 
  noTone(buzzer);
}

void loop() 
{
  now = RTC.now();
  
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState1 == LOW) {
    if(needReleasePage)
    {
      needReleasePage=false;
      mode=0;
      page++;
      if(page>4)
        page=0;     
    }
  }
  else
  {
    needReleasePage=true;
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState2 == LOW) {
    if(needReleaseMode)
    {
      needReleaseMode=false;
      mode++;
      if(mode>1)
        mode=0;     
    }
  }
  else
  {
    needReleaseMode=true;
  }
  

  if(page==0)printCredits();
  if(page==1)printMiniHour();
  if(page==2)printDate();
  if(page==3)printHumedad();    
  if(page==4)printProgress();      
  
  delay(waitTime);
}

void printHumedad()
{
 // Leemos la humedad relativa
  int h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  int t = dht.readTemperature();
  // Leemos la temperatura en grados Fahreheit
  float f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f) )
  {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    lcd.setCursor(0, 0);
    lcd.print("Sensor  "); 
    lcd.setCursor(0, 1);
    lcd.print("Error   "); 
    return;
  }

  if(mode==0)
  {
    lcd.setCursor(0, 0);
    lcd.print("T:"); 
    lcd.print(t);
    lcd.print((char)223);
    lcd.print("C  ");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("T:"); 
    lcd.print((int)f);
    lcd.print((char)223);
    lcd.print("F  ");
  }
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%   "); 
}

void printMiniHour()
{
  lcd.setCursor(0, 0);
  lcd.print("Time:      "); 
  
  lcd.setCursor(0, 1);
  lcd.print(twoDigitInt(now.hour()));
  lcd.print(':'); 
  lcd.print(twoDigitInt(now.minute()));
  lcd.print(':'); 
  lcd.print(twoDigitInt(now.second()));
}

void printDate()
{
  lcd.setCursor(0, 0);
  lcd.print("Date:      "); 
  
  lcd.setCursor(0, 1); 
  lcd.print(twoDigitInt(now.day()));
  lcd.print('/'); 
  lcd.print(twoDigitInt(now.month()));
  lcd.print('/'); 
  lcd.print(now.year() % 100);
}

void printCredits()
{
  lcd.setCursor(0, 0);
  lcd.print("brunoip "); 
  lcd.setCursor(0, 1);
  lcd.print("    2018");
}

void printProgress()
{
  long yearNumber = DayNumber(now.year(), now.month(), now.day());
  long yearProgress = (yearNumber*100/365);
  
  long dayNumber = now.hour();
  long dayProgress = (dayNumber*100/24);
  
  long current_value = (yearNumber*14/365);
  lcd.setCursor(0, 0);
  lcd.print("Year ");
  lcd.print(twoDigitInt(yearProgress));
  lcd.print("%");
  
  if(mode==1)
  {
    current_value = (dayNumber*14/24);
    lcd.setCursor(0, 0);
    lcd.print("Day ");
    lcd.print(twoDigitInt(dayProgress));
    lcd.print("%    ");  
  }
  
 
    
  lcd.setCursor(0, 1);

  int progress_bar[8] ={1,2,2,2,2,2,2,7};

  if(current_value>=1) progress_bar[0] =0;
  
  if(current_value>=2) progress_bar[1] =3;
  if(current_value>=3) progress_bar[1] =4;
  
  if(current_value>=4) progress_bar[2] =3;
  if(current_value>=5) progress_bar[2] =4;
  
  if(current_value>=6) progress_bar[3] =3;
  if(current_value>=7) progress_bar[3] =4;
  
  if(current_value>=8) progress_bar[4] =3;
  if(current_value>=9) progress_bar[4] =4;
  
  if(current_value>=10) progress_bar[5] =3;
  if(current_value>=11) progress_bar[5] =4;
  
  if(current_value>=12) progress_bar[6] =3;
  if(current_value>=13) progress_bar[6] =4;
  
  if(current_value>=14) progress_bar[7] =5;

  lcd.write(byte(progress_bar[0]));
  lcd.write(byte(progress_bar[1]));
  lcd.write(byte(progress_bar[2]));
  lcd.write(byte(progress_bar[3]));
  lcd.write(byte(progress_bar[4]));
  lcd.write(byte(progress_bar[5]));
  lcd.write(byte(progress_bar[6]));
  lcd.write(byte(progress_bar[7]));
}

int DayNumber(unsigned int y, unsigned int m, unsigned int d){
 int days[]={0,31,59,90,120,151,181,212,243,273,304,334};    // Number of days at the beginning of the month in a not leap year.
//Start to calculate the number of day
 if (m==1 || m==2){
   return (days[(m-1)]+d);                     //for any type of year, it calculate the number of days for January or february
 }                        // Now, try to calculate for the other months
 else if ((y % 4 == 0 && y % 100 != 0) ||  y % 400 == 0){  //those are the conditions to have a leap year
   return (days[(m-1)]+d+1);     // if leap year, calculate in the same way but increasing one day
 }
 else {                                //if not a leap year, calculate in the normal way, such as January or February
   return (days[(m-1)]+d);
 }
}

String twoDigitInt(unsigned long x)
{
  if (x<=9) return ("0"+ (String) x);  
  return (String)  x;  
}
