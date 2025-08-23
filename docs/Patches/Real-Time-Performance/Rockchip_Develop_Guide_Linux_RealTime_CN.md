# Rockchip Linux RealTime Develop Guide
文档标识：RK-KF-YF-A26

发布版本：V1.1.0

日期：2024-08-20

文件密级：□绝密   □秘密   □内部资料   ■公开

**免责声明**

本文档按“现状”提供，瑞芯微电子股份有限公司（“本公司”，下同）不对本文档的任何陈述、信息和内容的准确性、可靠性、完整性、适销性、特定目的性和非侵权性提供任何明示或暗示的声明或保证。本文档仅作为使用指导的参考。

由于产品版本升级或其他原因，本文档将可能在未经任何通知的情况下，不定期进行更新或修改。

**商标声明**

“Rockchip”、“瑞芯微”、“瑞芯”均为本公司的注册商标，归本公司所有。

本文档可能提及的其他所有注册商标或商标，由其各自拥有者所有。

**版权所有© 2024 瑞芯微电子股份有限公司**

超越合理使用范畴，非经本公司书面许可，任何单位和个人不得擅自摘抄、复制本文档内容的部分或全部，并不得以任何形式传播。

瑞芯微电子股份有限公司

Rockchip Electronics Co., Ltd.

地址：     福建省福州市铜盘路软件园A区18号

网址：     www.rock-chips.com

客户服务电话： +86-4007-700-590

客户服务传真： +86-591-83951833

客户服务邮箱： fae@rock-chips.com

---

**前言**

**概述**

本文主要描述了Rockchip Linux 内核实时性补丁基本使用方法，旨在帮助开发者快速了解并使用实时性系统。

**读者对象**

本文档（本指南）主要适用于以下工程师：

技术支持工程师

软件开发工程师

**产品版本**

| 芯片名称 | 内核版本                 |
| -------- | ------------------------ |
| RK3562   | kernel-5.10              |
| RK3568   | kernel-4.19，kernel-5.10 |
| RK3588   | kernel-5.10              |
| RK3576   | kernel-6.1               |
| RK3506   | kernel-6.1               |

 **修订记录**

| **日期**   | **版本** | **作者**   | **修改说明**           |
| ---------- | :------- | :--------- | :--------------------- |
| 2023-11-20 | V0.0.1   | czz        | 初始版本               |
| 2024-06-20 | V1.0.0   | LinJianhua | 更新到V1.0.0           |
| 2024-08-20 | V1.1.0   | LinJianhua | 增加Kernel-6.1.84 补丁 |

---

**目录**

[TOC]

------

## 概要

根据当前内核版本，选择对应的实时性系统内核补丁，确认内核版本的方法为查看`kernel$ vi Makefile`。

```shell
# SPDX-License-Identifier: GPL-2.0
VERSION = 5
PATCHLEVEL = 10
SUBLEVEL = 209
...
```

> 备注：以Kernel-5.10.209 举例

**不同版本补丁对应的内核提交点如下：**

Kernel-4.19

PREEMPT_RT 补丁对应的提交点：

```shell
commit a46049b85fd7e5e9f58701fbc387e5b4d3793f98 (rk/develop-4.19, m/master)
Author: Shunhua Lan <lsh@rock-chips.com>
Date:   Mon Feb 6 16:45:53 2023 +0800

media: i2c: lt6911uxc: create hdmirx_class devices

Signed-off-by: Shunhua Lan <lsh@rock-chips.com>
Change-Id: I61c840d812b88554aa154bfc7c1435e1345d287e
```

XENOMAI 补丁对应的提交点：

```shell
commit 09f54150e89f68cece4ba5af11a1fd07dfa35aa3 (rk/develop-4.19)
Author: Zhihuan He <huan.he@rock-chips.com>
Date:   Wed Aug 23 11:37:06 2023 +0800

arm64: configs: rockchip_linux_defconfig: enable rockchip edac

Signed-off-by: Zhihuan He <huan.he@rock-chips.com>
Change-Id: Ie3c9b6150e792cb1bca395f630bf35da82168f2b
```

Kernel-5.10.160

```shell
commit cae91899b67b031d95f9163fe1fda74fbe0d931a (tag: linux-5.10-stan-rkr1)
Author: Lan Honglin <helin.lan@rock-chips.com>
Date:   Wed Jun 7 15:01:26 2023 +0800
ARM: configs: rockchip: rv1106 enable sc301iot for battery-ipc

Signed-off-by: Lan Honglin <helin.lan@rock-chips.com>
Change-Id: Ib844385bfd58f73eaa5f4e415d598d1f983fa4cd
```

