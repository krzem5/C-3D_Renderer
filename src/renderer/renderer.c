#include <renderer.h>
#include <stdint.h>
#include <stdlib.h>



void renderer_context_create(renderer_context_size_t width,renderer_context_size_t height,renderer_context_t* out){
	out->width=width;
	out->height=height;
	out->pixels=malloc(width*height*sizeof(renderer_pixel_t));
	out->clear_color=0x000000;
}



void renderer_context_release(renderer_context_t* ctx){
	ctx->width=0;
	ctx->height=0;
	free(ctx->pixels);
	ctx->pixels=NULL;
	ctx->clear_color=0;
}



void renderer_context_clear(renderer_context_t* ctx){
	ctx->clear_color&=RENDERER_PIXEL_COLOR_MASK;
	for (uint16_t i=0;i<ctx->width*ctx->height;i++){
		ctx->pixels[i]=ctx->clear_color;
	}
}
