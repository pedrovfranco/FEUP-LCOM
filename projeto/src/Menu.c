#include "Menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "video_gr.h"
#include "i8254.h"
#include "kbd.h"
#include "RTC.h"
#include "Score.h"

// unsigned short transparent = TRANSPARENT_COLOUR;

Menu* newMenu() {
	Menu* menu = (Menu*) malloc(sizeof(Menu));

	menu->current_state = 0; // 0 = menu, 1 = game, 2 = high scores, 3 = about, 4 = game over, 5 = exit

	menu->drawSize = getXResolution()*getYResolution()*getBitsperpixel()/8;

	menu->video_mem = getVideoMem();
	
	menu->double_buf = malloc(menu->drawSize);
	menu->background_buf = malloc(menu->drawSize);
	menu->about_buf = malloc(menu->drawSize);
	menu->game_over_buf = malloc(menu->drawSize);
	menu->high_score_buf = malloc(menu->drawSize);

	flush_kbd();

	menu->timer_irq = timer_subscribe_int();
	printf("timer_irq: %u\n", menu->timer_irq);
	menu->kbd_irq = kbd_subscribe_int();
	printf("kbd_irq: %u\n", menu->kbd_irq);
	
	menu->gamestate = newGameState();

	int i;
	for (i = 0; i < 6; i++)
	{
		menu->high_scores[i] = newScore(-1);
	}

	menu->mouse = newMouse();
	menu->timer = createTimer();

	menu->scancode = 0;

	menu->background = loadBitmap("/home/images/menu.bmp");
	menu->about = loadBitmap("/home/images/about.bmp");
	menu->game_over = loadBitmap("/home/images/game_over.bmp");
	menu->high_score_bitmap = loadBitmap("/home/images/high_scores.bmp");

	drawBitmap(menu->background_buf, menu->background, 0,0, ALIGN_LEFT);
	drawBitmap(menu->about_buf, menu->about, 0,0, ALIGN_LEFT);
	drawBitmap(menu->game_over_buf, menu->game_over, 0,0, ALIGN_LEFT);
	drawBitmap(menu->high_score_buf, menu->high_score_bitmap, 0,0, ALIGN_LEFT);

	char* foo = "/home/images/letters/";
	char* bar = ".bmp";
	char* temp = malloc(27);
	for (i = 0; i < 26; i++)
	{
		strcpy(temp, foo);
		*(temp + strlen(foo)) = i + 'A';
		strcpy(temp + strlen(foo) + 1, bar);
		
		printf("String = %s\n", temp);
		
		menu->lettersandnumbers[i] = loadBitmap(temp);
	}

	for (i = 0; i < 10; i++)
	{
		strcpy(temp, foo);
		*(temp + strlen(foo)) = i + '0';
		strcpy(temp + strlen(foo) + 1, bar);
		
		printf("String = %s\n", temp);
		
		menu->lettersandnumbers[i + 26] = loadBitmap(temp);
	}
	menu->lettersandnumbers[36] = loadBitmap("/home/images/letters/slash.bmp");
	menu->lettersandnumbers[37] = loadBitmap("/home/images/letters/point.bmp");

	for (i = 0; i < 3; i++)
		menu->index[i] = 0;

	menu->flashing[0] = 0;	// -1 = not yet appearing, 0 = flashing, 1 = selected
	menu->flashing[1] = -1;
	menu->flashing[2] = -1;

	printf("Returned menu\n");
	return menu;
}


void updateMenu(Menu* menu) {
	
	timerHandler(menu->timer);

	if (menu->current_state == 1)
	{
		updateGameState(menu->gamestate);

		if (checkCollision(menu->gamestate))
		{
			loadHighScores(menu);
			menu->current_state = 4;
		}
	}
	else
	{
		drawMenu(menu);
	}

}

