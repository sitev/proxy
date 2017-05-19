#include "proxy.h"

#ifdef OS_WINDOWS
#pragma comment(lib, "core.lib")
#pragma comment(lib, "network.lib")
#endif

namespace proxy {

	Handler::Handler(Server *server, Socket *socket) {
		this->server = server;
		this->socket = socket;
	}

	Handler::~Handler() {
		delete socket;
	}

	void Handler::step() {
		if (!isRunning) return;

		Memory buf;
		buf.setPos(0);
		int len = socket->recv(buf);
		if (len > 0) {
			buf.setPos(0);

			/* Как-то должен бужет работать...
			socketSrv = new Socket();
			if (!socketSrv->create()) return false;

			socketSrv->setNonBlocking(true);
			socketSrv->connect(addr, port);
			*/

			///socketSrv->send(buf.data, len);
			cout << "send " << len << endl;
			for (int i = 0; i < len; i++) cout << buf.data[i];
			cout << endl;
		}
		return;

		buf.setPos(0);
		len = socketSrv->recv(buf);
		if (len > 0) {
			buf.setPos(0);
			socket->send(buf.data, len);
			cout << "send " << len << endl;
		}
	}
}