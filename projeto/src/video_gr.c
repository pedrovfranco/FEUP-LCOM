#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdint.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "i8254.h"
#include "Bitmap.h"

#include "video_gr.h"

/* Constants for VBE 0x105 mode */


#define VRAM_PHYS_ADDR		0xE0000000
#define H_RES				1024
#define V_RES				768
#define HRES				1024 //Compatibility reasons
#define VRES				768  //
#define BITS_PER_PIXEL		8

/* Private global variables */

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */
static unsigned int h_res = H_RES;
static unsigned int v_res = V_RES;
static unsigned int bitsperpixel = BITS_PER_PIXEL;

unsigned int getXResolution() {
	return h_res;
}

unsigned int getYResolution() {
	return v_res;
}

unsigned int getBitsperpixel() {
	return bitsperpixel;
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void *vg_init(unsigned short mode) {

	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1 << 14 | mode; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != OK) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	vbe_mode_info_t info;
	vbe_get_mode_info(mode, &info);

	h_res = info.XResolution;
	v_res = info.YResolution;
	bitsperpixel = info.BitsPerPixel;

	printf("h_res = %u, v_res = %u, bitsperpixel = %u\n", h_res, v_res, bitsperpixel);

	int a;
	struct mem_range mr;
	unsigned int vram_base = info.PhysBasePtr; /* VRAM’s physical addresss */
	unsigned int vram_size = h_res * v_res * bitsperpixel / 8; // bits per pixel / 8 = bytes per pxiel

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (a = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", a);

	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED) {
		printf("couldn’t map video memory\n");
		return NULL;
	}

	//buffer = (char*) malloc(vram_size);


	return video_mem;
}

char* getVideoMem(){

	return video_mem;
}

void clear_screen(unsigned long color)
{
	unsigned short i, j;

	for (i = 0; i < h_res; i++){
		for(j = 0; j < v_res; j++){
			paint_pixel(i, j, color);
		}
	}

}
void paint_pixel(unsigned short x, unsigned short y, unsigned long color) {

	if (x >= 0 && y >= 0 && (x < (h_res - 1) || y < (v_res - 1))) {
		//printf("x = %u, y = %u\n", x, y);
		unsigned long * ptr = (unsigned long*) &(video_mem[(y * h_res + x)*bitsperpixel/8]);
		*ptr = color;
	}

}

void draw_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	//unsigned short x1 = x - (size / 2 + 0.5);
	//unsigned short y1 = y - (size / 2 + 0.5);

	unsigned short i, j;

	for (i = x; i < (x + size); i++) {
		for (j = y; j < (y + size); j++) {
			//printf("x = %u, y = %u\n", i, j);
			paint_pixel(i, j, color);
		}
	}

}
