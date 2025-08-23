/**
  * Copyright (c) 2020-2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_spinand.c
  * @version V2.0
  * @brief   spi nand interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-06-16     Dingqiang Lin   the first version
  * 2023-11-06     Dingqiang Lin   Support Dhara
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup SPINAND
 *  @{
 */

/** @defgroup SPINAND_How_To_Use How To Use
 *  @{

    SPI_NAND is a framework protocol layer based on SPI Nand flash. It needs to be combined
 with the corresponding driver layer to complete the transmission of the protocol.

 @} */

#include <rtthread.h>

#ifdef RT_USING_SPINAND
#include <rthw.h>
#include <rtdevice.h>
#include <drivers/mtd_nand.h>
#include <dfs_fs.h>

#include "board.h"
#include "hal_bsp.h"
#include "dma.h"
#include "drv_clock.h"
#include "drv_fspi.h"
#include "hal_base.h"
#include "mini_ftl.h"
#include "drv_flash_partition.h"

#ifdef RT_USING_DHARA
#include "map.h"
#endif


/********************* Private MACRO Definition ******************************/
/** @defgroup SPINAND_Private_Macro Private Macro
 *  @{
 */
// #define SPINAND_DEBUG
#ifdef SPINAND_DEBUG
#define spinand_dbg(...)     rt_kprintf(__VA_ARGS__)
#else
#define spinand_dbg(...)
#endif

// #define DHARA_DEBUG
#ifdef DHARA_DEBUG
#define dhara_dbg(...)       rt_kprintf(__VA_ARGS__)
#else
#define dhara_dbg(...)
#endif

#define MTD_TO_SPINAND(mtd) ((struct SPI_NAND *)(mtd->priv))

