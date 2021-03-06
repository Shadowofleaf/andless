/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: m4a.h 17847 2008-06-28 18:10:04Z bagder $
 *
 * Copyright (C) 2005 Dave Chapman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#ifndef _M4A_H
#define _M4A_H

#ifndef ICODE_ATTR_ALAC
#define ICODE_ATTR_ALAC ICODE_ATTR
#define ICODE_ATTR
#define ICONST_ATTR
#define IBSS_ATTR
#endif


#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <sched.h>
#include "../main.h"



/* AAC codecdata appears to always be less than 8 bytes - see
   AudioSpecificConfig2 in libfaad/mp4.c

   ALAC codecdata appears to always be 44 bytes (see alac_set_info in
   libalac/alac.c) but my test file contains 56 bytes.

   So we go safe and round up to 64 bytes - if we find more than this,
   we give an error (even though we could possibly continue), so we
   can increase this buffer.
*/

#define MAX_CODECDATA_SIZE  64

typedef struct {
//  struct msm_ctx* ctx;
  int fd;
  int curpos;
  int eof;
  int err;
} stream_t;

typedef uint32_t fourcc_t;

typedef struct
{
    uint16_t num_channels;
    uint16_t sound_sample_size;
    uint32_t sound_sample_rate;
    fourcc_t format;
    void *buf;

    struct {
        uint32_t first_chunk;
        uint32_t num_samples;
    } *sample_to_chunk;
    uint32_t num_sample_to_chunks;
    
    uint32_t *chunk_offset;
    uint32_t num_chunk_offsets;
    
    struct {
        uint32_t sample_count;
        uint32_t sample_duration;
    } *time_to_sample;
    uint32_t num_time_to_samples;

    uint16_t *sample_byte_size;
    uint32_t num_sample_byte_sizes;

    uint32_t codecdata_len;
    uint8_t codecdata[MAX_CODECDATA_SIZE];

    int mdat_offset;
    uint32_t mdat_len;
#if 0
    void *mdat;
#endif
} demux_res_t;

int qtmovie_read(stream_t *stream, demux_res_t *demux_res);

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ( \
    ( (int32_t)(char)(ch0) << 24 ) | \
    ( (int32_t)(char)(ch1) << 16 ) | \
    ( (int32_t)(char)(ch2) << 8 ) | \
    ( (int32_t)(char)(ch3) ) )
#endif

#ifndef SLPITFOURCC
/* splits it into ch0, ch1, ch2, ch3 - use for printf's */
#define SPLITFOURCC(code) \
    (char)((int32_t)code >> 24), \
    (char)((int32_t)code >> 16), \
    (char)((int32_t)code >> 8), \
    (char)code
#endif

void stream_read(stream_t *stream, size_t len, void *buf);

int32_t stream_tell(stream_t *stream);
int32_t stream_read_int32(stream_t *stream);
uint32_t stream_read_uint32(stream_t *stream);

int16_t stream_read_int16(stream_t *stream);
uint16_t stream_read_uint16(stream_t *stream);

int8_t stream_read_int8(stream_t *stream);
uint8_t stream_read_uint8(stream_t *stream);

void stream_skip(stream_t *stream, size_t skip);

int stream_eof(stream_t *stream);

void stream_create(stream_t *stream, msm_ctx* ci);
int get_sample_info(demux_res_t *demux_res, uint32_t sample,
    uint32_t *sample_duration, uint32_t *sample_byte_size);
unsigned int get_sample_offset(demux_res_t *demux_res, uint32_t sample);
unsigned int alac_seek (demux_res_t* demux_res, stream_t* stream,
    uint32_t sound_sample_loc, uint32_t* sound_samples_done, 
    int* current_sample);
unsigned int alac_seek_raw (demux_res_t* demux_res, stream_t* stream,
    uint32_t file_loc, uint32_t* sound_samples_done, int* current_sample);

#endif /* STREAM_H */
