#ifndef NETWORK_H
#define NETWORK_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"


// Initiator module generating generic payload transactions

struct Network: sc_module
{
	
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<Network> initiator_socket;
  tlm_utils::simple_target_socket<Network> target_socket;

  SC_CTOR(Network)
  	: initiator_socket("initiator_socket"),
  	  target_socket("target_socket")
  {
    SC_THREAD(thread_process);
  }

  void thread_process()
  {
	
    
  }

  // Internal data buffer used by initiator with generic payload
  int data;
};

#endif
