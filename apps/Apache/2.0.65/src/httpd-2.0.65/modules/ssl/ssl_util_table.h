/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*                      _             _
 *  _ __ ___   ___   __| |    ___ ___| |  mod_ssl
 * | '_ ` _ \ / _ \ / _` |   / __/ __| |  Apache Interface to OpenSSL
 * | | | | | | (_) | (_| |   \__ \__ \ |
 * |_| |_| |_|\___/ \__,_|___|___/___/_|
 *                      |_____|
 *  ssl_util_table.h
 *  High Performance Hash Table Header
 */

/*
 * Generic hash table defines
 * Table 4.1.0 July-28-1998
 *
 * This library is a generic open hash table with buckets and
 * linked lists.  It is pretty high performance.  Each element
 * has a key and a data.  The user indexes on the key to find the
 * data.
 *
 * Copyright 1998 by Gray Watson <gray@letters.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose and without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies,
 * and that the name of Gray Watson not be used in advertising or
 * publicity pertaining to distribution of the document or software
 * without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#ifndef __SSL_UTIL_TABLE_H__
#define __SSL_UTIL_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * To build a "key" in any of the below routines, pass in a pointer to
 * the key and its size [i.e. sizeof(int), etc].  With any of the
 * "key" or "data" arguments, if their size is < 0, it will do an
 * internal strlen of the item and add 1 for the \0.
 *
 * If you are using firstkey() and nextkey() functions, be careful if,
 * after starting your firstkey loop, you use delete or insert, it
 * will not crash but may produce interesting results.  If you are
 * deleting from firstkey to NULL it will work fine.
 */

/* return types for table functions */
#define TABLE_ERROR_NONE    1   /* no error from function */
#define TABLE_ERROR_PNT     2   /* bad table pointer */
#define TABLE_ERROR_ARG_NULL    3   /* buffer args were null */
#define TABLE_ERROR_SIZE    4   /* size of data was bad */
#define TABLE_ERROR_OVERWRITE   5   /* key exists and we cant overwrite */
#define TABLE_ERROR_NOT_FOUND   6   /* key does not exist */
#define TABLE_ERROR_ALLOC   7   /* memory allocation error */
#define TABLE_ERROR_LINEAR  8   /* no linear access started */
#define TABLE_ERROR_OPEN    9   /* could not open file */
#define TABLE_ERROR_SEEK    10  /* could not seek to pos in file */
#define TABLE_ERROR_READ    11  /* could not read from file */
#define TABLE_ERROR_WRITE   12  /* could not write to file */
#define TABLE_ERROR_EMPTY   13  /* table is empty */
#define TABLE_ERROR_NOT_EMPTY   14  /* table contains data */
#define TABLE_ERROR_ALIGNMENT   15  /* invalid alignment value */

/*
 * Table flags set with table_attr.
 */

/*
 * Automatically adjust the number of table buckets on the fly.
 * Whenever the number of entries gets above some threshold, the
 * number of buckets is realloced to a new size and each entry is
 * re-hashed.  Although this may take some time when it re-hashes, the
 * table will perform better over time.
 */
#define TABLE_FLAG_AUTO_ADJUST  (1<<0)

/*
 * If the above auto-adjust flag is set, also adjust the number of
 * table buckets down as we delete entries.
 */
#define TABLE_FLAG_ADJUST_DOWN  (1<<1)

/* structure to walk through the fields in a linear order */
typedef struct {
  unsigned int  tl_magic;   /* magic structure to ensure correct init */
  unsigned int  tl_bucket_c;    /* where in the table buck array we are */
  unsigned int  tl_entry_c; /* in the bucket, which entry we are on */
} table_linear_t;

typedef int (*table_compare_t)(const void *key1, const int key1_size,
                   const void *data1, const int data1_size,
                   const void *key2, const int key2_size,
                   const void *data2, const int data2_size);

#ifndef TABLE_PRIVATE
typedef void    table_t;
typedef void    table_entry_t;
#endif

/*
 * Prototypes
 */
extern table_t        *table_alloc(const unsigned int bucket_n, int *error_p, void *(*malloc_f)(void *opt_param, size_t size), void *(*calloc_f)(void *opt_param, size_t number, size_t size), void *(*realloc_f)(void *opt_param, void *ptr, size_t size), void (*free_f)(void *opt_param, void *ptr), void *opt_param);
extern int             table_attr(table_t *table_p, const int attr);
extern int             table_set_data_alignment(table_t *table_p, const int alignment);
extern int             table_clear(table_t *table_p);
extern int             table_free(table_t *table_p);
extern int             table_insert_kd(table_t *table_p, const void *key_buf, const int key_size, const void *data_buf, const int data_size, void **key_buf_p, void **data_buf_p, const char overwrite_b);
extern int             table_insert(table_t *table_p, const void *key_buf, const int key_size, const void *data_buf, const int data_size, void **data_buf_p, const char overwrite_b);
extern int             table_retrieve(table_t *table_p, const void *key_buf, const int key_size, void **data_buf_p, int *data_size_p);
extern int             table_delete(table_t *table_p, const void *key_buf, const int key_size, void **data_buf_p, int *data_size_p);
extern int             table_delete_first(table_t *table_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern int             table_info(table_t *table_p, int *num_buckets_p, int *num_entries_p);
extern int             table_adjust(table_t *table_p, const int bucket_n);
extern const char     *table_strerror(const int error);
extern int             table_type_size(void);
extern int             table_first(table_t *table_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern int             table_next(table_t *table_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern int             table_this(table_t *table_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern int             table_first_r(table_t *table_p, table_linear_t *linear_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern int             table_next_r(table_t *table_p, table_linear_t *linear_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern int             table_this_r(table_t *table_p, table_linear_t *linear_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);
extern table_entry_t **table_order(table_t *table_p, table_compare_t compare, int *num_entries_p, int *error_p);
extern int             table_entry_info(table_t *table_p, table_entry_t *entry_p, void **key_buf_p, int *key_size_p, void **data_buf_p, int *data_size_p);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SSL_UTIL_TABLE_H__ */