Kernel-5.10.198

```shell
commit 604cec4004abe5a96c734f2fab7b74809d2d742f (tag: linux-5.10-gen-rkr7.1, tag: m/linux)
Author: Finley Xiao <finley.xiao@rock-chips.com>
Date:   Wed Dec 27 18:55:05 2023 +0800

    soc: rockchip: rockchip_system_monitor: Fix opp_info NULL pointer

    Fixes: feecbd010e4e ("soc: rockchip: rockchip_system_monitor: Add support to use low temp pvtpll config")
    Signed-off-by: Finley Xiao <finley.xiao@rock-chips.com>
    Change-Id: I17f5dbc2cd2da487f7e5c9f81a89520c6eb53799
```

Kernel-5.10.209

```shell
commit bc1fe92fa943551fd9519bcc191b058792f3a9be (HEAD, tag: linux-6.1-stan-rkr3, rk/develop-6.1, m/master)
Author: Cai YiWei <cyw@rock-chips.com>
Date:   Thu Jun 20 17:13:52 2024 +0800

    media: rockchip: isp: add stats log for isp21 and isp30

    Change-Id: I5562b78ce87d4773c08ffbe85f4e0bef351344da
    Signed-off-by: Cai YiWei <cyw@rock-chips.com>
```

Kernel-6.1.75

```shell
commit 6f0f65649115d2948432fbea8043c0e5d2d5969a
Author: Zhihuan He <huan.he@rock-chips.com>
Date:   Wed May 22 17:31:27 2024 +0800

    ARM: configs: rockchip_linux_defconfig: enable dsmc

    Change-Id: I31695bf34380b3a5926976a2d497390f098d6bfe
    Signed-off-by: Zhihuan He <huan.he@rock-chips.com>
```

Kernel-6.1.84

```bash
commit b453658077fbb9e67d117a5ab2bb0cf5af729a95 (demo_debug)
Merge: d7d3217791bd 347385861c50
Author: Tao Huang <huangtao@rock-chips.com>
Date:   Sat Aug 17 17:35:51 2024 +0800

    Merge tag 'v6.1.84'

    This is the 6.1.84 stable release

    * tag 'v6.1.84': (1865 commits)
      Linux 6.1.84
      tools/resolve_btfids: fix build with musl libc
      USB: core: Fix deadlock in usb_deauthorize_interface()
      x86/sev: Skip ROM range scans and validation for SEV-SNP guests
      scsi: libsas: Fix disk not being scanned in after being removed
      scsi: libsas: Add a helper sas_get_sas_addr_and_dev_type()
      scsi: lpfc: Correct size for wqe for memset()
      scsi: lpfc: Correct size for cmdwqe/rspwqe for memset()
      tls: fix use-after-free on failed backlog decryption
      x86/cpu: Enable STIBP on AMD if Automatic IBRS is enabled
      scsi: qla2xxx: Delay I/O Abort on PCI error
      scsi: qla2xxx: Change debug message during driver unload
      scsi: qla2xxx: Fix double free of fcport
      scsi: qla2xxx: Fix command flush on cable pull
      scsi: qla2xxx: NVME|FCP prefer flag not being honored
      scsi: qla2xxx: Update manufacturer detail
      scsi: qla2xxx: Split FCE|EFT trace control
      scsi: qla2xxx: Fix N2N stuck connection
      scsi: qla2xxx: Prevent command send on chip reset
      usb: typec: ucsi: Clear UCSI_CCI_RESET_COMPLETE before reset
      ...

    Change-Id: If6edd552c88012d97f5eefc5e1d97a4f1683f171
```

## PREEMPT_RT

###    内核打上补丁

根据当前内核版本，选择对应的内核版本补丁。

###   编译内核

```bash
$ cd $sdk/kernel/
$ export CROSS_COMPILE=../prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
$ make ARCH=arm64 rockchip_linux_defconfig rk3588_linux.config rockchip_rt.config
$ make ARCH=arm64 rk3588-evb1-lp4-v10-linux.img -j8
```

> 备注：此处以RK3588为例，其它芯片平台编译内核，内核配置要加上rockchip_rt.config。

