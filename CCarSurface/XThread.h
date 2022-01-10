//$ XThread.h
//$ Comments

#ifndef _XTHREAD_H_
#define _XTHREAD_H_

#include <windows.h>

class XThread
{
private:
	XThread( XThread &);
	//XThread operator=( XThread & );

public:
	XThread( int nPriority = THREAD_PRIORITY_NORMAL );	// 线程优先级
	virtual ~XThread( void );

private:
	HANDLE	m_hHandle;		// 线程句柄
	UINT	m_nId;			// 线程标识
	int		m_nPriority;	// 线程优先级
	volatile bool m_bExit;	// 线程是否已被设置终止
	bool	m_bStarted;		// 线程已经启动
	bool	m_bStopped;		// Stop是否已经调用过
public:
	void *m_pUserData;		// 保留给用户以扩展

public:
	HANDLE	ThreadHandle( void ){ return m_hHandle; }	// 返回线程句柄
	UINT	ThreadId( void ){ return m_nId; }			// 返回线程标识
	void ResetThread();
	
public:
	void SetExit( void ){ m_bExit = true; }			// 设定线程终止信号
	bool IsExitSet( void ){ return m_bExit; }		// 检测线程终止信号

	bool IsTerminated( void );						// 线程是否已经终止

public:
	bool Start( void );				// 启动线程
	bool Stop( DWORD dwTimeout = INFINITE );
		// dwTimeout: WaitForSingleObject的超时设置, 微秒级
		// 终止线程, 在派生类的析构函数中务必先调用本函数, 再做派生类的清理工作
	bool SetPriority( int nPriority = THREAD_PRIORITY_NORMAL );	// 设定线程优先级
private:
	static UINT WINAPI XThreadProc( LPVOID pThis );
public:
	virtual UINT Worker( void ) = NULL;
	// 用户必须在派生类中实现它
	virtual bool TermViolent( void ) = NULL;
	// 用户必须在派生类中实现它
	// 用户可以用以实现界面提示，是否强制退出线程
};

#endif
