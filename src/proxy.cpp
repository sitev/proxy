// proxy.cpp : Defines the entry point for the console application.
//

#include <iostream>
using namespace std;

#include "proxy.h"

int main()
{
	cout << "start proxy" << endl;

	proxy::Server *app = new proxy::Server(8080);
	app->init();
	app->run();
	return 0;
}

