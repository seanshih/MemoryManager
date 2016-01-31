/* *************************************************************************/
/*
 * @file   pool_seed.h
 * @author sean shih
 * @date   09/01/2014 Mon  04:55 PM
 * @brief
 *
 * @copyright
 *   All content (c) 2014-2015 DigiPen (USA) Corporation, all rights reserved.
 */
/* *************************************************************************/
//#define SENSITIVE

#ifndef SENSITIVE
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

#else
META(0, 0)
META(4, 2)
META(8, 2)
META(16, 2)
META(32, 2)
META(64, 2)
META(128, 2)
META(192, 2)
META(256, 2)
META(384, 2)
META(512, 2)
META(768, 2)
META(1024, 2)
META(1536, 2)
META(2048, 2)
META(3072, 2)
META(4096, 2)

#endif