#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#else
#include <sys/select.h>
#endif
#include <math.h>
#include <renderer.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>



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



static uint64_t get_time(void){
#ifdef _MSC_VER
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return ((((sll_time_t)ft.dwHighDateTime)<<32)|ft.dwLowDateTime)*100-11644473600000000000;
#else
	struct timespec tm;
	clock_gettime(CLOCK_REALTIME,&tm);
	return tm.tv_sec*1000000000+tm.tv_nsec;
#endif
}



int main(int argc,const char** argv){
#ifdef _MSC_VER
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleMode(GetStdHandle(-11),7);
#endif
	renderer_context_t ctx=renderer_context_create(32,16);
	ctx->clear_color=0xffffff;
	uint64_t start_time=get_time();
	while (1){
		float tm=(get_time()-start_time)*1e-9;
		renderer_clear(ctx);
		renderer_rasterize_triangle(ctx,0,0,127,10,10,127,10,0,127,0xff00ff);
		renderer_rasterize_triangle(ctx,0,0,127,0,10,127,10,10,127,0xff00ff);
		float rect_z=sin(tm)*127+127;
		float rect_z2=cos(tm)*127+127;
		renderer_rasterize_triangle(ctx,0,0,rect_z,0,5,rect_z,10,5,rect_z2,0xffff00);
		renderer_rasterize_triangle(ctx,0,0,rect_z,10,5,rect_z2,10,0,rect_z2,0xffff00);
		fputs("\x1b[H",stdout);
		renderer_flip_to_terminal(ctx,0);
		renderer_flip_to_terminal(ctx,1);
		sleep(16000000);
	}
	renderer_context_release(ctx);
	return 0;
}
