#pragma once

#include "i8254.h"

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps. Source:http://difusal.blogspot.pt/2014/07/minix-posts-index.html , author : Henrique Ferrolho
 */

// unsigned short transparent = TRANSPARENT_COLOUR;

/*! Alignment of the bitmap */
typedef enum {
    ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

/*! Bitmap File Header */
typedef struct {
    unsigned short type; /*!< specifies the file type */
    unsigned int size; /*!< specifies the size in bytes of the bitmap file */
    unsigned int reserved; /*!< reserved; must be 0 */
    unsigned int offset; /*!< specifies the offset in bytes from the bitmapfileheader to the bitmap bits */
} BitmapFileHeader;

/*! Bitmap Info Header */
typedef struct {
    unsigned int size; /*!< specifies the number of bytes required by the struct */
    int width; /*!< specifies width in pixels */
    int height; /*!< specifies height in pixels */
    unsigned short planes; /*!< specifies the number of color planes, must be 1 */
    unsigned short bits; /*!< specifies the number of bit per pixel */
    unsigned int compression; /*!< specifies the type of compression */
    unsigned int imageSize; /*!< size of image in bytes */
    int xResolution; /*!< number of pixels per meter in x axis */
    int yResolution; /*!< number of pixels per meter in y axis */
    unsigned int nColors; /*!< number of colors used by the bitmap */
    unsigned int importantColors; /*!< number of colors that are important */
} BitmapInfoHeader;

/*! Represents a bitmap */
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;  /*!< Pointer to the bitmap info header */
    unsigned char* bitmapData; /*!< Bitmap data */
} Bitmap;

/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws an unscaled, unrotated bitmap at the given position
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 * @param alignment image alignment
 */
void drawBitmap(char* destination, Bitmap* bitmap, int x, int y, Alignment alignment);
/**
 * @brief Draws an unscaled, unrotated bitmap at the given position with a transparent color
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 * @param alignment image alignment
 */
void drawBitmapTransparent(char* destination, Bitmap* bmp, int x, int y, Alignment alignment);

void drawArray(char* destination, const char* source, unsigned int length);

void fill(char * destination, unsigned short colour, unsigned int size);
/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 *
 * @param bitmap bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

/**@}*/
