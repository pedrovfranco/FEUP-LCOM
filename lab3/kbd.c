#include "kbd.h"
#include "i8254.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

int hook_id_kbd = 0;

int kbd_subscribe_int() {
	int temp = BIT(hook_id_kbd);

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd)
			== OK)
		if (sys_irqenable(&hook_id_kbd) == OK)
			return temp;

	return -1;
}

int kbd_unsubscribe_int() {

	if (sys_irqdisable(&hook_id_kbd) == OK)
		if (sys_irqrmpolicy(&hook_id_kbd) == OK)
			return 0;

	return 1;
}

int print_code(unsigned long code, int *spcl) {

	if (code == SPCL_KEY) {
		*spcl = 1;
		return 0;
	}

	if (*spcl) {

		if ((code & BIT(7)) >> 7)
			printf("Breakcode: 0xe0 0x%x \n", code);
		else
			printf("Makecode:  0xe0 0x%x \n", code);
		*spcl = 0;

	} else {

		if ((code & BIT(7)) >> 7)
			printf("Breakcode: 0x%x \n", code);
		else
			printf("Makecode:  0x%x \n", code);

	}

	return 0;
}

unsigned long kbd_handler_scan(int *kernelcallcounter) {
	unsigned long stat, data;
	unsigned int counter = 0;

	if (sys_inb(STATUS_PORT, &stat))
		return 1;

	counter++;

	if (stat & OBF) {
		if (sys_inb(KBD_OUT_BUF, &data))
			return 1;

		counter++;
		*kernelcallcounter = *kernelcallcounter + counter;

		if ((stat & (PAR_ERR | TO_ERR)) == 0)
			return data;
		else
			return -1;
	}
	tickdelay(micros_to_ticks(DELAY_US));

	return -1;
}

unsigned long kbd_handler_poll(int *kernelcallcounter) {
	unsigned long stat, data;
	unsigned int counter = 0;

	while (1) {
		if (sys_inb(STATUS_PORT, &stat))
			return 1;
		counter++;

		if ((stat & OBF) && (stat & BIT(5)) == 0) {
			if (sys_inb(KBD_OUT_BUF, &data))
				return 1;

			counter++;
			*kernelcallcounter = *kernelcallcounter + counter;

			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}

	return -1;
}

int write_to_kbc(unsigned long port, unsigned long cmd) {
	unsigned long stat;

	while (1) {
		if (sys_inb(STATUS_PORT, &stat))
			return 1;
		/* assuming it returns OK */
		/* loop while 8042 input buffer is not empty */
		if ((stat & IBF) == 0) {
			return sys_outb(port, cmd); /* no args command */
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}

	return 1;
}
