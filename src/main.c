#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif
#include <renderer.h>
#include <stdint.h>
#include <stdio.h>



int main(int argc,const char** argv){
#ifdef _MSC_VER
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleMode(GetStdHandle(-11),7);
#endif
	renderer_context_t ctx=renderer_context_create(32,16);
	ctx->clear_color=0xffffff;
	renderer_clear(ctx);
	renderer_flip_to_terminal(ctx);
	renderer_context_release(ctx);
	return 0;
}
