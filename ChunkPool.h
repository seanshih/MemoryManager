/* *************************************************************************/
/*
 * @file   MemoryManager.h
 * @author sean shih
 * @date   09/12/2014 Fri  05:48 PM
 * @brief
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
 /* *************************************************************************/

#pragma once

#include <cstdint>
#include <mutex>
#include "mem_config.h"

namespace Memory
{
  class ChunkPool
  {
  public:
    // header for each page
    struct Header
    {
      Header* next;
      Header* prev;
      ChunkPool* self;
      unsigned chunk_count;
    };

    // header for each chunk
    struct Info
    {
      Header*      head;
      uint16_t pattern;
#ifdef ALIGNED_ALLOC
      uint8_t dummy[16U - (sizeof(Header*) + sizeof(uint16_t))];
#endif
    };

    // pattern for corruption verification
    static const uint16_t k_pattern = 0xDADAU;
    static const size_t k_info_size = sizeof(Info);
    static const size_t k_header_size = sizeof(Header);

  public:
    // get page head of a given chunk with corruption checking
    static ChunkPool* GetPageHeadCheck(void* ptr);
    // get page head of a given chunk
    static Header*    GetPageHead(char* chunk);

  public:
    ChunkPool(unsigned chunk_size, unsigned chunk_num);
    ChunkPool(const ChunkPool& rhs) : ChunkPool(rhs._chunk_size, rhs._chunk_num) {};
    ~ChunkPool();

    // allocate a page
    void* Alloc();
    // deallocate a page
    void  Dealloc(void* ptr);

  private:
    // allocation helper
    Header* GetNewPage(); 
    // deallocation helper
    void    FreePage(Header* page);
    // insert a page to intrusive list of free pages
    void    InsertToFreeList(Header* page);

    // link a child page to a list, the child page becomes the new head
    void   Link(char*& list, char* child);
    // pop the head of the list
    char*  Pop(char*& list);
    // get next page in the list
    char*& Next(char* node);

    ChunkPool& operator=(const ChunkPool&) = delete;

    // automatic recycle is used when chunk pool is destructed
    // if there is remaining memory that is not deallocated
    // it may be static objects of class like smart pointers that deallocate themselves
    // as a result, chunkpools may be destructed before them
    // to solve this, if we meet a page not fully empty when chunkpool destructed
    // we keep that page until those static objects are deallocated too
    void  TriggerAutomaticRecycle();

  private:
    std::mutex _locker;
    Header* _page_list;
    char*   _free_list;
    const size_t _chunk_size;
    const size_t _chunk_num;
    const size_t _page_size;
    bool _automatic_recycle;
  };
}// namespace Memory
