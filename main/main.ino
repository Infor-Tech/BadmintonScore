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
 *  Features:
 *      -score reset without restarting arduino
 *      -changing sides by pressing a button
 *      -judge display
 *    
 *  Author: KRYSTIAN SLIWINSKI
 *  Contact: kskrystiansliwniski@gmail.com
 *  github: https://github.com/Infor-Tech
 */

//MAX7219 connection and setup to Arduino
//LedControl(DIN, CLK, LOAD, number of MAX7219 chips)
#include "LedControl.h"
LedControl lc = LedControl(12, 10, 11, 1);

//Setup of the LCD
//Set the LCD address to 0x27 for a 1602 display
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
int points_of_1st_player = 0, points_of_2nd_player = 0;
//Players sets won
int sets_of_1st_player = 0, sets_of_2nd_player = 0;

//variable defining on which side are players
//when value equals 0 -> player 1 is on a left side
int side = 0;

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
//On deafult Player 1 is on left side
void display_score_on_a_scoreboard()
{
    int temp = 0;
    int scores[2], sets[2];
    
    //variables used to split whole number in half
    int left_digit, right_digit;
    
    if (side == 0)
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

    for (int i = 0; i < 2; i++)
    {
        right_digit = scores[i] % 10;
        left_digit = scores[i] % 100 / 10;
        
        temp += i;

        lc.setDigit(0, temp + 1, left_digit, false);
        lc.setDigit(0, temp + 2, right_digit, false);
        if(temp == 0)
            lc.setDigit(0, 0, sets[i], false);
        else
            lc.setDigit(0, 5, sets[i], false);
        
        temp++;
    }
}

//Checks if button is pressed, and if so, it runs specific action
void get_button_state(const int button_pin, const int action){
    if(!digitalRead(button_pin)){
        switch(action)
        {
            case 1:
                points_of_1st_player++;
                break;
            case 2:
                points_of_2nd_player++;
                break;
            case 3:
                points_of_1st_player -= 1;
                points_of_2nd_player -= 1;
                break;
            case 4:
                points_of_1st_player = points_of_2nd_player = sets_of_1st_player = sets_of_2nd_player = side = 0;
                break;
            case 5:
                side = side == 0 ? 1 : 0;
                break;
            default:
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

void check_if_set_is_won(int score_of_supposed_winner, int score_of_opponent){
    if ((score_of_supposed_winner >= 21 && score_of_supposed_winner - score_of_opponent >= 2) || score_of_supposed_winner == 30)
    {
        if(score_of_supposed_winner == points_of_1st_player)
            sets_of_1st_player++;
        else
            sets_of_2nd_player++;

        points_of_1st_player = points_of_2nd_player = 0;

        side = side == 0 ? 1 : 0;

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

    //initialize the lcd
    lcd.init();
    lcd.backlight();

    //initialize MAX7219
    lc.shutdown(0, false); //Turn on MAX7219
    lc.setIntensity(0, 7); //Set brightness
    lc.clearDisplay(0);    //Clear all displays connected to MAX7219

    //startup screen
    lcd.print("   github.com/");
    lcd.setCursor(3, 1);
    lcd.print("Infor-Tech");

    //displaying numbers 0-9 on 7 segment displays
    for (int i = 0; i <= 9; i++)
    {
        for (int j = 0; j <= 5; j++)
            lc.setDigit(0, j, i, false);
        delay(400);
    }

    //after
    display_judge_interface();
    display_score_on_a_scoreboard();
}

void loop()
{
    //When side=0 on 7 segment displays player 1 is on a left
    get_button_state(CHANGE_SIDES, 5);

    //When ADD_SCORE_FOR_THE_1ST_PLAYER pressed there is added point for Player1 on lcd and 7 segment display
    get_button_state(ADD_SCORE_FOR_THE_1ST_PLAYER, 1);

    //When ADD_SCORE_FOR_THE_2ND_PLAYER pressed there is added point for Player 2 on lcd and 7 segment display
    get_button_state(ADD_SCORE_FOR_THE_2ND_PLAYER, 2);

    //Reset scoreboard
    get_button_state(RESET_SCORE, 4);

    //Subtract point from both players
    get_button_state(SUB_POINT_FROM_PLAYERS, 3);

    //When Player 1 won set
    check_if_set_is_won(points_of_1st_player, points_of_2nd_player);

    //When Player 2 won set
    check_if_set_is_won(points_of_2nd_player, points_of_1st_player);
}
