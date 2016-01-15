
typedef struct Glyph Glyph;
struct Glyph {
	Uint8 bytes[5];
	int n;
};

/*format_byte.c*/
int format_byte(unsigned char byte, int *bold, int *italic, int *underline, int *delete, int *overline, int *sup, int *sub);
int read_glyph(Glyph *glyph);
Uint16 utf8_to_utf16(Uint8 *byte, int n);
