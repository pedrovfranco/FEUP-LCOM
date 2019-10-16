#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <limits.h>

/** @defgroup i8254 i8254
 * @{
 *
 * Constants for the different peripherals of the computer, such as the timer, the keyboard, the mouse, the GPU or the RTC
 */

#define TIMER_HOOK_ID 		2
#define KBD_HOOK_ID 		0
#define MOUSE_HOOK_ID 		1



#define TIMER_FREQ  		1193182    /**< @brief clock frequency for timer in PC and AT */

#define BIT(n) 				(0x01<<(n))

#define TIMER0_IRQ			0    /**< @brief Timer 0 IRQ line */

/* I/O port addresses */

#define TIMER_0				0x40 /**< @brief Timer 0 count register */
#define TIMER_1				0x41 /**< @brief Timer 1 count register */
#define TIMER_2				0x42 /**< @brief Timer 2 count register */
#define TIMER_CTRL			0x43 /**< @brief Control register */

#define SPEAKER_CTRL		0x61 /**< @brief Register for speaker control  */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0			0x00     /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1			BIT(6)   /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2			BIT(7)   /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD		(BIT(7)|BIT(6))  /**< @brief Read Back Command */

/* Register selection: bits 5 and 4 */

#define TIMER_LSB			BIT(4)  /**< @brief Initialize Counter LSB only */
#define TIMER_MSB			BIT(5)  /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB		(TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */ 

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_SQR_WAVE		(BIT(2)|BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN		BIT(2)          /**< @brief Mode 2: rate generator */

/* Counting mode: bit 0 */

#define TIMER_BCD			0x01   /**< @brief Count in BCD */
#define TIMER_BIN			0x00   /**< @brief Count in binary */

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT		BIT(5)
#define TIMER_RB_STATUS_	BIT(4)
#define TIMER_RB_SEL(n)		BIT((n)+1)

/*-------------------------KEYBOARD-------------------------*/

#define KBD_IRQ 			1
#define KEY_ESC_MAKE 		0x01
#define KEY_ESC_BREAK 		0x81
#define KBD_OUT_BUF 		0x60
#define KBD_WRITE_COMMAND	0x60
#define STATUS_PORT 		0x64
#define KBC_CMD_REG 		0x64
#define ENABLE_INTERRUPTS 	0x20
#define OBF 				1
#define IBF 				BIT(1)
#define PAR_ERR 			BIT(7)
#define TO_ERR 				BIT(6)
#define SPCL_KEY 			0xe0
#define DELAY_US    		20000
#define KBD_IN_BUF			0x60


/*-------------------------MOUSE-------------------------*/
#define MOUSE_IRQ 			12
#define ACK					0xFA
#define DISABLE_SMODE		0xF5
#define WRITE_TO_MOUSE		0xD4
#define ENABLE_PACKETS		0xF4
#define DISABLE_PACKETS		0xF5
#define ENABLE_SMODE		0xEA
#define STATUS_REQUEST		0xE9
#define ENABLE_REMOTE_MODE	0xF0
#define READ_DATA		    0xEB
#define STATUS_AUX			BIT(5)


/*-------------------------Graphics-------------------------*/


#define mode1				0x101
#define mode2 				0x103
#define mode3 				0x105
#define mode4 				0x107
#define TRANSPARENT_COLOUR	USHRT_MAX


/*-------------------------RTC-------------------------*/

#define RTC_ADDR_REG 		0x70
#define RTC_DATA_REG 		0x71
#define REGISTER_A			10
#define REGISTER_B			11
#define SECONDS				0
#define MINUTES				2
#define HOURS				4
#define DAY 				7
#define MONTH				8
#define YEAR				9
#define UIP					BIT(7)
#define BCD_BIT				BIT(2)


/**@}*/

#endif /* _LCOM_I8254_H */