/** @} */  // SPINAND_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup SPINAND_Private_Structure Private Structure
 *  @{
 */

#define DHARA_DEFAULT_OFFSET (0x2000000) /* 32MB */
#define RK_SPINAND_RESERVED_BBT_BLOCKS 4 /* 4 blks for bad block table extention */

static struct rt_mutex spinand_lock;

/** @} */  // SPINAND_Private_Structure

/********************* Public Function Definition ****************************/
/** @defgroup SPINAND_Public_Function Public Function
 *  @{
 */
rt_err_t spinand_mtd_read(rt_mtd_nand_t mtd, rt_off_t page,
                          rt_uint8_t *data, rt_uint32_t data_len,
                          rt_uint8_t *spare, rt_uint32_t spare_len)
{
    struct SPI_NAND *spinand = MTD_TO_SPINAND(mtd);
    int ret;

    if (spare || spare_len)
    {
        rt_kprintf("%s oob is not supported\n", __func__);
        return -RT_EINVAL;
    }

    if (!data || data_len != mtd->page_size)
    {
        rt_kprintf("%s data param input invalid, %p %d\n", __func__, data, data_len);
        return -RT_EINVAL;
    }

    spinand_dbg("%s addr= %lx len= %x\n", __func__, (uint32_t)page, data_len);
    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_ReadPageRaw(spinand, page, data, false);
    rt_mutex_release(&spinand_lock);
    if (ret < 0)
    {
        rt_kprintf("%s addr %lx EIO, ret=%d\n", __func__, page, ret);
        ret = -RT_EIO;
    }
    else if (ret == SPINAND_ECC_ERROR)
    {
        rt_kprintf("%s addr %lx ecc failed, ret=%d\n", __func__, page, ret);
        ret = -RT_ERROR;
    }
    else if (ret == SPINAND_ECC_REFRESH)
    {
        spinand_dbg("%s addr %lx ecc reach flipping bits, ret=%d\n", __func__, page, ret);
        ret = RT_EOK;
    }
    else
    {
        ret = RT_EOK;
    }

    return ret;
}

rt_err_t spinand_mtd_write(rt_mtd_nand_t mtd, rt_off_t page,
                           const rt_uint8_t *data, rt_uint32_t data_len,
                           const rt_uint8_t *spare, rt_uint32_t spare_len)
{
    struct SPI_NAND *spinand = MTD_TO_SPINAND(mtd);
    int ret;

    if (spare && spare_len)
    {
        rt_kprintf("%s oob is not supported\n", __func__);
        return -RT_EINVAL;
    }

    if (!data || data_len != mtd->page_size)
    {
        rt_kprintf("%s data param input invalid, %p %d\n", __func__, data, data_len);
        return -RT_EINVAL;
    }

    spinand_dbg("%s addr= %lx len= %x\n", __func__, (uint32_t)page, data_len);
    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_ProgPageRaw(spinand, page, (uint32_t *)data, false);
    rt_mutex_release(&spinand_lock);
    if (ret)
    {
        rt_kprintf("%s addr %lx EIO, ret=%d\n", __func__, page, ret);
        ret = -RT_EIO;
    }

    return ret;
}

rt_err_t spinand_mtd_erase(rt_mtd_nand_t mtd, rt_uint32_t block)
{
    struct SPI_NAND *spinand = MTD_TO_SPINAND(mtd);
    int ret;

    spinand_dbg("%s addr= %lx\n", __func__, block);
    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_EraseBlock(spinand, block * mtd->pages_per_block);
    rt_mutex_release(&spinand_lock);
    if (ret)
    {
        rt_kprintf("%s addr %lx EIO, ret=%d\n", __func__, block, ret);
        ret = -RT_EIO;
    }

    return ret;
}

rt_err_t spinand_mtd_block_isbad(rt_mtd_nand_t mtd, rt_uint32_t block)
{
    struct SPI_NAND *spinand = MTD_TO_SPINAND(mtd);
    int32_t ret = RT_EOK;

    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_IsBad(spinand, block * spinand->pagePerBlk);
    rt_mutex_release(&spinand_lock);

    spinand_dbg("%s blk= %lx %d\n", __func__, block, ret);

    return ret;
}

rt_err_t spinand_mtd_block_markbad(rt_mtd_nand_t mtd, rt_uint32_t block)
{
    struct SPI_NAND *spinand = MTD_TO_SPINAND(mtd);
    int32_t ret = RT_EOK;

    spinand_dbg("%s blk= %lx\n", __func__, block);
    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_MarkBad(spinand, block * spinand->pagePerBlk);
    rt_mutex_release(&spinand_lock);
    if (ret)
    {
        rt_kprintf("%s addr %lx EIO, ret=%d\n", __func__, block, ret);
        ret = -RT_EIO;
    }

    return ret;
}

static const struct rt_mtd_nand_driver_ops spinand_mtd_nand_ops =
{
    NULL,
    spinand_mtd_read,
    spinand_mtd_write,
    NULL,
    spinand_mtd_erase,
    spinand_mtd_block_isbad,
    spinand_mtd_block_markbad,
};
/** @} */  // SPINAND_Public_Function

/********************* Private Function Definition ****************************/
/** @defgroup SPINAND_Private_Function Private Function
 *  @{
 */
#if defined(RT_USING_SPINAND_FSPI_HOST)
static HAL_Status fspi_xfer(struct SPI_NAND_HOST *spi, struct HAL_SPI_MEM_OP *op)
{
    struct rt_fspi_device *fspi_device = (struct rt_fspi_device *)spi->userdata;

    return rt_fspi_xfer(fspi_device, op);
}

static int rockchip_sfc_delay_lines_tuning(struct SPI_NAND *spinand, struct rt_fspi_device *fspi_device)
{
    uint8_t id_temp[SPINAND_MAX_ID_LEN];
    uint16_t cell_max = (uint16_t)rt_fspi_get_max_dll_cells(fspi_device);
    uint16_t right, left = 0;
    uint16_t step = HAL_FSPI_DLL_TRANING_STEP;
    bool dll_valid = false;
    uint32_t final;

    HAL_SPINAND_Init(spinand);
    for (right = 0; right <= cell_max; right += step)
    {
        int ret;

        ret = rt_fspi_set_delay_lines(fspi_device, right);
        if (ret)
        {
            dll_valid = false;
            break;
        }
        ret = HAL_SPINAND_ReadID(spinand, id_temp);
        if (ret)
        {
            dll_valid = false;
            break;
        }

        spinand_dbg("dll read flash id:%x %x %x\n",
                    id_temp[0], id_temp[1], id_temp[2]);

        ret = HAL_SPINAND_IsFlashSupported(id_temp);
        if (dll_valid && !ret)
        {
            right -= step;

            break;
        }
        if (!dll_valid && ret)
            left = right;

        if (ret)
            dll_valid = true;

        /* Add cell_max to loop */
        if (right == cell_max)
            break;
        if (right + step > cell_max)
            right = cell_max - step;
    }

    if (dll_valid && (right - left) >= HAL_FSPI_DLL_TRANING_VALID_WINDOW)
    {
        if (left == 0 && right < cell_max)
            final = left + (right - left) * 2 / 5;
        else
            final = left + (right - left) / 2;
    }
    else
    {
        final = 0;
    }

    if (final)
    {
        spinand_dbg("spinand %d %d %d dll training success in %dMHz max_cells=%u\n",
                    left, right, final, spinand->spi->speed, cell_max);
        return rt_fspi_set_delay_lines(fspi_device, final);
    }
    else
    {
        rt_kprintf("spinand %d %d dll training failed in %dMHz, reduce the frequency\n",
                   left, right, spinand->spi->speed);
        rt_fspi_dll_disable(fspi_device);
        return -1;
    }
}

RT_WEAK struct rt_fspi_device g_fspi_spinand =
{
#ifdef RT_USING_SPINAND_FSPI_CS1
    .host_id = 0,
    .dev_type = DEV_SPINAND,
    .chip_select = 1,
#else
#ifdef CONFIG_RT_USING_SNOR
    .host_id = 1,
#else
    .host_id = 0,
#endif
    .dev_type = DEV_SPINAND,
    .chip_select = 0,
#endif
};

static uint32_t spinand_adapt(struct SPI_NAND *spinand)
{
    struct rt_fspi_device *fspi_device = &g_fspi_spinand;
    uint32_t ret;
    int dll_result = 0;

    spinand_dbg("spinand_adapt in\n");
    ret = rt_hw_fspi_device_register(fspi_device);
    if (ret)
    {
        return ret;
    }

    /* Designated host to SPI_NAND */
    if (RT_SPINAND_SPEED > 0 && RT_SPINAND_SPEED <= SPINAND_SPEED_MAX)
    {
        spinand->spi->speed = RT_SPINAND_SPEED;
    }
    else
    {
        spinand->spi->speed = SPINAND_SPEED_DEFAULT;
    }
    spinand->spi->speed = rt_fspi_set_speed(fspi_device, spinand->spi->speed);

#ifdef RT_USING_SPINAND_FSPI_HOST_CS1_GPIO
    if (!fspi_device->cs_gpio.gpio)
    {
        rt_kprintf("it's needed to redefine g_fspi_spinand with cs_gpio in iomux.c!\n");
        return -RT_ERROR;
    }
#endif

    spinand->spi->userdata = (void *)fspi_device;
    spinand->spi->mode = HAL_SPI_MODE_3 | HAL_SPI_RX_QUAD;
    spinand->spi->xfer = fspi_xfer;
    spinand_dbg("%s fspi initial\n", __func__);
    rt_fspi_controller_init(fspi_device);

    if (spinand->spi->speed > HAL_FSPI_SPEED_THRESHOLD)
    {
        dll_result = rockchip_sfc_delay_lines_tuning(spinand, fspi_device);
    }
    else
    {
        rt_fspi_dll_disable(fspi_device);
    }

    /* Init SPI_NAND abstract */
    spinand_dbg("%s spinand initial\n", __func__);
    ret = HAL_SPINAND_Init(spinand);
    if (ret)
    {
        uint8_t idByte[5];

        HAL_SPINAND_ReadID(spinand, idByte);
        rt_kprintf("SPI Nand ID: %x %x %x\n", idByte[0], idByte[1], idByte[2]);
    }

    if (dll_result)
    {
        rt_fspi_set_speed(fspi_device, HAL_FSPI_SPEED_THRESHOLD);
        rt_kprintf("%s dll turning failed %d\n", __func__, dll_result);
    }

    return ret;
}
#elif defined(RT_USING_SPINAND_SFC_HOST)
static uint32_t spinand_adapt(struct SPI_NAND *spinand)
{
    struct HAL_SFC_HOST *host = (struct HAL_SFC_HOST *)rt_calloc(1, sizeof(*host));

    RT_ASSERT(host);

    /* Designated host to SPINAND */
    host->instance = SFC;
    HAL_SFC_Init(host);
    spinand->spi->userdata = (void *)host;
    spinand->spi->mode = HAL_SPI_MODE_3;
    spinand->spi->mode |= (HAL_SPI_TX_QUAD | HAL_SPI_RX_QUAD);
    spinand->spi->xfer = HAL_SFC_SPINandSpiXfer;

    /* Init SPI Nand abstract */
    if (HAL_SPINAND_Init(spinand))
    {
        rt_free(host);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}
#elif defined(RT_USING_SPINAND_SPI_HOST)
static HAL_Status spi_xfer(struct SPI_NAND_HOST *spi, struct HAL_SPI_MEM_OP *op)
{
    struct rt_spi_device *spi_device = (struct rt_spi_device *)spi->userdata;
    struct rt_spi_configuration cfg;
    uint32_t pos = 0;
    const uint8_t *tx_buf = NULL;
    uint8_t *rx_buf = NULL;
    uint8_t op_buf[HAL_SPI_OP_LEN_MAX];
    int32_t op_len;
    int32_t i, ret;

    if (op->data.nbytes)
    {
        if (op->data.dir == HAL_SPI_MEM_DATA_IN)
            rx_buf = op->data.buf.in;
        else
            tx_buf = op->data.buf.out;
    }

//    rt_kprintf("%s %x %lx\n", __func__, op->cmd.opcode, op->data.nbytes);
    op_len = sizeof(op->cmd.opcode) + op->addr.nbytes + op->dummy.nbytes;
    op_buf[pos++] = op->cmd.opcode;

    if (op->addr.nbytes)
    {
        for (i = 0; i < op->addr.nbytes; i++)
            op_buf[pos + i] = op->addr.val >> (8 * (op->addr.nbytes - i - 1));
        pos += op->addr.nbytes;
    }

    if (op->dummy.nbytes)
        memset(&op_buf[pos], 0xff, op->dummy.nbytes);

    cfg.data_width = 8;
    cfg.mode = spi->mode | RT_SPI_MSB;
    cfg.max_hz = spi->speed;
    rt_spi_configure(spi_device, &cfg);

    if (tx_buf)
    {
        ret = rt_spi_send_then_send(spi_device, op_buf, op_len, tx_buf, op->data.nbytes);
        if (ret)
            ret = HAL_ERROR;
    }
    else if (rx_buf)
    {
        ret = rt_spi_send_then_recv(spi_device, op_buf, op_len, rx_buf, op->data.nbytes);
        if (ret)
            ret = HAL_ERROR;
    }
    else
    {
        ret = rt_spi_send(spi_device, op_buf, op_len);
        if (ret != op_len)
            ret = HAL_ERROR;
        else
            ret = HAL_OK;
    }

//    rt_kprintf("%s finished %d\n", __func__, ret);

    return ret;
}

static uint32_t spinand_adapt(struct SPI_NAND *spinand)
{
    struct rt_spi_device *spi_device = NULL;

#if defined(RT_SPINAND_SPI_DEVICE_NAME)
    spi_device = (struct rt_spi_device *)rt_device_find(RT_SPINAND_SPI_DEVICE_NAME);
#endif
    if (!spi_device)
    {
        rt_kprintf("%s can not find %s\n", __func__, RT_SPINAND_SPI_DEVICE_NAME);

        return RT_EINVAL;
    }

    /* Designated host to SPI Nand */
    spinand->spi->userdata = (void *)spi_device;
    spinand->spi->mode = HAL_SPI_MODE_3;
    spinand->spi->xfer = spi_xfer;
    if (RT_SPINAND_SPEED > 0 && RT_SPINAND_SPEED <= HAL_SPI_MASTER_MAX_SCLK_OUT)
    {
        spinand->spi->speed = RT_SPINAND_SPEED;
    }
    else
    {
        spinand->spi->speed = HAL_SPI_MASTER_MAX_SCLK_OUT;
    }

    /* Init SPI Nand abstract */
    return HAL_SPINAND_Init(spinand);
}
#else
static uint32_t spinand_adapt(struct SPI_NAND *spinandF)
{
    return RT_EINVAL;
}
#endif

#ifdef RT_USING_DHARA
int dhara_nand_is_bad(const struct dhara_nand *n, dhara_block_t bno)
{
    struct SPI_NAND *spinand = (struct SPI_NAND *)n->priv_data;
    int ret;

    if (bno >= n->num_blocks)
    {
        rt_kprintf("NAND_is_bad called on invalid block: %ld\n", bno);
    }

    if (n->blocks[bno].bbm == NAND_BBT_BLOCK_STATUS_UNKNOWN)
    {
        rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
        ret = HAL_SPINAND_IsBad(spinand, (bno + n->start_blocks) * spinand->pagePerBlk);
        rt_mutex_release(&spinand_lock);
        dhara_dbg("NAND_is_bad blk=0x%x ret=%d\n", bno + n->start_blocks, ret);
        if (ret)
        {
            dhara_dbg("NAND_is_bad blk 0x%x is bad block, ret=%d\n", bno, ret);
            n->blocks[bno].bbm = NAND_BBT_BLOCK_WORN;
        }
        else
        {
            n->blocks[bno].bbm = NAND_BBT_BLOCK_GOOD;
        }
    }

    return n->blocks[bno].bbm == NAND_BBT_BLOCK_GOOD ? false : true;
}

void dhara_nand_mark_bad(const struct dhara_nand *n, dhara_block_t bno)
{
    struct SPI_NAND *spinand = (struct SPI_NAND *)n->priv_data;
    int ret;

    if (bno >= n->num_blocks)
    {
        rt_kprintf("NAND_mark_bad called on invalid block: %ld\n", bno);
    }

    n->blocks[bno].bbm = NAND_BBT_BLOCK_WORN;

    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_MarkBad(spinand, bno + n->start_blocks);
    rt_mutex_release(&spinand_lock);
    dhara_dbg("NAND_mark_bad blk=0x%x, ret=%d\n", bno + n->start_blocks, ret);
    if (ret)
    {
        rt_kprintf("NAND_mark_bad blk 0x%x failed, ret=%d\n", bno, ret);
    }
}

int dhara_nand_erase(const struct dhara_nand *n, dhara_block_t bno, dhara_error_t *err)
{
    struct SPI_NAND *spinand = (struct SPI_NAND *)n->priv_data;
    int ret;

    if (bno >= n->num_blocks)
    {
        rt_kprintf("NAND_erase called on invalid block: %ld\n", bno);
        return -RT_EINVAL;
    }

    if (n->blocks[bno].bbm == NAND_BBT_BLOCK_WORN)
    {
        rt_kprintf("NAND_erase called on block which is marked bad: %ld\n", bno);
        return -RT_EINVAL;
    }

    n->blocks[bno].next_page = 0;

    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_EraseBlock(spinand, (bno + n->start_blocks) << n->log2_ppb);
    rt_mutex_release(&spinand_lock);
    dhara_dbg("NAND_erase blk=0x%x, ret=%d\n", bno + n->start_blocks, ret);
    if (ret)
    {
        rt_kprintf("NAND_erase blk 0x%x failed, ret=%d\n", bno, ret);
    }
    dhara_trace_e_count++;

    return ret;
}

int dhara_nand_prog(const struct dhara_nand *n, dhara_page_t p,
                    const uint8_t *data, dhara_error_t *err)
{
    struct SPI_NAND *spinand = (struct SPI_NAND *)n->priv_data;
    const int bno = p >> n->log2_ppb;
    const uint16_t pno = p & ((n->page_per_block) - 1);
    uint32_t meta[SPINAND_META_WORDS_MAX] = { DHARA_NAND_META_MAGIC, 0, 0, 0 };
    int ret;

    if ((bno < 0) || (bno >= n->num_blocks))
    {
        rt_kprintf("NAND_prog called on invalid block: %ld\n", bno);
        return -RT_EINVAL;
    }

    if (n->blocks[bno].bbm == NAND_BBT_BLOCK_WORN)
    {
        rt_kprintf("NAND_prog called on block which is marked bad: %d\n", bno);
        return -RT_EINVAL;
    }

    if (pno < n->blocks[bno].next_page)
    {
        rt_kprintf("NAND_prog out-of-order page programming. Block %d, page %d (expected %d)\n",
                   bno, pno, n->blocks[bno].next_page);
        return -RT_EINVAL;
    }

    n->blocks[bno].next_page = pno + 1;

    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    ret = HAL_SPINAND_ProgPage(spinand, p + n->start_blocks * n->page_per_block, data, meta);
    rt_mutex_release(&spinand_lock);
    dhara_dbg("NAND_prog page=0x%x, ret=%d\n", p + n->start_blocks * n->page_per_block, ret);
    if (ret != HAL_OK)
    {
        rt_kprintf("NAND_prog page 0x%x failed, ret=%d\n", p, ret);
        dhara_set_error(err, DHARA_E_BAD_BLOCK);
    }
    dhara_trace_p_count++;

    return ret;
}

int dhara_nand_is_free(const struct dhara_nand *n, dhara_page_t p)
{
    struct SPI_NAND *spinand = (struct SPI_NAND *)n->priv_data;
    const int bno = p >> n->log2_ppb;
    const uint16_t pno = p & ((n->page_per_block) - 1);
    int ret, i;
    dhara_page_t read_page;

    if ((bno < 0) || (bno >= n->num_blocks))
    {
        rt_kprintf("NAND_is_free called on invalid block: %d\n", bno);
        return -RT_EINVAL;
    }

    if (n->blocks[bno].next_page == 0)
    {
        read_page = p + n->start_blocks * n->page_per_block;
        rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
        ret = HAL_SPINAND_ReadPageAnyWhere(spinand, read_page, n->copy_buf, 0, n->page_size);
        rt_mutex_release(&spinand_lock);
        if (ret < 0)
        {
            rt_kprintf("%s addr %lx EIO, ret=%d\n", __func__, read_page, ret);
            return false;
        }
        else if (ret == SPINAND_ECC_ERROR)
        {
            rt_kprintf("%s addr %lx ecc failed, ret=%d\n", __func__, read_page, ret);
            return false;
        }
        else if (ret == SPINAND_ECC_REFRESH)
        {
            dhara_dbg("%s addr %lx ecc reach flipping bits, ret=%d\n", __func__, read_page, ret);
        }

        dhara_dbg("NAND_is_free page=0x%x, ret=%d\n", read_page, ret);
        for (i = 0; i < n->page_size / 4; i++)
        {
            if (((uint32_t *)n->copy_buf)[i] != 0xFFFFFFFF)
            {
                return false;
            }
        }
        return true;
    }

    return (int)(n->blocks[bno].next_page <= pno);
}

int dhara_nand_read(const struct dhara_nand *n, dhara_page_t p,
                    size_t offset, size_t length,
                    uint8_t *data, dhara_error_t *err)
{
    struct SPI_NAND *spinand = (struct SPI_NAND *)n->priv_data;
    const int bno = p >> n->log2_ppb;
    int ret;
    dhara_page_t read_page;

    if ((bno < 0) || (bno >= n->num_blocks))
    {
        rt_kprintf("NAND_read called on invalid block: %d\n", bno);
        return -RT_EINVAL;
    }

    if (offset + length > n->page_size)
    {
        rt_kprintf("NAND_read called on invalid range: offset = %ld, length = %ld\n", offset, length);
        return -RT_EINVAL;
    }

    rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
    read_page = p + n->start_blocks * n->page_per_block;
    ret = HAL_SPINAND_ReadPageAnyWhere(spinand, read_page, data, offset, length);
    rt_mutex_release(&spinand_lock);
    dhara_dbg("NAND_read page=0x%x offset=0x%x length=%x\n", read_page, offset, length);
    if (ret < 0)
    {
        rt_kprintf("%s addr %lx EIO, ret=%d\n", __func__, read_page, ret);
        return false;
    }
    else if (ret == SPINAND_ECC_ERROR)
    {
        rt_kprintf("%s addr %lx ecc failed, ret=%d\n", __func__, read_page, ret);
        return false;
    }
    else if (ret == SPINAND_ECC_REFRESH)
    {
        dhara_dbg("%s addr %lx ecc reach flipping bits, ret=%d\n", __func__, read_page, ret);
    }
    if (ret < 0 || ret == SPINAND_ECC_ERROR)
    {
        rt_kprintf("NAND_read page 0x%x failed, ret=%d\n", p, ret);
        dhara_set_error(err, DHARA_E_ECC);
        ret = -RT_ERROR;
    }
    else if (ret == SPINAND_ECC_REFRESH)
    {
        dhara_set_error(err, DHARA_E_NONE);
        ret = RT_EOK;
    }
    else
    {
        dhara_set_error(err, DHARA_E_NONE);
        ret = RT_EOK;
    }
    dhara_trace_r_count++;

    return ret;
}

int dhara_nand_copy(const struct dhara_nand *n,
                    dhara_page_t src, dhara_page_t dst,
                    dhara_error_t *err)
{
    if ((dhara_nand_read(n, src, 0, n->page_size, n->copy_buf, err) < 0) ||
            (dhara_nand_prog(n, dst, n->copy_buf, err) < 0))
        return -1;

    return 0;
}

/********************* Public Structure Definition **************************/
/** @defgroup SPINAND_Public_Structure Public Structure
 *  @{
 */
static struct rt_mutex dhara_blk_lock;

static rt_err_t dhara_blk_control(rt_device_t dev, int cmd, void *args)
{
    struct rt_flash_partition *blk_part = DEV_2_PART(dev);
    struct dhara_device *dhara_dev = (struct dhara_device *)dev->user_data;
    struct dhara_map *map = &dhara_dev->map;
    struct SPI_NAND *spinand = (struct SPI_NAND *)dhara_dev->nand.priv_data;
    uint32_t blk;
    int32_t ret;

    dhara_dbg("%s %ld\n", __func__, blk_part->size);

    RT_ASSERT(dev != RT_NULL);
    switch (cmd)
    {
    case RT_DEVICE_CTRL_BLK_GETGEOME:
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL)
            return -RT_ERROR;
        geometry->bytes_per_sector  = dhara_dev->sector_size;
        geometry->sector_count      = blk_part->size / geometry->bytes_per_sector;
        geometry->block_size        = geometry->bytes_per_sector;
        dhara_dbg("%s sector=%x count=%x\n", __func__, geometry->bytes_per_sector, geometry->sector_count);
        break;
    }
    case RT_DEVICE_CTRL_BLK_SYNC:
    {
        dhara_dbg("%s flush\n", __func__);
        return dhara_map_sync(map, NULL);
        break;
    }
    case RT_DEVICE_CTRL_MTD_FORMAT:
        rt_mutex_take(&spinand_lock, RT_WAITING_FOREVER);
        for (blk = 0; blk < dhara_dev->nand.num_blocks; blk++)
        {
            if (HAL_SPINAND_IsBad(spinand, (blk + dhara_dev->nand.start_blocks) * dhara_dev->nand.page_per_block))
            {
                continue;
            }
            ret = HAL_SPINAND_EraseBlock(spinand, (blk + dhara_dev->nand.start_blocks) * dhara_dev->nand.page_per_block);
            if (ret)
            {
                rt_mutex_release(&spinand_lock);
                return -RT_ERROR;
            }
        }
        rt_mutex_release(&spinand_lock);

        rt_kprintf("dhara format\n");
        dhara_map_init(map, &dhara_dev->nand, map->journal.page_buf, GC_RATIO);
        dhara_dbg("dhara resume\n");
        dhara_map_resume(map, NULL);
        dhara_dbg("dhara sync\n");

        ret = dhara_map_sync(map, NULL);
        if (ret)
        {
            rt_kprintf("dhara sync failed, ret=%d\n", ret);
            return -RT_ERROR;
        }
        dhara_dev->capacity = dhara_map_capacity(map);
        blk_part->size = dhara_dev->capacity * dhara_dev->sector_size;
    default:
        break;
    }

    return RT_EOK;
}

