#ifndef _RTC_H
#define _RTC_H

#include "i8254.h"


/** @defgroup RTC RTC
 * @{
 * Functions to get the current day, month or year from the RTC
 */

/**
 * @ brief getsDay
 */
unsigned long getDay();

/**
 * @ brief gets Month
 */
unsigned long getMonth();

/**
 * @ brief gets year
 */
unsigned long getYear();


/**
 * @ brief write to rtc
 * @param port adress to write to
 * @param cmd command to write
 */
int write_to_port(unsigned long port, unsigned long cmd);

/**
 * @ brief reads data from rtc
 * @param port address
 * @param data*  pointer to data read from rtc
 */
int read_from_port(unsigned long port, unsigned long *data);

/**
 * @ converts bcd into binary
 */
int BCDtoBinary();


/**@}*/


#endif
