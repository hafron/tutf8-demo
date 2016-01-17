#include <stdio.h>
#include "fns.h"

enum {	HEADER = 0xC0, /*first two bits*/
	 FIRST = 0x80, /*first bit*/
	 BOLD = 0x20,
	 ITALIC = 0x10,
	 UNDERLINE = 0x08,
	 DELETE = 0x04,
	 SUP = 0x02,
	 SUB = 0x01,
	 };

int
format_byte(char byte, int *bold, int *italic, int *underline, int *delete, int *overline, int *sup, int *sub) {
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

int
ones(char byte) {
	int n;
	n = 0;
	while (byte & 0x80) {
		byte <<= 1;
		n++;
	}
	return n;
}

/* Function returns glyph length*/
int
read_glyph(Glyph *glyph, FILE *source) {
	char byte;
	int n, i;
	
	byte = fgetc(source);

	n = 0;
	/*we have read EOF*/
	if (byte == EOF)
		return 0;
	else if ((byte & FIRST) == 0) {
		glyph->n = 1;
		glyph->bytes[0] = byte;
		/*NULL byte at the end of char*/
		glyph->bytes[1] = '\0';
		return glyph->n;
	/*format byte*/
	} else if ((byte & HEADER) == 0x80) {
		glyph->bytes[n++] = byte;
		byte = fgetc(source);
	}
	
	glyph->n = n + 1;
	/*only one bit*/
	if ((byte & FIRST) == 0)
		glyph->bytes[1] = byte;
	else {
		glyph->n = n + ones(byte);
		glyph->bytes[n++] = byte;
		/*read bytes left*/
		for (i = n; i < glyph->n; i++) {
			byte = fgetc(source);
			glyph->bytes[i] = byte;
		}
	}
	/*NULL byte at the end of char*/
	glyph->bytes[glyph->n] = '\0';
	return glyph->n;
}

