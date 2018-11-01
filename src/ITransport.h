#pragma once

#include <string>
#include <cstdint>
using namespace std;

namespace OmronPlc
{
	class ITransport
	{
	public:
		bool Connected;

		virtual ~ITransport() {};
		virtual bool Connect() = 0;
		virtual void Close() = 0;
		virtual void SetRemote(string ip, uint16_t port) = 0;
		virtual int Send(const uint8_t command[], int cmdLen) = 0;
		virtual int Receive(uint8_t response[], int respLen) = 0;
	};
}
