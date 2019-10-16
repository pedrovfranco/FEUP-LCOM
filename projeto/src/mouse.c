#include "mouse.h"
#include "i8254.h"
#include "video_gr.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

int hook_id_mouse = MOUSE_HOOK_ID;


Mouse* newMouse() {
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->drawSize = getXResolution()*getYResolution()*getBitsperpixel()/8;
	mouse->mouse_buf = malloc(mouse->drawSize);

	mouse->x = getXResolution()/2;
	mouse->y = getYResolution()/2;
	mouse->LB = -1;
	mouse->MB = -1;
	mouse->RB = -1;
	mouse->click = -1;
	mouse->contador = 0;
	mouse->currlength = 0;

	mouse->mouse_irq = mouse_subscribe_int(mouse);
	printf("mouse_irq: %u\n", mouse->mouse_irq);

	enable_packet_reading(mouse);

	mouse->pointer = loadBitmap("/home/images/pointer.bmp");

	return mouse;
}

void updateMouse(Mouse* mouse) {

	int i;
	for (i = 0; i < 3; i++)
		mouse_handler(mouse);

	if ((mouse->packet[0] & BIT(4)) && mouse->packet[1] != 0)
		mouse->packet[1] |= 0xffffff00; //Negativo
	else
		mouse->packet[1] &= 0x000000ff; //Positivo		// print_mouse(mouse);

	if (((mouse->packet[0] & BIT(5)) && mouse->packet[2] != 0) || (mouse->packet[2] & BIT(7)))
		mouse->packet[2] |= 0xffffff00; //Negativo
	else
		mouse->packet[2] &= 0x000000ff; //Positivo

	if (abs(mouse->packet[1]) < 200 && (mouse->x + mouse->packet[1]) > 0 && (mouse->x + mouse->packet[1]) < getXResolution())
		mouse->x += mouse->packet[1];

	if (abs(mouse->packet[2]) < 200 && (mouse->y - mouse->packet[2]) > 0 && (mouse->y - mouse->packet[2]) < getYResolution())
		mouse->y -= mouse->packet[2];

	if (mouse->LB == 0 && (mouse->packet[0] & BIT(0)) >> 0 == 1) //LB passa de 0 para 1
		mouse->click = 1;
	else
		mouse->click = 0;
	
	mouse->LB = (mouse->packet[0] & BIT(0)) >> 0;
	mouse->MB = (mouse->packet[0] & BIT(1)) >> 1;
	mouse->RB = (mouse->packet[0] & BIT(2)) >> 2;

	// printf("LB = %d, MB = %d, RB = %d\n", mouse->LB, mouse->MB, mouse->RB);

}

void drawMouse(Mouse* mouse, char* buffer) {

	drawBitmapTransparent(buffer, mouse->pointer, mouse->x, mouse->y, ALIGN_LEFT);
}

void deleteMouse(Mouse* mouse) {
	free(mouse->mouse_buf);
	deleteBitmap(mouse->pointer);

	free(mouse);
}

int check_gesture(Mouse* mouse, short length) {
	
	if (((((int)mouse->packet[1] >= 0) && ((int)mouse->packet[2] >= 0)) || (((int)mouse->packet[1] <= 0) && ((int)mouse->packet[2] <= 0))) && ((mouse->packet[0] & BIT(1)) != 0))
	{
		mouse->currlength += abs(mouse->packet[1]);
		return (mouse->currlength >= length);
	}
	else
	{
		mouse->currlength = 0;
		return -1;
	}
}

int mouse_handler_remote(Mouse* mouse) {

	unsigned long data;

	//	printf("chegou ao hendler remote \n");

		int i;
		int a =1;
		while(a==1){
			a= write_to_mouse(mouse, READ_DATA);


		}

		for (i = 0; i < 3; i++) {

			read_kbc(KBD_OUT_BUF, &data);

			mouse->packet[i]=data;


		}

		tickdelay(micros_to_ticks(DELAY_US));

		return 0;
}

void print_mouse(Mouse* mouse) {

	printf("B1=0x%x B2=0x%x B3=0x%x LB=%d MB=%d RB=%d X0V=%d Y0V=%d", mouse->packet[0],
			mouse->packet[1], mouse->packet[2], mouse->packet[0] & BIT(0), (mouse->packet[0] & BIT(1)) >> 1,
			(mouse->packet[0] & BIT(2)) >> 2, (mouse->packet[0] & BIT(6)) >> 6,
			(mouse->packet[0] & BIT(7)) >> 7);

	// int temp = mouse->packet[1];

	// if ((mouse->packet[0] & BIT(4)) && mouse->packet[1] != 0)
	// 	mouse->packet[1] |= 0xffffff00; //Negativo
	// else
	// 	mouse->packet[1] &= 0x000000ff; //Positivo
	printf(" X = %d", mouse->packet[1]);

	//temp = mouse->packet[2];

	// if (((mouse->packet[0] & BIT(5)) && mouse->packet[2] != 0) || (mouse->packet[2] & BIT(7)))
	// 	mouse->packet[2] |= 0xffffff00; //Negativo
	// else
	// 	mouse->packet[2] &= 0x000000ff; //Positivo
		
	printf(" Y = %d\n", mouse->packet[2]);
}

