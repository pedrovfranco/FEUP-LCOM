#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	struct reg86u r;
	phys_bytes blk = (phys_bytes)vmi_p;
	
	lm_init();	/* use liblm.a to initialize buf */

	r.u.w.ax = 0x4F01; /* VBE get mode info */
	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(blk); /* set a segment base */
	r.u.w.di = PB2OFF(blk); /* set the offset accordingly */
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;

	if( sys_int86(&r) == OK ) { /* call BIOS */
		printf("vbe_get_mode_info Success\n");
		vmi_p = blk;

	}
	else
		printf("vbe_get_mode_info fail\n");

	return 0;
}

