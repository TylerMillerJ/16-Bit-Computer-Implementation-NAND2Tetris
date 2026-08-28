#include "memory_validation.h"
#include "symbol_table.h"

#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>

using std::uint16_t;

std::vector<uint16_t> directlyUsedAddresses;

uint16_t unusedMemoryCounter = PRE_DEFINED_MEMORY; 


bool isValidAddress(uint16_t memoryAddress){
    return true; //many test programs intentionally are accessing predefine memory so I may remove this
    if ((memoryAddress != SCREEN_MEMORY) && (memoryAddress != KBD_MEMORY) && (memoryAddress <= MEMORY_MAX)){
        return true;
    }
    std::cerr << "Program attempts to access predefined memory using memory address" << memoryAddress << std::endl;
    return false;
}


uint16_t assignValidMemoryAddress(){ //find a value that is valid and not taken
    while (++unusedMemoryCounter <= MEMORY_MAX) {
        if (isValidAddress(unusedMemoryCounter) && (addressIsNotUsed(unusedMemoryCounter))) {
            return unusedMemoryCounter; 
        }
    } 
    throw std::runtime_error("Memory Exhausted Or Attempted To Access Memory Above Max Address: " + std::to_string(MEMORY_MAX));
}

bool addressIsNotUsed(uint16_t target){
    for (uint16_t& address : directlyUsedAddresses){
        if (address == target){
            return false;
          }
    }
    return true;
}

