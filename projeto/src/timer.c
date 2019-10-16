#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>

#include "i8254.h"
#include "timer.h"

int hook_id =TIMER_HOOK_ID ;

Timer* createTimer() {

	Timer* timer = (Timer*)malloc(sizeof(Timer));

	timer->ticked = 0;
	timer->counter = 0;

	return timer;
}

void deleteTimer(Timer* timer) {
	free(timer);
}

void setTicked(Timer* timer, int flag){
	timer->ticked=flag;

}

int timer_subscribe_int(void) {
	int temp = BIT(hook_id);

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) == OK)
		if (sys_irqenable(&hook_id) == OK)
			return temp;

	return -1;
}

int timer_unsubscribe_int() {

	if (sys_irqdisable(&hook_id) == OK)
		if (sys_irqrmpolicy(&hook_id) == OK)
			return 0;
	return 1;

}

void timerHandler(Timer* timer) {

	timer->counter++;
	timer->ticked = 1;

}
