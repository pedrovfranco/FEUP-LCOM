#ifndef __KBD_H
#define __KBD_H

int kbd_subscribe_int();
int kbd_unsubscribe_int();

int print_code(unsigned long code, int *spcl);
unsigned long kbd_handler_scan(int *kernelcallcounter);
unsigned long kbd_handler_poll(int *kernelcallcounter);
int write_to_kbc(unsigned long port, unsigned long cmd);


#endif /* __KBD_H */
