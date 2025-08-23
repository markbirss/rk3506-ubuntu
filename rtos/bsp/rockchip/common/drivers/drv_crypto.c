/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_crypto.c
  * @author  Troy Lin
  * @version V0.1
  * @date    1-Mar-2024
  * @brief   CRYPTO Driver
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup CRYPTO
 *  @{
 */

/** @defgroup CRYPTO_How_To_Use How To Use
 *  @{

  See more information, click [here](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/crypto/crypto)

 @} */

#include <rtconfig.h>

#if defined(RT_USING_HWCRYPTO)
#include <rtdevice.h>
#include <rtthread.h>
#include <rthw.h>

#include <board.h>
#include "hal_bsp.h"
#include "hal_driver.h"

/* Private typedef --------------------------------------------------------------*/
#define RK_CRYPTO_DEBUG         0

#include <rtdbg.h>

#define BITS2BYTES(nbits)       ((nbits) / 8)
#define CRYPTO_NAME             "rk_crypto"
#define RNG_NAME                "rk_trng"
#define RSA_MAX_BITS             (4096)
#define ROCKCHIP_CRYPTO_TIMEOUT (1 * RT_TICK_PER_SECOND)

struct rockchip_crypto
{
    struct CRYPTO_DEV       crypto_dev;
    struct rt_mutex         crypto_mtx;
    struct rt_completion    crypto_done;
    int                     crypto_irqNum;
    rt_bool_t               busy;

#if defined(RT_HWCRYPTO_USING_RNG)
    const struct HAL_TRNG_DEV  *trng_dev;
    struct rt_mutex         rng_mtx;
#endif
};

struct rk_aligned_buf
{
    uint8_t     *origin_data;
    uint8_t     *aligned_data;
    uint32_t    data_len;
    rt_bool_t   is_aligned;
};

typedef struct
{
    uint8_t     last_block[128];
    uint8_t     *mid_data;
    uint32_t    mid_data_size;
    uint32_t    calced_len;
    uint32_t    last_block_len;
    uint32_t    error;
    uint32_t    priv_data_size;
    void        *priv_data; /**< crypto dev priv_data */

    struct CRYPTO_ALGO_CONFIG algo_config;
} S_SHA_CONTEXT;

/* Private functions ------------------------------------------------------------*/
static rt_err_t rk_hwcrypto_create(struct rt_hwcrypto_ctx *ctx);
static void rk_hwcrypto_destroy(struct rt_hwcrypto_ctx *ctx);
static rt_err_t rk_hwcrypto_clone(struct rt_hwcrypto_ctx *des, const struct rt_hwcrypto_ctx *src);
static void rk_hwcrypto_reset(struct rt_hwcrypto_ctx *ctx);

#if RK_CRYPTO_DEBUG
#define rk_crypto_dbg(fmt, args...) \
    rt_kprintf("%s D [%s %d]: " fmt, CRYPTO_NAME, __func__, __LINE__, ##args)

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

static void rt_rockchip_dump_hex(const char *name, const rt_uint8_t *ptr, rt_size_t buflen)
{
    int i, j;
    unsigned char *buf = (unsigned char *)ptr;

    rt_kprintf("%s(%p) len: 0x%X: \n", name, ptr, buflen);

    for (i = 0; i < buflen; i += 16)
    {
        rt_kprintf("%08X: ", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%02X ", buf[i + j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        rt_kprintf("\n");
    }
}
#else
#define rk_crypto_dbg(fmt, ...) \
do { \
} while(0)

static void rt_rockchip_dump_hex(const char *name, const rt_uint8_t *ptr, rt_size_t buflen)
{
}
#endif

/* Private variables ------------------------------------------------------------*/
static struct rockchip_crypto *g_crypto;

static const struct rt_hwcrypto_ops rk_hwcrypto_ops =
{
    .create     = rk_hwcrypto_create,
    .destroy    = rk_hwcrypto_destroy,
    .copy       = rk_hwcrypto_clone,
    .reset      = rk_hwcrypto_reset,
};

static rt_err_t hal_ret2rt_ret(HAL_Status ret)
{
    struct value_map
    {
        HAL_Status hal;
        rt_err_t   rt;
    }
    val_map [] =
    {
        {HAL_OK,        RT_EOK},
        {HAL_ERROR,     -RT_ERROR},
        {HAL_BUSY,      -RT_EBUSY},
        {HAL_NODEV,     -RT_ENOSYS},
        {HAL_INVAL,     -RT_EINVAL},
        {HAL_NOSYS,     -RT_ENOSYS},
        {HAL_TIMEOUT,   -RT_ETIMEOUT},
    };

    for (size_t i = 0; i < HAL_ARRAY_SIZE(val_map); i++)
    {
        if (ret == val_map[i].hal)
            return val_map[i].rt;
    }

    return RT_ERROR;
}

static rt_bool_t rk_is_force_align(const void *addr)
{
#ifdef XIP_MAP0_BASE0
    if (((uint32_t)addr & 0xFF000000) == XIP_MAP0_BASE0)
        return RT_TRUE;
#endif

#ifdef XIP_MAP1_BASE0
    if (((uint32_t)addr & 0xFF000000) == XIP_MAP1_BASE0)
        return RT_TRUE;
#endif

#ifdef XIP_MAP1_BASE1
    if (((uint32_t)addr & 0xFF000000) == XIP_MAP1_BASE1)
        return RT_TRUE;
#endif

    return RT_FALSE;
}

static struct rk_aligned_buf *rk_aligned_buf_get(const uint8_t *data, uint32_t data_len, uint32_t alignment)
{
    uint8_t *tmp_buf = RT_NULL;
    struct rk_aligned_buf *aligned_buf = RT_NULL;

    RT_ASSERT(data);
    RT_ASSERT(data_len);
    RT_ASSERT(alignment);

    aligned_buf = rt_malloc(sizeof(*aligned_buf));
    if (!aligned_buf)
    {
        LOG_E("fun[%s] memory allocate %d bytes failed!", __FUNCTION__, data_len);
        return RT_NULL;
    }

    rt_memset(aligned_buf, 0x00, sizeof(*aligned_buf));

    //Checking in/out data buffer address not alignment
    if ((uint32_t)data % alignment || rk_is_force_align(data))
    {
        tmp_buf = rt_malloc_align(data_len, alignment);
        if (tmp_buf == RT_NULL)
        {
            LOG_E("fun[%s] memory allocate %d bytes failed!", __FUNCTION__, data_len);
            return RT_NULL;
        }

        rt_memcpy(tmp_buf, data, data_len);
    }

    aligned_buf->origin_data = (uint8_t *)data;
    aligned_buf->data_len = data_len;
    if (tmp_buf)
    {
        aligned_buf->aligned_data = tmp_buf;
        aligned_buf->is_aligned = RT_TRUE;
    }
    else
    {
        aligned_buf->aligned_data = aligned_buf->origin_data;
        aligned_buf->is_aligned = RT_FALSE;
    }

    return aligned_buf;
}

static void rk_aligned_buf_put(struct rk_aligned_buf *aligned_buf)
{
    if (!aligned_buf)
        return;

    if (aligned_buf->is_aligned)
    {
        rt_memset(aligned_buf->aligned_data, 0x00, aligned_buf->data_len);
        rt_free_align(aligned_buf->aligned_data);
        aligned_buf->aligned_data = RT_NULL;
    }

    rt_free(aligned_buf);
}

inline struct rockchip_crypto *rk_get_crypto(struct rt_hwcrypto_ctx *hw_ctx)
{
    return (struct rockchip_crypto *)hw_ctx->device->user_data;
}

/* Crypto engine operation ------------------------------------------------------------*/
static void rockchip_crypto_irq(int irq, void *param)
{
    struct rockchip_crypto *crypto = g_crypto;

    /* enter interrupt */
    rt_interrupt_enter();

    HAL_CRYPTO_ClearISR(&crypto->crypto_dev);

    rt_completion_done(&crypto->crypto_done);

    /* leave interrupt */
    rt_interrupt_leave();
}

static rt_err_t rk_crypto_init(struct CRYPTO_DEV *crypto_dev)
{
    HAL_Status ret;

    ret = HAL_CRYPTO_Init(crypto_dev);

    crypto_dev->privData = rt_malloc_align(crypto_dev->privDataSize, crypto_dev->privAlign);
    if (crypto_dev->privData == RT_NULL)
    {
        LOG_E("fun[%s] memory allocate %d bytes failed!", __FUNCTION__, crypto_dev->privAlign);
        return -RT_ENOMEM;
    }

    rt_memset(crypto_dev->privData, 0x00, crypto_dev->privDataSize);

    return hal_ret2rt_ret(ret);
}

static rt_err_t rk_symm_crypt_run(struct rockchip_crypto *crypto, rt_bool_t is_encrypt,
                                  uint32_t algo, uint32_t mode, uint8_t *key, uint32_t key_size,
                                  uint8_t *iv, uint8_t *data_in, uint8_t *data_out, uint32_t data_len)
{
    HAL_Status hal_ret = HAL_OK;
    struct CRYPTO_ALGO_CONFIG algo_config;
    struct CRYPTO_DMA_CONFIG dma_config;
    struct CRYPTO_DEV *crypto_dev = &crypto->crypto_dev;

    memset(&algo_config, 0x00, sizeof(algo_config));

    algo_config.algo = algo;
    algo_config.mode = mode;

    switch (algo)
    {
    case CRYPTO_ALGO_AES:
    {
        struct CRYPTO_INFO_AES *aes = &algo_config.info.aes;

        memcpy(aes->key1, key, key_size);
        aes->keyLen = key_size;

        if (iv)
        {
            memcpy(aes->iv, (void *)iv, RK_AES_BLOCK_SIZE);
            aes->ivLen = RK_AES_BLOCK_SIZE;
        }

        aes->isDecrypt = !is_encrypt;
    }
    break;
    case CRYPTO_ALGO_DES:
    case CRYPTO_ALGO_TDES:
    {
        struct CRYPTO_INFO_DES *des = &algo_config.info.des;

        memcpy(des->key, key, key_size);
        des->keyLen = key_size;

        if (iv)
        {
            memcpy(des->iv, (void *)iv, RK_DES_BLOCK_SIZE);
            des->ivLen = RK_DES_BLOCK_SIZE;
        }

        des->isDecrypt = !is_encrypt;
    }
    break;
    default:
        return -RT_EINVAL;
    }

    rt_mutex_take(&crypto->crypto_mtx, RT_WAITING_FOREVER);

    hal_ret = HAL_CRYPTO_AlgoInit(crypto_dev, &algo_config);
    if (hal_ret !=  HAL_OK)
    {
        rk_crypto_dbg("HAL_CRYPTO_AlgoInit failed ret = %08x\n", hal_ret);
        goto exit;
    }

    memset(&dma_config, 0x00, sizeof(dma_config));
    dma_config.srcAddr = (uint32_t)data_in;
    dma_config.srcLen  = data_len;
    dma_config.dstAddr = (uint32_t)data_out;
    dma_config.dstLen  = data_len;
    dma_config.isLast  = RT_TRUE;

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, data_in, data_len);
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, data_out, data_len);

    hal_ret = HAL_CRYPTO_DMAConfig(crypto_dev, &dma_config);
    if (hal_ret !=  HAL_OK)
        goto exit;

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, crypto_dev->privData, crypto_dev->privDataSize);

    hal_ret = HAL_CRYPTO_DMAStart(crypto_dev);
    if (hal_ret !=  HAL_OK)
    {
        rk_crypto_dbg("HAL_CRYPTO_DMAStart failed ret = %08x\n", hal_ret);
        goto exit;
    }

    if (rt_completion_wait(&crypto->crypto_done, ROCKCHIP_CRYPTO_TIMEOUT) != RT_EOK)
    {
        rk_crypto_dbg("wait crypto done timeout!\n");
        hal_ret = HAL_TIMEOUT;
        goto exit;
    }

    if (!HAL_CRYPTO_CheckIntStatus(crypto_dev))
    {
        rk_crypto_dbg("check interrupt status error!\n");
        hal_ret = HAL_ERROR;
        goto exit;
    }

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, data_out, data_len);