```bash
$ cd $sdk/kernel/
$ export CROSS_COMPILE=../prebuilts/gcc/linux-x86/arm/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-
$ make ARCH=arm rk3506_defconfig rockchip_rt.config
$ make ARCH=arm rk3506g-evb1-v10.img -j8
```

> 备注：32位内核以RK3506G EVB1 为例，其它芯片平台编译内核，在原配置的基础上，加上rockchip_rt.config 实时性配置。

###   烧录boot.img 并测试实时性性能

   使用cyclictest测试

```bash
$ cyclictest -m -c 0 -p99 -t -D 12h
```

## XENOMAI

Buildroot需要更新，且包含如下补丁：

```shell
commit 4bd33add016f393c8ed62fca0ace075755465928
Author: ZhiZhan Chen <zhizhan.chen@rock-chips.com>
Date:   Wed Jul 19 20:03:59 2023 +0800

    xenomai: add rockchip support

    Fix compilation errors with clang version 12.0.5

    Change-Id: Ib5f7971495f339abce2613a1d6d6d0cbfce35b37
    Signed-off-by: Liang Chen <cl@rock-c 9hips.com>
```

> 注意：Kernel-6.1 的内核，Buildroot需要打上0001-xenomai-Support-3.2.4.patch，该补丁有分Buildroot-2021和Buildroot-2023版本，请根据所用Buildroot的版本选择对应的补丁。

### 内核打上补丁

根据当前内核版本，选择对应的内核版本补丁。

###   Buildroot打开XENOMAI配置，并编译rootfs.img：

```bash
BR2_PACKAGE_XENOMAI=y
BR2_PACKAGE_XENOMAI_3_2=y
BR2_PACKAGE_XENOMAI_VERSION="v3.2.2"
BR2_PACKAGE_XENOMAI_COBALT=y
BR2_PACKAGE_XENOMAI_TESTSUITE=y
BR2_PACKAGE_XENOMAI_ADDITIONAL_CONF_OPTS="--enable-demo"
```

> 注：Kernel6.1版本，XENOMAI使用v3.2.4版本, Buildroot需要包含0001-xenomai-Support-3.2.4.patch。
>

###   把xenomai系统打到内核上：

```bash
$ cd $sdk/kernel
$ ../buildroot/output/rockchip_rk3588/build/xenomai-v3.2.2/scripts/prepare-kernel.sh --arch=arm64
```

###   编译内核

编译命令：

kernel 6.1(以RK3506为例)：

```bash
$ cd $sdk/kernel/
$ export CROSS_COMPILE=../prebuilts/gcc/linux-x86/arm/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-
$ make ARCH=arm rk3506_defconfig
$ make ARCH=arm rk3506g-evb1-v10.img -j8
```

kernel 5.10(以RK3588为例)：

```bash
$ cd $sdk/kernel
$ export CROSS_COMPILE=../prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
$ make ARCH=arm64 rockchip_linux_defconfig rk3588_linux.config
$ make ARCH=arm64 LT0=none LLVM=1 LLVM_IAS=1 rk3588-evb1-lp4-v10-linux.img -j17
```

kernel 4.19(以RK3568为例)：

```bash
$ cd $sdk/kernel
$ export CROSS_COMPILE=../prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
$ make ARCH=arm64 rockchip_linux_defconfig
$ make ARCH=arm64 rk3568-evb1-ddr4-v10-linux.img -j17
```
### 烧录boot.img  rootfs.img

###   测试实时性能

#### 校准latency

```bash
 $ echo 0 > /proc/xenomai/latency
```

#### 使用cyclictest测试

```bash
 $ ./usr/demo/cyclictest -m -n -c 0 -p99 -t -D 12h
```

## 注意事项

### RK3568 需要使用RT版本的BL31,实时性能更好

rkbin需要更新到最新，且包含这个补丁：

```shell
commit c2df62ac1758a21cff946ea5d39a77a769b2052e (HEAD -> master, origin/master, origin/HEAD)
Author: Liang Chen <cl@rock-chips.com>
Date:   Thu Nov 2 16:33:14 2023 +0800

    rk3568: bl31 rt: update version to v1.02

    build from:
            30c17915b rk3568: optimize RT latency

    update feature:
            30c17915b rk3568: optimize RT latency
            4a7bee092 plat: rk3588: otp: support to read secure otp
            ...
            e7c694291 plat: rk3568: get l3 partition parameter from tags by default
            ...

            patch on gerrit: I05955dace13ec323d894583e664c128e8b582fe8 (Change-Id)

    Change-Id: I6a74ccc547624837872fbe930fec4c76a9012776
    Signed-off-by: Liang Chen <cl@rock-chips.com>
```

