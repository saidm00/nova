#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//#include "nova.h"

//#define SDL_MAIN_HANDLED
//#include <SDL.h>

/* Pins of 6502 */
typedef struct nova_m6502_pins
{
	/*unsigned VSS : 1;*/
	unsigned RDY : 1;
	/*unsigned phi_1_OUT : 1;*/ /* Unused */
	unsigned IRQ : 1; /* IRQ Interrupt */
	/*unsigned NC1 : 1;*/ /* No Connection pin */
	unsigned NMI : 1;
	unsigned SYNC : 1;
	/*unsigned VCC : 1;*/ /* Unused */
	
	/* 64K addressable bytes of memory (A0-A15) */
	union{
	struct{	unsigned A0  : 1;
			unsigned A1  : 1;
			unsigned A2  : 1;
			unsigned A3  : 1;
			unsigned A4  : 1;
			unsigned A5  : 1;
			unsigned A6  : 1;
			unsigned A7  : 1;
			unsigned A8  : 1;
			unsigned A9  : 1;
			unsigned A10 : 1;
			unsigned A11 : 1;
			/*unsigned VSS : 1;*/ /* Odd? */
			unsigned A12 : 1;
			unsigned A13 : 1;
			unsigned A14 : 1;
			unsigned A15 : 1;};
		uint16_t addr;	
	} ADDR;
	/* Data bus */
	unsigned D7 : 1;
	unsigned D6 : 1;
	unsigned D5 : 1;
	unsigned D4 : 1;
	unsigned D3 : 1;
	unsigned D2 : 1;
	unsigned D1 : 1;
	unsigned D0 : 1;
	
	unsigned RW : 1; /* Read/Write bit */
	
	/*unsigned NC2 : 1;*/ /* No Connection pin */
	/*unsigned NC3 : 1;*/ /* No Connection pin */

	/*unsigned phi_0_IN: 1;*/ /* Unused */

	unsigned SO: 1;
	/*unsigned phi_2_OUT: 1;*/
	unsigned RES: 1;
} nova_m6502_pins_t;

typedef struct nova_m6502
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
} nova_m6502_t;

typedef enum nova_m6502_flags
{
	NOVA_R6502_FLAG_CARRY      = (1 << 0),
	MOVA_R6502_FLAG_ZERO       = (1 << 1),
	NOVA_R6502_FLAG_INTERRUPTS = (1 << 2),
	NOVA_R6502_FLAG_DECIMAL    = (1 << 3),
	NOVA_R6502_FLAG_BREAK      = (1 << 4),
	NOVA_R6502_FLAG_UNUSED     = (1 << 5),
	NOVA_R6502_FLAG_OVERFLOW   = (1 << 6),
	NOVA_R6502_FLAG_NEGATIVE   = (1 << 7),
} nova_m6502_flags_t;

/* Sets status flags */
void nova_m6502_set_flag(nova_m6502_t *cpu, nova_m6502_flags_t flag)
{
	cpu->status |= flag;
}

/*
	6502 Memory Layout UwU

	$0000-$07FF 	$0800 	2KB internal RAM
	$0800-$0FFF 	$0800 	Mirrors of $0000-$07FF
	$1000-$17FF 	$0800
	$1800-$1FFF 	$0800
	$2000-$2007 	$0008 	NES PPU registers
	$2008-$3FFF 	$1FF8 	Mirrors of $2000-2007 (repeats every 8 bytes)
	$4000-$4017 	$0018 	NES APU and I/O registers
	$4018-$401F 	$0008 	APU and I/O functionality that is normally disabled. See CPU Test Mode.
	$4020-$FFFF 	$BFE0 	Cartridge space: PRG ROM, PRG RAM, and mapper registers (See Note) 
*/

typedef struct nova_ram
{
	uint8_t *data;
	size_t size;
} nova_ram_t;

/* Allocates memory */
nova_ram_t nova_create_ram(size_t size)
{
	nova_ram_t ram;
	ram.size = size;
	ram.data = malloc(size);
	return ram;
}

/* Deallocates memory */ 
void nova_destroy_ram(nova_ram_t *ram)
{
	free(ram->data);
}


/* Loads program into memory */
unsigned char *load_entire_file(const char *, size_t *);

