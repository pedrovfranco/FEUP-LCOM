#ifndef _PIPE_H
#define _PIPE_H

#include "Bitmap.h"

/** @defgroup Pipe Pipe
 * @{
 * Struct for controlling the movement and drawing of the ingame pipes. Also controls whether mario has passed the specific pipe or not 
 */

/*! Represents the PIPE */
typedef struct {

	double x;  /*!x coordinate x of pipe */
	int y; /*! y coordinate y of the pipe*/
	double speed; /*! speed speed of pipe moving */

	int crossed; /*! 1 if mario has passed it */

}Pipe;
/**
 * @ brief getPointer to Pipe
 */
Bitmap* getBotPipeBitmap();
/**
 * @ brief getPointer of the bitmap* to Pipe
 */
Bitmap** getBotPipeBitmapPointer();
/**
 * @ brief creates Pipe
 * @param x coordinate
 */
Pipe* newPipe(int x);
/**
 * @ brief updates pipe
 * @param pipe
 */
void updatePipe(Pipe* pipe);
/**
 * @ brief draws  Pipe
 * @param pipe
 * @char* buffer
 */
void drawPipe(Pipe* pipe, char* buffer);
/**
 * @ brief deletes  Pipe
 * @param pipe
 */
void deletePipe(Pipe* pipe);

/**@}*/

#endif
