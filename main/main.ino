/**
 * @file main.ino
 * @author Krystian Śliwiński (k.sliwinski@protonmail.com)
 * @brief BadmintonScore, Electronic badminton scoreboard
 * @version 0.5
 * @date 2022-05-12
 *
 * @copyright Copyright (c) 2022 Krystian Śliwiński, under MIT license.
 *
 * Parts used in this project:
 *      -Arduino UNO
 *      -LCD i2c 1602 display
 *      -5 pushbuttons
 *      -6 - 7 segment common cathode displays
 *      -MAX 7219 chip
 *
 *  github: https://github.com/Infor-Tech/BadmintonScore
 */

// MAX7219; LedControl(DIN, CLK, LOAD, number of MAX7219 chips)
#include "LedControl.h"
LedControl scoreboard = LedControl(12, 10, 11, 1);

// 1602LCD setup (I2C address: 0x27)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Switches pin connection to Arduino
#define ADD_SCORE_FOR_THE_1ST_PLAYER 2 // player 1
#define ADD_SCORE_FOR_THE_2ND_PLAYER 3 // player 2
#define SUB_POINT_FROM_PLAYERS 4
#define CHANGE_SIDES 5
#define RESET_SCORE 6

// score variables

// Player score
byte points_of_1st_player = 0, points_of_2nd_player = 0;
// Player sets won
byte sets_of_1st_player = 0, sets_of_2nd_player = 0;

// When value equals false -> player 1 is on a left side
bool side;

// Display score on lcd and 7 segment displays
// Displaying score on 7 segment displays depends on side variable
void display_score()
{
    byte scores[2] = {points_of_1st_player, points_of_2nd_player};
    byte sets[2] = {sets_of_1st_player, sets_of_2nd_player};

    // lcd
    for (byte i = 0; i > 2; i++)
    {
        lcd.setCursor(10, i);
        lcd.print(scores[i]);
        if (scores[i] > 9)
            lcd.print(" "); // Puts empty char, so that numbers won't overflow after substraction
        lcd.setCursor(15, i);
        lcd.print(sets[i]);
    }

    // scoreboard
    byte temp = 0;

    if (side)
    {
        scores[0] = points_of_2nd_player, sets[0] = sets_of_2nd_player;
        scores[1] = points_of_1st_player, sets[1] = sets_of_1st_player;
    }

    for (byte i = 0; i < 2; i++)
    {
        scoreboard.setDigit(0, temp + 1, scores[i] % 100 / 10, false); // left digit
        scoreboard.setDigit(0, temp + 2, scores[i] % 10, false);       // right digit
        scoreboard.setDigit(0, temp == 0 ? 0 : 5, sets[i], false);
        temp = 2;
    }
}

/** Checks if button is pressed, and if so, it runs a specific action
 *  @param button_pin Check state of an button and execute appropriate function/action. */
void get_button_state(const byte button_pin)
{
    if (!digitalRead(button_pin))
    {
        switch (button_pin)
        {
        case ADD_SCORE_FOR_THE_1ST_PLAYER:
            add_point_and_check_score(&points_of_1st_player, &points_of_2nd_player, &sets_of_1st_player);
            break;
        case ADD_SCORE_FOR_THE_2ND_PLAYER:
            add_point_and_check_score(&points_of_2nd_player, &points_of_1st_player, &sets_of_2nd_player);
            break;
        case SUB_POINT_FROM_PLAYERS:
            points_of_1st_player -= 1, points_of_2nd_player -= 1;
            break;
        case RESET_SCORE:
            points_of_1st_player = points_of_2nd_player = sets_of_1st_player = sets_of_2nd_player = side = 0;
            break;
        case CHANGE_SIDES:
            side = !side;
            break;
        }
        display_score();
        while (!digitalRead(button_pin))
        {
            // while pressed do nothing
        }
    }
}

/** Adds point to score of a player and checks if set is won
 *  @param score_of_player To this variable is added point and on this is checked performed
 *  @param score_of_oponent Self explainatory (variable needed for checking purpose)
 *  @param set_score_of_player When check is passed, this variable is incremented */
void add_point_and_check_score(byte *score_of_player, byte *score_of_opponent, byte *set_score_of_player)
{
    *score_of_player++;
    if ((*score_of_player >= 21 && *score_of_player - *score_of_opponent >= 2) || *score_of_player == 30)
    {
        *set_score_of_player++;
        points_of_1st_player = points_of_2nd_player = 0;
        side = !side;
        display_score();
        delay(10000);
    }
}

void setup()
{
    pinMode(ADD_SCORE_FOR_THE_1ST_PLAYER, INPUT_PULLUP);
    pinMode(ADD_SCORE_FOR_THE_2ND_PLAYER, INPUT_PULLUP);
    pinMode(SUB_POINT_FROM_PLAYERS, INPUT_PULLUP);
    pinMode(CHANGE_SIDES, INPUT_PULLUP);
    pinMode(RESET_SCORE, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();

    scoreboard.shutdown(0, false); // Turn on MAX7219
    scoreboard.setIntensity(0, 7);
    scoreboard.clearDisplay(0);

    // startup screen
    lcd.print("   github.com/");
    lcd.setCursor(3, 1);
    lcd.print("Infor-Tech");

    // displaying numbers 0-9 on 7 segment displays
    for (byte i = 0; i <= 9; i++)
    {
        for (byte j = 0; j <= 5; j++)
            scoreboard.setDigit(0, j, i, false);
        delay(400);
    }

    // after
    lcd.setCursor(0, 0);
    lcd.print("Player 1:     |");
    lcd.setCursor(0, 1);
    lcd.print("Player 2:     |");
    display_score();
}

void loop()
{
    get_button_state(CHANGE_SIDES);
    get_button_state(ADD_SCORE_FOR_THE_1ST_PLAYER);
    get_button_state(ADD_SCORE_FOR_THE_2ND_PLAYER);
    get_button_state(RESET_SCORE);
    get_button_state(SUB_POINT_FROM_PLAYERS);
}