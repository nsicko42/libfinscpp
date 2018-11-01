#include "udpTransport.h"
#include <unistd.h>

OmronPlc::udpTransport::udpTransport()
{
}

OmronPlc::udpTransport::~udpTransport()
{
}

void OmronPlc::udpTransport::SetRemote(string ip, uint16_t port)
{
	_ip = ip;
	_port = port;
}

bool OmronPlc::udpTransport::Connect()
{
	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_socket < 0)
	{
		return true;
	}
	//setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &ReceiveTimeout, sizeof(ReceiveTimeout));

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

void OmronPlc::udpTransport::Close()
{
	if (Connected)
	{
		close(_socket);
		_socket = 0;
		Connected = false;
	}
}

int OmronPlc::udpTransport::Send(const uint8_t command[], int cmdLen)
{
	if (!Connected)
	{
		throw "Socket is not connected.";
	}

	// sends the command
	//
	int bytesSent = send(_socket, command, cmdLen, 0);

	// it checks the number of bytes sent
	//
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

int OmronPlc::udpTransport::Receive(uint8_t response[], int respLen)
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
