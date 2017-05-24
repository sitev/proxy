#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "proxy.h"



namespace proxy {

	Handler::Handler(Server *server, Socket *socket) {
		this->server = server;
		this->socket = socket;
	}

	Handler::~Handler() {
		delete socket;
	}

	bool Handler::step() {
		if (!isRunning) return false;

		buf.setPos(0);
		buf.setReadPos(0);
		
		int len = socket->recv_new(buf);
		cout << "recv " << len << endl;
		if (len > 0) {
			if (!isConnected) {
				bool flag = parser.parse(buf);
				if (flag) {
					Str method = parser.getMethod();
					Str host = parser.getHost();
					int port = parser.getPort();
					cout << "method = " << method.to_string() << " host = " << host.to_string() <<
						" port = " << port << endl;

					socketSrv = new Socket();
					if (!socketSrv->create()) return false;
					Str addr = getIpByHost(host);
					if (addr == "") return false;
					flag = socketSrv->connect(addr, port);
					if (!flag) return false;
					socketSrv->setNonBlocking(true);

					if (method == "CONNECT") {
						string s = "HTTP/1.1 200 Connection established\r\n\r\n";
						Memory mem;
						mem.write((void*)s.c_str(), s.length());
						socket->send(mem);
					}
					else 
						socketSrv->send(buf.data, len);
					isConnected = true;
					return true;
				}
			}
			len = socketSrv->send(buf.data, len);
			if (len < 0) {
#ifdef OS_WINDOWS
				int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK) {
					isRunning = false;
					return false;
				}
#endif
			}
		}

		if (isConnected) {
			buf.setPos(0);
			buf.setReadPos(0);

			int len = socketSrv->recv_new(buf);
			cout << "recv Srv " << len << endl;
			if (len > 0) {
				buf.setPos(0);
				buf.setReadPos(0);
				socket->send(buf.data, len);
				cout << "send " << len << endl;
			}
			else if (len < 0) {
#ifdef OS_WINDOWS
				int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK && err != 0) {
					isRunning = false;
					return false;
				}
#endif
			}

		}
	}
}