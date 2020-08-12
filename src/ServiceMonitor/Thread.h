#ifndef THREAD_H
#define THREAD_H

#include "StdExport.h"
#include "StdTypes.h"


class CThreadImpl;
class CThread 
{
public:

#ifdef	_LINUX_64
	typedef uint64_t ID; 
#else
	typedef uint32_t ID; 
#endif

	CThread();
	virtual ~CThread();

	typedef void (*ThreadFunc) (void*);

	int Start(ThreadFunc, void*);

	int Join(int* pExitStatus = 0, int timeoutInMs = -1);

	ID GetThreadId() const;

	static ID GetCurrentThreadId();

private:
	friend class CThreadImpl;
	CThreadImpl* mImpl;

	struct ThreadStore {
		ThreadFunc func;
		void* arg;
		CThreadImpl* pImpl;
	};
	ThreadStore mThreadStore;

	CThread(const CThread&);
	CThread& operator= (const CThread&);
};


#endif //THREAD_H

