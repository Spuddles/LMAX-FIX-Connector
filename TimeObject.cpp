#include "stdafx.h"
#include "TimeObject.h"
#include <iostream>
#include <iomanip>

using namespace std;

unsigned int CTimeObject::GetMillisecond()
{
#ifdef _WIN32
	return m_st.wMilliseconds;
#else
	return m_tb.millitm;
#endif // _WIN32
}

unsigned int CTimeObject::GetSecond()
{
#ifdef _WIN32
	return m_st.wSecond;
#else
	return m_tm.tm_sec;
#endif // _WIN32
}

unsigned int CTimeObject::GetMinute()
{
#ifdef _WIN32
	return m_st.wMinute;
#else
	return m_tm.tm_min;
#endif // _WIN32
}

unsigned int CTimeObject::GetHour()
{
#ifdef _WIN32
	return m_st.wHour;
#else
	return m_tm.tm_hour;
#endif // _WIN32
}

unsigned int CTimeObject::GetDay()
{
#ifdef _WIN32
	return m_st.wDay;
#else
	return m_tm.tm_mday;
#endif // _WIN32
}

unsigned int CTimeObject::GetMonth()
{
#ifdef _WIN32
	return m_st.wMonth;
#else
	return m_tm.tm_mon+1;
#endif // _WIN32
}

unsigned int CTimeObject::GetYear()
{
#ifdef _WIN32
	return m_st.wYear;
#else
	return m_tm.tm_year+1900;
#endif // _WIN32
}

void CTimeObject::SetCurrentTime()
{
#ifdef _WIN32
	GetSystemTime(&m_st);
#else
	// Get the full time info
	ftime(&m_tb);
	// Split the time integer up into parts
	localtime_r(&(m_tb.time), &m_tm);
#endif // _WIN32
}

CTimeObject CTimeObject::Now()
{
	// Create a new object
	CTimeObject obj;
	
	// Set it to the current time
	obj.SetCurrentTime();
	
	// Return the new object
	return obj;
}

/*
 * Print the date and time information in the following format
 * YYYY-MM-DD:HH:MM:SS.MMM
 */
void CTimeObject::Print()
{
	cout << setfill('0');
	cout << setw(4) << GetYear() << "-";
	cout << setw(2) << GetMonth() << "-";
	cout << setw(2) << GetDay() << ":";
	cout << setw(2) << GetHour() << ":";
	cout << setw(2) << GetMinute() << ":";
	cout << setw(2) << GetSecond() << ".";
	cout << setw(3) << GetMillisecond() << endl;
}

/*
 * Print the time information in the following format
 * YYYY-MM-DD
 */
void CTimeObject::PrintTime()
{
	cout << setfill('0');
	cout << setw(4) << GetYear() << "-";
	cout << setw(2) << GetMonth() << "-";
	cout << setw(2) << GetDay() << endl;
}

/*
 * Print the date information in the following format
 * HH:MM:SS.MMM
 */
void CTimeObject::PrintDate()
{
	cout << setfill('0');
	cout << setw(2) << GetHour() << ":";
	cout << setw(2) << GetMinute() << ":";
	cout << setw(2) << GetSecond() << ".";
	cout << setw(3) << GetMillisecond() << endl;
}

