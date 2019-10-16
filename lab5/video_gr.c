#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdint.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "i8254.h"
#include "lmlib.h"
#include "video_test.h"
#include "sprite.h"
#include "video_gr.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */

// #define VRAM_PHYS_ADDR		0xE0000000
// #define H_RES				1024
// #define V_RES				768
// #define HRES				1024 //Compatibility reasons
// #define VRES				768  //
// #define BITS_PER_PIXEL		8
/* Private global variables */

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */

//static unsigned h_res; /* Horizontal screen resolution in pixels */
//static unsigned v_res; /* Vertical screen resolution in pixels */
//static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void swap(unsigned short * a, unsigned short * b) {
	unsigned short temp = *a;
	*a = *b;
	*b = temp;
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
	r.u.w.bx = 1 << 14 | 0x105; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != OK) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	if (lm_init() == NULL)
		return NULL;

	mmap_t map;

	vbe_mode_info_t *infoPtr = (vbe_mode_info_t*) lm_alloc(sizeof(vbe_mode_info_t), &map);
	vbe_get_mode_info(mode, &(map.phys));

	int a;
	struct mem_range mr;
	unsigned int vram_base = infoPtr->PhysBasePtr; /* VRAM’s physical addresss */
	unsigned int vram_size = infoPtr->XResolution * infoPtr->YResolution * infoPtr->BitsPerPixel / 8; /* VRAM’s size, but you can use
	 the frame-buffer size, instead */

	//void *video_mem; /* frame-buffer VM address */

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (a = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", a);

	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED) {
		printf("couldn’t map video memory");
		return NULL;
	}

	printf("Video_mem adress: 0x%x", video_mem);

	return video_mem;
}

void paint_pixel(unsigned short x, unsigned short y, unsigned long color) {

	if (x > 0 && x < H_RES && y > 0 && y < V_RES) {/**
	 pointer = video_mem + y * (BITS_PER_PIXEL/8) * H_RES + x * (BITS_PER_PIXEL/8); // pointer to pixel
	 *pointer = (char) color; // paints
	 */
		unsigned long * ptr = (unsigned long*) &(video_mem[(y * H_RES + x)]);
		*ptr = color;
	}

}

void draw_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	unsigned short x1 = x - (size / 2 + 0.5);
	unsigned short y1 = y - (size / 2 + 0.5);

	int j;
	int i;
	for (j = x1; j < (x1 + size); j++) {
		for (i = y1; i < (y1 + size); i++) {
			paint_pixel(j + H_RES / 2, i + V_RES / 2, color);
		}
	}

}

void draw_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	double m = (double) (yf - yi) / (xf - xi);
	double b;

	unsigned short i;
	if (abs(m) < 1) {
		printf("abs(m) < 1\n");

		m = (double) (yf - yi) / (xf - xi);
		b = yi - m * xi;

		if (xi > xf)
			swap(&xi, &xf);

		for (i = xi; i <= xf; i++) {
			printf("x = %u y = %u\n", i, (unsigned short) (m * i + b + 0.5));
			paint_pixel(i, m * i + b + 0.5, color);
		}
	} else {
		printf("abs(m) > 1\n");

		m = (double) (xf - xi) / (yf - yi);
		b = xi - m * yi;

		if (yi > yf)
			swap(&yi, &yf);

		for (i = yi; i <= yf; i++) {
			printf("x = %u y = %u\n", (unsigned short) (m * i + b + 0.5), i);
			paint_pixel(m * i + b + 0.5, i, color); //Added 0.5 to round value instead of truncating it
		}
	}
}

void draw_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	int xsize, ysize;

	char *pixmap = read_xpm(xpm, &xsize, &ysize);

	printf("xsize = %d ysize = %d\n", xsize, ysize);
	int x, y;

	for (y = yi; y < yi + ysize; y++) {

		for (x = xi; x < xi + xsize; x++) {
			printf(
					"x = %d y = %d xcolor = %d ycolor = %d index = %d color = %d\n",
					x, y, (x - xi), (y - yi), (x - xi) + xsize * (y - yi),
					pixmap[(x - xi) + xsize * (y - yi)]);
			paint_pixel(x, y, pixmap[(x - xi) + xsize * (y - yi)]);
		}

	}

}

void draw_sprite(Sprite* sp) {

	// printf("xsize = %d ysize = %d\n", xsize, ysize);

	int x, y;
	int yi = (int ) sp->y;
	int xi = (int ) sp->x;

	for (y = yi; y < yi + sp->height; y++) {

		for (x = xi; x < xi + sp->width; x++) {

			paint_pixel(x, y, sp->map[(x - xi) + sp->width * (y - yi)]);
		}

	}

}

void clear_screen()
{
	int i;
	int j;
	unsigned long color = 0;


	for (i = 0; i < V_RES ; i++){
		for(j=0;j<H_RES ; j++){
			paint_pixel(i,j,color);
		}
	}

}

void animation_handler(Sprite *sp, int direction, short speed, unsigned short finalPosition) {
	// direction= 1 -> movement in yy axis
	// direction= 0 -> movement in xx axis

	if (direction == 0) {

		if (sp->x <= finalPosition) { // teste se chegou ao fim da animação
			if (speed > 0) {
				sp->x = sp->x + speed; // calculo da nova posição
			} else {
				// Speed < 0 TODO
				sp->x = sp->x + 1;
			}
			clear_screen();
			video_dump_fb();
			draw_sprite(sp);
		} else {
			printf("animação chegou ao fim");
		}

	} else if (direction == 1) {

		if (sp->y <= finalPosition) { // teste se chegou ao fim da animação
			if (speed > 0) {
				sp->y = sp->y + speed; // calculo da nova posição
			} else {
				sp->y = sp->y + 1;
			}
			clear_screen();
			draw_sprite(sp);
			video_dump_fb();

		} else {
			printf("animação chegou ao fim");
		}
	}

	// clear screen?
}