void updateMenuKbd(Menu* menu) {
	menu->scancode = kbd_handler_scan();

	if (menu->current_state == 0)
	{
		if (menu->scancode == KEY_ESC)
		{
			menu->current_state = 5;
		}
	}
	else if (menu->current_state == 1)
	{
		if (menu->scancode == KEY_ESC)
		{
			menu->current_state = 0;
		}

		updateGameStateKbd(menu->gamestate, menu->scancode);
	}
	else if (menu->current_state == 2)
	{
		if (menu->scancode == KEY_ESC)
		{
			menu->current_state = 0;
		}

	}
	else if (menu->current_state == 3)
	{
		if (menu->scancode == KEY_ESC)
		{
			menu->current_state = 0;
		}

	}
	else if (menu->current_state == 4)
	{
		if (menu->scancode == KEY_ESC)
		{
			menu->current_state = 0;
		}
		else if (menu->scancode == KEY_ARR_RIGHT)
		{
			int i;
			for (i = 0; i < 3; i++)
			{
				if (menu->flashing[i] == 0) // Flashing
				{
					menu->index[i] = (menu->index[i] + 1) % 36;
				}
			}
		}
		else if (menu->scancode == KEY_ARR_LEFT)
		{
			int i;
			for (i = 0; i < 3; i++)
			{
				if (menu->flashing[i] == 0) // Flashing
				{
					if (menu->index[i] == 0)
						menu->index[i] = 35;
					else
						menu->index[i] = (menu->index[i] - 1) % 36;
				}
			}
		}
		else if (menu->scancode == KEY_ENTER || menu->scancode == KEY_SPACE)
		{
			int i;
			for (i = 0; i < 3; i++)
			{
				if (menu->flashing[i] == 0) // Flashing
				{
					menu->flashing[i] = 1; // Selected

					if (i < 2) // If not last member of flashing, then the next letter is flashing
					{
						menu->flashing[i+1] = 0; // Flashing
					}
					else
					{
						loadHighScores(menu);
						addHighScore(menu);
						saveHighScores(menu);
						menu->current_state = 2;
					}

					break;
				}

			}
		}

	}

}

void updateMenuMouse(Menu* menu) {

	updateMouse(menu->mouse);

	if (menu->mouse->click == 1) // Menu button press
	{
		menu->mouse->click = 0;

		if (menu->current_state == 0)
		{
			if (menu->mouse->x > 358 && menu->mouse->x < 665 && menu->mouse->y > 254 && menu->mouse->y < 316) // Start Game
			{
				menu->current_state = 1;

				int i;
				for (i = 0; i < 3; i++)
					menu->index[i] = 0;

				menu->flashing[0] = 0;
				menu->flashing[1] = -1;
				menu->flashing[2] = -1;

				menu->gamestate = newGameState();
			}
			else if (menu->mouse->x > 358 && menu->mouse->x < 665 && menu->mouse->y > 345 && menu->mouse->y < 407) // High Scores
			{
				loadHighScores(menu);
				menu->current_state = 2;
			}
			else if (menu->mouse->x > 358 && menu->mouse->x < 665 && menu->mouse->y > 436 && menu->mouse->y < 497) // About
			{
				menu->current_state = 3;
			}
			else if (menu->mouse->x > 358 && menu->mouse->x < 665 && menu->mouse->y > 527 && menu->mouse->y < 588) // Exit
			{
				menu->current_state = 5;
			}
		}
		else if (menu->current_state == 2)
		{
			if (menu->mouse->x > 359 && menu->mouse->x < 666 && menu->mouse->y > 625 && menu->mouse->y < 687) // Exit
			{
				menu->current_state = 0;
			}
		}
		else if (menu->current_state == 3)
		{
			if (menu->mouse->x > 332 && menu->mouse->x < 640 && menu->mouse->y > 492 && menu->mouse->y < 554) // Exit
			{
				menu->current_state = 0;
			}
		}

	}

}

void drawMenu(Menu* menu) {

	if (menu->current_state == 0)
	{
		memcpy(menu->double_buf, menu->background_buf, menu->drawSize);
		drawMouse(menu->mouse, menu->double_buf);

		memcpy(menu->video_mem, menu->double_buf, menu->drawSize);
	}
	else if (menu->current_state == 1)
	{
		drawGameState(menu->gamestate);
	}
	else if (menu->current_state == 2)
	{
		memcpy(menu->double_buf, menu->high_score_buf, menu->drawSize);
		drawHighScores(menu);
		drawMouse(menu->mouse, menu->double_buf);
		memcpy(menu->video_mem, menu->double_buf, menu->drawSize);
	}
	else if (menu->current_state == 3)
	{
		memcpy(menu->double_buf, menu->about_buf, menu->drawSize);
		drawMouse(menu->mouse, menu->double_buf);

		memcpy(menu->video_mem, menu->double_buf, menu->drawSize);
	}
	else if (menu->current_state == 4)
	{
		memcpy(menu->double_buf, menu->game_over_buf, menu->drawSize);

		// drawMouse(menu->mouse, menu->double_buf);

		drawScoreMenu(menu);

		int i;
		for (i = 0; i < 3; i++)
		{
			if (menu->flashing[i] == 0) // flashing
			{
				if (menu->timer->counter % 60 > 20)
					drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->index[i]], 413 + 95*i , 481 - 3 - menu->lettersandnumbers[menu->index[i]]->bitmapInfoHeader.height, ALIGN_CENTER);
			}
			else if (menu->flashing[i] == 1) // selected
				drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->index[i]], 413 + 95*i , 481 - 3 - menu->lettersandnumbers[menu->index[i]]->bitmapInfoHeader.height, ALIGN_CENTER);
		}

		memcpy(menu->video_mem, menu->double_buf, menu->drawSize);
	}

}

