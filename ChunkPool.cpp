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

#include "precompiled.h" // do not include before this line

// ********** same module headers ****************
#include "ChunkPool.h"

// ********** other module headers ***************
// ********** third party headers ****************
// ********** program part ***********************

using namespace Memory;

#define PRINT_REPORT

Memory::ChunkPool::ChunkPool(unsigned chunk_size, unsigned chunk_num)
    : _chunk_size(chunk_size),
    _page_list(nullptr),
    _free_list(nullptr),
    _chunk_num(chunk_num),
    _page_size(k_header_size + chunk_num * (_chunk_size + k_info_size)),
    _automatic_recycle(false)
{
}

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

void* Memory::ChunkPool::Alloc()
{
    DA_ASSERT(!_automatic_recycle, "Cannot allocate when automatic recycle is triggered!");

	std::lock_guard<std::mutex> lock(locker);
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

void Memory::ChunkPool::Dealloc(void* data)
{
    if (data)
    {
		std::lock_guard<std::mutex> lock(locker);
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

void Memory::ChunkPool::Link(char*& list, char* child)
{
    Next(child) = list;
    list = child;
}

void Memory::ChunkPool::FreePage(Header* page)
{
    if (page->prev)
        page->prev = page->next;
    else
        _page_list = page->next;
    _aligned_free(page);
}

char* Memory::ChunkPool::Pop(char*& list)
{
    if (!list)
        return nullptr;

    --GetPageHead(list)->chunk_count;
    char* popped = list;
    list = Next(list);

    return popped;
}

char*& Memory::ChunkPool::Next(char* node)
{
    return *(char**)node;
}

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

ChunkPool* Memory::ChunkPool::GetOrigin(void* ptr)
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

ChunkPool::Header* Memory::ChunkPool::GetPageHead(char* chunk)
{
    const Info& info = *(Info*)(chunk - k_info_size);
    return info.head;
}