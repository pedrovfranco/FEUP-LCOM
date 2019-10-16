#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "timer.h"
#include "Mario.h"
#include "Bitmap.h"
#include "Pipe.h"
#include "Score.h"

/** @defgroup GameState GameState
 * @{
 * Struct the controls the flow of the game itself
 */
typedef struct {
	char* video_mem;
	char* double_buf;
	char* background_buf;

	int drawSize;

	int backgroundX;
	Bitmap* background;

	Mario* mario;
	Score* score;

	int num_pipes;
	int space_between_pipes;

	Pipe* pipes[5];

	Bitmap* digits[10];


} GameState;


/**
 * @brief      Constructs a GameState
 *
 * @return     A pointer to the GameState
 */
GameState* newGameState();


/**
 * @brief      Checks if Mario collides with pipes
 *
 * @param      state  The GameState
 *
 * @return     1 if is colliding and 0 otherwise
 */
int checkCollision(GameState* state);


void updateGameState(GameState* state);

/**
 * @brief      Updates GameState at keyboard interrupt
 * 
 * @detailed	Updates Mario at keyboard interrupt and passed the scancode passed by the menu
 *
 * @param      state     The GameState
 * @param[in]  scancode  The keyboard scancode passed by the menu
 */
void updateGameStateKbd(GameState* state, unsigned long scancode);
void drawGameState(GameState* state);
void deleteGameState(GameState* state);

void drawPipes(GameState* state);
/**
 * @brief      Updates the pipes.
 *
 * @detailed	If mario has passed the pipe, then assigns 1 to crossed member variable of Pipe. If the left-most pipe touches the left side of the screen then 
 *
 * @param      state  The gamestate
 */
void updatePipes(GameState* state);
int checkCollision(GameState* state);
void shiftPipesLeft(GameState* state);
void deletePipes(GameState* state);

void drawScoreGamestate(GameState* state);

/**@}*/

#endif
