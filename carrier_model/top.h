#ifndef TOP_H
#define TOP_H

#include "network.h"
#include "target.h"
#include "carrier.h"

SC_MODULE(Top)
{
  Network	*network;
  Disk    *disk;
  Carrier 	*carrier;

  SC_CTOR(Top)
  {
    // Instantiate components
    network = new Network("net_initiator");
    disk = new Disk("disk");
    carrier = new Carrier("carrier");

    // Bind sockets
    carrier->mem_if_socket.bind(disk->socket);
    carrier->net_if_initiator_socket.bind(network->target_socket);
    network->initiator_socket.bind(carrier->net_if_target_socket);

  }
};
#endif
