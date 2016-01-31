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
#include "precompiled.h"

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

//static const size_t final_offset = init_offset + 15;
enum Sizes
{
#define META(chunk_size, chunk_num) Size ## chunk_size = chunk_size,
#include "pool_seed.h"
#undef META
    FINALSIZE_PLUS_ONE
};

static const size_t k_max_size = FINALSIZE_PLUS_ONE - 1;
static const size_t k_oversized = ~0U;

static uint8_t* get_group_list()
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
            DA_ASSERT(0, "incorrect poool table initialization");
    }

    return group_list;
};

static size_t find_group(size_t size)
{
    static uint8_t* group_list = get_group_list();
    if (size > k_max_size)
        return k_oversized;
    else
        return group_list[size];
}

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

void Memory::dealloca(void* addr)
{
    if (addr)
    {
        auto chunk_pool = Memory::ChunkPool::GetOrigin(addr);
        if (chunk_pool)
            chunk_pool->Dealloc(addr);
        else
            _aligned_free((char*)addr - Memory::ChunkPool::k_info_size);
    }
}

void* Memory::calloca(std::size_t size)
{
    auto returned = malloca(size);
    std::memset(returned, 0, size);
    return returned;
}

size_t g_total_lua_mem = 0;

#ifndef DETECT_MEMORY_LEAK
void* Memory::lua_alloc(void* userData, void* ptr, size_t oldSize, size_t newSize)
{
    g_total_lua_mem += newSize;
    g_total_lua_mem -= oldSize;
    if (oldSize != 0 && newSize == 0)
    {
        delete[](char*)ptr;
        return nullptr;
    }
    else
    {
        if (oldSize >= newSize)
            return ptr;
        else
        {
            size_t new_group = find_group(newSize);
            size_t old_group = find_group(oldSize);
            if (new_group == old_group && new_group != k_oversized)
                return ptr;
            else
            {
                void* new_ptr = new char[newSize];
                std::memcpy(new_ptr, ptr, oldSize);
                delete[](char*) ptr;
                return new_ptr;
            }
        }
    }
}
#else
void* Memory::lua_alloc(void* userData, void* ptr, size_t oldSize, size_t newSize)
{
    if (newSize == 0)
    {
        delete[](char*)ptr;
        return nullptr;
    }
    else
    {
        if (oldSize >= newSize)
            return ptr;
        else
        {
            void* new_ptr = new char[newSize];
            if (ptr)
            {
                std::memcpy(new_ptr, ptr, oldSize);
                delete[](char*) ptr;
            }
            return new_ptr;
        }
    }
}
#endif