//****************************************************************
// Clock/calendar based on salvaged Gotek LED board with TM1651 chip
//
// Author:	  Alex Wierzbowsky
// Mail:	  	wierzbowsky@rbsc.su
// Hardware:	Re-used Gotek 3-digit LED screens on custom boards
// Board:   	Arduino UNO R2
// IDE:       Arduino
// Function:	Clock and calendar
// Date:		  04.03.2023
//
// Portions:  Paul Brace - Feb 2021
//
// Portions:  Detlef Giessmann Germany 
// Mail:      derek_cooper@hotmail.com
// Comment:   Library to drive LEDC68 Gotek 3 digit LED display
// IDE:       Arduino-1.8.12
//
//****************************************************************

//#include <SoftwareSerial.h>
//#include <Wire.h>
#include "GotekSaver.h"

// Define pins
#define Btnpin 13   // button pin
#define Buzzpin 12  // buzzer pin
#define Clockpin 2  // clock pin
#define Maxchars 9  // max characters on a display
#define Dispchars 3 // width of display

// Variables
uint8_t hour;
uint8_t minute;
uint8_t second;
uint8_t millisecond;
uint8_t date;
uint8_t month;
uint8_t year;
uint8_t currentbrt = 2;
unsigned long timer;
uint8_t beep = 0;
uint8_t autobrt = 0;
signed long adjust = 0;  // ADJUST for inaccurate Arduino's clock
char buffer[Maxchars+2];
uint8_t currentMode = 0;  // Turns to 1 when time/date is set
volatile unsigned long currentTime;    //   Duration in milliseconds from midnight
unsigned long pushdelay = 0;


//******************************************
// Setup
//
void setup()
{
 // Set up  time interrupt - millis() rolls over after 50 days so
  // we are using our own millisecond counter which we can reset at
 // the   end of each day
 TCCR0A = (1 << WGM01);      //Set the CTC mode Compare time   and trigger interrupt
 OCR0A = 0xF9;               //Set value for time to compare   to ORC0A for 1ms = 249  (8 bits so max is 256)
 //[(Clock speed/Prescaler value)*Time   in seconds] - 1
 //[(16,000,000/64) * .001] - 1 = 249 = 1 millisecond
 TIMSK0   |= (1 << OCIE0A);    //set timer compare interrupt
 TCCR0B |= (1 << CS01);      //Set   the prescale 1/64 clock
 TCCR0B |= (1 << CS00);      // ie 110 for last 3 bits
  TCNT0  = 0;                 //initialize counter value to 0
 sei();                      //Enable   interrupt

 pinMode(Clockpin,OUTPUT);
 pinMode(Buzzpin,OUTPUT);
 pinMode(Btnpin,INPUT_PULLUP);

 for(Datapin = 3; Datapin < 9; Datapin++)
 {
  pinMode(Datapin,OUTPUT);
  displayOn();
  displaySet(currentbrt);
 }

 // Defaults
 hour=0;
 minute=0;
 second=0;
 date=01;
 month=01;
 year=23;
 currentMode = 0;
 
 DisplayTime();
 DisplayDate();
 interrupts();
}

//******************************************
//
// This is interrupt is called when   the compare time has been reached
// hence will be called once a millisecond   based on the
// OCR0A register setting.
ISR(TIMER0_COMPA_vect) {
  if (!currentMode) currentTime++;
}


//******************************************
// MAIN LOOP
//
void loop()
{
 int result;
 long elapsed;
  
 timer = currentTime;
 while(1)
 {
  if (adjust >= 0) elapsed = currentTime - timer + adjust;
  else elapsed = currentTime - timer - abs(adjust);
  if(elapsed >= 1000)
  {
   //Serial.print(hour);
   //Serial.print(":");
   //Serial.print(minute);
   //Serial.print(":");
   //Serial.println(second);
   //Serial.flush();
   
   delay(100);
   timer = currentTime;
   ClockTick();

   // Pulse dashes in clock
   if(second%2)
   {
    Datapin=4;
    displaySpChar(0,Minus);
    Datapin=5;
    displaySpChar(0,Minus);
   }
   else
   {
    Datapin=4;
    displaySpChar(0,Space);
    Datapin=5;
    displaySpChar(0,Space);
   }
  }
  delay(100);
  if (result = ButtonCheck())
  {
   switch(result)
   {
    case 1:
    {
     tone(Buzzpin,3000,100);
     if(currentbrt == 7) currentbrt = 0;
     else if(currentbrt == 0) currentbrt = 2;
     else if(currentbrt == 2) currentbrt = 7;
     autobrt = 0;
     DisplayTime();
     DisplayDate();
     break;
    }
    case 2:
    {
     tone(Buzzpin,3000,250);
     AdjustDateTime();
     break;
    }
    case 3:
    {
     tone(Buzzpin,5000,100);
     // Auto-control brightness
     autobrt = 1;
     for(Datapin = 3; Datapin < 9; Datapin++)
     {
      if(hour > 9 and hour < 18)
      {
       currentbrt = 2;
       displaySet(currentbrt);
      }
      else
      {
       currentbrt = 0;
       displaySet(currentbrt);
      }
     }
     DisplayTime();
     DisplayDate();
     break;
    }
   }
  }
 }
}

