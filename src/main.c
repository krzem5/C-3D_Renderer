#include <renderer.h>
#include <stdint.h>
#include <stdio.h>



int main(int argc,const char** argv){
	renderer_context_t ctx=renderer_context_create(32,16);
	renderer_clear(ctx);
	renderer_context_release(ctx);
	return 0;
}
