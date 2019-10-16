
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "read_xpm.h"

typedef struct {
  float x,y;             /**< current sprite position */
  int width, height;   /**< sprite dimensions */
  float xspeed, yspeed;  /**< current speeds in the x and y direction */
  char *map;           /**< the sprite pixmap (use read_xpm()) */
} Sprite;


Sprite *create_sprite(char *pic[], char *base);



void destroy_sprite(Sprite *fig, char *base);





#endif
