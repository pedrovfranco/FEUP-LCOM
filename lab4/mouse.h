#ifndef __MOUSE_H
#define __MOUSE_H

int check_gesture(short length);

void print_mouse();

int mouse_subscribe_int();
int mouse_unsubscribe_int();

int mouse_handler();

int enable_packet_reading();
int disable_packet_reading();

int set_stream_mode();

int write_to_mouse(unsigned long command);

int write_to_kbc(unsigned long port, unsigned long cmd) ;


int mouse_handler_remote();
int set_remote_mode();

int read_kbc(unsigned long * data);


#endif /* __MOUSE_H */