exit:
    HAL_CRYPTO_AlgoDeInit(crypto_dev);

    rt_mutex_release(&crypto->crypto_mtx);

    return hal_ret2rt_ret(hal_ret);
}

static rt_err_t rk_symm_crypt(struct hwcrypto_symmetric *symm_ctx, struct hwcrypto_symmetric_info *sym_info)
{
    uint8_t iv_temp[16];
    bool is_enc = RT_FALSE;
    rt_err_t ret = -RT_EINVAL;
    uint32_t key_size, alignment;
    uint32_t cipher_algo, cipher_mode;
    struct rk_aligned_buf *rk_in = RT_NULL;
    struct rk_aligned_buf *rk_out = RT_NULL;
    struct rockchip_crypto *crypto = RT_NULL;

    RT_ASSERT(symm_ctx != RT_NULL);
    RT_ASSERT(sym_info != RT_NULL);

    if ((sym_info->length % 4) != 0)
    {
        return -RT_EINVAL;
    }

    crypto = rk_get_crypto(&symm_ctx->parent);
    alignment = crypto->crypto_dev.dataAlign;

    key_size = BITS2BYTES(symm_ctx->key_bitlen);

    switch (symm_ctx->parent.type & (HWCRYPTO_MAIN_TYPE_MASK))
    {
    case HWCRYPTO_TYPE_AES:
        cipher_algo = CRYPTO_ALGO_AES;
        if (key_size != RK_AES_KEYSIZE_128 &&
                key_size != RK_AES_KEYSIZE_192 &&
                key_size != RK_AES_KEYSIZE_256)
            return -RT_EINVAL;
        break;
    case HWCRYPTO_TYPE_DES:
        cipher_algo = CRYPTO_ALGO_DES;
        if (key_size != RK_DES_KEYSIZE)
            return -RT_EINVAL;
        break;
    case HWCRYPTO_TYPE_3DES:
        cipher_algo = CRYPTO_ALGO_TDES;
        if (key_size != RK_DES_KEYSIZE &&
                key_size != RK_DES_KEYSIZE * 2 &&
                key_size != RK_DES_KEYSIZE * 3)
            return -RT_EINVAL;
        break;
    default :
        return -RT_ERROR;
    }

    //Select AES operation mode
    switch (symm_ctx->parent.type & (HWCRYPTO_MAIN_TYPE_MASK | HWCRYPTO_SUB_TYPE_MASK))
    {
    case HWCRYPTO_TYPE_AES_ECB:
        cipher_mode = CRYPTO_MODE_CIPHER_ECB;
        break;
    case HWCRYPTO_TYPE_AES_CBC:
        cipher_mode = CRYPTO_MODE_CIPHER_CBC;
        break;
    case HWCRYPTO_TYPE_AES_CFB:
        cipher_mode = CRYPTO_MODE_CIPHER_CFB;
        break;
    case HWCRYPTO_TYPE_AES_OFB:
        cipher_mode = CRYPTO_MODE_CIPHER_OFB;
        break;
    case HWCRYPTO_TYPE_AES_CTR:
        cipher_mode = CRYPTO_MODE_CIPHER_CTR;
        break;
    default :
        return -RT_ERROR;
    }

    rk_in = rk_aligned_buf_get(sym_info->in, sym_info->length, alignment);
    if (!rk_in)
    {
        ret = -RT_ENOMEM;
        goto exit;
    }

    rk_out = rk_aligned_buf_get(sym_info->out, sym_info->length, alignment);
    if (!rk_out)
    {
        ret = -RT_ENOMEM;
        goto exit;
    }

    if ((cipher_mode == CRYPTO_MODE_CIPHER_CBC) && (sym_info->mode == HWCRYPTO_MODE_DECRYPT))
    {
        uint32_t loop;

        loop = (sym_info->length - 1) / 16;
        rt_memcpy(iv_temp, rk_in->aligned_data + (loop * 16), 16);
    }

    is_enc = sym_info->mode == HWCRYPTO_MODE_ENCRYPT ? RT_TRUE : RT_FALSE;

    ret = rk_symm_crypt_run(crypto, is_enc, cipher_algo, cipher_mode,
                            symm_ctx->key, key_size, symm_ctx->iv,
                            rk_in->aligned_data, rk_out->aligned_data,
                            sym_info->length);
    if (ret)
        goto exit;

    if (cipher_mode == CRYPTO_MODE_CIPHER_CBC)
    {
        if (sym_info->mode == HWCRYPTO_MODE_DECRYPT)
        {
            rt_memcpy(symm_ctx->iv, iv_temp, 16);
        }
        else
        {
            uint32_t loop;

            loop = (sym_info->length - 1) / 16;
            rt_memcpy(symm_ctx->iv, rk_out->aligned_data + (loop * 16), 16);
        }
    }

    if (rk_out->aligned_data != rk_out->origin_data)
        rt_memcpy(rk_out->origin_data, rk_out->aligned_data, rk_out->data_len);

exit:
    rk_aligned_buf_put(rk_in);
    rk_aligned_buf_put(rk_out);
    return ret;
}

