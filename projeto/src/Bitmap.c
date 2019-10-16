#include "Bitmap.h"

#include "stdio.h"
#include "video_gr.h"
#include "i8254.h"
#include <stdlib.h>
#include <string.h>

#define hres				1024 //Compatibility reasons
#define vres				768  //
#define bytesPerPixel		1

Bitmap* loadBitmap(const char* filename) {
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd = !1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(
            bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}

void drawBitmap(char* destination, Bitmap* bmp, int x, int y, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > hres || y + height < 0
            || y > vres)
        return;

    int xCorrection = 0;
    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > hres)
            drawWidth = hres;
    } else if (x + drawWidth >= hres) {
        drawWidth = hres - x;
    }

    char* bufferStartPos;
    char* imgStartPos;

    int i;
    for (i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= vres)
            continue;

        bufferStartPos = destination + x * 2 + pos * hres * 2;

        imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;

        memcpy(bufferStartPos, imgStartPos, drawWidth * 2);
    }
}

void drawBitmapTransparent(char* destination, Bitmap* bmp, int x, int y, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > hres || y + height < 0
            || y > vres)
        return;

    int xCorrection = 0;
    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > hres)
            drawWidth = hres;
    } else if (x + drawWidth >= hres) {
        drawWidth = hres - x;
    }

    char* bufferStartPos;
    char* imgStartPos;

    int i, j;
    for (i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= vres)
            continue;

        bufferStartPos = destination + x * 2 + pos * hres * 2;

        imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;
        
        for (j = 0; j < drawWidth*2; j+=2)
        {
            // printf("Counter = %u\n", counter);
            if (*(unsigned short*)(imgStartPos + j) != TRANSPARENT_COLOUR)
                *(unsigned short*)(bufferStartPos + j) = *(unsigned short*)(imgStartPos + j);
            // counter++;
        }   

        //memcpy(bufferStartPos, imgStartPos, drawWidth * 2);
    }
}

void drawArray(char* destination, const char* source, unsigned int length) {

    int i;
    for (i = 0; i < length; i+=2)
    {
        if (*(unsigned short*)(source + i) != TRANSPARENT_COLOUR)
            *(unsigned short*)(destination + i) = *(unsigned short*)(source + i);
    }

}

void fill(char * destination, unsigned short colour, unsigned int size) {

    int i;
    for (i = 0; i < size; i+=2)
        *(unsigned short*)(destination + i) = colour;
}

void* memcpypos(void * destination, const void * source, unsigned int x, unsigned int y, unsigned int width, unsigned int bitsperpixel, unsigned int num) {
    memcpy(destination + (x + y*width)*bitsperpixel, source, num);
    return destination;
}

void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}