编译命令：

```shell
$ cd $sdk/uboot
$ ./make.sh rk3568-rt
```

烧录miniloader.bin 以及uboot.img。

开机过程会有cache_write_streaming_cfg相关打印，说明已经使用rt版本的bl31。

```shell
INFO:    Preloader serial: 2
NOTICE:  BL31: v2.3():v2.3-662-g30c17915b-dirty:cl, fwver: v1.02
NOTICE:  BL31: Built : 16:39:01, Nov  2 2023
INFO:    GICv3 without legacy support detected.
INFO:    ARM GICv3 driver initialized in EL3
INFO:    pmu v1 is valid 220114
INFO:    cache_write_streaming_cfg:0 2808bc00 PCTL:L3-7 L1-5 WSCTL:L1-0 L3-1
INFO:    cache_write_streaming_cfg:0 2808e400 PCTL:L3-1 L1-7 WSCTL:L1-0 L3-1
INFO:    l3 cache partition cfg-8421
INFO:    dfs DDR fsp_param[0].freq_mhz= 1560MHz
INFO:    dfs DDR fsp_param[1].freq_mhz= 324MHz
INFO:    dfs DDR fsp_param[2].freq_mhz= 528MHz
INFO:    dfs DDR fsp_param[3].freq_mhz= 780MHz
INFO:    Using opteed sec cpu_context!
INFO:    boot cpu mask: 0
INFO:    BL31: Initializing runtime services
INFO:    BL31: Initializing BL32
```

### RK3568 提高实时性的做法

#### 		cache 分片

ARM Cortex-A55 架构上面支持对L3空间进行划分，原理为：Cortex-A55 L3 mem空间划分为4块，可以配

置每个CPU使用4块L3中的哪几块，在rkbin中的RKBOOT/RK3568MINIALL.ini文件进行配置：

```c
[BOOT1_PARAM]

WORD_3=0xcc33
```

WORD_3的值0xcc33表示（以P0、P1、P2、P3表示L3的4块空间）：

cpu0、cpu1 共享L3的P0、P1。

cpu2、cpu3 共享L3的P2、P3。

WORD_3配置值，详细说明如下 ：

bit0~bit3：分配给cpu0的4份L3的mask bit， bit0为1，表示L3的第一份分给cpu0，bit1为1，表示L3的

第二份分给cpu0，以此类推。

bit4~bit7：分配给cpu1的4份L3的mask bit。

bit8~bit11：分配给cpu2的4份L3的mask bit。

bit12~bit15：分配给cpu3的4份L3的mask bit。

配置后可以通过下面开机LOG确认

```shell
INFO: L3 cache partition cfg-cc33
```

#### 		隔离核心

bootargs添加 `isolcpus=3 nohz_full=3` ，将核心cpu3隔离出来，不参与系统任务调度，并作为实时核心。

```shell
diff --git a/arch/arm64/boot/dts/rockchip/rk3568-linux.dtsi b/arch/arm64/boot/dts/rockchip/rk3568-linux.dtsi
index c7e309645099b..28fac4880744d 100644
--- a/arch/arm64/boot/dts/rockchip/rk3568-linux.dtsi
+++ b/arch/arm64/boot/dts/rockchip/rk3568-linux.dtsi
@@ -13,7 +13,7 @@ aliases {
        };
        chosen: chosen {
-               bootargs = "earlycon=uart8250,mmio32,0xfe660000 console=ttyFIQ0 root=PARTUUID=614e0000-0000 rw rootwait";
+               bootargs = "earlycon=uart8250,mmio32,0xfe660000 isolcpus=3 nohz_full=3 console=ttyFIQ0 root=PARTUUID=614e0000-0000 rw rootwait";
        };
        fiq-debugger {
```

#### 		实时任务绑定到实时核上运行

将cyclitest绑定到cpu3上运行，测试实时性能。

```shell
taskset -c 3 cyclictest -c0 -m -t -p99  -D 12h
```

> 注：`ps  -eo pid,psr,comm | grep cyclictest` 可以查看cyclitest是否绑定在cpu3。
>
