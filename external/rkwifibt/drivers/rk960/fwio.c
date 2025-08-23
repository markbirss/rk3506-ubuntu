/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/firmware.h>

#include "rk960.h"
#include "hwio.h"
#include "hwbus.h"
#include "debug.h"
#include "bh.h"

#include "fwio.h"
//#include "fw_data.h"

struct io_cmd {
	u32 id;
	u32 length;
	u32 addr;
	u32 code_copy_addr;
};

static s8 rk960_atoi(u8 * s)
{
	int num = 0, flag = 0;
	int i;

	for (i = 0; i <= strlen(s); i++) {
		if (s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] - '0';
		else if (s[0] == '-' && i == 0)
			flag = 1;
		else
			break;
	}

	if (flag == 1)
		num = num * -1;

	return num;
}

static int rk960_comma_del(u8 * s, int size)
{
	int i, num = 0;

	for (i = 0; i < size; i++) {
		if (s[i] == ',') {
			s[i] = 0;
			num++;
		}
	}
	return num;
}

#define RK960_TXPWR_COMP_GET(n)  \
    if (rk960_comma_del(p, strlen(p)) != 13) {          \
        result = -1;                                    \
        break;                                          \
    }                                                   \
    for (i = 0; i < 14; i++) {                          \
        hw_priv->txpwr_comp_tbl[i][n] = rk960_atoi(p);  \
        p += strlen(p) + 1;                             \
    }

int rk960_sdd_parse(struct rk960_common *hw_priv, u8 * sdd, int size)
{
	u8 *line = sdd;
	u8 *p;
	int offset = 0;
	int result = 0;
	int i;
        int flag = 0;

        if (!sdd || !size)
                return 0;

	while (1) {
		if (offset + 2 >= size)
			break;
		if (sdd[offset] == 0xd && sdd[offset + 1] == 0xa) {
			sdd[offset] = 0;
			sdd[offset + 1] = 0;

			// get a line to parse
			if (line[0] == '#') {
				//
			} else if (strstr(line, "txpwr_dsss_comp=") != NULL) {
				p = line + strlen("txpwr_dsss_comp=");
				RK960_TXPWR_COMP_GET(0);
                                flag |= BIT(0);
			} else if (strstr(line, "txpwr_cck_comp=") != NULL) {
				p = line + strlen("txpwr_cck_comp=");
				RK960_TXPWR_COMP_GET(1);
                                flag |= BIT(1);
			} else if (strstr(line, "txpwr_bpsk_12_comp=") != NULL) {
				p = line + strlen("txpwr_bpsk_12_comp=");
				RK960_TXPWR_COMP_GET(2);
                                flag |= BIT(2);
			} else if (strstr(line, "txpwr_bpsk_34_comp=") != NULL) {
				p = line + strlen("txpwr_bpsk_34_comp=");
				RK960_TXPWR_COMP_GET(3);
                                flag |= BIT(3);
			} else if (strstr(line, "txpwr_qpsk_12_comp=") != NULL) {
				p = line + strlen("txpwr_qpsk_12_comp=");
				RK960_TXPWR_COMP_GET(4);
                                flag |= BIT(4);
			} else if (strstr(line, "txpwr_qpsk_34_comp=") != NULL) {
				p = line + strlen("txpwr_qpsk_34_comp=");
				RK960_TXPWR_COMP_GET(5);
                                flag |= BIT(5);
			} else if (strstr(line, "txpwr_16qam_12_comp=") != NULL) {
				p = line + strlen("txpwr_16qam_12_comp=");
				RK960_TXPWR_COMP_GET(6);
                                flag |= BIT(6);
			} else if (strstr(line, "txpwr_16qam_34_comp=") != NULL) {
				p = line + strlen("txpwr_16qam_34_comp=");
				RK960_TXPWR_COMP_GET(7);
                                flag |= BIT(7);
			} else if (strstr(line, "txpwr_64qam_23_comp=") != NULL) {
				p = line + strlen("txpwr_64qam_23_comp=");
				RK960_TXPWR_COMP_GET(8);
                                flag |= BIT(8);
			} else if (strstr(line, "txpwr_64qam_34_comp=") != NULL) {
				p = line + strlen("txpwr_64qam_34_comp=");
				RK960_TXPWR_COMP_GET(9);
                                flag |= BIT(9);
			} else if (strstr(line, "txpwr_64qam_56_comp=") != NULL) {
				p = line + strlen("txpwr_64qam_56_comp=");
				RK960_TXPWR_COMP_GET(10);
                                flag |= BIT(10);
			}

			line = sdd + offset + 2;
		}
		offset += 1;
	}

        if (flag != 0x7ff)
                return -1;
        
	return result;
}

