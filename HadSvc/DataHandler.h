#pragma once
#include <fstream>

class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	bool PipInit();
	void PipFree();
	bool PipInitAnonymous();
	void PipFreeAnonymous();
	void DebugTaskInterface(const int taskid);

	bool ThreadPool_Init();
	bool ThreadPool_Free();

	// Recv PipCommand
	void OnPipMessageNotify(const std::shared_ptr<uint8_t>& data, size_t size);

	// Sub Data Handle
	void KerSublthreadProc();
	void EtwSublthreadProc();
	static DWORD WINAPI PTaskHandlerNotify(LPVOID lpThreadParameter);

	// Set Lib Ptr
	bool SetUMontiorLibPtr(void* ulibptr);
	bool SetKMontiorLibPtr(void* klibptr);

	// Set ExitEvent
	void SetExitSvcEvent(HANDLE& hexitEvent);

private:
	typedef std::vector<HANDLE> tThreads;
	tThreads m_ker_subthreads;
	tThreads m_etw_subthreads;
	tThreads m_threads_write;

	HANDLE m_jobAvailableEvnet_WriteTask = NULL;
};