static rt_err_t rk_hash_init(struct hwcrypto_hash *hash_ctx)
{
    uint32_t mode;
    HAL_Status hal_ret = HAL_OK;
    S_SHA_CONTEXT *sha_contex = RT_NULL;
    struct CRYPTO_ALGO_CONFIG *algo_config = RT_NULL;;

    RT_ASSERT(hash_ctx);

    sha_contex = hash_ctx->parent.contex;
    algo_config = &sha_contex->algo_config;

    //Select SHA operation mode
    switch (hash_ctx->parent.type & (HWCRYPTO_MAIN_TYPE_MASK | HWCRYPTO_SUB_TYPE_MASK))
    {
    case HWCRYPTO_TYPE_MD5:
        mode = CRYPTO_MODE_HASH_MD5;
        break;
    case HWCRYPTO_TYPE_SHA1:
        mode = CRYPTO_MODE_HASH_SHA1;
        break;
    case HWCRYPTO_TYPE_SHA224:
        mode = CRYPTO_MODE_HASH_SHA224;
        break;
    case HWCRYPTO_TYPE_SHA256:
        mode = CRYPTO_MODE_HASH_SHA256;
        break;
    case HWCRYPTO_TYPE_SHA384:
        mode = CRYPTO_MODE_HASH_SHA384;
        break;
    case HWCRYPTO_TYPE_SHA512:
        mode = CRYPTO_MODE_HASH_SHA512;
        break;
    default :
        return -RT_EINVAL;
    }

    algo_config->algo = CRYPTO_ALGO_HASH;
    algo_config->mode = mode;

    return hal_ret2rt_ret(hal_ret);
}