int mouse_handler(Mouse* mouse) {

	unsigned long stat, data;
	int i;
	for (i = 0; i < 5; i++) { // Try 5 times
		if (sys_inb(STATUS_PORT, &stat) == 0) {
			if ((stat & OBF) && (stat & STATUS_AUX)) {
				if (sys_inb(KBD_OUT_BUF, &data) == 0) {
					if ((stat & (PAR_ERR | TO_ERR)) == 0) {
						{
							mouse->packet[mouse->contador] = data;
							mouse->contador = (mouse->contador + 1) % 3;
							return 0;
						}
					}
				}
			}
		}
		tickdelay(micros_to_ticks(1000 * 1));
	}

	return 1;
}

int enable_packet_reading(Mouse* mouse) {
	int error = write_to_mouse(mouse, ENABLE_PACKETS);

	if (error)
		printf("enable_mouse->packet_reading() error number %d\n", error);

	return error;
}

int disable_packet_reading(Mouse* mouse) {
	int error = write_to_mouse(mouse, DISABLE_PACKETS); // 0xF5

	if (error) {
		printf("disable_mouse->packet_reading() error number %d\n", error);
	}
	return error;
}

int mouse_subscribe_int(Mouse* mouse) {

	int temp = BIT(hook_id_mouse);

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_mouse) == OK)
		if (sys_irqenable(&hook_id_mouse) == OK)
			return temp;

	return -1;
}

int mouse_unsubscribe_int(Mouse* mouse) {

	if (sys_irqdisable(&hook_id_mouse) == OK)
		if (sys_irqrmpolicy(&hook_id_mouse) == OK)
			return 0;

	return 1;
}

int set_stream_mode(Mouse* mouse) {
	int error = write_to_mouse(mouse, ENABLE_SMODE); //0xEA
	if (error)
		printf("set_stream_mode() error number %d\n", error);

	return error;
	// teste  de ACK feito em write to mouse
}

int set_remote_mode(Mouse* mouse) {

	// ENABLE_REMOTE_MODE		0xF0

	// should it send 0xf5 before?
	int error = write_to_mouse(mouse, ENABLE_REMOTE_MODE);

	if (error) {
		printf("set_remote_mode() error number %d\n", error);
	}

	return error;
}

int write_to_mouse(Mouse* mouse, unsigned long command) {

	unsigned long data;

		retry:


		if (write_to_kbc(STATUS_PORT, WRITE_TO_MOUSE) == 1)
			return 1;

		if (write_to_kbc(KBD_IN_BUF, command) == 1)
			return 1;

		// data = 50;
		/**
		 while (1) {
		 if (sys_inb(STATUS_PORT, &stat) == 0) {
		 if ((stat & OBF)) {
		 break;

		 }
		 }
		 tickdelay(micros_to_ticks(DELAY_US));

		 }
		 */
		sys_inb(KBD_OUT_BUF, &data);


		if (data == ACK) {
			//printf("ACK found\n");
			// success
			return 0;
		} else {

			goto retry;
			printf("write_to_mouse() error number 0x%x\n", data);
			return 1; // ou voltar a enviar?
		}

}

int read_kbc(unsigned long port, unsigned long * data) {
	unsigned long stat;

	while (1) {
			if (sys_inb(STATUS_PORT, &stat))
				return 1;

			if (stat & OBF) {
				return sys_inb(port, data);
			}
			tickdelay(micros_to_ticks(DELAY_US));
		}

}

int write_to_kbc(unsigned long port, unsigned long cmd) {
	unsigned long stat;
		while (1) {
			sys_inb(STATUS_PORT, &stat);
			/* assuming it returns OK */
			/* loop while 8042 input buffer is not empty */
			if ((stat & IBF) == 0) {
				return sys_outb(port, cmd); /* no args command */
			}
			tickdelay(micros_to_ticks(DELAY_US));
		}

		printf("write_to_kbc() error number 0x%x\n", stat);
		return 1;
}


void flush_mouse(Mouse* mouse) {

	int i;
	for (i = 0; i < 5; i++)
	{
		mouse_handler(mouse);
	}

}