/* Copyright 2000-2005 The Apache Software Foundation or its licensors, as
 * applicable.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "apr.h"
#include "apr_lib.h"
#include "apr_strings.h"
#include "apr_pools.h"
#include "apr_tables.h"
#include "apr_buckets.h"
#include "apr_errno.h"
#define APR_WANT_MEMFUNC
#define APR_WANT_STRFUNC
#include "apr_want.h"

#if APR_HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

static apr_status_t brigade_cleanup(void *data) 
{
    return apr_brigade_cleanup(data);
}

APU_DECLARE(apr_status_t) apr_brigade_cleanup(void *data)
{
    apr_bucket_brigade *b = data;
    apr_bucket *e;

    /*
     * Bah! We can't use APR_RING_FOREACH here because this bucket has
     * gone away when we dig inside it to get the next one.
     */
    while (!APR_BRIGADE_EMPTY(b)) {
        e = APR_BRIGADE_FIRST(b);
        apr_bucket_delete(e);
    }
    /*
     * We don't need to free(bb) because it's allocated from a pool.
     */
    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_destroy(apr_bucket_brigade *b)
{
    apr_pool_cleanup_kill(b->p, b, brigade_cleanup);
    return apr_brigade_cleanup(b);
}

APU_DECLARE(apr_bucket_brigade *) apr_brigade_create(apr_pool_t *p,
                                                     apr_bucket_alloc_t *list)
{
    apr_bucket_brigade *b;

    b = apr_palloc(p, sizeof(*b));
    b->p = p;
    b->bucket_alloc = list;

    APR_RING_INIT(&b->list, apr_bucket, link);

    apr_pool_cleanup_register(b->p, b, brigade_cleanup, apr_pool_cleanup_null);
    return b;
}

APU_DECLARE(apr_bucket_brigade *) apr_brigade_split(apr_bucket_brigade *b,
                                                    apr_bucket *e)
{
    apr_bucket_brigade *a;
    apr_bucket *f;

    a = apr_brigade_create(b->p, b->bucket_alloc);
    /* Return an empty brigade if there is nothing left in 
     * the first brigade to split off 
     */
    if (e != APR_BRIGADE_SENTINEL(b)) {
        f = APR_RING_LAST(&b->list);
        APR_RING_UNSPLICE(e, f, link);
        APR_RING_SPLICE_HEAD(&a->list, e, f, apr_bucket, link);
    }

    APR_BRIGADE_CHECK_CONSISTENCY(a);
    APR_BRIGADE_CHECK_CONSISTENCY(b);

    return a;
}

APU_DECLARE(apr_status_t) apr_brigade_partition(apr_bucket_brigade *b,
                                                apr_off_t point,
                                                apr_bucket **after_point)
{
    apr_bucket *e;
    const char *s;
    apr_size_t len;
    apr_status_t rv;

    if (point < 0) {
        /* this could cause weird (not necessarily SEGV) things to happen */
        return APR_EINVAL;
    }
    if (point == 0) {
        *after_point = APR_BRIGADE_FIRST(b);
        return APR_SUCCESS;
    }

    APR_BRIGADE_CHECK_CONSISTENCY(b);

    APR_BRIGADE_FOREACH(e, b) {
        if ((e->length == (apr_size_t)(-1)) && (point > (apr_size_t)(-1))) {
            /* XXX: point is too far out to simply split this bucket,
             * we must fix this bucket's size and keep going... */
            rv = apr_bucket_read(e, &s, &len, APR_BLOCK_READ);
            if (rv != APR_SUCCESS) {
                *after_point = e;
                return rv;
            }
        }
        if ((point < e->length) || (e->length == (apr_size_t)(-1))) {
            /* We already checked e->length -1 above, so we now
             * trust e->length < MAX_APR_SIZE_T.
             * First try to split the bucket natively... */
            if ((rv = apr_bucket_split(e, (apr_size_t)point)) 
                    != APR_ENOTIMPL) {
                *after_point = APR_BUCKET_NEXT(e);
                return rv;
            }

            /* if the bucket cannot be split, we must read from it,
             * changing its type to one that can be split */
            rv = apr_bucket_read(e, &s, &len, APR_BLOCK_READ);
            if (rv != APR_SUCCESS) {
                *after_point = e;
                return rv;
            }

            /* this assumes that len == e->length, which is okay because e
             * might have been morphed by the apr_bucket_read() above, but
             * if it was, the length would have been adjusted appropriately */
            if (point < e->length) {
                rv = apr_bucket_split(e, (apr_size_t)point);
                *after_point = APR_BUCKET_NEXT(e);
                return rv;
            }
        }
        if (point == e->length) {
            *after_point = APR_BUCKET_NEXT(e);
            return APR_SUCCESS;
        }
        point -= e->length;
    }
    *after_point = APR_BRIGADE_SENTINEL(b); 
    return APR_INCOMPLETE;
}