static rt_err_t rk_hash_update_run(struct rockchip_crypto *crypto, S_SHA_CONTEXT *sha_contex,
                                   const rt_uint8_t *in, rt_size_t length, rt_bool_t is_last)
{
    HAL_Status hal_ret = HAL_OK;
    struct CRYPTO_DMA_CONFIG dma_config;
    struct CRYPTO_DEV *crypto_dev = &crypto->crypto_dev;
    struct CRYPTO_ALGO_CONFIG *algo_config = &sha_contex->algo_config;

    memset(&dma_config, 0x00, sizeof(dma_config));
    dma_config.srcAddr = (uint32_t)in;
    dma_config.srcLen  = length;
    dma_config.isLast  = is_last;

    sha_contex->error = HAL_ERROR;

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (rt_uint8_t *)in, length);

    rt_mutex_take(&crypto->crypto_mtx, RT_WAITING_FOREVER);

    if (sha_contex->calced_len)
    {
        dma_config.forceRestart = 1;
        HAL_CRYPTO_RestoreMidData(crypto_dev, CRYPTO_ALGO_HASH,
                                  sha_contex->mid_data, sha_contex->mid_data_size);
    }

    hal_ret = HAL_CRYPTO_AlgoInit(crypto_dev, algo_config);
    if (hal_ret !=  HAL_OK)
    {
        rk_crypto_dbg("HAL_CRYPTO_AlgoInit failed ret = %08x\n", hal_ret);
        goto exit;
    }

    hal_ret = HAL_CRYPTO_DMAConfig(crypto_dev, &dma_config);
    if (hal_ret !=  HAL_OK)
        goto exit;

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, crypto_dev->privData, crypto_dev->privDataSize);

    hal_ret = HAL_CRYPTO_DMAStart(crypto_dev);
    if (hal_ret !=  HAL_OK)
    {
        rk_crypto_dbg("HAL_CRYPTO_DMAStart failed ret = %08x\n", hal_ret);
        goto exit;
    }

    if (rt_completion_wait(&crypto->crypto_done, ROCKCHIP_CRYPTO_TIMEOUT) != RT_EOK)
    {
        rk_crypto_dbg("wait crypto done timeout!\n");
        hal_ret = HAL_TIMEOUT;
        goto exit;
    }

    if (!HAL_CRYPTO_CheckIntStatus(crypto_dev))
    {
        rk_crypto_dbg("check interrupt status error!\n");
        hal_ret = HAL_ERROR;
        goto exit;
    }

    if (!dma_config.isLast)
        HAL_CRYPTO_StoreMidData(crypto_dev, CRYPTO_ALGO_HASH,
                                sha_contex->mid_data, sha_contex->mid_data_size);

    /* save privData */
    memcpy(sha_contex->priv_data, crypto_dev->privData, crypto_dev->privDataSize);

    sha_contex->calced_len += length;
    sha_contex->error = HAL_OK;

