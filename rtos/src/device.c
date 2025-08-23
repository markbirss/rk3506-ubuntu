/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2007-01-21     Bernard      the first version
 * 2010-05-04     Bernard      add rt_device_init implementation
 * 2012-10-20     Bernard      add device check in register function,
 *                             provided by Rob <rdent@iinet.net.au>
 * 2012-12-25     Bernard      return RT_EOK if the device interface not exist.
 * 2013-07-09     Grissiom     add ref_count support
 * 2016-04-02     Bernard      fix the open_flag initialization issue.
 * 2021-03-19     Meco Man     remove rt_device_init_all()
 */

#include <rtthread.h>
#ifdef RT_USING_POSIX_DEVIO
#include <rtdevice.h> /* for wqueue_init */
#endif /* RT_USING_POSIX_DEVIO */

#ifdef RT_USING_DEVICE

#define RT_DEVICE_BUF_UNALIGNED_SUPPORT

#ifdef RT_USING_DEVICE_OPS
#define device_init     (dev->ops->init)
#define device_open     (dev->ops->open)
#define device_close    (dev->ops->close)
#define device_read     (dev->ops->read)
#define device_write    (dev->ops->write)
#define device_control  (dev->ops->control)
#else
#define device_init     (dev->init)
#define device_open     (dev->open)
#define device_close    (dev->close)
#define device_read     (dev->read)
#define device_write    (dev->write)
#define device_control  (dev->control)
#endif /* RT_USING_DEVICE_OPS */

/**
 * @brief This function registers a device driver with a specified name.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param name is the device driver's name.
 *
 * @param flags is the capabilities flag of device.
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
rt_err_t rt_device_register(rt_device_t dev,
                            const char *name,
                            rt_uint16_t flags)
{
    if (dev == RT_NULL)
        return -RT_ERROR;

    if (rt_device_find(name) != RT_NULL)
        return -RT_ERROR;

    rt_object_init(&(dev->parent), RT_Object_Class_Device, name);
    dev->flag = flags;
    dev->ref_count = 0;
    dev->open_flag = 0;

#ifdef RT_USING_POSIX_DEVIO
    dev->fops = RT_NULL;
    rt_wqueue_init(&(dev->wait_queue));
#endif /* RT_USING_POSIX_DEVIO */

    return RT_EOK;
}
RTM_EXPORT(rt_device_register);

/**
 * @brief This function removes a previously registered device driver.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_device_unregister(rt_device_t dev)
{
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);
    RT_ASSERT(rt_object_is_systemobject(&dev->parent));

    rt_object_detach(&(dev->parent));

    return RT_EOK;
}
RTM_EXPORT(rt_device_unregister);

/**
 * @brief This function finds a device driver by specified name.
 *
 * @param name is the device driver's name.
 *
 * @return the registered device driver on successful, or RT_NULL on failure.
 */
rt_device_t rt_device_find(const char *name)
{
    return (rt_device_t)rt_object_find(name, RT_Object_Class_Device);
}
RTM_EXPORT(rt_device_find);

#ifdef RT_USING_HEAP
/**
 * @brief This function creates a device object with user data size.
 *
 * @param type is the type of the device object.
 *
 * @param attach_size is the size of user data.
 *
 * @return the allocated device object, or RT_NULL when failed.
 */
rt_device_t rt_device_create(int type, int attach_size)
{
    int size;
    rt_device_t device;

    size = RT_ALIGN(sizeof(struct rt_device), RT_ALIGN_SIZE);
    attach_size = RT_ALIGN(attach_size, RT_ALIGN_SIZE);
    /* use the total size */
    size += attach_size;

    device = (rt_device_t)rt_malloc(size);
    if (device)
    {
        rt_memset(device, 0x0, sizeof(struct rt_device));
        device->type = (enum rt_device_class_type)type;
    }

    return device;
}
RTM_EXPORT(rt_device_create);

/**
 * @brief This function destroy the specific device object.
 *
 * @param dev is a specific device object.
 */
void rt_device_destroy(rt_device_t dev)
{
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);
    RT_ASSERT(rt_object_is_systemobject(&dev->parent) == RT_FALSE);

    rt_object_detach(&(dev->parent));

    /* release this device object */
    rt_free(dev);
}
RTM_EXPORT(rt_device_destroy);
#endif /* RT_USING_HEAP */

/**
 * @brief This function will initialize the specified device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @return the result, RT_EOK on successfully.
 */
