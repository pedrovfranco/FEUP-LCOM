#include "RTC.h"

#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <assert.h>

#include <stdio.h>


unsigned long getDay() {

	unsigned long read, write = DAY;

	while(1)
	{
		write_to_port(RTC_ADDR_REG, REGISTER_A);
		read_from_port(RTC_DATA_REG, &read);

		if (!(read & UIP))
		{
			write_to_port(RTC_ADDR_REG, write);
			read_from_port(RTC_DATA_REG, &read);
			break;
		}
		tickdelay(20);
	}

	unsigned long res = read;

	write_to_port(RTC_ADDR_REG, REGISTER_B);
	read_from_port(RTC_DATA_REG, &read);

	if (read & BCD_BIT)
		return res;
	else
		return BCDtoBinary(res);
}

unsigned long getMonth() {

	unsigned long read, write = MONTH;

	while(1)
	{
		write_to_port(RTC_ADDR_REG, REGISTER_A);
		read_from_port(RTC_DATA_REG, &read);

		if (!(read & UIP))
		{
			write_to_port(RTC_ADDR_REG, write);
			read_from_port(RTC_DATA_REG, &read);
			break;
		}
		tickdelay(20);
	}

	unsigned long res = read;

	write_to_port(RTC_ADDR_REG, REGISTER_B);
	read_from_port(RTC_DATA_REG, &read);

	if (read & BCD_BIT)
		return res;
	else
		return BCDtoBinary(res);
}

unsigned long getYear() {

	unsigned long read, write = YEAR;

	while(1)
	{
		write_to_port(RTC_ADDR_REG, REGISTER_A);
		read_from_port(RTC_DATA_REG, &read);

		if (!(read & UIP))
		{
			write_to_port(RTC_ADDR_REG, write);
			read_from_port(RTC_DATA_REG, &read);
			break;
		}
		tickdelay(20);
	}

	unsigned long res = read;

	write_to_port(RTC_ADDR_REG, REGISTER_B);
	read_from_port(RTC_DATA_REG, &read);

	if (read & BCD_BIT)
		return res;
	else
		return BCDtoBinary(res);
}


int write_to_port(unsigned long port, unsigned long cmd) {

	if (sys_outb(port, cmd))
	{
		printf("RTC write error!\n");
		return 1;
	}

	return 0;
}


int read_from_port(unsigned long port, unsigned long *data) {

	if (sys_inb(port, data))
	{
		printf("RTC read error!\n");
		return 1;
	}

	return 0;

}

int BCDtoBinary(int input) {

    return ((input & 0xF0) >> 4) * 10 + (input & 0x0F);

}