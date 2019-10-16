#ifndef _MENU_H
#define _MENU_H

#include "Menu.h"
#include "Bitmap.h"
#include "GameState.h"
#include "mouse.h"
#include "timer.h"
#include "Score.h"


/** @defgroup Menu Menu
 * @{
 * Struct for controlling the flow of the entire program
 */

/*! Represents the game */
typedef struct {
	int current_state; // 0 = menu, 1 = game, 2 = about, 3 = game_over, 4 = exit

	char* video_mem;/*!< pointer to video_mem */
	char* double_buf; /*!< pointer to double buf */
	char* background_buf;/*!< pointer to background_buf  */
	char* about_buf; /*!< pointer to aboutMenu */
	char* game_over_buf; /*!< pointer to gameOverBuf */
	char* high_score_buf; /*!< pointer to  highScoreBuf*/

	GameState* gamestate; /*!< pointer to gameState struct */
	Score* high_scores[6]; /*!< pointerto highScores array */

	Mouse* mouse;/*!< pointer to mouse struct */
	Timer* timer; /*!< pointerto timer struct */

	int timer_irq; /*!< order of the timer bit in interrupt mask */
	int kbd_irq;     /*!< order of the kbd bit in interrupt mask */

	unsigned long scancode; /*!< scancode of pressed key*/
	int done;        /*!< 1 exit game */
	int drawSize; /*!< int drawSize   */

	Bitmap* background;/*!< pointer to the bitmap of backGroundMenu*/
	Bitmap* about; /*!< pointer to the aboutMenu */
	Bitmap* game_over; /*!< pointer to the bitmap of gameOver */
	Bitmap* high_score_bitmap; /*!< pointer to the bitmap of the highScoreBitmap  */

	Bitmap* lettersandnumbers[38]; /*!< pointer to the bitmap of the lettersArray  */
	int index[3]; /*!< array index  */
	int flashing[3]; // -1 = not yet appearing, 0 = flashing, 1 = selected


} Menu;
/**
 * @ brief creates Menu
 */
Menu* newMenu();
/**
 * @ brief updates Menu -timer event
 * @param menu
 */
void updateMenu(Menu* menu);
/**
 * @ brief updates Menu -keyboard event
 * @param menu
 */
void updateMenuKbd(Menu* menu);
/**
 * @ brief updates Menu -mouse event
 * @param menu
 */
void updateMenuMouse(Menu* menu);
/**
 * @ brief draw Menu
 * @param menu
 */
void drawMenu(Menu* menu);
/**
 * @ brief delete Menu
 * @param menu
 */
void deleteMenu(Menu* menu);
/**
 * @ brief draws score
 * @param menu
 */
void drawScoreMenu(Menu* menu);
/**
 * @ brief load HighScoreList
 * @param menu
 */
int loadHighScores(Menu* menu);
/**
 * @ brief adds highScore
 * @param menu
 */
void addHighScore(Menu* menu);
/**
 * @ brief save HighScore
 * @param menu
 */
void saveHighScores(Menu* menu);
/**
 * @ brief draw HighScores
 * @param menu
 */
void drawHighScores(Menu* menu);
/**
 * @ brief swaps 2 int numbers
 * @param pointer_to_int1
 * @param pointer_to_int2
 */
void swapInt(int * a, int * b);
/**
 * @ brief swpas 2 chars
 * @param char* a
 * @param char* b
 */
void swapChar(char *a, char *b);
/**
 * @ brief convert char to int
 * @param char*
 */
int stoi(char* input);

/**@}*/

#endif
