#ifndef __KBD_H
#define __KBD_H




/**
 * @brief subscribes kbd interrupts
 *
 */
int kbd_subscribe_int();
/**
 * @brief unsubscribes kbd interrupts
 *
 */
int kbd_unsubscribe_int();
/**
 * @brief user frendly display of scancode given
 *@param  code scancode
 *@param  spcl flag if special key
 */
int print_code(unsigned long code, int *spcl);
/**
 * @brief hndler of the kbd interrupts
 * @return scancodeof pressed key
 */
unsigned long kbd_handler_scan();
/**
 * @brief empties outbuffer
 */
void flush_kbd();

/// Keys
typedef enum {
    KEY_NONE = 0x00,
    KEY_ESC = 0x01,
    KEY_1 = 0x02,
    KEY_2 = 0x03,
    KEY_3 = 0x04,
    KEY_4 = 0x05,
    KEY_5 = 0x06,
    KEY_6 = 0x07,
    KEY_7 = 0x08,
    KEY_8 = 0x09,
    KEY_9 = 0x0A,
    KEY_0 = 0x0B,
    KEY_APOSTROPHE = 0x0C,
    KEY_ANGLE_QUOTES = 0x0D,
    KEY_BKSP = 0x0E,
    KEY_TAB = 0x0F,
    KEY_Q = 0x10,
    KEY_W = 0x11,
    KEY_E = 0x12,
    KEY_R = 0x13,
    KEY_T = 0x14,
    KEY_Y = 0x15,
    KEY_U = 0x16,
    KEY_I = 0x17,
    KEY_O = 0x18,
    KEY_P = 0x19,
    KEY_PLUS = 0x1A,
    KEY_ACCENT = 0x1B,
    KEY_ENTER = 0x1C,
    KEY_L_CTRL = 0x1D,
    KEY_A = 0x1E,
    KEY_S = 0x1F,
    KEY_D = 0x20,
    KEY_F = 0x21,
    KEY_G = 0x22,
    KEY_H = 0x23,
    KEY_J = 0x24,
    KEY_K = 0x25,
    KEY_L = 0x26,
    KEY_C_CEDILLA = 0x27,
    KEY_ORDINAL = 0x28,
    KEY_BACKSLASH = 0x29,
    KEY_L_SHIFT = 0x2A,
    KEY_TILDE = 0x2B,
    KEY_Z = 0x2C,
    KEY_X = 0x2D,
    KEY_C = 0x2E,
    KEY_V = 0x2F,
    KEY_B = 0x30,
    KEY_N = 0x31,
    KEY_M = 0x32,
    KEY_COMMA = 0x33,
    KEY_DOT = 0x34,
    KEY_MINUS = 0x35,
    KEY_R_SHIFT = 0x36,
    KEY_ALT = 0x38,
    KEY_SPACE = 0x39,
    KEY_CAPS = 0x3A,
    KEY_F1 = 0x3B,
    KEY_F2 = 0x3C,
    KEY_F3 = 0x3D,
    KEY_F4 = 0x3E,
    KEY_F5 = 0x3F,
    KEY_F6 = 0x40,
    KEY_F7 = 0x41,
    KEY_F8 = 0x42,
    KEY_F9 = 0x43,
    KEY_F10 = 0x44,
    KEY_NUM = 0x45,
    KEY_SCRLL = 0x46,
    KEY_NUM_7 = 0x47,
    KEY_NUM_8 = 0x48,
    KEY_NUM_9 = 0x49,
    KEY_NUM_MINUS = 0x4A,
    KEY_NUM_4 = 0x4B,
    KEY_NUM_5 = 0x4C,
    KEY_NUM_6 = 0x4D,
    KEY_NUM_PLUS = 0x4E,
    KEY_NUM_1 = 0x4F,
    KEY_NUM_2 = 0x50,
    KEY_NUM_3 = 0x51,
    KEY_NUM_0 = 0x52,
    KEY_NUM_DEL = 0x53,
    KEY_MINOR = 0x56,
    KEY_F11 = 0x57,
    KEY_F12 = 0x58,
    KEY_NUM_ENTER = 0x1C,
    KEY_R_CTRL = 0x1D,
    KEY_NUM_SLASH = 0x35,
    KEY_ALT_GR = 0x38,
    KEY_HOME = 0x47,
    KEY_ARR_UP = 0x48,
    KEY_ARR_LEFT = 0x4B,
    KEY_ARR_RIGHT = 0x4D,
    KEY_ARR_DOWN = 0x50,
    KEY_PGUP = 0x49,
    KEY_PGDN = 0x51,
    KEY_INS = 0x52,
    KEY_DEL = 0x53,
    KEY_WIN = 0x5B,
    KEY_CNTX = 0x5D,
    KEY_END = 0x4F
} KEY;


/**@}*/

#endif /* __KBD_H */
