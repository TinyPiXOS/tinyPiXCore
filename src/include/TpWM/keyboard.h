#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "typesDef.h"

/**define keyboard string length**/
#define TP_KEYBOARD_STRING_LEN 32

enum
{
    TP_SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    TP_SCANCODE_A = 4,
    TP_SCANCODE_B = 5,
    TP_SCANCODE_C = 6,
    TP_SCANCODE_D = 7,
    TP_SCANCODE_E = 8,
    TP_SCANCODE_F = 9,
    TP_SCANCODE_G = 10,
    TP_SCANCODE_H = 11,
    TP_SCANCODE_I = 12,
    TP_SCANCODE_J = 13,
    TP_SCANCODE_K = 14,
    TP_SCANCODE_L = 15,
    TP_SCANCODE_M = 16,
    TP_SCANCODE_N = 17,
    TP_SCANCODE_O = 18,
    TP_SCANCODE_P = 19,
    TP_SCANCODE_Q = 20,
    TP_SCANCODE_R = 21,
    TP_SCANCODE_S = 22,
    TP_SCANCODE_T = 23,
    TP_SCANCODE_U = 24,
    TP_SCANCODE_V = 25,
    TP_SCANCODE_W = 26,
    TP_SCANCODE_X = 27,
    TP_SCANCODE_Y = 28,
    TP_SCANCODE_Z = 29,

    TP_SCANCODE_1 = 30,
    TP_SCANCODE_2 = 31,
    TP_SCANCODE_3 = 32,
    TP_SCANCODE_4 = 33,
    TP_SCANCODE_5 = 34,
    TP_SCANCODE_6 = 35,
    TP_SCANCODE_7 = 36,
    TP_SCANCODE_8 = 37,
    TP_SCANCODE_9 = 38,
    TP_SCANCODE_0 = 39,

    TP_SCANCODE_RETURN = 40,
    TP_SCANCODE_ESCAPE = 41,
    TP_SCANCODE_BACKSPACE = 42,
    TP_SCANCODE_TAB = 43,
    TP_SCANCODE_SPACE = 44,

