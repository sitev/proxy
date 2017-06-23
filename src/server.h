#pragma once

//#include "network.h"
//#include "proxy.h"

#define MAX_SOCK 100000

namespace proxy {

	class Server {
	protected:
		int port;
		ServerSocket *ss;
		bool isRunning = false;
		SOCKET maxsock = 0;
	public:
		Handler* arrHandler[MAX_SOCK];
		List lstHandler;
		SocketHandler *sohandler;
		Server(int port);
		virtual bool init();
		virtual bool run();

		virtual Handler* newHandler(Server *server, Socket *socket = NULL, Socket *socketTo = NULL);
		virtual bool step();
	};

}