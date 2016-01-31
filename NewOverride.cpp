/* *************************************************************************/
/*
 * @file   NewOverride.cpp
 * @author sean shih
 * @date   09/12/2014 Fri  10:49 PM
 * @brief
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
/* *************************************************************************/

#include "precompiled.h" // do not include before this line

// ********** same module headers ****************
// ********** other module headers ***************
// ********** third party headers ****************
// ********** program part ***********************

#ifndef DETECT_MEMORY_LEAK
namespace Memory
{
void* malloca(std::size_t size);
void  dealloca(void*);
}

void* operator new(size_t size)
{
    return Memory::malloca(size);
}

void* operator new(size_t size, std::nothrow_t)
{
    return Memory::malloca(size);
}

void* operator new[](size_t size, std::nothrow_t) {
    return Memory::malloca(size);
}

void operator delete (void* ptr, std::nothrow_t)
{
    Memory::dealloca(ptr);
}

void operator delete[](void* ptr, std::nothrow_t) {
    Memory::dealloca(ptr);
}

void* operator new[](size_t size) {
    return Memory::malloca(size);
}

void operator delete[](void* ptr) {
    Memory::dealloca(ptr);
}

void operator delete(void* ptr)
{
    Memory::dealloca(ptr);
}
#endif