void rk960_free_firmware_buf(struct firmware_info *fw_info)
{
	RK960_DEBUG_FW("%s\n", __func__);

#ifdef FW_LOADER_FROM_FOPEN
	if (fw_info->loder_data)
		vfree(fw_info->loder_data);
	fw_info->loder_data = NULL;

	if (fw_info->fw_data)
		vfree(fw_info->fw_data);
	fw_info->fw_data = NULL;

	if (fw_info->fw_rfcal_data)
		vfree(fw_info->fw_rfcal_data);
	fw_info->fw_rfcal_data = NULL;

	if (fw_info->sdd_data)
		vfree(fw_info->sdd_data);
	fw_info->sdd_data = NULL;
#else
        if (fw_info->loder_data_r)
                release_firmware(fw_info->loder_data_r);
        if (fw_info->fw_data_r)
                release_firmware(fw_info->fw_data_r);
        if (fw_info->fw_rfcal_data_r)
                release_firmware(fw_info->fw_rfcal_data_r);
        if (fw_info->sdd_data_r)
                release_firmware(fw_info->sdd_data_r);
#endif

	if (fw_info->buf_data)
		kfree(fw_info->buf_data);

#ifdef FW_DOWNLOAD_CHECK
	if (fw_info->fw_data_check)
		vfree(fw_info->fw_data_check);
	fw_info->fw_data_check = NULL;
#endif

	if (fw_info->fw_start_data)
		kfree(fw_info->fw_start_data);
	fw_info->fw_start_data = NULL;
}

int rk960_alloc_firmware_buf(struct firmware_info *fw_info)
{
	RK960_DEBUG_FW("%s\n", __func__);

#ifdef FW_LOADER_FROM_FOPEN
	fw_info->loder_data = vmalloc(MAX_LOADER_DATA_SIZE);
	if (fw_info->loder_data == NULL) {
		RK960_ERROR_FW("alloc loder1_data failed\n");
		return -ENOMEM;
	}

	fw_info->fw_data = vmalloc(MAX_FW_DATA_SIZE);
	if (fw_info->fw_data == NULL) {
		RK960_ERROR_FW("alloc fw_data failed\n");
		return -ENOMEM;
	}

	fw_info->fw_rfcal_data = vmalloc(MAX_FW_RFCAL_DATA_SIZE);
	if (fw_info->fw_rfcal_data == NULL) {
		RK960_ERROR_FW("alloc fw_rfcal_data failed\n");
		return -ENOMEM;
	}

	fw_info->sdd_data = vmalloc(MAX_SDD_BUF_SIZE);
	if (fw_info->sdd_data == NULL) {
		RK960_ERROR_FW("alloc sdd_data failed\n");
		return -ENOMEM;
	}
#endif

	fw_info->buf_data = kmalloc(fw_info->buf_size, GFP_KERNEL);
	if (fw_info->buf_data == NULL) {
		RK960_ERROR_FW("alloc buf_data failed\n");
		return -ENOMEM;
	}

#ifdef FW_DOWNLOAD_CHECK
	fw_info->fw_data_check = vmalloc(MAX_FW_DATA_SIZE);
	if (fw_info->fw_data_check == NULL) {
		RK960_ERROR_FW("alloc fw_data_check failed\n");
		return -ENOMEM;
	}
#endif

	fw_info->fw_start_data = kmalloc(16, GFP_KERNEL);
	if (fw_info->fw_start_data == NULL) {
		RK960_ERROR_FW("alloc fw_start_data failed\n");
		return -1;
	}
        
	return 0;
}

