#pragma once

#include <vector>
#include <string>

#include "IFinsCommand.h"
#include "udpTransport.h"

using namespace std;

namespace OmronPlc
{
	class udpFinsCommand : public IFinsCommand
	{
	private:
		int finsCommandLen = 0;
		int finsResponseLen = 0;
//		vector<uint8_t> cmdFS;
		vector<uint8_t> respFS;
		vector<uint8_t> respFins;
		vector<uint8_t> respFinsData;
		vector<uint8_t> cmdFins;
		udpTransport * transport;
		string lastError;

	public:
		udpFinsCommand(uint8_t ServiceID = 0x02);
		~udpFinsCommand();
		bool Connect();
		void Close();
		virtual void SetRemote(string ipaddr, uint16_t port);
		virtual bool MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count);
		virtual bool MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[]);

	private:
		bool FrameSend(const vector<uint8_t> &data = vector<uint8_t>());
	};
}
