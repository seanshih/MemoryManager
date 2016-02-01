/* *************************************************************************/
/*
 * @file   memman.h
 * @author sean shih
 * @date   09/01/2014 Mon  04:55 PM
 * @brief
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
 /* *************************************************************************/
#pragma once
#include <cstdlib>

namespace Memory
{
  // allocate size memory from chunk pool
  void* malloca(std::size_t size);
  // deallocate size memory from chunk pool
  void  dealloca(void*);
  // allocate and initialize the chunk with 0
  void* calloca(std::size_t size);
}
