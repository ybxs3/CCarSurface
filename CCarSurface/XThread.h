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
	XThread( int nPriority = THREAD_PRIORITY_NORMAL );	// �߳����ȼ�
	virtual ~XThread( void );

private:
	HANDLE	m_hHandle;		// �߳̾��
	UINT	m_nId;			// �̱߳�ʶ
	int		m_nPriority;	// �߳����ȼ�
	volatile bool m_bExit;	// �߳��Ƿ��ѱ�������ֹ
	bool	m_bStarted;		// �߳��Ѿ�����
	bool	m_bStopped;		// Stop�Ƿ��Ѿ����ù�
public:
	void *m_pUserData;		// �������û�����չ

public:
	HANDLE	ThreadHandle( void ){ return m_hHandle; }	// �����߳̾��
	UINT	ThreadId( void ){ return m_nId; }			// �����̱߳�ʶ
	void ResetThread();
	
public:
	void SetExit( void ){ m_bExit = true; }			// �趨�߳���ֹ�ź�
	bool IsExitSet( void ){ return m_bExit; }		// ����߳���ֹ�ź�

	bool IsTerminated( void );						// �߳��Ƿ��Ѿ���ֹ

public:
	bool Start( void );				// �����߳�
	bool Stop( DWORD dwTimeout = INFINITE );
		// dwTimeout: WaitForSingleObject�ĳ�ʱ����, ΢�뼶
		// ��ֹ�߳�, �����������������������ȵ��ñ�����, �����������������
	bool SetPriority( int nPriority = THREAD_PRIORITY_NORMAL );	// �趨�߳����ȼ�
private:
	static UINT WINAPI XThreadProc( LPVOID pThis );
public:
	virtual UINT Worker( void ) = NULL;
	// �û���������������ʵ����
	virtual bool TermViolent( void ) = NULL;
	// �û���������������ʵ����
	// �û���������ʵ�ֽ�����ʾ���Ƿ�ǿ���˳��߳�
};

#endif