rt_err_t rt_device_init(rt_device_t dev)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(dev != RT_NULL);

    /* get device_init handler */
    if (device_init != RT_NULL)
    {
        if (!(dev->flag & RT_DEVICE_FLAG_ACTIVATED))
        {
            result = device_init(dev);
            if (result != RT_EOK)
            {
                RT_DEBUG_LOG(RT_DEBUG_DEVICE, ("To initialize device:%s failed. The error code is %d\n",
                           dev->parent.name, result));
            }
            else
            {
                dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
            }
        }
    }

    return result;
}

/**
 * @brief This function will open a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param oflag is the flags for device open.
 *
 * @return the result, RT_EOK on successfully.
 */
rt_err_t rt_device_open(rt_device_t dev, rt_uint16_t oflag)
{
    rt_err_t result = RT_EOK;

    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    /* if device is not initialized, initialize it. */
    if (!(dev->flag & RT_DEVICE_FLAG_ACTIVATED))
    {
        if (device_init != RT_NULL)
        {
            result = device_init(dev);
            if (result != RT_EOK)
            {
                RT_DEBUG_LOG(RT_DEBUG_DEVICE, ("To initialize device:%s failed. The error code is %d\n",
                           dev->parent.name, result));

                return result;
            }
        }

        dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
    }

    /* device is a stand alone device and opened */
    if ((dev->flag & RT_DEVICE_FLAG_STANDALONE) &&
        (dev->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
        return -RT_EBUSY;
    }

    /* call device_open interface */
    if (device_open != RT_NULL)
    {
        result = device_open(dev, oflag);
    }
    else
    {
        /* set open flag */
        dev->open_flag = (oflag & RT_DEVICE_OFLAG_MASK);
    }

    /* set open flag */
    if (result == RT_EOK || result == -RT_ENOSYS)
    {
        dev->open_flag |= RT_DEVICE_OFLAG_OPEN;

        dev->ref_count++;
        /* don't let bad things happen silently. If you are bitten by this assert,
         * please set the ref_count to a bigger type. */
        RT_ASSERT(dev->ref_count != 0);
    }

    return result;
}
RTM_EXPORT(rt_device_open);

/**
 * @brief This function will close a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @return the result, RT_EOK on successfully.
 */
rt_err_t rt_device_close(rt_device_t dev)
{
    rt_err_t result = RT_EOK;

    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    if (dev->ref_count == 0)
        return -RT_ERROR;

    dev->ref_count--;

    if (dev->ref_count != 0)
        return RT_EOK;

    /* call device_close interface */
    if (device_close != RT_NULL)
    {
        result = device_close(dev);
    }

    /* set open flag */
    if (result == RT_EOK || result == -RT_ENOSYS)
        dev->open_flag = RT_DEVICE_OFLAG_CLOSE;

    return result;
}
RTM_EXPORT(rt_device_close);

/**
 * @brief This function will read some data from a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param pos is the position when reading.
 *
 * @param buffer is a data buffer to save the read data.
 *
 * @param size is the size of buffer.
 *
 * @return the actually read size on successful, otherwise 0 will be returned.
 *
 * @note the unit of size/pos is a block for block device.
 */
#ifdef RT_DEVICE_BUF_UNALIGNED_SUPPORT
static ALIGN(128) int device_align_buffer[1024];
static struct rt_mutex *device_buffer_mutex;

static rt_err_t device_buffer_mutex_init(void)
{
    if (device_buffer_mutex == RT_NULL)
    {
        device_buffer_mutex = rt_mutex_create("device_buffer", RT_IPC_FLAG_PRIO);
        if (device_buffer_mutex == RT_NULL)
        {
            RT_ASSERT(0);
        }
    }

    return RT_EOK;
}

void device_buffer_lock(void)
{
    rt_err_t result = -RT_EBUSY;

    device_buffer_mutex_init();

    while (result == -RT_EBUSY)
    {
        result = rt_mutex_take(device_buffer_mutex, RT_WAITING_FOREVER);
    }

    if (result != RT_EOK)
    {
        RT_ASSERT(0);
    }
}

static void device_buffer_unlock(void)
{
    rt_mutex_release(device_buffer_mutex);
}

#endif
rt_size_t rt_device_read(rt_device_t dev,
                         rt_off_t    pos,
                         void       *buffer,
                         rt_size_t   size)
{
#ifdef RT_DEVICE_BUF_UNALIGNED_SUPPORT
    rt_size_t   align, ret, rsec, sec_size;
    void *buf = buffer;
    struct rt_device_blk_geometry geometry;
#endif
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    if (dev->ref_count == 0)
    {
        rt_set_errno(-RT_ERROR);
        return 0;
    }

    /* call device_read interface */
    if (device_read != RT_NULL)
    {
#ifdef RT_DEVICE_BUF_UNALIGNED_SUPPORT
        /* The buffer is not align to 4 bytes and the block device need aligned buffer */
        align = (long)buffer & 0x3;
        rt_device_control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
        sec_size = geometry.bytes_per_sector;
        if (RT_Device_Class_Block == dev->type && align && size)
        {
            device_buffer_lock();
            rsec = size - 1;
            if (size > 1)
            {
                buf = buffer + 4 - align;
                ret = device_read(dev, pos, buf, rsec);
                if (ret != rsec)
                {
                    device_buffer_unlock();
                    return ret;
                }
                rt_memmove(buffer, buf, rsec * sec_size);
                buf = buffer + sec_size * rsec;
            }
            ret = device_read(dev, pos + rsec, device_align_buffer, 1);
            rt_memcpy(buf, device_align_buffer, sec_size);
            device_buffer_unlock();
            if (ret != 1)
                return ret;
            return size;
        }
#endif
        return device_read(dev, pos, buffer, size);
    }

    /* set error code */
    rt_set_errno(-RT_ENOSYS);

    return 0;
}
RTM_EXPORT(rt_device_read);

/**
 * @brief This function will write some data to a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param pos is the position when writing.
 *
 * @param buffer is the data buffer to be written to device.
 *
 * @param size is the size of buffer.
 *
 * @return the actually written size on successful, otherwise 0 will be returned.
 *
 * @note the unit of size/pos is a block for block device.
 */
rt_size_t rt_device_write(rt_device_t dev,
                          rt_off_t    pos,
                          const void *buffer,
                          rt_size_t   size)
{
#ifdef RT_DEVICE_BUF_UNALIGNED_SUPPORT
    rt_size_t   ret = RT_EOK, sec_size;
    struct rt_device_blk_geometry geometry;
    void *buf;
#endif
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    if (dev->ref_count == 0)
    {
        rt_set_errno(-RT_ERROR);
        return 0;
    }

    /* call device_write interface */
    if (device_write != RT_NULL)
    {
#ifdef RT_DEVICE_BUF_UNALIGNED_SUPPORT
        /* The buffer is not align to 4 bytes and the block device need aligned buffer */
        if (RT_Device_Class_Block == dev->type && ((long)buffer & 0x3))
        {
            rt_device_control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
            sec_size = geometry.bytes_per_sector;
            device_buffer_lock();

            /* write 1 sector from align copy buffer */
            rt_memcpy(device_align_buffer, buffer, sec_size);
            ret = device_write(dev, pos, device_align_buffer, 1);
            if (ret != 1)
            {
                device_buffer_unlock();
                return 0;
            }

            /* write remain sector */
            buf = (void *)(((long)buffer & ~0x3) + 4);
            rt_memmove(buf, buffer + sec_size, (size - 1) * sec_size);
            ret = device_write(dev, pos + 1, buf, size - 1);
            if (ret != size - 1)
            {
                device_buffer_unlock();
                return (ret + 1);
            }

            device_buffer_unlock();
            return size;
        }
#endif
        return device_write(dev, pos, buffer, size);
    }

    /* set error code */
    rt_set_errno(-RT_ENOSYS);

    return 0;
}
RTM_EXPORT(rt_device_write);

/**
 * @brief This function will perform a variety of control functions on devices.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param cmd is the command sent to device.
 *
 * @param arg is the argument of command.
 *
 * @return the result, -RT_ENOSYS for failed.
 */
rt_err_t rt_device_control(rt_device_t dev, int cmd, void *arg)
{
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    /* call device_write interface */
    if (device_control != RT_NULL)
    {
        return device_control(dev, cmd, arg);
    }

    return -RT_ENOSYS;
}
RTM_EXPORT(rt_device_control);

/**
 * @brief This function will set the reception indication callback function. This callback function
 *        is invoked when this device receives data.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param rx_ind is the indication callback function.
 *
 * @return RT_EOK
 */
rt_err_t rt_device_set_rx_indicate(rt_device_t dev,
                                   rt_err_t (*rx_ind)(rt_device_t dev,
                                   rt_size_t size))
{
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    dev->rx_indicate = rx_ind;

    return RT_EOK;
}
RTM_EXPORT(rt_device_set_rx_indicate);

/**
 * @brief This function will set a callback function. The callback function
 *        will be called when device has written data to physical hardware.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param tx_done is the indication callback function.
 *
 * @return RT_EOK
 */
rt_err_t rt_device_set_tx_complete(rt_device_t dev,
                                   rt_err_t (*tx_done)(rt_device_t dev,
                                   void *buffer))
{
    /* parameter check */
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(rt_object_get_type(&dev->parent) == RT_Object_Class_Device);

    dev->tx_complete = tx_done;

    return RT_EOK;
}
RTM_EXPORT(rt_device_set_tx_complete);

#endif /* RT_USING_DEVICE */
