#ifndef __RENDERER_H__
#define __RENDERER_H__ 1
#include <stdint.h>



#define RENDERER_PIXEL_COLOR_MASK 0xffffff
#define RENDERER_PIXEL_DEPTH_SHIFT 24



typedef uint8_t renderer_context_size_t;



typedef uint32_t renderer_pixel_t;



typedef struct _RENDERER_CONTEXT{
	const renderer_context_size_t width;
	const renderer_context_size_t height;
	renderer_pixel_t clear_color;
	char* _terminal_line_buffer;
	renderer_pixel_t pixels[];
}* renderer_context_t;



renderer_context_t renderer_context_create(renderer_context_size_t width,renderer_context_size_t height);



void renderer_context_release(renderer_context_t ctx);



void renderer_clear(renderer_context_t ctx);



void renderer_flip_to_terminal(renderer_context_t ctx);



#endif
