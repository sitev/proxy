#pragma once

#include "network.h"
using namespace network;
#include "proxy.h"

#include "http_parser.h"
using namespace http;

namespace proxy {
	class Handler : public Object {
	protected:
		Server *server;
		Buffer buf, buf2;
		HttpParser parser;
		bool isConnected = false;
		bool isBack = false;
	public:
		bool isRunning = true;
		bool isClose = false;
		int close_timer;
		Socket *socket = NULL, *socketTo = NULL;
		int saveTicks;
		Handler(Server *server, Socket *socket = NULL, Socket *socketTo = NULL);
		virtual ~Handler();
		virtual bool read();
		virtual bool close();
		virtual void runClosing();
		virtual void deferClosing();
		virtual bool isCanClose();

		virtual bool step();
		virtual void setConnected(bool value);
		virtual int connectedToSomeServer();

		virtual bool getIsBack();
		virtual void setIsBack(bool value);
	};
}