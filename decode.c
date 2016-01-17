#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"

#include "fns.h"

enum { NGLYPHS = 100 };
enum { PTSIZE = 18 };
enum { WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 768 };
enum { FONT_NORMAL, FONT_BOLD, FONT_ITALIC, FONT_BOLD_ITALIC, FONT_LEN };

char *font_names[FONT_LEN] = {"DejaVuSans", "DejaVuSans-Bold", "DejaVuSans-Oblique", "DejaVuSans-BoldOblique"};
TTF_Font *fonts[FONT_LEN];

/*typedef struct Scene Scene;
struct Scene {
	int w, h;
	SDL_Texture *text;
	SDL_Rect text_rect;
};*/

typedef struct Scene Scene;
struct Scene {
	SDL_Texture *glyphs[NGLYPHS];
	SDL_Rect rects[NGLYPHS];
	int nglyphs;
};

void
cleanup(int code) {
	TTF_Quit();
	SDL_Quit();
	exit(code);
}

void
load_fonts(void) {
	int i;
	char path[100];
	for (i = 0; i < FONT_LEN; i++) {
		snprintf(path, sizeof path, "ttf/%s.ttf", font_names[i]);
		fonts[i] = TTF_OpenFont(path, PTSIZE);
		if (fonts[i] == NULL) {
			fprintf(stderr, "Couldn't load %d pt font from %s: %s\n", PTSIZE, path, SDL_GetError());
			cleanup(2);
		}
	}
}

void
close_fonts(void) {
	int i;
	for (i = 0; i < FONT_LEN; i++)
		TTF_CloseFont(fonts[i]);
}

void
draw_scene(SDL_Renderer *renderer, Scene *scene) {
	int i;
	
	/* Clear the background to background color */
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	
	for (i = 0; i < scene->nglyphs; i++)
		SDL_RenderCopy(renderer, scene->glyphs[i], NULL, &scene->rects[i]);
	SDL_RenderPresent(renderer);
}

int
scene_loop(SDL_Renderer *renderer, Scene *scene) {
	SDL_Event e;
	
	for(;;) {
		if (SDL_WaitEvent(&e) < 0) {
			fprintf(stderr, "SDL_PullEvent() error: %s\n", SDL_GetError());
			return 2;
        	}
        	switch (e.type) {
        	case SDL_QUIT:
        		return 0;
        	}
	}	
}

SDL_Surface *
render_glyph(Glyph *g) {
	SDL_Color color = {0,0,0}, bgcolor={0xff,0xff,0xff};
	int bold, italic, underline, delete, overline, sup, sub;
	char *byte;
	
	byte = g->bytes;
	if (format_byte(*byte, &bold, &italic, &underline, &delete, &overline, &sup, &sub))
		byte++;
	
	if (bold && italic)
		return TTF_RenderUTF8_Shaded(fonts[FONT_BOLD_ITALIC], byte, color, bgcolor);
	else if (bold)
		return TTF_RenderUTF8_Shaded(fonts[FONT_BOLD], byte, color, bgcolor);
	else if (italic)
		return TTF_RenderUTF8_Shaded(fonts[FONT_ITALIC], byte, color, bgcolor);
	else
		return TTF_RenderUTF8_Shaded(fonts[FONT_NORMAL], byte, color, bgcolor);
}

int
render_text(SDL_Renderer *renderer, Scene *scene, Glyph *glyphs, int nglyphs) {
	int i, pos;
	SDL_Surface *glyph_sur;
	
	pos = 0;
	scene->nglyphs = 0;
	for (i = 0; i < nglyphs; i++) {
		glyph_sur = render_glyph(&glyphs[i]);
		scene->glyphs[i] = SDL_CreateTextureFromSurface(renderer, glyph_sur);
		scene->rects[i].x = pos;
		scene->rects[i].y = 0;
		scene->rects[i].w = glyph_sur->w;
		scene->rects[i].h = glyph_sur->h;
		pos += glyph_sur->w;
		scene->nglyphs++;
		SDL_FreeSurface(glyph_sur);
	}
	return 0;
}

int
read_source(FILE *source, Glyph *glyph, int glyphs_max) {
	int nglyphs;
	nglyphs = 0;
	while (read_glyph(glyph++, source)) {
		if (nglyphs >= glyphs_max)
			return nglyphs;

		nglyphs++;
	}
	return nglyphs;
}

int
main(int argc, char *argv[]) {
	int ret, nglyphs;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Scene scene;
	Glyph glyphs[NGLYPHS];
	FILE *source;
	
	/* Initialize the TTF library */
	if (TTF_Init() < 0 ) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	if (argc == 1)
		nglyphs = read_source(stdin, glyphs, NGLYPHS);
	else {
		source = fopen(argv[1], "r");
		if (source == NULL) {
			fprintf(stderr, "Cannot open file: %s\n", argv[1]);
			cleanup(2);
		}
		nglyphs = read_source(source, glyphs, NGLYPHS);
		fclose(source);
	}
	load_fonts();
	if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) < 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
		cleanup(2);
	}

	ret = render_text(renderer, &scene, glyphs, nglyphs);
	if (ret > 0) {
		close_fonts();
		cleanup(ret);
	}
	draw_scene(renderer, &scene);
	
	ret = scene_loop(renderer, &scene);
	close_fonts();
	cleanup(ret);
	
	return 0;
}