void deleteMenu(Menu* menu) {
	printf("Entrou no deleteMenu\n");

	timer_unsubscribe_int();

	flush_kbd();
	kbd_unsubscribe_int();

	free(menu->double_buf);
	free(menu->background_buf);
	free(menu->about_buf);
	free(menu->game_over_buf);
	free(menu->high_score_buf);

	deleteGameState(menu->gamestate);

	int i;
	for (i = 0; i < 6; i++)
	{
		free(menu->high_scores[i]);
	}

	// flush_mouse(menu->mouse);
	deleteMouse(menu->mouse);
	
	deleteTimer(menu->timer);

	deleteBitmap(menu->background);
	deleteBitmap(menu->about);
	deleteBitmap(menu->game_over);
	deleteBitmap(menu->high_score_bitmap);

	for (i = 0; i < 38; i++)
	{
		deleteBitmap(menu->lettersandnumbers[i]);
	}
	
	flush_kbd();
	free(menu);

	printf("Saiu do deleteMenu\n");
}


void drawScoreMenu(Menu* menu) {
	int x = 458;
	int y = 265;

	drawBitmapTransparent(menu->double_buf, menu->gamestate->digits[(menu->gamestate->score->value/100) % 10], x ,y, ALIGN_LEFT);
	x += menu->gamestate->digits[(menu->gamestate->score->value/100) % 10]->bitmapInfoHeader.width + 3;
	drawBitmapTransparent(menu->double_buf, menu->gamestate->digits[(menu->gamestate->score->value/10) % 10], x ,y, ALIGN_LEFT);
	x += menu->gamestate->digits[(menu->gamestate->score->value/10) % 10]->bitmapInfoHeader.width + 3;
	drawBitmapTransparent(menu->double_buf, menu->gamestate->digits[menu->gamestate->score->value % 10], x ,y, ALIGN_LEFT);
	
}

