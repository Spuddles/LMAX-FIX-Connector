// FIXConnector.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "FIXAdapter.h"
#include "Platform.h"
#include <iostream>
#include "TimeObject.h"

using namespace std;

int main(int argc, char* argv[])
{
	// Testing code
	CTimeObject	TimeObj;

	for (int i=0;i<100;i++)
	{
		TimeObj.SetCurrentTime();
		TimeObj.Print();
		Sleep(250);
	}

	cout << "Waiting for 5 seconds...";
	Sleep(1000*5);
	cout << "Done" << endl;

	return 0;


	CFIXAdapter	app;
	app.SetLoginDetails("username", "password");
	app.SetCompIDs("CompID", "CompID");
	if (!app.Initialise())
	{
		cout << "Failed to initialise" << endl;
		return 1;
	}

	// Wait for it to complete
	cout << "Waiting for app to login" << endl;
	while (!app.IsTradeLoggedIn() &&
		   !app.IsPriceLoggedIn())
	{
		Sleep(1000);
	}
	Sleep(1000);
	
	// Request live price stream
	cout << "About to send request for live prices" << endl; 
	app.StartMarketDataRequest("GBPUSD");
	cout << "Waiting for prices" << endl;

	Sleep(1000*60*100);
	if (!app.Shutdown())
	{
		cout << "Failed to shutdown" << endl;
		return 1;
	}
	return 0;
}

