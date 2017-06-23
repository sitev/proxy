#pragma once


namespace proxy {
	class Socks5Server : public Server {
	public:
		Socks5Server(int port);

		virtual Handler* newHandler(Server *server, Socket *socket = NULL, Socket *socketTo = NULL);
	};

}