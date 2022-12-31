#ifndef _NTR_COLOR_H_
#define _NTR_COLOR_H_ 1
#include "common.h"

#define RGB_FG(R, G, B) ESC"[38;2;" #R ";" #G ";" #B "m"
#define RGB_BG(R, G, B) ESC"[48;2;" #R ";" #G ";" #B "m"
#define CRESET ESC"[0m"
#define FG_RED ESC"[38;5;1m"
#define BG_RED ESC"[48;5;1m"

#endif // _NTR_COLOR_H_