static rt_size_t dhara_blk_read(rt_device_t dev, rt_off_t sec, void *buffer, rt_size_t nsec)
{
    struct rt_flash_partition *blk_part = DEV_2_PART(dev);
    struct dhara_device *dhara_dev = (struct dhara_device *)dev->user_data;
    rt_size_t   read_count = 0;
    rt_uint8_t *ptr = (rt_uint8_t *)buffer;
    rt_size_t ret;
    dhara_error_t err;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(nsec != 0);

    dhara_dbg("%s sec = %08x,nsec = %08x %lx %lx\n", __func__, sec, nsec, blk_part->offset, blk_part->size);
    if (!(blk_part->mask_flags & PART_FLAG_RDONLY))
    {
        rt_kprintf("ERROR: partition %s is unreadable, mask_flags = %04x\n", blk_part->name, blk_part->mask_flags);
        return 0;
    }

    while (read_count < nsec)
    {
        if (((sec + 1) * dhara_dev->sector_size) > (blk_part->offset + blk_part->size))
        {
            rt_kprintf("ERROR: read overrun!\n");
            return read_count;
        }

        /* It'a BLOCK device */
        rt_mutex_take(&dhara_blk_lock, RT_WAITING_FOREVER);
        ret = dhara_map_read(&dhara_dev->map, sec, ptr, &err);
        rt_mutex_release(&dhara_blk_lock);
        if (ret)
            return read_count;
        sec++;
        ptr += dhara_dev->sector_size;
        read_count++;
    }

    return nsec;
}

