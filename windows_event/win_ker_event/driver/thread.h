#ifndef _THREAD_H
#define _THREAD_H

typedef struct _THREADINFO
{
	int threadid;
}THREADINFO, *PTHREADINFO;

typedef struct _THREADBUFFER
{
	LIST_ENTRY			pEntry;
	ULONG				dataLength;
	char*				dataBuffer;
}THREADBUFFER, * P_THREADBUFFER;

typedef struct _THREADDATA
{
	KSPIN_LOCK thread_lock;
	LIST_ENTRY thread_pending;
}THREADDATA, * PTHREADDATA;

void thread_init();
void trhead_clean();
void thread_free();


#endif // !_THREAD_H