static const char *const fw_path[] = {
	"/etc/firmware",
	"/vendor/etc/firmware",
	"/lib/firmware",
	"/system/etc/firmware"
};

static int rk960_read_firmware_file(struct firmware_info *fw_info,
				    char *name, u8 * buf, int *len,
				    int max_size, int append)
{
	int i, find = 0;
	char path[64];
	struct file *file;
	int read, size = 1024;
	u8 *buf_start = buf;

	RK960_DEBUG_FW("%s: %s\n", __func__, name);

	for (i = 0; i < ARRAY_SIZE(fw_path); i++) {
		if (!fw_path[i][0])
			continue;

		sprintf(path, "%s/%s", fw_path[i], name);
		//pr_info("%s\n", path);

		file = filp_open(path, O_RDONLY, 0);
		if (IS_ERR(file))
			continue;

		find = 1;
		break;
	}

	if (!find) {
		RK960_ERROR_FW("%s: can't find %s\n", __func__, path);
		return -ENOENT;
	}

	RK960_DEBUG_FW("%s: find %s\n", __func__, path);

        if (rk960_get_file_size(path) > max_size) {
                RK960_ERROR_FW("%s: file(%s) size exceed fw buf(%d)\n",
                        __func__, path, max_size);
                return -ENOMEM;     
        }

	*len = 0;
	while (1) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
		read = kernel_read(file, buf, size, &file->f_pos);
#else
		read = kernel_read(file, file->f_pos, buf, size);
#endif
		if (read <= 0)
			break;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
		file->f_pos += read;
#endif
		buf += read;
		*len += read;
	}

	filp_close(file, NULL);
	*len = ALIGN(*len, 4);

	if (append) {
		fw_info->useful_code_size = *len;

		//We copy the data with this address whichbe crashed by LOADER's start CMD.
		memcpy(buf_start + *len, buf_start + 0x10000, 16);
		*len += 16;
	}

	if (*len > max_size) {
		RK960_ERROR_FW("%s file exceed max size %d(%d)\n", name, *len,
			       max_size);
		return -ENOMEM;
	}

	return 0;
}

int rk960_get_firmware_from_open(struct firmware_info *fw_info)
{
	int ret;

	RK960_INFO_FW("firmware and patch file from open\n");

	ret = rk960_read_firmware_file(fw_info, "rk960_wifi_loader.bin",
				       fw_info->loder_data,
				       &fw_info->loder_size,
				       MAX_LOADER_DATA_SIZE, 0);
	if ((ret < 0) || (fw_info->loder_size <= 0)) {
		//RK960_INFO_FW("no rk960_wifi_loader.bin file\n");
	}
	fw_info->loder_size = ALIGN(fw_info->loder_size, 1024);

	ret = rk960_read_firmware_file(fw_info, "rk960_wifi.bin",
				       fw_info->fw_data, &fw_info->fw_size,
				       MAX_FW_DATA_SIZE, 0);
	if ((ret < 0) || (fw_info->fw_size <= 0)) {
		if (ret == -ENOENT) {
			ret = rk960_read_firmware_file(fw_info, "rk960_fw.bin",
						       fw_info->fw_data,
						       &fw_info->fw_size,
						       MAX_FW_DATA_SIZE, 0);
			if ((ret < 0) || (fw_info->fw_size <= 0))
				return -ENOENT;
		} else {
			return -ENOENT;
		}
	}

	ret = rk960_read_firmware_file(fw_info, "rk960_wifi_rf.bin",
				       fw_info->fw_rfcal_data,
				       &fw_info->fw_rfcal_size,
				       MAX_FW_RFCAL_DATA_SIZE, 0);
	if ((ret < 0) || (fw_info->fw_rfcal_size <= 0)) {
		//RK960_INFO_FW("no rk960_wifi_rfcal.bin file\n");
	}

	fw_info->fw_size = ALIGN(fw_info->fw_size, 1024);
	fw_info->fw_rfcal_size = ALIGN(fw_info->fw_rfcal_size, 1024);

	ret = rk960_read_firmware_file(fw_info, "rk960_sdd.txt",
				       fw_info->sdd_data, &fw_info->sdd_size,
				       MAX_SDD_BUF_SIZE, 0);
	if ((ret < 0) || (fw_info->sdd_size <= 0)) {
		//RK960_INFO_FW("no rk960_sdd.txt file\n");
	}

	return 0;
}