exit:
    if (hal_ret !=  HAL_OK)
    {
        HAL_CRYPTO_AlgoDeInit(crypto_dev);
    }

    rt_mutex_release(&crypto->crypto_mtx);

    return hal_ret2rt_ret(hal_ret);
}


static rt_err_t rk_hash_update(struct hwcrypto_hash *hash_ctx, const rt_uint8_t *in, rt_size_t length)
{
    rt_err_t ret = RT_EOK;
    struct rk_aligned_buf *rk_in = RT_NULL;
    struct rockchip_crypto *crypto = RT_NULL;
    uint32_t alignment, new_last_len, new_data_len;
    S_SHA_CONTEXT *sha_contex = RT_NULL;

    RT_ASSERT(hash_ctx);
    RT_ASSERT(in);
    RT_ASSERT(length);

    crypto = rk_get_crypto(&hash_ctx->parent);
    alignment = crypto->crypto_dev.dataAlign;
    sha_contex = hash_ctx->parent.contex;

    RT_ASSERT(sha_contex);

    rk_crypto_dbg("hash_ctx = %p, sha_contex = %p, in = %p, length = %lu\n", hash_ctx, sha_contex, in, length);

    if (sha_contex->last_block_len + length <= sizeof(sha_contex->last_block))
    {
        rt_memcpy(sha_contex->last_block + sha_contex->last_block_len, in, length);
        sha_contex->last_block_len += length;
        return RT_EOK;
    }

    rk_in = rk_aligned_buf_get(in, length, alignment);
    if (!rk_in)
    {
        return -RT_ENOMEM;
    }

    new_last_len = length % sizeof(sha_contex->last_block);
    if (new_last_len == 0)
        new_last_len = sizeof(sha_contex->last_block);

    new_data_len = rk_in->data_len - new_last_len;

    if (sha_contex->last_block_len)
    {
        ret = rk_hash_update_run(crypto, sha_contex, sha_contex->last_block, sha_contex->last_block_len, RT_FALSE);

        sha_contex->last_block_len = 0;
    }

    if (new_data_len)
        ret |= rk_hash_update_run(crypto, sha_contex, rk_in->aligned_data, new_data_len, RT_FALSE);

    if (ret == RT_EOK)
    {
        rt_memcpy(sha_contex->last_block, rk_in->aligned_data + new_data_len, new_last_len);
        sha_contex->last_block_len = new_last_len;
    }

    rk_aligned_buf_put(rk_in);

    return ret;
}

static rt_err_t rk_hash_finish(struct hwcrypto_hash *hash_ctx, rt_uint8_t *out, rt_size_t length)
{
    rt_err_t ret;
    uint8_t tmp_hash[64];
    uint32_t tmp_hash_len = 0;
    HAL_Status hal_ret = HAL_OK;
    S_SHA_CONTEXT *sha_contex = RT_NULL;
    struct CRYPTO_DEV *crypto_dev = RT_NULL;
    struct rockchip_crypto *crypto = RT_NULL;

    RT_ASSERT(hash_ctx);
    RT_ASSERT(out);
    RT_ASSERT(length);

    crypto = rk_get_crypto(&hash_ctx->parent);
    sha_contex = hash_ctx->parent.contex;
    crypto_dev = &crypto->crypto_dev;

    RT_ASSERT(crypto);
    RT_ASSERT(sha_contex);

    rk_crypto_dbg("xxxxx = %08x, out = %p, len = %lu\n", sha_contex->error, out, length);

    /* get hash result */
    if (sha_contex->error)
        return hal_ret2rt_ret(sha_contex->error);

    if (sha_contex->calced_len == 0 && sha_contex->last_block_len == 0)
        return -RT_EINVAL;

    if (sha_contex->last_block_len)
    {
        ret = rk_hash_update_run(crypto, sha_contex, sha_contex->last_block, sha_contex->last_block_len, RT_TRUE);
        if (ret)
            return ret;
    }

    /* restore privData */
    memcpy(crypto_dev->privData, sha_contex->priv_data, crypto_dev->privDataSize);

    while (!HAL_CRYPTO_CheckHashValid(crypto_dev));

    memset(tmp_hash, 0x00, sizeof(tmp_hash));

    hal_ret = HAL_CRYPTO_ReadHashReg(crypto_dev, tmp_hash, &tmp_hash_len);
    if (hal_ret != HAL_OK)
    {
        rk_crypto_dbg("HAL_CRYPTO_ReadHashReg failed ret = %08x\n", hal_ret);
        goto exit;
    }

    if (length > tmp_hash_len)
    {
        rk_crypto_dbg("hash out length(%zu) > tmp_hash_len(%u)\n", length, tmp_hash_len);
        hal_ret = HAL_INVAL;
        goto exit;
    }

    memcpy(out, tmp_hash, length);

exit:
    return hal_ret2rt_ret(hal_ret);
}

