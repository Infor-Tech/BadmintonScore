/*  Scoreboard designed for badminton
 * 
 *  Program designed for:
 *      -Arduino UNO
 *      -LCD i2c 1602 display
 *      -5 pushbuttons
 *      -6 - 7 segment common cathode displays
 *      -MAX 7219 chip
 *      
 *  fritzing project avaiable
 *  Shematics avaiable soon
 *    
 *  Features:
 *      -score reset without restarting arduino
 *      -changing sides by pressing a button
 *      -judge display
 *    
 *  Author: KRYSTIAN SLIWINSKI
 *  Contact: kskrystiansliwniski@gmail.com
 *  github: https://github.com/Infor-Tech
 */

//included libraries 
#include "LedControl.h"
#include <LiquidCrystal_I2C.h>
#include "wire.h"

//MAX7219 connection to Arduino
//LedControl(DIN, CLK, LOAD, number of MAX7219 chips)
LedControl lc = LedControl(12, 10, 11, 1);

//Set the LCD address to 0x27 for a 1602 display
LiquidCrystal_I2C lcd(0x27, 20, 4);

//scores variables
int ptk1 = 0; //Player 1 score
int ptk2 = 0; //Player 2 score
int s1 = 0; //Player 1 sets won
int s2 = 0; //Player 2 sets won

//variable defining on which side are players
int side = 0;

//variables to split whole number
int LeftDigit;
int RightDigit;

//startup variable
int startup = 0;

//Switches pin connection to Arduino
#define AddScore1 2   //player 1
#define AddScore2 3   //player 2 
#define SubScore 4
#define ChangeSides 5
#define ScoreReset 6

//displaying all data on LCD
void interface() {
    lcd.clear();
    lcd.print("Player 1: ");
    lcd.print(ptk1);
    lcd.setCursor(14,0);
    lcd.print("|");
    lcd.print(s1);
    lcd.setCursor(0, 1);
    lcd.print("Player 2: ");
    lcd.print(ptk2);
    lcd.setCursor(14,1);
    lcd.print("|");
    lcd.print(s2);
}

//Displaying score on 7 segment displays depends on side variable
//On deafult Player 1 is on left side
void ScorePrint() {
    if(side == 0){
  //Player 1 score
      RightDigit=ptk1%10;
      LeftDigit=ptk1%100/10;
      lc.setDigit(0,1,LeftDigit,false);
      lc.setDigit(0,2,RightDigit,false);
      lc.setDigit(0,0,s1,false);
      
  //Player 2 score
      RightDigit=ptk2%10;
      LeftDigit=ptk2%100/10;
      lc.setDigit(0,3,LeftDigit,false);
      lc.setDigit(0,4,RightDigit,false);
      lc.setDigit(0,5,s2, false);
    }else{
  //Player 1 score
      RightDigit=ptk1%10;
      LeftDigit=ptk1%100/10;
      lc.setDigit(0,3,LeftDigit,false);
      lc.setDigit(0,4,RightDigit,false);
      lc.setDigit(0,5,s1,false);
      
  //Player 2 score
      RightDigit=ptk2%10;
      LeftDigit=ptk2%100/10;
      lc.setDigit(0,1,LeftDigit,false);
      lc.setDigit(0,2,RightDigit,false);
      lc.setDigit(0,0,s2,false);
    }
}
    
void setup() {
//buttons pin mode
    pinMode(AddScore1, INPUT_PULLUP);
    pinMode(AddScore2, INPUT_PULLUP);
    pinMode(SubScore, INPUT_PULLUP);
    pinMode(ChangeSides, INPUT_PULLUP);
    pinMode(ScoreReset, INPUT_PULLUP);
    
//initialize the lcd
    lcd.init();
    lcd.backlight();
    
//initialize MAX7219
    lc.shutdown(0,false); //Turn on MAX7219
    lc.setIntensity(0,7); //Set brightness
    lc.clearDisplay(0); //Clear all displays connected to MAX7219
    
//startup screen
  //on lcd
    lcd.print("   github.com/");
    lcd.setCursor(3, 1);
    lcd.print("Infor-Tech");

  //on 7 segment display
  //displaying numbers 0-9
    for(startup = 0; startup <= 9; startup++){
      lc.setDigit(0,0,startup,false); //left set count
      lc.setDigit(0,1,startup,false); //left score
      lc.setDigit(0,2,startup,false); //left score
      lc.setDigit(0,3,startup,false); //right score
      lc.setDigit(0,4,startup,false); // right score
      lc.setDigit(0,5,startup,false); // right set count
      delay(400);
    }

  //after startup
    interface();
    ScorePrint();
    
}

void loop() {
//When side=0 on 7 segment displays player 1 is on a left
    if(!digitalRead(ChangeSides)) {
      if(side == 0){
        side = 1;
      }else{
        side = 0;
      }
      ScorePrint();
      delay(750);
    }

//When AddScore1 pressed there is added point for Player1 on lcd and 7 segment display
    if(!digitalRead(AddScore1)){
      ptk1++;
      interface();
      ScorePrint();
      delay(750);
    }

//When AddScore2 pressed there is added point for Player 2 on lcd and 7 segment display
    if(!digitalRead(AddScore2)){
      ptk2++;
      interface();
      ScorePrint();
      delay(750);
    }

//Reset scoreboard
    if(!digitalRead(ScoreReset)){
      ptk1 = 0;
      ptk2 = 0;
      s1 = 0;
      s2 = 0;
      side = 0;
      ScorePrint();
      interface();
      delay(750);
    }

//Subtract point from both players
    if(!digitalRead(SubScore)){
      ptk1 = ptk1 - 1;
      ptk2 = ptk2 - 1;
      ScorePrint();
      interface();
      delay(750);
    }

  //When Player 1 won set
    if(ptk1 >= 21 && ptk1 - ptk2 >= 2){
      s1 = s1 + 1;
      ptk1 = 0;
      ptk2 = 0;
      if(side == 0){
        side = 1;
      }else{
        side = 0;
      }
      delay(10000);
      interface();
      ScorePrint();
    }

  //When Player 2 won set
    if(ptk2 >= 21 && ptk2 - ptk1 >= 2){
      s2 = s2 + 1;
      ptk1 = 0;
      ptk2 = 0;
      if(side == 0){
        side = 1;
      }else{
        side = 0;
      }
      delay(10000);
      interface();
    }
}
