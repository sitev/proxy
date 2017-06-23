#include "proxy.h"

namespace proxy {

	Server::Server(int port) {
		this->port = port;
		ss = new ServerSocket();
		sohandler = new WSASocketHandler();

		memset(arrHandler, 0, MAX_SOCK);
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

	Handler* Server::newHandler(Server *server, Socket *socket, Socket *socketTo) {
		return new Handler(this, socket, socketTo);
	}

	bool Server::step() {
		if (!isRunning) 
			return false;

		Socket *socket = ss->acceptLight();
		if (socket) {
			cout << "accept " << socket->m_sock << endl;
			if (socket->m_sock > maxsock) maxsock = socket->m_sock;

			Handler *handler = newHandler(this, socket);
			try {
				lstHandler.add(handler);
			}
			catch (...) {
				lstHandler.add(handler);
			}
			arrHandler[socket->m_sock] = handler;
			handler->saveTicks = GetTickCount();

			sohandler->add(socket->m_sock);
		}

		ulong events;
		SOCKET sock;
		if (sohandler->wait(events, sock)) {
			if ((events & FD_CONNECT) != 0) {
				cout << "connected sock = " << sock << endl;
				Handler *handler = arrHandler[sock];
				handler->connectedToSomeServer();
			}
			if ((events & FD_WRITE) != 0) {
				///cout << "write sock = " << sock << endl;
			}
			if ((events & FD_CLOSE) != 0) {
				cout << "close sock = " << sock << endl;
				if ((events & FD_READ) != 0) {
					Handler *handler = arrHandler[sock];
					handler->runClosing();
				}
				else {
					Handler *handler = arrHandler[sock];
					handler->close();
					//delete handler;
				}
			}
			if ((events & FD_READ) != 0) {
				char buf[MAXRECV];
				int len = ::recv(sock, buf, MAXRECV, MSG_PEEK);
				cout << "read sock = " << sock << " len = " << len << endl;
				Handler *handler = arrHandler[sock];
				if (handler) {
					if (!handler->read()) {
						cout << "!read" << endl;
						//handler->close();
					}
				}
			}
		}
		
		int handlerCount = lstHandler.getCount();
		for (int i = handlerCount - 1; i >= 0; i--) {
			Handler *handler = (Handler*)lstHandler.getItem(i);
			if (handler == NULL) continue;

			try {
				handler->read();
			}
			catch (...) {
				handlerCount = lstHandler.getCount();
				handler->read();
			}
			if (!handler->isRunning) {

				handler->close();

				handlerCount = lstHandler.getCount();
				if (i > handlerCount) {
					i = handlerCount;
				}
				//delete handler;
			}
		}
		
		usleep(1000);

	}

}