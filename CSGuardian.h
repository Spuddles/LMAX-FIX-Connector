#pragma once

#ifdef _WIN32
#include <afxmt.h>
typedef CCriticalSection CRIT_SECTION; 
#else
#include <pthread.h>
typedef pthread_mutex_t CRIT_SECTION; 
#endif // _WIN32

class CCSGuardian
{
	CRIT_SECTION &m_CS;
public:
	CCSGuardian(CRIT_SECTION &CS);
	~CCSGuardian(void);
};

