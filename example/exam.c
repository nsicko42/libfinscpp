#include <cstdio>

#include "fins.h"

using namespace OmronPlc;

int main()
{
	Fins fins(TransportType::Udp);

	// Set Remote Node, but it is not nessecery when connect with Hostlink
	//
	fins.SetRemote("192.168.0.1");
	fins.Connect();

	fins.MemoryAreaRead(MemoryArea::DM, 1000, 0, 1);

	fins.Close();

	return 0;
}