//******************************************
// Adjust date/time
//
void AdjustDateTime(void)
{
 uint16_t cnt;
 uint8_t shift;
 uint8_t param = 0;
 uint8_t set = 0;
 int result;
 uint8_t leapyear=0;

 // Adjust time
 Datapin=4;
 displaySpChar(0,Space);
 Datapin=5;
 displaySpChar(0,Space);
 
 sprintf(buffer, " SET TIME");
 Datapin = 8;
 PrintLine();

 param = 0;
 Datapin = 5;
 cnt = 0;
 while(!set)
 {
  delay(100);
  if(result = ButtonCheck())
  {
   switch(result)
   {
    case 1:
    {
     tone(Buzzpin,3000,50);
     if(!param) second = 0;
     else if(param == 1)
     {
      minute++;
      if(minute == 60) minute = 0;
     }
     else if(param == 2)
     {
      hour++;
      if(hour == 24) hour = 0;
     }
     cnt = 1000;
     break;
    }
    case 2:
    {
     tone(Buzzpin,3000,150);
     param++;
     if(param > 2) param = 0;
     cnt = 1000;
     break;
    }
    case 3:
    {
     tone(Buzzpin,3000,250);
     set = 1;
     break;
    }    
   }
  }
  cnt++;
  if(cnt == 500)
  {
   Datapin = 5 - param;
   displayClear();
  }
  else if(cnt >= 1000)
  {
   DisplayTime();
   cnt = 0;
  }
 }
 DisplayTime();
 DisplayDate();

 // Adjust date
 sprintf(buffer, " SET DATE");
 Datapin = 5;
 PrintLine();

 param = 0;
 Datapin = 8;
 cnt = 0;
 set = 0;
 while(!set)
 {
  delay(100);
  if(result = ButtonCheck())
  {
   switch(result)
   {
    case 1:
    {
     tone(Buzzpin,3000,50);
     if(!param)
     {
      year++;
      if(year > 50) year = 20;
     }
     else if(param == 1)
     {
      month++;
      if(month == 13) month = 1;
     }
     else if(param == 2)
     {
      if(year%4==0 and ((year%400==0) or (year%100!=0))) leapyear=1;
      if((month==4 or month==6 or month==9 or month==11) and date==30) date=1;
      else if(month==2 and ((leapyear==0 and date==28) or (leapyear==1 and date==29))) date=1;
      else date++;
      if(date==32) date=1;
     }
     cnt = 1000;
     break;
    }
    case 2:
    {
     tone(Buzzpin,3000,150);
     param++;
     if(param > 2) param = 0;
     cnt = 1000;
     break;
    }
    case 3:
    {
     tone(Buzzpin,3000,250);
     set = 1;
     break;
    }    
   }
  }
  cnt++;
  if(cnt == 500)
  {
   Datapin = 8 - param;
   displayClear();
  }
  else if(cnt >= 1000)
  {
   DisplayDate();
   cnt = 0;
  }
 }

 // Adjust beep
 sprintf(buffer, " SET BEEP");
 Datapin = 5;
 PrintLine();
 if (beep) sprintf(buffer, "       ON");
 else sprintf(buffer, "      OFF");
 Datapin = 8;
 PrintLine(); 
 
 cnt = 0;
 set = 0;
 while(!set)
 {
  delay(100);
  if(result = ButtonCheck())
  {
   switch(result)
   {
    case 1:
    {
     tone(Buzzpin,3000,50);
     if(!beep) beep = 1;
     else beep = 0;
     cnt = 1000;     
     break;
    }
    case 3:
    {
     tone(Buzzpin,3000,250);
     set = 1;
     break;
    }    
   }
  }
  cnt++;
  if(cnt == 500)
  {
   Datapin = 8;
   displayClear();
  }
  else if(cnt >= 1000)
  {
   if (beep) sprintf(buffer, "       ON");
   else sprintf(buffer, "      OFF");
   Datapin = 8;
   PrintLine(); 
   cnt = 0;
  }
 }
 
 // Set adjust
 sprintf(buffer, "   ADJUST");
 Datapin = 5;
 PrintLine();
 if (adjust >= 0) sprintf(buffer, "      %03d", adjust);
 else sprintf(buffer, "     -%03d", adjust);
 Datapin = 8;
 PrintLine(); 
 
 cnt = 0;
 set = 0;
 while(!set)
 {
  delay(100);
  if(result = ButtonCheck())
  {
   switch(result)
   {
    case 1:
    {
     currentMode = 1; // stop count
     tone(Buzzpin,3000,50);
     adjust++;
     if(adjust > 15) adjust = -15;
     cnt = 1000;
     currentMode = 0; // resume count
     break;
    }
    case 3:
    {
     tone(Buzzpin,3000,250);
     set = 1;
     break;
    }    
   }
  }
  cnt++;
  if(cnt == 500)
  {
   Datapin = 8;
   displayClear();
  }
  else if(cnt >= 1000)
  {
   if (adjust >= 0) sprintf(buffer, "      %03d", adjust);
   else sprintf(buffer, "     -%03d", abs(adjust));
   Datapin = 8;
   PrintLine(); 
   cnt = 0;
  }
 }
 
 currentMode = 0; // Normal mode
 DisplayTime();
 DisplayDate();
}


