/* *************************************************************************/
/*
 * @file   pool_seed.h
 * @author sean shih
 * @date   09/01/2014 Mon  04:55 PM
 * @brief  page size definition
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
 /* *************************************************************************/

// use META(chunk_size, count) to define page size
// where 
// chunk_size = the size of chunk
// count = number of chunks per page
// the page size will be chunk_size * count
// 
// smaller page size will result in more allocations but less memory waste

META(0, 0)
META(4, 2048)
META(8, 2048)
META(16, 4096 * 3)
META(32, 16384 * 3)
META(64, 2048)
META(128, 4096)
META(192, 1024)
META(256, 128)
META(384, 128)
META(512, 128)
META(768, 128)
META(1024, 32)
META(1536, 32)
META(2048, 32)
META(3072, 16)
META(4096, 16)
