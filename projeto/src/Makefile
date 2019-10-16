# Makefile for Project

COMPILER_TYPE= gnu

CC= gcc

PROG= projeto
SRCS= main.c
SRCS += vbe.c video_gr.c timer.c Mario.c kbd.c Pipe.c GameState.c Menu.c mouse.c Bitmap.c RTC.c Score.c

CFLAGS= -Wall 

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
