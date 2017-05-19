#pragma once

#include "network.h"
#include "proxy.h"

#define MAX_SOCK 10000

namespace proxy {

	class Server {
	protected:
		int port;
		cj::ServerSocket *ss;
		Handler* arrHandler[MAX_SOCK];
		List lstHandler;
		bool isRunning = false;
		SOCKET maxsock = 0;
	public:
		Server(int port);
		virtual bool init();
		virtual bool run();
		virtual bool step();

	};

}