#include "test5.h"
#include "i8254.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "test5.h"
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);


int main(int argc, char **argv)
{
	sef_startup();
	/* DO NOT FORGET TO initialize service */

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init <unsigned short mode, unsigned short delay>\"\n"
			"\t service run %s -args \"square <unsigned short x, unsigned short y, unsigned short size, unsigned long color>\"\n"
			"\t service run %s -args \"line <unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color>\"\n",
			"\t service run %s -args \"xpm <char *xpm[], unsigned short xi, unsigned short yi>\"\n",
			"\t service run %s -args \"move <char *xpm[], unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, short s, unsigned short f>\"\n",
			"\t service run %s -args \"controller\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, f;
	unsigned long color;
	short s;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("wrong no. of arguments for video_test_init()\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 10);					/* Parses string to unsigned long */
		if (mode == ULONG_MAX)
			return 1;
		delay = parse_ulong(argv[3], 10);
		if (delay == ULONG_MAX)
			return 1;
		printf("video::video_test_init(%lu)\n",delay);
		return video_test_init(mode, delay);
	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("wrong no. of arguments for video_test_square()\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;
		y = parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;
		size = parse_ulong(argv[4], 10);						/* Parses string to unsigned long */
		if (size == ULONG_MAX)
			return 1;
		color = parse_ulong(argv[5], 10);						/* Parses string to unsigned long */
		if (color == ULONG_MAX)
			return 1;
		printf("video::video_test_square()\n");
		return video_test_square(x, y, size, color);
	}
	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("video: wrong no of arguments for video_test_line()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (xi == ULONG_MAX)
			return 1;
		yi = parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		if (yi == ULONG_MAX)
			return 1;
		xf = parse_ulong(argv[4], 10);						/* Parses string to unsigned long */
		if (xf == ULONG_MAX)
			return 1;
		yf = parse_ulong(argv[5], 10);						/* Parses string to unsigned long */
		if (yf == ULONG_MAX)
			return 1;
		color = parse_ulong(argv[6], 10);						/* Parses string to unsigned long */
		if (color == ULONG_MAX)
			return 1;
		printf("video::video_test_line()\n");
		return video_test_line(xi, yi, xf, yf, color);
	}
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf("video: wrong no of arguments for video_test_xpm()\n");
			return 1;
		}
		xi = parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		if (xi == ULONG_MAX)
			return 1;
		yi = parse_ulong(argv[4], 10);						/* Parses string to unsigned long */
		if (yi == ULONG_MAX)
			return 1;



		if (strncmp(argv[2], "pic1", strlen("pic1")) == 0)
		{
			printf("video::test_xpm()\n");
			return test_xpm(pic1 , xi, yi);		
		}

		else if (strncmp(argv[2], "pic2", strlen("pic2")) == 0)
		{	
			printf("video::test_xpm()\n");
			return test_xpm(pic2, xi, yi);
		}

		else if (strncmp(argv[2], "cross", strlen("cross")) == 0)
		{	
			printf("video::test_xpm()\n");
			return test_xpm(cross, xi, yi);
		}

		else if (strncmp(argv[2], "pic3", strlen("pic3")) == 0)
		{	
			printf("video::test_xpm()\n");
			return test_xpm(pic3, xi, yi);
		}

		else if (strncmp(argv[2], "penguin", strlen("penguin")) == 0)
		{	
			printf("video::test_xpm()\n");
			return test_xpm(penguin, xi, yi);
		}

		return 1;
	}

	else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc != 9) {
			printf("wrong no. of arguments for test_move()\n");
			return 1;
		}
		xi = parse_ulong(argv[3], 10);					/* Parses string to unsigned long */
		if (xi == ULONG_MAX)
			return 1;
		yi = parse_ulong(argv[4], 10);
		if (yi == ULONG_MAX)
			return 1;
		xf = parse_ulong(argv[5], 10);					/* Parses string to unsigned long */
		if (xf == ULONG_MAX)
			return 1;
		yf = parse_ulong(argv[6], 10);					/* Parses string to unsigned long */
		if (yf == ULONG_MAX)
			return 1;
		s = parse_ulong(argv[7], 10);					/* Parses string to unsigned long */
		if (s == ULONG_MAX)
			return 1;
		f = parse_ulong(argv[8], 10);					/* Parses string to unsigned long */
		if (f == ULONG_MAX)
			return 1;

		if (strncmp(argv[2], "pic1", strlen("pic1")) == 0)
		{	
			printf("video::test_move()\n");
			return test_move(pic1, xi, yi, xf, yf, s,  f);
		}
		else if (strncmp(argv[2], "pic2", strlen("pic2")) == 0)
		{	
			printf("video::test_move()\n");
			return test_move(pic2, xi, yi, xf, yf, s,  f);
		}
		else if (strncmp(argv[2], "cross", strlen("cross")) == 0)
		{	
			printf("video::test_move()\n");
			return test_move(cross, xi, yi, xf, yf, s,  f);
		}
		else if (strncmp(argv[2], "pic3", strlen("pic3")) == 0)
		{	
			printf("video::test_move()\n");
			return test_move(pic3, xi, yi, xf, yf, s,  f);
		}
		else if (strncmp(argv[2], "penguin", strlen("penguin")) == 0)
		{	
			printf("video::test_move()\n");
			return test_move(penguin, xi, yi, xf, yf, s,  f);
		}

		return 1;
	}

	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
		if (argc != 2) {
			printf("wrong no. of arguments for video_test_controller()\n");
			return 1;
		}

		printf("video::test_controller()\n");
		return test_controller();
	}
	else {
		printf("video: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);			

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
