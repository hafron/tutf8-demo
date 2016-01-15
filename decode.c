#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"

#include "fns.h"

enum { PTSIZE = 18 };
enum { WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 768 };
enum { FONT_NORMAL, FONT_BOLD, FONT_ITALIC, FONT_BOLD_ITALIC, FONT_LEN };

char *font_names[FONT_LEN] = {"DejaVuSans", "DejaVuSans-Bold", "DejaVuSans-Oblique", "DejaVuSans-BoldOblique"};
TTF_Font *fonts[FONT_LEN];

typedef struct Scene Scene;
struct Scene {
	int w, h;
	SDL_Texture *text;
	SDL_Rect text_rect;
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
	/* Clear the background to background color */
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	
	SDL_RenderCopy(renderer, scene->text, NULL, &scene->text_rect);
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
        	case SDL_MOUSEBUTTONDOWN:
			scene->text_rect.x = e.button.x - scene->w/2;
			scene->text_rect.y = e.button.y - scene->h/2;
			scene->text_rect.w = scene->w;
			scene->text_rect.h = scene->h;
			draw_scene(renderer, scene);
			break;
        		case SDL_QUIT:
        			return 0;
        	}
	}	
}

SDL_Surface *
render_glyph(Glyph *g) {
	SDL_Color color = {0,0,0};
	int n;
	int bold, italic, underline, delete, overline, sup, sub;
	Uint8 *byte;
	Uint16 ch;
	
	n = g->n;
	byte = g->bytes;
	if (format_byte(*byte, &bold, &italic, &underline, &delete, &overline, &sup, &sub)) {
		n--;
		byte++;
	}
	ch = utf8_to_utf16(byte, n);
	
	if (bold && italic)
		return TTF_RenderGlyph_Solid(fonts[FONT_BOLD_ITALIC], ch, color);
	else if (bold)
		return TTF_RenderGlyph_Solid(fonts[FONT_BOLD], ch, color);
	else if (italic)
		return TTF_RenderGlyph_Solid(fonts[FONT_ITALIC], ch, color);
	else
		return TTF_RenderGlyph_Solid(fonts[FONT_NORMAL], ch, color);
}

void
render_text(SDL_Renderer *renderer, Scene *scene) {
	SDL_Surface *text;
	Glyph glyph;
	
	read_glyph(&glyph);
	text = render_glyph(&glyph);
	
	scene->text_rect.x = (WINDOW_WIDTH - text->w)/2;
	scene->text_rect.y = (WINDOW_HEIGHT - text->h)/2;
	scene->text_rect.w = text->w;
	scene->text_rect.h = text->h;

	scene->text = SDL_CreateTextureFromSurface(renderer, text);
	scene->w = text->w;
	scene->h = text->h;
}

int
main(void) {
	int ret;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Scene scene;
	
	/* Initialize the TTF library */
	if (TTF_Init() < 0 ) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	
	load_fonts();
	
	if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) < 0) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
		cleanup(2);
	}

	render_text(renderer, &scene);
	draw_scene(renderer, &scene);
	
	ret = scene_loop(renderer, &scene);
	
	close_fonts();
	cleanup(ret);
	
	return 0;
}
