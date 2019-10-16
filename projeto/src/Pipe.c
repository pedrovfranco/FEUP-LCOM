#include "Pipe.h"

#include "stdio.h"
#include "stdlib.h"

#include "video_gr.h"

Bitmap* bottomPipe=NULL;

Bitmap* getBotPipeBitmap(){

	if(!bottomPipe)
		bottomPipe= loadBitmap("/home/images/pipe.bmp");

	return bottomPipe;
}
Bitmap** getBotPipeBitmapPointer(){

	return &bottomPipe;
}

Pipe* newPipe(int x){

	Pipe* pipe = (Pipe*)malloc(sizeof(Pipe));

	pipe->x = x;
	pipe->speed = 1;

	pipe->y = 665 - getBotPipeBitmap()->bitmapInfoHeader.height + 2;

	pipe->crossed = 0;

	return pipe;
}

void updatePipe(Pipe* pipe){

	pipe->speed += 0.001;
	pipe->x -= pipe->speed; // pipe speed
}

void drawPipe(Pipe* pipe, char* buffer){

	drawBitmapTransparent(buffer, getBotPipeBitmap(), pipe->x, pipe->y, ALIGN_LEFT);

}

void deletePipe(Pipe* pipe){
	free(pipe);

}
