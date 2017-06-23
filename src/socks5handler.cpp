#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "proxy.h"

namespace proxy {

	Socks5Handler::Socks5Handler(Server *server, Socket *socket, Socket *socketTo) : Handler(server, socket, socketTo) {
	}

	Socks5Handler::~Socks5Handler() {
	}

	void Socks5Handler::readAddrPort(Str &addr, int &port) {
		uchar typ;
		buf.readUChar(typ);
		if (typ == 1) {
			uchar data[6];
			buf.read(data, 6);
			addr = to_string(data[0]) + "." + to_string(data[1]) + "." + to_string(data[2]) + "." + to_string(data[3]);
			int d5 = data[4], d6 = data[5];
			port = d5 * 256 + d6;
		}
		else if (typ == 3) {
			uchar len;
			buf.readUChar(len);
			char host[256];
			buf.read(host, len);
			host[len] = 0;

			//buf.readUChar(len);
			//uchar a;
			//buf.readUChar(a);
			uchar data[2];
			buf.read(data, 2);
			int d5 = data[0], d6 = data[1];
			port = d5 * 256 + d6;

			hostent* hostname = gethostbyname(host);
			if (hostname) {
				in_addr * address = (in_addr *)hostname->h_addr;
				addr = inet_ntoa(*address);
			}
			else addr = "";
		}
		else {
			cout << "ERROR: typ == 4" << endl;
			getchar();
		}
	}

	int Socks5Handler::queryFromClient(Str &addr, int &port) {
		uchar data[3];
		buf.read(data, 3);

		if (data[0] == 5 && data[1] == 1 && data[2] == 0) {
			readAddrPort(addr, port);

			return 1;
		}
		return 0;
	}

	int Socks5Handler::tryConnectToSomeServer(Str &addr, int &port) {
		socketTo = new Socket();
		if (!socketTo->create()) return -1;
		cout << ">>> try connect sock = " << socketTo->m_sock << endl;
		socketTo->setNonBlocking(true);
		bool flag = socketTo->connect(addr, port);

		Handler *handler = new Socks5Handler(server, socketTo, socket);
		handler->setIsBack(true);
		server->lstHandler.add(handler);
		server->arrHandler[socketTo->m_sock] = handler;
		handler->saveTicks = GetTickCount();

		server->sohandler->add(socketTo->m_sock);

		return 1;
	}

	int Socks5Handler::connectedToSomeServer() {
		cout << ">>> connected sock = " << socketTo->m_sock << endl;

		isConnected = true;

		Socks5Handler *handler = (Socks5Handler*)server->arrHandler[socketTo->m_sock];
		handler->setConnected(true);

		return handler->sendToSomeServer();
	}

	int Socks5Handler::sendToSomeServer() {
		int len1 = socketTo->send(buf);
		if (len1 < 0) {
#ifdef OS_WINDOWS
			int err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK) {
				isRunning = false;
				return -1;
			}
#endif
		}
		if (buf.isEmpty()) {
			buf.clear();
			if (isCanClose()) {
				cout << "--- isCanClose()" << endl;
				isRunning = false;
			}
		}

		return 1;
	}

	bool Socks5Handler::doCommunication() {
		//cout << "r1 ";

		int len = socket->recv(buf);
		if (len > 0) {
			if (!isBack) {
				if (!isAuth) {
					if (buf.getDepth() < 3) return true;

					uchar ver, nmethods, methods;
					buf.readUChar(ver);
					if (ver != 5) return false;
					buf.readUChar(nmethods);
					for (int i = 0; i < nmethods; i++) buf.readUChar(methods);

					omem.setPos(0);
					omem.writeUChar(5);
					omem.writeUChar(0);

					// Отсылаем ответ клиенту
					bool iResult = socket->sendAll(omem.data, 2);
					if (!iResult) {
						//LOGGER_SCREEN("SOCKS5: send failed with error: %d", WSAGetLastError());
						LOGGER_SCREEN("SOCKS5: send failed with error: ");
						//break;
					}

					if (buf.isEmpty()) buf.clear();

					isAuth = true;

					return true;
				}
				else if (!isTryConnect) {
					int depth = buf.getDepth();
					if (depth < 10) return true;
					int save_read_pos = buf.getReadPos();
					if (save_read_pos != 0)
						int a = 1;

					Str addr;
					int port;
					int ret = queryFromClient(addr, port);
					if (ret == 1) {
						ret = tryConnectToSomeServer(addr, port);
						if (ret == 1) {
							isTryConnect = true;

							char ch = buf.data[save_read_pos + 1];
							buf.data[save_read_pos + 1] = 0;
							bool flag = socket->sendAll(&(buf.data[save_read_pos]), depth);
							int a = 1;
						}
					}
					if (ret < 0) {
						cout << "--- ret < 0" << endl;
						return false;
					}
					return true;
				}
			}

			if (isBack)
				int a = 1;

			//cout << "r2 ";
			if (isConnected) {
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
				//cout << "r3 ";
				if (buf.isEmpty()) {
					buf.clear();
					if (isCanClose()) {
						cout << "--- isCanClose()" << endl;
						isRunning = false;
						return false;
					}
				}
			}
		
		}

		//cout << "r4 ";
		return true;
	}

	bool Socks5Handler::read() {
		if (!isRunning) {
			cout << "!isRunning" << endl;
			return false;
		}

		return doCommunication();
	}



}