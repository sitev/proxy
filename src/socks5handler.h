#pragma once

namespace proxy {

	class Socks5Handler : public Handler {
	protected:
		int mode = 0;
		Memory omem;
		bool isAuth = false;
		bool isTryConnect = false;
		Str addr;
		int port;
	public:
		Socks5Handler(Server *server, Socket *socket = NULL, Socket *socketTo = NULL);
		~Socks5Handler();

		virtual void readAddrPort(Str &addr, int &port);
		virtual int queryFromClient(Str &addr, int &port);
		virtual int tryConnectToSomeServer(Str &addr, int &port);
		virtual int connectedToSomeServer();

		virtual bool doCommunication();
		virtual bool read();

		virtual int sendToSomeServer();

	};

}