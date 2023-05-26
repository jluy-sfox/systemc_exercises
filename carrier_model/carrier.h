#ifndef CARRIER_H
#define CARRIER_H

#include "systemc"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include <string>

#define DATA_LENGTH 512
#define NUM_TRANSACTIONS 5
#define BYTE_ALIGNMENT 4096

// Initiator module generating generic payload transactions

struct Carrier: sc_module
{
	
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<Carrier> net_if_initiator_socket;
  tlm_utils::simple_target_socket<Carrier> net_if_target_socket;
  tlm_utils::simple_initiator_socket<Carrier> mem_if_socket;
  
  // Initialize internal buffer array
  unsigned int *data_buffer;
  int data;

  SC_CTOR(Carrier)
  	: net_if_initiator_socket("net_if_initiator_socket"),
  	  net_if_target_socket("net_if_target_socket"),
  	  mem_if_socket("mem_if_socket")
  {
    SC_THREAD(thread_process);
  }

  void thread_process()
  {
    
    // Allocate memory space in heap for the data buffer
    data_buffer = (unsigned int*)malloc(sizeof(unsigned int)*BYTE_ALIGNMENT);
	
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10, SC_NS);
    
    int j = 0;
    	
    // Generate a random sequence of reads and writes
    for (int i = 0; i < BYTE_ALIGNMENT*NUM_TRANSACTIONS; i = i+BYTE_ALIGNMENT)
    {

      tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2); // randomize a command
      
      if (cmd == tlm::TLM_WRITE_COMMAND) data = 0xFF000000 | i;

      // Initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
      trans->set_command(cmd);
      trans->set_address(i);
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data_buffer[i]) );
      trans->set_data_length(DATA_LENGTH);
      trans->set_dmi_allowed(false); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

      // Print the memory payload
      std::cout << "Carrier:" << std::endl;
      std::cout << "Initiated a memory request!" << std::endl;
      std::cout << "Pointer: " << &data_buffer[i] << std::endl;
      std::cout << "Command: " << (cmd ? 'W' : 'R') << std::endl;
      std::cout << "Data Length: " << DATA_LENGTH << std::endl;
      printf("Address: 0x%x\n\n", i);
    
      // Initiate memory request
      mem_if_socket->b_transport(*trans, delay);
     
      // Initiator obliged to check response status and delay
      if ( trans->is_response_error() )
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
      
      // Store read data or written data in the carrier buffer
      data_buffer[j] = data;
      ++j;
      
    }
    
    printf("Data Buffer:\n");
    printf("[");
    for (int i=0; i < j; ++i) {
      printf(" %d", data_buffer[i]);
    }
    printf(" ]\n\n");
    
    // For testing of read_data and write_data
    /*
    int buffer_size = j;
    buffer_size = read_data(1000, 2, data_buffer, buffer_size);
    
    printf("Data Buffer:\n");
    printf("[");
    for (int i=0; i < buffer_size; ++i) {
      printf(" %d", data_buffer[i]);
    }
    printf(" ]\n\n");
    */
  
  }
  
  void read_data(long int blk_addr, int blk_cnt, unsigned int *buffer, unsigned int buffer_size) {
    
    sc_time delay = sc_time(10, SC_NS);
    
    tlm::tlm_command cmd = tlm::TLM_READ_COMMAND;
              
    // Print the payload properties
    std::cout << "Carrier:" << std::endl;
    std::cout << "Initiated a memory read request!" << std::endl;
    std::cout << "Data Length: " << DATA_LENGTH << "\n" << std::endl;
        
    for (int i=0; i<blk_cnt; ++i) {
      
      // Initialize a memory payload
      tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
      
      // Set the attributes of the payload
      trans->set_command(cmd);
      trans->set_address(blk_addr);
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(&buffer[buffer_size]) ); // Need to check
      trans->set_data_length(DATA_LENGTH);
      trans->set_dmi_allowed(false); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value
      
      printf("Reading data from 0x%lx\n", blk_addr);
      
      // Initiate memory request
      mem_if_socket->b_transport(*trans, delay);
      
      data = rand()%4095; // Dummy data which simulates data read from the memory
            
      if ( trans->is_response_error() ) {
      	SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
      }
      
      // Store read data in the carrier buffer
      buffer[buffer_size] = data;
      ++buffer_size;
      
      // Update the block address to point to the next block
      blk_addr = blk_addr+BYTE_ALIGNMENT;
    
    }
    
    printf("Done reading data!\n\n");
    
  }
  
  void write_data(long int blk_addr, int blk_cnt, unsigned int *buffer, unsigned int buffer_size) {
    
    sc_time delay = sc_time(10, SC_NS);
    
    tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
          
    // Print the payload properties
    std::cout << "Carrier:" << std::endl;
    std::cout << "Initiated a memory write request!" << std::endl;
    std::cout << "Data Length: " << DATA_LENGTH << "\n" << std::endl;
    
    int rand_data = rand()%4095;
    
    for (int i=0; i<blk_cnt; ++i) {
      
      // Initialize a memory payload
      tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
      
      // Insert data in the buffer to be written to the memory
      buffer[buffer_size] = rand_data;
      
      // Set the attributes of the payload
      trans->set_command(cmd);
      trans->set_address(blk_addr);
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(&buffer[buffer_size]) ); // Need to check
      trans->set_data_length(DATA_LENGTH);
      trans->set_dmi_allowed(false); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value
      
      printf("Writing data to 0x%lx\n", blk_addr);
      
      // Initiate memory request
      mem_if_socket->b_transport(*trans, delay);
      
      if ( trans->is_response_error() ) {
      	SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
      }
      
      // Update the block address to point to the next block
      blk_addr = blk_addr+BYTE_ALIGNMENT;
    
    }
    
    printf("Done reading data!\n\n"); 
  
  }

};

#endif
