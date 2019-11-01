#include "../../headers/cpu.h"
#include "../../headers/ui/gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>

static SDL_Window *g_window = NULL;
static SDL_Renderer *g_renderer = NULL;
static SDL_Surface *g_surface = NULL;

uint8_t *ppu_memory = NULL;
processor_t *processor = NULL;

extern void set_processor(processor_t *p) {
  processor = p;
}

extern _Bool setup_ppu(void) {
  if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Error: %s", SDL_GetError());
    return 1;
  }
  g_window = SDL_CreateWindow("NES emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
  g_surface = SDL_GetWindowSurface(g_window);
  SDL_FillRect(g_surface, NULL, SDL_MapRGB(g_surface->format, 0xff, 0xff, 0xff));
  SDL_UpdateWindowSurface(g_window);
  SDL_Delay(2000);
  ppu_memory = calloc(0x4000, sizeof(uint8_t));
  return 0;
}


extern void free_ui(void) {
  if(g_window != NULL) {
    SDL_DestroyWindow(g_window);
    g_window = NULL;
  }
  if(g_renderer != NULL) {
    SDL_DestroyRenderer(g_renderer);
    g_renderer = NULL;
  }
  if(ppu_memory != NULL) {
    free(ppu_memory);
    ppu_memory = NULL;
  }
}
