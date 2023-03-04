//  Auther: Derek Cooper
//  Based on the work of and credit given to:
//  Fred.Chu
//  Detlef Giessmann Germany 
//
//  Date:20 September 2020
//
//  Applicable Module:
//                     Gotek 3-digit LED display driver
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//Modified record:
//Autor: 	Detlef Giessmann Germany 
//Mail:		derek_cooper@hotmail.com
//Comment:	Library to drive LEDC68 Gotek 3 digit LED display
//IDE:  	Arduino-1.8.12
//
// Modified record:
// Autor:	Alex Wierzbowsky
// Mail:	wierzbowsky@rbsc.su
// Comment:	Added several functions to display characters and special characters with or without zeroes
//		Added full alphabet and special characters
// IDE:  	Arduino-1.8.19
//
/***************************************************************/
//

#ifndef TM1651_H
#define TM1651_H
#include <inttypes.h>
#include <Arduino.h>

//************Definitions for TM1651*********************
//
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0 

//************Definitions for special chars*********************
//
#define AT B01011111
#define Question B01010011
#define Minus B01000000
#define UpperDash B00000001
#define LowerDash B00001000
#define Plus B01110000
#define UpperBall B01100011
#define LowerBall B01011100
#define LeftBracket B00111001
#define RightBracket B00001111
#define Equal B01001000
#define Slash B01010010
#define BackSlash B01100100
#define ThreeDash B01001001
#define OR B00110110
#define Space B00000000

// Special character table
//
static uint8_t SpCharTab[] = 
{
  B01011111, B01010011, B01000000, B00000001, B00001000, B01110000, B01100011, B01011100, B00111001, B00001111,
  B01001000, B01010010, B01100100, B01001001, B00110110,B00000000
};

// Month table
//
static char MonthTab[] =
{
 "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"  
};

// Definitions for the decimal point, it's not well implememnted in hardwar
// and of little use.
//
#define DP_ON   1
#define DP_OFF  0

//**************Definitions for brightness***********************
//
#define  BRIGHT_DARKEST 0
#define  BRIGHT_TYPICAL 2
#define  BRIGHTEST      7

// 8-segment LED display assignment
//
//    a
//  f   b
//    g 
//  e   c
//    d
//
// Data byte: 0gfedcba

// Character table for 0-F
//
static int8_t NumTab[] = 
{ 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,	// 0 - 9
  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00			// A-F + space
};

// Character table for A-Z
//
static uint8_t CharTab[] = 
{
  B01110111, B01111100, B00111001, B01011110, B01111001, B01110001, B00111101, B01110100, B00110000, B00011110,
  B01110101, B00111000, B00010101, B00110111, B00111111, B01110011, B01100111, B00110001, B01101101, B01111000,
  B00111110, B00101110, B00101010, B01110110, B01101110, B01001011
};


uint8_t Cmd_SetData;
uint8_t Cmd_SetAddr;
uint8_t Cmd_DispCtrl;
uint8_t Clkpin;
uint8_t Datapin;

int writeByte(int8_t wr_data);//write 8bit data to tm1651
void start(void);//send start bits
void stop(void); //send stop bits
void displayNum(uint8_t pos, uint8_t num);
void displayChar(uint8_t pos, uint8_t letter);
void displaySpChar(uint8_t dig, uint8_t letter);
void displayHex(uint16_t num);
void displayHexNZ(uint16_t num);
void displayInteger(uint16_t num);
void displayDec(uint16_t num);
void displayDecNZ(uint16_t num);
void displayDec1Z(uint16_t num);
void displayDP(uint8_t pos);
void displayClear(void);
void displayZero(void);
void displaySet(uint8_t num);
void displayOff();
void displayOn();
void bitDelay(void);
void DisplayTime(void);
void DisplayDate(void);
int ButtonCheck(void);
void AdjustDateTime(void);
void PrintLine(void);
void ClockTick(void);

#endif
