#include <stdio.h>
//#include <timer.h>
#include <stdlib.h>
//#include <minix/drivers.h>

//#include <minix/drivers.h>

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>

#include <stdint.h>
#include <machine/int86.h>

#include "video_gr.h"
#include "kbd.h"
#include "Menu.h"
#include "RTC.h"

int main(int argc, char **argv) {
	/* Initialize service */
	sef_startup();

	sys_enable_iop(SELF);

	vg_init(0x117); // 0x105 = 1024x768 256 colours, 0x117 = 1024x768 16-bit colours 

	Menu* menu = newMenu();

	int ipc_status;
	message msg;
	int r;

	 while (menu->current_state != 5) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d\n", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & menu->timer_irq) {
					
					updateMenu(menu);

				}
				if (msg.NOTIFY_ARG & menu->kbd_irq) { 

					updateMenuKbd(menu);

				}
				if (msg.NOTIFY_ARG & menu->mouse->mouse_irq) {

					updateMenuMouse(menu);

				}

				break;

			default:
				break;

			}

		}

	}
	
	deleteMenu(menu);

	printf("Dia = %u, mes = %u, ano = %u\n", getDay(), getMonth(), getYear());
	
	vg_exit();


	return 0;

}
