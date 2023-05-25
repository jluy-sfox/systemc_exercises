#ifndef TARGET_H
#define TARGET_H

// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#define SIZE 1024

// Target module representing a simple memory

struct Memory: sc_module
{
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_target_socket<Memory> socket;
  
  int mem[SIZE];

  SC_CTOR(Memory)
  : socket ("socket")
  {
    // Register callback for incoming b_transport interface method call
    socket.register_b_transport(this, &Memory::b_transport);

    // Initialize memory with random data
    for (int i = 0; i < SIZE; i++)
      mem[i] = 0xAA000000 | (rand() % 256);
  }

  // TLM-2 blocking transport method
  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
  {
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();

    // Obliged to check address range and check for unsupported features,
    //   i.e. byte enables, streaming, and bursts
    // Can ignore DMI hint and extensions
    // Using the SystemC report handler is an acceptable way of signalling an error
    

    // Obliged to implement read and write commands
    printf("Memory:\n");
    if (cmd == tlm::TLM_READ_COMMAND) {
      memcpy(ptr, &mem[0], len);
      //std::cout << "Pointer: " << ptr << std::endl;
      printf("Successfully read data from 0x%llx\n\n", adr);
    } else if (cmd == tlm::TLM_WRITE_COMMAND) {
      memcpy(&mem[adr], ptr, len);
      printf("Successfully written data to 0x%llx\n\n", adr);
    }

    // Obliged to set response status to indicate successful completion
    trans.set_response_status( tlm::TLM_OK_RESPONSE );
  }

};

#endif
