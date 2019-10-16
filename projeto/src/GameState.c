#include "GameState.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include  <limits.h>

#include "video_gr.h"
#include "kbd.h"


GameState* newGameState(){
	GameState* state = (GameState*) malloc(sizeof(GameState));

	state->drawSize = getXResolution()*getYResolution()*getBitsperpixel()/8;

	state->video_mem = getVideoMem();
	state->double_buf = malloc(state->drawSize);

	state->backgroundX=0;

	state->background = loadBitmap("/home/images/mario_background.bmp");

	state->mario = createMario();
	state->score = newScore(0);

	state->background_buf = malloc(state->drawSize);
	
	drawBitmap(state->background_buf, state->background, 0,0, ALIGN_LEFT);

	state->num_pipes = 5;
	state->space_between_pipes = getXResolution()/4 + 80;
	printf("space_between_pipes = %d\n", state->space_between_pipes);
	

	int i;
	for(i = 0; i < 5; i++)
	{
		state->pipes[i] = newPipe(getXResolution() + state->space_between_pipes*i);
	}

	char* foo = "/home/images/numbers/";
	char* bar = ".bmp";
	char* temp = malloc(27);
	for (i = 0; i < 10; i++)
	{
		strcpy(temp, foo);
		*(temp + strlen(foo)) = i + '0';
		strcpy(temp + strlen(foo) + 1, bar);
		
		printf("String = %s\n", temp);
		
		state->digits[i] = loadBitmap(temp);
	}

	return state;
}



void updateGameState(GameState* state){
	// printf("Entrou no updateGameState\n");

	updateMario(state->mario);
	updatePipes(state);
	drawGameState(state);

	// printf("Saiu do updateGameState\n");
}



void updatePipes(GameState* state){
	// printf("Entrou no updatePipes\n");

	int i;
	for (i = 0; i < state->num_pipes; i++)
	{
		// previousx = state->pipes[i]->x;

		updatePipe(state->pipes[i]);

		if (!state->pipes[i]->crossed)
		{
			if (state->mario->x > state->pipes[i]->x + getBotPipeBitmap()->bitmapInfoHeader.width)
			{
				state->score->value++;
				printf("Score update to %d\n", state->score->value);
				state->pipes[i]->crossed = 1;
			}
		}

		// if ((previousx + getBotPipeBitmap()->bitmapInfoHeader.width >= state->mario->x) && (state->pipes[i]->x + getBotPipeBitmap()->bitmapInfoHeader.width < state->mario->x))
		// {
		// 	state->mario->score++;
		// 	printf("Score update to %d\n", state->mario->score)
		// }
	}

	if (state->pipes[0]->x < 0)
	{
		shiftPipesLeft(state);
	}
	// printf("Saiu do updatePipes\n");
}


void updateGameStateKbd(GameState* state, unsigned long scancode) {

	updateMarioKbd(state->mario, scancode);

}

void drawPipes(GameState* state){
	// printf("Entrou no drawPipes\n");

	int i;
	for(i = 0; i < state->num_pipes; i++)
	{
		drawPipe(state->pipes[i], state->double_buf);
	}

	// printf("Saiu do drawPipes\n");
}

void drawGameState(GameState* state){
	// printf("Entrou no drawGameState\n");

	memcpy(state->double_buf, state->background_buf, state->drawSize);
	
	drawPipes(state);

	drawMario(state->mario, state->double_buf);

	drawScoreGamestate(state);
	
	memcpy(state->video_mem, state->double_buf, state->drawSize);

	// printf("Saiu do drawGameState\n");
}

void drawScoreGamestate(GameState* state) {
	int x = 35;
	int y = 40;

	drawBitmapTransparent(state->double_buf, state->digits[(state->score->value/100) % 10], x ,y, ALIGN_LEFT);
	x += state->digits[(state->score->value/100) % 10]->bitmapInfoHeader.width + 3;
	drawBitmapTransparent(state->double_buf, state->digits[(state->score->value/10) % 10], x ,y, ALIGN_LEFT);
	x += state->digits[(state->score->value/10) % 10]->bitmapInfoHeader.width + 3;
	drawBitmapTransparent(state->double_buf, state->digits[state->score->value % 10], x ,y, ALIGN_LEFT);
	
}

void shiftPipesLeft(GameState* state){
	// printf("Entrou no shiftPipesLeft\n");

	deletePipe(state->pipes[0]);

	int i;
	for(i = 0; i < state->num_pipes - 1; i++){

		state->pipes[i] = state->pipes[i+1];
	}

	state->pipes[state->num_pipes - 1] = newPipe(state->pipes[state->num_pipes - 2]->x + state->space_between_pipes);
	state->pipes[state->num_pipes - 1]->speed = state->pipes[state->num_pipes - 2]->speed;

	// printf("Saiu do shiftPipesLeft\n");
}

int checkCollision(GameState* state) { // Verifies for each of the 4 corners of mario if they are inside of a pipe, if so returns 1 (True)

	int i, j, k, newx, newy;
	for (i = 0; i < state->num_pipes; ++i)
	{
		for (j = 0; j < 2; j++)
		{
			newx = state->mario->x + j*(state->mario->background->bitmapInfoHeader.width);

			for (k = 0; k < 2; k++)
			{
				newy = state->mario->y + k*(state->mario->background->bitmapInfoHeader.height);

				if (newx > state->pipes[i]->x && newx < (state->pipes[i]->x + getBotPipeBitmap()->bitmapInfoHeader.width) && newy > state->pipes[i]->y && newy < (state->pipes[i]->y + getBotPipeBitmap()->bitmapInfoHeader.height))
					return 1; // True
			}
		}

	}

	return 0; // False
}


void deletePipes(GameState* state){
	
	int i;
	for (i = 0; i < state->num_pipes; i++)
	{
		deletePipe(state->pipes[i]);
	}

}

void deleteGameState(GameState* state){

	deleteBitmap(state->background);

	deleteMario(state->mario);
	free(state->score);

	deletePipes(state);
	deleteBitmap(getBotPipeBitmap());
	// *getBotPipeBitmapPointer()=NULL;

	free(state->double_buf);
	free(state->background_buf);


	free(state);
}
