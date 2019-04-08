#pragma once

#include "base_define.h"
#include "singleton.h"
#include "cpsock.h"
#include <thread>
#include <vector>

using namespace wind;

#define THREAD_PER_CPU 2
void IocpThread();

class CIocpCtrl : public TSingleton<CIocpCtrl>
{
public:
	bool Init();

	void Fini();

	void OnExecute();

	bool AssociateWithIocp(SOCKET hSocket, SPerHandleData * pStData);

private:
	HANDLE           m_hCompletionPort;
	uint32           m_nNumOfWorkers;
	vector<thread*>  m_vecWorkerThread;
};