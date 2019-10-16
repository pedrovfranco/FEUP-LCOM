#ifndef _MARIO_H
#define _MARIO_H

#include "Bitmap.h"
#include "i8254.h"

/** @defgroup Mario Mario
 * @{
 * Struct for controlling the movement and drawing of the character mario
 */

/*! Represents the Charater Super MArio */
typedef struct {
	char* mario_buf; /*!mario_buf pointer to mario */

	int done; /*!1 if dead */
	int draw;/*!1 if its supposed to be drawn */
	Bitmap* background; /*!background Bitmap of the character*/

	int x; /*!x coordinate x of mario */
	int y;/*!y coordinate y of mario */
	int size; /*!size size of pipe */
	int drawSize;/*!drawSize size supposed to be drawn */

	int jump_height; /*!jump_height height of mario's jump */
	int ascending;  /*!1 if  jumping */
	int lower_bound; /*!lower bound coordinate */
	

}Mario;
/**
 * @ brief creates Mario
 */
Mario* createMario();
/**
 * @ brief updates Mario
 * @param game mario
 * @return mario
 */
void updateMario(Mario* game);
/**
 * @ brief draws Mario
 * @param game mario
 * @param buf pointer to mario in mem
 */
void drawMario(Mario* game, char* buf);
/**
 * @ brief makes Mario jump (updates his y coordinate)
 * @param game mario
 */
void jump(Mario* game);
/**
 * @ brief deletes mario character
 * @param game mario
 */
void deleteMario(Mario* game);

/**
 * @ brief updates mario coordinates given key pressed
 * @param mario mario
 * @param scancode key pressed
 */
void updateMarioKbd(Mario* mario, unsigned long scancode);


/**@}*/

#endif
