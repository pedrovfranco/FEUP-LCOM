#include "Mario.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <minix/drivers.h>
#include <stdint.h>
#include <machine/int86.h>
#include <limits.h>

#include "video_gr.h"
#include "kbd.h"

Mario* createMario() {

	Mario* mario = (Mario*) malloc(sizeof(Mario));

	mario->done = 0;
	printf("mario_done: %u\n", mario->done);

	mario->drawSize = getXResolution()*getYResolution()*getBitsperpixel()/8;

	mario->mario_buf = malloc(mario->drawSize);

	mario->background = loadBitmap("/home/images/mario.bmp");

	mario->x = 20;
	mario->y = 500;
	mario->size = mario->background->bitmapInfoHeader.height;
	
	
	mario->jump_height = -1;
	mario->ascending = 0;
	mario->lower_bound = 665 - mario->size;


	return mario;
}

void updateMario(Mario* mario) {

	int newy;

	if (mario->ascending)
	{
		newy = mario->y - 3;

		if (newy < mario->jump_height)
		{
			mario->ascending = 0;
			mario->y = mario->jump_height;
		}
		else
			mario->y = newy;
	}
	else
	{
		newy = mario->y + 3;

		if (newy > mario->lower_bound)
			mario->y = mario->lower_bound;
		else
			mario->y = newy;
	}


}

void jump(Mario* mario) {

	mario->ascending = 1;
	mario->jump_height = mario->y - 150;

}

void updateMarioKbd(Mario* mario, unsigned long scancode) {
	// printf("Entrou no updateMarioKbd\n");

	if (scancode == 0x4D && mario->x < (getXResolution() - mario->background->bitmapInfoHeader.width)) //right arrow key
	{
		mario->x += 10;
	}

	if (scancode == 0x4B && mario->x > 20) //left arrow key
	{
		mario->x -= 10;
	}
	if (scancode == KEY_SPACE && mario->y == mario->lower_bound) // space key
	{
		jump(mario);
	}

	// printf("Saiu do updateMarioKbd\n");
}


void drawMario(Mario* mario, char* buffer) {
	
	drawBitmapTransparent(buffer, mario->background, mario->x, mario->y, ALIGN_LEFT);

}

void deleteMario(Mario* mario) {
	deleteBitmap(mario->background);
	free(mario);
}

