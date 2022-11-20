#include <renderer.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



renderer_context_t renderer_context_create(renderer_context_size_t width,renderer_context_size_t height){
	struct _RENDERER_CONTEXT ctx_data={
		width,
		height,
		0x000000,
		NULL
	};
	renderer_context_t out=malloc(sizeof(struct _RENDERER_CONTEXT)+width*height*sizeof(renderer_pixel_t));
	memcpy(out,&ctx_data,sizeof(struct _RENDERER_CONTEXT));
	return out;
}



void renderer_context_release(renderer_context_t ctx){
	if (ctx->_terminal_line_buffer){
		free(ctx->_terminal_line_buffer);
	}
	free(ctx);
}



void renderer_clear(renderer_context_t ctx){
	ctx->clear_color&=RENDERER_PIXEL_COLOR_MASK;
	uint32_t value=(0xff<<RENDERER_PIXEL_DEPTH_SHIFT)|ctx->clear_color;
	for (uint16_t i=0;i<ctx->width*ctx->height;i++){
		ctx->pixels[i]=value;
	}
}



void renderer_flip_to_terminal(renderer_context_t ctx){
	if (!ctx->_terminal_line_buffer){
		ctx->_terminal_line_buffer=malloc(21*ctx->width+5);
	}
	uint16_t y_offset=0;
	for (renderer_context_size_t y=0;y<ctx->height;y++){
		char* buffer=ctx->_terminal_line_buffer;
		renderer_pixel_t current_color=0xffffffff;
		for (renderer_context_size_t x=0;x<ctx->width;x++){
			renderer_pixel_t color=ctx->pixels[x+y_offset]&RENDERER_PIXEL_COLOR_MASK;
			if (color!=current_color){
				current_color=color;
				uint8_t red=color>>16;
				uint8_t green=color>>8;
				uint8_t blue=color;
				buffer[0]='\x1b';
				buffer[1]='[';
				buffer[2]='4';
				buffer[3]='8';
				buffer[4]=';';
				buffer[5]='2';
				buffer[6]=';';
				buffer[7]=red/100+48;
				buffer[8]=((red/10)%10)+48;
				buffer[9]=(red%10)+48;
				buffer[10]=';';
				buffer[11]=green/100+48;
				buffer[12]=((green/10)%10)+48;
				buffer[13]=(green%10)+48;
				buffer[14]=';';
				buffer[15]=blue/100+48;
				buffer[16]=((blue/10)%10)+48;
				buffer[17]=(blue%10)+48;
				buffer[18]='m';
				buffer+=19;
			}
			buffer[0]=' ';
			buffer[1]=' ';
			buffer+=2;
		}
		buffer[0]='\x1b';
		buffer[1]='[';
		buffer[2]='0';
		buffer[3]='m';
		buffer[4]='\n';
		buffer+=5;
		fwrite(ctx->_terminal_line_buffer,1,buffer-ctx->_terminal_line_buffer,stdout);
		y_offset+=ctx->width;
	}
}
