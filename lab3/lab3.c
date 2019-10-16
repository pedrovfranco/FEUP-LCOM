#include "i8254.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test3.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);


int main(int argc, char **argv)
{
	sef_startup();
	/* DO NOT FORGET TO initialize service */

	 /* Enable IO-sensitive operations for ourselves */
	   sys_enable_iop(SELF);

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <unsigned short asm>\"\n"
			"\t service run %s -args \"poll <no args>\"\n"
			"\t service run %s -args \"timed <unsigned short n >\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	// unsigned long timer, freq, time;
	unsigned short a,n;

	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf(" wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		n = parse_ulong(argv[2], 10);
		if (n == USHRT_MAX)
			return 1;
		printf("lab3::kbd_test_scan\n");
		return kbd_test_scan(n);
	}
	else if (strncmp(argv[1], "poll", strlen("poll")) == 0) {
		if (argc != 2) {
			printf(" wrong no. of arguments for kbd_test_poll()\n");
			return 1;
		}
		else{
			printf("lab3::test_poll");
			return kbd_test_poll();

		}
	}
	else if (strncmp(argv[1], "timed", strlen("timed")) == 0) {
		if (argc != 3) {
			printf(" wrong no of arguments for timed_scan\n");
			return 1;
		}
		a = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (a == ULONG_MAX)
			return 1;
		printf("lab3::timed scan::\n");
		return kbd_test_timed_scan(a);
	}
	else {
		printf("KBD: %s - no valid function!\n", argv[1]);
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
