#include "server.h"
#include "cj.h"
using namespace cj;
#include "network.h"

namespace proxy {

	Server::Server(int port) {
		this->port = port;
		ss = new ServerSocket();

		for (int i = 0; i < MAX_SOCK; i++) {
			arrHandler[i] = NULL;
		}
	}

	bool Server::init() {
		isRunning = false;
		bool flag = ss->create(AF_INET, SOCK_STREAM, 0);
		if (!flag) return false;
		if (!ss->bind(port)) return false;

		ss->setNonBlocking(true);
		ss->listen();
		isRunning = true;
		return true;
	}

	bool Server::run() {
		while (isRunning) {
			step();
		}
		return true;
	}

	bool Server::step() {
		if (!isRunning) return false;
		Socket *socket = ss->acceptLight();
		if (socket) {
			cout << "accept " << socket->m_sock << endl;
			if (socket->m_sock > maxsock) maxsock = socket->m_sock;

			Handler *handler = new Handler(this, socket);
			lstHandler.add(handler);
			arrHandler[socket->m_sock] = handler;
			handler->saveTicks = GetTickCount();
		}

		int handlerCount = lstHandler.getCount();
		for (int i = handlerCount - 1; i >= 0; i--) {
			Handler *handler = (Handler*)lstHandler.getItem(i);
			handler->step();
			if (!handler->isRunning) {
				socket = handler->socket;
				lstHandler.del(handler);
				arrHandler[socket->m_sock] = NULL;
				delete handler;
			}
		}

		usleep(1000);
	}

}