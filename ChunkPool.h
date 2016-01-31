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

// hint: try not to include headers in header files
// ********** same module headers ****************
// ********** other module headers ***************
// ********** third party headers ****************
// ********** program part ***********************

namespace Memory
{
class ChunkPool
{
public:
    struct Header
    {
        Header* next;
        Header* prev;
        ChunkPool* self;
        unsigned chunk_count;
    };

    struct Info
    {
        Header*      head;
        uint16_t pattern;
        uint8_t dummy[16U - (sizeof(Header*) + sizeof(uint16_t))];
    };

    static const uint16_t k_pattern = 0xDADAU;
    static const size_t k_info_size = sizeof(Info);
    static const size_t k_header_size = sizeof(Header);

public:
    static ChunkPool* GetOrigin(void* ptr);
    static Header*    GetPageHead(char* chunk);

public:
    ChunkPool(unsigned chunk_size, unsigned chunk_num);
	ChunkPool(const ChunkPool& rhs) : ChunkPool(rhs._chunk_size, rhs._chunk_num) {};
    ~ChunkPool();

    void* Alloc();
    void  Dealloc(void* ptr);

private:
    Header* GetNewPage();
    void    FreePage(Header* page);
    void    InsertToFreeList(Header* page);

    void   Link(char*& list, char* child);
    char*  Pop(char*& list);
    char*& Next(char* node);

    ChunkPool& operator=(const ChunkPool&) = delete;

    void  TriggerAutomaticRecycle();

private:
	std::mutex locker;
    Header* _page_list;
    char*   _free_list;
    const size_t _chunk_size;
    const size_t _chunk_num;
    const size_t _page_size;
    bool _automatic_recycle;
};
}// namespace Memory
