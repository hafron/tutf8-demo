#include <stdio.h>

#include "SDL.h"

#include "fns.h"

enum {	HEADER = 0xC0, /*first two bits*/
	 BOLD = 0x20,
	 ITALIC = 0x10,
	 UNDERLINE = 0x08,
	 DELETE = 0x04,
	 SUP = 0x02,
	 SUB = 0x01,
	 };

int
format_byte(Uint8 byte, int *bold, int *italic, int *underline, int *delete, int *overline, int *sup, int *sub) {
	*bold = *italic = *underline = *delete = *overline = *sup = *sub = 0;
	if ((byte & HEADER) != 0x80)
		return 0;
	if (byte & BOLD)
		*bold = 1;
	if (byte & ITALIC)
		*italic = 1;
	if (byte & UNDERLINE)
		*underline = 1;
	if (byte & DELETE)
		*delete = 1;
	if ((byte & SUB) && (byte && SUP))
		*overline = 1;
	else if (byte & SUP)
		*sup = 1;
	else if (byte & SUB)
		*sub = 1;
		
	return 1;
}

/* Function returns glyph length*/
int
read_glyph(Glyph *glyph) {
	glyph->bytes[0] = 0xA0;
	glyph->bytes[1] = 0x41; /*A*/
	glyph->n = 2;
	return glyph->n;
}

Uint16
utf8_to_utf16(Uint8 *byte, int n) {
	Uint16 ch = 0;
	ch += *byte;
	return ch;
}
