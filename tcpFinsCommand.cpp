#include <pthread.h>
#include <iterator>

#include "tcpFinsCommand.h"

namespace OmronPlc
{
	tcpFinsCommand::tcpFinsCommand(uint8_t ServiceID)
	{
		// transport layer
		//
		transport = new tcpTransport();

		Response = &respFinsData[0];

		cmdFins.reserve(22);
		cmdFins.empty();

		// Default fins command fields
		//
		//---- COMMAND HEADER -------------------------------------------------------
		cmdFins[ICF] = 0x80;			// 00 ICF Information control field 
		cmdFins[RSC] = 0x00;			// 01 RSC Reserved 
		cmdFins[GTC] = 0x02;			// 02 GTC Gateway count
		cmdFins[DNA] = 0x00;			// 03 DNA Destination network address (0=local network)
		cmdFins[DA1] = 0x00;			// 04 DA1 Destination node number
		cmdFins[DA2] = 0x00;			// 05 DA2 Destination unit address
		cmdFins[SNA] = 0x00;			// 06 SNA Source network address (0=local network)
		cmdFins[SA1] = 0x00;			// 07 SA1 Source node number
		cmdFins[SA2] = 0x00;			// 08 SA2 Source unit address
		cmdFins[SID] = ServiceID;		// 09 SID Service ID
		//---- COMMAND --------------------------------------------------------------
		cmdFins[MC] = 0x00;				// 10 MC Main command
		cmdFins[SC] = 0x00;				// 11 SC Subcommand
		//---- PARAMS ---------------------------------------------------------------
		cmdFins[12] = 0x00;				// 12 reserved area for additional params
		cmdFins[13] = 0x00;				// depending on fins command
		cmdFins[14] = 0x00;
		cmdFins[15] = 0x00;
		cmdFins[16] = 0x00;
		cmdFins[17] = 0x00;
		cmdFins[18] = 0x00;
		cmdFins[19] = 0x00;
		cmdFins[20] = 0x00;
		cmdFins[21] = 0x00;

		cmdFS.reserve(16);
		cmdFS.clear();
		cmdFS[0] = 0x46;				// 'F'
		cmdFS[1] = 0x49;				// 'I'
		cmdFS[2] = 0x4E;				// 'N'
		cmdFS[3] = 0x53;				// 'S'
		cmdFS[11] = 0x02;				// Command FS Sending=2 / Receiving=3

		respFS.reserve(16);
		respFS.clear();

		respFins.reserve(2048);
		respFinsData.reserve(2048);

		finsCommandLen = 0;
		finsResponseLen = 0;
	}

	tcpFinsCommand::~tcpFinsCommand()
	{
		delete transport;
	}

	bool tcpFinsCommand::Connect()
	{
		try
		{
			transport->Connect();

			return NodeAddressDataSend();
		}
		catch (const char * msg)
		{
			return false;
		}
	}

	void tcpFinsCommand::Close()
	{
		transport->Close();
	}

	void tcpFinsCommand::SetRemote(string ipaddr, uint16_t port)
	{
		transport->SetRemote(ipaddr, port);
	}

