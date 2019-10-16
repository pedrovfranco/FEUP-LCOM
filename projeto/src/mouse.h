#ifndef __MOUSE_H
#define __MOUSE_H

#include "Bitmap.h"

/*! Represents the mouse */

typedef struct {
	char* mouse_buf; /*! pointer to mouse_buf */
	int drawSize;/*! int Size  of mouse */

	int x; /*! z pos of mouse */
	int y; /*! y pos of mouse */

	int LB; // -1 = nothing, 0 = false, 1 = true
	int MB;	// -1 = nothing, 0 = false, 1 = true
	int RB; // -1 = nothing, 0 = false, 1 = true
	int click;
	
	int mouse_irq; /*! mouse interrupt mask */

	unsigned long packet[3]; /*! packet array */
	int contador; /*! counter */
	int currlength; /*! length  */

	Bitmap* pointer; /*! pointer to bitmap image of mouse */

} Mouse;
/**
 * @ brief creates mouse
 */
Mouse* newMouse();
/**
 * @ brief updates Mouse
 * @param mouse
 */
void updateMouse(Mouse* mouse);
/**
 * @ brief draws mouse
 * @param mouse
 */
void drawMouse(Mouse* mouse, char* buffer);
/**
 * @ brief deletes mouse
 * @param mouse
 */
void deleteMouse(Mouse* mouse);
/**
 * @ brief chech_gesture done with mouse
 * @param mouse
 * @param length of gesture
 */
int check_gesture(Mouse* mouse, short length);
/**
 * @ brief prints mouse packet
 * @param mouse
 */
void print_mouse(Mouse* mouse);
/**
 * @ brief subscribe mouse
 * @param mouse
 */
int mouse_subscribe_int(Mouse* mouse);
/**
 * @ brief unsubscribe mouse
 * @param mouse
 */
int mouse_unsubscribe_int(Mouse* mouse);
/**
 * @ brief  mouse handler
 * @param mouse
 */
int mouse_handler(Mouse* mouse);
/**
 * @ brief enables packet reading from kbc
 * @param mouse
 */
int enable_packet_reading(Mouse* mouse);
/**
 * @ brief disables packet reading from kbc
 * @param mouse
 */
int disable_packet_reading(Mouse* mouse);
/**
 * @ brief sets stream mode use of mouse in  kbc
 * @param mouse
 */
int set_stream_mode(Mouse* mouse);

/**
 * @ brief write to mouse
 * @param mouse
 * @param word
 */
int write_to_mouse(Mouse* mouse, unsigned long command);

/**
 * @ brief writes cmd to kbc
 * @param mouse
 * @param cdm
 */
int write_to_kbc(unsigned long port, unsigned long cmd);


int mouse_handler_remote(Mouse* mouse);
int set_remote_mode(Mouse* mouse);
/**
 * @ brief reades mosue packets from kbc
 * @param mouse
 */
int read_kbc(unsigned long port, unsigned long * data);
/**
 * @ brief empties kbc buffer
 */
void flush_mouse();

#endif /* __MOUSE_H */
