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

namespace Memory
{
class ChunkPool;

void* malloca(std::size_t size);
void  dealloca(void*);
void* calloca(std::size_t size);
void* lua_alloc(void* userData, void* ptr, std::size_t oldSize, std::size_t newSize);
}
