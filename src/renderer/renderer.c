#include <renderer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



renderer_context_t renderer_context_create(renderer_context_size_t width,renderer_context_size_t height){
	struct _RENDERER_CONTEXT ctx_data={
		width,
		height,
		0x000000
	};
	renderer_context_t out=malloc(sizeof(struct _RENDERER_CONTEXT)+width*height*sizeof(renderer_pixel_t));
	memcpy(out,&ctx_data,sizeof(struct _RENDERER_CONTEXT));
	return out;
}



void renderer_context_release(renderer_context_t ctx){
	free(ctx);
}



void renderer_clear(renderer_context_t ctx){
	ctx->clear_color&=RENDERER_PIXEL_COLOR_MASK;
	for (uint16_t i=0;i<ctx->width*ctx->height;i++){
		ctx->pixels[i]=ctx->clear_color;
	}
}