//******************************************
// Prints a text line on a display from buffer
// Set Datapin for rightmost screen's pin number
//
void PrintLine()
{
 uint16_t cnt;
 uint8_t shift;

 shift = Maxchars-1;
 for(cnt = 0; cnt < Dispchars; cnt++)
 {
  displayChar(0, buffer[shift-2]);
  displayChar(1, buffer[shift-1]);
  displayChar(2, buffer[shift]);
  Datapin--;
  shift = shift - Dispchars;
 }
}

//******************************************
// Display time
//
void DisplayTime()
{
 uint8_t cnt;
 uint8_t shift;
  
 sprintf(buffer, " %02d %02d %02d",hour,minute,second);
 Datapin = 5;
 displaySet(currentbrt);
 PrintLine();
}

//******************************************
// Display date
//
void DisplayDate()
{
 char monthname[4];
 uint8_t cnt;
 uint8_t shift;

 for(cnt = 0; cnt < 3; cnt++) monthname[cnt] = MonthTab[cnt + ((month - 1) * 3)];
 monthname[3] = '\0';
 if(date > 9) sprintf(buffer, "%02d %s %02d",date,monthname,year);
 else sprintf(buffer, " %d %s %02d",date,monthname,year);
 Datapin = 8;
 displaySet(currentbrt);
 PrintLine();
}

//******************************************
// Checks for keypress (short, long, very long)
//
int ButtonCheck(void)
{
 if (digitalRead(Btnpin) == LOW) pushdelay++;
 if (pushdelay > 0 and digitalRead(Btnpin) == HIGH)
 {
  if((pushdelay <= 500))
  {
    pushdelay = 0;
    return 1; // short press
  }
  else if(pushdelay <= 1000)
  {
    pushdelay = 0;
    return 2;  // long press
  }
  else if(pushdelay <= 3000)
  {
    pushdelay = 0;
    return 3;  // very long press
  }
  else
  {
   pushdelay = 0;
   return 0;
  }
 }
 else return 0;
}

