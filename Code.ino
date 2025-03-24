#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  //main
LiquidCrystal_I2C lcd2(0x25, 16, 2);  // car amount
LiquidCrystal_I2C lcd3(0x26, 16, 2);  // income

Servo servo;
Servo servo2;
int servoPos;
int servoPos2;

#define sensorPin1 A2
#define sensorPin2 A3
#define sensorPin3 A1

#define buzzerPin 6

int senVal1 = 0;
int senVal2 = 0;
int senVal3 = 0;

#define RST_PIN 49
#define SS_PIN 53

int card1Balance = 2000;
int card2Balance = 400;

#define passSize 5
char MasterPass1[passSize] = "1234";
char MasterPass2[passSize] = "5678";
int count;
int income;
int lock = 0;

#define num 7
char Data[num];
byte data_count = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);

String num1, num2, card, card2;
int a, b;
char Key;
bool recharge = true;

int state = 0;
boolean state2 = true;
boolean state3 = true;

const byte ROWS =4;
const byte COLS =4;

char keys[ROWS][COLS] = {
  '1', '2', '3', 'A',
  '4', '5', '6', 'B',
  '7', '8', '9', 'C',
  '*', '0', '#', 'D'
};

byte rowPins[ROWS] = {22,23,24,25};
byte colPins[COLS] = {26,27,28,29};

Keypad KP2 = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


///////////////////////////////////////////////////////////////////
void setup() {
  lcd.init();
  lcd.backlight();
  lcd2.init();
  lcd2.backlight();
  lcd3.init();
  lcd3.backlight();

  servo.attach(9);
  servo.write(0);
  servo2.attach(10);
  servo2.write(180);

  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  pinMode(buzzerPin, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.print(" Automatic toll");
  lcd2.print(" Automatic toll");
  lcd3.print("Automatic toll:");
  lcd3.setCursor(0,1);
  lcd3.print(income);
  //delay(7000);
 // lcd.clear();
  //lcd2.clear();
  //lcd3.clear();

  Serial.begin(9600);

}
//////////////////////////////////////////////////
void loop()
{
  counting();

  if (recharge == 0)
  {
    reCharge();
  }
  else
  {
    
 lcd.setCursor(0, 0);
 lcd.print("   Welcome!!!");
 sensorRead();  
   rfid();
    KeyPad();
  if (senVal1 == 0){
    servoDown();
    
    lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle detected");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Put your card to");
      lcd.setCursor(0, 1);
      lcd.print("the reader......");
      delay(2000);
      lcd.clear();
    }
     else if (senVal2 == 0 && state == 1)
    {
      count--;
      lcd2.setCursor(0, 1);
      lcd2.print(count);
      servoUp();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Have a safe");
      lcd.setCursor(0, 1);
      lcd.print("journey");
      delay(1000);
      lcd.clear();
      state = 0;
    }
  
}
}
void sensorRead()
{
  senVal1 = digitalRead(sensorPin1);
  senVal2 = digitalRead(sensorPin2);
}

void servoDown()
{
  servo.attach(9);
   servoPos = 90; 
    servo.write(servoPos);
    

    servo2.attach(10);
   servoPos = 90; 
    servo2.write(servoPos);
   // count--;
  
}

void servoUp()
{
  servo.attach(9);
 servoPos = 0; 
    servo.write(servoPos);
    delay(5);

    servo2.attach(10);
 servoPos = 180; 
    servo2.write(servoPos);
    delay(5);
  
}


void counting(){

 if(!digitalRead(sensorPin3) && state3){
count++;
income++;
state3 = false;
delay(100);
 lcd2.setCursor(0, 0);
 lcd2.print("Remaining Cars:");
 lcd2.setCursor(0, 1);
 lcd2.print(count);

  lcd3.print("Todays update:");
  lcd3.setCursor(0,1);
  lcd3.print(income);
}
if(digitalRead(sensorPin3)){
  state3 = true;
  
} 
}

void rfid()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  if (content.substring(1) == "24 4F 3F 2B")
  {
    if (card1Balance >= 500)
    {
      lcdPrint();
      card1Balance = card1Balance - 500;
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      delay(2000);
      lcd.clear();
      state = 1;
    }
    else
    {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      lcd.print(" Tk");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please Recharge");
      delay(1000);
      lcd.clear();
      state = 0;
    }
  }
  else if (content.substring(1) == "A7 62 82 73")
  {
    if (card2Balance >= 500)
    {
      lcdPrint();
      card2Balance = card2Balance - 500;
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      delay(2000);
      lcd.clear();
      state = 1;
    }
    else
    {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      lcd.print(" Tk");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please Recharge");
      lcd.clear();
      delay(1000);
      state = 0;
    }
  }

  else   {
    digitalWrite(buzzerPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Unknown Vehicle");
    lcd.setCursor(0, 1);
    lcd.print("Access denied");
    delay(1500);
    lcd.clear();
    digitalWrite(buzzerPin, LOW);
  }
}
void lcdPrint()
{
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Successfully");
  lcd.setCursor(0, 1);
  lcd.print(" paid your bill");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Your Remaining");
  lcd.setCursor(0, 1);
  lcd.print("balance: ");
}
void LcdPrint()
{
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Your balance");
  lcd.setCursor(0, 1);
  lcd.print(" is insufficent");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Your Remaining");
  lcd.setCursor(0, 1);
  lcd.print("balance: ");
}

void KeyPad()
{
  Key = KP2.getKey();
    if (Key)
    {
      if (Key == 'A')
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Recharging Mode.");
        lcd.setCursor(0, 1);
        lcd.print("................");
        delay(1500);
        lcd.clear();
        recharge = 0;
}
    }
}

void reCharge(){
Key = KP2.getKey();
  if (Key)
    if (Key == 'D')
       if (card == "24 4F 3F 2B")
        card1Balance = 500 + card1Balance; 

   
       else if (card == "A7 62 82 73")
        card2Balance = 500 + card2Balance;

 recharge = 1;
  
}
/*
void reCharge(){
  Key = KP2.getKey();
  if (Key){
    if (Key == 'D'){
       if (card == "24 4F 3F 2B"){
        card1Balance = 500 + card1Balance;
        lcd.clear();
        lcd.print("Successfull");
        recharge = 1;
        }
        else if (card == "A7 62 82 73"){
          card2Balance =500 + card2Balance;
          lcd.clear();
        lcd.print("Successfull");
        recharge = 1;
          }
      }
    
  }  }

/*void reCharge(){

  lcd.setCursor(0,0);
  lcd.print("Enter Password");
  
  Key = KP2.getKey();

 if (card == "24 4F 3F 2B"){

  if(Key){
  Data[data_count] = Key;
  data_count++;
  }
if(data_count == passSize - 1){
  if(!strcmp(Data,MasterPass1)){
    lcd.clear();
    lcd.print("Password Match);
    lock = 1;
  }
  else{
  lcd.clear();
  lcd.print("Wrong Password");
  }
data_count = 0;
if(lock == 1){
  lcd.clear();
        lcd.print("Tap D to recharge 500 Taka");
    Key = KP2.getKey();
    if (Key)
    {
      if (Key == 'D')
      {
        lcd.clear();
        lcd.print("Successfully paid");
        card1Balance = 500 + card1Balance;
      }
      else{
        lcd.clear();
        lcd.print("Tap D to recharge 500 Taka");
      }
     }
}
lock = 0;
}
}
 
}

 
 
 }



 
 else if(card == "A7 62 82 73"){
  
 }
 else{
  
 }
        

*/
