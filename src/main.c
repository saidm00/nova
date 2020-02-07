#include <stdlib.h>
#include <stdio.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

/*
	The Ricoh 2A03 (RP2A03) CPU was used for the NTSC version
	of the Nintendo Entertainment System console.
	It's pretty much a R6502 though.
*/
typedef struct nova_r6502
{
	/* Accumulator */
	uint8_t A;

	/* Index Registers */
	uint8_t X, Y;

	/* Stack Pointer */
	uint8_t stkp;

	/* Program Counter */
	uint16_t pc;

	/* Status Register */
	uint8_t status;
} nova_r6502_t;

typedef enum nova_r6502_flags
{
	NOVA_R6502_FLAG_CARRY      = (1 << 0),
	MOVA_R6502_FLAG_ZERO       = (1 << 1),
	NOVA_R6502_FLAG_INTERRUPTS = (1 << 2),
	NOVA_R6502_FLAG_DECIMAL    = (1 << 3),
	NOVA_R6502_FLAG_BREAK      = (1 << 4),
	NOVA_R6502_FLAG_UNUSED     = (1 << 5),
	NOVA_R6502_FLAG_OVERFLOW   = (1 << 6),
	NOVA_R6502_FLAG_NEGATIVE   = (1 << 7),
} nova_r6502_flags_t;

void nova_r6502_set_flag(nova_r6502_t *cpu, nova_r6502_flags_t flag)
{
	cpu->status |= flag;
}

struct NovaRAM
{
	uint8_t *data;
	size_t size;
};

struct NovaRAM nova_create_ram(size_t byteCount)
{
	struct NovaRAM ram;
	ram.size = byteCount;
	ram.data = malloc(byteCount);
	return ram;
}

void nova_destroy_ram(struct NovaRAM *ram)
{
	free(ram->data);
}


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	

	SDL_Window *window = SDL_CreateWindow("nova", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	if(window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", "Failed to create window!");
	}
	

	SDL_Surface *window_surface = SDL_GetWindowSurface(window);
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(window_surface);

	SDL_Texture *nes_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

	/* Render onto NES's SDL texture */
	SDL_RenderClear(renderer);
	
	int format;
	SDL_QueryTexture(nes_tex, &format, NULL, NULL, NULL);
	SDL_PixelFormat *pixelFormat = SDL_AllocFormat(format);

	uint32_t *pixels;
	int pitch;
	if(SDL_LockTexture(nes_tex, NULL, (void**)&pixels, &pitch))
	{
		/* Failed to lock SDL texture */
	}

	int x = 5;
	int y = 25;
	
	int index = y * (pitch / sizeof(uint32_t)) + x;

	pixels[index] = SDL_MapRGBA(pixelFormat, 255, 0, 0, 255);

	SDL_UnlockTexture(nes_tex);

	SDL_RenderCopy(renderer, nes_tex, NULL, NULL);

	SDL_RenderPresent(renderer);
	SDL_UpdateWindowSurface(window);


	SDL_Delay(2000);

	SDL_Quit();
	return EXIT_SUCCESS;
}