//******************************************
// Clock tick (every 1000ms)
//
void ClockTick()
{

 // Clock advance by 1 second
 second++;
 if(second==60)
 {
  second=0;
  minute++;
  if(minute==60)
  {
   minute=0;
   hour++;
   if (beep and (hour >= 8 and hour <= 20)) tone(Buzzpin,3000,50); // beep every hour during daytime if on
   if(hour==24)
   {
     hour=0;
     currentMode = 1;
     timer = 0;
     currentTime = 0; // Reset counter daily
     currentMode = 0;
     DateTick();
   }
  }
 }

 // Check if auto-brightness mode is on
 if(autobrt)
 {
  for(Datapin = 3; Datapin < 9; Datapin++)
  {
   if(hour > 9 and hour < 18)
   {
    currentbrt = 2;
    displaySet(currentbrt);
   }
   else
   {
    currentbrt = 0;
    displaySet(currentbrt);
   }
  }
  DisplayDate();
 }

 DisplayTime();
}

//******************************************
// Date tick (every 24h)
//
void DateTick()
{
 uint8_t leapyear=0;
 
 if(year%4==0 and ((year%400==0) or (year%100!=0))) leapyear=1;
 if((month==4 or month==6 or month==9 or month==11) and date==30)
 {
  date=1;
  month++;
 }
 else if(month==2 and ((leapyear==0 and date==28) or (leapyear==1 and date==29)))
 {
  date=1;
  month++;
 }
 else date++;
 if(date==32)
 {
  date=1;
  month++;
 }
 if(month==13)
 {
  date=1;
  month=1;
  year++;
 }
 DisplayDate();
}

//******************************************
// Write byte to chip
//
int writeByte(int8_t wr_data)
{
  uint8_t i,count1;       
  for(i=0;i<8;i++)        //sent 8bit data
  { 
    digitalWrite(Clockpin,LOW);
    if(wr_data & 0x01)digitalWrite(Datapin,HIGH);//LSB first
    else digitalWrite(Datapin,LOW);
    wr_data >>= 1;
    digitalWrite(Clockpin,HIGH);
  
  }
  digitalWrite(Clockpin,LOW); //wait for the ACK
  digitalWrite(Datapin,HIGH);
  digitalWrite(Clockpin,HIGH);
  pinMode(Datapin,INPUT);
  
  bitDelay(); 
  uint8_t ack = digitalRead(Datapin);
  if (ack == 0)
  {  
     pinMode(Datapin,OUTPUT);
     digitalWrite(Datapin,LOW);
  }
  bitDelay();
  pinMode(Datapin,OUTPUT);
  bitDelay();
  
  return ack;
}

//******************************************
//send start signal to TM1651
//
void start(void)
{
  digitalWrite(Clockpin,HIGH);//send start signal to TM1637
  digitalWrite(Datapin,HIGH);
  digitalWrite(Datapin,LOW);
  digitalWrite(Clockpin,LOW);
} 

//******************************************
//End signal
//
void stop(void)
{
  digitalWrite(Clockpin,LOW);
  digitalWrite(Datapin,LOW);
  digitalWrite(Clockpin,HIGH);
  digitalWrite(Datapin,HIGH);
}

//******************************************
// Display hexadecimal number 0-F at selected position
//
void displayNum(uint8_t dig, uint8_t number)
{
  start();          //start signal sent to TM1651 from MCU
  writeByte(ADDR_FIXED);//
  stop();           //
  start();          //
  writeByte(STARTADDR + dig);// digit pos 0-2
  writeByte(NumTab[number]);//
  stop();            //
  start();          //
  writeByte(Cmd_DispCtrl);// 88+0 to 7 brightness, 88=display on
  stop();           //
}

//******************************************
// Clear display (no numbers lit)
//
void displayClear(void)
{
 displayDP(0);
 displayNum(0,16);
 displayNum(1,16);
 displayNum(2,16);
}

//******************************************
// Show 3 zeroes on display
//
void displayZero(void)
{
 displayDP(0);
 displayInteger(0);
}

//******************************************
// Set brightness
//
void displaySet(uint8_t brightness)
{
  Cmd_DispCtrl = 0x88 + brightness;	//Set the brightness and turn on
}

//******************************************
// Switch off display
//
void displayOff()
{
  Cmd_DispCtrl = 0x80;
  start();
  writeByte(Cmd_DispCtrl);	// 80=display off
  stop();
}

//******************************************
// Init display
//
void displayOn()
{
 displayClear();
 displayDP(0);
 displaySet(currentbrt);
}

//******************************************
// Small delay
//
void bitDelay(void)
{
 delayMicroseconds(50);
}