void swapInt(int * a, int * b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void swapChar(char *a, char *b)
{
	char temp = *a;
	*a = *b;
	*b = temp;
}

void swapScore(Score **a, Score **b)
{
	Score *temp = *a;
	*a = *b;
	*b = temp;
}

int stoi(char* input) {
	int res = 0;

	int i, multiply = 1, length = strlen(input);
	for (i = length - 1; i >= 0; i--)
	{
		res += multiply*(input[i] - '0');
	
		multiply *= 10;
	}

	return res;
}

int loadHighScores(Menu* menu) {
	printf("Entrou no loadHighScores\n");

	if (menu->high_scores[0]->value > -1)
	{	
		printf("Entrou na condicao de exitencia dos high_scores\n");

		int i;
		for (i = 0; i < 6; i++) // Get number of valid scores
		{
			if (menu->high_scores[i]->value < 0)
				break;
		}
		int j, k, index;
		for (j = 0; j < i; ++j) // Sort numscore, keeps score - name pair intact
		{
			index = j;

			for (k = j + 1; k < i; ++k)
			{
				if (!(menu->high_scores[k]->value < menu->high_scores[index]->value))
				{
					index = k;
				}
			}

			swapScore(&menu->high_scores[index], &menu->high_scores[j]);

		}
		menu->high_scores[5]->value = -1; // "Removes" last element by making it invalid

		printf("Saiu na condicao de exitencia dos high_scores\n");
		return 1;
	}

	FILE* fp = fopen("/home/lcom/.high_score", "r");

	if (!(access("/home/lcom/.high_score", F_OK ) != -1))
	{
    	return 0;
	}

	printf("Started parser\n");
	char score[6][4];
	char name[6][4];
	char* str = "000,aaa,00,00,00"; // score,name,day,month,year
	char days[5][3], months[5][3], years[5][3];
	int i;
	for (i = 0; i < 5; i++)
	{
		if (fgets(str, strlen(str)+2, fp) == NULL) // End of stream
			break;

		memcpy(&score[i], str, 3);
		score[i][3] = 0;
		memcpy(&name[i], &str[4], 3);
		name[i][3] = 0;
		memcpy(&days[i], &str[8], 2);
		days[i][2] = 0;
		memcpy(&months[i], &str[11], 2);
		months[i][2] = 0;
		memcpy(&years[i], &str[14], 2);
		years[i][2] = 0;

	}

	fclose(fp);

	int j;

	for (j = 0; j < i; j++)
	{
		menu->high_scores[j]->value = stoi(score[j]);
		strcpy(menu->high_scores[j]->name, name[j]);

		menu->high_scores[j]->day = stoi(days[j]);
		menu->high_scores[j]->month = stoi(months[j]);
		menu->high_scores[j]->year = stoi(years[j]);
	}

	int index, k;

	for (j = 0; j < i; ++j) // Sort numscore, keeps score - name pair intact
	{
		index = j;

		for (k = j + 1; k < i; ++k)
		{
			if (!(menu->high_scores[k]->value < menu->high_scores[index]->value))
			{
				index = k;
			}
		}

		swapScore(&menu->high_scores[index], &menu->high_scores[j]);

	}
	menu->high_scores[5]->value = -1; // "Removes" last element by making it invalid

	printf("Saiu no loadHighScores\n");

	return 0;
}

void addHighScore(Menu* menu) {

	int i;
	for (i = 0; i < 5; i++)
	{
		if (menu->high_scores[i]->value < 0)
			break;
	}

	menu->high_scores[i]->value = menu->gamestate->score->value;

	menu->high_scores[i]->name[0] = menu->index[0] + 'A';
	menu->high_scores[i]->name[1] = menu->index[1] + 'A';
	menu->high_scores[i]->name[2] = menu->index[2] + 'A';

	menu->high_scores[i]->day = getDay();
	menu->high_scores[i]->month = getMonth();
	menu->high_scores[i]->year = getYear();

	int j, k, index;
	for (j = 0; j < i+1; ++j) // Sort numscore, keeps score - name pair intact
	{
		index = j;

		for (k = j + 1; k < i+1; ++k)
		{
			if (!(menu->high_scores[k]->value < menu->high_scores[index]->value))
			{
				index = k;
			}
		}

		swapScore(&menu->high_scores[index], &menu->high_scores[j]);

	}
	menu->high_scores[5]->value = -1; // "Removes" last element by making it invalid
}

void saveHighScores(Menu* menu) {

	FILE* fp = fopen("/home/lcom/.high_score", "w");

	int i;
	for (i = 0; i < 5; i++)
	{
		if (menu->high_scores[i]->value > -1)
			fprintf (fp, "%u%u%u,%c%c%c,%u%u,%u%u,%u%u\n", (menu->high_scores[i]->value/100) % 10, (menu->high_scores[i]->value/10) % 10, menu->high_scores[i]->value % 10, menu->high_scores[i]->name[0], menu->high_scores[i]->name[1], menu->high_scores[i]->name[2], (menu->high_scores[i]->day/10) % 10, menu->high_scores[i]->day % 10, (menu->high_scores[i]->month/10) % 10, menu->high_scores[i]->month % 10, (menu->high_scores[i]->year/10) % 10, menu->high_scores[i]->year % 10);
	}

	fclose(fp);
}

void drawHighScores(Menu* menu) {

	int x = 90, y = 165, newx;
	int i;
	for (i = 0; i < 5; ++i)
	{
		if (menu->high_scores[i]->value > -1)
		{
			newx = x;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[26 + i + 1], newx, y, ALIGN_LEFT); // Draw position
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[37], newx + menu->lettersandnumbers[26 + i + 1]->bitmapInfoHeader.width + 3, y + menu->lettersandnumbers[26 + i + 1]->bitmapInfoHeader.height - menu->lettersandnumbers[37]->bitmapInfoHeader.height, ALIGN_LEFT);
			newx += 150;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->value/100 % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->value/10 % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->value % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 80;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->name[0] - 'A'], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->name[1] - 'A'], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->name[2] - 'A'], newx, y, ALIGN_LEFT);
			newx += 80;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->day/10 % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->day % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[36], newx, y, ALIGN_LEFT);
			newx += 22;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->month/10 % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->month % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[36], newx, y, ALIGN_LEFT);
			newx += 22;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->year/10 % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;
			drawBitmapTransparent(menu->double_buf, menu->lettersandnumbers[menu->high_scores[i]->year % 10 + 26], newx, y, ALIGN_LEFT);
			newx += 50;

			y += 90;
		}
	}

}