static void rk_hash_destroy(struct rt_hwcrypto_ctx *ctx)
{
    struct rockchip_crypto *crypto = rk_get_crypto(ctx);

    rt_mutex_take(&crypto->crypto_mtx, RT_WAITING_FOREVER);
    if (crypto->busy)
        crypto->busy = RT_FALSE;

    if (ctx->contex)
    {
        S_SHA_CONTEXT *sha_ctx = ctx->contex;

        if (sha_ctx->mid_data)
            rt_free_align(sha_ctx->mid_data);

        if (sha_ctx->priv_data)
            rt_free_align(sha_ctx->priv_data);

        rt_free_align(ctx->contex);
        ctx->contex = RT_NULL;
    }

    rt_mutex_release(&crypto->crypto_mtx);
}

static rt_err_t rk_hash_create(struct rt_hwcrypto_ctx *ctx)
{
    rt_err_t res;
    S_SHA_CONTEXT *sha_ctx;
    struct rockchip_crypto *crypto = rk_get_crypto(ctx);
    uint32_t alignment = crypto->crypto_dev.dataAlign;

    rt_mutex_take(&crypto->crypto_mtx, RT_WAITING_FOREVER);
    if (crypto->busy)
    {
        res = -RT_EBUSY;
        goto exit;
    }

    ctx->contex = rt_malloc_align(sizeof(S_SHA_CONTEXT), alignment);
    if (ctx->contex == RT_NULL)
    {
        res = -RT_ENOMEM;
        goto exit;
    }

    rt_memset(ctx->contex, 0, sizeof(S_SHA_CONTEXT));

    sha_ctx = ctx->contex;

    sha_ctx->priv_data_size = crypto->crypto_dev.privDataSize;
    sha_ctx->priv_data = rt_malloc_align(sha_ctx->priv_data_size, alignment);
    if (sha_ctx->priv_data == RT_NULL)
    {
        res = -RT_ENOMEM;
        goto exit;
    }

    rt_memset(sha_ctx->priv_data, 0, sha_ctx->priv_data_size);

    sha_ctx->mid_data_size = crypto->crypto_dev.hashMidDataSize;
    if (sha_ctx->mid_data_size)
    {
        sha_ctx->mid_data = rt_malloc_align(sha_ctx->mid_data_size, alignment);
        if (sha_ctx->mid_data == RT_NULL)
        {
            res = -RT_ENOMEM;
            goto exit;
        }

        rt_memset(sha_ctx->mid_data, 0, sha_ctx->mid_data_size);
    }
    else
    {
        crypto->busy = RT_TRUE;
    }

    res = rk_hash_init((struct hwcrypto_hash *)ctx);

exit:
    if (res)
        rk_hash_destroy(ctx);

    rt_mutex_release(&crypto->crypto_mtx);

    return res;
}

static void rk_hash_reset(struct rt_hwcrypto_ctx *ctx)
{
    if (ctx->contex)
    {
        S_SHA_CONTEXT sha_ctx_bak;
        S_SHA_CONTEXT *sha_ctx = ctx->contex;

        rt_memcpy(&sha_ctx_bak, sha_ctx, sizeof(*sha_ctx));

        rt_memset(sha_ctx, 0x00, sizeof(*sha_ctx));
        sha_ctx->mid_data = sha_ctx_bak.mid_data;
        sha_ctx->mid_data_size = sha_ctx_bak.mid_data_size;
        sha_ctx->priv_data = sha_ctx_bak.priv_data;
        sha_ctx->priv_data_size = sha_ctx_bak.priv_data_size;

        if (sha_ctx->mid_data && sha_ctx->mid_data_size)
            rt_memset(sha_ctx->mid_data, 0x00, sha_ctx->mid_data_size);

        if (sha_ctx->priv_data && sha_ctx->priv_data_size)
            rt_memset(sha_ctx->priv_data, 0x00, sha_ctx->priv_data_size);
    }
}

static const struct hwcrypto_symmetric_ops rk_symm_ops =
{
    .crypt = rk_symm_crypt,
};

static const struct hwcrypto_hash_ops rk_hash_ops =
{
    .update = rk_hash_update,
    .finish = rk_hash_finish,
};

#if defined(RT_HWCRYPTO_USING_BIGNUM)
/* BIGNUM operation ------------------------------------------------------------*/
static rt_err_t rk_bignum_alloc(struct CRYPTO_BIGNUM *bignum, int nwords)
{
    rt_memset(bignum, 0x00, sizeof(*bignum));

    bignum->data = rt_malloc(nwords * 4);
    if (!bignum->data)
    {
        rk_crypto_dbg("malloc %d Byte failed.\n", nwords * 4);
        return -RT_ENOMEM;
    }

    rt_memset(bignum->data, 0x00, nwords * 4);

    bignum->nWords = nwords;

    return RT_EOK;
}

