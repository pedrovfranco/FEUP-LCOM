#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"
#include "timer.h"

unsigned long counter = 0;
static int hook_id = 0;

int timer_set_frequency(unsigned char timer, unsigned long freq) {

	if (timer < 0 || timer > 2) {
		printf("wrong timer");
		return 1;
	}

	unsigned long controlWord;
	unsigned long res;
	unsigned char conf;

	if (timer_get_conf(timer, &conf) == 1) {
		return 1;
	}

	res = TIMER_FREQ / freq;

	unsigned char lsb = (unsigned char) res;
	unsigned char msb = (unsigned char) (res >> 8);
	unsigned char saved;
	saved = (conf << 4) >> 4;

	if (timer == 0)
		controlWord = (TIMER_SEL0 | TIMER_LSB_MSB | saved);
	else if (timer == 1)
		controlWord = (TIMER_SEL1 | TIMER_LSB_MSB | saved);
	else if (timer == 2)
		controlWord = (TIMER_SEL2 | TIMER_LSB_MSB | saved);
	else
		return 1;

	if (sys_outb(TIMER_CTRL, controlWord) != 0) {
		return 1;
	}

	if (sys_outb(TIMER_0 + timer, lsb) != 0) {
		return 1;
	}
	if (sys_outb(TIMER_0 + timer, msb) != 0) {
		return 1;
	}

	return 0;
}

int timer_subscribe_int(void) {
	int temp = BIT(hook_id);

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) == OK)
		if (sys_irqenable(&hook_id) == OK)
			return temp;

	return -1;
}

int timer_unsubscribe_int() {

	if (sys_irqdisable(&hook_id) == OK)
		if (sys_irqrmpolicy(&hook_id) == OK)
			return 0;
	return 1;

}

void timer_int_handler() {
	counter++;

}

int timer_get_conf(unsigned char timer, unsigned char *st) {

	//Creating Read Back Command Mask
	unsigned long rbCommand = TIMER_RB_CMD | TIMER_RB_COUNT_
			| TIMER_RB_SEL(timer);

	// Need to write to the control register before accessing any
	// of the timers

	if (sys_outb(TIMER_CTRL, rbCommand) != 0) {
		return 1;
	}

	unsigned long info;

	if (timer == 0) {
		if (sys_inb(TIMER_0, &info) != 0)
			return 1;
	} else if (timer == 1) {
		if (sys_inb(TIMER_1, &info) != 0)
			return 1;
	} else if (timer == 2) {
		if (sys_inb(TIMER_2, &info) != 0)
			return 1;
	} else
		return 1;

	*st = (unsigned char) info;

	return 0;

}

int timer_display_conf(unsigned char conf) {

	printf("%s %d\n", "BCD : ", conf & 1);

	if ((conf & (3 << 1) >> 1) == 2 || (conf & (3 << 1) >> 1) == 3)
		printf("%s %d \n", "Programmed Mode : ", (conf & (3 << 1)) >> 1);
	else
		printf("%s %d \n", "Programmed Mode : ", (conf & (7 << 1)) >> 1);

	printf("%s", "Type of access : ");

	switch ((conf & (3 << 4)) >> 4) {
	case 0:
		printf("%s\n", "Counter Latch Command");
		break;

	case 1:
		printf("%s\n", "Read/Write least significant byte only");
		break;

	case 2:
		printf("%s\n", "Read/Write most significant byte only");
		break;

	case 3:
		printf("%s\n",
				"Read/Write least significant byte first, then most significant byte");
		break;

	}

	printf("%s %d\n", "Null count : ", (conf & (1 << 6)) >> 7);

	printf("%s %d\n", "Output : ", (conf & (1 << 7)) >> 7);

	return 0;
}

int timer_test_time_base(unsigned long freq) {

	if (timer_set_frequency(0, freq) == 1) {
		printf("Error in set_freq");
		return 1;
	}

	return 0;
}

int timer_test_int(unsigned long time) {

	int ipc_status;
	message msg;
	int r;
	int irq_set;
	if ((irq_set = timer_subscribe_int()) == -1){
		return 1;
	printf("chegou \n");
	}

	while (counter<(time*60)) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					// ... /* process it */
					timer_int_handler();
					if(counter % 60 ==0 )
						printf("Mensagem \n");
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (timer_unsubscribe_int() != OK)
		return 1;
	else
		return 0;

}

int timer_test_config(unsigned char timer) {

	unsigned char conf;

	if (timer < 0 || timer > 2) {
		printf("wrong timer");
		return 1;
	}

	if (timer_get_conf(timer, &conf) == 0) {
		if (timer_display_conf(conf) == 1)
			return 1;
	} else {
		printf("Error get_conf");
		return 1;
	}

	return 0;

}