static rt_size_t dhara_blk_write(rt_device_t dev, rt_off_t sec, const void *buffer, rt_size_t nsec)
{
    struct rt_flash_partition *blk_part = DEV_2_PART(dev);
    struct dhara_device *dhara_dev = (struct dhara_device *)dev->user_data;
    rt_size_t   write_count = 0;
    rt_uint8_t *ptr = (rt_uint8_t *)buffer;
    rt_size_t ret;
    dhara_error_t err;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(nsec != 0);

    dhara_dbg("%s sec = %08x,nsec = %08x %lx %lx\n", __func__, sec, nsec, blk_part->offset, blk_part->size);

    if (!(blk_part->mask_flags & PART_FLAG_WRONLY))
    {
        rt_kprintf("ERROR: partition %s is unwriteable, mask_flags = %04x\n", blk_part->name, blk_part->mask_flags);
        return 0;
    }

    while (write_count < nsec)
    {
        if (((sec + 1) * dhara_dev->sector_size) > (blk_part->offset + blk_part->size))
        {
            rt_kprintf("ERROR: write overrun!\n");
            return write_count;
        }
        /* It'a BLOCK device */
        rt_mutex_take(&dhara_blk_lock, RT_WAITING_FOREVER);
        ret = dhara_map_write(&dhara_dev->map, sec, ptr, &err);
        if (ret)
        {
            rt_mutex_release(&dhara_blk_lock);
            return write_count;
        }
#if !defined(RT_USING_DHARA_ENABLE_CACHE)
        ret = dhara_map_sync(&dhara_dev->map, NULL);
#endif
        rt_mutex_release(&dhara_blk_lock);
        if (ret)
            return write_count;
        sec++;
        ptr += dhara_dev->sector_size;
        write_count++;
    }

    return write_count;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops dhara_blk_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    dhara_blk_read,
    dhara_blk_write,
    dhara_blk_control,
};
#endif