	bool tcpFinsCommand::MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count)
	{
		cmdFins[MC] = 0x01;
		cmdFins[SC] = 0x01;

		cmdFins[F_PARAM] = area;
		cmdFins[F_PARAM + 1] = (uint8_t)((address >> 8) & 0xFF);
		cmdFins[F_PARAM + 2] = (uint8_t)(address & 0xFF);
		cmdFins[F_PARAM + 3] = (uint8_t)(bit_position);
		cmdFins[F_PARAM + 4] = (uint8_t)((count >> 8) & 0xFF);
		cmdFins[F_PARAM + 5] = (uint8_t)(count & 0xFF);

		finsCommandLen = 18;

		return FrameSend();
	}

	bool tcpFinsCommand::MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[])
	{
		// command & subcomand
		//
		cmdFins[MC] = 0x01;
		cmdFins[SC] = 0x02;

		// memory area
		//
		cmdFins[F_PARAM] = area;

		// address
		//
		cmdFins[F_PARAM + 1] = (uint8_t)((address >> 8) & 0xFF);
		cmdFins[F_PARAM + 2] = (uint8_t)(address & 0xFF);

		// bit position
		//
		cmdFins[F_PARAM + 3] = bit_position;

		// count items
		//
		cmdFins[F_PARAM + 4] = (uint8_t)((count >> 8) & 0xFF);
		cmdFins[F_PARAM + 5] = (uint8_t)(count & 0xFF);

		// set command lenght (12 + additional params)
		//
		finsCommandLen = 18;

		// send the message
		//
		vector<uint8_t> w_data(data, data + count * 2);
		return FrameSend(w_data);
	}

	bool tcpFinsCommand::NodeAddressDataSend()
	{
		/* NODE ADDRESS DATA SEND buffer */
		uint8_t cmdNADS[] =
		{
		0x46, 0x49, 0x4E, 0x53, // 'F' 'I' 'N' 'S'
		0x00, 0x00, 0x00, 0x0C,	// 12 Bytes expected
		0x00, 0x00, 0x00, 0x00,	// NADS Command (0 Client to server, 1 server to client)
		0x00, 0x00, 0x00, 0x00,	// Error code (Not used)
		0x00, 0x00, 0x00, 0x00	// Client node address, 0 = auto assigned
		};


		// send NADS command
		//
		transport->Send(cmdNADS, sizeof(cmdNADS));

		// wait for a plc response
		//
		uint8_t respNADS[24] = { 0 };
		transport->Receive(respNADS, sizeof(respNADS));


		// checks response error
		//
		if (respNADS[15] != 0)
		{
			lastError = "NASD command error: ";
			lastError += respNADS[15];

			// no more actions
			//
			Close();
			return false;
		}


		// checking header error
		//
		if (respNADS[8] != 0 || respNADS[9] != 0 || respNADS[10] != 0 || respNADS[11] != 1)
		{
			lastError = "Error sending NADS command. ";
			lastError += respNADS[8];
			lastError += " ";
			lastError += respNADS[9];
			lastError += " ";
			lastError += respNADS[10];
			lastError += " ";
			lastError += respNADS[11];

			// no more actions
			//
			Close();

			return false;
		}


		// save the client & server node in the FINS command for all next conversations
		//
		cmdFins[DA1] = respNADS[23];
		cmdFins[SA1] = respNADS[19];

		return true;
	}

	bool tcpFinsCommand::FrameSend(const vector<uint8_t> &data)
	{
		static pthread_mutex_t cs_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

		respFS.clear();

		int fsLen = finsCommandLen + 8;
		fsLen += data.size();

		cmdFS[6] = (uint8_t)((fsLen >> 8) & 0xFF);
		cmdFS[7] = (uint8_t)(fsLen & 0xFF);

		/* Enter the critical section -- other threads are locked out */
		pthread_mutex_lock(&cs_mutex);

		// send frame header
		//
		transport->Send(&cmdFS[0], cmdFS.size());

		// send FINS command
		//
		transport->Send(&cmdFins[0], finsCommandLen);

		// send additional data
		//
		if (data.size() > 0)
		{
			transport->Send(&data[0], data.size());
		}

		// frame response
		//
		transport->Receive(&respFS[0], respFS.size());

		// check frame error [8]+[9]+[10]+[11]
		//
		string FSR_ERR = "";
		FSR_ERR += respFS[8];
		FSR_ERR += respFS[9];
		FSR_ERR += respFS[10];
		FSR_ERR += respFS[11];
		if (FSR_ERR.compare("0002") == 0)
		{
			lastError = "FRAME SEND error: " + FSR_ERR;
			return false;
		}

		// checks response error
		//
		if (respFS[15] != 0)
		{
			lastError = "Error receving FS command: " + respFS[15];
			return false;
		}

		// calculate the expedted response lenght
		//
		// 16 bits word ([6] + [7])
		// substract the additional 8 bytes
		//
		finsResponseLen = (respFS[6] << 8) + respFS[7];
		finsResponseLen -= 8;


		// fins command response
		//
		transport->Receive(&respFins[0], 14);


		if (finsResponseLen > 14)
		{
			// fins command response data
			//
			transport->Receive(&respFinsData[0], finsResponseLen - 14);
		}

		/*Leave the critical section -- other threads can now pthread_mutex_lock()  */
		pthread_mutex_unlock(&cs_mutex);

		if (respFins[12] != 0 || respFins[13] != 0)
		{
			lastError += "Response Code error: (Code: ";
			lastError += respFins[12];
			lastError += "Subcode: ";
			lastError += respFins[13];
			lastError += ")";

			return false;
		}

		return true;
	}
}