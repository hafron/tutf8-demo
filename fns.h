
typedef struct Glyph Glyph;
struct Glyph {
	char bytes[6];
	int n;
};

/*format_byte.c*/
int format_byte(char byte, int *bold, int *italic, int *underline, int *strikethrough, int *sup, int *sub, int *supsub);
int read_glyph(Glyph *glyph, FILE *source);