    TP_SCANCODE_MINUS = 45,
    TP_SCANCODE_EQUALS = 46,
    TP_SCANCODE_LEFTBRACKET = 47,
    TP_SCANCODE_RIGHTBRACKET = 48,
    TP_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                 *   key on ISO keyboards and at the right end
                                 *   of the QWERTY row on ANSI keyboards.
                                 *   Produces REVERSE SOLIDUS (backslash) and
                                 *   VERTICAL LINE in a US layout, REVERSE
                                 *   SOLIDUS and VERTICAL LINE in a UK Mac
                                 *   layout, NUMBER SIGN and TILDE in a UK
                                 *   Windows layout, DOLLAR SIGN and POUND SIGN
                                 *   in a Swiss German layout, NUMBER SIGN and
                                 *   APOSTROPHE in a German layout, GRAVE
                                 *   ACCENT and POUND SIGN in a French Mac
                                 *   layout, and ASTERISK and MICRO SIGN in a
                                 *   French Windows layout.
                                 */
    TP_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                 *   instead of 49 for the same key, but all
                                 *   OSes I've seen treat the two codes
                                 *   identically. So, as an implementor, unless
                                 *   your keyboard generates both of those
                                 *   codes and your OS treats them differently,
                                 *   you should generate TP_SCANCODE_BACKSLASH
                                 *   instead of this code. As a user, you
                                 *   should not rely on this code because SDL
                                 *   will never generate it with most (all?)
                                 *   keyboards.
                                 */
    TP_SCANCODE_SEMICOLON = 51,
    TP_SCANCODE_APOSTROPHE = 52,
    TP_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                             *   and ISO keyboards). Produces GRAVE ACCENT and
                             *   TILDE in a US Windows layout and in US and UK
                             *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                             *   and NOT SIGN in a UK Windows layout, SECTION
                             *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                             *   layouts on ISO keyboards, SECTION SIGN and
                             *   DEGREE SIGN in a Swiss German layout (Mac:
                             *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                             *   DEGREE SIGN in a German layout (Mac: only on
                             *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                             *   French Windows layout, COMMERCIAL AT and
                             *   NUMBER SIGN in a French Mac layout on ISO
                             *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                             *   SIGN in a Swiss German, German, or French Mac
                             *   layout on ANSI keyboards.
                             */
    TP_SCANCODE_COMMA = 54,
    TP_SCANCODE_PERIOD = 55,
    TP_SCANCODE_SLASH = 56,

    TP_SCANCODE_CAPSLOCK = 57,

    TP_SCANCODE_F1 = 58,
    TP_SCANCODE_F2 = 59,
    TP_SCANCODE_F3 = 60,
    TP_SCANCODE_F4 = 61,
    TP_SCANCODE_F5 = 62,
    TP_SCANCODE_F6 = 63,
    TP_SCANCODE_F7 = 64,
    TP_SCANCODE_F8 = 65,
    TP_SCANCODE_F9 = 66,
    TP_SCANCODE_F10 = 67,
    TP_SCANCODE_F11 = 68,
    TP_SCANCODE_F12 = 69,

    TP_SCANCODE_PRINTSCREEN = 70,
    TP_SCANCODE_SCROLLLOCK = 71,
    TP_SCANCODE_PAUSE = 72,
    TP_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    TP_SCANCODE_HOME = 74,
    TP_SCANCODE_PAGEUP = 75,
    TP_SCANCODE_DELETE = 76,
    TP_SCANCODE_END = 77,
    TP_SCANCODE_PAGEDOWN = 78,
    TP_SCANCODE_RIGHT = 79,
    TP_SCANCODE_LEFT = 80,
    TP_SCANCODE_DOWN = 81,
    TP_SCANCODE_UP = 82,

    TP_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                    */
    TP_SCANCODE_KP_DIVIDE = 84,
    TP_SCANCODE_KP_MULTIPLY = 85,
    TP_SCANCODE_KP_MINUS = 86,
    TP_SCANCODE_KP_PLUS = 87,
    TP_SCANCODE_KP_ENTER = 88,
    TP_SCANCODE_KP_1 = 89,
    TP_SCANCODE_KP_2 = 90,
    TP_SCANCODE_KP_3 = 91,
    TP_SCANCODE_KP_4 = 92,
    TP_SCANCODE_KP_5 = 93,
    TP_SCANCODE_KP_6 = 94,
    TP_SCANCODE_KP_7 = 95,
    TP_SCANCODE_KP_8 = 96,
    TP_SCANCODE_KP_9 = 97,
    TP_SCANCODE_KP_0 = 98,
    TP_SCANCODE_KP_PERIOD = 99,

    TP_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                       *   keyboards have over ANSI ones,
                                       *   located between left shift and Y.
                                       *   Produces GRAVE ACCENT and TILDE in a
                                       *   US or UK Mac layout, REVERSE SOLIDUS
                                       *   (backslash) and VERTICAL LINE in a
                                       *   US or UK Windows layout, and
                                       *   LESS-THAN SIGN and GREATER-THAN SIGN
                                       *   in a Swiss German, German, or French
                                       *   layout. */
    TP_SCANCODE_APPLICATION = 101,    /**< windows contextual menu, compose */
    TP_SCANCODE_POWER = 102,          /**< The USB document says this is a status flag,
                                       *   not a physical key - but some Mac keyboards
                                       *   do have a power key. */
    TP_SCANCODE_KP_EQUALS = 103,
    TP_SCANCODE_F13 = 104,
    TP_SCANCODE_F14 = 105,
    TP_SCANCODE_F15 = 106,
    TP_SCANCODE_F16 = 107,
    TP_SCANCODE_F17 = 108,
    TP_SCANCODE_F18 = 109,
    TP_SCANCODE_F19 = 110,
    TP_SCANCODE_F20 = 111,
    TP_SCANCODE_F21 = 112,
    TP_SCANCODE_F22 = 113,
    TP_SCANCODE_F23 = 114,
    TP_SCANCODE_F24 = 115,
    TP_SCANCODE_EXECUTE = 116,
    TP_SCANCODE_HELP = 117,
    TP_SCANCODE_MENU = 118,
    TP_SCANCODE_SELECT = 119,
    TP_SCANCODE_STOP = 120,
    TP_SCANCODE_AGAIN = 121, /**< redo */
    TP_SCANCODE_UNDO = 122,
    TP_SCANCODE_CUT = 123,
    TP_SCANCODE_COPY = 124,
    TP_SCANCODE_PASTE = 125,
    TP_SCANCODE_FIND = 126,
    TP_SCANCODE_MUTE = 127,
    TP_SCANCODE_VOLUMEUP = 128,
    TP_SCANCODE_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     TP_SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     TP_SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     TP_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    TP_SCANCODE_KP_COMMA = 133,
    TP_SCANCODE_KP_EQUALSAS400 = 134,

    TP_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    TP_SCANCODE_INTERNATIONAL2 = 136,
    TP_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    TP_SCANCODE_INTERNATIONAL4 = 138,
    TP_SCANCODE_INTERNATIONAL5 = 139,
    TP_SCANCODE_INTERNATIONAL6 = 140,
    TP_SCANCODE_INTERNATIONAL7 = 141,
    TP_SCANCODE_INTERNATIONAL8 = 142,
    TP_SCANCODE_INTERNATIONAL9 = 143,
    TP_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    TP_SCANCODE_LANG2 = 145, /**< Hanja conversion */
    TP_SCANCODE_LANG3 = 146, /**< Katakana */
    TP_SCANCODE_LANG4 = 147, /**< Hiragana */
    TP_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    TP_SCANCODE_LANG6 = 149, /**< reserved */
    TP_SCANCODE_LANG7 = 150, /**< reserved */
    TP_SCANCODE_LANG8 = 151, /**< reserved */
    TP_SCANCODE_LANG9 = 152, /**< reserved */

    TP_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
    TP_SCANCODE_SYSREQ = 154,
    TP_SCANCODE_CANCEL = 155,
    TP_SCANCODE_CLEAR = 156,
    TP_SCANCODE_PRIOR = 157,
    TP_SCANCODE_RETURN2 = 158,
    TP_SCANCODE_SEPARATOR = 159,
    TP_SCANCODE_OUT = 160,
    TP_SCANCODE_OPER = 161,
    TP_SCANCODE_CLEARAGAIN = 162,
    TP_SCANCODE_CRSEL = 163,
    TP_SCANCODE_EXSEL = 164,

    TP_SCANCODE_KP_00 = 176,
    TP_SCANCODE_KP_000 = 177,
    TP_SCANCODE_THOUSANDSSEPARATOR = 178,
    TP_SCANCODE_DECIMALSEPARATOR = 179,
    TP_SCANCODE_CURRENCYUNIT = 180,
    TP_SCANCODE_CURRENCYSUBUNIT = 181,
    TP_SCANCODE_KP_LEFTPAREN = 182,
    TP_SCANCODE_KP_RIGHTPAREN = 183,
    TP_SCANCODE_KP_LEFTBRACE = 184,
    TP_SCANCODE_KP_RIGHTBRACE = 185,
    TP_SCANCODE_KP_TAB = 186,
    TP_SCANCODE_KP_BACKSPACE = 187,
    TP_SCANCODE_KP_A = 188,
    TP_SCANCODE_KP_B = 189,
    TP_SCANCODE_KP_C = 190,
    TP_SCANCODE_KP_D = 191,
    TP_SCANCODE_KP_E = 192,
    TP_SCANCODE_KP_F = 193,
    TP_SCANCODE_KP_XOR = 194,
    TP_SCANCODE_KP_POWER = 195,
    TP_SCANCODE_KP_PERCENT = 196,
    TP_SCANCODE_KP_LESS = 197,
    TP_SCANCODE_KP_GREATER = 198,
    TP_SCANCODE_KP_AMPERSAND = 199,
    TP_SCANCODE_KP_DBLAMPERSAND = 200,
    TP_SCANCODE_KP_VERTICALBAR = 201,
    TP_SCANCODE_KP_DBLVERTICALBAR = 202,
    TP_SCANCODE_KP_COLON = 203,
    TP_SCANCODE_KP_HASH = 204,
    TP_SCANCODE_KP_SPACE = 205,
    TP_SCANCODE_KP_AT = 206,
    TP_SCANCODE_KP_EXCLAM = 207,
    TP_SCANCODE_KP_MEMSTORE = 208,
    TP_SCANCODE_KP_MEMRECALL = 209,
    TP_SCANCODE_KP_MEMCLEAR = 210,
    TP_SCANCODE_KP_MEMADD = 211,
    TP_SCANCODE_KP_MEMSUBTRACT = 212,
    TP_SCANCODE_KP_MEMMULTIPLY = 213,
    TP_SCANCODE_KP_MEMDIVIDE = 214,
    TP_SCANCODE_KP_PLUSMINUS = 215,
    TP_SCANCODE_KP_CLEAR = 216,
    TP_SCANCODE_KP_CLEARENTRY = 217,
    TP_SCANCODE_KP_BINARY = 218,
    TP_SCANCODE_KP_OCTAL = 219,
    TP_SCANCODE_KP_DECIMAL = 220,
    TP_SCANCODE_KP_HEXADECIMAL = 221,

    TP_SCANCODE_LCTRL = 224,
    TP_SCANCODE_LSHIFT = 225,
    TP_SCANCODE_LALT = 226, /**< alt, option */
    TP_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    TP_SCANCODE_RCTRL = 228,
    TP_SCANCODE_RSHIFT = 229,
    TP_SCANCODE_RALT = 230, /**< alt gr, option */
    TP_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

    TP_SCANCODE_MODE = 257, /**< I'm not sure if this is really not covered
                             *   by any of the above, but since there's a
                             *   special KMOD_MODE for it I'm adding it here
                             */

    /* @} */ /* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    TP_SCANCODE_AUDIONEXT = 258,
    TP_SCANCODE_AUDIOPREV = 259,
    TP_SCANCODE_AUDIOSTOP = 260,
    TP_SCANCODE_AUDIOPLAY = 261,
    TP_SCANCODE_AUDIOMUTE = 262,
    TP_SCANCODE_MEDIASELECT = 263,
    TP_SCANCODE_WWW = 264,
    TP_SCANCODE_MAIL = 265,
    TP_SCANCODE_CALCULATOR = 266,
    TP_SCANCODE_COMPUTER = 267,
    TP_SCANCODE_AC_SEARCH = 268,
    TP_SCANCODE_AC_HOME = 269,
    TP_SCANCODE_AC_BACK = 270,
    TP_SCANCODE_AC_FORWARD = 271,
    TP_SCANCODE_AC_STOP = 272,
    TP_SCANCODE_AC_REFRESH = 273,
    TP_SCANCODE_AC_BOOKMARKS = 274,

    /* @} */ /* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    TP_SCANCODE_BRIGHTNESSDOWN = 275,
    TP_SCANCODE_BRIGHTNESSUP = 276,
    TP_SCANCODE_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    TP_SCANCODE_KBDILLUMTOGGLE = 278,
    TP_SCANCODE_KBDILLUMDOWN = 279,
    TP_SCANCODE_KBDILLUMUP = 280,
    TP_SCANCODE_EJECT = 281,
    TP_SCANCODE_SLEEP = 282,

    TP_SCANCODE_APP1 = 283,
    TP_SCANCODE_APP2 = 284,

    /* @} */ /* Walther keys */

    /* Add any other keys here. */

    TP_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
};

#define PIK_SCANCODE_MASK (1 << 30)
#define TP_SCANCODE_TO_KEYCODE(X) (X | PIK_SCANCODE_MASK)

/**
 * \brief Enumeration of valid key mods (possibly OR'd together).
 */
enum KeyModeType
{
    TP_KMOD_NONE = 0x0000,
    TP_KMOD_LSHIFT = 0x0001,
    TP_KMOD_RSHIFT = 0x0002,
    TP_KMOD_LCTRL = 0x0040,
    TP_KMOD_RCTRL = 0x0080,
    TP_KMOD_LALT = 0x0100,
    TP_KMOD_RALT = 0x0200,
    TP_KMOD_LGUI = 0x0400,
    TP_KMOD_RGUI = 0x0800,
    TP_KMOD_NUM = 0x1000,
    TP_KMOD_CAPS = 0x2000,
    TP_KMOD_MODE = 0x4000,
    TP_KMOD_RESERVED = 0x8000
};

#endif
