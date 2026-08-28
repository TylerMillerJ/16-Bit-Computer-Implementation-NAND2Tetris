#ifndef MEMORY_VALIDATION
#define MEMORY_VALIDATION

#define MEMORY_MAX 32767
#define PRE_DEFINED_MEMORY 15

#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>

using std::uint16_t;


//external variables
extern std::vector<uint16_t> directlyUsedAddresses;

extern uint16_t unusedMemoryCounter; 


//functions
bool isValidAddress(uint16_t memoryAddress);

uint16_t assignValidMemoryAddress();

bool addressIsNotUsed(uint16_t target);

#endif