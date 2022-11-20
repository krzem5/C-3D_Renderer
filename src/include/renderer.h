#ifndef __RENDERER_H__
#define __RENDERER_H__ 1
#include <stdint.h>



#define RENDERER_PIXEL_COLOR_MASK 0xffffff
#define RENDERER_PIXEL_DEPTH_SHIFT 24



typedef uint8_t renderer_context_size_t;



typedef uint32_t renderer_pixel_t;



typedef struct _RENDERER_CONTEXT{
	renderer_context_size_t width;
	renderer_context_size_t height;
	renderer_pixel_t* pixels;
	renderer_pixel_t clear_color;
} renderer_context_t;



void renderer_context_create(renderer_context_size_t width,renderer_context_size_t height,renderer_context_t* out);



void renderer_context_release(renderer_context_t* ctx);



void renderer_context_clear(renderer_context_t* ctx);



#endif
