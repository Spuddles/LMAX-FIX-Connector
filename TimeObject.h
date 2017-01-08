// Pull in the correct includes depending on
// which platform we are building on
#ifdef _WIN32
#else
#include <sys/timeb.h>
#include <time.h>
#endif // _WIN32

class CTimeObject
{
private:
#ifdef _WIN32
	SYSTEMTIME	m_st;
#else
	timeb		m_tb;
	tm			m_tm;
#endif // _WIN32

public:
	unsigned int			GetMillisecond();
	unsigned int			GetSecond();
	unsigned int			GetMinute();
	unsigned int			GetHour();
	unsigned int			GetDay();
	unsigned int			GetMonth();
	unsigned int			GetYear();
	static	CTimeObject		Now();
	void 					SetCurrentTime();
	void					Print();
	void					PrintDate();
	void					PrintTime();
};