static int rk960_get_firmware_info(struct rk960_common *priv,
                struct firmware_info *fw_info)
{
	int ret = 0;

	if (fw_info->fw_saved)
		return 0;

#ifdef FW_LOADER_FROM_FOPEN
	ret = rk960_get_firmware_from_open(fw_info);
#else
        ret = request_firmware(&fw_info->fw_data_r,
                        "rk960_wifi.bin", priv->pdev);
        if (ret) {
                ret = request_firmware(&fw_info->fw_data_r,
                        "rk960_fw.bin", priv->pdev);
                if (ret) {
                        RK960_ERROR_FW("Can't load firmware file %s.\n",
                                "rk960_wifi.bin");
                        goto firmware_release;
                } else {
                        RK960_INFO_FW("%s: loaded firmware %s\n",
                                __func__, "rk960_fw.bin");
                }
        } else {
                RK960_INFO_FW("%s: loaded firmware %s\n",
                        __func__, "rk960_wifi.bin");
        }

        ret = request_firmware(&fw_info->loder_data_r,
                        "rk960_wifi_loader.bin", priv->pdev);
        if (ret) {
                RK960_ERROR_FW("Can't load firmware file %s.\n",
                        "rk960_wifi_loader.bin");
        } else {
                RK960_INFO_FW("%s: loaded firmware %s\n",
                        __func__, "rk960_wifi_loader.bin");
        }

        ret = request_firmware(&fw_info->fw_rfcal_data_r,
                        "rk960_wifi_rf.bin", priv->pdev);
        if (ret) {
                RK960_ERROR_FW("Can't load firmware file %s.\n",
                        "rk960_wifi_rf.bin");
        } else {
                RK960_INFO_FW("%s: loaded firmware %s\n",
                        __func__, "rk960_wifi_rf.bin");
        }

        ret = request_firmware(&fw_info->sdd_data_r,
                        "rk960_sdd.txt", priv->pdev);
        if (ret) {
                RK960_ERROR_FW("Can't load firmware file %s.\n",
                        "rk960_sdd.txt");
        } else {
                RK960_INFO_FW("%s: loaded firmware %s\n",
                        __func__, "rk960_sdd.txt");
        }

        return 0;

firmware_release:
        if (fw_info->loder_data_r)
                release_firmware(fw_info->loder_data_r);
        if (fw_info->fw_data_r)
                release_firmware(fw_info->fw_data_r);
        if (fw_info->fw_rfcal_data_r)
                release_firmware(fw_info->fw_rfcal_data_r);
        if (fw_info->sdd_data_r)
                release_firmware(fw_info->sdd_data_r);
#endif

	return ret;
}

static int rk960_get_rom_version(struct rk960_common *priv)
{
	int ret;
	struct firmware_info *fw = &priv->firmware;

	memset(fw->buf_data, 0, 16);
	ret = rk960_reg_read(priv, SDIO_ROM_VER_ADDR, fw->buf_data, 10);
	if (ret) {
		RK960_ERROR_FW("%s: read rom_ver failed (%d)\n", __func__, ret);
		return -1;
	}

	RK960_INFO_FW("%s: rom_ver:", __func__);
        print_hex_dump(KERN_INFO, " ", DUMP_PREFIX_NONE,
                16, 1, fw->buf_data, 10, 1);
	return 0;
}

