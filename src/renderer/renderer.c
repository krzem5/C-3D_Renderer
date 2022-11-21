#include <math.h>
#include <renderer.h>
#include <stdint.h>
#include <stdio.h>
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



void renderer_flip_to_terminal(renderer_context_t ctx,_Bool use_depth_buffer){
	if (!ctx->_terminal_line_buffer){
		ctx->_terminal_line_buffer=malloc(21*ctx->width+5);
	}
	uint16_t y_offset=0;
	for (renderer_context_size_t y=0;y<ctx->height;y++){
		char* buffer=ctx->_terminal_line_buffer;
		renderer_pixel_t current_color=0xffffffff;
		for (renderer_context_size_t x=0;x<ctx->width;x++){
			renderer_pixel_t color=(use_depth_buffer?0x010101*(255-(ctx->pixels[x+y_offset]>>RENDERER_PIXEL_DEPTH_SHIFT)):(ctx->pixels[x+y_offset]&RENDERER_PIXEL_COLOR_MASK));
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



void renderer_rasterize_triangle(renderer_context_t ctx,float ax,float ay,float az,float bx,float by,float bz,float cx,float cy,float cz,renderer_rasterizer_callback_t callback){
	float min_x=ax;
	float max_x=ax;
	if (bx<min_x){
		min_x=bx;
	}
	else if (bx>max_x){
		max_x=bx;
	}
	if (cx<min_x){
		min_x=cx;
	}
	else if (cx>max_x){
		max_x=cx;
	}
	float min_y=ay;
	float max_y=ay;
	if (by<min_y){
		min_y=by;
	}
	else if (by>max_y){
		max_y=by;
	}
	if (cy<min_y){
		min_y=cy;
	}
	else if (cy>max_y){
		max_y=cy;
	}
	renderer_context_size_t pixel_min_x=min_x;
	renderer_context_size_t pixel_max_x=ceilf(max_x);
	renderer_context_size_t pixel_min_y=min_y;
	renderer_context_size_t pixel_max_y=ceilf(max_y);
	if (pixel_min_x<0){
		pixel_min_x=0;
	}
	if (pixel_max_x>=ctx->width){
		pixel_max_x=ctx->width-1;
	}
	if (pixel_min_y<0){
		pixel_min_y=0;
	}
	if (pixel_max_y>=ctx->height){
		pixel_max_y=ctx->height-1;
	}
	float area_inv=1/((cx-ax)*(by-ay)-(cy-ay)*(bx-ax));
	float t0_x_mult=(by-ay)*area_inv;
	float t0_y_mult=(bx-ax)*area_inv;
	float t0_bias=ay*t0_y_mult-ax*t0_x_mult;
	float t1_x_mult=(cy-by)*area_inv;
	float t1_y_mult=(cx-bx)*area_inv;
	float t1_bias=by*t1_y_mult-bx*t1_x_mult;
	float t2_x_mult=(ay-cy)*area_inv;
	float t2_y_mult=(ax-cx)*area_inv;
	float t2_bias=cy*t2_y_mult-cx*t2_x_mult;
	uint16_t y_offset=pixel_min_y*ctx->width;
	for (renderer_context_size_t y=pixel_min_y;y<=pixel_max_y;y++){
		for (renderer_context_size_t x=pixel_min_x;x<=pixel_max_x;x++){
			float t0=x*t0_x_mult-y*t0_y_mult+t0_bias;
			if (t0<0){
				continue;
			}
			float t1=x*t1_x_mult-y*t1_y_mult+t1_bias;
			if (t1<0){
				continue;
			}
			float t2=x*t2_x_mult-y*t2_y_mult+t2_bias;
			if (t2<0){
				continue;
			}
			float z=t0*cz+t1*az+t2*bz;
			if (z<0||z>255){
				continue;
			}
			uint8_t pixel_z=z;
			if (pixel_z>=(ctx->pixels[x+y_offset]>>RENDERER_PIXEL_DEPTH_SHIFT)){
				continue;
			}
			ctx->pixels[x+y_offset]=(pixel_z<<RENDERER_PIXEL_DEPTH_SHIFT)|(callback(t1,t2,t0)&RENDERER_PIXEL_COLOR_MASK);
		}
		y_offset+=ctx->width;
	}
}
