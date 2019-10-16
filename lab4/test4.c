#include "test4.h"
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
//#include <minix/com.h>
#include "i8254.h"
#include "mouse.h"
#include "timer.h"



int mouse_test_packet(unsigned short cnt){

	int irq_set;
	if ((irq_set = mouse_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;

		// default is stream mode

	int i = 0;
	if(enable_packet_reading()==1)
		return 1;


	while (i <= (cnt*3)) { /* You may want to use a different condition */
			/* Get a request message. */
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /* received notification */
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

						i++;
						mouse_handler();
						if (i % 3 == 0)
							print_mouse();

					}
					break;
				default:
					break; /* no other notifications expected: do nothing */
				}
			}
		}

	if (disable_packet_reading() != 0)
		return 1;

	if(set_stream_mode()!=0)
		return 1;

	if (mouse_unsubscribe_int())
		return 1;

	return 0;
}	

int mouse_test_async(unsigned short idle_time) {

	unsigned int counter = 0;

	int irq_set_timer, irq_set_mouse;

	if ((irq_set_timer = timer_subscribe_int()) == -1)
		return 1;

	if ((irq_set_mouse = mouse_subscribe_int()) == -1)
		return 1;

	disable_packet_reading();
	enable_packet_reading();

	int ipc_status;
	message msg;
	int r;
	int i = 0;

	while (counter < (idle_time * 60)) { /* You may want to use a different condition */
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
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */
					counter = 0;
					i++;
					mouse_handler();
					if (i % 3 == 0)
						print_mouse();

				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}

		}

	}


	if (disable_packet_reading() != 0)
		return 1;

	if(set_stream_mode())
		return 1;

	if (mouse_unsubscribe_int())
		return 1;

	if (timer_unsubscribe_int())
		return 1;

	return 0;
}	

int mouse_test_remote(unsigned long period, unsigned short cnt){


	if (disable_packet_reading())
			return 1;

		// disable mouse interrupts on the KBC, by changing the KBC's Command Byte.

		if (write_to_kbc(KBC_CMD_REG, ENABLE_INTERRUPTS)) // port: 0x64 comando: 0x20
			return 1;

		unsigned long output;

		if (sys_inb(KBD_OUT_BUF, &output))
			return 1;

		output &= 0b11011111; // coloca o bit(1) a zero  --> disable mouse interrupts

		if (write_to_kbc(KBC_CMD_REG, KBD_WRITE_COMMAND))
			return 1;

		if (write_to_kbc(KBD_OUT_BUF, output))
			return 1;

		if (set_remote_mode())
			return 1;

		while (cnt > 0) {
			//	printf("while \n ");

			//	write_to_mouse(0xEB);

			mouse_handler_remote();

			cnt--;
			tickdelay(micros_to_ticks(period * 1000));
		}

		// reenable mouse interrupts:

		output |= BIT(1); // coloca o bit(1) a zero  --> disable mouse interrupts

		if (write_to_kbc(KBC_CMD_REG, KBD_WRITE_COMMAND))
			return 1;

		if (write_to_kbc(KBD_OUT_BUF, output))
			return 1;

		if (set_stream_mode())
			return 1;

		if (disable_packet_reading())
			return 1;
		/**
		 if (mouse_unsubscribe_int())
		 return 1;

		 printf("Deu bem\n");
		 */
		return 0;


}	

int mouse_test_gesture(short length){

	int irq_set;
	if ((irq_set = mouse_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;

		// default is stream mode

	int i = 0;
	if(enable_packet_reading()==1)
		return 1;
	
	int exitflag = 0;

	while (exitflag == 0 || i % 3 != 1) { /* You may want to use a different condition */
			/* Get a request message. */
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /* received notification */
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

						i++;
						mouse_handler();
						if (i % 3 == 0)
						{
							print_mouse();
							if (check_gesture(length) == 1)
								exitflag = 1;
						}

					}
					break;
				default:
					break; /* no other notifications expected: do nothing */
				}
			}
	}

	// unsigned long data;
	// read_kbc(&data);

	if (disable_packet_reading() != 0)
		return 1;

	if(set_stream_mode()!=0)
		return 1;

	if (mouse_unsubscribe_int())
		return 1;

	return 0;
}	
