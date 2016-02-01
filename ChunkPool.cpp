/* *************************************************************************/
/*
 * @file   MemoryManager.cpp
 * @author sean shih
 * @date   09/12/2014 Fri  05:48 PM
 * @brief
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
 /* *************************************************************************/

#include <cassert>
#include "ChunkPool.h"
#include "mem_config.h"

using namespace Memory;


Memory::ChunkPool::ChunkPool(unsigned chunk_size, unsigned chunk_num)
  : _chunk_size(chunk_size),
  _page_list(nullptr),
  _free_list(nullptr),
  _chunk_num(chunk_num),
  _page_size(k_header_size + chunk_num * (_chunk_size + k_info_size)),
  _automatic_recycle(false)
{
}

// during destruction, free all pages if page empty
// if page not empty, keep them for automatic recycle
Memory::ChunkPool::~ChunkPool()
{
  Header* next_page = 0;
  for (Header* page = _page_list; page != nullptr; page = next_page)
  {
    next_page = page->next;
    if (page->chunk_count == _chunk_num)
      FreePage(page);
    else
    {
#ifdef PRINT_REPORT
      volatile int i = 0;
      printf("one remaining page in size %d chunk pool: %d\n", _chunk_size, i);
#endif
    }
  }
  TriggerAutomaticRecycle();
}

// allocate a new page and insert into free list
void* Memory::ChunkPool::Alloc()
{
  assert(!_automatic_recycle && "Cannot allocate when automatic recycle is triggered!");

#ifdef THREAD_SAFE
  std::lock_guard<std::mutex> lock(_locker);
#endif
  if (!_free_list)
  {
#ifdef PRINT_REPORT
    std::printf("%d chunk page used up, cache missed\n", _chunk_size);
#endif
    Header* newpage = GetNewPage();
    InsertToFreeList(newpage);
  }

  return Pop(_free_list);
}

// deallocate a page
void Memory::ChunkPool::Dealloc(void* data)
{
  if (data)
  {
#ifdef THREAD_SAFE
    std::lock_guard<std::mutex> lock(_locker);
#endif
    Link(_free_list, (char*)data);
    const auto pagehead = GetPageHead((char*)data);
    auto chunk_num = ++pagehead->chunk_count;

    if (_automatic_recycle && chunk_num == _chunk_num)
    {
      FreePage(pagehead);

#ifdef PRINT_REPORT
      volatile int i = 0;
      printf("automatically freed page with %d chunk size %d\n", _chunk_size, i);
#endif
    }
  }
}

// low level page allocation function for page initialization
ChunkPool::Header* Memory::ChunkPool::GetNewPage()
{
  if (_page_size > 0)
  {
    auto page = (Header*)_aligned_malloc(_page_size, 16);
    page->chunk_count = _chunk_num;
    page->self = this;
    page->prev = _page_list;
    page->next = nullptr;
    if (_page_list)
      _page_list->next = page;
    else
      _page_list = page;
    return page;
  }
  else
    return nullptr;
}

// link two pages together, child becomes the new list head
void Memory::ChunkPool::Link(char*& list, char* child)
{
  Next(child) = list;
  list = child;
}

// low level page free function for linking old pages together
void Memory::ChunkPool::FreePage(Header* page)
{
  if (page->prev)
    page->prev = page->next;
  else
    _page_list = page->next;
  _aligned_free(page);
}

// pop page list head
char* Memory::ChunkPool::Pop(char*& list)
{
  if (!list)
    return nullptr;

  --GetPageHead(list)->chunk_count;
  char* popped = list;
  list = Next(list);

  return popped;
}

// get next page
char*& Memory::ChunkPool::Next(char* node)
{
  return *(char**)node;
}

// insert page into free list
// the list is instrusive, which means it use part of the header as the pointer to next page in the list
void Memory::ChunkPool::InsertToFreeList(Header* page)
{
  char* node = ((char*)page + k_header_size + k_info_size);
  for (size_t i = 0; i < _chunk_num; ++i, node += (_chunk_size + k_info_size))
  {
    Info& info = *(Info*)(node - k_info_size);
    info.pattern = k_pattern;
    info.head = page;
    Link(_free_list, node);
  }
}

// check chunk validity and get page head from a chunk
ChunkPool* Memory::ChunkPool::GetPageHeadCheck(void* ptr)
{
  if (!ptr)
    return nullptr;
  else
  {
    Info& info = *(Info*)((char*)ptr - k_info_size);
    if (info.pattern != k_pattern)
      return nullptr;
    else
      return info.head->self;
  }
}

void Memory::ChunkPool::TriggerAutomaticRecycle()
{
  _automatic_recycle = true;
}

// get page head from a chunk directly without checking
ChunkPool::Header* Memory::ChunkPool::GetPageHead(char* chunk)
{
  const Info& info = *(Info*)(chunk - k_info_size);
  return info.head;
}