/* Register a partition as block partition */
static rt_err_t dhara_blk_register(struct dhara_device *dev, struct rt_flash_partition *blk_part)
{
    if (dev == RT_NULL)
        return -RT_EIO;

    if (blk_part == RT_NULL)
        return -RT_EINVAL;

    dhara_dbg("blk part name: %s\n", blk_part->name);
    /* blk dev setting */
    blk_part->blk.type      = RT_Device_Class_Block;
#ifdef RT_USING_DEVICE_OPS
    blk_part->blk.ops       = &dhara_blk_ops;
#else
    blk_part->blk.read      = dhara_blk_read;
    blk_part->blk.write     = dhara_blk_write;
    blk_part->blk.control   = dhara_blk_control;
#endif
    blk_part->blk.user_data = dev;  /* spinad blk dev for operation */
    /* register device */
    return rt_device_register(&blk_part->blk, blk_part->name, blk_part->mask_flags | RT_DEVICE_FLAG_STANDALONE);
}

static struct rt_flash_partition dhara_blk_default_partition =
{
    .name = "dhara",
    .offset = 0,
    .size = 48400 * 2048, /* 128MB maximum */
    .type = 0x8,
    .mask_flags = PART_FLAG_BLK | PART_FLAG_RDWR,
};

static int dhara_register(struct dhara_device *dhara_dev)
{
    const size_t page_size = dhara_dev->nand.page_size;
    uint8_t *page_buf;
    struct dhara_map *map;
    int ret;

    dhara_dev->nand.copy_buf = rt_dma_malloc(page_size);
    RT_ASSERT(dhara_dev->nand.copy_buf);
    dhara_dev->nand.blocks = rt_calloc(dhara_dev->nand.num_blocks, sizeof(struct block_status));
    RT_ASSERT(dhara_dev->nand.blocks);
    page_buf = rt_dma_malloc(page_size);
    RT_ASSERT(page_buf);

    map = &dhara_dev->map;

    dhara_dbg("dhara init\n");
    dhara_map_init(map, &dhara_dev->nand, page_buf, GC_RATIO);
    dhara_dbg("dhara resume\n");
    dhara_map_resume(map, NULL);
    dhara_dbg("dhara sync\n");

    ret = dhara_map_sync(map, NULL);
    if (ret)
    {
        rt_kprintf("dhara sync failed, ret=%d\n", ret);
        goto exit;
    }
    dhara_dev->sector_size = dhara_dev->nand.page_size;
    dhara_dev->capacity = dhara_map_capacity(map);
    rt_kprintf("  log2_page_size: %d\n", dhara_dev->nand.log2_page_size);
    rt_kprintf("  log2_ppb: %d\n", dhara_dev->nand.log2_ppb);
    rt_kprintf("  num_blocks: %d\n", dhara_dev->nand.num_blocks);
    rt_kprintf("  sector_size: %d\n", dhara_dev->sector_size);
    rt_kprintf("  page_per_block: %d\n", dhara_dev->nand.page_per_block);
    rt_kprintf("  capacity(sec): %d\n", dhara_dev->capacity);
    rt_kprintf("  capacity(MB): %d\n", dhara_dev->capacity * dhara_dev->sector_size / 1024 / 1024);
    rt_kprintf("  start_blocks: %d\n", dhara_dev->nand.start_blocks);
    rt_kprintf("  num_blocks: %d\n", dhara_dev->nand.num_blocks);
    rt_kprintf("  use count: %d\n", dhara_map_size(map));
#ifdef DHARA_RANDOM_TEST
    dhara_rand_test(map, dhara_map_size(map));
#endif

    /* Register partitions */
    dhara_blk_default_partition.size = dhara_dev->capacity * dhara_dev->sector_size;
    ret = dhara_blk_register(dhara_dev, &dhara_blk_default_partition);

exit:
    if (ret)
    {
        rt_free_align(dhara_dev->nand.copy_buf);
        rt_free(dhara_dev->nand.blocks);
        rt_free_align(page_buf);
    }

    return ret;
}
#endif /* #ifdef RT_USING_DHARA */
/** @} */  // SPINAND_Private_Function