//******************************************
// Display characters 0-9, A-Z, space in a defined position (letters must be in uppercase!)
//
void displayChar(uint8_t dig, uint8_t letter)
{
  start();          //start signal sent to TM1651 from MCU
  writeByte(ADDR_FIXED);//
  stop();           //
  start();          //
  writeByte(STARTADDR + dig);// digit pos 0-2
  if(letter >= 'A' and letter <= 'Z') writeByte(CharTab[letter - 0x41]);  // A-Z
  else if(letter >='0' and letter <= '9') writeByte(NumTab[letter - 0x30]); // 0-9
  else if(letter =='-') writeByte(SpCharTab[2]); // -
  else writeByte(NumTab[16]);  // space
  stop();            //
  start();          //
  writeByte(Cmd_DispCtrl);// 88+0 to 7 brightness, 88=display on
  stop();           //
}

//******************************************
// Display special character (see definitions)
//
void displaySpChar(uint8_t dig, uint8_t letter)
{
  start();          //start signal sent to TM1651 from MCU
  writeByte(ADDR_FIXED);//
  stop();           //
  start();          //
  writeByte(STARTADDR + dig);// digit pos 0-2
  writeByte(letter);//
  stop();            //
  start();          //
  writeByte(Cmd_DispCtrl);// 88+0 to 7 brightness, 88=display on
  stop();           //
}

//******************************************
// Display decimal number 0-999 with prepended zeroes
//
void displayDec(uint16_t num)
{
  displayNum(0, (num/10/10) % 10);
  displayNum(1, (num/10) % 10);
  displayNum(2, num % 10);
}

//******************************************
// Display decimal number 0-999 without prepended zeroes
//
void displayDecNZ(uint16_t num)
{
  if(num > 99) displayNum(0, (num/10/10) % 10);
  if(num > 9) displayNum(1, (num/10) % 10);
  displayNum(2, num % 10);
}

//******************************************
// Display decimal number 0-99 with one zero
//
void displayDec1Z(uint16_t num)
{
  if(num > 99) displayNum(0, (num/10/10) % 10);
  displayNum(1, (num/10) % 10);
  displayNum(2, num % 10);
}

//******************************************
// Display dot (doesn't work on all displays)
//
void displayDP(uint8_t dp)
{
  uint8_t SegData;

  //if (dp == 1) SegData = 0x08;
  //else SegData = 0x00;
  start();          //start signal sent to TM1651 from MCU
  writeByte(ADDR_FIXED);
  stop();  
  start();
  writeByte(STARTADDR + 3);// digit pos 3 controls decimal point
  //writeByte(SegData);
  writeByte(dp);
  stop();      
  start();  
  writeByte(Cmd_DispCtrl);// 88+0 to 7 brightness, 88=display on
  stop();  
}

//******************************************
// Display hex number 0-FFF (4095)
//
void displayHex(uint16_t num)
{
 if(num > 0xFFF)
 {
  displaySpChar(0, Minus);
  displaySpChar(1, Minus);
  displaySpChar(2, Minus);
 }
 else
 { 
  displayNum(0, (num/16/16) % 16);
  displayNum(1, (num/16) % 16);
  displayNum(2, num % 16);
 }
}

void displayHexNZ(uint16_t num)
{
 if(num > 0xFFF)
 {
  displaySpChar(0, Minus);
  displaySpChar(1, Minus);
  displaySpChar(2, Minus);
 }
 else
 { 
  if(num > 0xFF) displayNum(0, (num/16/16) % 16);
  if(num > 0x0F) displayNum(1, (num/16) % 16);
  displayNum(2, num % 16);
 }
}

//******************************************
// Display decimal number 0-999 with prepended zeroes
//
void displayInteger(uint16_t number)
{
 uint8_t i;   

 if(number > 999)
 {
  displaySpChar(0, Minus);
  displaySpChar(1, Minus);
  displaySpChar(2, Minus);
 }
 else
 {
  start();          //start signal sent to TM1651 from MCU
  writeByte(ADDR_AUTO);// auto increment the address
  stop(); 
  start();
  writeByte(STARTADDR);// start at 0
   writeByte(NumTab[(number/100) % 10]);
   writeByte(NumTab[(number/10) % 10]);
   writeByte(NumTab[number % 10]);
  stop();
  start();
  writeByte(Cmd_DispCtrl);// 88+0 to 7 brightness, 88=display on
  stop();
 }
}
