#include "test3.h"
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
//#include <minix/com.h>
#include "i8254.h"
#include "timer.h"
#include "kbd.h"

int kernelcallcounter = 0;

int kbd_handler_asm();
int kbd_test_scan(unsigned short assembly) {

	if (assembly < 0 || assembly > 1)
	{
		printf("Wrong argument, assembly must be bool\n");
		return 1;
	}

	kernelcallcounter = 0;

	int irq_set;
	if ((irq_set = kbd_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;
	unsigned long key = 0;
	int spcl = 0;

	while (key != KEY_ESC_BREAK) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d\n", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (assembly)
						key = kbd_handler_asm();
					else
						key = kbd_handler_scan(&kernelcallcounter);

					if (key != -1)
						print_code(key, &spcl);
					else
						printf("Status error!\n");
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	if (kbd_unsubscribe_int())
		return 1;

	if (!assembly)
		printf("Number of kernel calls: %d\n", kernelcallcounter);

	return 0;
}

int kbd_test_poll() {

	kernelcallcounter = 0;

	unsigned long key = 0;
	int spcl = 0;

	printf("\n");

	while (key != KEY_ESC_BREAK) {
		key = kbd_handler_poll(&kernelcallcounter);

		if (key != -1)
			print_code(key, &spcl);
		else
			printf("Status Error!\n");

	}

	printf("Number of kernel calls: %d\n", kernelcallcounter);

	if (write_to_kbc(KBC_CMD_REG, ENABLE_INTERRUPTS))
		return 1;

	unsigned long output;

	if (sys_inb(KBD_OUT_BUF, &output))
		return 1;

	output |= 0x01;

	if (write_to_kbc(KBC_CMD_REG, KBD_WRITE_COMMAND))
		return 1;

	if (write_to_kbc(KBD_OUT_BUF, output))
		return 1;

	return 0;
}

int kbd_test_timed_scan(unsigned short n) {

	unsigned int counter = 0;

	int irq_set_kbd;
	int irq_set_timer;

	if ((irq_set_timer = timer_subscribe_int()) == -1)
		return 1;

	if ((irq_set_kbd = kbd_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;
	unsigned long key = 0;
	int spcl = 0;

	while (key != KEY_ESC_BREAK && counter < (n * 60)) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d\n", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_timer) {
					counter++;
				}
				if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
					counter = 0;
					key = kbd_handler_scan(&kernelcallcounter);

					if (key != -1)
						print_code(key, &spcl);
					else
						printf("Status error!\n");
				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}

		}

	}

	printf("Parou\n");

	if (kbd_unsubscribe_int())
		return 1;

	if (timer_unsubscribe_int())
		return 1;


	return 0;
}
