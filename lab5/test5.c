#include "test5.h"
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
//#include <minix/com.h>
#include "i8254.h"
 #include "video_test.h"
#include "kbd.h"
#include <stdint.h>
#include <machine/int86.h>
#include "vbe.h"
#include "video_gr.h"
#include "timer.h"
#include "sprite.h"

static int counter = 0;

int video_test_init(unsigned short mode, unsigned short delay) {
	char *video_mem;

	video_mem = vg_init(mode);

	sleep(delay);

	if (vg_exit() != 0) {
		printf("test_init(): vg_exit() failed\n");
		return 1;
	}

	return 0;

}

int video_test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	char *video_mem;

	video_mem = vg_init(0x105);

	int irq_set;
	if ((irq_set = kbd_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;
	unsigned long key = 0;

	int kernelcallcounter = 0;

	draw_square(x, y, size, color);

	video_dump_fb();

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

					key = kbd_handler_scan(&kernelcallcounter);

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	if (kbd_unsubscribe_int())
		return 1;

	if (vg_exit() != 0) {
		printf("test_init(): vg_exit() failed\n");
		return 1;

	}
	printf("terminou\n ");

	return 0;

}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	char *video_mem;

	video_mem = vg_init(0x105);

	int irq_set;
	if ((irq_set = kbd_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;

	unsigned long key = 0;

	int kernelcallcounter = 0;

	draw_line(xi, yi, xf, yf, color);

	video_dump_fb();

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
					key = kbd_handler_scan(&kernelcallcounter);
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	if (kbd_unsubscribe_int())
		return 1;

	if (vg_exit() != 0) {
		printf("test_init(): vg_exit() failed\n");
		return 1;

	}
	printf("terminou\n ");

	return 0;
}

int test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	char *video_mem;

	video_mem = vg_init(0x105);

	int irq_set;
	if ((irq_set = kbd_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;

	unsigned long key = 0;

	int kernelcallcounter = 0;

	draw_xpm(xpm, xi, yi);

	video_dump_fb();

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
					key = kbd_handler_scan(&kernelcallcounter);
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	if (kbd_unsubscribe_int())
		return 1;

	if (vg_exit() != 0) {
		printf("\ttest_init(): vg_exit() failed\n");
		return 1;

	}
	printf("terminou\n ");

	return 0;
}

int test_move(char *xpm[], unsigned short xi, unsigned short yi,
		unsigned short xf, unsigned short yf, short speed,
		unsigned short frameRate) {

	if (xf > 1024 || xi > 1024 || yi > 768 || yf > 768) {
		printf("Wrong coordinates");
		return 1;
	}
	if (60 % frameRate != 0) {
		printf("wrong frame rate");
		return 1;
	}

	int rate;

	char *video_mem;

	video_mem = vg_init(0x105);

	Sprite *sp = create_sprite(xpm, video_mem); // cria sprite

	// set das posiçoes iniciais
	sp->x = xi;
	sp->y = yi;

	int direction;
	unsigned short finalPosition;

	// Set speed

	if(speed <0 ){
		 rate = 0-speed;

	}
	else
		rate = 60 / frameRate;


	if (xi == xf) {
		sp->yspeed = speed;
		sp->xspeed = 0;
		direction = 1;
		finalPosition = yf;

	} else if (yi == yf) {
		sp->xspeed = speed;
		sp->yspeed = 0;
		direction = 0;
		finalPosition = xf;
	}

	// DESENHA SPRITE NA POSIÇAO ININIAL

	draw_sprite(sp);

	int irq_set_kbd;
	int irq_set_timer;

	// subscribe kbd and timer
	if ((irq_set_timer = timer_subscribe_int()) == -1)
		return 1;

	if ((irq_set_kbd = kbd_subscribe_int()) == -1)
		return 1;

	int ipc_status;
	message msg;
	int r;

	unsigned long key = 0;

	int kernelcallcounter = 0;

	//video_dump_fb();

	while (key != KEY_ESC_BREAK) { /* You may want to use a different condition */
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
					if (counter % rate == 0) {
						animation_handler(sp, direction, speed, finalPosition);
					}
				}
				if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
					key = kbd_handler_scan(&kernelcallcounter);
					printf("encontrou esc");
				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}

		}

	}

	printf("Parou\n");

	destroy_sprite(sp, video_mem);

	if (kbd_unsubscribe_int())
		return 1;

	if (timer_unsubscribe_int())
		return 1;

	if (vg_exit() != 0) {
		printf("\ttest_init(): vg_exit() failed\n");
		return 1;

	}
	printf("terminou\n ");

	return 0;
}

int test_controller() {

	//vbe_mode_info_t* infoPtr = (vbe_mode_info_t*)lm_alloc(sizeof(vbe))

	return 0;
}
