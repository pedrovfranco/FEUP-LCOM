#include "i8254.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test4.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv) {
	sef_startup();
	/* DO NOT FORGET TO initialize service */

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

	if (argc == 1) { /* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv) {
printf("Usage: one of the following:\n"
		"\t service run %s -args \"packet <unsigned short cnt>\"\n"
		"\t service run %s -args \"async <unsigned short idle_time>\"\n"
		"\t service run %s -args \"remote <unsigned long period, unsigned short cnt >\"\n",
		"\t service run %s -args \"gesture < short length>\"\n",
		argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	unsigned short cnt,time;
	short len;

	unsigned long period;

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf(" wrong no. of arguments for mouse_test_packet()\n");
			return 1;
		}
		cnt = parse_ulong(argv[2], 10);
		if (cnt == USHRT_MAX)
			return 1;
		printf("lab4::mouse_test_packet()\n");
		return mouse_test_packet(cnt);
	} else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf(" wrong no. of arguments for mouse_test_async()\n");
			return 1;
		} else {

			time = parse_ulong(argv[2], 10);
			if (time == USHRT_MAX)
				return 1;
			printf("lab4::mouse_test_async()\n");
			return mouse_test_async(time);

		}
	} else if (strncmp(argv[1], "remote", strlen("remote")) == 0) {
		if (argc != 4) {
			printf(" wrong no of arguments for mouse_test_remote\n");
			return 1;
		}
		period = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (period == ULONG_MAX)
			return 1;
		cnt = parse_ulong(argv[3], 10); /* Parses string to unsigned long */

		if (cnt == ULONG_MAX)
			return 1;
		printf("lab4::mouse_test_remote()\n");
		return mouse_test_remote(period,cnt);
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
			if (argc != 3) {
				printf(" wrong no. of arguments for mouse_test_gesture()\n");
				return 1;
			} else {
				len = parse_ulong(argv[2], 10);
				if (len == USHRT_MAX)
					return 1;
				printf("lab4::mouse_test_async()\n");
				return mouse_test_gesture(len);

			}
		}


	else {
		printf("MOUSE: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
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
		printf("MOUSE: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
