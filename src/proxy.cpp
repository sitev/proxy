// proxy.cpp : Defines the entry point for the console application.
//

#include <iostream>
using namespace std;

#include "proxy.h"

#ifdef OS_WINDOWS
#pragma comment(lib, "core.lib")
#pragma comment(lib, "network.lib")
#pragma comment(lib, "http.lib")
#endif

int main()
{
	cout << "start proxy" << endl;

	proxy::Server *app = new proxy::Server(8080);
	app->init();
	app->run();
	return 0;
}

