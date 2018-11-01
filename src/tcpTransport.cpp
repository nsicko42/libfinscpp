#include "tcpTransport.h"
#include <unistd.h>

namespace OmronPlc
{
	tcpTransport::tcpTransport()
	{
	}

	tcpTransport::~tcpTransport()
	{
	}

	void tcpTransport::SetRemote(string ip, uint16_t port)
	{
		_ip = ip;
		_port = port;
	}

	bool tcpTransport::Connect()
	{
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket < 0)
		{
			return true;
		}

		_serveraddr.sin_family = AF_INET;
		_serveraddr.sin_addr.s_addr = inet_addr(_ip.c_str());
		_serveraddr.sin_port = htons(_port);

		if (connect(_socket, (struct sockaddr *)&_serveraddr, sizeof(_serveraddr)) < 0)
		{
			return true;
		}

		Connected = true;

		return false;
	}

	void tcpTransport::Close()
	{
		if (Connected)
		{
			close(_socket);
			_socket = 0;
			Connected = false;
		}
	}

	int tcpTransport::Send(const uint8_t command[], int cmdLen)
	{
		if (!Connected)
		{
			throw "Sockect is not connected.";
		}

//		int byteSent = send(_socket, command, cmdLen, MSG_DONTWAIT);
		int bytesSent = send(_socket, command, cmdLen, 0);

		if (bytesSent != cmdLen)
		{
			string msg = "Sending error. (Expected bytes:";
			msg += to_string(cmdLen);
			msg += " Sent: ";
			msg += to_string(bytesSent);
			msg += ")";

			throw msg.c_str();
		}
		return bytesSent;
	}

	int tcpTransport::Receive(uint8_t response[], int respLen)
	{
		if (!Connected)
		{
			throw "Socket is not connected.";
		}

		// receives the response, this is a synchronous method and can hang the process
		//
		int bytesRecv = recv(_socket, response, respLen, 0);

		// check the number of bytes received
		//
		if (bytesRecv != respLen)
		{
			string msg = "Receiving error. (Expected:";
			msg += to_string(respLen);
			msg += " Received: ";
			msg += to_string(bytesRecv);
			msg += ")";

			throw msg.c_str();
		}

		return bytesRecv;
	}
}
