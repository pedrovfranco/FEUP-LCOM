#include "mouse.h"
#include "i8254.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

int hook_id_mouse = 1;

unsigned long packet[3];
int contador = 0;

int currlength = 0;


int check_gesture(short length) {
	
	if (((((int)packet[1] >= 0) && ((int)packet[2] >= 0)) || (((int)packet[1] <= 0) && ((int)packet[2] <= 0))) && ((packet[0] & BIT(1)) != 0))
	{
		currlength += abs(packet[1]);
		return (currlength >= length);
	}
	else
	{
		currlength = 0;
		return -1;
	}
}

int mouse_handler_remote() {

	unsigned long data;

	//	printf("chegou ao hendler remote \n");

		int i;
		int a =1;
		while(a==1){
			a= write_to_mouse(READ_DATA);


		}

		for (i = 0; i < 3; i++) {

			read_kbc(&data);

			packet[i]=data;


		}


		print_mouse();
		tickdelay(micros_to_ticks(DELAY_US));



		return 0;
}

void print_mouse() {

	printf("B1=0x%x B2=0x%x B3=0x%x LB=%d MB=%d RB=%d X0V=%d Y0V=%d", packet[0],
			packet[1], packet[2], packet[0] & BIT(0), (packet[0] & BIT(1)) >> 1,
			(packet[0] & BIT(2)) >> 2, (packet[0] & BIT(6)) >> 6,
			(packet[0] & BIT(7)) >> 7);

	// int temp = packet[1];

	if ((packet[0] & BIT(4)) && packet[1] != 0)
		packet[1] |= 0xffffff00; //Negativo
	else
		packet[1] &= 0x000000ff; //Positivo
	printf(" X = %d", packet[1]);

	//temp = packet[2];

	if (((packet[0] & BIT(5)) && packet[2] != 0) || (packet[2] & BIT(7)))
		packet[2] |= 0xffffff00; //Negativo
	else
		packet[2] &= 0x000000ff; //Positivo
		
		printf(" Y = %d\n", packet[2]);
}

int mouse_handler() {
	unsigned long stat, data;
	int i;
	for (i = 0; i < 5; i++) {
		if (sys_inb(STATUS_PORT, &stat) == 0) {
			if ((stat & OBF)) {
				if (sys_inb(KBD_OUT_BUF, &data) == 0) {
					if ((stat & (PAR_ERR | TO_ERR)) == 0) {
						{
							packet[contador] = data;
							contador = (contador + 1) % 3;
							return 0;
						}
					}
				}
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}

	return 1;
}

int enable_packet_reading() {
	int error = write_to_mouse(ENABLE_PACKETS);

	if (error)
		printf("enable_packet_reading() error number %d\n", error);

	return error;
}

int disable_packet_reading() {
	int error = write_to_mouse(DISABLE_PACKETS); // 0xF5

	if (error) {
		printf("disable_packet_reading() error number %d\n", error);
	}
	return error;
}

int mouse_subscribe_int() {

	int temp = BIT(hook_id_mouse);

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_mouse) == OK)
		if (sys_irqenable(&hook_id_mouse) == OK)
			return temp;

	return -1;
}

int mouse_unsubscribe_int() {

	if (sys_irqdisable(&hook_id_mouse) == OK)
		if (sys_irqrmpolicy(&hook_id_mouse) == OK)
			return 0;

	return 1;
}

int set_stream_mode() {
	int error = write_to_mouse(ENABLE_SMODE); //0xEA
	if (error)
		printf("set_stream_mode() error number %d\n", error);

	return error;
	// teste  de ACK feito em write to mouse
}

int set_remote_mode() {

	// ENABLE_REMOTE_MODE		0xF0

	// should it send 0xf5 before?
	int error = write_to_mouse(ENABLE_REMOTE_MODE);

	if (error) {
		printf("set_remote_mode() error number %d\n", error);
	}

	return error;
}

int write_to_mouse(unsigned long command) {

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

int read_kbc(unsigned long * data) {
	unsigned long stat;

	while (1) {
			if (sys_inb(STATUS_PORT, &stat))
				return 1;

			if (stat & OBF) {
				return sys_inb(KBD_OUT_BUF, data);
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