/* A0-A15 act as memory addressing, D0-D7 are actual data I/O */
/* 6502 has 13 addressing modes, instructions range from 0x00 */
/* to 0xFF													  */

const uint64_t NOVA_M6502_PIN_RW = (1UL << 33UL);
const uint64_t NOVA_M6502_PIN_OFFSET_DATA = 25UL;
const uint64_t NOVA_M6502_PIN_OFFSET_ADDR = 33UL;
const uint64_t NOVA_M6502_PIN_MASK_DATA = 0x3FC000000UL;
const uint64_t NOVA_M6502_PIN_MASK_ADDR = 0x3FFFC00000000UL;

void nova_m6502_set_data(uint64_t *pins, uint8_t data)
{
	*pins = (*pins & ~NOVA_M6502_PIN_MASK_DATA)
	| (uint64_t)data << NOVA_M6502_PIN_OFFSET_DATA;
}

uint8_t nova_m6502_get_data(uint64_t pins)
{
	return (pins & NOVA_M6502_PIN_MASK_DATA)
	>> NOVA_M6502_PIN_OFFSET_DATA;
}

uint16_t nova_m6502_get_addr(uint64_t pins)
{
	return (pins & NOVA_M6502_PIN_MASK_ADDR)
	>> NOVA_M6502_PIN_OFFSET_ADDR;
}

uint64_t nova_m6502_tick(nova_m6502_t *cpu, uint64_t pins)
{
	return pins;
}

#define NOVA_KIB (1024)
#define NOVA_NES_RAM_CAPACITY (64 * NOVA_KIB)

void nova_start_emulation(void)
{
	
	const char *bin_file_name = "../test/bin_files/6502_functional_test.bin";
	size_t bin_data_size;
	unsigned char *bin_data = load_entire_file(bin_file_name, &bin_data_size);

	if (bin_data == NULL)
	{
		/* Failed to open the file */
	}

	printf("[INFO]: Loaded binary file %s\n", bin_file_name);
	printf("[INFO]: Loaded %u bytes binary\n", bin_data_size);

	nova_ram_t ram = nova_create_ram( NOVA_NES_RAM_CAPACITY );

	if (ram.size >= bin_data_size)
	{
		memcpy(ram.data, bin_data, bin_data_size);
	}
	else
	{
		/* Binary too large */
	}

	nova_m6502_t cpu = { 0 };
	
	uint64_t pins = 0;
	
	while(1)
	{ 
		/* execute shit UwU */
		pins = nova_m6502_tick(&cpu, pins);
		const uint16_t addr = nova_m6502_get_addr(pins);
		
		/* Check RW pin to decide between read or write */
		if (pins & NOVA_M6502_PIN_RW)
		{
			/* RW pin is 'active', read memory byte into data bus pins: */
			nova_m6502_set_data(&pins, ram.data[addr]);
		}
		else
		{
			/* RW pin is 'inactive', write data bus pins into memory: */
			ram.data[addr] = nova_m6502_get_data(pins);
		}

		printf("[INFO]: Register A value: %02x\n", cpu.A);
		printf("[INFO]: Register X value: %02x\n", cpu.X);
		printf("[INFO]: Register Y value: %02x\n", cpu.Y);
		printf("[INFO]: Register P(Stack Pointer) value: 0x%02x\n", cpu.stkp);
		printf("[INFO]: Register PC(Program Counter) value: 0x%04x\n", cpu.pc);
		printf("[INFO]: Register S(Status Counter) value: 0x%04x\n", cpu.status);
	}

}

/* Loads program into memory */
unsigned char *load_entire_file(const char *file_name, size_t *file_size)
{
	unsigned char *file_data = NULL;
	FILE *file = NULL;
	file = fopen(file_name, "rb");

	if (file != NULL) 
	{
		fseek(file, 0, SEEK_END);
		*file_size = ftell(file);
		file_data = malloc(*file_size);
		fseek(file, 0, SEEK_SET);

		if (file_data != NULL)
			fread(file_data, *file_size, 1, file);
		
	}

	return file_data;
}

int main(int argc, char *argv[])
{
	nova_start_emulation();
	
#if 0
	/* Rendering stuff */
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
#endif
	return EXIT_SUCCESS;
}