/**
 * @brief  Init SPI_NAND framwork and apply to use.
 * @attention The SPI_NAND will be enabled when board initialization, do not
 *      dynamically switch SPINAND unless specifically required.
 */
int rt_hw_spinand_init(void)
{
    struct rt_mtd_nand_device *mtd_dev;
    struct SPI_NAND *spinand;
    struct SPI_NAND_HOST *spi;
    int32_t ret, i = 0;
    int32_t part_num;
    struct rt_flash_partition *part_info;
#ifdef RT_USING_DHARA
    uint32_t block_size;
    uint32_t block_reserved;
    struct dhara_device *dhara_dev;
    uint32_t dhara_part_offset;
    uint32_t dhara_part_size;
#endif

    /* Initial spinand */
    mtd_dev = (struct rt_mtd_nand_device *)rt_calloc(1, sizeof(*mtd_dev));
    RT_ASSERT(mtd_dev);
    spinand = (struct SPI_NAND *)rt_calloc(1, sizeof(*spinand));
    RT_ASSERT(spinand);
    spi = (struct SPI_NAND_HOST *)rt_calloc(1, sizeof(*spi));
    RT_ASSERT(spi);
    spinand->spi = spi;

    ret = spinand_adapt(spinand);
    if (ret)
    {
        rt_kprintf("SPI Nand init adapt error, ret= %ld\n", ret);
        goto err_init;
    }
    if (rt_mutex_init(&(spinand_lock), "spinandLock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("Init mutex error\n");
        RT_ASSERT(0);
    }

#ifdef RT_USING_DHARA
    if (rt_mutex_init(&(dhara_blk_lock), "dharaLock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("Init mutex error\n");
        RT_ASSERT(0);
    }
#endif

    /* Register mtd spinand */
    mtd_dev->page_size       = spinand->secPerPage * SPINAND_SECTOR_SIZE;
    mtd_dev->oob_size        = spinand->secPerPage * 16;
    mtd_dev->oob_free        = spinand->secPerPage * 2;
    mtd_dev->plane_num       = spinand->planePerDie;
    mtd_dev->pages_per_block = spinand->pagePerBlk;
    mtd_dev->block_total     = spinand->blkPerPlane * spinand->planePerDie;
    mtd_dev->block_start     = 0;
    mtd_dev->block_end       = mtd_dev->block_total;
    mtd_dev->ops             = &spinand_mtd_nand_ops;
    mtd_dev->priv            = spinand;

    spinand_dbg("page_size %lx\n", mtd_dev->page_size);
    spinand_dbg("oob_size %lx\n", mtd_dev->oob_size);
    spinand_dbg("oob_free %lx\n", mtd_dev->oob_free);
    spinand_dbg("plane_num %lx\n", mtd_dev->plane_num);
    spinand_dbg("pages_per_block %lx\n", mtd_dev->pages_per_block);
    spinand_dbg("block_total %lx\n", mtd_dev->block_total);
    spinand_dbg("block_start %lx\n", mtd_dev->block_start);
    spinand_dbg("block_end %lx\n", mtd_dev->block_end);

    ret = rt_mtd_nand_register_device("spinand0", mtd_dev);
    if (ret < 0)
    {
        rt_kprintf("rt_mtd_register failed, ret=%d\n", ret);
        goto err_init;
    }

    spinand->pageBuf = rt_dma_malloc(2 * spinand->secPerPage * SPINAND_SECTOR_SIZE);
    RT_ASSERT(spinand->pageBuf);

    /* Parse the partition */
    ret = mtd_nand_rk_partition_init(mtd_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("Scan block in the tail, blk[%d]=%d\n", i, ret);
        goto err_init;
    }
    part_num = get_rk_partition(&part_info);

    /* Register mini ftl */
#ifdef RT_USING_MINI_FTL
    for (i = 0; i < part_num; i++)
    {
        mini_ftl_map_table_init(mtd_dev, part_info[i].offset, part_info[i].size, part_info[i].name);
    }
#endif

    /* Register DHARA FTL, reserved blocks in the tail */
#ifdef RT_USING_DHARA
    dhara_dev = (struct dhara_device *)rt_calloc(1, sizeof(*dhara_dev));
    RT_ASSERT(dhara_dev);

    rt_strcpy(dhara_blk_default_partition.name, part_info[part_num - 1].name);
    if (part_num)
    {
        dhara_part_offset = part_info[part_num - 1].offset >> 9;
        dhara_part_size = part_info[part_num - 1].size >> 9;
    }
    else
    {
        dhara_part_offset = DHARA_DEFAULT_OFFSET;
        dhara_part_size = 0xFFFFFFFF;
    }

    block_reserved = RK_SPINAND_RESERVED_BBT_BLOCKS + 1;
    i = mtd_dev->block_total - block_reserved;
    for (; i >= 0; i--)
    {
        ret = HAL_SPINAND_IsBad(spinand, i * mtd_dev->pages_per_block);
        rt_kprintf("Scan block in the tail, blk[%d]=%d\n", i, ret);
        if (!ret)
        {
            block_reserved = mtd_dev->block_total - i;
            break;
        }
    }

    block_size = mtd_dev->pages_per_block * mtd_dev->page_size;
    dhara_dev->nand.start_blocks = dhara_part_offset * 512 / block_size;
    dhara_dev->nand.num_blocks = mtd_dev->block_total - dhara_dev->nand.start_blocks - block_reserved;
    if (dhara_part_size && dhara_part_size != 0xFFFFFFFF)
    {
        dhara_part_size = dhara_part_size * 512 / block_size;
        if (dhara_part_size < (dhara_dev->nand.num_blocks))
        {
            dhara_dev->nand.num_blocks = dhara_part_size;
        }
    }

    dhara_dev->nand.log2_page_size = __rt_ffs(mtd_dev->page_size) - 1;
    dhara_dev->nand.log2_ppb = __rt_ffs(mtd_dev->pages_per_block) - 1;
    dhara_dev->nand.page_size = mtd_dev->page_size;
    dhara_dev->nand.page_per_block = mtd_dev->pages_per_block;
    dhara_dev->nand.priv_data = spinand;
    ret = dhara_register(dhara_dev);
    if (ret < 0)
    {
        rt_free_align(spinand->pageBuf);
        rt_free(dhara_dev);
    }
#endif

err_init:
    if (ret < 0)
    {
        rt_kprintf("%s failed, ret=%d\n", __func__, ret);
        rt_free(spinand->spi);
        rt_free(spinand);
        rt_free(mtd_dev);
    }

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_spinand_init);

/** @} */  // SPINAND_Public_Function

#endif

/** @} */  // SPINAND

/** @} */  // RKBSP_Common_Driver
