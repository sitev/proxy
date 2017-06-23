#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "proxy.h"

namespace proxy {

	Handler::Handler(Server *server, Socket *socket, Socket *socketTo) {
		this->server = server;
		this->socket = socket;
		this->socketTo = socketTo;
	}

	Handler::~Handler() {
		delete socket;
		isRunning = false;
	}

	bool Handler::read() {
		if (!isRunning) {
			cout << "!isRunning" << endl;
			return false;
		}
		int len = socket->recv(buf);
		//buf.data[len] = 0;
		//cout << "recv len = " << len << endl;
		if (len > 0) {
			if (!isConnected && !isBack) {
				bool flag = parser.parse(buf);
				if (flag) {
					Str method = parser.getMethod();
					Str host = parser.getHost();
					int port = parser.getPort();
					cout << "method = " << method.to_string() << " host = " << host.to_string() <<
						" port = " << port << endl;

					socketTo = new Socket();
					if (!socketTo->create()) {
						cout << "if (!socketTo->create()) {" << endl;
						return false;
					}
					Str addr = getIpByHost(host, to_string(port));
					if (addr == "") {
						cout << "addr == \"\"" << endl;
						return false;
					}
					cout << ">>> connect";
					flag = socketTo->connect(addr, port);
					cout << "ed " << flag << endl;
					if (!flag) {
						cout << "!flag" << endl;
						return false;
					}
					socketTo->setNonBlocking(true);

					Handler *handler = new Handler(server, socketTo, socket);
					handler->setIsBack(true);
					server->lstHandler.add(handler);
					server->arrHandler[socketTo->m_sock] = handler;
					handler->saveTicks = GetTickCount();

					server->sohandler->add(socketTo->m_sock);



					if (method == "CONNECT") {
						string s = "HTTP/1.1 200 Connection established\r\n\r\n";
						int len = s.length();
						Memory mem;
						mem.write((void*)s.c_str(), len);
						int len1 = socket->send(mem);
						buf.clear();
					}
					else {
						int len1 = socketTo->send(buf);
						if (len1 < 0) {
#ifdef OS_WINDOWS
							int err = WSAGetLastError();
							if (err != WSAEWOULDBLOCK) {
								isRunning = false;
								return false;
							}
#endif
						}
						if (buf.isEmpty()) buf.clear();
					}
					isConnected = true;

					//server->arrHandler[socketTo->m_sock] = this;
					//this->saveTicks = GetTickCount();

					//server->sohandler->add(socketTo->m_sock);


					return true;
				}
			}

			int len1 = socketTo->send(buf);
			if (len1 < 0) {
#ifdef OS_WINDOWS
				int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK) {
					isRunning = false;
					return false;
				}
#endif
			}
			if (buf.isEmpty()) {
				buf.clear();
				if (isCanClose()) {
					isRunning = false;
				}
			}
		}

		return true;
	}

	bool Handler::close() {
		cout << ">>> Handler::close()" << endl;
		server->lstHandler.del(this);
		if (socket != NULL) {
			server->arrHandler[socket->m_sock] = NULL;
			server->sohandler->delSock(socket->m_sock);
		}
		if (socketTo != NULL) {
			Handler *handler2 = server->arrHandler[this->socketTo->m_sock];
			if (handler2 != NULL) {
				server->lstHandler.del(handler2);
				server->arrHandler[socketTo->m_sock] = NULL;
				server->sohandler->delSock(socketTo->m_sock);
				delete handler2;
			}
		}

		delete this;

		return true;
	}

	void Handler::runClosing() {
		isClose = true;
		close_timer = GetTickCount();
	}

	void Handler::deferClosing() {
		close_timer = GetTickCount();
	}

	bool Handler::isCanClose() {
		if (!isClose) return false;

		int tick = GetTickCount();
		if (tick - close_timer > 10000) return true;
		return false;
	}

	bool Handler::step() {
		if (!isRunning) return false;
	}

	void Handler::setConnected(bool value) {
		isConnected = value;
	}

	int Handler::connectedToSomeServer() {
		return 1;
	}
	
	bool Handler::getIsBack() {
		return isBack;
	}

	void Handler::setIsBack(bool value) {
		isBack = true;
	}
}