int rk960_download_fw(struct rk960_common *priv, int start_fw)
{
	int ret = -ENOENT;
	struct firmware_info *fw = &priv->firmware;
	int cnt, left_size, write_size, sdio_cmd_addr, write_addr_block_size;
	struct io_cmd *scmd;
	int i;
	bool is_big_fw;
        int loder_size = 0;
        unsigned char *loder_data = NULL;
        int fw_rfcal_size = 0;
        unsigned char *fw_rfcal_data = NULL;
        int fw_size = 0;
        unsigned char *fw_data = NULL;

#ifdef FW_LOADER_FROM_FOPEN
        fw_size = fw->fw_size;
#else
        fw_size = fw->fw_data_r->size;
#endif
	is_big_fw = fw_size > 64 * 1024 ? true : false;
	if (priv->chip_id == RK960_DEVICE_ID_D) {
		if (is_big_fw)
			sdio_cmd_addr = SDIO_CMD_ADDR_VER_D;
		else
			sdio_cmd_addr = SDIO_CMD_ADDR_VER_ROM;
		write_addr_block_size = 512;
	} else {
		sdio_cmd_addr = SDIO_CMD_ADDR_VER_ABC;
		write_addr_block_size = 1;
	}

	RK960_DEBUG_FW("%s: chip id %x block size %d buf size %d\n",
		       __func__, priv->chip_id, write_addr_block_size,
		       fw->buf_size);

	scmd = (struct io_cmd *)fw->fw_start_data;

#ifdef FW_LOADER_FROM_FOPEN
        loder_size = fw->loder_size;
        loder_data = fw->loder_data;
#else
        if (fw->loder_data_r) {
                loder_size = fw->loder_data_r->size;
                loder_data = (unsigned char *)fw->loder_data_r->data;
        }
#endif
	//download loader
	if (!is_big_fw && loder_size) {
		RK960_INFO_FW("%s: start download loader size: %d\n", __func__,
			      loder_size);
		cnt = ((loder_size - 1) / fw->buf_size) + 1;
		left_size = loder_size;
		for (i = 0; i < cnt; i++) {
			if (left_size >= fw->buf_size) {
				write_size = fw->buf_size;
			} else if (left_size > 0) {
				write_size = left_size;
			} else {
				break;
			}
                        memset(fw->buf_data, 0, fw->buf_size);
			memcpy(fw->buf_data, loder_data + i * fw->buf_size,
			       write_size);
			ret =
			    rk960_reg_write(priv,
					    SDIO_LOADER_VCT_ADDR /
					    write_addr_block_size, fw->buf_data,
					    ALIGN(write_size, 1024));
			if (ret) {
				RK960_ERROR_FW
				    ("%s: rk960_reg_write failed (%d), write_size = %d\n",
				     __func__, ret, write_size);
				goto fail;
			}
			left_size -= write_size;
		}

		//send start CMD to reboot
		scmd->id = SDIO_START_CMD_ID;
		scmd->length = 4;
		scmd->addr = SDIO_LOADER_VCT_ADDR;
		scmd->code_copy_addr = fw->useful_code_size;
		ret =
		    rk960_reg_write(priv, sdio_cmd_addr, (void *)scmd,
				    sizeof(struct io_cmd));
		if (ret) {
			RK960_ERROR_FW("%s: start loader failed (%d)\n",
				       __func__, ret);
			goto fail;
		}

		msleep(10);

		rk960_get_rom_version(priv);
	}

#ifdef FW_LOADER_FROM_FOPEN
        fw_rfcal_size = fw->fw_rfcal_size;
        fw_rfcal_data = fw->fw_rfcal_data;
#else
        if (fw->fw_rfcal_data_r) {
                fw_rfcal_size = fw->fw_rfcal_data_r->size;
                fw_rfcal_data = (unsigned char *)fw->fw_rfcal_data_r->data;
        }
#endif        
	//download rfcal fw
	if (!is_big_fw && fw_rfcal_size) {
		u16 ctrl_reg;
		int to_count = 500;

		RK960_INFO_FW("%s: start download rfcal firmware size: %d\n",
			      __func__, fw_rfcal_size);
		cnt = ((fw_rfcal_size - 1) / fw->buf_size) + 1;
		left_size = fw_rfcal_size;
		for (i = 0; i < cnt; i++) {
			if (left_size >= fw->buf_size) {
				write_size = fw->buf_size;
			} else if (left_size > 0) {
				write_size = left_size;
			} else {
				break;
			}
                        memset(fw->buf_data, 0, fw->buf_size);
			memcpy(fw->buf_data,
			       fw_rfcal_data + i * fw->buf_size,
			       write_size);
			ret =
			    rk960_reg_write(priv,
					    (i * fw->buf_size) /
					    write_addr_block_size, fw->buf_data,
					    ALIGN(write_size, 1024));
			if (ret) {
				RK960_ERROR_FW
				    ("%s: rk960_reg_write failed (%d), write_size = %d\n",
				     __func__, ret, write_size);
				goto fail;
			}
			left_size -= write_size;
		}

		//send start CMD to reboot
		scmd->id = SDIO_START_CMD_ID;
		scmd->length = 4;
		scmd->addr = SDIO_FIRMWARE_VCT_ADDR;
		scmd->code_copy_addr = fw->useful_code_size;
		ret =
		    rk960_reg_write(priv, sdio_cmd_addr, (void *)scmd,
				    sizeof(struct io_cmd));
		if (ret) {
			RK960_ERROR_FW("%s: start rfcal fw failed (%d)\n",
				       __func__, ret);
			goto fail;
		}
		// wait for rfcal complete
		while (to_count--) {
			rk960_bh_read_ctrl_reg(priv, &ctrl_reg);
			if (ctrl_reg) {
				RK960_INFO_FW("rfcal complete %x\n", ctrl_reg);
				__rk960_clear_irq(priv);
				break;
			}
			msleep(10);
		}
		if (to_count <= 0) {
			RK960_ERROR_FW("rfcal failed\n");
			ret = -1;
			goto fail;
		}
	}

        /*ret = rk960_sdio_irq_subscribe(priv->hwbus_priv);
        if (ret) {
                goto fail;
        }*/

#ifdef FW_LOADER_FROM_FOPEN
        fw_size = fw->fw_size;
        fw_data = fw->fw_data;
#else
        fw_size = fw->fw_data_r->size;
        fw_data = (unsigned char *)fw->fw_data_r->data;
#endif         
	//download fw and read back
	RK960_INFO_FW("%s: start download firmware size: %d\n", __func__,
		      fw_size);
	cnt = ((fw_size - 1) / fw->buf_size) + 1;
	left_size = fw_size;
	for (i = 0; i < cnt; i++) {
		if (left_size >= fw->buf_size) {
			write_size = fw->buf_size;
		} else if (left_size > 0) {
			write_size = left_size;
		} else {
			break;
		}
                memset(fw->buf_data, 0, fw->buf_size);
		memcpy(fw->buf_data, fw_data + i * fw->buf_size,
		       write_size);
		ret =
		    rk960_reg_write(priv,
				    (i * fw->buf_size) / write_addr_block_size,
				    fw->buf_data, ALIGN(write_size, 1024));
		if (ret) {
			RK960_ERROR_FW
			    ("%s: rk960_reg_write failed (%d), write_size = %d\n",
			     __func__, ret, write_size);
			goto fail;
		}
#ifdef FW_DOWNLOAD_CHECK
		ret =
		    rk960_reg_read(priv,
				   (i * fw->buf_size) / write_addr_block_size,
				   fw->buf_data, ALIGN(write_size, 1024));
		if (ret) {
			RK960_ERROR_FW("%s: read fw failed (%d)\n", __func__,
				       ret);
			goto fail;
		}
		memcpy(fw->fw_data_check + i * fw->buf_size, fw->buf_data,
		       write_size);
#endif
		left_size -= write_size;
	}

#ifdef FW_DOWNLOAD_CHECK
	//check read back's fw data
	ret = memcmp(fw_data, fw->fw_data_check, fw_size);
	if (ret && !priv->fw_error_processing) {
		RK960_ERROR_FW("%s: check downloaded fw failed\n", __func__);
		for (i = 0; i < fw_size; i++) {
			if (fw->fw_data_check[i] != fw_data[i]) {
				RK960_ERROR_FW
				    ("Addr: %d, data: 0x%02x,	bad: 0x%02x\n", i,
				     fw_data[i], fw->fw_data_check[i]);
			}
		}
		ret = -ENOENT;
		goto fail;
	} else {
		RK960_INFO_FW("%s: check downloaded fw ok.\n", __func__);
	}
#endif

	//send start CMD to reboot
	scmd->id = SDIO_START_CMD_ID;
	scmd->length = 4;
	scmd->addr = SDIO_FIRMWARE_VCT_ADDR;
	scmd->code_copy_addr = fw->useful_code_size;

	if (start_fw) {
		ret =
	    		rk960_reg_write(priv, sdio_cmd_addr, (void *)scmd,
			    sizeof(struct io_cmd));
		if (ret) {
			RK960_ERROR_FW("%s: start fw failed (%d)\n",
					__func__, ret);
			goto fail;
		}
		//mdelay(20);
	} else {
        	priv->fw_scmd = scmd;
        	priv->sdio_cmd_addr = sdio_cmd_addr;
	}

	RK960_INFO_FW("%s: download firmware success\n", __func__);

	return 0;

fail:
	return ret;
}

