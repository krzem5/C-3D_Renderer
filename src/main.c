#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#else
#include <time.h>
#include <sys/select.h>
#endif
#include <renderer.h>
#include <stdint.h>
#include <stdio.h>



static void sleep(uint64_t time_ns){
#ifdef _MSC_VER
	static HANDLE event=CreateEventA(NULL,TRUE,FALSE,FALSE);
	uint64_t end=GetTickCount64()*1000000+time_ns;
	while (1){
		ResetEvent(event);
		if (WaitForSingleObjectEx(event,(DWORD)((time_ns+999999)/1000000),FALSE)!=WAIT_OBJECT_0){
			return;
		}
		uint64_t current=GetTickCount64()*1000000;
		if (current>=end){
			break;
		}
		time_ns=current-end;
	}
#else
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME,&ts);
	uint64_t end=ts.tv_sec*1000000000+ts.tv_nsec+time_ns;
	while (1){
		struct timeval tv;
		tv.tv_sec=time_ns/1000000000;
		tv.tv_usec=(time_ns/1000)%1000000;
		if (!select(0,NULL,NULL,NULL,&tv)){
			return;
		}
		clock_gettime(CLOCK_REALTIME,&ts);
		uint64_t current=ts.tv_sec*1000000000+ts.tv_nsec;
		if (current>=end){
			break;
		}
		time_ns=current-end;
	}
#endif
}



int main(int argc,const char** argv){
#ifdef _MSC_VER
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleMode(GetStdHandle(-11),7);
#endif
	renderer_context_t ctx=renderer_context_create(32,16);
	ctx->clear_color=0xffffff;
	while (1){
		renderer_clear(ctx);
		renderer_rasterize_triangle(ctx,0,0,5,10,10,5,10,0,5,0xff00ff);
		renderer_rasterize_triangle(ctx,0,0,5,0,10,5,10,10,5,0xff00ff);
		renderer_rasterize_triangle(ctx,0,0,9,0,5,9,10,5,1,0xffff00);
		renderer_rasterize_triangle(ctx,0,0,9,10,5,1,10,0,1,0xffff00);
		fputs("\x1b[H",stdout);
		renderer_flip_to_terminal(ctx);
		sleep(16000000);
	}
	renderer_context_release(ctx);
	return 0;
}
