#ifndef TOP_H
#define TOP_H

#include "initiator.h"
#include "target.h"

SC_MODULE(Top)
{
  Initiator *initiator;
  Memory    *memory_0;
  Memory    *memory_1;

  SC_CTOR(Top)
  {
    // Instantiate components
    initiator = new Initiator("initiator");
    memory_0 = new Memory("memory_0");
    memory_1 = new Memory("memory_1");


    // One initiator is bound directly to one target with no intervening bus

    // Bind initiator socket to target socket
    initiator->socket_0.bind(memory_0->socket);
    initiator->socket_1.bind(memory_1->socket);
  }
};
#endif