APU_DECLARE(apr_status_t) apr_brigade_length(apr_bucket_brigade *bb,
                                             int read_all, apr_off_t *length)
{
    apr_off_t total = 0;
    apr_bucket *bkt;

    APR_BRIGADE_FOREACH(bkt, bb) {
        if (bkt->length == (apr_size_t)(-1)) {
            const char *ignore;
            apr_size_t len;
            apr_status_t status;

            if (!read_all) {
                *length = -1;
                return APR_SUCCESS;
            }

            if ((status = apr_bucket_read(bkt, &ignore, &len,
                                          APR_BLOCK_READ)) != APR_SUCCESS) {
                return status;
            }
        }

        total += bkt->length;
    }

    *length = total;
    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_flatten(apr_bucket_brigade *bb,
                                              char *c, apr_size_t *len)
{
    apr_size_t actual = 0;
    apr_bucket *b;
 
    APR_BRIGADE_FOREACH(b, bb) {
        const char *str;
        apr_size_t str_len;
        apr_status_t status;

        status = apr_bucket_read(b, &str, &str_len, APR_BLOCK_READ);
        if (status != APR_SUCCESS) {
            return status;
        }

        /* If we would overflow. */
        if (str_len + actual > *len) {
            str_len = *len - actual;
        }

        /* XXX: It appears that overflow of the final bucket
         * is DISCARDED without any warning to the caller.
         *
         * No, we only copy the data up to their requested size.  -- jre
         */
        memcpy(c, str, str_len);

        c += str_len;
        actual += str_len;

        /* This could probably be actual == *len, but be safe from stray
         * photons. */
        if (actual >= *len) {
            break;
        }
    }

    *len = actual;
    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_pflatten(apr_bucket_brigade *bb,
                                               char **c,
                                               apr_size_t *len,
                                               apr_pool_t *pool)
{
    apr_off_t actual;
    apr_size_t total;
    apr_status_t rv;

    apr_brigade_length(bb, 1, &actual);
    
    /* XXX: This is dangerous beyond belief.  At least in the
     * apr_brigade_flatten case, the user explicitly stated their
     * buffer length - so we don't up and palloc 4GB for a single
     * file bucket.  This API must grow a useful max boundry,
     * either compiled-in or preset via the *len value.
     *
     * Shouldn't both fn's grow an additional return value for 
     * the case that the brigade couldn't be flattened into the
     * provided or allocated buffer (such as APR_EMOREDATA?)
     * Not a failure, simply an advisory result.
     */
    total = (apr_size_t)actual;

    *c = apr_palloc(pool, total);
    
    rv = apr_brigade_flatten(bb, *c, &total);

    if (rv != APR_SUCCESS) {
        return rv;
    }

    *len = total;
    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_split_line(apr_bucket_brigade *bbOut,
                                                 apr_bucket_brigade *bbIn,
                                                 apr_read_type_e block,
                                                 apr_off_t maxbytes)
{
    apr_off_t readbytes = 0;

    while (!APR_BRIGADE_EMPTY(bbIn)) {
        const char *pos;
        const char *str;
        apr_size_t len;
        apr_status_t rv;
        apr_bucket *e;

        e = APR_BRIGADE_FIRST(bbIn);
        rv = apr_bucket_read(e, &str, &len, block);

        if (rv != APR_SUCCESS) {
            return rv;
        }

        pos = memchr(str, APR_ASCII_LF, len);
        /* We found a match. */
        if (pos != NULL) {
            apr_bucket_split(e, pos - str + 1);
            APR_BUCKET_REMOVE(e);
            APR_BRIGADE_INSERT_TAIL(bbOut, e);
            return APR_SUCCESS;
        }
        APR_BUCKET_REMOVE(e);
        if (APR_BUCKET_IS_METADATA(e) || len > APR_BUCKET_BUFF_SIZE/4) {
            APR_BRIGADE_INSERT_TAIL(bbOut, e);
        }
        else {
            if (len > 0) {
                rv = apr_brigade_write(bbOut, NULL, NULL, str, len);
                if (rv != APR_SUCCESS) {
                    return rv;
                }
            }
            apr_bucket_destroy(e);
        }
        readbytes += len;
        /* We didn't find an APR_ASCII_LF within the maximum line length. */
        if (readbytes >= maxbytes) {
            break;
        }
    }

    return APR_SUCCESS;
}


APU_DECLARE(apr_status_t) apr_brigade_to_iovec(apr_bucket_brigade *b, 
                                               struct iovec *vec, int *nvec)
{
    int left = *nvec;
    apr_bucket *e;
    struct iovec *orig;
    apr_size_t iov_len;
    apr_status_t rv;

    orig = vec;
    APR_BRIGADE_FOREACH(e, b) {
        if (left-- == 0)
            break;

        rv = apr_bucket_read(e, (const char **)&vec->iov_base, &iov_len,
                             APR_NONBLOCK_READ);
        if (rv != APR_SUCCESS)
            return rv;
        vec->iov_len = iov_len; /* set indirectly in case size differs */
        ++vec;
    }

    *nvec = vec - orig;
    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_vputstrs(apr_bucket_brigade *b, 
                                               apr_brigade_flush flush,
                                               void *ctx,
                                               va_list va)
{
    for (;;) {
        const char *str = va_arg(va, const char *);
        apr_status_t rv;

        if (str == NULL)
            break;

        rv = apr_brigade_write(b, flush, ctx, str, strlen(str));
        if (rv != APR_SUCCESS)
            return rv;
    }

    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_putc(apr_bucket_brigade *b,
                                           apr_brigade_flush flush, void *ctx,
                                           const char c)
{
    return apr_brigade_write(b, flush, ctx, &c, 1);
}

APU_DECLARE(apr_status_t) apr_brigade_write(apr_bucket_brigade *b,
                                            apr_brigade_flush flush,
                                            void *ctx, 
                                            const char *str, apr_size_t nbyte)
{
    apr_bucket *e = APR_BRIGADE_LAST(b);
    apr_size_t remaining = APR_BUCKET_BUFF_SIZE;
    char *buf = NULL;

    if (!APR_BRIGADE_EMPTY(b) && APR_BUCKET_IS_HEAP(e)) {
        apr_bucket_heap *h = e->data;

        /* HEAP bucket start offsets are always in-memory, safe to cast */
        remaining = h->alloc_len - (e->length + (apr_size_t)e->start);
        buf = h->base + e->start + e->length;
    }

    if (nbyte > remaining) {
        /* either a buffer bucket exists but is full, 
         * or no buffer bucket exists and the data is too big
         * to buffer.  In either case, we should flush.  */
        if (flush) {
            e = apr_bucket_transient_create(str, nbyte, b->bucket_alloc);
            APR_BRIGADE_INSERT_TAIL(b, e);
            return flush(b, ctx);
        }
        else {
            e = apr_bucket_heap_create(str, nbyte, NULL, b->bucket_alloc);
            APR_BRIGADE_INSERT_TAIL(b, e);
            return APR_SUCCESS;
        }
    }
    else if (!buf) {
        /* we don't have a buffer, but the data is small enough
         * that we don't mind making a new buffer */
        buf = apr_bucket_alloc(APR_BUCKET_BUFF_SIZE, b->bucket_alloc);
        e = apr_bucket_heap_create(buf, APR_BUCKET_BUFF_SIZE,
                                   apr_bucket_free, b->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(b, e);
        e->length = 0;   /* We are writing into the brigade, and
                          * allocating more memory than we need.  This
                          * ensures that the bucket thinks it is empty just
                          * after we create it.  We'll fix the length
                          * once we put data in it below.
                          */
    }

    /* there is a sufficiently big buffer bucket available now */
    memcpy(buf, str, nbyte);
    e->length += nbyte;

    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_writev(apr_bucket_brigade *b,
                                             apr_brigade_flush flush,
                                             void *ctx,
                                             const struct iovec *vec,
                                             apr_size_t nvec)
{
    apr_bucket *e;
    apr_size_t total_len;
    apr_size_t i;
    char *buf;

    /* Compute the total length of the data to be written.
     */
    total_len = 0;
    for (i = 0; i < nvec; i++) {
       total_len += vec[i].iov_len;
    }

    /* If the data to be written is very large, try to convert
     * the iovec to transient buckets rather than copying.
     */
    if (total_len > APR_BUCKET_BUFF_SIZE) {
        if (flush) {
            for (i = 0; i < nvec; i++) {
                e = apr_bucket_transient_create(vec[i].iov_base,
                                                vec[i].iov_len,
                                                b->bucket_alloc);
                APR_BRIGADE_INSERT_TAIL(b, e);
            }
            return flush(b, ctx);
        }
        else {
            for (i = 0; i < nvec; i++) {
                e = apr_bucket_heap_create((const char *) vec[i].iov_base,
                                           vec[i].iov_len, NULL,
                                           b->bucket_alloc);
                APR_BRIGADE_INSERT_TAIL(b, e);
            }
            return APR_SUCCESS;
        }
    }

    i = 0;

    /* If there is a heap bucket at the end of the brigade
     * already, copy into the existing bucket.
     */
    e = APR_BRIGADE_LAST(b);
    if (!APR_BRIGADE_EMPTY(b) && APR_BUCKET_IS_HEAP(e)) {
        apr_bucket_heap *h = e->data;
        apr_size_t remaining = h->alloc_len -
            (e->length + (apr_size_t)e->start);
        buf = h->base + e->start + e->length;

        if (remaining >= total_len) {
            /* Simple case: all the data will fit in the
             * existing heap bucket
             */
            for (; i < nvec; i++) {
                apr_size_t len = vec[i].iov_len;
                memcpy(buf, (const void *) vec[i].iov_base, len);
                buf += len;
            }
            e->length += total_len;
            return APR_SUCCESS;
        }
        else {
            /* More complicated case: not all of the data
             * will fit in the existing heap bucket.  The
             * total data size is <= APR_BUCKET_BUFF_SIZE,
             * so we'll need only one additional bucket.
             */
            const char *start_buf = buf;
            for (; i < nvec; i++) {
                apr_size_t len = vec[i].iov_len;
                if (len > remaining) {
                    break;
                }
                memcpy(buf, (const void *) vec[i].iov_base, len);
                buf += len;
                remaining -= len;
            }
            e->length += (buf - start_buf);
            total_len -= (buf - start_buf);

            if (flush) {
                apr_status_t rv = flush(b, ctx);
                if (rv != APR_SUCCESS) {
                    return rv;
                }
            }

            /* Now fall through into the case below to
             * allocate another heap bucket and copy the
             * rest of the array.  (Note that i is not
             * reset to zero here; it holds the index
             * of the first vector element to be
             * written to the new bucket.)
             */
        }
    }

    /* Allocate a new heap bucket, and copy the data into it.
     * The checks above ensure that the amount of data to be
     * written here is no larger than APR_BUCKET_BUFF_SIZE.
     */
    buf = apr_bucket_alloc(APR_BUCKET_BUFF_SIZE, b->bucket_alloc);
    e = apr_bucket_heap_create(buf, APR_BUCKET_BUFF_SIZE,
                               apr_bucket_free, b->bucket_alloc);
    for (; i < nvec; i++) {
        apr_size_t len = vec[i].iov_len;
        memcpy(buf, (const void *) vec[i].iov_base, len);
        buf += len;
    }
    e->length = total_len;
    APR_BRIGADE_INSERT_TAIL(b, e);

    return APR_SUCCESS;
}

APU_DECLARE(apr_status_t) apr_brigade_puts(apr_bucket_brigade *bb,
                                           apr_brigade_flush flush, void *ctx,
                                           const char *str)
{
    apr_size_t len = strlen(str);
    apr_bucket *bkt = APR_BRIGADE_LAST(bb);
    if (!APR_BRIGADE_EMPTY(bb) && APR_BUCKET_IS_HEAP(bkt)) {
        /* If there is enough space available in a heap bucket
         * at the end of the brigade, copy the string directly
         * into the heap bucket
         */
        apr_bucket_heap *h = bkt->data;
        apr_size_t bytes_avail = h->alloc_len - bkt->length;

        if (bytes_avail >= len) {
            char *buf = h->base + bkt->start + bkt->length;
            memcpy(buf, str, len);
            bkt->length += len;
            return APR_SUCCESS;
        }
    }

    /* If the string could not be copied into an existing heap
     * bucket, delegate the work to apr_brigade_write(), which
     * knows how to grow the brigade
     */
    return apr_brigade_write(bb, flush, ctx, str, len);
}

APU_DECLARE_NONSTD(apr_status_t) apr_brigade_putstrs(apr_bucket_brigade *b, 
                                                     apr_brigade_flush flush,
                                                     void *ctx, ...)
{
    va_list va;
    apr_status_t rv;

    va_start(va, ctx);
    rv = apr_brigade_vputstrs(b, flush, ctx, va);
    va_end(va);
    return rv;
}

APU_DECLARE_NONSTD(apr_status_t) apr_brigade_printf(apr_bucket_brigade *b, 
                                                    apr_brigade_flush flush,
                                                    void *ctx, 
                                                    const char *fmt, ...)
{
    va_list ap;
    apr_status_t rv;

    va_start(ap, fmt);
    rv = apr_brigade_vprintf(b, flush, ctx, fmt, ap);
    va_end(ap);
    return rv;
}

struct brigade_vprintf_data_t {
    apr_vformatter_buff_t vbuff;

    apr_bucket_brigade *b;  /* associated brigade */
    apr_brigade_flush *flusher; /* flushing function */
    void *ctx;

    char *cbuff; /* buffer to flush from */
};

static apr_status_t brigade_flush(apr_vformatter_buff_t *buff)
{
    /* callback function passed to ap_vformatter to be
     * called when vformatter needs to buff and
     * buff.curpos > buff.endpos
     */

    /* "downcast," have really passed a brigade_vprintf_data_t* */
    struct brigade_vprintf_data_t *vd = (struct brigade_vprintf_data_t*)buff;
    apr_status_t res = APR_SUCCESS;

    res = apr_brigade_write(vd->b, *vd->flusher, vd->ctx, vd->cbuff,
                          APR_BUCKET_BUFF_SIZE);

    if(res != APR_SUCCESS) {
      return -1;
    }

    vd->vbuff.curpos = vd->cbuff;
    vd->vbuff.endpos = vd->cbuff + APR_BUCKET_BUFF_SIZE;

    return res;
}

APU_DECLARE(apr_status_t) apr_brigade_vprintf(apr_bucket_brigade *b,
                                              apr_brigade_flush flush,
                                              void *ctx,
                                              const char *fmt, va_list va)
{
    /* the cast, in order of appearance */
    struct brigade_vprintf_data_t vd;
    char buf[APR_BUCKET_BUFF_SIZE];
    apr_size_t written;

    vd.vbuff.curpos = buf;
    vd.vbuff.endpos = buf + APR_BUCKET_BUFF_SIZE;
    vd.b = b;
    vd.flusher = &flush;
    vd.ctx = ctx;
    vd.cbuff = buf;

    written = apr_vformatter(brigade_flush, &vd.vbuff, fmt, va);

    if (written == -1) {
      return -1;
    }

    /* write out what remains in the buffer */
    return apr_brigade_write(b, flush, ctx, buf, vd.vbuff.curpos - buf);
}