static rt_err_t rk_bignum_alloc_import(struct CRYPTO_BIGNUM *bignum, const struct hw_bignum_mpi *mpi)
{
    rt_err_t ret;
    int nbytes, nwords;

    rt_memset(bignum, 0x00, sizeof(*bignum));

    nbytes = rt_hwcrypto_bignum_get_len(mpi);
    nwords = (nbytes + 3) / 4;

    ret = rk_bignum_alloc(bignum, nwords);
    if (ret)
        return ret;

    rt_memcpy((uint8_t *)bignum->data, mpi->p, nbytes);

    return RT_EOK;
}

static void rk_bignum_free(struct CRYPTO_BIGNUM *bignum)
{
    if (bignum)
    {
        if (bignum->data)
            rt_free(bignum->data);

        rt_memset(bignum, 0x00, sizeof(*bignum));
    }
}

static rt_err_t rk_bignum_exptmod(struct hwcrypto_bignum *bignum_ctx,
                                  struct hw_bignum_mpi *x,
                                  const struct hw_bignum_mpi *a,
                                  const struct hw_bignum_mpi *b,
                                  const struct hw_bignum_mpi *c)
{
    int nbytes, nwords;
    HAL_Status hal_ret = HAL_ERROR;
    struct CRYPTO_BIGNUM b_x, b_a, b_n, b_e, b_tmp;
    struct CRYPTO_DEV *crypto_dev = RT_NULL;
    struct rockchip_crypto *crypto = RT_NULL;

    RT_ASSERT(bignum_ctx);
    RT_ASSERT(x);
    RT_ASSERT(a);
    RT_ASSERT(b);
    RT_ASSERT(c);

    crypto = rk_get_crypto(&bignum_ctx->parent);
    crypto_dev = &crypto->crypto_dev;

    RT_ASSERT(crypto);

    nbytes = rt_hwcrypto_bignum_get_len(c);
    nwords = (nbytes + 3) / 4;

    if (nbytes > BITS2BYTES(RSA_MAX_BITS))
    {
        rk_crypto_dbg("nbytes(%d) > %d error!\n", nbytes, BITS2BYTES(RSA_MAX_BITS));
        return -RT_EINVAL;
    }

    if (rt_hwcrypto_bignum_get_len(a) != nbytes)
    {
        rk_crypto_dbg("bignum a (%d) != c(%d) error!\n", nbytes, rt_hwcrypto_bignum_get_len(a));
        return -RT_EINVAL;
    }

    x->p = rt_malloc(nbytes);

    rk_bignum_alloc(&b_x, nwords);
    rk_bignum_alloc(&b_tmp, nwords);

    rk_bignum_alloc_import(&b_a, a);
    rk_bignum_alloc_import(&b_e, b);
    rk_bignum_alloc_import(&b_n, c);

    rt_mutex_take(&crypto->crypto_mtx, RT_WAITING_FOREVER);

    hal_ret = HAL_CRYPTO_ExptMod(crypto_dev, &b_a, &b_e, &b_n, &b_x, &b_tmp);

    rt_mutex_release(&crypto->crypto_mtx);

    if (hal_ret == HAL_OK)
    {
        rt_memcpy(x->p, (uint8_t *)b_x.data, b_x.nWords * 4);
        x->total = b_x.nWords * 4;
    }

    rk_bignum_free(&b_x);
    rk_bignum_free(&b_a);
    rk_bignum_free(&b_n);
    rk_bignum_free(&b_e);
    rk_bignum_free(&b_tmp);

    return hal_ret2rt_ret(hal_ret);
}

static struct hwcrypto_bignum_ops rk_bignum_ops =
{
    .exptmod = rk_bignum_exptmod,
};
#endif

#if defined(RT_HWCRYPTO_USING_RNG)
/* RNG operation ------------------------------------------------------------*/
static rt_err_t rk_trng_init(const struct HAL_TRNG_DEV *trng_dev)
{
    HAL_Status ret;

    ret = HAL_TRNG_Init(trng_dev);

    return hal_ret2rt_ret(ret);
}

static rt_uint32_t rk_trng_rand(struct hwcrypto_rng *ctx)
{
    HAL_Status res;
    rt_uint32_t rand_val;
    struct rockchip_crypto *crypto;

    RT_ASSERT(ctx);

    crypto = rk_get_crypto(&ctx->parent);

    RT_ASSERT(crypto);

    rt_mutex_take(&crypto->rng_mtx, RT_WAITING_FOREVER);

    res = HAL_TRNG_Get(crypto->trng_dev, (uint8_t *)&rand_val, sizeof(rand_val));

    rt_mutex_release(&crypto->rng_mtx);

    return res == HAL_OK ? rand_val : 0;
}

static struct hwcrypto_rng_ops rk_rng_ops =
{
    .update = rk_trng_rand,
};
#endif

