#include "i8254.h"
.intel_syntax
.global _kbd_handler_asm
.data

.text
	_kbd_handler_asm:
		xor eax, eax
		in al, KBD_OUT_BUF
		ret
