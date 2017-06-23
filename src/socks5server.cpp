#include "proxy.h"

namespace proxy {
	Socks5Server::Socks5Server(int port) : Server(port) {
		
	}

	Handler* Socks5Server::newHandler(Server *server, Socket *socket, Socket *socketTo) {
		return new Socks5Handler(server, socket, socketTo);
	}

}