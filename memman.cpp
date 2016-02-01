/* *************************************************************************/
/*
 * @file   memman.cpp
 * @author sean shih
 * @date   09/01/2014 Mon  04:55 PM
 * @brief
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
 /* *************************************************************************/

#include <cstdint>
#include <cassert>
#include "memman.h"
#include "ChunkPool.h"

#pragma warning(disable:4290)

static const size_t init_offset = 2;

Memory::ChunkPool* GetChunkPools()
{
  static Memory::ChunkPool chunk_pools[] =
  {
#define META(chunk_size, chunk_num) Memory::ChunkPool(chunk_size, chunk_num),
#include "pool_seed.h"
#undef META
  };

  return chunk_pools;
}

enum Sizes
{
#define META(chunk_size, chunk_num) Size ## chunk_size = chunk_size,
#include "pool_seed.h"
#undef META
  FINALSIZE_PLUS_ONE
};

static const size_t k_max_size = FINALSIZE_PLUS_ONE - 1;
static const size_t k_oversized = ~0U;

// create chunk pool list, should only be called once
static uint8_t* make_group_list()
{
  enum GroupDef
  {
#define META(chunk_size, chunk_num) Group ## chunk_size,
#include "pool_seed.h"
#undef META
  };

  static uint8_t group_list[FINALSIZE_PLUS_ONE];
  for (size_t n = 0; n < FINALSIZE_PLUS_ONE; ++n)
  {
    if (0, false) {}
#define META(chunk_size, chunk_num) else if (n <= chunk_size) group_list[n] = Group ## chunk_size;
#include "pool_seed.h"
#undef META
    else
    {
      assert(0 && "incorrect poool table initialization");
    }
  }

  return group_list;
};

// find a chunkpool that allocates chunk just large enough for size
static size_t find_group(size_t size)
{
  const static uint8_t* const group_list = make_group_list();
  if (size > k_max_size)
    return k_oversized;
  else
    return group_list[size];
}

// malloc from chunkpool
// when the size is larger than max chunkpool size, allocate directly
void* Memory::malloca(std::size_t size)
{
  char* returned = nullptr;
  if (size > 0)
  {
    const size_t new_group = find_group(size);
    if (new_group == k_oversized)
    {
      returned = (char*)_aligned_malloc(size + ChunkPool::k_info_size, 16U);
      returned += ChunkPool::k_info_size;
      //std::printf("%u oversize, cache missed\n", size);
    }
    else
      returned = (char*)GetChunkPools()[new_group].Alloc();
  }
  return  returned;
}

// deallocate from chunkpool
// when the size is larger than max chunkpool size, deallocate directly
void Memory::dealloca(void* addr)
{
  if (addr)
  {
    auto chunk_pool = Memory::ChunkPool::GetPageHeadCheck(addr);
    if (chunk_pool)
      chunk_pool->Dealloc(addr);
    else
      _aligned_free((char*)addr - Memory::ChunkPool::k_info_size);
  }
}

// allocate an initialize to 0
void* Memory::calloca(std::size_t size)
{
  auto returned = malloca(size);
  std::memset(returned, 0, size);
  return returned;
}