#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal.h>
#include <elapsedMillis.h>

#define Password_Lenght 5 // Give enough room for six chars + NULL char
#define Input_pin 7
#define buzzer 6

elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "1234"; 
char Master2[Password_Lenght] = "0000"; 

char Menu[Password_Lenght] = "8888";
int entry_delay = 5000;
int exit_delay = 3000;
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
bool disarm_flag = false;
bool exit_loop = true;
bool Alaram_buff;
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {
  2,3,4,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  10,9,8}; //connect to the column pinouts of the keypad

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad 

void setup()
{
  lcd.begin(16, 2);
  pinMode(Input_pin,INPUT);
   pinMode(buzzer,OUTPUT);
   digitalWrite(buzzer,LOW);
}

void loop()
{
  lcd.setCursor(0,0);
  lcd.print("Ready to Arm");
  disarm_flag = false;
  getKey();
  if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
  {
    lcd.clear();
    lcd.setCursor(0, 0);

    if(!strcmp(Data, Master) || !strcmp(Data, Master2)) // equal to (strcmp(Data, Master) == 0)
    {
      clearData();  
      arm();
    }
    else if(!strcmp(Data, Menu))
    {
      menu();
      clearData(); 
      lcd.clear();
    }
    else
    {
      lcd.print("Bad Password");
      delay(2000);
      clearData(); 
      lcd.clear();
    }
  }
}

void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void arm()
{
  exit_loop = true;
  lcd.clear();
  lcd.print("System Arming");
  timeElapsed = 0;
  char Time[2];
  while (timeElapsed < exit_delay)
  {
    int T = (exit_delay - timeElapsed) / 1000;
    sprintf(Time,"%0.2d",T);
    lcd.setCursor(14,0);
    lcd.print(Time);
    //buzzer_beep(1000,1);
    bool status = authentication();
    if (status)
    {
      exit_loop = false;
      status = false;
      break;
    }
    else
    {
       exit_loop = true;
    }
  }
  lcd.clear();
  lcd.print("Arm");
while(exit_loop)
  {
    Alaram_buff = digitalRead(Input_pin);
    if(Alaram_buff == HIGH)
    {
      checkAlarm();
      break;
    }
    bool status = authentication();
    if (status)
    {
      break;
    }
  }
}

void disarm()
{
 // delay(entry_delay);
  lcd.print("Disarm");
}

void menu()
{
  lcd.clear();
  lcd.print("Menu");
  delay(2000);
}

void checkAlarm()
{
  char Time[2];
  lcd.clear();
  lcd.print("Input Key");
  timeElapsed = 0;
  while (timeElapsed < entry_delay)
  {
    int T = (entry_delay - timeElapsed) / 1000;
    sprintf(Time,"%0.2d",T);
    lcd.setCursor(14,0);
    lcd.print(Time);
    //buzzer_beep(500,2);
    bool status = authentication();
    if (status)
    {
      disarm_flag = true;
      status = false;
      break;
    }
  }
  if (!disarm_flag)
  {
    alarm();
    disarm_flag = false;
  }
}
void alarm()
{
  lcd.clear();
  lcd.print("Alarm");
  while(true)
  {
    bool status = authentication();
    if (status)
    {
      disarm_flag = false;
      status = false;
      break;
    }
  }
}

void getKey()
{
  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count,1); // move cursor to show each new char
    lcd.print("*"); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }
}

bool authentication()
{
  getKey();
  if(!strcmp(Data, Master) || !strcmp(Data, Master2) ) // equal to (strcmp(Data, Master) == 0)
  {
      lcd.clear();
      disarm();
      delay(1000);
      lcd.clear();
      clearData(); 
      return true;
  }
  else
  {
    return false;
  }
  
}

void buzzer_beep(int ms_delay,int count)
{
  for(int i=0;i<=count;i++)
  {
    digitalWrite(buzzer,HIGH);
    delay(ms_delay);
    digitalWrite(buzzer,LOW);
    delay(ms_delay);
  }
}