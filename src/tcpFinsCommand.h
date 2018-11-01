#pragma once

#include <vector>
#include <string>

#include "IFinsCommand.h"
#include "tcpTransport.h"

using namespace std;

namespace OmronPlc
{
	class tcpFinsCommand : public IFinsCommand
	{
	private:
		int finsCommandLen = 0;
		int finsResponseLen = 0;
		vector<uint8_t> cmdFS;
		vector<uint8_t> respFS;
		vector<uint8_t> respFinsData;
		vector<uint8_t> cmdFins;
		vector<uint8_t> respFins;
		tcpTransport * transport;
		string lastError;

	public:
		tcpFinsCommand(uint8_t ServiceID = 0x01);
		~tcpFinsCommand();
		virtual bool Connect();
		virtual void Close();
		virtual void SetRemote(string ipaddr, uint16_t port);
		virtual bool MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count);
		virtual bool MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[]);

	private:
		bool NodeAddressDataSend();
		bool FrameSend(const vector<uint8_t> &data = vector<uint8_t>());
	};
}
