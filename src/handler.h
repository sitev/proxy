#pragma once

#include "network.h"
using namespace cj;
#include "proxy.h"

#include "http_parser.h"
using namespace http;

namespace proxy {
	class Handler : public Object {
	protected:
		Server *server;
		Buffer buf;
		HttpParser parser;
		bool isConnected = false;
	public:
		bool isRunning = true;
		Socket *socket, *socketSrv;
		int saveTicks;
		Handler(Server *server, Socket *socket);
		virtual ~Handler();
		virtual bool step();
	};
}