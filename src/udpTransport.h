#pragma once

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>

#include "ITransport.h"

namespace OmronPlc
{
	class udpTransport : ITransport
	{
	private:
		struct sockaddr_in _serveraddr;
		int _socket;
		uint16_t _port;
		string _ip;

	public:
		udpTransport();
		~udpTransport();
		virtual void SetRemote(string ip, uint16_t port);
		virtual bool Connect();
		virtual void Close();
		virtual int Send(const uint8_t command[], int cmdLen);
		virtual int Receive(uint8_t response[], int respLen);
	};
}
