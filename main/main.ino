/*  Badminton scoreboard
 * 
 *  Github repo: https://github.com/Infor-Tech/Badminton-Scoreboard
 * 
 *  Parts used in this project:
 *      -Arduino UNO
 *      -LCD i2c 1602 display
 *      -5 pushbuttons
 *      -6 - 7 segment common cathode displays
 *      -MAX 7219 chip
 *      
 *  fritzing project with schematics avaiable
 *    
 *  Author: KRYSTIAN SLIWINSKI
 *  Contact: k.sliwinski@protonmail.com
 *  github: https://github.com/Infor-Tech
 */

//MAX7219 connection and setup to Arduino
//LedControl(DIN, CLK, LOAD, number of MAX7219 chips)
#include "LedControl.h"
LedControl scoreboard = LedControl(12, 10, 11, 1);

//1602LCD setup (I2C address: 0x27)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

//Switches pin connection to Arduino
#define ADD_SCORE_FOR_THE_1ST_PLAYER 2 //player 1
#define ADD_SCORE_FOR_THE_2ND_PLAYER 3 //player 2
#define SUB_POINT_FROM_PLAYERS 4
#define CHANGE_SIDES 5
#define RESET_SCORE 6

//score variables
//Players score
byte points_of_1st_player = 0, points_of_2nd_player = 0;
//Players sets won
byte sets_of_1st_player = 0, sets_of_2nd_player = 0;

//variable defining on which side are players
//when value equals false -> player 1 is on a left side
bool side;

//displaying all data on LCD
void display_judge_interface()
{
    lcd.clear();
    lcd.print("Player 1: ");
    lcd.print(points_of_1st_player);
    lcd.setCursor(14, 0);
    lcd.print("|");
    lcd.print(sets_of_1st_player);
    lcd.setCursor(0, 1);
    lcd.print("Player 2: ");
    lcd.print(points_of_2nd_player);
    lcd.setCursor(14, 1);
    lcd.print("|");
    lcd.print(sets_of_2nd_player);
}

//Displaying score on 7 segment displays depends on side variable
void display_score_on_a_scoreboard()
{
    byte temp = 0;
    byte scores[2], sets[2];
    
    //variables used to split whole number in half, so that it could be displayed on 7 segment displays
    byte left_digit, right_digit;
    
    if (!side)
    {
        scores[0] = points_of_1st_player;
        sets[0] = sets_of_1st_player;
        scores[1] = points_of_2nd_player;
        sets[1] = sets_of_2nd_player;
    }
    else
    {
        scores[0] = points_of_2nd_player;
        sets[0] = sets_of_2nd_player;
        scores[1] = points_of_1st_player;
        sets[1] = sets_of_1st_player;
    }

    for (byte i = 0; i < 2; i++)
    {
        scoreboard.setDigit(0, temp + 1, scores[i] % 100 / 10, false);  //left digit
        scoreboard.setDigit(0, temp + 2, scores[i] % 10, false);        //right digit
        scoreboard.setDigit(0, temp == 0 ? 0 : 5, sets[i], false);
        temp = 2;
    }
}

//Checks if button is pressed, and if so, it runs a specific action
void get_button_state(const byte button_pin, const byte action)
{
    if(!digitalRead(button_pin)){
        switch(action)
        {
            case 1:
                points_of_1st_player++;
                check_if_set_is_won(&points_of_1st_player, &points_of_2nd_player, &sets_of_1st_player);
                break;
            case 2:
                points_of_2nd_player++;
                check_if_set_is_won(&points_of_2nd_player, &points_of_1st_player, &sets_of_2nd_player);
                break;
            case 3:
                points_of_1st_player -= 1;
                points_of_2nd_player -= 1;
                break;
            case 4:
                points_of_1st_player = points_of_2nd_player = sets_of_1st_player = sets_of_2nd_player = side = 0;
                break;
            case 5:
                side = !side;
                break;
        }
        display_judge_interface();
        display_score_on_a_scoreboard();
        while (!digitalRead(button_pin))
        {
            //while pressed do nothing
        }
    }
}

void check_if_set_is_won(byte* score_of_supposed_winner, byte* score_of_opponent, byte* set_score_of_supposed_winner)
{
    if ((*score_of_supposed_winner >= 21 && *score_of_supposed_winner - *score_of_opponent >= 2) || *score_of_supposed_winner == 30)
    {
        *set_score_of_supposed_winner++;
        points_of_1st_player = points_of_2nd_player = 0;
        side = !side;
        display_judge_interface();
        display_score_on_a_scoreboard();
        delay(10000);
    }
}

void setup()
{
    //buttons pin mode
    pinMode(ADD_SCORE_FOR_THE_1ST_PLAYER, INPUT_PULLUP);
    pinMode(ADD_SCORE_FOR_THE_2ND_PLAYER, INPUT_PULLUP);
    pinMode(SUB_POINT_FROM_PLAYERS, INPUT_PULLUP);
    pinMode(CHANGE_SIDES, INPUT_PULLUP);
    pinMode(RESET_SCORE, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();

    scoreboard.shutdown(0, false); //Turn on MAX7219
    scoreboard.setIntensity(0, 7);
    scoreboard.clearDisplay(0);

    //startup screen
    lcd.print("   github.com/");
    lcd.setCursor(3, 1);
    lcd.print("Infor-Tech");

    //displaying numbers 0-9 on 7 segment displays
    for (byte i = 0; i <= 9; i++)
    {
        for (byte j = 0; j <= 5; j++)
            scoreboard.setDigit(0, j, i, false);
        delay(400);
    }

    //after
    display_judge_interface();
    display_score_on_a_scoreboard();
}

void loop()
{
    get_button_state(CHANGE_SIDES, 5);
    get_button_state(ADD_SCORE_FOR_THE_1ST_PLAYER, 1);
    get_button_state(ADD_SCORE_FOR_THE_2ND_PLAYER, 2);
    get_button_state(RESET_SCORE, 4);
    get_button_state(SUB_POINT_FROM_PLAYERS, 3);
}
