#ifndef __TIMER_H
#define __TIMER_H

/*! Represents the timer */

typedef struct
{
	int ticked;/*!ticked flag =1 if there was timer interrupt */
	unsigned long counter; /*counter counter of number of mouse interrupts */
} Timer;

/**
 * @brief sets ticked flag
 *
 * @param timer timer
 * @param flag 1 or 0
 */
void setTicked(Timer* timer, int flag);

/**
 * @brief creates timer
 *
 * @return timer
 */
Timer* createTimer();
/**
 * @brief creates timer
 *
 * @param timer
 */

void deleteTimer(Timer* timer);

/**
 * @brief subscribes  timer0 interrupts
 *
 * @return 0 upon success
 */
int timer_subscribe_int(void );

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timerHandler(Timer* timer);


#endif /* __TIMER_H */