/* Register crypto interface ----------------------------------------------------------*/
static rt_err_t rk_hwcrypto_create(struct rt_hwcrypto_ctx *ctx)
{
    rt_err_t res = RT_EOK;
    RT_ASSERT(ctx != RT_NULL);

    rk_crypto_dbg("enter\n");

    switch (ctx->type & HWCRYPTO_MAIN_TYPE_MASK)
    {
#if defined(RT_HWCRYPTO_USING_RNG)
    case HWCRYPTO_TYPE_RNG:
    {
        ctx->contex = RT_NULL;

        //Setup RNG operation
        ((struct hwcrypto_rng *)ctx)->ops = &rk_rng_ops;
        break;
    }
#endif /* RT_HWCRYPTO_USING_RNG */

    case HWCRYPTO_TYPE_AES:
    case HWCRYPTO_TYPE_DES:
    case HWCRYPTO_TYPE_3DES:
    {
        ctx->contex = RT_NULL;
        //Setup symm operation
        ((struct hwcrypto_symmetric *)ctx)->ops = &rk_symm_ops;
        break;
    }

    case HWCRYPTO_TYPE_MD5:
    case HWCRYPTO_TYPE_SHA1:
    case HWCRYPTO_TYPE_SHA2:
    {
        res = rk_hash_create(ctx);
        //Setup operation
        if (res == RT_EOK)
            ((struct hwcrypto_hash *)ctx)->ops = &rk_hash_ops;
        break;
    }

#if defined(RT_HWCRYPTO_USING_BIGNUM)
    case HWCRYPTO_TYPE_BIGNUM:
    {
        ctx->contex = RT_NULL;
        //Setup bignum operation
        ((struct hwcrypto_bignum *)ctx)->ops = &rk_bignum_ops;
        break;
    }
#endif /* BSP_USING_CRYPTO */

    default:
        res = -RT_ERROR;
        break;
    }

    return res;
}

static void rk_hwcrypto_destroy(struct rt_hwcrypto_ctx *ctx)
{
    RT_ASSERT(ctx != RT_NULL);

    rk_crypto_dbg("enter, contex = %p\n", ctx->contex);

    switch (ctx->type & HWCRYPTO_MAIN_TYPE_MASK)
    {
    case HWCRYPTO_TYPE_MD5:
    case HWCRYPTO_TYPE_SHA1:
    case HWCRYPTO_TYPE_SHA2:
    {
        rk_hash_destroy(ctx);
        break;
    }
    default:
        break;
    }

    ctx->contex = RT_NULL;
}

static rt_err_t rk_hwcrypto_clone(struct rt_hwcrypto_ctx *des, const struct rt_hwcrypto_ctx *src)
{
    RT_ASSERT(des != RT_NULL);
    RT_ASSERT(src != RT_NULL);

    rt_rockchip_dump_hex("source contex", (const rt_uint8_t *)src, sizeof(struct rt_hwcrypto_ctx));

    return -RT_ENOSYS;
}

static void rk_hwcrypto_reset(struct rt_hwcrypto_ctx *ctx)
{
    rk_crypto_dbg("enter\n");

    switch (ctx->type & HWCRYPTO_MAIN_TYPE_MASK)
    {
    case HWCRYPTO_TYPE_MD5:
    case HWCRYPTO_TYPE_SHA1:
    case HWCRYPTO_TYPE_SHA2:
        rk_hash_reset(ctx);
        break;

    default:
        break;
    }
}

/* Init and register rk_hwcrypto_dev */
int rk_hwcrypto_device_init(void)
{
    rt_err_t result = RT_EOK;
    static struct rt_hwcrypto_device rk_hwcrypto_dev;
    struct rockchip_crypto *crypto = RT_NULL;

    rk_hwcrypto_dev.ops = &rk_hwcrypto_ops;
    rk_hwcrypto_dev.id = 0;

    crypto = rt_malloc(sizeof(struct rockchip_crypto));
    if (!crypto)
        return RT_ENOMEM;

    rt_memset(crypto, 0x00, sizeof(*crypto));

    rk_hwcrypto_dev.user_data = crypto;

    g_crypto = crypto;

    result = rk_crypto_init(&crypto->crypto_dev);
    RT_ASSERT(result == RT_EOK);

    /* init crypto mutex */
    result = rt_mutex_init(&crypto->crypto_mtx, CRYPTO_NAME, RT_IPC_FLAG_PRIO);
    RT_ASSERT(result == RT_EOK);

    rt_completion_init(&crypto->crypto_done);

    /* register irq */
    crypto->crypto_irqNum = CRYPTO_IRQn;

    rt_hw_interrupt_install(crypto->crypto_irqNum, rockchip_crypto_irq, crypto, CRYPTO_NAME);
    rt_hw_interrupt_umask(crypto->crypto_irqNum);

#if defined(RT_HWCRYPTO_USING_RNG)
    /* init crypto mutex */
    result = rt_mutex_init(&crypto->rng_mtx, RNG_NAME, RT_IPC_FLAG_PRIO);
    RT_ASSERT(result == RT_EOK);

    crypto->trng_dev = &g_trngnsDev;

    result = rk_trng_init(crypto->trng_dev);
    if (result == RT_EOK)
    {
        LOG_I("TRNG is used as default RNG.");
    }
#endif

    /* register hwcrypto operation */
    result = rt_hwcrypto_register(&rk_hwcrypto_dev, RT_HWCRYPTO_DEFAULT_NAME);
    RT_ASSERT(result == RT_EOK);

    return 0;
}
INIT_DEVICE_EXPORT(rk_hwcrypto_device_init);

#endif //#if defined(RT_USING_HWCRYPTO))

/** @} */  // CRYPTO

/** @} */  // RKBSP_Driver_Reference

