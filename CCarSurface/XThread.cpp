//$ XThread.cpp
//$ Comments
#include "stdafx.h"
#include "XThread.h"
#include <process.h>

//XThread XThread::operator=(XThread &)
//{
//	return XThread();
//}

XThread::XThread( int nPriority /* = THREAD_PRIORITY_NORMAL */ )
						// �߳����ȼ�
{
	m_hHandle = NULL;
	m_nId = 0;

	m_nPriority = nPriority;

	m_bExit		= true;		// ��δ����, ��Ȼ�Ѿ������˳�
	m_bStarted	= false;	// ��δ����, ��Ȼ���
	m_bStopped	= true;		// ��δ����, ��Ȼ�Ѿ�ֹͣ
}

XThread::~XThread( void )
{
	Stop();
	if( m_hHandle ) CloseHandle( m_hHandle );
}

bool XThread::SetPriority( int nPriority /* = THREAD_PRIORITY_NORMAL */ )
{
	if( !m_hHandle ) return false;
	return FALSE != SetThreadPriority( m_hHandle, nPriority );
}

UINT WINAPI XThread::XThreadProc( LPVOID pThis )
{
	XThread *pT = (XThread*)pThis;
	UINT uiRet = pT->Worker();
	return uiRet;
}

bool XThread::Start( void )
{
	if( m_hHandle ) return false;
	m_hHandle = (HANDLE)::_beginthreadex( NULL, 0, XThread::XThreadProc, this, 
										  CREATE_SUSPENDED, &m_nId );
	if( m_hHandle == NULL ) return false;
	SetThreadPriority( m_hHandle, m_nPriority );
	m_bExit = false; m_bStarted = true; m_bStopped = false;
	ResumeThread( m_hHandle );
	return true;
}

bool XThread::Stop( DWORD dwTimeout /* = INFINITE */ )
{
	if( !m_hHandle || m_bStopped ) return true;
	SetExit();
	DWORD dwRet;
	do{
		dwRet = WaitForSingleObject( m_hHandle, dwTimeout );
		if( dwRet == WAIT_OBJECT_0 ) break;
		if( TermViolent() ){
			TerminateThread( m_hHandle, -1 );
			break;
		}
	}while(1);
	m_bStopped = true;	// �Ѿ�����ֹͣ��
	return true;
}

bool XThread::IsTerminated( void )
{
	if( !m_hHandle ) return true;
	return  WAIT_OBJECT_0 == WaitForSingleObject( m_hHandle, 0 );
}

void XThread::ResetThread()
{
	m_bExit = true; 
	m_bStarted = false;
	m_bStopped = true;
	m_hHandle = 0;
	m_nId = 0 ;
}