int rk960_start_fw(struct rk960_common *priv)
{
        int ret;
        struct io_cmd *scmd = priv->fw_scmd;
        int sdio_cmd_addr = priv->sdio_cmd_addr;
        
	ret =
	    rk960_reg_write(priv, sdio_cmd_addr, (void *)scmd,
			    sizeof(struct io_cmd));
	if (ret) {
		RK960_ERROR_FW("%s: start fw failed (%d)\n", __func__, ret);
                return -1;
	}
        RK960_INFO_FW("%s: start fw success\n", __func__);
        
	//mdelay(20);
	return 0;
}

/* return 0 means fw download success, otherwise fail */
int rk960_load_firmware(struct rk960_common *priv)
{
	struct firmware_info *fw_info;
	int ret;

	fw_info = &priv->firmware;
	if (priv->chip_id == RK960_DEVICE_ID_D) {
		fw_info->buf_size = MAX_FW_BUF_SIZE_SMALL;
	} else {
		fw_info->buf_size = MAX_FW_BUF_SIZE_BIG;
	}

	if (rk960_alloc_firmware_buf(fw_info) != 0) {
		RK960_ERROR_FW("%s: rk960_alloc_firmware_buf failed\n",
			       __func__);
		ret = -ENOENT;
		goto fw_out;
	}

	/* get info of firmware and rompatch */
	if (rk960_get_firmware_info(priv, fw_info)) {
		RK960_ERROR_FW("%s: get firmeware error!.\n", __func__);
		ret = -ENOENT;
		goto fw_out;
	}

#ifdef FW_LOADER_FROM_FOPEN
	if (rk960_sdd_parse(priv, fw_info->sdd_data, fw_info->sdd_size)) {
#else
        if (rk960_sdd_parse(priv,
                        fw_info->sdd_data_r ?
                        (unsigned char *)fw_info->sdd_data_r->data:NULL,
                        fw_info->sdd_data_r ? fw_info->sdd_data_r->size:0)) {
#endif
		RK960_ERROR_FW("%s: sdd parse error!.\n", __func__);
		ret = -ENOENT;
		goto fw_out;
	}
	//fw_info->fw_saved = 1;

#ifdef SUPPORT_FWCR
        if (priv->fw_hotboot)
                return 0;
#endif

	ret = rk960_download_fw(priv, 0);
	if (!ret)
		return ret;

fw_out:
	rk960_free_firmware_buf(fw_info);
	return ret;
}
