#pragma once

#include "network.h"
using namespace cj;
#include "proxy.h"

namespace proxy {
	class Handler : public Object {
	protected:
		Server *server;
	public:
		bool isRunning = true;
		Socket *socket, *socketSrv;
		int saveTicks;
		Handler(Server *server, Socket *socket);
		virtual ~Handler();
		virtual void step();
	};
}