#ifdef _WIN32

#else

# include <unistd.h>
void Sleep(int milliseconds)
{
	usleep(milliseconds * 1000);
}

#endif // _WIN32