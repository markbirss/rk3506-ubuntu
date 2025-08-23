/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#ifndef __RV1103B_H
#define __RV1103B_H
#ifdef __cplusplus
  extern "C" {
#endif
/****************************************************************************************/
/*                                                                                      */
/*                               Module Structure Section                               */
/*                                                                                      */
/****************************************************************************************/
#ifndef __ASSEMBLY__
/* PERICRU Register Structure Define */
struct PERICRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t PERICLKSEL_CON[5];                  /* Address Offset: 0x0300 */
         uint32_t RESERVED0314[315];                  /* Address Offset: 0x0314 */
    __IO uint32_t PERIGATE_CON[12];                   /* Address Offset: 0x0800 */
         uint32_t RESERVED0830[116];                  /* Address Offset: 0x0830 */
    __IO uint32_t PERISOFTRST_CON[12];                /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A30[118];                  /* Address Offset: 0x0A30 */
    __IO uint32_t PERICRU_IP_CON;                     /* Address Offset: 0x0C08 */
};
/* VEPUCRU Register Structure Define */
struct VEPUCRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t VEPUCLKSEL_CON[3];                  /* Address Offset: 0x0300 */
    __IO uint32_t VEPUCLKSEL_CON02;                   /* Address Offset: 0x0308 */
         uint32_t RESERVED030C[317];                  /* Address Offset: 0x030C */
    __IO uint32_t VEPUGATE_CON[1];                    /* Address Offset: 0x0800 */
         uint32_t RESERVED0804[127];                  /* Address Offset: 0x0804 */
    __IO uint32_t VEPUSOFTRST_CON[1];                 /* Address Offset: 0x0A00 */
};
/* NPUCRU Register Structure Define */
struct NPUCRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t NPUCLKSEL_CON[3];                   /* Address Offset: 0x0300 */
         uint32_t RESERVED030C[317];                  /* Address Offset: 0x030C */
    __IO uint32_t NPUGATE_CON[1];                     /* Address Offset: 0x0800 */
         uint32_t RESERVED0804[127];                  /* Address Offset: 0x0804 */
    __IO uint32_t NPUSOFTRST_CON[1];                  /* Address Offset: 0x0A00 */
};
/* VICRU Register Structure Define */
struct VICRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t VICLKSEL_CON[1];                    /* Address Offset: 0x0300 */
         uint32_t RESERVED0304[319];                  /* Address Offset: 0x0304 */
    __IO uint32_t VIGATE_CON[3];                      /* Address Offset: 0x0800 */
         uint32_t RESERVED080C[125];                  /* Address Offset: 0x080C */
    __IO uint32_t VISOFTRST_CON[3];                   /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A0C[127];                  /* Address Offset: 0x0A0C */
    __IO uint32_t VICRU_IP_CON;                       /* Address Offset: 0x0C08 */
};
/* CORECRU Register Structure Define */
struct CORECRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t CORECLKSEL_CON[3];                  /* Address Offset: 0x0300 */
         uint32_t RESERVED030C[317];                  /* Address Offset: 0x030C */
    __IO uint32_t COREGATE_CON[2];                    /* Address Offset: 0x0800 */
         uint32_t RESERVED0808[126];                  /* Address Offset: 0x0808 */
    __IO uint32_t CORESOFTRST_CON[2];                 /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A08[190];                  /* Address Offset: 0x0A08 */
    __IO uint32_t AUTOCS_CORE_SRC_CON[2];             /* Address Offset: 0x0D00 */
};
/* DDRCRU Register Structure Define */
struct DDRCRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t DDRCLKSEL_CON[1];                   /* Address Offset: 0x0300 */
         uint32_t RESERVED0304[319];                  /* Address Offset: 0x0304 */
    __IO uint32_t DDRGATE_CON[1];                     /* Address Offset: 0x0800 */
         uint32_t RESERVED0804[127];                  /* Address Offset: 0x0804 */
    __IO uint32_t DDRSOFTRST_CON[1];                  /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A04[191];                  /* Address Offset: 0x0A04 */
    __IO uint32_t AUTOCS_LSCLK_DDR_ROOT_CON[2];       /* Address Offset: 0x0D00 */
};
/* SUBDDRCRU Register Structure Define */
struct SUBDDRCRU_REG {
         uint32_t RESERVED0000[512];                  /* Address Offset: 0x0000 */
    __IO uint32_t SUBDDRGATE_CON[1];                  /* Address Offset: 0x0800 */
         uint32_t RESERVED0804[127];                  /* Address Offset: 0x0804 */
    __IO uint32_t SUBDDRSOFTRST_CON[1];               /* Address Offset: 0x0A00 */
};
/* TOPCRU Register Structure Define */
struct TOPCRU_REG {
         uint32_t RESERVED0000[16];                   /* Address Offset: 0x0000 */
    __IO uint32_t DPLL_CON[5];                        /* Address Offset: 0x0040 */
         uint32_t RESERVED0054[3];                    /* Address Offset: 0x0054 */
    __IO uint32_t GPLL_CON[5];                        /* Address Offset: 0x0060 */
         uint32_t RESERVED0074[51];                   /* Address Offset: 0x0074 */
    __IO uint32_t CRU_SSCGTBL_CON00;                  /* Address Offset: 0x0140 */
    __IO uint32_t CRU_SSCGTBL_CON01;                  /* Address Offset: 0x0144 */
    __IO uint32_t CRU_SSCGTBL_CON02;                  /* Address Offset: 0x0148 */
    __IO uint32_t CRU_SSCGTBL_CON03;                  /* Address Offset: 0x014C */
    __IO uint32_t CRU_SSCGTBL_CON04;                  /* Address Offset: 0x0150 */
    __IO uint32_t CRU_SSCGTBL_CON05;                  /* Address Offset: 0x0154 */
    __IO uint32_t CRU_SSCGTBL_CON06;                  /* Address Offset: 0x0158 */
    __IO uint32_t CRU_SSCGTBL_CON07;                  /* Address Offset: 0x015C */
    __IO uint32_t CRU_SSCGTBL_CON08;                  /* Address Offset: 0x0160 */
    __IO uint32_t CRU_SSCGTBL_CON09;                  /* Address Offset: 0x0164 */
    __IO uint32_t CRU_SSCGTBL_CON10;                  /* Address Offset: 0x0168 */
    __IO uint32_t CRU_SSCGTBL_CON11;                  /* Address Offset: 0x016C */
    __IO uint32_t CRU_SSCGTBL_CON12;                  /* Address Offset: 0x0170 */
    __IO uint32_t CRU_SSCGTBL_CON13;                  /* Address Offset: 0x0174 */
    __IO uint32_t CRU_SSCGTBL_CON14;                  /* Address Offset: 0x0178 */
    __IO uint32_t CRU_SSCGTBL_CON15;                  /* Address Offset: 0x017C */
    __IO uint32_t CRU_SSCGTBL_CON16;                  /* Address Offset: 0x0180 */
    __IO uint32_t CRU_SSCGTBL_CON17;                  /* Address Offset: 0x0184 */
    __IO uint32_t CRU_SSCGTBL_CON18;                  /* Address Offset: 0x0188 */
    __IO uint32_t CRU_SSCGTBL_CON19;                  /* Address Offset: 0x018C */
    __IO uint32_t CRU_SSCGTBL_CON20;                  /* Address Offset: 0x0190 */
    __IO uint32_t CRU_SSCGTBL_CON21;                  /* Address Offset: 0x0194 */
    __IO uint32_t CRU_SSCGTBL_CON22;                  /* Address Offset: 0x0198 */
    __IO uint32_t CRU_SSCGTBL_CON23;                  /* Address Offset: 0x019C */
    __IO uint32_t CRU_SSCGTBL_CON24;                  /* Address Offset: 0x01A0 */
    __IO uint32_t CRU_SSCGTBL_CON25;                  /* Address Offset: 0x01A4 */
    __IO uint32_t CRU_SSCGTBL_CON26;                  /* Address Offset: 0x01A8 */
    __IO uint32_t CRU_SSCGTBL_CON27;                  /* Address Offset: 0x01AC */
    __IO uint32_t CRU_SSCGTBL_CON28;                  /* Address Offset: 0x01B0 */
    __IO uint32_t CRU_SSCGTBL_CON29;                  /* Address Offset: 0x01B4 */
    __IO uint32_t CRU_SSCGTBL_CON30;                  /* Address Offset: 0x01B8 */
    __IO uint32_t CRU_SSCGTBL_CON31;                  /* Address Offset: 0x01BC */
         uint32_t RESERVED01C0[48];                   /* Address Offset: 0x01C0 */
    __IO uint32_t MODE_CON00;                         /* Address Offset: 0x0280 */
         uint32_t RESERVED0284[31];                   /* Address Offset: 0x0284 */
    __IO uint32_t CRU_CLKSEL_CON[42];                 /* Address Offset: 0x0300 */
         uint32_t RESERVED03A8[278];                  /* Address Offset: 0x03A8 */
    __IO uint32_t CRU_CLKGATE_CON[7];                 /* Address Offset: 0x0800 */
         uint32_t RESERVED081C[121];                  /* Address Offset: 0x081C */
    __IO uint32_t CRU_SOFTRST_CON[1];                 /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A04[127];                  /* Address Offset: 0x0A04 */
    __IO uint32_t CRU_GLB_CNT_TH;                     /* Address Offset: 0x0C00 */
    __IO uint32_t CRU_GLBRST_ST;                      /* Address Offset: 0x0C04 */
    __IO uint32_t CRU_GLB_SRST_FST_VALUE;             /* Address Offset: 0x0C08 */
    __IO uint32_t CRU_GLB_SRST_SND_VALUE;             /* Address Offset: 0x0C0C */
    __IO uint32_t CRU_GLB_RST_CON;                    /* Address Offset: 0x0C10 */
    __I  uint32_t CRU_GLB_RST_ST_NCLR;                /* Address Offset: 0x0C14 */
         uint32_t RESERVED0C18[42];                   /* Address Offset: 0x0C18 */
    __IO uint32_t CLK_SAI_FRAC_DIV_HIGH;              /* Address Offset: 0x0CC0 */
         uint32_t RESERVED0CC4[15];                   /* Address Offset: 0x0CC4 */
    __IO uint32_t AUTOCS_ACLK_PERI_SRC_CON[2];        /* Address Offset: 0x0D00 */
    __IO uint32_t AUTOCS_HCLK_HPMCU_SRC_CON[2];       /* Address Offset: 0x0D08 */
    __IO uint32_t AUTOCS_LSCLK_PERI_SRC_CON[2];       /* Address Offset: 0x0D10 */
    __IO uint32_t AUTOCS_CON_ACLK_PERI_SRC;           /* Address Offset: 0x0D18 */
    __IO uint32_t AUTOCS_CON_HCLK_HPMCU_SRC;          /* Address Offset: 0x0D1C */
    __IO uint32_t AUTOCS_CON_LSCLK_PERI_SRC;          /* Address Offset: 0x0D20 */
};
/* PMU0CRU Register Structure Define */
struct PMU0CRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t PMUCLKSEL_CON[3];                   /* Address Offset: 0x0300 */
         uint32_t RESERVED030C[317];                  /* Address Offset: 0x030C */
    __IO uint32_t PMUGATE_CON[3];                     /* Address Offset: 0x0800 */
         uint32_t RESERVED080C[125];                  /* Address Offset: 0x080C */
    __IO uint32_t PMUSOFTRST_CON[3];                  /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A0C[189];                  /* Address Offset: 0x0A0C */
    __IO uint32_t AUTOCS_LSCLK_PMU_ROOT_CON[2];       /* Address Offset: 0x0D00 */
};
/* PMU1CRU Register Structure Define */
struct PMU1CRU_REG {
         uint32_t RESERVED0000[192];                  /* Address Offset: 0x0000 */
    __IO uint32_t PMU1CLKSEL_CON[1];                  /* Address Offset: 0x0300 */
         uint32_t RESERVED0304[319];                  /* Address Offset: 0x0304 */
    __IO uint32_t PMU1GATE_CON[2];                    /* Address Offset: 0x0800 */
         uint32_t RESERVED0808[126];                  /* Address Offset: 0x0808 */
    __IO uint32_t PMU1SOFTRST_CON[2];                 /* Address Offset: 0x0A00 */
         uint32_t RESERVED0A08[128];                  /* Address Offset: 0x0A08 */
    __IO uint32_t PMU1CRU_IP_CON;                     /* Address Offset: 0x0C08 */
};
/* GRF_VEPU Register Structure Define */
struct GRF_VEPU_REG {
    __IO uint32_t MEM_CON_SPRA;                       /* Address Offset: 0x0000 */
    __IO uint32_t MEM_CON_DPRA;                       /* Address Offset: 0x0004 */
};
/* GRF_NPU Register Structure Define */
struct GRF_NPU_REG {
    __IO uint32_t MEM_CON_SPRA;                       /* Address Offset: 0x0000 */
};
/* GRF_VI Register Structure Define */
struct GRF_VI_REG {
    __IO uint32_t MEM_CON_SPRA;                       /* Address Offset: 0x0000 */
    __IO uint32_t MEM_CON_DPRA;                       /* Address Offset: 0x0004 */
         uint32_t RESERVED0008;                       /* Address Offset: 0x0008 */
    __IO uint32_t VI_HPROT2_CON;                      /* Address Offset: 0x000C */
    __IO uint32_t STATUS;                             /* Address Offset: 0x0010 */
    __IO uint32_t CSIPHY_CON;                         /* Address Offset: 0x0014 */
    __IO uint32_t CSIPHY_STATUS;                      /* Address Offset: 0x0018 */
         uint32_t RESERVED001C;                       /* Address Offset: 0x001C */
    __IO uint32_t MISC_CON;                           /* Address Offset: 0x0020 */
    __IO uint32_t SDMMC_DET_CNT;                      /* Address Offset: 0x0024 */
    __IO uint32_t SDMMC_SIG_DETECT_CON;               /* Address Offset: 0x0028 */
    __IO uint32_t SDMMC_SIG_DETECT_STATUS;            /* Address Offset: 0x002C */
    __IO uint32_t SDMMC_STATUS_CLR;                   /* Address Offset: 0x0030 */
};
/* GRF_CPU Register Structure Define */
struct GRF_CPU_REG {
    __IO uint32_t CON0;                               /* Address Offset: 0x0000 */
    __IO uint32_t MEM_CFG_UHDSPRA;                    /* Address Offset: 0x0004 */
    __IO uint32_t STATUS;                             /* Address Offset: 0x0008 */
};
/* GRF_DDR Register Structure Define */
struct GRF_DDR_REG {
    __IO uint32_t CON0;                               /* Address Offset: 0x0000 */
    __IO uint32_t CON1;                               /* Address Offset: 0x0004 */
    __IO uint32_t CON2;                               /* Address Offset: 0x0008 */
    __IO uint32_t CON3;                               /* Address Offset: 0x000C */
    __IO uint32_t CON4;                               /* Address Offset: 0x0010 */
    __IO uint32_t CON5;                               /* Address Offset: 0x0014 */
    __IO uint32_t CON6;                               /* Address Offset: 0x0018 */
    __IO uint32_t CON7;                               /* Address Offset: 0x001C */
    __IO uint32_t CON8;                               /* Address Offset: 0x0020 */
    __IO uint32_t CON9;                               /* Address Offset: 0x0024 */
    __IO uint32_t CON10;                              /* Address Offset: 0x0028 */
    __IO uint32_t CON11;                              /* Address Offset: 0x002C */
    __IO uint32_t CON12;                              /* Address Offset: 0x0030 */
    __IO uint32_t CON13;                              /* Address Offset: 0x0034 */
    __IO uint32_t CON14;                              /* Address Offset: 0x0038 */
         uint32_t RESERVED003C[17];                   /* Address Offset: 0x003C */
    __IO uint32_t PROBE_CTRL;                         /* Address Offset: 0x0080 */
         uint32_t RESERVED0084[39];                   /* Address Offset: 0x0084 */
    __I  uint32_t STATUS8;                            /* Address Offset: 0x0120 */
    __I  uint32_t STATUS9;                            /* Address Offset: 0x0124 */
};
/* GRF_SYS Register Structure Define */
struct GRF_SYS_REG {
    __IO uint32_t PERI_CON0;                          /* Address Offset: 0x0000 */
    __IO uint32_t PERI_CON1;                          /* Address Offset: 0x0004 */
    __IO uint32_t PERI_CON2;                          /* Address Offset: 0x0008 */
    __IO uint32_t PERI_HPROT2_CON;                    /* Address Offset: 0x000C */
    __IO uint32_t PERI_STATUS;                        /* Address Offset: 0x0010 */
         uint32_t RESERVED0014[3];                    /* Address Offset: 0x0014 */
    __IO uint32_t AUDIO_CON0;                         /* Address Offset: 0x0020 */
    __IO uint32_t AUDIO_CON1;                         /* Address Offset: 0x0024 */
         uint32_t RESERVED0028[2];                    /* Address Offset: 0x0028 */
    __IO uint32_t USBOTG_CON0;                        /* Address Offset: 0x0030 */
    __IO uint32_t USBOTG_CON1;                        /* Address Offset: 0x0034 */
         uint32_t RESERVED0038[2];                    /* Address Offset: 0x0038 */
    __I  uint32_t USBOTG_STATUS0;                     /* Address Offset: 0x0040 */
    __I  uint32_t USBOTG_STATUS1;                     /* Address Offset: 0x0044 */
    __I  uint32_t USBOTG_STATUS2;                     /* Address Offset: 0x0048 */
         uint32_t RESERVED004C;                       /* Address Offset: 0x004C */
    __IO uint32_t USBPHY_CON0;                        /* Address Offset: 0x0050 */
    __IO uint32_t USBPHY_CON1;                        /* Address Offset: 0x0054 */
    __IO uint32_t USBPHY_CON2;                        /* Address Offset: 0x0058 */
    __IO uint32_t USBPHY_CON3;                        /* Address Offset: 0x005C */
    __I  uint32_t USBPHY_STATUS;                      /* Address Offset: 0x0060 */
         uint32_t RESERVED0064[3];                    /* Address Offset: 0x0064 */
    __IO uint32_t SARADC_CON;                         /* Address Offset: 0x0070 */
    __IO uint32_t TSADC_CON;                          /* Address Offset: 0x0074 */
    __IO uint32_t OTP_CON;                            /* Address Offset: 0x0078 */
         uint32_t RESERVED007C;                       /* Address Offset: 0x007C */
    __IO uint32_t MEM_CON_SPRA;                       /* Address Offset: 0x0080 */
    __IO uint32_t MEM_CON_DPRA;                       /* Address Offset: 0x0084 */
    __IO uint32_t MEM_CON_ROM;                        /* Address Offset: 0x0088 */
    __IO uint32_t MEM_CON_GATE;                       /* Address Offset: 0x008C */
    __IO uint32_t BIU_CON0;                           /* Address Offset: 0x0090 */
         uint32_t RESERVED0094;                       /* Address Offset: 0x0094 */
    __I  uint32_t BIU_STATUS0;                        /* Address Offset: 0x0098 */
    __I  uint32_t BIU_STATUS1;                        /* Address Offset: 0x009C */
    __IO uint32_t GMAC_CON0;                          /* Address Offset: 0x00A0 */
    __IO uint32_t GMAC_CLK_CON;                       /* Address Offset: 0x00A4 */
    __I  uint32_t GMAC_ST;                            /* Address Offset: 0x00A8 */
         uint32_t RESERVED00AC;                       /* Address Offset: 0x00AC */
    __IO uint32_t MACPHY_CON0;                        /* Address Offset: 0x00B0 */
    __IO uint32_t MACPHY_CON1;                        /* Address Offset: 0x00B4 */
         uint32_t RESERVED00B8[18];                   /* Address Offset: 0x00B8 */
    __IO uint32_t USBOTG_SIG_DETECT_CON;              /* Address Offset: 0x0100 */
    __I  uint32_t USBOTG_SIG_DETECT_STATUS;           /* Address Offset: 0x0104 */
    __IO uint32_t USBOTG_SIG_DETECT_CLR;              /* Address Offset: 0x0108 */
         uint32_t RESERVED010C;                       /* Address Offset: 0x010C */
    __IO uint32_t USBOTG_LINESTATE_DETECT_CON;        /* Address Offset: 0x0110 */
    __IO uint32_t USBOTG_DISCONNECT_DETECT_CON;       /* Address Offset: 0x0114 */
    __IO uint32_t USBOTG_BVALID_DETECT_CON;           /* Address Offset: 0x0118 */
    __IO uint32_t USBOTG_ID_DETECT_CON;               /* Address Offset: 0x011C */
         uint32_t RESERVED0120[56];                   /* Address Offset: 0x0120 */
    __IO uint32_t CACHE_PERI_ADDR_START;              /* Address Offset: 0x0200 */
    __IO uint32_t CACHE_PERI_ADDR_END;                /* Address Offset: 0x0204 */
    __IO uint32_t HPMCU_CODE_ADDR_START;              /* Address Offset: 0x0208 */
    __IO uint32_t HPMCU_SRAM_ADDR_START;              /* Address Offset: 0x020C */
    __IO uint32_t HPMCU_EXSRAM_ADDR_START;            /* Address Offset: 0x0210 */
    __IO uint32_t HPMCU_CACHE_MISC;                   /* Address Offset: 0x0214 */
    __IO uint32_t HPMCU_CACHE_STATUS;                 /* Address Offset: 0x0218 */
         uint32_t RESERVED021C[377];                  /* Address Offset: 0x021C */
    __IO uint32_t CHIP_ID;                            /* Address Offset: 0x0800 */
    __IO uint32_t CHIP_VERSION;                       /* Address Offset: 0x0804 */
};
/* GRF_PMU Register Structure Define */
struct GRF_PMU_REG {
    __IO uint32_t SOC_CON0;                           /* Address Offset: 0x0000 */
    __IO uint32_t SOC_CON1;                           /* Address Offset: 0x0004 */
    __IO uint32_t SOC_CON2;                           /* Address Offset: 0x0008 */
    __IO uint32_t SOC_CON3;                           /* Address Offset: 0x000C */
    __IO uint32_t SOC_CON4;                           /* Address Offset: 0x0010 */
    __IO uint32_t SOC_CON5;                           /* Address Offset: 0x0014 */
    __IO uint32_t SOC_CON6;                           /* Address Offset: 0x0018 */
    __IO uint32_t SOC_CON7;                           /* Address Offset: 0x001C */
    __IO uint32_t SOC_CON8;                           /* Address Offset: 0x0020 */
    __IO uint32_t SOC_CON9;                           /* Address Offset: 0x0024 */
    __IO uint32_t SOC_CON10;                          /* Address Offset: 0x0028 */
         uint32_t RESERVED002C;                       /* Address Offset: 0x002C */
    __I  uint32_t SOC_STATUS0;                        /* Address Offset: 0x0030 */
         uint32_t RESERVED0034[3];                    /* Address Offset: 0x0034 */
    __IO uint32_t MEN_CON;                            /* Address Offset: 0x0040 */
         uint32_t RESERVED0044[3];                    /* Address Offset: 0x0044 */
    __IO uint32_t SOC_SPECIAL0;                       /* Address Offset: 0x0050 */
         uint32_t RESERVED0054[3];                    /* Address Offset: 0x0054 */
    __IO uint32_t SOC_PREROLL_INT_CON;                /* Address Offset: 0x0060 */
         uint32_t RESERVED0064[103];                  /* Address Offset: 0x0064 */
    __IO uint32_t OS_REG0;                            /* Address Offset: 0x0200 */
    __IO uint32_t OS_REG1;                            /* Address Offset: 0x0204 */
    __IO uint32_t OS_REG2;                            /* Address Offset: 0x0208 */
    __IO uint32_t OS_REG3;                            /* Address Offset: 0x020C */
    __IO uint32_t OS_REG4;                            /* Address Offset: 0x0210 */
    __IO uint32_t OS_REG5;                            /* Address Offset: 0x0214 */
    __IO uint32_t OS_REG6;                            /* Address Offset: 0x0218 */
    __IO uint32_t OS_REG7;                            /* Address Offset: 0x021C */
    __IO uint32_t OS_REG8;                            /* Address Offset: 0x0220 */
    __IO uint32_t OS_REG9;                            /* Address Offset: 0x0224 */
    __IO uint32_t OS_REG10;                           /* Address Offset: 0x0228 */
    __IO uint32_t OS_REG11;                           /* Address Offset: 0x022C */
    __I  uint32_t RESET_FUNCTION_STATUS;              /* Address Offset: 0x0230 */
    __O  uint32_t RESET_FUNCTION_CLR;                 /* Address Offset: 0x0234 */
};
/* GPIO1_IOC Register Structure Define */
struct GPIO1_IOC_REG {
         uint32_t RESERVED0000[8];                    /* Address Offset: 0x0000 */
    __IO uint32_t GPIO1A_IOMUX_SEL_0;                 /* Address Offset: 0x0020 */
    __IO uint32_t GPIO1A_IOMUX_SEL_1_0;               /* Address Offset: 0x0024 */
         uint32_t RESERVED0028[70];                   /* Address Offset: 0x0028 */
    __IO uint32_t GPIO1A_DS_0;                        /* Address Offset: 0x0140 */
    __IO uint32_t GPIO1A_DS_1;                        /* Address Offset: 0x0144 */
    __IO uint32_t GPIO1A_DS_2;                        /* Address Offset: 0x0148 */
         uint32_t RESERVED014C[49];                   /* Address Offset: 0x014C */
    __IO uint32_t GPIO1A_PULL_0;                      /* Address Offset: 0x0210 */
         uint32_t RESERVED0214[63];                   /* Address Offset: 0x0214 */
    __IO uint32_t GPIO1A_IE_0;                        /* Address Offset: 0x0310 */
         uint32_t RESERVED0314[63];                   /* Address Offset: 0x0314 */
    __IO uint32_t GPIO1A_SMT_0;                       /* Address Offset: 0x0410 */
         uint32_t RESERVED0414[63];                   /* Address Offset: 0x0414 */
    __IO uint32_t GPIO1A_SUS_0;                       /* Address Offset: 0x0510 */
         uint32_t RESERVED0514[63];                   /* Address Offset: 0x0514 */
    __IO uint32_t GPIO1A_SL_0;                        /* Address Offset: 0x0610 */
         uint32_t RESERVED0614[63];                   /* Address Offset: 0x0614 */
    __IO uint32_t GPIO1A_OD_0;                        /* Address Offset: 0x0710 */
         uint32_t RESERVED0714[59];                   /* Address Offset: 0x0714 */
    __IO uint32_t IO_VSEL_VCCIO3;                     /* Address Offset: 0x0800 */
         uint32_t RESERVED0804[15880];                /* Address Offset: 0x0804 */
    __IO uint32_t GPIO1A_IOMUX_SEL_1_1;               /* Address Offset: 0x10024 */
    __IO uint32_t GPIO1B_IOMUX_SEL_0;                 /* Address Offset: 0x10028 */
    __IO uint32_t GPIO1B_IOMUX_SEL_1;                 /* Address Offset: 0x1002C */
    __IO uint32_t GPIO1C_IOMUX_SEL_0;                 /* Address Offset: 0x10030 */
         uint32_t RESERVED10034[70];                  /* Address Offset: 0x10034 */
    __IO uint32_t GPIO1A_DS_3;                        /* Address Offset: 0x1014C */
    __IO uint32_t GPIO1B_DS_0;                        /* Address Offset: 0x10150 */
    __IO uint32_t GPIO1B_DS_1;                        /* Address Offset: 0x10154 */
    __IO uint32_t GPIO1B_DS_2;                        /* Address Offset: 0x10158 */
    __IO uint32_t GPIO1B_DS_3;                        /* Address Offset: 0x1015C */
    __IO uint32_t GPIO1C_DS_0;                        /* Address Offset: 0x10160 */
         uint32_t RESERVED10164[43];                  /* Address Offset: 0x10164 */
    __IO uint32_t GPIO1A_PULL_1;                      /* Address Offset: 0x10210 */
    __IO uint32_t GPIO1B_PULL;                        /* Address Offset: 0x10214 */
    __IO uint32_t GPIO1C_PULL;                        /* Address Offset: 0x10218 */
         uint32_t RESERVED1021C[61];                  /* Address Offset: 0x1021C */
    __IO uint32_t GPIO1A_IE_1;                        /* Address Offset: 0x10310 */
    __IO uint32_t GPIO1B_IE;                          /* Address Offset: 0x10314 */
    __IO uint32_t GPIO1C_IE;                          /* Address Offset: 0x10318 */
         uint32_t RESERVED1031C[61];                  /* Address Offset: 0x1031C */
    __IO uint32_t GPIO1A_SMT_1;                       /* Address Offset: 0x10410 */
    __IO uint32_t GPIO1B_SMT;                         /* Address Offset: 0x10414 */
    __IO uint32_t GPIO1C_SMT;                         /* Address Offset: 0x10418 */
         uint32_t RESERVED1041C[61];                  /* Address Offset: 0x1041C */
    __IO uint32_t GPIO1A_SUS_1;                       /* Address Offset: 0x10510 */
    __IO uint32_t GPIO1B_SUS;                         /* Address Offset: 0x10514 */
    __IO uint32_t GPIO1C_SUS;                         /* Address Offset: 0x10518 */
         uint32_t RESERVED1051C[61];                  /* Address Offset: 0x1051C */
    __IO uint32_t GPIO1A_SL_1;                        /* Address Offset: 0x10610 */
    __IO uint32_t GPIO1B_SL;                          /* Address Offset: 0x10614 */
    __IO uint32_t GPIO1C_SL;                          /* Address Offset: 0x10618 */
         uint32_t RESERVED1061C[61];                  /* Address Offset: 0x1061C */
    __IO uint32_t GPIO1A_OD_1;                        /* Address Offset: 0x10710 */
    __IO uint32_t GPIO1B_OD;                          /* Address Offset: 0x10714 */
    __IO uint32_t GPIO1C_OD;                          /* Address Offset: 0x10718 */
         uint32_t RESERVED1071C[57];                  /* Address Offset: 0x1071C */
    __IO uint32_t IO_VSEL_VCCIO4;                     /* Address Offset: 0x10800 */
    __IO uint32_t IO_VSEL_VCCIO7;                     /* Address Offset: 0x10804 */
    __IO uint32_t MISC_CON;                           /* Address Offset: 0x10808 */
    __IO uint32_t SDCARD_IO_CON;                      /* Address Offset: 0x1080C */
    __IO uint32_t JTAG_M2_CON;                        /* Address Offset: 0x10810 */
};
/* GPIO2_IOC Register Structure Define */
struct GPIO2_IOC_REG {
         uint32_t RESERVED0000[16];                   /* Address Offset: 0x0000 */
    __IO uint32_t GPIO2A_IOMUX_SEL_0;                 /* Address Offset: 0x0040 */
    __IO uint32_t GPIO2A_IOMUX_SEL_1;                 /* Address Offset: 0x0044 */
    __IO uint32_t GPIO2B_IOMUX_SEL_0;                 /* Address Offset: 0x0048 */
         uint32_t RESERVED004C[77];                   /* Address Offset: 0x004C */
    __IO uint32_t GPIO2A_DS_0;                        /* Address Offset: 0x0180 */
    __IO uint32_t GPIO2A_DS_1;                        /* Address Offset: 0x0184 */
    __IO uint32_t GPIO2A_DS_2;                        /* Address Offset: 0x0188 */
    __IO uint32_t GPIO2A_DS_3;                        /* Address Offset: 0x018C */
    __IO uint32_t GPIO2B_DS_0;                        /* Address Offset: 0x0190 */
    __IO uint32_t GPIO2B_DS_1;                        /* Address Offset: 0x0194 */
         uint32_t RESERVED0198[34];                   /* Address Offset: 0x0198 */
    __IO uint32_t GPIO2A_PULL;                        /* Address Offset: 0x0220 */
    __IO uint32_t GPIO2B_PULL;                        /* Address Offset: 0x0224 */
         uint32_t RESERVED0228[62];                   /* Address Offset: 0x0228 */
    __IO uint32_t GPIO2A_IE;                          /* Address Offset: 0x0320 */
    __IO uint32_t GPIO2B_IE;                          /* Address Offset: 0x0324 */
         uint32_t RESERVED0328[62];                   /* Address Offset: 0x0328 */
    __IO uint32_t GPIO2A_SMT;                         /* Address Offset: 0x0420 */
    __IO uint32_t GPIO2B_SMT;                         /* Address Offset: 0x0424 */
         uint32_t RESERVED0428[62];                   /* Address Offset: 0x0428 */
    __IO uint32_t GPIO2A_SUS;                         /* Address Offset: 0x0520 */
    __IO uint32_t GPIO2B_SUS;                         /* Address Offset: 0x0524 */
         uint32_t RESERVED0528[62];                   /* Address Offset: 0x0528 */
    __IO uint32_t GPIO2A_SL;                          /* Address Offset: 0x0620 */
    __IO uint32_t GPIO2B_SL;                          /* Address Offset: 0x0624 */
         uint32_t RESERVED0628[62];                   /* Address Offset: 0x0628 */
    __IO uint32_t GPIO2A_OD;                          /* Address Offset: 0x0720 */
    __IO uint32_t GPIO2B_OD;                          /* Address Offset: 0x0724 */
         uint32_t RESERVED0728[54];                   /* Address Offset: 0x0728 */
    __IO uint32_t IO_VSEL_VCCIO6;                     /* Address Offset: 0x0800 */
    __IO uint32_t MISC_CON;                           /* Address Offset: 0x0804 */
         uint32_t RESERVED0808;                       /* Address Offset: 0x0808 */
    __IO uint32_t SARADC_CON0;                        /* Address Offset: 0x080C */
    __IO uint32_t SARADC_CON1;                        /* Address Offset: 0x0810 */
};
/* GPIO0_A_IOC Register Structure Define */
struct GPIO0_A_IOC_REG {
    __IO uint32_t GPIO0A_IOMUX_SEL_0;                 /* Address Offset: 0x0000 */
    __IO uint32_t GPIO0A_IOMUX_SEL_1;                 /* Address Offset: 0x0004 */
         uint32_t RESERVED0008[62];                   /* Address Offset: 0x0008 */
    __IO uint32_t GPIO0A_DS_0;                        /* Address Offset: 0x0100 */
    __IO uint32_t GPIO0A_DS_1;                        /* Address Offset: 0x0104 */
    __IO uint32_t GPIO0A_DS_2;                        /* Address Offset: 0x0108 */
    __IO uint32_t GPIO0A_DS_3;                        /* Address Offset: 0x010C */
         uint32_t RESERVED0110[60];                   /* Address Offset: 0x0110 */
    __IO uint32_t GPIO0A_PULL;                        /* Address Offset: 0x0200 */
         uint32_t RESERVED0204[63];                   /* Address Offset: 0x0204 */
    __IO uint32_t GPIO0A_IE;                          /* Address Offset: 0x0300 */
         uint32_t RESERVED0304[63];                   /* Address Offset: 0x0304 */
    __IO uint32_t GPIO0A_SMT;                         /* Address Offset: 0x0400 */
         uint32_t RESERVED0404[63];                   /* Address Offset: 0x0404 */
    __IO uint32_t GPIO0A_SUS;                         /* Address Offset: 0x0500 */
         uint32_t RESERVED0504[63];                   /* Address Offset: 0x0504 */
    __IO uint32_t GPIO0A_SL;                          /* Address Offset: 0x0600 */
         uint32_t RESERVED0604[63];                   /* Address Offset: 0x0604 */
    __IO uint32_t GPIO0A_OD;                          /* Address Offset: 0x0700 */
         uint32_t RESERVED0704[63];                   /* Address Offset: 0x0704 */
    __IO uint32_t IO_VSEL;                            /* Address Offset: 0x0800 */
    __IO uint32_t GRF_JTAG_CON0;                      /* Address Offset: 0x0804 */
    __IO uint32_t GRF_JTAG_CON1;                      /* Address Offset: 0x0808 */
         uint32_t RESERVED080C[61];                   /* Address Offset: 0x080C */
    __IO uint32_t XIN_CON;                            /* Address Offset: 0x0900 */
};
/* GPIO0_B_IOC Register Structure Define */
struct GPIO0_B_IOC_REG {
         uint32_t RESERVED0000[2];                    /* Address Offset: 0x0000 */
    __IO uint32_t GPIO0B_IOMUX_SEL_0;                 /* Address Offset: 0x0008 */
    __IO uint32_t GPIO0B_IOMUX_SEL_1;                 /* Address Offset: 0x000C */
         uint32_t RESERVED0010[64];                   /* Address Offset: 0x0010 */
    __IO uint32_t GPIO0B_DS_0;                        /* Address Offset: 0x0110 */
    __IO uint32_t GPIO0B_DS_1;                        /* Address Offset: 0x0114 */
    __IO uint32_t GPIO0B_DS_2;                        /* Address Offset: 0x0118 */
         uint32_t RESERVED011C[58];                   /* Address Offset: 0x011C */
    __IO uint32_t GPIO0B_PULL;                        /* Address Offset: 0x0204 */
         uint32_t RESERVED0208[63];                   /* Address Offset: 0x0208 */
    __IO uint32_t GPIO0B_IE;                          /* Address Offset: 0x0304 */
         uint32_t RESERVED0308[63];                   /* Address Offset: 0x0308 */
    __IO uint32_t GPIO0B_SMT;                         /* Address Offset: 0x0404 */
         uint32_t RESERVED0408[63];                   /* Address Offset: 0x0408 */
    __IO uint32_t GPIO0B_SUS;                         /* Address Offset: 0x0504 */
         uint32_t RESERVED0508[63];                   /* Address Offset: 0x0508 */
    __IO uint32_t GPIO0B_SL;                          /* Address Offset: 0x0604 */
         uint32_t RESERVED0608[63];                   /* Address Offset: 0x0608 */
    __IO uint32_t GPIO0B_OD;                          /* Address Offset: 0x0704 */
         uint32_t RESERVED0708[62];                   /* Address Offset: 0x0708 */
    __IO uint32_t IO_VSEL;                            /* Address Offset: 0x0800 */
    __IO uint32_t GRF_JTAG_CON0;                      /* Address Offset: 0x0804 */
    __IO uint32_t GRF_JTAG_CON1;                      /* Address Offset: 0x0808 */
};
/* GPIO Register Structure Define */
struct GPIO_REG {
    __IO uint32_t SWPORT_DR_L;                        /* Address Offset: 0x0000 */
    __IO uint32_t SWPORT_DR_H;                        /* Address Offset: 0x0004 */
    __IO uint32_t SWPORT_DDR_L;                       /* Address Offset: 0x0008 */
    __IO uint32_t SWPORT_DDR_H;                       /* Address Offset: 0x000C */
    __IO uint32_t INT_EN_L;                           /* Address Offset: 0x0010 */
    __IO uint32_t INT_EN_H;                           /* Address Offset: 0x0014 */
    __IO uint32_t INT_MASK_L;                         /* Address Offset: 0x0018 */
    __IO uint32_t INT_MASK_H;                         /* Address Offset: 0x001C */
    __IO uint32_t INT_TYPE_L;                         /* Address Offset: 0x0020 */
    __IO uint32_t INT_TYPE_H;                         /* Address Offset: 0x0024 */
    __IO uint32_t INT_POLARITY_L;                     /* Address Offset: 0x0028 */
    __IO uint32_t INT_POLARITY_H;                     /* Address Offset: 0x002C */
    __IO uint32_t INT_BOTHEDGE_L;                     /* Address Offset: 0x0030 */
    __IO uint32_t INT_BOTHEDGE_H;                     /* Address Offset: 0x0034 */
    __IO uint32_t DEBOUNCE_L;                         /* Address Offset: 0x0038 */
    __IO uint32_t DEBOUNCE_H;                         /* Address Offset: 0x003C */
    __IO uint32_t DBCLK_DIV_EN_L;                     /* Address Offset: 0x0040 */
    __IO uint32_t DBCLK_DIV_EN_H;                     /* Address Offset: 0x0044 */
    __IO uint32_t DBCLK_DIV_CON;                      /* Address Offset: 0x0048 */
         uint32_t RESERVED004C;                       /* Address Offset: 0x004C */
    __I  uint32_t INT_STATUS;                         /* Address Offset: 0x0050 */
         uint32_t RESERVED0054;                       /* Address Offset: 0x0054 */
    __I  uint32_t INT_RAWSTATUS;                      /* Address Offset: 0x0058 */
         uint32_t RESERVED005C;                       /* Address Offset: 0x005C */
    __IO uint32_t PORT_EOI_L;                         /* Address Offset: 0x0060 */
    __IO uint32_t PORT_EOI_H;                         /* Address Offset: 0x0064 */
         uint32_t RESERVED0068[2];                    /* Address Offset: 0x0068 */
    __I  uint32_t EXT_PORT;                           /* Address Offset: 0x0070 */
         uint32_t RESERVED0074;                       /* Address Offset: 0x0074 */
    __I  uint32_t VER_ID;                             /* Address Offset: 0x0078 */
         uint32_t RESERVED007C;                       /* Address Offset: 0x007C */
    __IO uint32_t STORE_ST_L;                         /* Address Offset: 0x0080 */
    __IO uint32_t STORE_ST_H;                         /* Address Offset: 0x0084 */
         uint32_t RESERVED0088[30];                   /* Address Offset: 0x0088 */
    __IO uint32_t GPIO_REG_GROUP_L;                   /* Address Offset: 0x0100 */
    __IO uint32_t GPIO_REG_GROUP_H;                   /* Address Offset: 0x0104 */
    __IO uint32_t GPIO_VIRTUAL_EN;                    /* Address Offset: 0x0108 */
         uint32_t RESERVED010C;                       /* Address Offset: 0x010C */
    __IO uint32_t GPIO_REG_GROUP1_L;                  /* Address Offset: 0x0110 */
    __IO uint32_t GPIO_REG_GROUP1_H;                  /* Address Offset: 0x0114 */
    __IO uint32_t GPIO_REG_GROUP2_L;                  /* Address Offset: 0x0118 */
    __IO uint32_t GPIO_REG_GROUP2_H;                  /* Address Offset: 0x011C */
    __IO uint32_t GPIO_REG_GROUP3_L;                  /* Address Offset: 0x0120 */
    __IO uint32_t GPIO_REG_GROUP3_H;                  /* Address Offset: 0x0124 */
};
/* I2C Register Structure Define */
struct I2C_REG {
    __IO uint32_t CON;                                /* Address Offset: 0x0000 */
    __IO uint32_t CLKDIV;                             /* Address Offset: 0x0004 */
    __IO uint32_t MRXADDR;                            /* Address Offset: 0x0008 */
    __IO uint32_t MRXRADDR;                           /* Address Offset: 0x000C */
    __IO uint32_t MTXCNT;                             /* Address Offset: 0x0010 */
    __IO uint32_t MRXCNT;                             /* Address Offset: 0x0014 */
    __IO uint32_t IEN;                                /* Address Offset: 0x0018 */
    __IO uint32_t IPD;                                /* Address Offset: 0x001C */
    __I  uint32_t FCNT;                               /* Address Offset: 0x0020 */
    __IO uint32_t SCL_OE_DB;                          /* Address Offset: 0x0024 */
         uint32_t RESERVED0028[54];                   /* Address Offset: 0x0028 */
    __IO uint32_t TXDATA[8];                          /* Address Offset: 0x0100 */
         uint32_t RESERVED0120[56];                   /* Address Offset: 0x0120 */
    __I  uint32_t RXDATA[8];                          /* Address Offset: 0x0200 */
    __I  uint32_t ST;                                 /* Address Offset: 0x0220 */
    __IO uint32_t DBGCTRL;                            /* Address Offset: 0x0224 */
    __IO uint32_t CON1;                               /* Address Offset: 0x0228 */
};
/* UART Register Structure Define */
struct UART_REG {
    union {
        __I  uint32_t RBR;                                /* Address Offset: 0x0000 */
        __IO uint32_t DLL;                                /* Address Offset: 0x0000 */
        __O  uint32_t THR;                                /* Address Offset: 0x0000 */
    };
    union {
        __IO uint32_t DLH;                                /* Address Offset: 0x0004 */
        __IO uint32_t IER;                                /* Address Offset: 0x0004 */
    };
    union {
        __O  uint32_t FCR;                                /* Address Offset: 0x0008 */
        __I  uint32_t IIR;                                /* Address Offset: 0x0008 */
    };
    __IO uint32_t LCR;                                /* Address Offset: 0x000C */
    __IO uint32_t MCR;                                /* Address Offset: 0x0010 */
    __I  uint32_t LSR;                                /* Address Offset: 0x0014 */
    __I  uint32_t MSR;                                /* Address Offset: 0x0018 */
    __IO uint32_t SCR;                                /* Address Offset: 0x001C */
         uint32_t RESERVED0020[4];                    /* Address Offset: 0x0020 */
    union {
        __I  uint32_t SRBR;                               /* Address Offset: 0x0030 */
        __O  uint32_t STHR;                               /* Address Offset: 0x0030 */
    };
         uint32_t RESERVED0034[15];                   /* Address Offset: 0x0034 */
    __IO uint32_t FAR;                                /* Address Offset: 0x0070 */
    __I  uint32_t TFR;                                /* Address Offset: 0x0074 */
    __O  uint32_t RFW;                                /* Address Offset: 0x0078 */
    __I  uint32_t USR;                                /* Address Offset: 0x007C */
    __I  uint32_t TFL;                                /* Address Offset: 0x0080 */
    __I  uint32_t RFL;                                /* Address Offset: 0x0084 */
    __O  uint32_t SRR;                                /* Address Offset: 0x0088 */
    __IO uint32_t SRTS;                               /* Address Offset: 0x008C */
    __IO uint32_t SBCR;                               /* Address Offset: 0x0090 */
    __IO uint32_t SDMAM;                              /* Address Offset: 0x0094 */
    __IO uint32_t SFE;                                /* Address Offset: 0x0098 */
    __IO uint32_t SRT;                                /* Address Offset: 0x009C */
    __IO uint32_t STET;                               /* Address Offset: 0x00A0 */
    __IO uint32_t HTX;                                /* Address Offset: 0x00A4 */
    __O  uint32_t DMASA;                              /* Address Offset: 0x00A8 */
         uint32_t RESERVED00AC[18];                   /* Address Offset: 0x00AC */
    __I  uint32_t CPR;                                /* Address Offset: 0x00F4 */
    __I  uint32_t UCV;                                /* Address Offset: 0x00F8 */
    __I  uint32_t CTR;                                /* Address Offset: 0x00FC */
};
/* PWM Register Structure Define */
struct PWM_REG {
    __I  uint32_t VERSION_ID;                         /* Address Offset: 0x0000 */
    __IO uint32_t ENABLE;                             /* Address Offset: 0x0004 */
    __IO uint32_t CLK_CTRL;                           /* Address Offset: 0x0008 */
    __IO uint32_t CTRL;                               /* Address Offset: 0x000C */
    __IO uint32_t PERIOD;                             /* Address Offset: 0x0010 */
    __IO uint32_t DUTY;                               /* Address Offset: 0x0014 */
    __IO uint32_t OFFSET;                             /* Address Offset: 0x0018 */
    __IO uint32_t RPT;                                /* Address Offset: 0x001C */
    __IO uint32_t FILTER_CTRL;                        /* Address Offset: 0x0020 */
    __I  uint32_t CNT;                                /* Address Offset: 0x0024 */
    __IO uint32_t ENABLE_DELAY;                       /* Address Offset: 0x0028 */
    __IO uint32_t HPC;                                /* Address Offset: 0x002C */
    __IO uint32_t LPC;                                /* Address Offset: 0x0030 */
         uint32_t RESERVED0034[15];                   /* Address Offset: 0x0034 */
    __IO uint32_t INTSTS;                             /* Address Offset: 0x0070 */
    __IO uint32_t INT_EN;                             /* Address Offset: 0x0074 */
         uint32_t RESERVED0078[18];                   /* Address Offset: 0x0078 */
    __IO uint32_t GLOBAL_ARBITER;                     /* Address Offset: 0x00C0 */
    __IO uint32_t GLOBAL_CTRL;                        /* Address Offset: 0x00C4 */
         uint32_t RESERVED00C8[14];                   /* Address Offset: 0x00C8 */
    __IO uint32_t PWRMATCH_ARBITER;                   /* Address Offset: 0x0100 */
    __IO uint32_t PWRMATCH_CTRL;                      /* Address Offset: 0x0104 */
    __IO uint32_t PWRMATCH_LPRE;                      /* Address Offset: 0x0108 */
    __IO uint32_t PWRMATCH_HPRE;                      /* Address Offset: 0x010C */
    __IO uint32_t PWRMATCH_LD;                        /* Address Offset: 0x0110 */
    __IO uint32_t PWRMATCH_HD_ZERO;                   /* Address Offset: 0x0114 */
    __IO uint32_t PWRMATCH_HD_ONE;                    /* Address Offset: 0x0118 */
    __IO uint32_t PWRMATCH_VALUE[16];                 /* Address Offset: 0x011C */
    __I  uint32_t PWRCAPTURE_VALUE;                   /* Address Offset: 0x015C */
         uint32_t RESERVED0160[24];                   /* Address Offset: 0x0160 */
    __IO uint32_t FREQ_ARBITER;                       /* Address Offset: 0x01C0 */
    __IO uint32_t FREQ_CTRL;                          /* Address Offset: 0x01C4 */
    __IO uint32_t FREQ_TIMER_VALUE;                   /* Address Offset: 0x01C8 */
    __IO uint32_t FREQ_RESULT_VALUE;                  /* Address Offset: 0x01CC */
         uint32_t RESERVED01D0[12];                   /* Address Offset: 0x01D0 */
    __IO uint32_t COUNTER_ARBITER;                    /* Address Offset: 0x0200 */
    __IO uint32_t COUNTER_CTRL;                       /* Address Offset: 0x0204 */
    __I  uint32_t COUNTER_LOW;                        /* Address Offset: 0x0208 */
    __I  uint32_t COUNTER_HIGH;                       /* Address Offset: 0x020C */
};
/* SPI2AHB Register Structure Define */
struct SPI2AHB_REG {
    __IO uint32_t SPI_CTRL;                           /* Address Offset: 0x0000 */
    __IO uint32_t DMA_CTRL;                           /* Address Offset: 0x0004 */
         uint32_t RESERVED0008;                       /* Address Offset: 0x0008 */
    __IO uint32_t SPI_EN;                             /* Address Offset: 0x000C */
    __IO uint32_t FRAME_ADDR;                         /* Address Offset: 0x0010 */
    __IO uint32_t FRAME_BNUM;                         /* Address Offset: 0x0014 */
         uint32_t RESERVED0018[4];                    /* Address Offset: 0x0018 */
    __IO uint32_t FRAME_TME_EN;                       /* Address Offset: 0x0028 */
    __IO uint32_t FRAME_TME;                          /* Address Offset: 0x002C */
    __IO uint32_t SW_RST;                             /* Address Offset: 0x0030 */
         uint32_t RESERVED0034;                       /* Address Offset: 0x0034 */
    __I  uint32_t CSN;                                /* Address Offset: 0x0038 */
         uint32_t RESERVED003C;                       /* Address Offset: 0x003C */
    __IO uint32_t IMR;                                /* Address Offset: 0x0040 */
    __IO uint32_t ICLR;                               /* Address Offset: 0x0044 */
    __I  uint32_t ISR;                                /* Address Offset: 0x0048 */
    __I  uint32_t RISR;                               /* Address Offset: 0x004C */
    __I  uint32_t VER;                                /* Address Offset: 0x0050 */
    __I  uint32_t MONITOR;                            /* Address Offset: 0x0054 */
    __I  uint32_t FRAME_CNT_MONITOR;                  /* Address Offset: 0x0058 */
};
/* MBOX Register Structure Define */
struct MBOX_REG {
    __IO uint32_t A2B_INTEN;                          /* Address Offset: 0x0000 */
    __IO uint32_t A2B_STATUS;                         /* Address Offset: 0x0004 */
    __IO uint32_t A2B_CMD;                            /* Address Offset: 0x0008 */
    __IO uint32_t A2B_DATA;                           /* Address Offset: 0x000C */
    __IO uint32_t B2A_INTEN;                          /* Address Offset: 0x0010 */
    __IO uint32_t B2A_STATUS;                         /* Address Offset: 0x0014 */
    __IO uint32_t B2A_CMD;                            /* Address Offset: 0x0018 */
    __IO uint32_t B2A_DATA;                           /* Address Offset: 0x001C */
};
/* FSPI Register Structure Define */
struct FSPI_REG {
    __IO uint32_t CTRL0;                              /* Address Offset: 0x0000 */
    __IO uint32_t IMR;                                /* Address Offset: 0x0004 */
    __IO uint32_t ICLR;                               /* Address Offset: 0x0008 */
    __IO uint32_t FTLR;                               /* Address Offset: 0x000C */
    __IO uint32_t RCVR;                               /* Address Offset: 0x0010 */
    __IO uint32_t AX0;                                /* Address Offset: 0x0014 */
    __IO uint32_t ABIT0;                              /* Address Offset: 0x0018 */
    __IO uint32_t ISR;                                /* Address Offset: 0x001C */
    __IO uint32_t FSR;                                /* Address Offset: 0x0020 */
    __I  uint32_t SR;                                 /* Address Offset: 0x0024 */
    __IO uint32_t RISR;                               /* Address Offset: 0x0028 */
    __IO uint32_t VER;                                /* Address Offset: 0x002C */
    __IO uint32_t QOP;                                /* Address Offset: 0x0030 */
    __IO uint32_t EXT_CTRL;                           /* Address Offset: 0x0034 */
         uint32_t RESERVED0038;                       /* Address Offset: 0x0038 */
    __IO uint32_t DLL_CTRL0;                          /* Address Offset: 0x003C */
    __IO uint32_t HRDYMASK;                           /* Address Offset: 0x0040 */
    __IO uint32_t EXT_AX;                             /* Address Offset: 0x0044 */
    __IO uint32_t SCLK_INATM_CNT;                     /* Address Offset: 0x0048 */
         uint32_t RESERVED004C;                       /* Address Offset: 0x004C */
    __IO uint32_t XMMC_WCMD0;                         /* Address Offset: 0x0050 */
    __IO uint32_t XMMC_RCMD0;                         /* Address Offset: 0x0054 */
    __IO uint32_t XMMC_CTRL;                          /* Address Offset: 0x0058 */
    __IO uint32_t MODE;                               /* Address Offset: 0x005C */
    __IO uint32_t DEVRGN;                             /* Address Offset: 0x0060 */
    __IO uint32_t DEVSIZE0;                           /* Address Offset: 0x0064 */
    __IO uint32_t TME0;                               /* Address Offset: 0x0068 */
         uint32_t RESERVED006C;                       /* Address Offset: 0x006C */
    __IO uint32_t RX_FULL_WTMRK;                      /* Address Offset: 0x0070 */
    __IO uint32_t DUMM_CTRL;                          /* Address Offset: 0x0074 */
    __IO uint32_t CMD_EXT;                            /* Address Offset: 0x0078 */
    __IO uint32_t TRC_CTRL;                           /* Address Offset: 0x007C */
    __IO uint32_t DMATR;                              /* Address Offset: 0x0080 */
    __IO uint32_t DMAADDR;                            /* Address Offset: 0x0084 */
    __IO uint32_t LEN_CTRL;                           /* Address Offset: 0x0088 */
    __IO uint32_t LEN_EXT;                            /* Address Offset: 0x008C */
         uint32_t RESERVED0090;                       /* Address Offset: 0x0090 */
    __IO uint32_t XMMCSR;                             /* Address Offset: 0x0094 */
    __IO uint32_t HYPER_RSVD_ADDR;                    /* Address Offset: 0x0098 */
    __IO uint32_t VDMC0;                              /* Address Offset: 0x009C */
    __IO uint32_t DBG_IO_CTRL;                        /* Address Offset: 0x00A0 */
    __IO uint32_t DEV_RSTN;                           /* Address Offset: 0x00A4 */
    __IO uint32_t SLF_DQS_CTRL;                       /* Address Offset: 0x00A8 */
    __IO uint32_t TRNS_TR_CTRL;                       /* Address Offset: 0x00AC */
    __IO uint32_t TRNS_TR;                            /* Address Offset: 0x00B0 */
    __IO uint32_t DMA_CHAIN_TR;                       /* Address Offset: 0x00B4 */
    __IO uint32_t DMA_HEAD_ADDR;                      /* Address Offset: 0x00B8 */
    __IO uint32_t DMA_CTRL;                           /* Address Offset: 0x00BC */
         uint32_t RESERVED00C0[16];                   /* Address Offset: 0x00C0 */
    __O  uint32_t CMD;                                /* Address Offset: 0x0100 */
    __O  uint32_t ADDR;                               /* Address Offset: 0x0104 */
    __IO uint32_t DATA;                               /* Address Offset: 0x0108 */
         uint32_t RESERVED010C[61];                   /* Address Offset: 0x010C */
    __IO uint32_t CTRL1;                              /* Address Offset: 0x0200 */
         uint32_t RESERVED0204[4];                    /* Address Offset: 0x0204 */
    __IO uint32_t AX1;                                /* Address Offset: 0x0214 */
    __IO uint32_t ABIT1;                              /* Address Offset: 0x0218 */
         uint32_t RESERVED021C[8];                    /* Address Offset: 0x021C */
    __IO uint32_t DLL_CTRL1;                          /* Address Offset: 0x023C */
         uint32_t RESERVED0240[4];                    /* Address Offset: 0x0240 */
    __IO uint32_t XMMC_WCMD1;                         /* Address Offset: 0x0250 */
    __IO uint32_t XMMC_RCMD1;                         /* Address Offset: 0x0254 */
         uint32_t RESERVED0258[3];                    /* Address Offset: 0x0258 */
    __IO uint32_t DEVSIZE1;                           /* Address Offset: 0x0264 */
    __IO uint32_t TME1;                               /* Address Offset: 0x0268 */
         uint32_t RESERVED026C[12];                   /* Address Offset: 0x026C */
    __IO uint32_t VDMC1;                              /* Address Offset: 0x029C */
};
/* ICACHE Register Structure Define */
struct ICACHE_REG {
    __IO uint32_t CACHE_CTRL;                         /* Address Offset: 0x0000 */
    __IO uint32_t CACHE_MAINTAIN[2];                  /* Address Offset: 0x0004 */
    __IO uint32_t STB_TIMEOUT_CTRL;                   /* Address Offset: 0x000C */
         uint32_t RESERVED0010[4];                    /* Address Offset: 0x0010 */
    __IO uint32_t CACHE_INT_EN;                       /* Address Offset: 0x0020 */
    __IO uint32_t CACHE_INT_ST;                       /* Address Offset: 0x0024 */
    __I  uint32_t CACHE_ERR_HADDR;                    /* Address Offset: 0x0028 */
         uint32_t RESERVED002C;                       /* Address Offset: 0x002C */
    __I  uint32_t CACHE_STATUS;                       /* Address Offset: 0x0030 */
         uint32_t RESERVED0034[3];                    /* Address Offset: 0x0034 */
    __I  uint32_t PMU_RD_NUM_CNT;                     /* Address Offset: 0x0040 */
    __I  uint32_t PMU_WR_NUM_CNT;                     /* Address Offset: 0x0044 */
    __I  uint32_t PMU_SRAM_RD_HIT_CNT;                /* Address Offset: 0x0048 */
    __I  uint32_t PMU_HB_RD_HIT_CNT;                  /* Address Offset: 0x004C */
    __I  uint32_t PMU_STB_RD_HIT_CNT;                 /* Address Offset: 0x0050 */
    __I  uint32_t PMU_RD_HIT_CNT;                     /* Address Offset: 0x0054 */
    __I  uint32_t PMU_WR_HIT_CNT;                     /* Address Offset: 0x0058 */
    __I  uint32_t PMU_RD_MISS_PENALTY_CNT;            /* Address Offset: 0x005C */
    __I  uint32_t PMU_WR_MISS_PENALTY_CNT;            /* Address Offset: 0x0060 */
    __I  uint32_t PMU_RD_LAT_CNT;                     /* Address Offset: 0x0064 */
    __I  uint32_t PMU_WR_LAT_CNT;                     /* Address Offset: 0x0068 */
         uint32_t RESERVED006C[33];                   /* Address Offset: 0x006C */
    __I  uint32_t REVISION;                           /* Address Offset: 0x00F0 */
};
/* DCACHE Register Structure Define */
struct DCACHE_REG {
    __IO uint32_t CACHE_CTRL;                         /* Address Offset: 0x0000 */
    __IO uint32_t CACHE_MAINTAIN[2];                  /* Address Offset: 0x0004 */
    __IO uint32_t STB_TIMEOUT_CTRL;                   /* Address Offset: 0x000C */
         uint32_t RESERVED0010[4];                    /* Address Offset: 0x0010 */
    __IO uint32_t CACHE_INT_EN;                       /* Address Offset: 0x0020 */
    __IO uint32_t CACHE_INT_ST;                       /* Address Offset: 0x0024 */
    __I  uint32_t CACHE_ERR_HADDR;                    /* Address Offset: 0x0028 */
         uint32_t RESERVED002C;                       /* Address Offset: 0x002C */
    __I  uint32_t CACHE_STATUS;                       /* Address Offset: 0x0030 */
         uint32_t RESERVED0034[3];                    /* Address Offset: 0x0034 */
    __I  uint32_t PMU_RD_NUM_CNT;                     /* Address Offset: 0x0040 */
    __I  uint32_t PMU_WR_NUM_CNT;                     /* Address Offset: 0x0044 */
    __I  uint32_t PMU_SRAM_RD_HIT_CNT;                /* Address Offset: 0x0048 */
    __I  uint32_t PMU_HB_RD_HIT_CNT;                  /* Address Offset: 0x004C */
    __I  uint32_t PMU_STB_RD_HIT_CNT;                 /* Address Offset: 0x0050 */
    __I  uint32_t PMU_RD_HIT_CNT;                     /* Address Offset: 0x0054 */
    __I  uint32_t PMU_WR_HIT_CNT;                     /* Address Offset: 0x0058 */
    __I  uint32_t PMU_RD_MISS_PENALTY_CNT;            /* Address Offset: 0x005C */
    __I  uint32_t PMU_WR_MISS_PENALTY_CNT;            /* Address Offset: 0x0060 */
    __I  uint32_t PMU_RD_LAT_CNT;                     /* Address Offset: 0x0064 */
    __I  uint32_t PMU_WR_LAT_CNT;                     /* Address Offset: 0x0068 */
         uint32_t RESERVED006C[33];                   /* Address Offset: 0x006C */
    __I  uint32_t REVISION;                           /* Address Offset: 0x00F0 */
};
/* SPINLOCK Register Structure Define */
struct SPINLOCK_REG {
    __IO uint32_t STATUS[64];                         /* Address Offset: 0x0000 */
};
/* TIMER Register Structure Define */
struct TIMER_REG {
    __IO uint32_t LOAD_COUNT[2];                      /* Address Offset: 0x0000 */
    __I  uint32_t CURRENT_VALUE[2];                   /* Address Offset: 0x0008 */
    __IO uint32_t CONTROLREG;                         /* Address Offset: 0x0010 */
         uint32_t RESERVED0014;                       /* Address Offset: 0x0014 */
    __IO uint32_t INTSTATUS;                          /* Address Offset: 0x0018 */
};
/* SPI Register Structure Define */
struct SPI_REG {
    __IO uint32_t CTRLR[2];                           /* Address Offset: 0x0000 */
    __IO uint32_t ENR;                                /* Address Offset: 0x0008 */
    __IO uint32_t SER;                                /* Address Offset: 0x000C */
    __IO uint32_t BAUDR;                              /* Address Offset: 0x0010 */
    __IO uint32_t TXFTLR;                             /* Address Offset: 0x0014 */
    __IO uint32_t RXFTLR;                             /* Address Offset: 0x0018 */
    __I  uint32_t TXFLR;                              /* Address Offset: 0x001C */
    __I  uint32_t RXFLR;                              /* Address Offset: 0x0020 */
    __I  uint32_t SR;                                 /* Address Offset: 0x0024 */
    __IO uint32_t IPR;                                /* Address Offset: 0x0028 */
    __IO uint32_t IMR;                                /* Address Offset: 0x002C */
    __IO uint32_t ISR;                                /* Address Offset: 0x0030 */
    __IO uint32_t RISR;                               /* Address Offset: 0x0034 */
    __O  uint32_t ICR;                                /* Address Offset: 0x0038 */
    __IO uint32_t DMACR;                              /* Address Offset: 0x003C */
    __IO uint32_t DMATDLR;                            /* Address Offset: 0x0040 */
    __IO uint32_t DMARDLR;                            /* Address Offset: 0x0044 */
    __I  uint32_t VERSION;                            /* Address Offset: 0x0048 */
    __IO uint32_t TIMEOUT;                            /* Address Offset: 0x004C */
    __IO uint32_t BYPASS;                             /* Address Offset: 0x0050 */
         uint32_t RESERVED0054[235];                  /* Address Offset: 0x0054 */
    __O  uint32_t TXDR;                               /* Address Offset: 0x0400 */
         uint32_t RESERVED0404[255];                  /* Address Offset: 0x0404 */
    __I  uint32_t RXDR;                               /* Address Offset: 0x0800 */
};
/* SARADC Register Structure Define */
struct SARADC_REG {
    __IO uint32_t CONV_CON;                           /* Address Offset: 0x0000 */
    __IO uint32_t T_PD_SOC;                           /* Address Offset: 0x0004 */
    __IO uint32_t T_AS_SOC;                           /* Address Offset: 0x0008 */
    __IO uint32_t T_DAS_SOC;                          /* Address Offset: 0x000C */
    __IO uint32_t T_SEL_SOC;                          /* Address Offset: 0x0010 */
    __IO uint32_t HIGH_COMP[16];                      /* Address Offset: 0x0014 */
    __IO uint32_t LOW_COMP[16];                       /* Address Offset: 0x0054 */
    __IO uint32_t DEBOUNCE;                           /* Address Offset: 0x0094 */
    __IO uint32_t HT_INT_EN;                          /* Address Offset: 0x0098 */
    __IO uint32_t LT_INT_EN;                          /* Address Offset: 0x009C */
         uint32_t RESERVED00A0[24];                   /* Address Offset: 0x00A0 */
    __IO uint32_t MT_INT_EN;                          /* Address Offset: 0x0100 */
    __IO uint32_t END_INT_EN;                         /* Address Offset: 0x0104 */
    __IO uint32_t ST_CON;                             /* Address Offset: 0x0108 */
    __I  uint32_t STATUS;                             /* Address Offset: 0x010C */
    __IO uint32_t END_INT_ST;                         /* Address Offset: 0x0110 */
    __IO uint32_t HT_INT_ST;                          /* Address Offset: 0x0114 */
    __IO uint32_t LT_INT_ST;                          /* Address Offset: 0x0118 */
    __IO uint32_t MT_INT_ST;                          /* Address Offset: 0x011C */
    __I  uint32_t DATA[16];                           /* Address Offset: 0x0120 */
    __IO uint32_t AUTO_CH_EN;                         /* Address Offset: 0x0160 */
};
#endif /*  __ASSEMBLY__  */
/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#define PERICRU_BASE                   0x20000000U /* PERICRU base address */
#define VEPUCRU_BASE                   0x20010000U /* VEPUCRU base address */
#define NPUCRU_BASE                    0x20020000U /* NPUCRU base address */
#define VICRU_BASE                     0x20030000U /* VICRU base address */
#define CORECRU_BASE                   0x20040000U /* CORECRU base address */
#define DDRCRU_BASE                    0x20050000U /* DDRCRU base address */
#define SUBDDRCRU_BASE                 0x20058000U /* SUBDDRCRU base address */
#define TOPCRU_BASE                    0x20060000U /* TOPCRU base address */
#define PMU0CRU_BASE                   0x20070000U /* PMU0CRU base address */
#define PMU1CRU_BASE                   0x20080000U /* PMU1CRU base address */
#define GRF_VEPU_BASE                  0x20100000U /* GRF_VEPU base address */
#define GRF_NPU_BASE                   0x20110000U /* GRF_NPU base address */
#define GRF_VI_BASE                    0x20120000U /* GRF_VI base address */
#define GRF_CPU_BASE                   0x20130000U /* GRF_CPU base address */
#define GRF_DDR_BASE                   0x20140000U /* GRF_DDR base address */
#define GRF_SYS_BASE                   0x20150000U /* GRF_SYS base address */
#define GRF_PMU_BASE                   0x20160000U /* GRF_PMU base address */
#define GPIO1_IOC_BASE                 0x20170000U /* GPIO1_IOC base address */
#define GPIO2_IOC_BASE                 0x201A0000U /* GPIO2_IOC base address */
#define GPIO0_A_IOC_BASE               0x201B0000U /* GPIO0_A_IOC base address */
#define GPIO0_B_IOC_BASE               0x201C0000U /* GPIO0_B_IOC base address */
#define GPIO0_BASE                     0x20520000U /* GPIO0 base address */
#define I2C0_BASE                      0x20530000U /* I2C0 base address */
#define UART0_BASE                     0x20540000U /* UART0 base address */
#define PWM0_BASE                      0x20550000U /* PWM0 base address */
#define SPI2AHB_BASE                   0x20570000U /* SPI2AHB base address */
#define MBOX0_BASE                     0x20580000U /* MBOX0 base address */
#define MBOX1_BASE                     0x20581000U /* MBOX1 base address */
#define MBOX2_BASE                     0x20582000U /* MBOX2 base address */
#define MBOX3_BASE                     0x20583000U /* MBOX3 base address */
#define FSPI1_BASE                     0x205C0000U /* FSPI1 base address */
#define ICACHE_BASE                    0x206A0000U /* ICACHE base address */
#define DCACHE_BASE                    0x206A0000U /* DCACHE base address */
#define SPINLOCK_BASE                  0x20700000U /* SPINLOCK base address */
#define TIMER0_BASE                    0x20830000U /* TIMER0 base address */
#define TIMER1_BASE                    0x20831000U /* TIMER1 base address */
#define TIMER2_BASE                    0x20832000U /* TIMER2 base address */
#define TIMER3_BASE                    0x20833000U /* TIMER3 base address */
#define TIMER4_BASE                    0x20834000U /* TIMER4 base address */
#define TIMER5_BASE                    0x20835000U /* TIMER5 base address */
#define GPIO2_BASE                     0x20840000U /* GPIO2 base address */
#define SPI0_BASE                      0x20850000U /* SPI0 base address */
#define UART1_BASE                     0x20870000U /* UART1 base address */
#define UART2_BASE                     0x20880000U /* UART2 base address */
#define I2C1_BASE                      0x20910000U /* I2C1 base address */
#define I2C2_BASE                      0x20920000U /* I2C2 base address */
#define I2C3_BASE                      0x20930000U /* I2C3 base address */
#define I2C4_BASE                      0x20940000U /* I2C4 base address */
#define PWM1_BASE                      0x20970000U /* PWM1 base address */
#define PWM2_BASE                      0x20980000U /* PWM2 base address */
#define SARADC_BASE                    0x209A0000U /* SARADC base address */
#define MBOX4_BASE                     0x20A10000U /* MBOX4 base address */
#define MBOX5_BASE                     0x20A11000U /* MBOX5 base address */
#define MBOX6_BASE                     0x20A12000U /* MBOX6 base address */
#define MBOX7_BASE                     0x20A13000U /* MBOX7 base address */
#define FSPI0_BASE                     0x20D40000U /* FSPI0 base address */
#define GPIO1_BASE                     0x20D80000U /* GPIO1 base address */
/****************************************************************************************/
/*                                                                                      */
/*                               Module Variable Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Module Variable Define */

#define PERICRU             ((struct PERICRU_REG *) PERICRU_BASE)
#define VEPUCRU             ((struct VEPUCRU_REG *) VEPUCRU_BASE)
#define NPUCRU              ((struct NPUCRU_REG *) NPUCRU_BASE)
#define VICRU               ((struct VICRU_REG *) VICRU_BASE)
#define CORECRU             ((struct CORECRU_REG *) CORECRU_BASE)
#define DDRCRU              ((struct DDRCRU_REG *) DDRCRU_BASE)
#define SUBDDRCRU           ((struct SUBDDRCRU_REG *) SUBDDRCRU_BASE)
#define TOPCRU              ((struct TOPCRU_REG *) TOPCRU_BASE)
#define PMU0CRU             ((struct PMU0CRU_REG *) PMU0CRU_BASE)
#define PMU1CRU             ((struct PMU1CRU_REG *) PMU1CRU_BASE)
#define GRF_VEPU            ((struct GRF_VEPU_REG *) GRF_VEPU_BASE)
#define GRF_NPU             ((struct GRF_NPU_REG *) GRF_NPU_BASE)
#define GRF_VI              ((struct GRF_VI_REG *) GRF_VI_BASE)
#define GRF_CPU             ((struct GRF_CPU_REG *) GRF_CPU_BASE)
#define GRF_DDR             ((struct GRF_DDR_REG *) GRF_DDR_BASE)
#define GRF_SYS             ((struct GRF_SYS_REG *) GRF_SYS_BASE)
#define GRF_PMU             ((struct GRF_PMU_REG *) GRF_PMU_BASE)
#define GPIO1_IOC           ((struct GPIO1_IOC_REG *) GPIO1_IOC_BASE)
#define GPIO2_IOC           ((struct GPIO2_IOC_REG *) GPIO2_IOC_BASE)
#define GPIO0_A_IOC         ((struct GPIO0_A_IOC_REG *) GPIO0_A_IOC_BASE)
#define GPIO0_B_IOC         ((struct GPIO0_B_IOC_REG *) GPIO0_B_IOC_BASE)
#define GPIO0               ((struct GPIO_REG *) GPIO0_BASE)
#define I2C0                ((struct I2C_REG *) I2C0_BASE)
#define UART0               ((struct UART_REG *) UART0_BASE)
#define PWM0                ((struct PWM_REG *) PWM0_BASE)
#define SPI2AHB             ((struct SPI2AHB_REG *) SPI2AHB_BASE)
#define MBOX0               ((struct MBOX_REG *) MBOX0_BASE)
#define MBOX1               ((struct MBOX_REG *) MBOX1_BASE)
#define MBOX2               ((struct MBOX_REG *) MBOX2_BASE)
#define MBOX3               ((struct MBOX_REG *) MBOX3_BASE)
#define FSPI1               ((struct FSPI_REG *) FSPI1_BASE)
#define ICACHE              ((struct ICACHE_REG *) ICACHE_BASE)
#define DCACHE              ((struct DCACHE_REG *) DCACHE_BASE)
#define SPINLOCK            ((struct SPINLOCK_REG *) SPINLOCK_BASE)
#define TIMER0              ((struct TIMER_REG *) TIMER0_BASE)
#define TIMER1              ((struct TIMER_REG *) TIMER1_BASE)
#define TIMER2              ((struct TIMER_REG *) TIMER2_BASE)
#define TIMER3              ((struct TIMER_REG *) TIMER3_BASE)
#define TIMER4              ((struct TIMER_REG *) TIMER4_BASE)
#define TIMER5              ((struct TIMER_REG *) TIMER5_BASE)
#define GPIO2               ((struct GPIO_REG *) GPIO2_BASE)
#define SPI0                ((struct SPI_REG *) SPI0_BASE)
#define UART1               ((struct UART_REG *) UART1_BASE)
#define UART2               ((struct UART_REG *) UART2_BASE)
#define I2C1                ((struct I2C_REG *) I2C1_BASE)
#define I2C2                ((struct I2C_REG *) I2C2_BASE)
#define I2C3                ((struct I2C_REG *) I2C3_BASE)
#define I2C4                ((struct I2C_REG *) I2C4_BASE)
#define PWM1                ((struct PWM_REG *) PWM1_BASE)
#define PWM2                ((struct PWM_REG *) PWM2_BASE)
#define SARADC              ((struct SARADC_REG *) SARADC_BASE)
#define MBOX4               ((struct MBOX_REG *) MBOX4_BASE)
#define MBOX5               ((struct MBOX_REG *) MBOX5_BASE)
#define MBOX6               ((struct MBOX_REG *) MBOX6_BASE)
#define MBOX7               ((struct MBOX_REG *) MBOX7_BASE)
#define FSPI0               ((struct FSPI_REG *) FSPI0_BASE)
#define GPIO1               ((struct GPIO_REG *) GPIO1_BASE)

#define IS_PERICRU_INSTANCE(instance) ((instance) == PERICRU)
#define IS_VEPUCRU_INSTANCE(instance) ((instance) == VEPUCRU)
#define IS_NPUCRU_INSTANCE(instance) ((instance) == NPUCRU)
#define IS_VICRU_INSTANCE(instance) ((instance) == VICRU)
#define IS_CORECRU_INSTANCE(instance) ((instance) == CORECRU)
#define IS_DDRCRU_INSTANCE(instance) ((instance) == DDRCRU)
#define IS_SUBDDRCRU_INSTANCE(instance) ((instance) == SUBDDRCRU)
#define IS_TOPCRU_INSTANCE(instance) ((instance) == TOPCRU)
#define IS_PMU0CRU_INSTANCE(instance) ((instance) == PMU0CRU)
#define IS_PMU1CRU_INSTANCE(instance) ((instance) == PMU1CRU)
#define IS_GRF_VEPU_INSTANCE(instance) ((instance) == GRF_VEPU)
#define IS_GRF_NPU_INSTANCE(instance) ((instance) == GRF_NPU)
#define IS_GRF_VI_INSTANCE(instance) ((instance) == GRF_VI)
#define IS_GRF_CPU_INSTANCE(instance) ((instance) == GRF_CPU)
#define IS_GRF_DDR_INSTANCE(instance) ((instance) == GRF_DDR)
#define IS_GRF_SYS_INSTANCE(instance) ((instance) == GRF_SYS)
#define IS_GRF_PMU_INSTANCE(instance) ((instance) == GRF_PMU)
#define IS_GPIO1_IOC_INSTANCE(instance) ((instance) == GPIO1_IOC)
#define IS_GPIO2_IOC_INSTANCE(instance) ((instance) == GPIO2_IOC)
#define IS_GPIO0_A_IOC_INSTANCE(instance) ((instance) == GPIO0_A_IOC)
#define IS_GPIO0_B_IOC_INSTANCE(instance) ((instance) == GPIO0_B_IOC)
#define IS_SPI2AHB_INSTANCE(instance) ((instance) == SPI2AHB)
#define IS_ICACHE_INSTANCE(instance) ((instance) == ICACHE)
#define IS_DCACHE_INSTANCE(instance) ((instance) == DCACHE)
#define IS_SPINLOCK_INSTANCE(instance) ((instance) == SPINLOCK)
#define IS_SARADC_INSTANCE(instance) ((instance) == SARADC)
#define IS_GPIO_INSTANCE(instance) (((instance) == GPIO0) || ((instance) == GPIO2) || ((instance) == GPIO1))
#define IS_I2C_INSTANCE(instance) (((instance) == I2C0) || ((instance) == I2C1) || ((instance) == I2C2) || ((instance) == I2C3) || ((instance) == I2C4))
#define IS_UART_INSTANCE(instance) (((instance) == UART0) || ((instance) == UART1) || ((instance) == UART2))
#define IS_PWM_INSTANCE(instance) (((instance) == PWM0) || ((instance) == PWM1) || ((instance) == PWM2))
#define IS_MBOX_INSTANCE(instance) (((instance) == MBOX0) || ((instance) == MBOX1) || ((instance) == MBOX2) || ((instance) == MBOX3) || ((instance) == MBOX4) || ((instance) == MBOX5) || ((instance) == MBOX6) || ((instance) == MBOX7))
#define IS_FSPI_INSTANCE(instance) (((instance) == FSPI1) || ((instance) == FSPI0))
#define IS_TIMER_INSTANCE(instance) (((instance) == TIMER0) || ((instance) == TIMER1) || ((instance) == TIMER2) || ((instance) == TIMER3) || ((instance) == TIMER4) || ((instance) == TIMER5))
#define IS_SPI_INSTANCE(instance) ((instance) == SPI0)
/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/****************************************PERICRU*****************************************/
/* PERICLKSEL_CON00 */
#define PERICRU_PERICLKSEL_CON00_OFFSET                    (0x300U)
#define PERICRU_PERICLKSEL_CON00_PCLK_PERI_ROOT_DIV_SHIFT  (0U)
#define PERICRU_PERICLKSEL_CON00_PCLK_PERI_ROOT_DIV_MASK   (0x3U << PERICRU_PERICLKSEL_CON00_PCLK_PERI_ROOT_DIV_SHIFT)  /* 0x00000003 */
#define PERICRU_PERICLKSEL_CON00_CLK_TSADC_DIV_SHIFT       (4U)
#define PERICRU_PERICLKSEL_CON00_CLK_TSADC_DIV_MASK        (0x1FU << PERICRU_PERICLKSEL_CON00_CLK_TSADC_DIV_SHIFT)      /* 0x000001F0 */
#define PERICRU_PERICLKSEL_CON00_CLK_TSADC_TSEN_DIV_SHIFT  (10U)
#define PERICRU_PERICLKSEL_CON00_CLK_TSADC_TSEN_DIV_MASK   (0x1FU << PERICRU_PERICLKSEL_CON00_CLK_TSADC_TSEN_DIV_SHIFT) /* 0x00007C00 */
/* PERICLKSEL_CON01 */
#define PERICRU_PERICLKSEL_CON01_OFFSET                    (0x304U)
#define PERICRU_PERICLKSEL_CON01_CLK_SARADC_DIV_SHIFT      (0U)
#define PERICRU_PERICLKSEL_CON01_CLK_SARADC_DIV_MASK       (0x7U << PERICRU_PERICLKSEL_CON01_CLK_SARADC_DIV_SHIFT)      /* 0x00000007 */
#define PERICRU_PERICLKSEL_CON01_CLK_USER_OTPC_NS_DIV_SHIFT (4U)
#define PERICRU_PERICLKSEL_CON01_CLK_USER_OTPC_NS_DIV_MASK (0x7U << PERICRU_PERICLKSEL_CON01_CLK_USER_OTPC_NS_DIV_SHIFT) /* 0x00000070 */
#define PERICRU_PERICLKSEL_CON01_CLK_USER_OTPC_S_DIV_SHIFT (8U)
#define PERICRU_PERICLKSEL_CON01_CLK_USER_OTPC_S_DIV_MASK  (0x7U << PERICRU_PERICLKSEL_CON01_CLK_USER_OTPC_S_DIV_SHIFT) /* 0x00000700 */
/* PERICLKSEL_CON02 */
#define PERICRU_PERICLKSEL_CON02_OFFSET                    (0x308U)
#define PERICRU_PERICLKSEL_CON02_MCLK_ACODEC_TX_DIV_SHIFT  (0U)
#define PERICRU_PERICLKSEL_CON02_MCLK_ACODEC_TX_DIV_MASK   (0x7U << PERICRU_PERICLKSEL_CON02_MCLK_ACODEC_TX_DIV_SHIFT)  /* 0x00000007 */
#define PERICRU_PERICLKSEL_CON02_PCLK_RTC_ROOT_DIV_SHIFT   (12U)
#define PERICRU_PERICLKSEL_CON02_PCLK_RTC_ROOT_DIV_MASK    (0xFU << PERICRU_PERICLKSEL_CON02_PCLK_RTC_ROOT_DIV_SHIFT)   /* 0x0000F000 */
/* PERICLKSEL_CON03 */
#define PERICRU_PERICLKSEL_CON03_OFFSET                    (0x30CU)
#define PERICRU_PERICLKSEL_CON03_CLK_TESTOUT_PERI_DIV_SHIFT (4U)
#define PERICRU_PERICLKSEL_CON03_CLK_TESTOUT_PERI_DIV_MASK (0x1FU << PERICRU_PERICLKSEL_CON03_CLK_TESTOUT_PERI_DIV_SHIFT) /* 0x000001F0 */
#define PERICRU_PERICLKSEL_CON03_CLK_TESTOUT_PERI_SEL_SHIFT (10U)
#define PERICRU_PERICLKSEL_CON03_CLK_TESTOUT_PERI_SEL_MASK (0x1FU << PERICRU_PERICLKSEL_CON03_CLK_TESTOUT_PERI_SEL_SHIFT) /* 0x00007C00 */
/* PERIGATE_CON00 */
#define PERICRU_PERIGATE_CON00_OFFSET                      (0x800U)
#define PERICRU_PERIGATE_CON00_PCLK_PERI_ROOT_EN_SHIFT     (0U)
#define PERICRU_PERIGATE_CON00_PCLK_PERI_ROOT_EN_MASK      (0x1U << PERICRU_PERIGATE_CON00_PCLK_PERI_ROOT_EN_SHIFT)     /* 0x00000001 */
#define PERICRU_PERIGATE_CON00_CLK_TIMER_ROOT_EN_SHIFT     (1U)
#define PERICRU_PERIGATE_CON00_CLK_TIMER_ROOT_EN_MASK      (0x1U << PERICRU_PERIGATE_CON00_CLK_TIMER_ROOT_EN_SHIFT)     /* 0x00000002 */
#define PERICRU_PERIGATE_CON00_ACLK_PERI_BIU_EN_SHIFT      (2U)
#define PERICRU_PERIGATE_CON00_ACLK_PERI_BIU_EN_MASK       (0x1U << PERICRU_PERIGATE_CON00_ACLK_PERI_BIU_EN_SHIFT)      /* 0x00000004 */
#define PERICRU_PERIGATE_CON00_HCLK_HPMCU_BIU_EN_SHIFT     (3U)
#define PERICRU_PERIGATE_CON00_HCLK_HPMCU_BIU_EN_MASK      (0x1U << PERICRU_PERIGATE_CON00_HCLK_HPMCU_BIU_EN_SHIFT)     /* 0x00000008 */
#define PERICRU_PERIGATE_CON00_LSCLK_PERI_BIU_EN_SHIFT     (4U)
#define PERICRU_PERIGATE_CON00_LSCLK_PERI_BIU_EN_MASK      (0x1U << PERICRU_PERIGATE_CON00_LSCLK_PERI_BIU_EN_SHIFT)     /* 0x00000010 */
#define PERICRU_PERIGATE_CON00_PCLK_PERI_BIU_EN_SHIFT      (5U)
#define PERICRU_PERIGATE_CON00_PCLK_PERI_BIU_EN_MASK       (0x1U << PERICRU_PERIGATE_CON00_PCLK_PERI_BIU_EN_SHIFT)      /* 0x00000020 */
#define PERICRU_PERIGATE_CON00_PCLK_RTC_BIU_EN_SHIFT       (6U)
#define PERICRU_PERIGATE_CON00_PCLK_RTC_BIU_EN_MASK        (0x1U << PERICRU_PERIGATE_CON00_PCLK_RTC_BIU_EN_SHIFT)       /* 0x00000040 */
#define PERICRU_PERIGATE_CON00_HCLK_BOOTROM_EN_SHIFT       (7U)
#define PERICRU_PERIGATE_CON00_HCLK_BOOTROM_EN_MASK        (0x1U << PERICRU_PERIGATE_CON00_HCLK_BOOTROM_EN_SHIFT)       /* 0x00000080 */
#define PERICRU_PERIGATE_CON00_PCLK_RTC_ROOT_EN_SHIFT      (8U)
#define PERICRU_PERIGATE_CON00_PCLK_RTC_ROOT_EN_MASK       (0x1U << PERICRU_PERIGATE_CON00_PCLK_RTC_ROOT_EN_SHIFT)      /* 0x00000100 */
/* PERIGATE_CON01 */
#define PERICRU_PERIGATE_CON01_OFFSET                      (0x804U)
#define PERICRU_PERIGATE_CON01_PCLK_TIMER_EN_SHIFT         (0U)
#define PERICRU_PERIGATE_CON01_PCLK_TIMER_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_PCLK_TIMER_EN_SHIFT)         /* 0x00000001 */
#define PERICRU_PERIGATE_CON01_CLK_TIMER0_EN_SHIFT         (1U)
#define PERICRU_PERIGATE_CON01_CLK_TIMER0_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_CLK_TIMER0_EN_SHIFT)         /* 0x00000002 */
#define PERICRU_PERIGATE_CON01_CLK_TIMER1_EN_SHIFT         (2U)
#define PERICRU_PERIGATE_CON01_CLK_TIMER1_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_CLK_TIMER1_EN_SHIFT)         /* 0x00000004 */
#define PERICRU_PERIGATE_CON01_CLK_TIMER2_EN_SHIFT         (3U)
#define PERICRU_PERIGATE_CON01_CLK_TIMER2_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_CLK_TIMER2_EN_SHIFT)         /* 0x00000008 */
#define PERICRU_PERIGATE_CON01_CLK_TIMER3_EN_SHIFT         (4U)
#define PERICRU_PERIGATE_CON01_CLK_TIMER3_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_CLK_TIMER3_EN_SHIFT)         /* 0x00000010 */
#define PERICRU_PERIGATE_CON01_CLK_TIMER4_EN_SHIFT         (5U)
#define PERICRU_PERIGATE_CON01_CLK_TIMER4_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_CLK_TIMER4_EN_SHIFT)         /* 0x00000020 */
#define PERICRU_PERIGATE_CON01_CLK_TIMER5_EN_SHIFT         (6U)
#define PERICRU_PERIGATE_CON01_CLK_TIMER5_EN_MASK          (0x1U << PERICRU_PERIGATE_CON01_CLK_TIMER5_EN_SHIFT)         /* 0x00000040 */
#define PERICRU_PERIGATE_CON01_PCLK_STIMER_EN_SHIFT        (7U)
#define PERICRU_PERIGATE_CON01_PCLK_STIMER_EN_MASK         (0x1U << PERICRU_PERIGATE_CON01_PCLK_STIMER_EN_SHIFT)        /* 0x00000080 */
#define PERICRU_PERIGATE_CON01_CLK_STIMER0_EN_SHIFT        (8U)
#define PERICRU_PERIGATE_CON01_CLK_STIMER0_EN_MASK         (0x1U << PERICRU_PERIGATE_CON01_CLK_STIMER0_EN_SHIFT)        /* 0x00000100 */
#define PERICRU_PERIGATE_CON01_CLK_STIMER1_EN_SHIFT        (9U)
#define PERICRU_PERIGATE_CON01_CLK_STIMER1_EN_MASK         (0x1U << PERICRU_PERIGATE_CON01_CLK_STIMER1_EN_SHIFT)        /* 0x00000200 */
/* PERIGATE_CON02 */
#define PERICRU_PERIGATE_CON02_OFFSET                      (0x808U)
#define PERICRU_PERIGATE_CON02_PCLK_WDT_NS_EN_SHIFT        (0U)
#define PERICRU_PERIGATE_CON02_PCLK_WDT_NS_EN_MASK         (0x1U << PERICRU_PERIGATE_CON02_PCLK_WDT_NS_EN_SHIFT)        /* 0x00000001 */
#define PERICRU_PERIGATE_CON02_TCLK_WDT_NS_EN_SHIFT        (1U)
#define PERICRU_PERIGATE_CON02_TCLK_WDT_NS_EN_MASK         (0x1U << PERICRU_PERIGATE_CON02_TCLK_WDT_NS_EN_SHIFT)        /* 0x00000002 */
#define PERICRU_PERIGATE_CON02_PCLK_WDT_S_EN_SHIFT         (2U)
#define PERICRU_PERIGATE_CON02_PCLK_WDT_S_EN_MASK          (0x1U << PERICRU_PERIGATE_CON02_PCLK_WDT_S_EN_SHIFT)         /* 0x00000004 */
#define PERICRU_PERIGATE_CON02_TCLK_WDT_S_EN_SHIFT         (3U)
#define PERICRU_PERIGATE_CON02_TCLK_WDT_S_EN_MASK          (0x1U << PERICRU_PERIGATE_CON02_TCLK_WDT_S_EN_SHIFT)         /* 0x00000008 */
#define PERICRU_PERIGATE_CON02_PCLK_WDT_HPMCU_EN_SHIFT     (4U)
#define PERICRU_PERIGATE_CON02_PCLK_WDT_HPMCU_EN_MASK      (0x1U << PERICRU_PERIGATE_CON02_PCLK_WDT_HPMCU_EN_SHIFT)     /* 0x00000010 */
#define PERICRU_PERIGATE_CON02_TCLK_WDT_HPMCU_EN_SHIFT     (5U)
#define PERICRU_PERIGATE_CON02_TCLK_WDT_HPMCU_EN_MASK      (0x1U << PERICRU_PERIGATE_CON02_TCLK_WDT_HPMCU_EN_SHIFT)     /* 0x00000020 */
#define PERICRU_PERIGATE_CON02_PCLK_I2C1_EN_SHIFT          (6U)
#define PERICRU_PERIGATE_CON02_PCLK_I2C1_EN_MASK           (0x1U << PERICRU_PERIGATE_CON02_PCLK_I2C1_EN_SHIFT)          /* 0x00000040 */
#define PERICRU_PERIGATE_CON02_CLK_I2C1_EN_SHIFT           (7U)
#define PERICRU_PERIGATE_CON02_CLK_I2C1_EN_MASK            (0x1U << PERICRU_PERIGATE_CON02_CLK_I2C1_EN_SHIFT)           /* 0x00000080 */
#define PERICRU_PERIGATE_CON02_PCLK_I2C2_EN_SHIFT          (8U)
#define PERICRU_PERIGATE_CON02_PCLK_I2C2_EN_MASK           (0x1U << PERICRU_PERIGATE_CON02_PCLK_I2C2_EN_SHIFT)          /* 0x00000100 */
#define PERICRU_PERIGATE_CON02_CLK_I2C2_EN_SHIFT           (9U)
#define PERICRU_PERIGATE_CON02_CLK_I2C2_EN_MASK            (0x1U << PERICRU_PERIGATE_CON02_CLK_I2C2_EN_SHIFT)           /* 0x00000200 */
#define PERICRU_PERIGATE_CON02_PCLK_I2C3_EN_SHIFT          (10U)
#define PERICRU_PERIGATE_CON02_PCLK_I2C3_EN_MASK           (0x1U << PERICRU_PERIGATE_CON02_PCLK_I2C3_EN_SHIFT)          /* 0x00000400 */
#define PERICRU_PERIGATE_CON02_CLK_I2C3_EN_SHIFT           (11U)
#define PERICRU_PERIGATE_CON02_CLK_I2C3_EN_MASK            (0x1U << PERICRU_PERIGATE_CON02_CLK_I2C3_EN_SHIFT)           /* 0x00000800 */
#define PERICRU_PERIGATE_CON02_PCLK_I2C4_EN_SHIFT          (12U)
#define PERICRU_PERIGATE_CON02_PCLK_I2C4_EN_MASK           (0x1U << PERICRU_PERIGATE_CON02_PCLK_I2C4_EN_SHIFT)          /* 0x00001000 */
#define PERICRU_PERIGATE_CON02_CLK_I2C4_EN_SHIFT           (13U)
#define PERICRU_PERIGATE_CON02_CLK_I2C4_EN_MASK            (0x1U << PERICRU_PERIGATE_CON02_CLK_I2C4_EN_SHIFT)           /* 0x00002000 */
/* PERIGATE_CON03 */
#define PERICRU_PERIGATE_CON03_OFFSET                      (0x80CU)
#define PERICRU_PERIGATE_CON03_PCLK_UART2_EN_SHIFT         (0U)
#define PERICRU_PERIGATE_CON03_PCLK_UART2_EN_MASK          (0x1U << PERICRU_PERIGATE_CON03_PCLK_UART2_EN_SHIFT)         /* 0x00000001 */
#define PERICRU_PERIGATE_CON03_PCLK_UART1_EN_SHIFT         (2U)
#define PERICRU_PERIGATE_CON03_PCLK_UART1_EN_MASK          (0x1U << PERICRU_PERIGATE_CON03_PCLK_UART1_EN_SHIFT)         /* 0x00000004 */
#define PERICRU_PERIGATE_CON03_PCLK_SPI0_EN_SHIFT          (10U)
#define PERICRU_PERIGATE_CON03_PCLK_SPI0_EN_MASK           (0x1U << PERICRU_PERIGATE_CON03_PCLK_SPI0_EN_SHIFT)          /* 0x00000400 */
/* PERIGATE_CON04 */
#define PERICRU_PERIGATE_CON04_OFFSET                      (0x810U)
#define PERICRU_PERIGATE_CON04_PCLK_PWM1_EN_SHIFT          (6U)
#define PERICRU_PERIGATE_CON04_PCLK_PWM1_EN_MASK           (0x1U << PERICRU_PERIGATE_CON04_PCLK_PWM1_EN_SHIFT)          /* 0x00000040 */
#define PERICRU_PERIGATE_CON04_CLK_OSC_PWM1_EN_SHIFT       (8U)
#define PERICRU_PERIGATE_CON04_CLK_OSC_PWM1_EN_MASK        (0x1U << PERICRU_PERIGATE_CON04_CLK_OSC_PWM1_EN_SHIFT)       /* 0x00000100 */
#define PERICRU_PERIGATE_CON04_PCLK_PWM2_EN_SHIFT          (12U)
#define PERICRU_PERIGATE_CON04_PCLK_PWM2_EN_MASK           (0x1U << PERICRU_PERIGATE_CON04_PCLK_PWM2_EN_SHIFT)          /* 0x00001000 */
#define PERICRU_PERIGATE_CON04_CLK_OSC_PWM2_EN_SHIFT       (13U)
#define PERICRU_PERIGATE_CON04_CLK_OSC_PWM2_EN_MASK        (0x1U << PERICRU_PERIGATE_CON04_CLK_OSC_PWM2_EN_SHIFT)       /* 0x00002000 */
/* PERIGATE_CON05 */
#define PERICRU_PERIGATE_CON05_OFFSET                      (0x814U)
#define PERICRU_PERIGATE_CON05_ACLK_RKDMA_EN_SHIFT         (8U)
#define PERICRU_PERIGATE_CON05_ACLK_RKDMA_EN_MASK          (0x1U << PERICRU_PERIGATE_CON05_ACLK_RKDMA_EN_SHIFT)         /* 0x00000100 */
#define PERICRU_PERIGATE_CON05_PCLK_TSADC_EN_SHIFT         (9U)
#define PERICRU_PERIGATE_CON05_PCLK_TSADC_EN_MASK          (0x1U << PERICRU_PERIGATE_CON05_PCLK_TSADC_EN_SHIFT)         /* 0x00000200 */
#define PERICRU_PERIGATE_CON05_CLK_TSADC_EN_SHIFT          (10U)
#define PERICRU_PERIGATE_CON05_CLK_TSADC_EN_MASK           (0x1U << PERICRU_PERIGATE_CON05_CLK_TSADC_EN_SHIFT)          /* 0x00000400 */
#define PERICRU_PERIGATE_CON05_CLK_TSADC_TSEN_EN_SHIFT     (11U)
#define PERICRU_PERIGATE_CON05_CLK_TSADC_TSEN_EN_MASK      (0x1U << PERICRU_PERIGATE_CON05_CLK_TSADC_TSEN_EN_SHIFT)     /* 0x00000800 */
#define PERICRU_PERIGATE_CON05_PCLK_SARADC_EN_SHIFT        (12U)
#define PERICRU_PERIGATE_CON05_PCLK_SARADC_EN_MASK         (0x1U << PERICRU_PERIGATE_CON05_PCLK_SARADC_EN_SHIFT)        /* 0x00001000 */
#define PERICRU_PERIGATE_CON05_CLK_SARADC_EN_SHIFT         (13U)
#define PERICRU_PERIGATE_CON05_CLK_SARADC_EN_MASK          (0x1U << PERICRU_PERIGATE_CON05_CLK_SARADC_EN_SHIFT)         /* 0x00002000 */
/* PERIGATE_CON06 */
#define PERICRU_PERIGATE_CON06_OFFSET                      (0x818U)
#define PERICRU_PERIGATE_CON06_PCLK_RTC_TEST_EN_SHIFT      (1U)
#define PERICRU_PERIGATE_CON06_PCLK_RTC_TEST_EN_MASK       (0x1U << PERICRU_PERIGATE_CON06_PCLK_RTC_TEST_EN_SHIFT)      /* 0x00000002 */
#define PERICRU_PERIGATE_CON06_PCLK_GPIO2_EN_SHIFT         (3U)
#define PERICRU_PERIGATE_CON06_PCLK_GPIO2_EN_MASK          (0x1U << PERICRU_PERIGATE_CON06_PCLK_GPIO2_EN_SHIFT)         /* 0x00000008 */
#define PERICRU_PERIGATE_CON06_DBCLK_GPIO2_EN_SHIFT        (4U)
#define PERICRU_PERIGATE_CON06_DBCLK_GPIO2_EN_MASK         (0x1U << PERICRU_PERIGATE_CON06_DBCLK_GPIO2_EN_SHIFT)        /* 0x00000010 */
#define PERICRU_PERIGATE_CON06_PCLK_IOC_VCCIO6_EN_SHIFT    (5U)
#define PERICRU_PERIGATE_CON06_PCLK_IOC_VCCIO6_EN_MASK     (0x1U << PERICRU_PERIGATE_CON06_PCLK_IOC_VCCIO6_EN_SHIFT)    /* 0x00000020 */
#define PERICRU_PERIGATE_CON06_PCLK_PERI_SGRF_EN_SHIFT     (6U)
#define PERICRU_PERIGATE_CON06_PCLK_PERI_SGRF_EN_MASK      (0x1U << PERICRU_PERIGATE_CON06_PCLK_PERI_SGRF_EN_SHIFT)     /* 0x00000040 */
#define PERICRU_PERIGATE_CON06_PCLK_PERI_GRF_EN_SHIFT      (7U)
#define PERICRU_PERIGATE_CON06_PCLK_PERI_GRF_EN_MASK       (0x1U << PERICRU_PERIGATE_CON06_PCLK_PERI_GRF_EN_SHIFT)      /* 0x00000080 */
#define PERICRU_PERIGATE_CON06_PCLK_CRU_PERI_EN_SHIFT      (8U)
#define PERICRU_PERIGATE_CON06_PCLK_CRU_PERI_EN_MASK       (0x1U << PERICRU_PERIGATE_CON06_PCLK_CRU_PERI_EN_SHIFT)      /* 0x00000100 */
#define PERICRU_PERIGATE_CON06_ACLK_USBOTG_EN_SHIFT        (9U)
#define PERICRU_PERIGATE_CON06_ACLK_USBOTG_EN_MASK         (0x1U << PERICRU_PERIGATE_CON06_ACLK_USBOTG_EN_SHIFT)        /* 0x00000200 */
#define PERICRU_PERIGATE_CON06_CLK_REF_USBOTG_EN_SHIFT     (10U)
#define PERICRU_PERIGATE_CON06_CLK_REF_USBOTG_EN_MASK      (0x1U << PERICRU_PERIGATE_CON06_CLK_REF_USBOTG_EN_SHIFT)     /* 0x00000400 */
/* PERIGATE_CON07 */
#define PERICRU_PERIGATE_CON07_OFFSET                      (0x81CU)
#define PERICRU_PERIGATE_CON07_HCLK_SDMMC1_EN_SHIFT        (0U)
#define PERICRU_PERIGATE_CON07_HCLK_SDMMC1_EN_MASK         (0x1U << PERICRU_PERIGATE_CON07_HCLK_SDMMC1_EN_SHIFT)        /* 0x00000001 */
#define PERICRU_PERIGATE_CON07_HCLK_SAI_EN_SHIFT           (1U)
#define PERICRU_PERIGATE_CON07_HCLK_SAI_EN_MASK            (0x1U << PERICRU_PERIGATE_CON07_HCLK_SAI_EN_SHIFT)           /* 0x00000002 */
#define PERICRU_PERIGATE_CON07_MCLK_SAI_EN_SHIFT           (2U)
#define PERICRU_PERIGATE_CON07_MCLK_SAI_EN_MASK            (0x1U << PERICRU_PERIGATE_CON07_MCLK_SAI_EN_SHIFT)           /* 0x00000004 */
/* PERIGATE_CON08 */
#define PERICRU_PERIGATE_CON08_OFFSET                      (0x820U)
#define PERICRU_PERIGATE_CON08_ACLK_CRYPTO_EN_SHIFT        (2U)
#define PERICRU_PERIGATE_CON08_ACLK_CRYPTO_EN_MASK         (0x1U << PERICRU_PERIGATE_CON08_ACLK_CRYPTO_EN_SHIFT)        /* 0x00000004 */
#define PERICRU_PERIGATE_CON08_HCLK_CRYPTO_EN_SHIFT        (3U)
#define PERICRU_PERIGATE_CON08_HCLK_CRYPTO_EN_MASK         (0x1U << PERICRU_PERIGATE_CON08_HCLK_CRYPTO_EN_SHIFT)        /* 0x00000008 */
#define PERICRU_PERIGATE_CON08_HCLK_RK_RNG_NS_EN_SHIFT     (4U)
#define PERICRU_PERIGATE_CON08_HCLK_RK_RNG_NS_EN_MASK      (0x1U << PERICRU_PERIGATE_CON08_HCLK_RK_RNG_NS_EN_SHIFT)     /* 0x00000010 */
#define PERICRU_PERIGATE_CON08_HCLK_RK_RNG_S_EN_SHIFT      (5U)
#define PERICRU_PERIGATE_CON08_HCLK_RK_RNG_S_EN_MASK       (0x1U << PERICRU_PERIGATE_CON08_HCLK_RK_RNG_S_EN_SHIFT)      /* 0x00000020 */
#define PERICRU_PERIGATE_CON08_PCLK_OTPC_NS_EN_SHIFT       (6U)
#define PERICRU_PERIGATE_CON08_PCLK_OTPC_NS_EN_MASK        (0x1U << PERICRU_PERIGATE_CON08_PCLK_OTPC_NS_EN_SHIFT)       /* 0x00000040 */
#define PERICRU_PERIGATE_CON08_CLK_OTPC_ROOT_NS_EN_SHIFT   (7U)
#define PERICRU_PERIGATE_CON08_CLK_OTPC_ROOT_NS_EN_MASK    (0x1U << PERICRU_PERIGATE_CON08_CLK_OTPC_ROOT_NS_EN_SHIFT)   /* 0x00000080 */
#define PERICRU_PERIGATE_CON08_CLK_SBPI_OTPC_NS_EN_SHIFT   (8U)
#define PERICRU_PERIGATE_CON08_CLK_SBPI_OTPC_NS_EN_MASK    (0x1U << PERICRU_PERIGATE_CON08_CLK_SBPI_OTPC_NS_EN_SHIFT)   /* 0x00000100 */
#define PERICRU_PERIGATE_CON08_CLK_USER_OTPC_NS_EN_SHIFT   (9U)
#define PERICRU_PERIGATE_CON08_CLK_USER_OTPC_NS_EN_MASK    (0x1U << PERICRU_PERIGATE_CON08_CLK_USER_OTPC_NS_EN_SHIFT)   /* 0x00000200 */
#define PERICRU_PERIGATE_CON08_PCLK_OTPC_S_EN_SHIFT        (10U)
#define PERICRU_PERIGATE_CON08_PCLK_OTPC_S_EN_MASK         (0x1U << PERICRU_PERIGATE_CON08_PCLK_OTPC_S_EN_SHIFT)        /* 0x00000400 */
#define PERICRU_PERIGATE_CON08_CLK_OTPC_ROOT_S_EN_SHIFT    (11U)
#define PERICRU_PERIGATE_CON08_CLK_OTPC_ROOT_S_EN_MASK     (0x1U << PERICRU_PERIGATE_CON08_CLK_OTPC_ROOT_S_EN_SHIFT)    /* 0x00000800 */
#define PERICRU_PERIGATE_CON08_CLK_SBPI_OTPC_S_EN_SHIFT    (12U)
#define PERICRU_PERIGATE_CON08_CLK_SBPI_OTPC_S_EN_MASK     (0x1U << PERICRU_PERIGATE_CON08_CLK_SBPI_OTPC_S_EN_SHIFT)    /* 0x00001000 */
#define PERICRU_PERIGATE_CON08_CLK_USER_OTPC_S_EN_SHIFT    (13U)
#define PERICRU_PERIGATE_CON08_CLK_USER_OTPC_S_EN_MASK     (0x1U << PERICRU_PERIGATE_CON08_CLK_USER_OTPC_S_EN_SHIFT)    /* 0x00002000 */
#define PERICRU_PERIGATE_CON08_CLK_OTPC_ARB_EN_SHIFT       (14U)
#define PERICRU_PERIGATE_CON08_CLK_OTPC_ARB_EN_MASK        (0x1U << PERICRU_PERIGATE_CON08_CLK_OTPC_ARB_EN_SHIFT)       /* 0x00004000 */
#define PERICRU_PERIGATE_CON08_PCLK_OTP_MASK_EN_SHIFT      (15U)
#define PERICRU_PERIGATE_CON08_PCLK_OTP_MASK_EN_MASK       (0x1U << PERICRU_PERIGATE_CON08_PCLK_OTP_MASK_EN_SHIFT)      /* 0x00008000 */
/* PERIGATE_CON09 */
#define PERICRU_PERIGATE_CON09_OFFSET                      (0x824U)
#define PERICRU_PERIGATE_CON09_HCLK_RGA_EN_SHIFT           (0U)
#define PERICRU_PERIGATE_CON09_HCLK_RGA_EN_MASK            (0x1U << PERICRU_PERIGATE_CON09_HCLK_RGA_EN_SHIFT)           /* 0x00000001 */
#define PERICRU_PERIGATE_CON09_ACLK_RGA_EN_SHIFT           (1U)
#define PERICRU_PERIGATE_CON09_ACLK_RGA_EN_MASK            (0x1U << PERICRU_PERIGATE_CON09_ACLK_RGA_EN_SHIFT)           /* 0x00000002 */
#define PERICRU_PERIGATE_CON09_ACLK_MAC_EN_SHIFT           (3U)
#define PERICRU_PERIGATE_CON09_ACLK_MAC_EN_MASK            (0x1U << PERICRU_PERIGATE_CON09_ACLK_MAC_EN_SHIFT)           /* 0x00000008 */
#define PERICRU_PERIGATE_CON09_PCLK_MAC_EN_SHIFT           (4U)
#define PERICRU_PERIGATE_CON09_PCLK_MAC_EN_MASK            (0x1U << PERICRU_PERIGATE_CON09_PCLK_MAC_EN_SHIFT)           /* 0x00000010 */
#define PERICRU_PERIGATE_CON09_CLK_MACPHY_EN_SHIFT         (11U)
#define PERICRU_PERIGATE_CON09_CLK_MACPHY_EN_MASK          (0x1U << PERICRU_PERIGATE_CON09_CLK_MACPHY_EN_SHIFT)         /* 0x00000800 */
/* PERIGATE_CON10 */
#define PERICRU_PERIGATE_CON10_OFFSET                      (0x828U)
#define PERICRU_PERIGATE_CON10_ACLK_SPINLOCK_EN_SHIFT      (0U)
#define PERICRU_PERIGATE_CON10_ACLK_SPINLOCK_EN_MASK       (0x1U << PERICRU_PERIGATE_CON10_ACLK_SPINLOCK_EN_SHIFT)      /* 0x00000001 */
#define PERICRU_PERIGATE_CON10_HCLK_CACHE_EN_SHIFT         (1U)
#define PERICRU_PERIGATE_CON10_HCLK_CACHE_EN_MASK          (0x1U << PERICRU_PERIGATE_CON10_HCLK_CACHE_EN_SHIFT)         /* 0x00000002 */
#define PERICRU_PERIGATE_CON10_PCLK_HPMCU_MAILBOX_EN_SHIFT (2U)
#define PERICRU_PERIGATE_CON10_PCLK_HPMCU_MAILBOX_EN_MASK  (0x1U << PERICRU_PERIGATE_CON10_PCLK_HPMCU_MAILBOX_EN_SHIFT) /* 0x00000004 */
#define PERICRU_PERIGATE_CON10_PCLK_HPMCU_INTMUX_EN_SHIFT  (3U)
#define PERICRU_PERIGATE_CON10_PCLK_HPMCU_INTMUX_EN_MASK   (0x1U << PERICRU_PERIGATE_CON10_PCLK_HPMCU_INTMUX_EN_SHIFT)  /* 0x00000008 */
#define PERICRU_PERIGATE_CON10_CLK_HPMCU_EN_SHIFT          (4U)
#define PERICRU_PERIGATE_CON10_CLK_HPMCU_EN_MASK           (0x1U << PERICRU_PERIGATE_CON10_CLK_HPMCU_EN_SHIFT)          /* 0x00000010 */
#define PERICRU_PERIGATE_CON10_CLK_HPMCU_RTC_EN_SHIFT      (8U)
#define PERICRU_PERIGATE_CON10_CLK_HPMCU_RTC_EN_MASK       (0x1U << PERICRU_PERIGATE_CON10_CLK_HPMCU_RTC_EN_SHIFT)      /* 0x00000100 */
/* PERIGATE_CON11 */
#define PERICRU_PERIGATE_CON11_OFFSET                      (0x82CU)
#define PERICRU_PERIGATE_CON11_DCLK_DECOM_EN_SHIFT         (0U)
#define PERICRU_PERIGATE_CON11_DCLK_DECOM_EN_MASK          (0x1U << PERICRU_PERIGATE_CON11_DCLK_DECOM_EN_SHIFT)         /* 0x00000001 */
#define PERICRU_PERIGATE_CON11_ACLK_DECOM_EN_SHIFT         (1U)
#define PERICRU_PERIGATE_CON11_ACLK_DECOM_EN_MASK          (0x1U << PERICRU_PERIGATE_CON11_ACLK_DECOM_EN_SHIFT)         /* 0x00000002 */
#define PERICRU_PERIGATE_CON11_PCLK_DECOM_EN_SHIFT         (2U)
#define PERICRU_PERIGATE_CON11_PCLK_DECOM_EN_MASK          (0x1U << PERICRU_PERIGATE_CON11_PCLK_DECOM_EN_SHIFT)         /* 0x00000004 */
#define PERICRU_PERIGATE_CON11_ACLK_SYS_SRAM_EN_SHIFT      (3U)
#define PERICRU_PERIGATE_CON11_ACLK_SYS_SRAM_EN_MASK       (0x1U << PERICRU_PERIGATE_CON11_ACLK_SYS_SRAM_EN_SHIFT)      /* 0x00000008 */
#define PERICRU_PERIGATE_CON11_PCLK_DMA2DDR_EN_SHIFT       (4U)
#define PERICRU_PERIGATE_CON11_PCLK_DMA2DDR_EN_MASK        (0x1U << PERICRU_PERIGATE_CON11_PCLK_DMA2DDR_EN_SHIFT)       /* 0x00000010 */
#define PERICRU_PERIGATE_CON11_ACLK_DMA2DDR_EN_SHIFT       (5U)
#define PERICRU_PERIGATE_CON11_ACLK_DMA2DDR_EN_MASK        (0x1U << PERICRU_PERIGATE_CON11_ACLK_DMA2DDR_EN_SHIFT)       /* 0x00000020 */
#define PERICRU_PERIGATE_CON11_PCLK_DCF_EN_SHIFT           (6U)
#define PERICRU_PERIGATE_CON11_PCLK_DCF_EN_MASK            (0x1U << PERICRU_PERIGATE_CON11_PCLK_DCF_EN_SHIFT)           /* 0x00000040 */
#define PERICRU_PERIGATE_CON11_ACLK_DCF_EN_SHIFT           (7U)
#define PERICRU_PERIGATE_CON11_ACLK_DCF_EN_MASK            (0x1U << PERICRU_PERIGATE_CON11_ACLK_DCF_EN_SHIFT)           /* 0x00000080 */
#define PERICRU_PERIGATE_CON11_MCLK_ACODEC_TX_EN_SHIFT     (9U)
#define PERICRU_PERIGATE_CON11_MCLK_ACODEC_TX_EN_MASK      (0x1U << PERICRU_PERIGATE_CON11_MCLK_ACODEC_TX_EN_SHIFT)     /* 0x00000200 */
#define PERICRU_PERIGATE_CON11_CLK_REF_USBPHY_EN_SHIFT     (12U)
#define PERICRU_PERIGATE_CON11_CLK_REF_USBPHY_EN_MASK      (0x1U << PERICRU_PERIGATE_CON11_CLK_REF_USBPHY_EN_SHIFT)     /* 0x00001000 */
#define PERICRU_PERIGATE_CON11_CLK_TESTOUT_PERI_EN_SHIFT   (15U)
#define PERICRU_PERIGATE_CON11_CLK_TESTOUT_PERI_EN_MASK    (0x1U << PERICRU_PERIGATE_CON11_CLK_TESTOUT_PERI_EN_SHIFT)   /* 0x00008000 */
/* PERISOFTRST_CON00 */
#define PERICRU_PERISOFTRST_CON00_OFFSET                   (0xA00U)
#define PERICRU_PERISOFTRST_CON00_ARESETN_PERI_BIU_SHIFT   (2U)
#define PERICRU_PERISOFTRST_CON00_ARESETN_PERI_BIU_MASK    (0x1U << PERICRU_PERISOFTRST_CON00_ARESETN_PERI_BIU_SHIFT)   /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON00_HRESETN_HPMCU_BIU_SHIFT  (3U)
#define PERICRU_PERISOFTRST_CON00_HRESETN_HPMCU_BIU_MASK   (0x1U << PERICRU_PERISOFTRST_CON00_HRESETN_HPMCU_BIU_SHIFT)  /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON00_LSRESETN_PERI_BIU_SHIFT  (4U)
#define PERICRU_PERISOFTRST_CON00_LSRESETN_PERI_BIU_MASK   (0x1U << PERICRU_PERISOFTRST_CON00_LSRESETN_PERI_BIU_SHIFT)  /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON00_PRESETN_PERI_BIU_SHIFT   (5U)
#define PERICRU_PERISOFTRST_CON00_PRESETN_PERI_BIU_MASK    (0x1U << PERICRU_PERISOFTRST_CON00_PRESETN_PERI_BIU_SHIFT)   /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON00_PRESETN_RTC_BIU_SHIFT    (6U)
#define PERICRU_PERISOFTRST_CON00_PRESETN_RTC_BIU_MASK     (0x1U << PERICRU_PERISOFTRST_CON00_PRESETN_RTC_BIU_SHIFT)    /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON00_HRESETN_BOOTROM_SHIFT    (7U)
#define PERICRU_PERISOFTRST_CON00_HRESETN_BOOTROM_MASK     (0x1U << PERICRU_PERISOFTRST_CON00_HRESETN_BOOTROM_SHIFT)    /* 0x00000080 */
/* PERISOFTRST_CON01 */
#define PERICRU_PERISOFTRST_CON01_OFFSET                   (0xA04U)
#define PERICRU_PERISOFTRST_CON01_PRESETN_TIMER_SHIFT      (0U)
#define PERICRU_PERISOFTRST_CON01_PRESETN_TIMER_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_PRESETN_TIMER_SHIFT)      /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER0_SHIFT      (1U)
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER0_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_TIMER0_SHIFT)      /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER1_SHIFT      (2U)
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER1_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_TIMER1_SHIFT)      /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER2_SHIFT      (3U)
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER2_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_TIMER2_SHIFT)      /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER3_SHIFT      (4U)
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER3_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_TIMER3_SHIFT)      /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER4_SHIFT      (5U)
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER4_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_TIMER4_SHIFT)      /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER5_SHIFT      (6U)
#define PERICRU_PERISOFTRST_CON01_RESETN_TIMER5_MASK       (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_TIMER5_SHIFT)      /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON01_PRESETN_STIMER_SHIFT     (7U)
#define PERICRU_PERISOFTRST_CON01_PRESETN_STIMER_MASK      (0x1U << PERICRU_PERISOFTRST_CON01_PRESETN_STIMER_SHIFT)     /* 0x00000080 */
#define PERICRU_PERISOFTRST_CON01_RESETN_STIMER0_SHIFT     (8U)
#define PERICRU_PERISOFTRST_CON01_RESETN_STIMER0_MASK      (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_STIMER0_SHIFT)     /* 0x00000100 */
#define PERICRU_PERISOFTRST_CON01_RESETN_STIMER1_SHIFT     (9U)
#define PERICRU_PERISOFTRST_CON01_RESETN_STIMER1_MASK      (0x1U << PERICRU_PERISOFTRST_CON01_RESETN_STIMER1_SHIFT)     /* 0x00000200 */
/* PERISOFTRST_CON02 */
#define PERICRU_PERISOFTRST_CON02_OFFSET                   (0xA08U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_WDT_NS_SHIFT     (0U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_WDT_NS_MASK      (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_WDT_NS_SHIFT)     /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON02_TRESETN_WDT_NS_SHIFT     (1U)
#define PERICRU_PERISOFTRST_CON02_TRESETN_WDT_NS_MASK      (0x1U << PERICRU_PERISOFTRST_CON02_TRESETN_WDT_NS_SHIFT)     /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON02_PRESETN_WDT_S_SHIFT      (2U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_WDT_S_MASK       (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_WDT_S_SHIFT)      /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON02_TRESETN_WDT_S_SHIFT      (3U)
#define PERICRU_PERISOFTRST_CON02_TRESETN_WDT_S_MASK       (0x1U << PERICRU_PERISOFTRST_CON02_TRESETN_WDT_S_SHIFT)      /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON02_PRESETN_WDT_HPMCU_SHIFT  (4U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_WDT_HPMCU_MASK   (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_WDT_HPMCU_SHIFT)  /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON02_TRESETN_WDT_HPMCU_SHIFT  (5U)
#define PERICRU_PERISOFTRST_CON02_TRESETN_WDT_HPMCU_MASK   (0x1U << PERICRU_PERISOFTRST_CON02_TRESETN_WDT_HPMCU_SHIFT)  /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C1_SHIFT       (6U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C1_MASK        (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_I2C1_SHIFT)       /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C1_SHIFT        (7U)
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C1_MASK         (0x1U << PERICRU_PERISOFTRST_CON02_RESETN_I2C1_SHIFT)        /* 0x00000080 */
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C2_SHIFT       (8U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C2_MASK        (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_I2C2_SHIFT)       /* 0x00000100 */
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C2_SHIFT        (9U)
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C2_MASK         (0x1U << PERICRU_PERISOFTRST_CON02_RESETN_I2C2_SHIFT)        /* 0x00000200 */
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C3_SHIFT       (10U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C3_MASK        (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_I2C3_SHIFT)       /* 0x00000400 */
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C3_SHIFT        (11U)
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C3_MASK         (0x1U << PERICRU_PERISOFTRST_CON02_RESETN_I2C3_SHIFT)        /* 0x00000800 */
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C4_SHIFT       (12U)
#define PERICRU_PERISOFTRST_CON02_PRESETN_I2C4_MASK        (0x1U << PERICRU_PERISOFTRST_CON02_PRESETN_I2C4_SHIFT)       /* 0x00001000 */
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C4_SHIFT        (13U)
#define PERICRU_PERISOFTRST_CON02_RESETN_I2C4_MASK         (0x1U << PERICRU_PERISOFTRST_CON02_RESETN_I2C4_SHIFT)        /* 0x00002000 */
/* PERISOFTRST_CON03 */
#define PERICRU_PERISOFTRST_CON03_OFFSET                   (0xA0CU)
#define PERICRU_PERISOFTRST_CON03_PRESETN_UART2_SHIFT      (0U)
#define PERICRU_PERISOFTRST_CON03_PRESETN_UART2_MASK       (0x1U << PERICRU_PERISOFTRST_CON03_PRESETN_UART2_SHIFT)      /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON03_SRESETN_UART2_SHIFT      (1U)
#define PERICRU_PERISOFTRST_CON03_SRESETN_UART2_MASK       (0x1U << PERICRU_PERISOFTRST_CON03_SRESETN_UART2_SHIFT)      /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON03_PRESETN_UART1_SHIFT      (2U)
#define PERICRU_PERISOFTRST_CON03_PRESETN_UART1_MASK       (0x1U << PERICRU_PERISOFTRST_CON03_PRESETN_UART1_SHIFT)      /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON03_SRESETN_UART1_SHIFT      (3U)
#define PERICRU_PERISOFTRST_CON03_SRESETN_UART1_MASK       (0x1U << PERICRU_PERISOFTRST_CON03_SRESETN_UART1_SHIFT)      /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON03_PRESETN_SPI0_SHIFT       (10U)
#define PERICRU_PERISOFTRST_CON03_PRESETN_SPI0_MASK        (0x1U << PERICRU_PERISOFTRST_CON03_PRESETN_SPI0_SHIFT)       /* 0x00000400 */
#define PERICRU_PERISOFTRST_CON03_RESETN_SPI0_SHIFT        (11U)
#define PERICRU_PERISOFTRST_CON03_RESETN_SPI0_MASK         (0x1U << PERICRU_PERISOFTRST_CON03_RESETN_SPI0_SHIFT)        /* 0x00000800 */
/* PERISOFTRST_CON04 */
#define PERICRU_PERISOFTRST_CON04_OFFSET                   (0xA10U)
#define PERICRU_PERISOFTRST_CON04_PRESETN_PWM1_SHIFT       (6U)
#define PERICRU_PERISOFTRST_CON04_PRESETN_PWM1_MASK        (0x1U << PERICRU_PERISOFTRST_CON04_PRESETN_PWM1_SHIFT)       /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON04_RESETN_PWM1_SHIFT        (7U)
#define PERICRU_PERISOFTRST_CON04_RESETN_PWM1_MASK         (0x1U << PERICRU_PERISOFTRST_CON04_RESETN_PWM1_SHIFT)        /* 0x00000080 */
#define PERICRU_PERISOFTRST_CON04_PRESETN_PWM2_SHIFT       (12U)
#define PERICRU_PERISOFTRST_CON04_PRESETN_PWM2_MASK        (0x1U << PERICRU_PERISOFTRST_CON04_PRESETN_PWM2_SHIFT)       /* 0x00001000 */
#define PERICRU_PERISOFTRST_CON04_RESETN_PWM2_SHIFT        (13U)
#define PERICRU_PERISOFTRST_CON04_RESETN_PWM2_MASK         (0x1U << PERICRU_PERISOFTRST_CON04_RESETN_PWM2_SHIFT)        /* 0x00002000 */
/* PERISOFTRST_CON05 */
#define PERICRU_PERISOFTRST_CON05_OFFSET                   (0xA14U)
#define PERICRU_PERISOFTRST_CON05_ARESETN_RKDMA_SHIFT      (8U)
#define PERICRU_PERISOFTRST_CON05_ARESETN_RKDMA_MASK       (0x1U << PERICRU_PERISOFTRST_CON05_ARESETN_RKDMA_SHIFT)      /* 0x00000100 */
#define PERICRU_PERISOFTRST_CON05_PRESETN_TSADC_SHIFT      (9U)
#define PERICRU_PERISOFTRST_CON05_PRESETN_TSADC_MASK       (0x1U << PERICRU_PERISOFTRST_CON05_PRESETN_TSADC_SHIFT)      /* 0x00000200 */
#define PERICRU_PERISOFTRST_CON05_RESETN_TSADC_SHIFT       (10U)
#define PERICRU_PERISOFTRST_CON05_RESETN_TSADC_MASK        (0x1U << PERICRU_PERISOFTRST_CON05_RESETN_TSADC_SHIFT)       /* 0x00000400 */
#define PERICRU_PERISOFTRST_CON05_PRESETN_SARADC_SHIFT     (12U)
#define PERICRU_PERISOFTRST_CON05_PRESETN_SARADC_MASK      (0x1U << PERICRU_PERISOFTRST_CON05_PRESETN_SARADC_SHIFT)     /* 0x00001000 */
#define PERICRU_PERISOFTRST_CON05_RESETN_SARADC_SHIFT      (13U)
#define PERICRU_PERISOFTRST_CON05_RESETN_SARADC_MASK       (0x1U << PERICRU_PERISOFTRST_CON05_RESETN_SARADC_SHIFT)      /* 0x00002000 */
/* PERISOFTRST_CON06 */
#define PERICRU_PERISOFTRST_CON06_OFFSET                   (0xA18U)
#define PERICRU_PERISOFTRST_CON06_RESETN_SARADC_PHY_SHIFT  (0U)
#define PERICRU_PERISOFTRST_CON06_RESETN_SARADC_PHY_MASK   (0x1U << PERICRU_PERISOFTRST_CON06_RESETN_SARADC_PHY_SHIFT)  /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON06_PRESETN_RTC_TEST_SHIFT   (1U)
#define PERICRU_PERISOFTRST_CON06_PRESETN_RTC_TEST_MASK    (0x1U << PERICRU_PERISOFTRST_CON06_PRESETN_RTC_TEST_SHIFT)   /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON06_PRESETN_GPIO2_SHIFT      (3U)
#define PERICRU_PERISOFTRST_CON06_PRESETN_GPIO2_MASK       (0x1U << PERICRU_PERISOFTRST_CON06_PRESETN_GPIO2_SHIFT)      /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON06_DBRESETN_GPIO2_SHIFT     (4U)
#define PERICRU_PERISOFTRST_CON06_DBRESETN_GPIO2_MASK      (0x1U << PERICRU_PERISOFTRST_CON06_DBRESETN_GPIO2_SHIFT)     /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON06_PRESETN_IOC_VCCIO6_SHIFT (5U)
#define PERICRU_PERISOFTRST_CON06_PRESETN_IOC_VCCIO6_MASK  (0x1U << PERICRU_PERISOFTRST_CON06_PRESETN_IOC_VCCIO6_SHIFT) /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON06_PRESETN_PERI_SGRF_SHIFT  (6U)
#define PERICRU_PERISOFTRST_CON06_PRESETN_PERI_SGRF_MASK   (0x1U << PERICRU_PERISOFTRST_CON06_PRESETN_PERI_SGRF_SHIFT)  /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON06_PRESETN_PERI_GRF_SHIFT   (7U)
#define PERICRU_PERISOFTRST_CON06_PRESETN_PERI_GRF_MASK    (0x1U << PERICRU_PERISOFTRST_CON06_PRESETN_PERI_GRF_SHIFT)   /* 0x00000080 */
#define PERICRU_PERISOFTRST_CON06_PRESETN_CRU_PERI_SHIFT   (8U)
#define PERICRU_PERISOFTRST_CON06_PRESETN_CRU_PERI_MASK    (0x1U << PERICRU_PERISOFTRST_CON06_PRESETN_CRU_PERI_SHIFT)   /* 0x00000100 */
#define PERICRU_PERISOFTRST_CON06_ARESETN_USBOTG_SHIFT     (9U)
#define PERICRU_PERISOFTRST_CON06_ARESETN_USBOTG_MASK      (0x1U << PERICRU_PERISOFTRST_CON06_ARESETN_USBOTG_SHIFT)     /* 0x00000200 */
/* PERISOFTRST_CON07 */
#define PERICRU_PERISOFTRST_CON07_OFFSET                   (0xA1CU)
#define PERICRU_PERISOFTRST_CON07_HRESETN_SDMMC1_SHIFT     (0U)
#define PERICRU_PERISOFTRST_CON07_HRESETN_SDMMC1_MASK      (0x1U << PERICRU_PERISOFTRST_CON07_HRESETN_SDMMC1_SHIFT)     /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON07_HRESETN_SAI_SHIFT        (1U)
#define PERICRU_PERISOFTRST_CON07_HRESETN_SAI_MASK         (0x1U << PERICRU_PERISOFTRST_CON07_HRESETN_SAI_SHIFT)        /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON07_MRESETN_SAI_SHIFT        (2U)
#define PERICRU_PERISOFTRST_CON07_MRESETN_SAI_MASK         (0x1U << PERICRU_PERISOFTRST_CON07_MRESETN_SAI_SHIFT)        /* 0x00000004 */
/* PERISOFTRST_CON08 */
#define PERICRU_PERISOFTRST_CON08_OFFSET                   (0xA20U)
#define PERICRU_PERISOFTRST_CON08_RESETN_CORE_CRYPTO_SHIFT (0U)
#define PERICRU_PERISOFTRST_CON08_RESETN_CORE_CRYPTO_MASK  (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_CORE_CRYPTO_SHIFT) /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON08_RESETN_PKA_CRYPTO_SHIFT  (1U)
#define PERICRU_PERISOFTRST_CON08_RESETN_PKA_CRYPTO_MASK   (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_PKA_CRYPTO_SHIFT)  /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON08_ARESETN_CRYPTO_SHIFT     (2U)
#define PERICRU_PERISOFTRST_CON08_ARESETN_CRYPTO_MASK      (0x1U << PERICRU_PERISOFTRST_CON08_ARESETN_CRYPTO_SHIFT)     /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON08_HRESETN_CRYPTO_SHIFT     (3U)
#define PERICRU_PERISOFTRST_CON08_HRESETN_CRYPTO_MASK      (0x1U << PERICRU_PERISOFTRST_CON08_HRESETN_CRYPTO_SHIFT)     /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON08_HRESETN_RK_RNG_NS_SHIFT  (4U)
#define PERICRU_PERISOFTRST_CON08_HRESETN_RK_RNG_NS_MASK   (0x1U << PERICRU_PERISOFTRST_CON08_HRESETN_RK_RNG_NS_SHIFT)  /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON08_HRESETN_RK_RNG_S_SHIFT   (5U)
#define PERICRU_PERISOFTRST_CON08_HRESETN_RK_RNG_S_MASK    (0x1U << PERICRU_PERISOFTRST_CON08_HRESETN_RK_RNG_S_SHIFT)   /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON08_PRESETN_OTPC_NS_SHIFT    (6U)
#define PERICRU_PERISOFTRST_CON08_PRESETN_OTPC_NS_MASK     (0x1U << PERICRU_PERISOFTRST_CON08_PRESETN_OTPC_NS_SHIFT)    /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON08_RESETN_SBPI_OTPC_NS_SHIFT (8U)
#define PERICRU_PERISOFTRST_CON08_RESETN_SBPI_OTPC_NS_MASK (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_SBPI_OTPC_NS_SHIFT) /* 0x00000100 */
#define PERICRU_PERISOFTRST_CON08_RESETN_USER_OTPC_NS_SHIFT (9U)
#define PERICRU_PERISOFTRST_CON08_RESETN_USER_OTPC_NS_MASK (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_USER_OTPC_NS_SHIFT) /* 0x00000200 */
#define PERICRU_PERISOFTRST_CON08_PRESETN_OTPC_S_SHIFT     (10U)
#define PERICRU_PERISOFTRST_CON08_PRESETN_OTPC_S_MASK      (0x1U << PERICRU_PERISOFTRST_CON08_PRESETN_OTPC_S_SHIFT)     /* 0x00000400 */
#define PERICRU_PERISOFTRST_CON08_RESETN_SBPI_OTPC_S_SHIFT (12U)
#define PERICRU_PERISOFTRST_CON08_RESETN_SBPI_OTPC_S_MASK  (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_SBPI_OTPC_S_SHIFT) /* 0x00001000 */
#define PERICRU_PERISOFTRST_CON08_RESETN_USER_OTPC_S_SHIFT (13U)
#define PERICRU_PERISOFTRST_CON08_RESETN_USER_OTPC_S_MASK  (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_USER_OTPC_S_SHIFT) /* 0x00002000 */
#define PERICRU_PERISOFTRST_CON08_RESETN_OTPC_ARB_SHIFT    (14U)
#define PERICRU_PERISOFTRST_CON08_RESETN_OTPC_ARB_MASK     (0x1U << PERICRU_PERISOFTRST_CON08_RESETN_OTPC_ARB_SHIFT)    /* 0x00004000 */
#define PERICRU_PERISOFTRST_CON08_PRESETN_OTP_MASK_SHIFT   (15U)
#define PERICRU_PERISOFTRST_CON08_PRESETN_OTP_MASK_MASK    (0x1U << PERICRU_PERISOFTRST_CON08_PRESETN_OTP_MASK_SHIFT)   /* 0x00008000 */
/* PERISOFTRST_CON09 */
#define PERICRU_PERISOFTRST_CON09_OFFSET                   (0xA24U)
#define PERICRU_PERISOFTRST_CON09_HRESETN_RGA_SHIFT        (0U)
#define PERICRU_PERISOFTRST_CON09_HRESETN_RGA_MASK         (0x1U << PERICRU_PERISOFTRST_CON09_HRESETN_RGA_SHIFT)        /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON09_ARESETN_RGA_SHIFT        (1U)
#define PERICRU_PERISOFTRST_CON09_ARESETN_RGA_MASK         (0x1U << PERICRU_PERISOFTRST_CON09_ARESETN_RGA_SHIFT)        /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON09_RESETN_CORE_RGA_SHIFT    (2U)
#define PERICRU_PERISOFTRST_CON09_RESETN_CORE_RGA_MASK     (0x1U << PERICRU_PERISOFTRST_CON09_RESETN_CORE_RGA_SHIFT)    /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON09_ARESETN_MAC_SHIFT        (3U)
#define PERICRU_PERISOFTRST_CON09_ARESETN_MAC_MASK         (0x1U << PERICRU_PERISOFTRST_CON09_ARESETN_MAC_SHIFT)        /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON09_RESETN_MACPHY_SHIFT      (11U)
#define PERICRU_PERISOFTRST_CON09_RESETN_MACPHY_MASK       (0x1U << PERICRU_PERISOFTRST_CON09_RESETN_MACPHY_SHIFT)      /* 0x00000800 */
/* PERISOFTRST_CON10 */
#define PERICRU_PERISOFTRST_CON10_OFFSET                   (0xA28U)
#define PERICRU_PERISOFTRST_CON10_ARESETN_SPINLOCK_SHIFT   (0U)
#define PERICRU_PERISOFTRST_CON10_ARESETN_SPINLOCK_MASK    (0x1U << PERICRU_PERISOFTRST_CON10_ARESETN_SPINLOCK_SHIFT)   /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON10_HRESETN_CACHE_SHIFT      (1U)
#define PERICRU_PERISOFTRST_CON10_HRESETN_CACHE_MASK       (0x1U << PERICRU_PERISOFTRST_CON10_HRESETN_CACHE_SHIFT)      /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON10_PRESETN_HPMCU_MAILBOX_SHIFT (2U)
#define PERICRU_PERISOFTRST_CON10_PRESETN_HPMCU_MAILBOX_MASK (0x1U << PERICRU_PERISOFTRST_CON10_PRESETN_HPMCU_MAILBOX_SHIFT) /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON10_PRESETN_HPMCU_INTMUX_SHIFT (3U)
#define PERICRU_PERISOFTRST_CON10_PRESETN_HPMCU_INTMUX_MASK (0x1U << PERICRU_PERISOFTRST_CON10_PRESETN_HPMCU_INTMUX_SHIFT) /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_FULL_CLUSTER_SHIFT (4U)
#define PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_FULL_CLUSTER_MASK (0x1U << PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_FULL_CLUSTER_SHIFT) /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_PWUP_SHIFT  (5U)
#define PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_PWUP_MASK   (0x1U << PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_PWUP_SHIFT)  /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_ONLY_CORE_SHIFT (6U)
#define PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_ONLY_CORE_MASK (0x1U << PERICRU_PERISOFTRST_CON10_RESETN_HPMCU_ONLY_CORE_SHIFT) /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON10_TRESETN_HPMCU_JTAG_SHIFT (7U)
#define PERICRU_PERISOFTRST_CON10_TRESETN_HPMCU_JTAG_MASK  (0x1U << PERICRU_PERISOFTRST_CON10_TRESETN_HPMCU_JTAG_SHIFT) /* 0x00000080 */
/* PERISOFTRST_CON11 */
#define PERICRU_PERISOFTRST_CON11_OFFSET                   (0xA2CU)
#define PERICRU_PERISOFTRST_CON11_DRESETN_DECOM_SHIFT      (0U)
#define PERICRU_PERISOFTRST_CON11_DRESETN_DECOM_MASK       (0x1U << PERICRU_PERISOFTRST_CON11_DRESETN_DECOM_SHIFT)      /* 0x00000001 */
#define PERICRU_PERISOFTRST_CON11_ARESETN_DECOM_SHIFT      (1U)
#define PERICRU_PERISOFTRST_CON11_ARESETN_DECOM_MASK       (0x1U << PERICRU_PERISOFTRST_CON11_ARESETN_DECOM_SHIFT)      /* 0x00000002 */
#define PERICRU_PERISOFTRST_CON11_PRESETN_DECOM_SHIFT      (2U)
#define PERICRU_PERISOFTRST_CON11_PRESETN_DECOM_MASK       (0x1U << PERICRU_PERISOFTRST_CON11_PRESETN_DECOM_SHIFT)      /* 0x00000004 */
#define PERICRU_PERISOFTRST_CON11_ARESETN_SYS_SRAM_SHIFT   (3U)
#define PERICRU_PERISOFTRST_CON11_ARESETN_SYS_SRAM_MASK    (0x1U << PERICRU_PERISOFTRST_CON11_ARESETN_SYS_SRAM_SHIFT)   /* 0x00000008 */
#define PERICRU_PERISOFTRST_CON11_PRESETN_DMA2DDR_SHIFT    (4U)
#define PERICRU_PERISOFTRST_CON11_PRESETN_DMA2DDR_MASK     (0x1U << PERICRU_PERISOFTRST_CON11_PRESETN_DMA2DDR_SHIFT)    /* 0x00000010 */
#define PERICRU_PERISOFTRST_CON11_ARESETN_DMA2DDR_SHIFT    (5U)
#define PERICRU_PERISOFTRST_CON11_ARESETN_DMA2DDR_MASK     (0x1U << PERICRU_PERISOFTRST_CON11_ARESETN_DMA2DDR_SHIFT)    /* 0x00000020 */
#define PERICRU_PERISOFTRST_CON11_PRESETN_DCF_SHIFT        (6U)
#define PERICRU_PERISOFTRST_CON11_PRESETN_DCF_MASK         (0x1U << PERICRU_PERISOFTRST_CON11_PRESETN_DCF_SHIFT)        /* 0x00000040 */
#define PERICRU_PERISOFTRST_CON11_ARESETN_DCF_SHIFT        (7U)
#define PERICRU_PERISOFTRST_CON11_ARESETN_DCF_MASK         (0x1U << PERICRU_PERISOFTRST_CON11_ARESETN_DCF_SHIFT)        /* 0x00000080 */
#define PERICRU_PERISOFTRST_CON11_RESETN_USBPHY_POR_SHIFT  (12U)
#define PERICRU_PERISOFTRST_CON11_RESETN_USBPHY_POR_MASK   (0x1U << PERICRU_PERISOFTRST_CON11_RESETN_USBPHY_POR_SHIFT)  /* 0x00001000 */
#define PERICRU_PERISOFTRST_CON11_RESETN_USBPHY_OTG_SHIFT  (13U)
#define PERICRU_PERISOFTRST_CON11_RESETN_USBPHY_OTG_MASK   (0x1U << PERICRU_PERISOFTRST_CON11_RESETN_USBPHY_OTG_SHIFT)  /* 0x00002000 */
/* PERICRU_IP_CON */
#define PERICRU_PERICRU_IP_CON_OFFSET                      (0xC08U)
#define PERICRU_PERICRU_IP_CON_SCLKIN_SPI0_EN_SHIFT        (12U)
#define PERICRU_PERICRU_IP_CON_SCLKIN_SPI0_EN_MASK         (0x1U << PERICRU_PERICRU_IP_CON_SCLKIN_SPI0_EN_SHIFT)        /* 0x00001000 */
#define PERICRU_PERICRU_IP_CON_CLK_UTMI_USBOTG_EN_SHIFT    (14U)
#define PERICRU_PERICRU_IP_CON_CLK_UTMI_USBOTG_EN_MASK     (0x1U << PERICRU_PERICRU_IP_CON_CLK_UTMI_USBOTG_EN_SHIFT)    /* 0x00004000 */
#define PERICRU_PERICRU_IP_CON_CLK_HPMCU_JTAG_EN_SHIFT     (15U)
#define PERICRU_PERICRU_IP_CON_CLK_HPMCU_JTAG_EN_MASK      (0x1U << PERICRU_PERICRU_IP_CON_CLK_HPMCU_JTAG_EN_SHIFT)     /* 0x00008000 */
/****************************************VEPUCRU*****************************************/
/* VEPUCLKSEL_CON00 */
#define VEPUCRU_VEPUCLKSEL_CON00_OFFSET                    (0x300U)
#define VEPUCRU_VEPUCLKSEL_CON00_CLK_VEPU_PVTPLL_SRC_SEL_SHIFT (0U)
#define VEPUCRU_VEPUCLKSEL_CON00_CLK_VEPU_PVTPLL_SRC_SEL_MASK (0x1U << VEPUCRU_VEPUCLKSEL_CON00_CLK_VEPU_PVTPLL_SRC_SEL_SHIFT) /* 0x00000001 */
#define VEPUCRU_VEPUCLKSEL_CON00_CLK_CORE_VEPU_SEL_SHIFT   (1U)
#define VEPUCRU_VEPUCLKSEL_CON00_CLK_CORE_VEPU_SEL_MASK    (0x1U << VEPUCRU_VEPUCLKSEL_CON00_CLK_CORE_VEPU_SEL_SHIFT)   /* 0x00000002 */
#define VEPUCRU_VEPUCLKSEL_CON00_LSCLK_VEPU_ROOT_DIV_SHIFT (2U)
#define VEPUCRU_VEPUCLKSEL_CON00_LSCLK_VEPU_ROOT_DIV_MASK  (0x3U << VEPUCRU_VEPUCLKSEL_CON00_LSCLK_VEPU_ROOT_DIV_SHIFT) /* 0x0000000C */
#define VEPUCRU_VEPUCLKSEL_CON00_MBIST_CLK_CLK_CORE_VEPU_SEL_SHIFT (4U)
#define VEPUCRU_VEPUCLKSEL_CON00_MBIST_CLK_CLK_CORE_VEPU_SEL_MASK (0x1U << VEPUCRU_VEPUCLKSEL_CON00_MBIST_CLK_CLK_CORE_VEPU_SEL_SHIFT) /* 0x00000010 */
/* VEPUCLKSEL_CON02 */
#define VEPUCRU_VEPUCLKSEL_CON02_OFFSET                    (0x308U)
#define VEPUCRU_VEPUCLKSEL_CON02_CLK_TESTOUT_VEPU_DIV_SHIFT (4U)
#define VEPUCRU_VEPUCLKSEL_CON02_CLK_TESTOUT_VEPU_DIV_MASK (0x1FU << VEPUCRU_VEPUCLKSEL_CON02_CLK_TESTOUT_VEPU_DIV_SHIFT) /* 0x000001F0 */
#define VEPUCRU_VEPUCLKSEL_CON02_CLK_TESTOUT_VEPU_SEL_SHIFT (10U)
#define VEPUCRU_VEPUCLKSEL_CON02_CLK_TESTOUT_VEPU_SEL_MASK (0x3U << VEPUCRU_VEPUCLKSEL_CON02_CLK_TESTOUT_VEPU_SEL_SHIFT) /* 0x00000C00 */
/* VEPUGATE_CON00 */
#define VEPUCRU_VEPUGATE_CON00_OFFSET                      (0x800U)
#define VEPUCRU_VEPUGATE_CON00_LSCLK_VEPU_ROOT_EN_SHIFT    (0U)
#define VEPUCRU_VEPUGATE_CON00_LSCLK_VEPU_ROOT_EN_MASK     (0x1U << VEPUCRU_VEPUGATE_CON00_LSCLK_VEPU_ROOT_EN_SHIFT)    /* 0x00000001 */
#define VEPUCRU_VEPUGATE_CON00_ACLK_VEPU_BIU_EN_SHIFT      (1U)
#define VEPUCRU_VEPUGATE_CON00_ACLK_VEPU_BIU_EN_MASK       (0x1U << VEPUCRU_VEPUGATE_CON00_ACLK_VEPU_BIU_EN_SHIFT)      /* 0x00000002 */
#define VEPUCRU_VEPUGATE_CON00_LSCLK_VEPU_BIU_EN_SHIFT     (2U)
#define VEPUCRU_VEPUGATE_CON00_LSCLK_VEPU_BIU_EN_MASK      (0x1U << VEPUCRU_VEPUGATE_CON00_LSCLK_VEPU_BIU_EN_SHIFT)     /* 0x00000004 */
#define VEPUCRU_VEPUGATE_CON00_CLK_REF_PVTPLL_VEPU_EN_SHIFT (3U)
#define VEPUCRU_VEPUGATE_CON00_CLK_REF_PVTPLL_VEPU_EN_MASK (0x1U << VEPUCRU_VEPUGATE_CON00_CLK_REF_PVTPLL_VEPU_EN_SHIFT) /* 0x00000008 */
#define VEPUCRU_VEPUGATE_CON00_HCLK_VEPU_EN_SHIFT          (4U)
#define VEPUCRU_VEPUGATE_CON00_HCLK_VEPU_EN_MASK           (0x1U << VEPUCRU_VEPUGATE_CON00_HCLK_VEPU_EN_SHIFT)          /* 0x00000010 */
#define VEPUCRU_VEPUGATE_CON00_ACLK_VEPU_EN_SHIFT          (5U)
#define VEPUCRU_VEPUGATE_CON00_ACLK_VEPU_EN_MASK           (0x1U << VEPUCRU_VEPUGATE_CON00_ACLK_VEPU_EN_SHIFT)          /* 0x00000020 */
#define VEPUCRU_VEPUGATE_CON00_CLK_CORE_VEPU_EN_SHIFT      (6U)
#define VEPUCRU_VEPUGATE_CON00_CLK_CORE_VEPU_EN_MASK       (0x1U << VEPUCRU_VEPUGATE_CON00_CLK_CORE_VEPU_EN_SHIFT)      /* 0x00000040 */
#define VEPUCRU_VEPUGATE_CON00_PCLK_VEPU_PVTPLL_EN_SHIFT   (7U)
#define VEPUCRU_VEPUGATE_CON00_PCLK_VEPU_PVTPLL_EN_MASK    (0x1U << VEPUCRU_VEPUGATE_CON00_PCLK_VEPU_PVTPLL_EN_SHIFT)   /* 0x00000080 */
#define VEPUCRU_VEPUGATE_CON00_PCLK_CRU_VEPU_EN_SHIFT      (8U)
#define VEPUCRU_VEPUGATE_CON00_PCLK_CRU_VEPU_EN_MASK       (0x1U << VEPUCRU_VEPUGATE_CON00_PCLK_CRU_VEPU_EN_SHIFT)      /* 0x00000100 */
#define VEPUCRU_VEPUGATE_CON00_PCLK_VEPU_GRF_EN_SHIFT      (10U)
#define VEPUCRU_VEPUGATE_CON00_PCLK_VEPU_GRF_EN_MASK       (0x1U << VEPUCRU_VEPUGATE_CON00_PCLK_VEPU_GRF_EN_SHIFT)      /* 0x00000400 */
#define VEPUCRU_VEPUGATE_CON00_PCLK_IOC_VCCIO3_EN_SHIFT    (11U)
#define VEPUCRU_VEPUGATE_CON00_PCLK_IOC_VCCIO3_EN_MASK     (0x1U << VEPUCRU_VEPUGATE_CON00_PCLK_IOC_VCCIO3_EN_SHIFT)    /* 0x00000800 */
#define VEPUCRU_VEPUGATE_CON00_PCLK_ACODEC_EN_SHIFT        (13U)
#define VEPUCRU_VEPUGATE_CON00_PCLK_ACODEC_EN_MASK         (0x1U << VEPUCRU_VEPUGATE_CON00_PCLK_ACODEC_EN_SHIFT)        /* 0x00002000 */
#define VEPUCRU_VEPUGATE_CON00_PCLK_USBPHY_EN_SHIFT        (14U)
#define VEPUCRU_VEPUGATE_CON00_PCLK_USBPHY_EN_MASK         (0x1U << VEPUCRU_VEPUGATE_CON00_PCLK_USBPHY_EN_SHIFT)        /* 0x00004000 */
#define VEPUCRU_VEPUGATE_CON00_CLK_TESTOUT_VEPU_EN_SHIFT   (15U)
#define VEPUCRU_VEPUGATE_CON00_CLK_TESTOUT_VEPU_EN_MASK    (0x1U << VEPUCRU_VEPUGATE_CON00_CLK_TESTOUT_VEPU_EN_SHIFT)   /* 0x00008000 */
/* VEPUSOFTRST_CON00 */
#define VEPUCRU_VEPUSOFTRST_CON00_OFFSET                   (0xA00U)
#define VEPUCRU_VEPUSOFTRST_CON00_ARESETN_VEPU_BIU_SHIFT   (1U)
#define VEPUCRU_VEPUSOFTRST_CON00_ARESETN_VEPU_BIU_MASK    (0x1U << VEPUCRU_VEPUSOFTRST_CON00_ARESETN_VEPU_BIU_SHIFT)   /* 0x00000002 */
#define VEPUCRU_VEPUSOFTRST_CON00_LSRESETN_VEPU_BIU_SHIFT  (2U)
#define VEPUCRU_VEPUSOFTRST_CON00_LSRESETN_VEPU_BIU_MASK   (0x1U << VEPUCRU_VEPUSOFTRST_CON00_LSRESETN_VEPU_BIU_SHIFT)  /* 0x00000004 */
#define VEPUCRU_VEPUSOFTRST_CON00_RESETN_REF_PVTPLL_VEPU_SHIFT (3U)
#define VEPUCRU_VEPUSOFTRST_CON00_RESETN_REF_PVTPLL_VEPU_MASK (0x1U << VEPUCRU_VEPUSOFTRST_CON00_RESETN_REF_PVTPLL_VEPU_SHIFT) /* 0x00000008 */
#define VEPUCRU_VEPUSOFTRST_CON00_HRESETN_VEPU_SHIFT       (4U)
#define VEPUCRU_VEPUSOFTRST_CON00_HRESETN_VEPU_MASK        (0x1U << VEPUCRU_VEPUSOFTRST_CON00_HRESETN_VEPU_SHIFT)       /* 0x00000010 */
#define VEPUCRU_VEPUSOFTRST_CON00_ARESETN_VEPU_SHIFT       (5U)
#define VEPUCRU_VEPUSOFTRST_CON00_ARESETN_VEPU_MASK        (0x1U << VEPUCRU_VEPUSOFTRST_CON00_ARESETN_VEPU_SHIFT)       /* 0x00000020 */
#define VEPUCRU_VEPUSOFTRST_CON00_RESETN_CORE_VEPU_SHIFT   (6U)
#define VEPUCRU_VEPUSOFTRST_CON00_RESETN_CORE_VEPU_MASK    (0x1U << VEPUCRU_VEPUSOFTRST_CON00_RESETN_CORE_VEPU_SHIFT)   /* 0x00000040 */
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_VEPU_PVTPLL_SHIFT (7U)
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_VEPU_PVTPLL_MASK (0x1U << VEPUCRU_VEPUSOFTRST_CON00_PRESETN_VEPU_PVTPLL_SHIFT) /* 0x00000080 */
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_CRU_VEPU_SHIFT   (8U)
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_CRU_VEPU_MASK    (0x1U << VEPUCRU_VEPUSOFTRST_CON00_PRESETN_CRU_VEPU_SHIFT)   /* 0x00000100 */
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_VEPU_GRF_SHIFT   (10U)
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_VEPU_GRF_MASK    (0x1U << VEPUCRU_VEPUSOFTRST_CON00_PRESETN_VEPU_GRF_SHIFT)   /* 0x00000400 */
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_IOC_VCCIO3_SHIFT (11U)
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_IOC_VCCIO3_MASK  (0x1U << VEPUCRU_VEPUSOFTRST_CON00_PRESETN_IOC_VCCIO3_SHIFT) /* 0x00000800 */
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_ACODEC_SHIFT     (13U)
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_ACODEC_MASK      (0x1U << VEPUCRU_VEPUSOFTRST_CON00_PRESETN_ACODEC_SHIFT)     /* 0x00002000 */
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_USBPHY_SHIFT     (14U)
#define VEPUCRU_VEPUSOFTRST_CON00_PRESETN_USBPHY_MASK      (0x1U << VEPUCRU_VEPUSOFTRST_CON00_PRESETN_USBPHY_SHIFT)     /* 0x00004000 */
/*****************************************NPUCRU*****************************************/
/* NPUCLKSEL_CON00 */
#define NPUCRU_NPUCLKSEL_CON00_OFFSET                      (0x300U)
#define NPUCRU_NPUCLKSEL_CON00_CLK_NPU_PVTPLL_SRC_SEL_SHIFT (0U)
#define NPUCRU_NPUCLKSEL_CON00_CLK_NPU_PVTPLL_SRC_SEL_MASK (0x1U << NPUCRU_NPUCLKSEL_CON00_CLK_NPU_PVTPLL_SRC_SEL_SHIFT) /* 0x00000001 */
#define NPUCRU_NPUCLKSEL_CON00_ACLK_NPU_ROOT_SEL_SHIFT     (1U)
#define NPUCRU_NPUCLKSEL_CON00_ACLK_NPU_ROOT_SEL_MASK      (0x1U << NPUCRU_NPUCLKSEL_CON00_ACLK_NPU_ROOT_SEL_SHIFT)     /* 0x00000002 */
#define NPUCRU_NPUCLKSEL_CON00_MBIST_CLK_ACLK_NPU_SEL_SHIFT (2U)
#define NPUCRU_NPUCLKSEL_CON00_MBIST_CLK_ACLK_NPU_SEL_MASK (0x1U << NPUCRU_NPUCLKSEL_CON00_MBIST_CLK_ACLK_NPU_SEL_SHIFT) /* 0x00000004 */
#define NPUCRU_NPUCLKSEL_CON00_CLK_TESTOUT_NPU_DIV_SHIFT   (4U)
#define NPUCRU_NPUCLKSEL_CON00_CLK_TESTOUT_NPU_DIV_MASK    (0x1FU << NPUCRU_NPUCLKSEL_CON00_CLK_TESTOUT_NPU_DIV_SHIFT)  /* 0x000001F0 */
/* NPUCLKSEL_CON02 */
#define NPUCRU_NPUCLKSEL_CON02_OFFSET                      (0x308U)
#define NPUCRU_NPUCLKSEL_CON02_CLK_TESTOUT_NPU_SEL_SHIFT   (10U)
#define NPUCRU_NPUCLKSEL_CON02_CLK_TESTOUT_NPU_SEL_MASK    (0x3U << NPUCRU_NPUCLKSEL_CON02_CLK_TESTOUT_NPU_SEL_SHIFT)   /* 0x00000C00 */
/* NPUGATE_CON00 */
#define NPUCRU_NPUGATE_CON00_OFFSET                        (0x800U)
#define NPUCRU_NPUGATE_CON00_CLK_REF_PVTPLL_NPU_EN_SHIFT   (0U)
#define NPUCRU_NPUGATE_CON00_CLK_REF_PVTPLL_NPU_EN_MASK    (0x1U << NPUCRU_NPUGATE_CON00_CLK_REF_PVTPLL_NPU_EN_SHIFT)   /* 0x00000001 */
#define NPUCRU_NPUGATE_CON00_ACLK_NPU_ROOT_EN_SHIFT        (1U)
#define NPUCRU_NPUGATE_CON00_ACLK_NPU_ROOT_EN_MASK         (0x1U << NPUCRU_NPUGATE_CON00_ACLK_NPU_ROOT_EN_SHIFT)        /* 0x00000002 */
#define NPUCRU_NPUGATE_CON00_ACLK_NPU_BIU_EN_SHIFT         (2U)
#define NPUCRU_NPUGATE_CON00_ACLK_NPU_BIU_EN_MASK          (0x1U << NPUCRU_NPUGATE_CON00_ACLK_NPU_BIU_EN_SHIFT)         /* 0x00000004 */
#define NPUCRU_NPUGATE_CON00_LSCLK_NPU_BIU_EN_SHIFT        (3U)
#define NPUCRU_NPUGATE_CON00_LSCLK_NPU_BIU_EN_MASK         (0x1U << NPUCRU_NPUGATE_CON00_LSCLK_NPU_BIU_EN_SHIFT)        /* 0x00000008 */
#define NPUCRU_NPUGATE_CON00_HCLK_RKNN_EN_SHIFT            (4U)
#define NPUCRU_NPUGATE_CON00_HCLK_RKNN_EN_MASK             (0x1U << NPUCRU_NPUGATE_CON00_HCLK_RKNN_EN_SHIFT)            /* 0x00000010 */
#define NPUCRU_NPUGATE_CON00_ACLK_RKNN_EN_SHIFT            (5U)
#define NPUCRU_NPUGATE_CON00_ACLK_RKNN_EN_MASK             (0x1U << NPUCRU_NPUGATE_CON00_ACLK_RKNN_EN_SHIFT)            /* 0x00000020 */
#define NPUCRU_NPUGATE_CON00_PCLK_NPU_PVTPLL_EN_SHIFT      (6U)
#define NPUCRU_NPUGATE_CON00_PCLK_NPU_PVTPLL_EN_MASK       (0x1U << NPUCRU_NPUGATE_CON00_PCLK_NPU_PVTPLL_EN_SHIFT)      /* 0x00000040 */
#define NPUCRU_NPUGATE_CON00_PCLK_CRU_NPU_EN_SHIFT         (7U)
#define NPUCRU_NPUGATE_CON00_PCLK_CRU_NPU_EN_MASK          (0x1U << NPUCRU_NPUGATE_CON00_PCLK_CRU_NPU_EN_SHIFT)         /* 0x00000080 */
#define NPUCRU_NPUGATE_CON00_PCLK_NPU_GRF_EN_SHIFT         (9U)
#define NPUCRU_NPUGATE_CON00_PCLK_NPU_GRF_EN_MASK          (0x1U << NPUCRU_NPUGATE_CON00_PCLK_NPU_GRF_EN_SHIFT)         /* 0x00000200 */
#define NPUCRU_NPUGATE_CON00_CLK_TESTOUT_NPU_EN_SHIFT      (15U)
#define NPUCRU_NPUGATE_CON00_CLK_TESTOUT_NPU_EN_MASK       (0x1U << NPUCRU_NPUGATE_CON00_CLK_TESTOUT_NPU_EN_SHIFT)      /* 0x00008000 */
/* NPUSOFTRST_CON00 */
#define NPUCRU_NPUSOFTRST_CON00_OFFSET                     (0xA00U)
#define NPUCRU_NPUSOFTRST_CON00_RESETN_REF_PVTPLL_NPU_SHIFT (0U)
#define NPUCRU_NPUSOFTRST_CON00_RESETN_REF_PVTPLL_NPU_MASK (0x1U << NPUCRU_NPUSOFTRST_CON00_RESETN_REF_PVTPLL_NPU_SHIFT) /* 0x00000001 */
#define NPUCRU_NPUSOFTRST_CON00_ARESETN_NPU_BIU_SHIFT      (2U)
#define NPUCRU_NPUSOFTRST_CON00_ARESETN_NPU_BIU_MASK       (0x1U << NPUCRU_NPUSOFTRST_CON00_ARESETN_NPU_BIU_SHIFT)      /* 0x00000004 */
#define NPUCRU_NPUSOFTRST_CON00_LSRESETN_NPU_BIU_SHIFT     (3U)
#define NPUCRU_NPUSOFTRST_CON00_LSRESETN_NPU_BIU_MASK      (0x1U << NPUCRU_NPUSOFTRST_CON00_LSRESETN_NPU_BIU_SHIFT)     /* 0x00000008 */
#define NPUCRU_NPUSOFTRST_CON00_HRESETN_RKNN_SHIFT         (4U)
#define NPUCRU_NPUSOFTRST_CON00_HRESETN_RKNN_MASK          (0x1U << NPUCRU_NPUSOFTRST_CON00_HRESETN_RKNN_SHIFT)         /* 0x00000010 */
#define NPUCRU_NPUSOFTRST_CON00_ARESETN_RKNN_SHIFT         (5U)
#define NPUCRU_NPUSOFTRST_CON00_ARESETN_RKNN_MASK          (0x1U << NPUCRU_NPUSOFTRST_CON00_ARESETN_RKNN_SHIFT)         /* 0x00000020 */
#define NPUCRU_NPUSOFTRST_CON00_PRESETN_NPU_PVTPLL_SHIFT   (6U)
#define NPUCRU_NPUSOFTRST_CON00_PRESETN_NPU_PVTPLL_MASK    (0x1U << NPUCRU_NPUSOFTRST_CON00_PRESETN_NPU_PVTPLL_SHIFT)   /* 0x00000040 */
#define NPUCRU_NPUSOFTRST_CON00_PRESETN_CRU_NPU_SHIFT      (7U)
#define NPUCRU_NPUSOFTRST_CON00_PRESETN_CRU_NPU_MASK       (0x1U << NPUCRU_NPUSOFTRST_CON00_PRESETN_CRU_NPU_SHIFT)      /* 0x00000080 */
#define NPUCRU_NPUSOFTRST_CON00_PRESETN_NPU_GRF_SHIFT      (9U)
#define NPUCRU_NPUSOFTRST_CON00_PRESETN_NPU_GRF_MASK       (0x1U << NPUCRU_NPUSOFTRST_CON00_PRESETN_NPU_GRF_SHIFT)      /* 0x00000200 */
/*****************************************VICRU******************************************/
/* VICLKSEL_CON00 */
#define VICRU_VICLKSEL_CON00_OFFSET                        (0x300U)
#define VICRU_VICLKSEL_CON00_CLK_ISP_PVTPLL_SRC_SEL_SHIFT  (0U)
#define VICRU_VICLKSEL_CON00_CLK_ISP_PVTPLL_SRC_SEL_MASK   (0x1U << VICRU_VICLKSEL_CON00_CLK_ISP_PVTPLL_SRC_SEL_SHIFT)  /* 0x00000001 */
#define VICRU_VICLKSEL_CON00_CLK_CORE_ISP_SEL_SHIFT        (1U)
#define VICRU_VICLKSEL_CON00_CLK_CORE_ISP_SEL_MASK         (0x1U << VICRU_VICLKSEL_CON00_CLK_CORE_ISP_SEL_SHIFT)        /* 0x00000002 */
#define VICRU_VICLKSEL_CON00_MBIST_CLK_CLK_CORE_ISP_SEL_SHIFT (2U)
#define VICRU_VICLKSEL_CON00_MBIST_CLK_CLK_CORE_ISP_SEL_MASK (0x1U << VICRU_VICLKSEL_CON00_MBIST_CLK_CLK_CORE_ISP_SEL_SHIFT) /* 0x00000004 */
#define VICRU_VICLKSEL_CON00_LSCLK_VI_ROOT_SEL_SHIFT       (3U)
#define VICRU_VICLKSEL_CON00_LSCLK_VI_ROOT_SEL_MASK        (0x1U << VICRU_VICLKSEL_CON00_LSCLK_VI_ROOT_SEL_SHIFT)       /* 0x00000008 */
#define VICRU_VICLKSEL_CON00_CLK_TESTOUT_VI_DIV_SHIFT      (4U)
#define VICRU_VICLKSEL_CON00_CLK_TESTOUT_VI_DIV_MASK       (0xFU << VICRU_VICLKSEL_CON00_CLK_TESTOUT_VI_DIV_SHIFT)      /* 0x000000F0 */
#define VICRU_VICLKSEL_CON00_CLK_TESTOUT_VI_SEL_SHIFT      (10U)
#define VICRU_VICLKSEL_CON00_CLK_TESTOUT_VI_SEL_MASK       (0xFU << VICRU_VICLKSEL_CON00_CLK_TESTOUT_VI_SEL_SHIFT)      /* 0x00003C00 */
/* VIGATE_CON00 */
#define VICRU_VIGATE_CON00_OFFSET                          (0x800U)
#define VICRU_VIGATE_CON00_LSCLK_VI_ROOT_EN_SHIFT          (0U)
#define VICRU_VIGATE_CON00_LSCLK_VI_ROOT_EN_MASK           (0x1U << VICRU_VIGATE_CON00_LSCLK_VI_ROOT_EN_SHIFT)          /* 0x00000001 */
#define VICRU_VIGATE_CON00_LSCLK_VI_BIU_EN_SHIFT           (1U)
#define VICRU_VIGATE_CON00_LSCLK_VI_BIU_EN_MASK            (0x1U << VICRU_VIGATE_CON00_LSCLK_VI_BIU_EN_SHIFT)           /* 0x00000002 */
#define VICRU_VIGATE_CON00_ACLK_VI_BIU_EN_SHIFT            (2U)
#define VICRU_VIGATE_CON00_ACLK_VI_BIU_EN_MASK             (0x1U << VICRU_VIGATE_CON00_ACLK_VI_BIU_EN_SHIFT)            /* 0x00000004 */
#define VICRU_VIGATE_CON00_CLK_REF_PVTPLL_NPU_EN_SHIFT     (3U)
#define VICRU_VIGATE_CON00_CLK_REF_PVTPLL_NPU_EN_MASK      (0x1U << VICRU_VIGATE_CON00_CLK_REF_PVTPLL_NPU_EN_SHIFT)     /* 0x00000008 */
#define VICRU_VIGATE_CON00_HCLK_ISP_EN_SHIFT               (4U)
#define VICRU_VIGATE_CON00_HCLK_ISP_EN_MASK                (0x1U << VICRU_VIGATE_CON00_HCLK_ISP_EN_SHIFT)               /* 0x00000010 */
#define VICRU_VIGATE_CON00_ACLK_ISP_EN_SHIFT               (5U)
#define VICRU_VIGATE_CON00_ACLK_ISP_EN_MASK                (0x1U << VICRU_VIGATE_CON00_ACLK_ISP_EN_SHIFT)               /* 0x00000020 */
#define VICRU_VIGATE_CON00_CLK_CORE_ISP_EN_SHIFT           (6U)
#define VICRU_VIGATE_CON00_CLK_CORE_ISP_EN_MASK            (0x1U << VICRU_VIGATE_CON00_CLK_CORE_ISP_EN_SHIFT)           /* 0x00000040 */
/* VIGATE_CON01 */
#define VICRU_VIGATE_CON01_OFFSET                          (0x804U)
#define VICRU_VIGATE_CON01_ACLK_VICAP_EN_SHIFT             (2U)
#define VICRU_VIGATE_CON01_ACLK_VICAP_EN_MASK              (0x1U << VICRU_VIGATE_CON01_ACLK_VICAP_EN_SHIFT)             /* 0x00000004 */
#define VICRU_VIGATE_CON01_HCLK_VICAP_EN_SHIFT             (3U)
#define VICRU_VIGATE_CON01_HCLK_VICAP_EN_MASK              (0x1U << VICRU_VIGATE_CON01_HCLK_VICAP_EN_SHIFT)             /* 0x00000008 */
#define VICRU_VIGATE_CON01_ISP0CLK_VICAP_EN_SHIFT          (8U)
#define VICRU_VIGATE_CON01_ISP0CLK_VICAP_EN_MASK           (0x1U << VICRU_VIGATE_CON01_ISP0CLK_VICAP_EN_SHIFT)          /* 0x00000100 */
#define VICRU_VIGATE_CON01_PCLK_CSI2HOST0_EN_SHIFT         (9U)
#define VICRU_VIGATE_CON01_PCLK_CSI2HOST0_EN_MASK          (0x1U << VICRU_VIGATE_CON01_PCLK_CSI2HOST0_EN_SHIFT)         /* 0x00000200 */
#define VICRU_VIGATE_CON01_PCLK_CSI2HOST1_EN_SHIFT         (11U)
#define VICRU_VIGATE_CON01_PCLK_CSI2HOST1_EN_MASK          (0x1U << VICRU_VIGATE_CON01_PCLK_CSI2HOST1_EN_SHIFT)         /* 0x00000800 */
#define VICRU_VIGATE_CON01_HCLK_EMMC_EN_SHIFT              (13U)
#define VICRU_VIGATE_CON01_HCLK_EMMC_EN_MASK               (0x1U << VICRU_VIGATE_CON01_HCLK_EMMC_EN_SHIFT)              /* 0x00002000 */
#define VICRU_VIGATE_CON01_HCLK_SFC_EN_SHIFT               (14U)
#define VICRU_VIGATE_CON01_HCLK_SFC_EN_MASK                (0x1U << VICRU_VIGATE_CON01_HCLK_SFC_EN_SHIFT)               /* 0x00004000 */
#define VICRU_VIGATE_CON01_HCLK_SFC_XIP_EN_SHIFT           (15U)
#define VICRU_VIGATE_CON01_HCLK_SFC_XIP_EN_MASK            (0x1U << VICRU_VIGATE_CON01_HCLK_SFC_XIP_EN_SHIFT)           /* 0x00008000 */
/* VIGATE_CON02 */
#define VICRU_VIGATE_CON02_OFFSET                          (0x808U)
#define VICRU_VIGATE_CON02_HCLK_SDMMC0_EN_SHIFT            (0U)
#define VICRU_VIGATE_CON02_HCLK_SDMMC0_EN_MASK             (0x1U << VICRU_VIGATE_CON02_HCLK_SDMMC0_EN_SHIFT)            /* 0x00000001 */
#define VICRU_VIGATE_CON02_PCLK_CSIPHY_EN_SHIFT            (2U)
#define VICRU_VIGATE_CON02_PCLK_CSIPHY_EN_MASK             (0x1U << VICRU_VIGATE_CON02_PCLK_CSIPHY_EN_SHIFT)            /* 0x00000004 */
#define VICRU_VIGATE_CON02_PCLK_GPIO1_EN_SHIFT             (3U)
#define VICRU_VIGATE_CON02_PCLK_GPIO1_EN_MASK              (0x1U << VICRU_VIGATE_CON02_PCLK_GPIO1_EN_SHIFT)             /* 0x00000008 */
#define VICRU_VIGATE_CON02_DBCLK_GPIO1_EN_SHIFT            (4U)
#define VICRU_VIGATE_CON02_DBCLK_GPIO1_EN_MASK             (0x1U << VICRU_VIGATE_CON02_DBCLK_GPIO1_EN_SHIFT)            /* 0x00000010 */
#define VICRU_VIGATE_CON02_PCLK_IOC_VCCIO47_EN_SHIFT       (5U)
#define VICRU_VIGATE_CON02_PCLK_IOC_VCCIO47_EN_MASK        (0x1U << VICRU_VIGATE_CON02_PCLK_IOC_VCCIO47_EN_SHIFT)       /* 0x00000020 */
#define VICRU_VIGATE_CON02_PCLK_VI_GRF_EN_SHIFT            (6U)
#define VICRU_VIGATE_CON02_PCLK_VI_GRF_EN_MASK             (0x1U << VICRU_VIGATE_CON02_PCLK_VI_GRF_EN_SHIFT)            /* 0x00000040 */
#define VICRU_VIGATE_CON02_PCLK_CRU_VI_EN_SHIFT            (8U)
#define VICRU_VIGATE_CON02_PCLK_CRU_VI_EN_MASK             (0x1U << VICRU_VIGATE_CON02_PCLK_CRU_VI_EN_SHIFT)            /* 0x00000100 */
#define VICRU_VIGATE_CON02_PCLK_VI_PVTPLL_EN_SHIFT         (9U)
#define VICRU_VIGATE_CON02_PCLK_VI_PVTPLL_EN_MASK          (0x1U << VICRU_VIGATE_CON02_PCLK_VI_PVTPLL_EN_SHIFT)         /* 0x00000200 */
#define VICRU_VIGATE_CON02_CLK_TESTOUT_VI_EN_SHIFT         (15U)
#define VICRU_VIGATE_CON02_CLK_TESTOUT_VI_EN_MASK          (0x1U << VICRU_VIGATE_CON02_CLK_TESTOUT_VI_EN_SHIFT)         /* 0x00008000 */
/* VISOFTRST_CON00 */
#define VICRU_VISOFTRST_CON00_OFFSET                       (0xA00U)
#define VICRU_VISOFTRST_CON00_LSRESETN_VI_BIU_SHIFT        (1U)
#define VICRU_VISOFTRST_CON00_LSRESETN_VI_BIU_MASK         (0x1U << VICRU_VISOFTRST_CON00_LSRESETN_VI_BIU_SHIFT)        /* 0x00000002 */
#define VICRU_VISOFTRST_CON00_ARESETN_VI_BIU_SHIFT         (2U)
#define VICRU_VISOFTRST_CON00_ARESETN_VI_BIU_MASK          (0x1U << VICRU_VISOFTRST_CON00_ARESETN_VI_BIU_SHIFT)         /* 0x00000004 */
#define VICRU_VISOFTRST_CON00_RESETN_REF_PVTPLL_ISP_SHIFT  (3U)
#define VICRU_VISOFTRST_CON00_RESETN_REF_PVTPLL_ISP_MASK   (0x1U << VICRU_VISOFTRST_CON00_RESETN_REF_PVTPLL_ISP_SHIFT)  /* 0x00000008 */
#define VICRU_VISOFTRST_CON00_RESETN_CORE_ISP_SHIFT        (6U)
#define VICRU_VISOFTRST_CON00_RESETN_CORE_ISP_MASK         (0x1U << VICRU_VISOFTRST_CON00_RESETN_CORE_ISP_SHIFT)        /* 0x00000040 */
/* VISOFTRST_CON01 */
#define VICRU_VISOFTRST_CON01_OFFSET                       (0xA04U)
#define VICRU_VISOFTRST_CON01_DRESETN_VICAP_SHIFT          (0U)
#define VICRU_VISOFTRST_CON01_DRESETN_VICAP_MASK           (0x1U << VICRU_VISOFTRST_CON01_DRESETN_VICAP_SHIFT)          /* 0x00000001 */
#define VICRU_VISOFTRST_CON01_ARESETN_VICAP_SHIFT          (2U)
#define VICRU_VISOFTRST_CON01_ARESETN_VICAP_MASK           (0x1U << VICRU_VISOFTRST_CON01_ARESETN_VICAP_SHIFT)          /* 0x00000004 */
#define VICRU_VISOFTRST_CON01_HRESETN_VICAP_SHIFT          (3U)
#define VICRU_VISOFTRST_CON01_HRESETN_VICAP_MASK           (0x1U << VICRU_VISOFTRST_CON01_HRESETN_VICAP_SHIFT)          /* 0x00000008 */
#define VICRU_VISOFTRST_CON01_ISP0RESETN_VICAP_SHIFT       (8U)
#define VICRU_VISOFTRST_CON01_ISP0RESETN_VICAP_MASK        (0x1U << VICRU_VISOFTRST_CON01_ISP0RESETN_VICAP_SHIFT)       /* 0x00000100 */
#define VICRU_VISOFTRST_CON01_PRESETN_CSI2HOST0_SHIFT      (9U)
#define VICRU_VISOFTRST_CON01_PRESETN_CSI2HOST0_MASK       (0x1U << VICRU_VISOFTRST_CON01_PRESETN_CSI2HOST0_SHIFT)      /* 0x00000200 */
#define VICRU_VISOFTRST_CON01_PRESETN_CSI2HOST1_SHIFT      (11U)
#define VICRU_VISOFTRST_CON01_PRESETN_CSI2HOST1_MASK       (0x1U << VICRU_VISOFTRST_CON01_PRESETN_CSI2HOST1_SHIFT)      /* 0x00000800 */
#define VICRU_VISOFTRST_CON01_SRESETN_SFC_2X_SHIFT         (12U)
#define VICRU_VISOFTRST_CON01_SRESETN_SFC_2X_MASK          (0x1U << VICRU_VISOFTRST_CON01_SRESETN_SFC_2X_SHIFT)         /* 0x00001000 */
#define VICRU_VISOFTRST_CON01_HRESETN_EMMC_SHIFT           (13U)
#define VICRU_VISOFTRST_CON01_HRESETN_EMMC_MASK            (0x1U << VICRU_VISOFTRST_CON01_HRESETN_EMMC_SHIFT)           /* 0x00002000 */
#define VICRU_VISOFTRST_CON01_HRESETN_SFC_SHIFT            (14U)
#define VICRU_VISOFTRST_CON01_HRESETN_SFC_MASK             (0x1U << VICRU_VISOFTRST_CON01_HRESETN_SFC_SHIFT)            /* 0x00004000 */
#define VICRU_VISOFTRST_CON01_HRESETN_SFC_XIP_SHIFT        (15U)
#define VICRU_VISOFTRST_CON01_HRESETN_SFC_XIP_MASK         (0x1U << VICRU_VISOFTRST_CON01_HRESETN_SFC_XIP_SHIFT)        /* 0x00008000 */
/* VISOFTRST_CON02 */
#define VICRU_VISOFTRST_CON02_OFFSET                       (0xA08U)
#define VICRU_VISOFTRST_CON02_HRESETN_SDMMC0_SHIFT         (0U)
#define VICRU_VISOFTRST_CON02_HRESETN_SDMMC0_MASK          (0x1U << VICRU_VISOFTRST_CON02_HRESETN_SDMMC0_SHIFT)         /* 0x00000001 */
#define VICRU_VISOFTRST_CON02_PRESETN_CSIPHY_SHIFT         (2U)
#define VICRU_VISOFTRST_CON02_PRESETN_CSIPHY_MASK          (0x1U << VICRU_VISOFTRST_CON02_PRESETN_CSIPHY_SHIFT)         /* 0x00000004 */
#define VICRU_VISOFTRST_CON02_PRESETN_GPIO1_SHIFT          (3U)
#define VICRU_VISOFTRST_CON02_PRESETN_GPIO1_MASK           (0x1U << VICRU_VISOFTRST_CON02_PRESETN_GPIO1_SHIFT)          /* 0x00000008 */
#define VICRU_VISOFTRST_CON02_DBRESETN_GPIO1_SHIFT         (4U)
#define VICRU_VISOFTRST_CON02_DBRESETN_GPIO1_MASK          (0x1U << VICRU_VISOFTRST_CON02_DBRESETN_GPIO1_SHIFT)         /* 0x00000010 */
#define VICRU_VISOFTRST_CON02_PRESETN_IOC_VCCIO47_SHIFT    (5U)
#define VICRU_VISOFTRST_CON02_PRESETN_IOC_VCCIO47_MASK     (0x1U << VICRU_VISOFTRST_CON02_PRESETN_IOC_VCCIO47_SHIFT)    /* 0x00000020 */
#define VICRU_VISOFTRST_CON02_PRESETN_VI_GRF_SHIFT         (6U)
#define VICRU_VISOFTRST_CON02_PRESETN_VI_GRF_MASK          (0x1U << VICRU_VISOFTRST_CON02_PRESETN_VI_GRF_SHIFT)         /* 0x00000040 */
#define VICRU_VISOFTRST_CON02_PRESETN_CRU_VI_SHIFT         (8U)
#define VICRU_VISOFTRST_CON02_PRESETN_CRU_VI_MASK          (0x1U << VICRU_VISOFTRST_CON02_PRESETN_CRU_VI_SHIFT)         /* 0x00000100 */
#define VICRU_VISOFTRST_CON02_PRESETN_VI_PVTPLL_SHIFT      (9U)
#define VICRU_VISOFTRST_CON02_PRESETN_VI_PVTPLL_MASK       (0x1U << VICRU_VISOFTRST_CON02_PRESETN_VI_PVTPLL_SHIFT)      /* 0x00000200 */
/* VICRU_IP_CON */
#define VICRU_VICRU_IP_CON_OFFSET                          (0xC08U)
#define VICRU_VICRU_IP_CON_I0RESETN_VICAP_MIPI_SHIFT       (4U)
#define VICRU_VICRU_IP_CON_I0RESETN_VICAP_MIPI_MASK        (0x1U << VICRU_VICRU_IP_CON_I0RESETN_VICAP_MIPI_SHIFT)       /* 0x00000010 */
#define VICRU_VICRU_IP_CON_I1RESETN_VICAP_MIPI_SHIFT       (5U)
#define VICRU_VICRU_IP_CON_I1RESETN_VICAP_MIPI_MASK        (0x1U << VICRU_VICRU_IP_CON_I1RESETN_VICAP_MIPI_SHIFT)       /* 0x00000020 */
#define VICRU_VICRU_IP_CON_I0CLK_VICAP_MIPI_EN_SHIFT       (10U)
#define VICRU_VICRU_IP_CON_I0CLK_VICAP_MIPI_EN_MASK        (0x1U << VICRU_VICRU_IP_CON_I0CLK_VICAP_MIPI_EN_SHIFT)       /* 0x00000400 */
#define VICRU_VICRU_IP_CON_I1CLK_VICAP_MIPI_EN_SHIFT       (11U)
#define VICRU_VICRU_IP_CON_I1CLK_VICAP_MIPI_EN_MASK        (0x1U << VICRU_VICRU_IP_CON_I1CLK_VICAP_MIPI_EN_SHIFT)       /* 0x00000800 */
#define VICRU_VICRU_IP_CON_CLK_RXBYTECLKHS_DFT_0_EN_SHIFT  (14U)
#define VICRU_VICRU_IP_CON_CLK_RXBYTECLKHS_DFT_0_EN_MASK   (0x1U << VICRU_VICRU_IP_CON_CLK_RXBYTECLKHS_DFT_0_EN_SHIFT)  /* 0x00004000 */
#define VICRU_VICRU_IP_CON_CLK_RXBYTECLKHS_DFT_1_EN_SHIFT  (15U)
#define VICRU_VICRU_IP_CON_CLK_RXBYTECLKHS_DFT_1_EN_MASK   (0x1U << VICRU_VICRU_IP_CON_CLK_RXBYTECLKHS_DFT_1_EN_SHIFT)  /* 0x00008000 */
/****************************************CORECRU*****************************************/
/* CORECLKSEL_CON00 */
#define CORECRU_CORECLKSEL_CON00_OFFSET                    (0x300U)
#define CORECRU_CORECLKSEL_CON00_CLK_CORE_PVTPLL_SRC_SEL_SHIFT (0U)
#define CORECRU_CORECLKSEL_CON00_CLK_CORE_PVTPLL_SRC_SEL_MASK (0x1U << CORECRU_CORECLKSEL_CON00_CLK_CORE_PVTPLL_SRC_SEL_SHIFT) /* 0x00000001 */
#define CORECRU_CORECLKSEL_CON00_CLK_CORE_SRC_SEL_SHIFT    (1U)
#define CORECRU_CORECLKSEL_CON00_CLK_CORE_SRC_SEL_MASK     (0x1U << CORECRU_CORECLKSEL_CON00_CLK_CORE_SRC_SEL_SHIFT)    /* 0x00000002 */
#define CORECRU_CORECLKSEL_CON00_MBIST_CLK_A7_NDFT_SEL_SHIFT (2U)
#define CORECRU_CORECLKSEL_CON00_MBIST_CLK_A7_NDFT_SEL_MASK (0x1U << CORECRU_CORECLKSEL_CON00_MBIST_CLK_A7_NDFT_SEL_SHIFT) /* 0x00000004 */
#define CORECRU_CORECLKSEL_CON00_CLK_TESTOUT_CORE_DIV_SHIFT (4U)
#define CORECRU_CORECLKSEL_CON00_CLK_TESTOUT_CORE_DIV_MASK (0x1FU << CORECRU_CORECLKSEL_CON00_CLK_TESTOUT_CORE_DIV_SHIFT) /* 0x000001F0 */
#define CORECRU_CORECLKSEL_CON00_CLK_TESTOUT_CORE_SEL_SHIFT (10U)
#define CORECRU_CORECLKSEL_CON00_CLK_TESTOUT_CORE_SEL_MASK (0x7U << CORECRU_CORECLKSEL_CON00_CLK_TESTOUT_CORE_SEL_SHIFT) /* 0x00001C00 */
/* CORECLKSEL_CON01 */
#define CORECRU_CORECLKSEL_CON01_OFFSET                    (0x304U)
#define CORECRU_CORECLKSEL_CON01_CLK_SCANHS_CLK_CORE_DIV2_DIV_SHIFT (0U)
#define CORECRU_CORECLKSEL_CON01_CLK_SCANHS_CLK_CORE_DIV2_DIV_MASK (0x1FU << CORECRU_CORECLKSEL_CON01_CLK_SCANHS_CLK_CORE_DIV2_DIV_SHIFT) /* 0x0000001F */
#define CORECRU_CORECLKSEL_CON01_CLK_SCANHS_CLK_CORE_DIV8_DIV_SHIFT (8U)
#define CORECRU_CORECLKSEL_CON01_CLK_SCANHS_CLK_CORE_DIV8_DIV_MASK (0x1FU << CORECRU_CORECLKSEL_CON01_CLK_SCANHS_CLK_CORE_DIV8_DIV_SHIFT) /* 0x00001F00 */
/* CORECLKSEL_CON02 */
#define CORECRU_CORECLKSEL_CON02_OFFSET                    (0x308U)
#define CORECRU_CORECLKSEL_CON02_ACLK_CORE_ROOT_DIV_SHIFT  (0U)
#define CORECRU_CORECLKSEL_CON02_ACLK_CORE_ROOT_DIV_MASK   (0x1FU << CORECRU_CORECLKSEL_CON02_ACLK_CORE_ROOT_DIV_SHIFT) /* 0x0000001F */
#define CORECRU_CORECLKSEL_CON02_PCLK_CORE_ROOT_DIV_SHIFT  (8U)
#define CORECRU_CORECLKSEL_CON02_PCLK_CORE_ROOT_DIV_MASK   (0x1FU << CORECRU_CORECLKSEL_CON02_PCLK_CORE_ROOT_DIV_SHIFT) /* 0x00001F00 */
/* COREGATE_CON00 */
#define CORECRU_COREGATE_CON00_OFFSET                      (0x800U)
#define CORECRU_COREGATE_CON00_CLK_SCANHS_CLK_CORE_EN_SHIFT (0U)
#define CORECRU_COREGATE_CON00_CLK_SCANHS_CLK_CORE_EN_MASK (0x1U << CORECRU_COREGATE_CON00_CLK_SCANHS_CLK_CORE_EN_SHIFT) /* 0x00000001 */
#define CORECRU_COREGATE_CON00_CLK_REF_PVTPLL_CORE_EN_SHIFT (1U)
#define CORECRU_COREGATE_CON00_CLK_REF_PVTPLL_CORE_EN_MASK (0x1U << CORECRU_COREGATE_CON00_CLK_REF_PVTPLL_CORE_EN_SHIFT) /* 0x00000002 */
#define CORECRU_COREGATE_CON00_CLK_CORE_SRC_EN_SHIFT       (2U)
#define CORECRU_COREGATE_CON00_CLK_CORE_SRC_EN_MASK        (0x1U << CORECRU_COREGATE_CON00_CLK_CORE_SRC_EN_SHIFT)       /* 0x00000004 */
#define CORECRU_COREGATE_CON00_CLK_CORE_ROOT_EN_SHIFT      (3U)
#define CORECRU_COREGATE_CON00_CLK_CORE_ROOT_EN_MASK       (0x1U << CORECRU_COREGATE_CON00_CLK_CORE_ROOT_EN_SHIFT)      /* 0x00000008 */
#define CORECRU_COREGATE_CON00_CLK_CORE_EN_SHIFT           (4U)
#define CORECRU_COREGATE_CON00_CLK_CORE_EN_MASK            (0x1U << CORECRU_COREGATE_CON00_CLK_CORE_EN_SHIFT)           /* 0x00000010 */
#define CORECRU_COREGATE_CON00_ACLK_CORE_ROOT_EN_SHIFT     (5U)
#define CORECRU_COREGATE_CON00_ACLK_CORE_ROOT_EN_MASK      (0x1U << CORECRU_COREGATE_CON00_ACLK_CORE_ROOT_EN_SHIFT)     /* 0x00000020 */
#define CORECRU_COREGATE_CON00_PCLK_CORE_ROOT_EN_SHIFT     (6U)
#define CORECRU_COREGATE_CON00_PCLK_CORE_ROOT_EN_MASK      (0x1U << CORECRU_COREGATE_CON00_PCLK_CORE_ROOT_EN_SHIFT)     /* 0x00000040 */
#define CORECRU_COREGATE_CON00_ACLK_CORE_BIU_EN_SHIFT      (7U)
#define CORECRU_COREGATE_CON00_ACLK_CORE_BIU_EN_MASK       (0x1U << CORECRU_COREGATE_CON00_ACLK_CORE_BIU_EN_SHIFT)      /* 0x00000080 */
#define CORECRU_COREGATE_CON00_PCLK_CORE_BIU_EN_SHIFT      (8U)
#define CORECRU_COREGATE_CON00_PCLK_CORE_BIU_EN_MASK       (0x1U << CORECRU_COREGATE_CON00_PCLK_CORE_BIU_EN_SHIFT)      /* 0x00000100 */
#define CORECRU_COREGATE_CON00_HCLK_CORE_BIU_EN_SHIFT      (9U)
#define CORECRU_COREGATE_CON00_HCLK_CORE_BIU_EN_MASK       (0x1U << CORECRU_COREGATE_CON00_HCLK_CORE_BIU_EN_SHIFT)      /* 0x00000200 */
#define CORECRU_COREGATE_CON00_PCLK_DBG_EN_SHIFT           (10U)
#define CORECRU_COREGATE_CON00_PCLK_DBG_EN_MASK            (0x1U << CORECRU_COREGATE_CON00_PCLK_DBG_EN_SHIFT)           /* 0x00000400 */
#define CORECRU_COREGATE_CON00_CLK_TESTOUT_CORE_EN_SHIFT   (15U)
#define CORECRU_COREGATE_CON00_CLK_TESTOUT_CORE_EN_MASK    (0x1U << CORECRU_COREGATE_CON00_CLK_TESTOUT_CORE_EN_SHIFT)   /* 0x00008000 */
/* COREGATE_CON01 */
#define CORECRU_COREGATE_CON01_OFFSET                      (0x804U)
#define CORECRU_COREGATE_CON01_PCLK_CORE_PVTPLL_EN_SHIFT   (0U)
#define CORECRU_COREGATE_CON01_PCLK_CORE_PVTPLL_EN_MASK    (0x1U << CORECRU_COREGATE_CON01_PCLK_CORE_PVTPLL_EN_SHIFT)   /* 0x00000001 */
#define CORECRU_COREGATE_CON01_PCLK_CRU_CORE_EN_SHIFT      (1U)
#define CORECRU_COREGATE_CON01_PCLK_CRU_CORE_EN_MASK       (0x1U << CORECRU_COREGATE_CON01_PCLK_CRU_CORE_EN_SHIFT)      /* 0x00000002 */
#define CORECRU_COREGATE_CON01_PCLK_CORE_GRF_EN_SHIFT      (2U)
#define CORECRU_COREGATE_CON01_PCLK_CORE_GRF_EN_MASK       (0x1U << CORECRU_COREGATE_CON01_PCLK_CORE_GRF_EN_SHIFT)      /* 0x00000004 */
#define CORECRU_COREGATE_CON01_PCLK_DFT2APB_EN_SHIFT       (4U)
#define CORECRU_COREGATE_CON01_PCLK_DFT2APB_EN_MASK        (0x1U << CORECRU_COREGATE_CON01_PCLK_DFT2APB_EN_SHIFT)       /* 0x00000010 */
/* CORESOFTRST_CON00 */
#define CORECRU_CORESOFTRST_CON00_OFFSET                   (0xA00U)
#define CORECRU_CORESOFTRST_CON00_RESETN_REF_PVTPLL_CORE_SHIFT (0U)
#define CORECRU_CORESOFTRST_CON00_RESETN_REF_PVTPLL_CORE_MASK (0x1U << CORECRU_CORESOFTRST_CON00_RESETN_REF_PVTPLL_CORE_SHIFT) /* 0x00000001 */
#define CORECRU_CORESOFTRST_CON00_NCOREPORESET_SHIFT       (1U)
#define CORECRU_CORESOFTRST_CON00_NCOREPORESET_MASK        (0x1U << CORECRU_CORESOFTRST_CON00_NCOREPORESET_SHIFT)       /* 0x00000002 */
#define CORECRU_CORESOFTRST_CON00_NCORESET_SHIFT           (2U)
#define CORECRU_CORESOFTRST_CON00_NCORESET_MASK            (0x1U << CORECRU_CORESOFTRST_CON00_NCORESET_SHIFT)           /* 0x00000004 */
#define CORECRU_CORESOFTRST_CON00_NDBGRESET_SHIFT          (3U)
#define CORECRU_CORESOFTRST_CON00_NDBGRESET_MASK           (0x1U << CORECRU_CORESOFTRST_CON00_NDBGRESET_SHIFT)          /* 0x00000008 */
#define CORECRU_CORESOFTRST_CON00_NL2RESET_SHIFT           (4U)
#define CORECRU_CORESOFTRST_CON00_NL2RESET_MASK            (0x1U << CORECRU_CORESOFTRST_CON00_NL2RESET_SHIFT)           /* 0x00000010 */
#define CORECRU_CORESOFTRST_CON00_ARESETN_CORE_BIU_SHIFT   (5U)
#define CORECRU_CORESOFTRST_CON00_ARESETN_CORE_BIU_MASK    (0x1U << CORECRU_CORESOFTRST_CON00_ARESETN_CORE_BIU_SHIFT)   /* 0x00000020 */
#define CORECRU_CORESOFTRST_CON00_PRESETN_CORE_BIU_SHIFT   (6U)
#define CORECRU_CORESOFTRST_CON00_PRESETN_CORE_BIU_MASK    (0x1U << CORECRU_CORESOFTRST_CON00_PRESETN_CORE_BIU_SHIFT)   /* 0x00000040 */
#define CORECRU_CORESOFTRST_CON00_HRESETN_CORE_BIU_SHIFT   (7U)
#define CORECRU_CORESOFTRST_CON00_HRESETN_CORE_BIU_MASK    (0x1U << CORECRU_CORESOFTRST_CON00_HRESETN_CORE_BIU_SHIFT)   /* 0x00000080 */
#define CORECRU_CORESOFTRST_CON00_PRESETN_DBG_SHIFT        (8U)
#define CORECRU_CORESOFTRST_CON00_PRESETN_DBG_MASK         (0x1U << CORECRU_CORESOFTRST_CON00_PRESETN_DBG_SHIFT)        /* 0x00000100 */
#define CORECRU_CORESOFTRST_CON00_POTRESETN_DBG_SHIFT      (9U)
#define CORECRU_CORESOFTRST_CON00_POTRESETN_DBG_MASK       (0x1U << CORECRU_CORESOFTRST_CON00_POTRESETN_DBG_SHIFT)      /* 0x00000200 */
#define CORECRU_CORESOFTRST_CON00_NTRESETN_DBG_SHIFT       (10U)
#define CORECRU_CORESOFTRST_CON00_NTRESETN_DBG_MASK        (0x1U << CORECRU_CORESOFTRST_CON00_NTRESETN_DBG_SHIFT)       /* 0x00000400 */
/* CORESOFTRST_CON01 */
#define CORECRU_CORESOFTRST_CON01_OFFSET                   (0xA04U)
#define CORECRU_CORESOFTRST_CON01_PRESETN_CORE_PVTPLL_SHIFT (0U)
#define CORECRU_CORESOFTRST_CON01_PRESETN_CORE_PVTPLL_MASK (0x1U << CORECRU_CORESOFTRST_CON01_PRESETN_CORE_PVTPLL_SHIFT) /* 0x00000001 */
#define CORECRU_CORESOFTRST_CON01_PRESETN_CRU_CORE_SHIFT   (1U)
#define CORECRU_CORESOFTRST_CON01_PRESETN_CRU_CORE_MASK    (0x1U << CORECRU_CORESOFTRST_CON01_PRESETN_CRU_CORE_SHIFT)   /* 0x00000002 */
#define CORECRU_CORESOFTRST_CON01_PRESETN_CORE_GRF_SHIFT   (2U)
#define CORECRU_CORESOFTRST_CON01_PRESETN_CORE_GRF_MASK    (0x1U << CORECRU_CORESOFTRST_CON01_PRESETN_CORE_GRF_SHIFT)   /* 0x00000004 */
#define CORECRU_CORESOFTRST_CON01_PRESETN_DFT2APB_SHIFT    (4U)
#define CORECRU_CORESOFTRST_CON01_PRESETN_DFT2APB_MASK     (0x1U << CORECRU_CORESOFTRST_CON01_PRESETN_DFT2APB_SHIFT)    /* 0x00000010 */
/* AUTOCS_CLK_CORE_SRC_CON0 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_OFFSET            (0xD00U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_CLKIN_DIV_CON_SHIFT (0U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_CLKIN_DIV_CON_MASK (0x7U << CORECRU_AUTOCS_CLK_CORE_SRC_CON0_CLKIN_DIV_CON_SHIFT) /* 0x00000007 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_CLKIN_DIV_EN_SHIFT (3U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_CLKIN_DIV_EN_MASK (0x1U << CORECRU_AUTOCS_CLK_CORE_SRC_CON0_CLKIN_DIV_EN_SHIFT) /* 0x00000008 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_IDLE_TH_SHIFT     (4U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_IDLE_TH_MASK      (0xFFFU << CORECRU_AUTOCS_CLK_CORE_SRC_CON0_IDLE_TH_SHIFT)   /* 0x0000FFF0 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_WAIT_TH_SHIFT     (16U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON0_WAIT_TH_MASK      (0xFFFFU << CORECRU_AUTOCS_CLK_CORE_SRC_CON0_WAIT_TH_SHIFT)  /* 0xFFFF0000 */
/* AUTOCS_CLK_CORE_SRC_CON1 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_OFFSET            (0xD04U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_AUTOCS_CTRL_SHIFT (0U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_AUTOCS_CTRL_MASK  (0xFFFU << CORECRU_AUTOCS_CLK_CORE_SRC_CON1_AUTOCS_CTRL_SHIFT) /* 0x00000FFF */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_AUTOCS_EN_SHIFT   (12U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_AUTOCS_EN_MASK    (0x1U << CORECRU_AUTOCS_CLK_CORE_SRC_CON1_AUTOCS_EN_SHIFT)   /* 0x00001000 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_SWITCH_EN_SHIFT   (13U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_SWITCH_EN_MASK    (0x1U << CORECRU_AUTOCS_CLK_CORE_SRC_CON1_SWITCH_EN_SHIFT)   /* 0x00002000 */
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_CLKSEL_CFG_SHIFT  (14U)
#define CORECRU_AUTOCS_CLK_CORE_SRC_CON1_CLKSEL_CFG_MASK   (0x3U << CORECRU_AUTOCS_CLK_CORE_SRC_CON1_CLKSEL_CFG_SHIFT)  /* 0x0000C000 */
/*****************************************DDRCRU*****************************************/
/* DDRCLKSEL_CON00 */
#define DDRCRU_DDRCLKSEL_CON00_OFFSET                      (0x300U)
#define DDRCRU_DDRCLKSEL_CON00_CLK_TESTOUT_DDR_DIV_SHIFT   (0U)
#define DDRCRU_DDRCLKSEL_CON00_CLK_TESTOUT_DDR_DIV_MASK    (0x1FU << DDRCRU_DDRCLKSEL_CON00_CLK_TESTOUT_DDR_DIV_SHIFT)  /* 0x0000001F */
#define DDRCRU_DDRCLKSEL_CON00_CLK_TESTOUT_DDR_SEL_SHIFT   (8U)
#define DDRCRU_DDRCLKSEL_CON00_CLK_TESTOUT_DDR_SEL_MASK    (0x1U << DDRCRU_DDRCLKSEL_CON00_CLK_TESTOUT_DDR_SEL_SHIFT)   /* 0x00000100 */
/* DDRGATE_CON00 */
#define DDRCRU_DDRGATE_CON00_OFFSET                        (0x800U)
#define DDRCRU_DDRGATE_CON00_LSCLK_DDR_ROOT_EN_SHIFT       (0U)
#define DDRCRU_DDRGATE_CON00_LSCLK_DDR_ROOT_EN_MASK        (0x1U << DDRCRU_DDRGATE_CON00_LSCLK_DDR_ROOT_EN_SHIFT)       /* 0x00000001 */
#define DDRCRU_DDRGATE_CON00_LSCLK_DDR_BIU_EN_SHIFT        (1U)
#define DDRCRU_DDRGATE_CON00_LSCLK_DDR_BIU_EN_MASK         (0x1U << DDRCRU_DDRGATE_CON00_LSCLK_DDR_BIU_EN_SHIFT)        /* 0x00000002 */
#define DDRCRU_DDRGATE_CON00_PCLK_DDRC_EN_SHIFT            (2U)
#define DDRCRU_DDRGATE_CON00_PCLK_DDRC_EN_MASK             (0x1U << DDRCRU_DDRGATE_CON00_PCLK_DDRC_EN_SHIFT)            /* 0x00000004 */
#define DDRCRU_DDRGATE_CON00_PCLK_DDRMON_EN_SHIFT          (3U)
#define DDRCRU_DDRGATE_CON00_PCLK_DDRMON_EN_MASK           (0x1U << DDRCRU_DDRGATE_CON00_PCLK_DDRMON_EN_SHIFT)          /* 0x00000008 */
#define DDRCRU_DDRGATE_CON00_CLK_TIMER_DDRMON_EN_SHIFT     (4U)
#define DDRCRU_DDRGATE_CON00_CLK_TIMER_DDRMON_EN_MASK      (0x1U << DDRCRU_DDRGATE_CON00_CLK_TIMER_DDRMON_EN_SHIFT)     /* 0x00000010 */
#define DDRCRU_DDRGATE_CON00_PCLK_DFICTRL_EN_SHIFT         (5U)
#define DDRCRU_DDRGATE_CON00_PCLK_DFICTRL_EN_MASK          (0x1U << DDRCRU_DDRGATE_CON00_PCLK_DFICTRL_EN_SHIFT)         /* 0x00000020 */
#define DDRCRU_DDRGATE_CON00_PCLK_DDR_GRF_EN_SHIFT         (6U)
#define DDRCRU_DDRGATE_CON00_PCLK_DDR_GRF_EN_MASK          (0x1U << DDRCRU_DDRGATE_CON00_PCLK_DDR_GRF_EN_SHIFT)         /* 0x00000040 */
#define DDRCRU_DDRGATE_CON00_PCLK_CRU_DDR_EN_SHIFT         (7U)
#define DDRCRU_DDRGATE_CON00_PCLK_CRU_DDR_EN_MASK          (0x1U << DDRCRU_DDRGATE_CON00_PCLK_CRU_DDR_EN_SHIFT)         /* 0x00000080 */
#define DDRCRU_DDRGATE_CON00_HCLK_DDRPHY_EN_SHIFT          (8U)
#define DDRCRU_DDRGATE_CON00_HCLK_DDRPHY_EN_MASK           (0x1U << DDRCRU_DDRGATE_CON00_HCLK_DDRPHY_EN_SHIFT)          /* 0x00000100 */
#define DDRCRU_DDRGATE_CON00_CLK_TESTOUT_DDR_EN_SHIFT      (15U)
#define DDRCRU_DDRGATE_CON00_CLK_TESTOUT_DDR_EN_MASK       (0x1U << DDRCRU_DDRGATE_CON00_CLK_TESTOUT_DDR_EN_SHIFT)      /* 0x00008000 */
/* DDRSOFTRST_CON00 */
#define DDRCRU_DDRSOFTRST_CON00_OFFSET                     (0xA00U)
#define DDRCRU_DDRSOFTRST_CON00_LSRESETN_DDR_BIU_SHIFT     (1U)
#define DDRCRU_DDRSOFTRST_CON00_LSRESETN_DDR_BIU_MASK      (0x1U << DDRCRU_DDRSOFTRST_CON00_LSRESETN_DDR_BIU_SHIFT)     /* 0x00000002 */
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DDRC_SHIFT         (2U)
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DDRC_MASK          (0x1U << DDRCRU_DDRSOFTRST_CON00_PRESETN_DDRC_SHIFT)         /* 0x00000004 */
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DDRMON_SHIFT       (3U)
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DDRMON_MASK        (0x1U << DDRCRU_DDRSOFTRST_CON00_PRESETN_DDRMON_SHIFT)       /* 0x00000008 */
#define DDRCRU_DDRSOFTRST_CON00_RESETN_TIMER_DDRMON_SHIFT  (4U)
#define DDRCRU_DDRSOFTRST_CON00_RESETN_TIMER_DDRMON_MASK   (0x1U << DDRCRU_DDRSOFTRST_CON00_RESETN_TIMER_DDRMON_SHIFT)  /* 0x00000010 */
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DFICTRL_SHIFT      (5U)
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DFICTRL_MASK       (0x1U << DDRCRU_DDRSOFTRST_CON00_PRESETN_DFICTRL_SHIFT)      /* 0x00000020 */
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DDR_GRF_SHIFT      (6U)
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_DDR_GRF_MASK       (0x1U << DDRCRU_DDRSOFTRST_CON00_PRESETN_DDR_GRF_SHIFT)      /* 0x00000040 */
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_CRU_DDR_SHIFT      (7U)
#define DDRCRU_DDRSOFTRST_CON00_PRESETN_CRU_DDR_MASK       (0x1U << DDRCRU_DDRSOFTRST_CON00_PRESETN_CRU_DDR_SHIFT)      /* 0x00000080 */
#define DDRCRU_DDRSOFTRST_CON00_HRESETN_DDRPHY_SHIFT       (8U)
#define DDRCRU_DDRSOFTRST_CON00_HRESETN_DDRPHY_MASK        (0x1U << DDRCRU_DDRSOFTRST_CON00_HRESETN_DDRPHY_SHIFT)       /* 0x00000100 */
/* AUTOCS_LSCLK_DDR_ROOT_CON0 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_OFFSET           (0xD00U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_CLKIN_DIV_CON_SHIFT (0U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_CLKIN_DIV_CON_MASK (0x7U << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_CLKIN_DIV_CON_SHIFT) /* 0x00000007 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_CLKIN_DIV_EN_SHIFT (3U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_CLKIN_DIV_EN_MASK (0x1U << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_CLKIN_DIV_EN_SHIFT) /* 0x00000008 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_IDLE_TH_SHIFT    (4U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_IDLE_TH_MASK     (0xFFFU << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_IDLE_TH_SHIFT)  /* 0x0000FFF0 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_WAIT_TH_SHIFT    (16U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_WAIT_TH_MASK     (0xFFFFU << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON0_WAIT_TH_SHIFT) /* 0xFFFF0000 */
/* AUTOCS_LSCLK_DDR_ROOT_CON1 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_OFFSET           (0xD04U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_AUTOCS_CTRL_SHIFT (0U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_AUTOCS_CTRL_MASK (0xFFFU << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_AUTOCS_CTRL_SHIFT) /* 0x00000FFF */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_AUTOCS_EN_SHIFT  (12U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_AUTOCS_EN_MASK   (0x1U << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_AUTOCS_EN_SHIFT)  /* 0x00001000 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_SWITCH_EN_SHIFT  (13U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_SWITCH_EN_MASK   (0x1U << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_SWITCH_EN_SHIFT)  /* 0x00002000 */
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_CLKSEL_CFG_SHIFT (14U)
#define DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_CLKSEL_CFG_MASK  (0x3U << DDRCRU_AUTOCS_LSCLK_DDR_ROOT_CON1_CLKSEL_CFG_SHIFT) /* 0x0000C000 */
/***************************************SUBDDRCRU****************************************/
/* SUBDDRGATE_CON00 */
#define SUBDDRCRU_SUBDDRGATE_CON00_OFFSET                  (0x800U)
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DDR_BIU_EN_SHIFT    (1U)
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DDR_BIU_EN_MASK     (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_CLK_DDR_BIU_EN_SHIFT)    /* 0x00000002 */
#define SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_CPU_EN_SHIFT (2U)
#define SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_CPU_EN_MASK (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_CPU_EN_SHIFT) /* 0x00000004 */
#define SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_VI_EN_SHIFT (4U)
#define SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_VI_EN_MASK  (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_VI_EN_SHIFT) /* 0x00000010 */
#define SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_NPVD_EN_SHIFT (5U)
#define SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_NPVD_EN_MASK (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_ACLK_DDRSCH_NPVD_EN_SHIFT) /* 0x00000020 */
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_CORE_DDRC_EN_SHIFT  (6U)
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_CORE_DDRC_EN_MASK   (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_CLK_CORE_DDRC_EN_SHIFT)  /* 0x00000040 */
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DDRMON_EN_SHIFT     (7U)
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DDRMON_EN_MASK      (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_CLK_DDRMON_EN_SHIFT)     /* 0x00000080 */
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DFICTRL_EN_SHIFT    (8U)
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DFICTRL_EN_MASK     (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_CLK_DFICTRL_EN_SHIFT)    /* 0x00000100 */
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DFI_SCRAMBLE_EN_SHIFT (9U)
#define SUBDDRCRU_SUBDDRGATE_CON00_CLK_DFI_SCRAMBLE_EN_MASK (0x1U << SUBDDRCRU_SUBDDRGATE_CON00_CLK_DFI_SCRAMBLE_EN_SHIFT) /* 0x00000200 */
/* SUBDDRSOFTRST_CON00 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_OFFSET               (0xA00U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DDR_BIU_SHIFT (1U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DDR_BIU_MASK  (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DDR_BIU_SHIFT) /* 0x00000002 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_CPU_SHIFT (2U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_CPU_MASK (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_CPU_SHIFT) /* 0x00000004 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_VI_SHIFT (4U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_VI_MASK (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_VI_SHIFT) /* 0x00000010 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_NPVD_SHIFT (5U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_NPVD_MASK (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_ARESETN_DDRSCH_NPVD_SHIFT) /* 0x00000020 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_CORE_DDRC_SHIFT (6U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_CORE_DDRC_MASK (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_CORE_DDRC_SHIFT) /* 0x00000040 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DDRMON_SHIFT  (7U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DDRMON_MASK   (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DDRMON_SHIFT)  /* 0x00000080 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DFICTRL_SHIFT (8U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DFICTRL_MASK  (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DFICTRL_SHIFT) /* 0x00000100 */
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DFI_SCRAMBLE_SHIFT (9U)
#define SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DFI_SCRAMBLE_MASK (0x1U << SUBDDRCRU_SUBDDRSOFTRST_CON00_RESETN_DFI_SCRAMBLE_SHIFT) /* 0x00000200 */
/*****************************************TOPCRU*****************************************/
/* CLKSEL_CON00 */
#define TOPCRU_CLKSEL_CON00_OFFSET                         (0x300U)
#define TOPCRU_CLKSEL_CON00_CLK_MATRIX_050M_SRC_DIV_SHIFT  (0U)
#define TOPCRU_CLKSEL_CON00_CLK_MATRIX_050M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON00_CLK_MATRIX_050M_SRC_DIV_SHIFT) /* 0x0000001F */
#define TOPCRU_CLKSEL_CON00_CLK_MATRIX_100M_SRC_DIV_SHIFT  (5U)
#define TOPCRU_CLKSEL_CON00_CLK_MATRIX_100M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON00_CLK_MATRIX_100M_SRC_DIV_SHIFT) /* 0x000003E0 */
/* CLKSEL_CON01 */
#define TOPCRU_CLKSEL_CON01_OFFSET                         (0x304U)
#define TOPCRU_CLKSEL_CON01_CLK_MATRIX_200M_SRC_DIV_SHIFT  (0U)
#define TOPCRU_CLKSEL_CON01_CLK_MATRIX_200M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON01_CLK_MATRIX_200M_SRC_DIV_SHIFT) /* 0x0000001F */
#define TOPCRU_CLKSEL_CON01_CLK_MATRIX_300M_SRC_DIV_SHIFT  (10U)
#define TOPCRU_CLKSEL_CON01_CLK_MATRIX_300M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON01_CLK_MATRIX_300M_SRC_DIV_SHIFT) /* 0x00007C00 */
/* CLKSEL_CON02 */
#define TOPCRU_CLKSEL_CON02_OFFSET                         (0x308U)
#define TOPCRU_CLKSEL_CON02_CLK_MATRIX_400M_SRC_DIV_SHIFT  (0U)
#define TOPCRU_CLKSEL_CON02_CLK_MATRIX_400M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON02_CLK_MATRIX_400M_SRC_DIV_SHIFT) /* 0x0000001F */
#define TOPCRU_CLKSEL_CON02_CLK_MATRIX_480M_SRC_DIV_SHIFT  (5U)
#define TOPCRU_CLKSEL_CON02_CLK_MATRIX_480M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON02_CLK_MATRIX_480M_SRC_DIV_SHIFT) /* 0x000003E0 */
#define TOPCRU_CLKSEL_CON02_CLK_MATRIX_600M_SRC_DIV_SHIFT  (10U)
#define TOPCRU_CLKSEL_CON02_CLK_MATRIX_600M_SRC_DIV_MASK   (0x1FU << TOPCRU_CLKSEL_CON02_CLK_MATRIX_600M_SRC_DIV_SHIFT) /* 0x00007C00 */
/* CLKSEL_CON05 */
#define TOPCRU_CLKSEL_CON05_OFFSET                         (0x314U)
#define TOPCRU_CLKSEL_CON05_CLK_UART0_SRC_DIV_SHIFT        (0U)
#define TOPCRU_CLKSEL_CON05_CLK_UART0_SRC_DIV_MASK         (0x1FU << TOPCRU_CLKSEL_CON05_CLK_UART0_SRC_DIV_SHIFT)       /* 0x0000001F */
#define TOPCRU_CLKSEL_CON05_CLK_UART1_SRC_DIV_SHIFT        (5U)
#define TOPCRU_CLKSEL_CON05_CLK_UART1_SRC_DIV_MASK         (0x1FU << TOPCRU_CLKSEL_CON05_CLK_UART1_SRC_DIV_SHIFT)       /* 0x000003E0 */
#define TOPCRU_CLKSEL_CON05_CLK_UART2_SRC_DIV_SHIFT        (10U)
#define TOPCRU_CLKSEL_CON05_CLK_UART2_SRC_DIV_MASK         (0x1FU << TOPCRU_CLKSEL_CON05_CLK_UART2_SRC_DIV_SHIFT)       /* 0x00007C00 */
/* CLKSEL_CON10 */
#define TOPCRU_CLKSEL_CON10_OFFSET                         (0x328U)
#define TOPCRU_CLKSEL_CON10_CLK_UART0_FRAC_DIV_SHIFT       (0U)
#define TOPCRU_CLKSEL_CON10_CLK_UART0_FRAC_DIV_MASK        (0xFFFFFFFFU << TOPCRU_CLKSEL_CON10_CLK_UART0_FRAC_DIV_SHIFT) /* 0xFFFFFFFF */
/* CLKSEL_CON11 */
#define TOPCRU_CLKSEL_CON11_OFFSET                         (0x32CU)
#define TOPCRU_CLKSEL_CON11_CLK_UART1_FRAC_DIV_SHIFT       (0U)
#define TOPCRU_CLKSEL_CON11_CLK_UART1_FRAC_DIV_MASK        (0xFFFFFFFFU << TOPCRU_CLKSEL_CON11_CLK_UART1_FRAC_DIV_SHIFT) /* 0xFFFFFFFF */
/* CLKSEL_CON12 */
#define TOPCRU_CLKSEL_CON12_OFFSET                         (0x330U)
#define TOPCRU_CLKSEL_CON12_CLK_UART2_FRAC_DIV_SHIFT       (0U)
#define TOPCRU_CLKSEL_CON12_CLK_UART2_FRAC_DIV_MASK        (0xFFFFFFFFU << TOPCRU_CLKSEL_CON12_CLK_UART2_FRAC_DIV_SHIFT) /* 0xFFFFFFFF */
/* CLKSEL_CON20 */
#define TOPCRU_CLKSEL_CON20_OFFSET                         (0x350U)
#define TOPCRU_CLKSEL_CON20_CLK_SAI_SRC_DIV_SHIFT          (0U)
#define TOPCRU_CLKSEL_CON20_CLK_SAI_SRC_DIV_MASK           (0x1FU << TOPCRU_CLKSEL_CON20_CLK_SAI_SRC_DIV_SHIFT)         /* 0x0000001F */
/* CLKSEL_CON21 */
#define TOPCRU_CLKSEL_CON21_OFFSET                         (0x354U)
#define TOPCRU_CLKSEL_CON21_CLK_SAI_FRAC_DIV_SHIFT         (0U)
#define TOPCRU_CLKSEL_CON21_CLK_SAI_FRAC_DIV_MASK          (0xFFFFFFFFU << TOPCRU_CLKSEL_CON21_CLK_SAI_FRAC_DIV_SHIFT)  /* 0xFFFFFFFF */
/* CLKSEL_CON30 */
#define TOPCRU_CLKSEL_CON30_OFFSET                         (0x378U)
#define TOPCRU_CLKSEL_CON30_LSCLK_NPU_SRC_SEL_SHIFT        (0U)
#define TOPCRU_CLKSEL_CON30_LSCLK_NPU_SRC_SEL_MASK         (0x1U << TOPCRU_CLKSEL_CON30_LSCLK_NPU_SRC_SEL_SHIFT)        /* 0x00000001 */
#define TOPCRU_CLKSEL_CON30_ACLK_VEPU_SRC_SEL_SHIFT        (8U)
#define TOPCRU_CLKSEL_CON30_ACLK_VEPU_SRC_SEL_MASK         (0x3U << TOPCRU_CLKSEL_CON30_ACLK_VEPU_SRC_SEL_SHIFT)        /* 0x00000300 */
#define TOPCRU_CLKSEL_CON30_ACLK_VI_SRC_SEL_SHIFT          (12U)
#define TOPCRU_CLKSEL_CON30_ACLK_VI_SRC_SEL_MASK           (0x3U << TOPCRU_CLKSEL_CON30_ACLK_VI_SRC_SEL_SHIFT)          /* 0x00003000 */
#define TOPCRU_CLKSEL_CON30_DCLK_VICAP_SRC_SEL_SHIFT       (14U)
#define TOPCRU_CLKSEL_CON30_DCLK_VICAP_SRC_SEL_MASK        (0x1U << TOPCRU_CLKSEL_CON30_DCLK_VICAP_SRC_SEL_SHIFT)       /* 0x00004000 */
/* CLKSEL_CON31 */
#define TOPCRU_CLKSEL_CON31_OFFSET                         (0x37CU)
#define TOPCRU_CLKSEL_CON31_CCLK_EMMC_SRC_DIV_SHIFT        (0U)
#define TOPCRU_CLKSEL_CON31_CCLK_EMMC_SRC_DIV_MASK         (0xFFU << TOPCRU_CLKSEL_CON31_CCLK_EMMC_SRC_DIV_SHIFT)       /* 0x000000FF */
#define TOPCRU_CLKSEL_CON31_LSCLK_PERI_SRC_SEL_SHIFT       (9U)
#define TOPCRU_CLKSEL_CON31_LSCLK_PERI_SRC_SEL_MASK        (0x1U << TOPCRU_CLKSEL_CON31_LSCLK_PERI_SRC_SEL_SHIFT)       /* 0x00000200 */
#define TOPCRU_CLKSEL_CON31_ACLK_PERI_SRC_SEL_SHIFT        (10U)
#define TOPCRU_CLKSEL_CON31_ACLK_PERI_SRC_SEL_MASK         (0x3U << TOPCRU_CLKSEL_CON31_ACLK_PERI_SRC_SEL_SHIFT)        /* 0x00000C00 */
#define TOPCRU_CLKSEL_CON31_HCLK_HPMCU_SRC_SEL_SHIFT       (12U)
#define TOPCRU_CLKSEL_CON31_HCLK_HPMCU_SRC_SEL_MASK        (0x1U << TOPCRU_CLKSEL_CON31_HCLK_HPMCU_SRC_SEL_SHIFT)       /* 0x00001000 */
#define TOPCRU_CLKSEL_CON31_CCLK_EMMC_SRC_SEL_SHIFT        (15U)
#define TOPCRU_CLKSEL_CON31_CCLK_EMMC_SRC_SEL_MASK         (0x1U << TOPCRU_CLKSEL_CON31_CCLK_EMMC_SRC_SEL_SHIFT)        /* 0x00008000 */
/* CLKSEL_CON32 */
#define TOPCRU_CLKSEL_CON32_OFFSET                         (0x380U)
#define TOPCRU_CLKSEL_CON32_CCLK_SDMMC0_SRC_DIV_SHIFT      (0U)
#define TOPCRU_CLKSEL_CON32_CCLK_SDMMC0_SRC_DIV_MASK       (0xFFU << TOPCRU_CLKSEL_CON32_CCLK_SDMMC0_SRC_DIV_SHIFT)     /* 0x000000FF */
#define TOPCRU_CLKSEL_CON32_SCLK_UART0_SRC_SEL_SHIFT       (8U)
#define TOPCRU_CLKSEL_CON32_SCLK_UART0_SRC_SEL_MASK        (0x3U << TOPCRU_CLKSEL_CON32_SCLK_UART0_SRC_SEL_SHIFT)       /* 0x00000300 */
#define TOPCRU_CLKSEL_CON32_SCLK_UART1_SRC_SEL_SHIFT       (10U)
#define TOPCRU_CLKSEL_CON32_SCLK_UART1_SRC_SEL_MASK        (0x3U << TOPCRU_CLKSEL_CON32_SCLK_UART1_SRC_SEL_SHIFT)       /* 0x00000C00 */
#define TOPCRU_CLKSEL_CON32_SCLK_UART2_SRC_SEL_SHIFT       (12U)
#define TOPCRU_CLKSEL_CON32_SCLK_UART2_SRC_SEL_MASK        (0x3U << TOPCRU_CLKSEL_CON32_SCLK_UART2_SRC_SEL_SHIFT)       /* 0x00003000 */
#define TOPCRU_CLKSEL_CON32_CCLK_SDMMC0_SRC_SEL_SHIFT      (15U)
#define TOPCRU_CLKSEL_CON32_CCLK_SDMMC0_SRC_SEL_MASK       (0x1U << TOPCRU_CLKSEL_CON32_CCLK_SDMMC0_SRC_SEL_SHIFT)      /* 0x00008000 */
/* CLKSEL_CON33 */
#define TOPCRU_CLKSEL_CON33_OFFSET                         (0x384U)
#define TOPCRU_CLKSEL_CON33_SCLK_SFC_2X_SRC_DIV_SHIFT      (0U)
#define TOPCRU_CLKSEL_CON33_SCLK_SFC_2X_SRC_DIV_MASK       (0xFFU << TOPCRU_CLKSEL_CON33_SCLK_SFC_2X_SRC_DIV_SHIFT)     /* 0x000000FF */
#define TOPCRU_CLKSEL_CON33_SCLK_SFC_2X_SRC_SEL_SHIFT      (15U)
#define TOPCRU_CLKSEL_CON33_SCLK_SFC_2X_SRC_SEL_MASK       (0x1U << TOPCRU_CLKSEL_CON33_SCLK_SFC_2X_SRC_SEL_SHIFT)      /* 0x00008000 */
/* CLKSEL_CON34 */
#define TOPCRU_CLKSEL_CON34_OFFSET                         (0x388U)
#define TOPCRU_CLKSEL_CON34_CLK_I2C_PMU_SRC_SEL_SHIFT      (0U)
#define TOPCRU_CLKSEL_CON34_CLK_I2C_PMU_SRC_SEL_MASK       (0x1U << TOPCRU_CLKSEL_CON34_CLK_I2C_PMU_SRC_SEL_SHIFT)      /* 0x00000001 */
#define TOPCRU_CLKSEL_CON34_CLK_I2C_PERI_SRC_SEL_SHIFT     (1U)
#define TOPCRU_CLKSEL_CON34_CLK_I2C_PERI_SRC_SEL_MASK      (0x1U << TOPCRU_CLKSEL_CON34_CLK_I2C_PERI_SRC_SEL_SHIFT)     /* 0x00000002 */
#define TOPCRU_CLKSEL_CON34_CLK_SPI0_SRC_SEL_SHIFT         (2U)
#define TOPCRU_CLKSEL_CON34_CLK_SPI0_SRC_SEL_MASK          (0x3U << TOPCRU_CLKSEL_CON34_CLK_SPI0_SRC_SEL_SHIFT)         /* 0x0000000C */
#define TOPCRU_CLKSEL_CON34_CLK_PWM0_SRC_SEL_SHIFT         (12U)
#define TOPCRU_CLKSEL_CON34_CLK_PWM0_SRC_SEL_MASK          (0x1U << TOPCRU_CLKSEL_CON34_CLK_PWM0_SRC_SEL_SHIFT)         /* 0x00001000 */
#define TOPCRU_CLKSEL_CON34_CLK_PWM1_SRC_SEL_SHIFT         (13U)
#define TOPCRU_CLKSEL_CON34_CLK_PWM1_SRC_SEL_MASK          (0x1U << TOPCRU_CLKSEL_CON34_CLK_PWM1_SRC_SEL_SHIFT)         /* 0x00002000 */
#define TOPCRU_CLKSEL_CON34_CLK_PWM2_SRC_SEL_SHIFT         (14U)
#define TOPCRU_CLKSEL_CON34_CLK_PWM2_SRC_SEL_MASK          (0x1U << TOPCRU_CLKSEL_CON34_CLK_PWM2_SRC_SEL_SHIFT)         /* 0x00004000 */
/* CLKSEL_CON35 */
#define TOPCRU_CLKSEL_CON35_OFFSET                         (0x38CU)
#define TOPCRU_CLKSEL_CON35_DCLK_DECOM_SRC_SEL_SHIFT       (0U)
#define TOPCRU_CLKSEL_CON35_DCLK_DECOM_SRC_SEL_MASK        (0x3U << TOPCRU_CLKSEL_CON35_DCLK_DECOM_SRC_SEL_SHIFT)       /* 0x00000003 */
#define TOPCRU_CLKSEL_CON35_CLK_CORE_CRYPTO_SRC_SEL_SHIFT  (2U)
#define TOPCRU_CLKSEL_CON35_CLK_CORE_CRYPTO_SRC_SEL_MASK   (0x3U << TOPCRU_CLKSEL_CON35_CLK_CORE_CRYPTO_SRC_SEL_SHIFT)  /* 0x0000000C */
#define TOPCRU_CLKSEL_CON35_CLK_PKA_CRYPTO_SRC_SEL_SHIFT   (4U)
#define TOPCRU_CLKSEL_CON35_CLK_PKA_CRYPTO_SRC_SEL_MASK    (0x3U << TOPCRU_CLKSEL_CON35_CLK_PKA_CRYPTO_SRC_SEL_SHIFT)   /* 0x00000030 */
#define TOPCRU_CLKSEL_CON35_CLK_CORE_RGA_SRC_SEL_SHIFT     (8U)
#define TOPCRU_CLKSEL_CON35_CLK_CORE_RGA_SRC_SEL_MASK      (0x1U << TOPCRU_CLKSEL_CON35_CLK_CORE_RGA_SRC_SEL_SHIFT)     /* 0x00000100 */
#define TOPCRU_CLKSEL_CON35_MCLK_SAI_SRC_SEL_SHIFT         (10U)
#define TOPCRU_CLKSEL_CON35_MCLK_SAI_SRC_SEL_MASK          (0x3U << TOPCRU_CLKSEL_CON35_MCLK_SAI_SRC_SEL_SHIFT)         /* 0x00000C00 */
#define TOPCRU_CLKSEL_CON35_CLK_FREQ_PWM0_SRC_SEL_SHIFT    (12U)
#define TOPCRU_CLKSEL_CON35_CLK_FREQ_PWM0_SRC_SEL_MASK     (0x3U << TOPCRU_CLKSEL_CON35_CLK_FREQ_PWM0_SRC_SEL_SHIFT)    /* 0x00003000 */
#define TOPCRU_CLKSEL_CON35_CLK_COUNTER_PWM0_SRC_SEL_SHIFT (14U)
#define TOPCRU_CLKSEL_CON35_CLK_COUNTER_PWM0_SRC_SEL_MASK  (0x3U << TOPCRU_CLKSEL_CON35_CLK_COUNTER_PWM0_SRC_SEL_SHIFT) /* 0x0000C000 */
/* CLKSEL_CON36 */
#define TOPCRU_CLKSEL_CON36_OFFSET                         (0x390U)
#define TOPCRU_CLKSEL_CON36_CCLK_SDMMC1_SRC_DIV_SHIFT      (0U)
#define TOPCRU_CLKSEL_CON36_CCLK_SDMMC1_SRC_DIV_MASK       (0xFFU << TOPCRU_CLKSEL_CON36_CCLK_SDMMC1_SRC_DIV_SHIFT)     /* 0x000000FF */
#define TOPCRU_CLKSEL_CON36_CCLK_SDMMC1_SRC_SEL_SHIFT      (15U)
#define TOPCRU_CLKSEL_CON36_CCLK_SDMMC1_SRC_SEL_MASK       (0x1U << TOPCRU_CLKSEL_CON36_CCLK_SDMMC1_SRC_SEL_SHIFT)      /* 0x00008000 */
/* CLKSEL_CON37 */
#define TOPCRU_CLKSEL_CON37_OFFSET                         (0x394U)
#define TOPCRU_CLKSEL_CON37_CLK_NPU_GPLL_SRC_SEL_SHIFT     (0U)
#define TOPCRU_CLKSEL_CON37_CLK_NPU_GPLL_SRC_SEL_MASK      (0x1U << TOPCRU_CLKSEL_CON37_CLK_NPU_GPLL_SRC_SEL_SHIFT)     /* 0x00000001 */
#define TOPCRU_CLKSEL_CON37_CLK_NPU_GPLL_SRC_DIV_SHIFT     (1U)
#define TOPCRU_CLKSEL_CON37_CLK_NPU_GPLL_SRC_DIV_MASK      (0x3U << TOPCRU_CLKSEL_CON37_CLK_NPU_GPLL_SRC_DIV_SHIFT)     /* 0x00000006 */
#define TOPCRU_CLKSEL_CON37_CLK_VEPU_GPLL_SRC_SEL_SHIFT    (4U)
#define TOPCRU_CLKSEL_CON37_CLK_VEPU_GPLL_SRC_SEL_MASK     (0x1U << TOPCRU_CLKSEL_CON37_CLK_VEPU_GPLL_SRC_SEL_SHIFT)    /* 0x00000010 */
#define TOPCRU_CLKSEL_CON37_CLK_VEPU_GPLL_SRC_DIV_SHIFT    (5U)
#define TOPCRU_CLKSEL_CON37_CLK_VEPU_GPLL_SRC_DIV_MASK     (0x3U << TOPCRU_CLKSEL_CON37_CLK_VEPU_GPLL_SRC_DIV_SHIFT)    /* 0x00000060 */
#define TOPCRU_CLKSEL_CON37_CLK_ISP_GPLL_SRC_SEL_SHIFT     (8U)
#define TOPCRU_CLKSEL_CON37_CLK_ISP_GPLL_SRC_SEL_MASK      (0x1U << TOPCRU_CLKSEL_CON37_CLK_ISP_GPLL_SRC_SEL_SHIFT)     /* 0x00000100 */
#define TOPCRU_CLKSEL_CON37_CLK_ISP_GPLL_SRC_DIV_SHIFT     (9U)
#define TOPCRU_CLKSEL_CON37_CLK_ISP_GPLL_SRC_DIV_MASK      (0x3U << TOPCRU_CLKSEL_CON37_CLK_ISP_GPLL_SRC_DIV_SHIFT)     /* 0x00000600 */
#define TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_SEL_SHIFT    (12U)
#define TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_SEL_MASK     (0x1U << TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_SEL_SHIFT)    /* 0x00001000 */
#define TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_DIV_SHIFT    (13U)
#define TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_DIV_MASK     (0x7U << TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_DIV_SHIFT)    /* 0x0000E000 */
/* CLKSEL_CON40 */
#define TOPCRU_CLKSEL_CON40_OFFSET                         (0x3A0U)
#define TOPCRU_CLKSEL_CON40_CLK_REF_MIPI0_SRC_DIV_SHIFT    (0U)
#define TOPCRU_CLKSEL_CON40_CLK_REF_MIPI0_SRC_DIV_MASK     (0x1FU << TOPCRU_CLKSEL_CON40_CLK_REF_MIPI0_SRC_DIV_SHIFT)   /* 0x0000001F */
#define TOPCRU_CLKSEL_CON40_CLK_MIPI0_OUT2IO_SEL_SHIFT     (6U)
#define TOPCRU_CLKSEL_CON40_CLK_MIPI0_OUT2IO_SEL_MASK      (0x1U << TOPCRU_CLKSEL_CON40_CLK_MIPI0_OUT2IO_SEL_SHIFT)     /* 0x00000040 */
#define TOPCRU_CLKSEL_CON40_CLK_REF_MIPI1_SRC_DIV_SHIFT    (8U)
#define TOPCRU_CLKSEL_CON40_CLK_REF_MIPI1_SRC_DIV_MASK     (0x1FU << TOPCRU_CLKSEL_CON40_CLK_REF_MIPI1_SRC_DIV_SHIFT)   /* 0x00001F00 */
#define TOPCRU_CLKSEL_CON40_CLK_MIPI1_OUT2IO_SEL_SHIFT     (14U)
#define TOPCRU_CLKSEL_CON40_CLK_MIPI1_OUT2IO_SEL_MASK      (0x1U << TOPCRU_CLKSEL_CON40_CLK_MIPI1_OUT2IO_SEL_SHIFT)     /* 0x00004000 */
/* CLKSEL_CON41 */
#define TOPCRU_CLKSEL_CON41_OFFSET                         (0x3A4U)
#define TOPCRU_CLKSEL_CON41_MCLK_SAI_OUT2IO_SEL_SHIFT      (7U)
#define TOPCRU_CLKSEL_CON41_MCLK_SAI_OUT2IO_SEL_MASK       (0x1U << TOPCRU_CLKSEL_CON41_MCLK_SAI_OUT2IO_SEL_SHIFT)      /* 0x00000080 */
#define TOPCRU_CLKSEL_CON41_CLK_TESTOUT_TOP_DIV_SHIFT      (8U)
#define TOPCRU_CLKSEL_CON41_CLK_TESTOUT_TOP_DIV_MASK       (0x3U << TOPCRU_CLKSEL_CON41_CLK_TESTOUT_TOP_DIV_SHIFT)      /* 0x00000300 */
#define TOPCRU_CLKSEL_CON41_CLK_TESTOUT_TOP_SEL_SHIFT      (10U)
#define TOPCRU_CLKSEL_CON41_CLK_TESTOUT_TOP_SEL_MASK       (0x7U << TOPCRU_CLKSEL_CON41_CLK_TESTOUT_TOP_SEL_SHIFT)      /* 0x00001C00 */
#define TOPCRU_CLKSEL_CON41_MCLK_SAI_OUT2IO_DIV_SHIFT      (13U)
#define TOPCRU_CLKSEL_CON41_MCLK_SAI_OUT2IO_DIV_MASK       (0x7U << TOPCRU_CLKSEL_CON41_MCLK_SAI_OUT2IO_DIV_SHIFT)      /* 0x0000E000 */
/* GATE_CON00 */
#define TOPCRU_GATE_CON00_OFFSET                           (0x800U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_050M_SRC_EN_SHIFT     (0U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_050M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_050M_SRC_EN_SHIFT)     /* 0x00000001 */
#define TOPCRU_GATE_CON00_CLK_MATRIX_100M_SRC_EN_SHIFT     (1U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_100M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_100M_SRC_EN_SHIFT)     /* 0x00000002 */
#define TOPCRU_GATE_CON00_CLK_MATRIX_200M_SRC_EN_SHIFT     (3U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_200M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_200M_SRC_EN_SHIFT)     /* 0x00000008 */
#define TOPCRU_GATE_CON00_CLK_MATRIX_300M_SRC_EN_SHIFT     (5U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_300M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_300M_SRC_EN_SHIFT)     /* 0x00000020 */
#define TOPCRU_GATE_CON00_CLK_MATRIX_400M_SRC_EN_SHIFT     (7U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_400M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_400M_SRC_EN_SHIFT)     /* 0x00000080 */
#define TOPCRU_GATE_CON00_CLK_MATRIX_480M_SRC_EN_SHIFT     (8U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_480M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_480M_SRC_EN_SHIFT)     /* 0x00000100 */
#define TOPCRU_GATE_CON00_CLK_MATRIX_600M_SRC_EN_SHIFT     (9U)
#define TOPCRU_GATE_CON00_CLK_MATRIX_600M_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON00_CLK_MATRIX_600M_SRC_EN_SHIFT)     /* 0x00000200 */
#define TOPCRU_GATE_CON00_CLK_DPLL_DDRPHY_EN_SHIFT         (11U)
#define TOPCRU_GATE_CON00_CLK_DPLL_DDRPHY_EN_MASK          (0x1U << TOPCRU_GATE_CON00_CLK_DPLL_DDRPHY_EN_SHIFT)         /* 0x00000800 */
#define TOPCRU_GATE_CON00_CLK4X_SCANHS_DDR_SRC_EN_SHIFT    (12U)
#define TOPCRU_GATE_CON00_CLK4X_SCANHS_DDR_SRC_EN_MASK     (0x1U << TOPCRU_GATE_CON00_CLK4X_SCANHS_DDR_SRC_EN_SHIFT)    /* 0x00001000 */
/* GATE_CON01 */
#define TOPCRU_GATE_CON01_OFFSET                           (0x804U)
#define TOPCRU_GATE_CON01_CLK_UART0_SRC_EN_SHIFT           (0U)
#define TOPCRU_GATE_CON01_CLK_UART0_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON01_CLK_UART0_SRC_EN_SHIFT)           /* 0x00000001 */
#define TOPCRU_GATE_CON01_CLK_UART1_SRC_EN_SHIFT           (1U)
#define TOPCRU_GATE_CON01_CLK_UART1_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON01_CLK_UART1_SRC_EN_SHIFT)           /* 0x00000002 */
#define TOPCRU_GATE_CON01_CLK_UART2_SRC_EN_SHIFT           (2U)
#define TOPCRU_GATE_CON01_CLK_UART2_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON01_CLK_UART2_SRC_EN_SHIFT)           /* 0x00000004 */
#define TOPCRU_GATE_CON01_CLK_UART0_FRAC_EN_SHIFT          (6U)
#define TOPCRU_GATE_CON01_CLK_UART0_FRAC_EN_MASK           (0x1U << TOPCRU_GATE_CON01_CLK_UART0_FRAC_EN_SHIFT)          /* 0x00000040 */
#define TOPCRU_GATE_CON01_CLK_UART1_FRAC_EN_SHIFT          (7U)
#define TOPCRU_GATE_CON01_CLK_UART1_FRAC_EN_MASK           (0x1U << TOPCRU_GATE_CON01_CLK_UART1_FRAC_EN_SHIFT)          /* 0x00000080 */
#define TOPCRU_GATE_CON01_CLK_UART2_FRAC_EN_SHIFT          (8U)
#define TOPCRU_GATE_CON01_CLK_UART2_FRAC_EN_MASK           (0x1U << TOPCRU_GATE_CON01_CLK_UART2_FRAC_EN_SHIFT)          /* 0x00000100 */
#define TOPCRU_GATE_CON01_CLK_SAI_EN_SHIFT                 (12U)
#define TOPCRU_GATE_CON01_CLK_SAI_EN_MASK                  (0x1U << TOPCRU_GATE_CON01_CLK_SAI_EN_SHIFT)                 /* 0x00001000 */
#define TOPCRU_GATE_CON01_CLK_SAI_FRAC_EN_SHIFT            (13U)
#define TOPCRU_GATE_CON01_CLK_SAI_FRAC_EN_MASK             (0x1U << TOPCRU_GATE_CON01_CLK_SAI_FRAC_EN_SHIFT)            /* 0x00002000 */
/* GATE_CON02 */
#define TOPCRU_GATE_CON02_OFFSET                           (0x808U)
#define TOPCRU_GATE_CON02_LSCLK_NPU_SRC_EN_SHIFT           (0U)
#define TOPCRU_GATE_CON02_LSCLK_NPU_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON02_LSCLK_NPU_SRC_EN_SHIFT)           /* 0x00000001 */
#define TOPCRU_GATE_CON02_ACLK_VEPU_SRC_EN_SHIFT           (4U)
#define TOPCRU_GATE_CON02_ACLK_VEPU_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON02_ACLK_VEPU_SRC_EN_SHIFT)           /* 0x00000010 */
#define TOPCRU_GATE_CON02_ACLK_VI_SRC_EN_SHIFT             (8U)
#define TOPCRU_GATE_CON02_ACLK_VI_SRC_EN_MASK              (0x1U << TOPCRU_GATE_CON02_ACLK_VI_SRC_EN_SHIFT)             /* 0x00000100 */
#define TOPCRU_GATE_CON02_DCLK_VICAP_SRC_EN_SHIFT          (9U)
#define TOPCRU_GATE_CON02_DCLK_VICAP_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON02_DCLK_VICAP_SRC_EN_SHIFT)          /* 0x00000200 */
#define TOPCRU_GATE_CON02_CCLK_EMMC_SRC_EN_SHIFT           (10U)
#define TOPCRU_GATE_CON02_CCLK_EMMC_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON02_CCLK_EMMC_SRC_EN_SHIFT)           /* 0x00000400 */
#define TOPCRU_GATE_CON02_CCLK_SDMMC0_SRC_EN_SHIFT         (11U)
#define TOPCRU_GATE_CON02_CCLK_SDMMC0_SRC_EN_MASK          (0x1U << TOPCRU_GATE_CON02_CCLK_SDMMC0_SRC_EN_SHIFT)         /* 0x00000800 */
#define TOPCRU_GATE_CON02_SCLK_SFC_2X_SRC_EN_SHIFT         (12U)
#define TOPCRU_GATE_CON02_SCLK_SFC_2X_SRC_EN_MASK          (0x1U << TOPCRU_GATE_CON02_SCLK_SFC_2X_SRC_EN_SHIFT)         /* 0x00001000 */
/* GATE_CON03 */
#define TOPCRU_GATE_CON03_OFFSET                           (0x80CU)
#define TOPCRU_GATE_CON03_LSCLK_PERI_SRC_EN_SHIFT          (0U)
#define TOPCRU_GATE_CON03_LSCLK_PERI_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON03_LSCLK_PERI_SRC_EN_SHIFT)          /* 0x00000001 */
#define TOPCRU_GATE_CON03_ACLK_PERI_SRC_EN_SHIFT           (1U)
#define TOPCRU_GATE_CON03_ACLK_PERI_SRC_EN_MASK            (0x1U << TOPCRU_GATE_CON03_ACLK_PERI_SRC_EN_SHIFT)           /* 0x00000002 */
#define TOPCRU_GATE_CON03_HCLK_HPMCU_SRC_EN_SHIFT          (2U)
#define TOPCRU_GATE_CON03_HCLK_HPMCU_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON03_HCLK_HPMCU_SRC_EN_SHIFT)          /* 0x00000004 */
#define TOPCRU_GATE_CON03_SCLK_UART0_SRC_EN_SHIFT          (3U)
#define TOPCRU_GATE_CON03_SCLK_UART0_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON03_SCLK_UART0_SRC_EN_SHIFT)          /* 0x00000008 */
#define TOPCRU_GATE_CON03_SCLK_UART1_SRC_EN_SHIFT          (4U)
#define TOPCRU_GATE_CON03_SCLK_UART1_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON03_SCLK_UART1_SRC_EN_SHIFT)          /* 0x00000010 */
#define TOPCRU_GATE_CON03_SCLK_UART2_SRC_EN_SHIFT          (8U)
#define TOPCRU_GATE_CON03_SCLK_UART2_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON03_SCLK_UART2_SRC_EN_SHIFT)          /* 0x00000100 */
/* GATE_CON04 */
#define TOPCRU_GATE_CON04_OFFSET                           (0x810U)
#define TOPCRU_GATE_CON04_CLK_I2C_PMU_SRC_EN_SHIFT         (0U)
#define TOPCRU_GATE_CON04_CLK_I2C_PMU_SRC_EN_MASK          (0x1U << TOPCRU_GATE_CON04_CLK_I2C_PMU_SRC_EN_SHIFT)         /* 0x00000001 */
#define TOPCRU_GATE_CON04_CLK_I2C_PERI_SRC_EN_SHIFT        (4U)
#define TOPCRU_GATE_CON04_CLK_I2C_PERI_SRC_EN_MASK         (0x1U << TOPCRU_GATE_CON04_CLK_I2C_PERI_SRC_EN_SHIFT)        /* 0x00000010 */
#define TOPCRU_GATE_CON04_CLK_SPI0_SRC_EN_SHIFT            (5U)
#define TOPCRU_GATE_CON04_CLK_SPI0_SRC_EN_MASK             (0x1U << TOPCRU_GATE_CON04_CLK_SPI0_SRC_EN_SHIFT)            /* 0x00000020 */
#define TOPCRU_GATE_CON04_CLK_PWM0_SRC_EN_SHIFT            (10U)
#define TOPCRU_GATE_CON04_CLK_PWM0_SRC_EN_MASK             (0x1U << TOPCRU_GATE_CON04_CLK_PWM0_SRC_EN_SHIFT)            /* 0x00000400 */
#define TOPCRU_GATE_CON04_CLK_PWM1_SRC_EN_SHIFT            (11U)
#define TOPCRU_GATE_CON04_CLK_PWM1_SRC_EN_MASK             (0x1U << TOPCRU_GATE_CON04_CLK_PWM1_SRC_EN_SHIFT)            /* 0x00000800 */
#define TOPCRU_GATE_CON04_CLK_PWM2_SRC_EN_SHIFT            (12U)
#define TOPCRU_GATE_CON04_CLK_PWM2_SRC_EN_MASK             (0x1U << TOPCRU_GATE_CON04_CLK_PWM2_SRC_EN_SHIFT)            /* 0x00001000 */
/* GATE_CON05 */
#define TOPCRU_GATE_CON05_OFFSET                           (0x814U)
#define TOPCRU_GATE_CON05_DCLK_DECOM_SRC_EN_SHIFT          (0U)
#define TOPCRU_GATE_CON05_DCLK_DECOM_SRC_EN_MASK           (0x1U << TOPCRU_GATE_CON05_DCLK_DECOM_SRC_EN_SHIFT)          /* 0x00000001 */
#define TOPCRU_GATE_CON05_CCLK_SDMMC1_SRC_EN_SHIFT         (1U)
#define TOPCRU_GATE_CON05_CCLK_SDMMC1_SRC_EN_MASK          (0x1U << TOPCRU_GATE_CON05_CCLK_SDMMC1_SRC_EN_SHIFT)         /* 0x00000002 */
#define TOPCRU_GATE_CON05_CLK_CORE_CRYPTO_SRC_EN_SHIFT     (2U)
#define TOPCRU_GATE_CON05_CLK_CORE_CRYPTO_SRC_EN_MASK      (0x1U << TOPCRU_GATE_CON05_CLK_CORE_CRYPTO_SRC_EN_SHIFT)     /* 0x00000004 */
#define TOPCRU_GATE_CON05_CLK_PKA_CRYPTO_SRC_EN_SHIFT      (3U)
#define TOPCRU_GATE_CON05_CLK_PKA_CRYPTO_SRC_EN_MASK       (0x1U << TOPCRU_GATE_CON05_CLK_PKA_CRYPTO_SRC_EN_SHIFT)      /* 0x00000008 */
#define TOPCRU_GATE_CON05_CLK_CORE_RGA_SRC_EN_SHIFT        (4U)
#define TOPCRU_GATE_CON05_CLK_CORE_RGA_SRC_EN_MASK         (0x1U << TOPCRU_GATE_CON05_CLK_CORE_RGA_SRC_EN_SHIFT)        /* 0x00000010 */
#define TOPCRU_GATE_CON05_MCLK_SAI_SRC_EN_SHIFT            (5U)
#define TOPCRU_GATE_CON05_MCLK_SAI_SRC_EN_MASK             (0x1U << TOPCRU_GATE_CON05_MCLK_SAI_SRC_EN_SHIFT)            /* 0x00000020 */
#define TOPCRU_GATE_CON05_CLK_FREQ_PWM0_SRC_EN_SHIFT       (6U)
#define TOPCRU_GATE_CON05_CLK_FREQ_PWM0_SRC_EN_MASK        (0x1U << TOPCRU_GATE_CON05_CLK_FREQ_PWM0_SRC_EN_SHIFT)       /* 0x00000040 */
#define TOPCRU_GATE_CON05_CLK_COUNTER_PWM0_SRC_EN_SHIFT    (7U)
#define TOPCRU_GATE_CON05_CLK_COUNTER_PWM0_SRC_EN_MASK     (0x1U << TOPCRU_GATE_CON05_CLK_COUNTER_PWM0_SRC_EN_SHIFT)    /* 0x00000080 */
#define TOPCRU_GATE_CON05_CLK_NPU_GPLL_SRC_EN_SHIFT        (12U)
#define TOPCRU_GATE_CON05_CLK_NPU_GPLL_SRC_EN_MASK         (0x1U << TOPCRU_GATE_CON05_CLK_NPU_GPLL_SRC_EN_SHIFT)        /* 0x00001000 */
#define TOPCRU_GATE_CON05_CLK_VEPU_GPLL_SRC_EN_SHIFT       (13U)
#define TOPCRU_GATE_CON05_CLK_VEPU_GPLL_SRC_EN_MASK        (0x1U << TOPCRU_GATE_CON05_CLK_VEPU_GPLL_SRC_EN_SHIFT)       /* 0x00002000 */
#define TOPCRU_GATE_CON05_CLK_ISP_GPLL_SRC_EN_SHIFT        (14U)
#define TOPCRU_GATE_CON05_CLK_ISP_GPLL_SRC_EN_MASK         (0x1U << TOPCRU_GATE_CON05_CLK_ISP_GPLL_SRC_EN_SHIFT)        /* 0x00004000 */
#define TOPCRU_GATE_CON05_CLK_CORE_GPLL_SRC_EN_SHIFT       (15U)
#define TOPCRU_GATE_CON05_CLK_CORE_GPLL_SRC_EN_MASK        (0x1U << TOPCRU_GATE_CON05_CLK_CORE_GPLL_SRC_EN_SHIFT)       /* 0x00008000 */
/* GATE_CON06 */
#define TOPCRU_GATE_CON06_OFFSET                           (0x818U)
#define TOPCRU_GATE_CON06_PCLK_TOP_ROOT_EN_SHIFT           (0U)
#define TOPCRU_GATE_CON06_PCLK_TOP_ROOT_EN_MASK            (0x1U << TOPCRU_GATE_CON06_PCLK_TOP_ROOT_EN_SHIFT)           /* 0x00000001 */
#define TOPCRU_GATE_CON06_PCLK_CRU_EN_SHIFT                (1U)
#define TOPCRU_GATE_CON06_PCLK_CRU_EN_MASK                 (0x1U << TOPCRU_GATE_CON06_PCLK_CRU_EN_SHIFT)                /* 0x00000002 */
#define TOPCRU_GATE_CON06_PCLK_CRU_BIU_EN_SHIFT            (2U)
#define TOPCRU_GATE_CON06_PCLK_CRU_BIU_EN_MASK             (0x1U << TOPCRU_GATE_CON06_PCLK_CRU_BIU_EN_SHIFT)            /* 0x00000004 */
#define TOPCRU_GATE_CON06_CLK_REF_MIPI0_SRC_EN_SHIFT       (3U)
#define TOPCRU_GATE_CON06_CLK_REF_MIPI0_SRC_EN_MASK        (0x1U << TOPCRU_GATE_CON06_CLK_REF_MIPI0_SRC_EN_SHIFT)       /* 0x00000008 */
#define TOPCRU_GATE_CON06_CLK_MIPI0_OUT2IO_EN_SHIFT        (4U)
#define TOPCRU_GATE_CON06_CLK_MIPI0_OUT2IO_EN_MASK         (0x1U << TOPCRU_GATE_CON06_CLK_MIPI0_OUT2IO_EN_SHIFT)        /* 0x00000010 */
#define TOPCRU_GATE_CON06_CLK_REF_MIPI1_SRC_EN_SHIFT       (5U)
#define TOPCRU_GATE_CON06_CLK_REF_MIPI1_SRC_EN_MASK        (0x1U << TOPCRU_GATE_CON06_CLK_REF_MIPI1_SRC_EN_SHIFT)       /* 0x00000020 */
#define TOPCRU_GATE_CON06_CLK_MIPI1_OUT2IO_EN_SHIFT        (6U)
#define TOPCRU_GATE_CON06_CLK_MIPI1_OUT2IO_EN_MASK         (0x1U << TOPCRU_GATE_CON06_CLK_MIPI1_OUT2IO_EN_SHIFT)        /* 0x00000040 */
#define TOPCRU_GATE_CON06_MCLK_SAI_OUT2IO_EN_SHIFT         (9U)
#define TOPCRU_GATE_CON06_MCLK_SAI_OUT2IO_EN_MASK          (0x1U << TOPCRU_GATE_CON06_MCLK_SAI_OUT2IO_EN_SHIFT)         /* 0x00000200 */
#define TOPCRU_GATE_CON06_CLK_GPLLTEST_EN_SHIFT            (12U)
#define TOPCRU_GATE_CON06_CLK_GPLLTEST_EN_MASK             (0x1U << TOPCRU_GATE_CON06_CLK_GPLLTEST_EN_SHIFT)            /* 0x00001000 */
#define TOPCRU_GATE_CON06_CLK_DPLLTEST_EN_SHIFT            (13U)
#define TOPCRU_GATE_CON06_CLK_DPLLTEST_EN_MASK             (0x1U << TOPCRU_GATE_CON06_CLK_DPLLTEST_EN_SHIFT)            /* 0x00002000 */
#define TOPCRU_GATE_CON06_CLK_TESTOUT_TOP_EN_SHIFT         (15U)
#define TOPCRU_GATE_CON06_CLK_TESTOUT_TOP_EN_MASK          (0x1U << TOPCRU_GATE_CON06_CLK_TESTOUT_TOP_EN_SHIFT)         /* 0x00008000 */
/* MODE_CON00 */
#define TOPCRU_MODE_CON00_OFFSET                           (0x280U)
#define TOPCRU_MODE_CON00_CLK_GPLL_MODE_SHIFT              (0U)
#define TOPCRU_MODE_CON00_CLK_GPLL_MODE_MASK               (0x3U << TOPCRU_MODE_CON00_CLK_GPLL_MODE_SHIFT)              /* 0x00000003 */
/* SOFTRST_CON00 */
#define TOPCRU_SOFTRST_CON00_OFFSET                        (0xA00U)
#define TOPCRU_SOFTRST_CON00_PRESETN_CRU_SHIFT             (0U)
#define TOPCRU_SOFTRST_CON00_PRESETN_CRU_MASK              (0x1U << TOPCRU_SOFTRST_CON00_PRESETN_CRU_SHIFT)             /* 0x00000001 */
#define TOPCRU_SOFTRST_CON00_PRESETN_CRU_BIU_SHIFT         (1U)
#define TOPCRU_SOFTRST_CON00_PRESETN_CRU_BIU_MASK          (0x1U << TOPCRU_SOFTRST_CON00_PRESETN_CRU_BIU_SHIFT)         /* 0x00000002 */
#define TOPCRU_SOFTRST_CON00_RESETN_DDRPHY_SHIFT           (12U)
#define TOPCRU_SOFTRST_CON00_RESETN_DDRPHY_MASK            (0x1U << TOPCRU_SOFTRST_CON00_RESETN_DDRPHY_SHIFT)           /* 0x00001000 */
/* CRU_DPLL_CON0 */
#define TOPCRU_CRU_DPLL_CON0_OFFSET                        (0x40U)
#define TOPCRU_CRU_DPLL_CON0_FBDIV_SHIFT                   (0U)
#define TOPCRU_CRU_DPLL_CON0_FBDIV_MASK                    (0xFFFU << TOPCRU_CRU_DPLL_CON0_FBDIV_SHIFT)                 /* 0x00000FFF */
#define TOPCRU_CRU_DPLL_CON0_POSTDIV1_SHIFT                (12U)
#define TOPCRU_CRU_DPLL_CON0_POSTDIV1_MASK                 (0x7U << TOPCRU_CRU_DPLL_CON0_POSTDIV1_SHIFT)                /* 0x00007000 */
#define TOPCRU_CRU_DPLL_CON0_BYPASS_SHIFT                  (15U)
#define TOPCRU_CRU_DPLL_CON0_BYPASS_MASK                   (0x1U << TOPCRU_CRU_DPLL_CON0_BYPASS_SHIFT)                  /* 0x00008000 */
/* CRU_DPLL_CON1 */
#define TOPCRU_CRU_DPLL_CON1_OFFSET                        (0x44U)
#define TOPCRU_CRU_DPLL_CON1_REFDIV_SHIFT                  (0U)
#define TOPCRU_CRU_DPLL_CON1_REFDIV_MASK                   (0x3FU << TOPCRU_CRU_DPLL_CON1_REFDIV_SHIFT)                 /* 0x0000003F */
#define TOPCRU_CRU_DPLL_CON1_POSTDIV2_SHIFT                (6U)
#define TOPCRU_CRU_DPLL_CON1_POSTDIV2_MASK                 (0x7U << TOPCRU_CRU_DPLL_CON1_POSTDIV2_SHIFT)                /* 0x000001C0 */
#define TOPCRU_CRU_DPLL_CON1_PLL_LOCK_SHIFT                (10U)
#define TOPCRU_CRU_DPLL_CON1_PLL_LOCK_MASK                 (0x1U << TOPCRU_CRU_DPLL_CON1_PLL_LOCK_SHIFT)                /* 0x00000400 */
#define TOPCRU_CRU_DPLL_CON1_DSMPD_SHIFT                   (12U)
#define TOPCRU_CRU_DPLL_CON1_DSMPD_MASK                    (0x1U << TOPCRU_CRU_DPLL_CON1_DSMPD_SHIFT)                   /* 0x00001000 */
#define TOPCRU_CRU_DPLL_CON1_PLLPD0_SHIFT                  (13U)
#define TOPCRU_CRU_DPLL_CON1_PLLPD0_MASK                   (0x1U << TOPCRU_CRU_DPLL_CON1_PLLPD0_SHIFT)                  /* 0x00002000 */
#define TOPCRU_CRU_DPLL_CON1_PLLPD1_SHIFT                  (14U)
#define TOPCRU_CRU_DPLL_CON1_PLLPD1_MASK                   (0x1U << TOPCRU_CRU_DPLL_CON1_PLLPD1_SHIFT)                  /* 0x00004000 */
#define TOPCRU_CRU_DPLL_CON1_PLLPDSEL_SHIFT                (15U)
#define TOPCRU_CRU_DPLL_CON1_PLLPDSEL_MASK                 (0x1U << TOPCRU_CRU_DPLL_CON1_PLLPDSEL_SHIFT)                /* 0x00008000 */
/* CRU_DPLL_CON2 */
#define TOPCRU_CRU_DPLL_CON2_OFFSET                        (0x48U)
#define TOPCRU_CRU_DPLL_CON2_FRACDIV_SHIFT                 (0U)
#define TOPCRU_CRU_DPLL_CON2_FRACDIV_MASK                  (0xFFFFFFU << TOPCRU_CRU_DPLL_CON2_FRACDIV_SHIFT)            /* 0x00FFFFFF */
#define TOPCRU_CRU_DPLL_CON2_DACPD_SHIFT                   (24U)
#define TOPCRU_CRU_DPLL_CON2_DACPD_MASK                    (0x1U << TOPCRU_CRU_DPLL_CON2_DACPD_SHIFT)                   /* 0x01000000 */
#define TOPCRU_CRU_DPLL_CON2_FOUTPOSTDIVPD_SHIFT           (25U)
#define TOPCRU_CRU_DPLL_CON2_FOUTPOSTDIVPD_MASK            (0x1U << TOPCRU_CRU_DPLL_CON2_FOUTPOSTDIVPD_SHIFT)           /* 0x02000000 */
#define TOPCRU_CRU_DPLL_CON2_FOUTVCOPD_SHIFT               (26U)
#define TOPCRU_CRU_DPLL_CON2_FOUTVCOPD_MASK                (0x1U << TOPCRU_CRU_DPLL_CON2_FOUTVCOPD_SHIFT)               /* 0x04000000 */
#define TOPCRU_CRU_DPLL_CON2_FOUT4PHASEPD_SHIFT            (27U)
#define TOPCRU_CRU_DPLL_CON2_FOUT4PHASEPD_MASK             (0x1U << TOPCRU_CRU_DPLL_CON2_FOUT4PHASEPD_SHIFT)            /* 0x08000000 */
/* CRU_DPLL_CON3 */
#define TOPCRU_CRU_DPLL_CON3_OFFSET                        (0x4CU)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_BP_SHIFT                (0U)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_BP_MASK                 (0x1U << TOPCRU_CRU_DPLL_CON3_SSMOD_BP_SHIFT)                /* 0x00000001 */
#define TOPCRU_CRU_DPLL_CON3_SSMOD_DISABLE_SSCG_SHIFT      (1U)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_DISABLE_SSCG_MASK       (0x1U << TOPCRU_CRU_DPLL_CON3_SSMOD_DISABLE_SSCG_SHIFT)      /* 0x00000002 */
#define TOPCRU_CRU_DPLL_CON3_SSMOD_RESET_SHIFT             (2U)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_RESET_MASK              (0x1U << TOPCRU_CRU_DPLL_CON3_SSMOD_RESET_SHIFT)             /* 0x00000004 */
#define TOPCRU_CRU_DPLL_CON3_SSMOD_DOWNSPREAD_SHIFT        (3U)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_DOWNSPREAD_MASK         (0x1U << TOPCRU_CRU_DPLL_CON3_SSMOD_DOWNSPREAD_SHIFT)        /* 0x00000008 */
#define TOPCRU_CRU_DPLL_CON3_SSMOD_DIVVAL_SHIFT            (4U)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_DIVVAL_MASK             (0xFU << TOPCRU_CRU_DPLL_CON3_SSMOD_DIVVAL_SHIFT)            /* 0x000000F0 */
#define TOPCRU_CRU_DPLL_CON3_SSMOD_SPREAD_SHIFT            (8U)
#define TOPCRU_CRU_DPLL_CON3_SSMOD_SPREAD_MASK             (0x1FU << TOPCRU_CRU_DPLL_CON3_SSMOD_SPREAD_SHIFT)           /* 0x00001F00 */
/* CRU_DPLL_CON4 */
#define TOPCRU_CRU_DPLL_CON4_OFFSET                        (0x50U)
#define TOPCRU_CRU_DPLL_CON4_SSMOD_SEL_EXT_WAVE_SHIFT      (0U)
#define TOPCRU_CRU_DPLL_CON4_SSMOD_SEL_EXT_WAVE_MASK       (0x1U << TOPCRU_CRU_DPLL_CON4_SSMOD_SEL_EXT_WAVE_SHIFT)      /* 0x00000001 */
#define TOPCRU_CRU_DPLL_CON4_SSMOD_EXT_MAXADDR_SHIFT       (8U)
#define TOPCRU_CRU_DPLL_CON4_SSMOD_EXT_MAXADDR_MASK        (0xFFU << TOPCRU_CRU_DPLL_CON4_SSMOD_EXT_MAXADDR_SHIFT)      /* 0x0000FF00 */
/* CRU_GPLL_CON0 */
#define TOPCRU_CRU_GPLL_CON0_OFFSET                        (0x60U)
#define TOPCRU_CRU_GPLL_CON0_FBDIV_SHIFT                   (0U)
#define TOPCRU_CRU_GPLL_CON0_FBDIV_MASK                    (0xFFFU << TOPCRU_CRU_GPLL_CON0_FBDIV_SHIFT)                 /* 0x00000FFF */
#define TOPCRU_CRU_GPLL_CON0_POSTDIV1_SHIFT                (12U)
#define TOPCRU_CRU_GPLL_CON0_POSTDIV1_MASK                 (0x7U << TOPCRU_CRU_GPLL_CON0_POSTDIV1_SHIFT)                /* 0x00007000 */
#define TOPCRU_CRU_GPLL_CON0_BYPASS_SHIFT                  (15U)
#define TOPCRU_CRU_GPLL_CON0_BYPASS_MASK                   (0x1U << TOPCRU_CRU_GPLL_CON0_BYPASS_SHIFT)                  /* 0x00008000 */
/* CRU_GPLL_CON1 */
#define TOPCRU_CRU_GPLL_CON1_OFFSET                        (0x64U)
#define TOPCRU_CRU_GPLL_CON1_REFDIV_SHIFT                  (0U)
#define TOPCRU_CRU_GPLL_CON1_REFDIV_MASK                   (0x3FU << TOPCRU_CRU_GPLL_CON1_REFDIV_SHIFT)                 /* 0x0000003F */
#define TOPCRU_CRU_GPLL_CON1_POSTDIV2_SHIFT                (6U)
#define TOPCRU_CRU_GPLL_CON1_POSTDIV2_MASK                 (0x7U << TOPCRU_CRU_GPLL_CON1_POSTDIV2_SHIFT)                /* 0x000001C0 */
#define TOPCRU_CRU_GPLL_CON1_PLL_LOCK_SHIFT                (10U)
#define TOPCRU_CRU_GPLL_CON1_PLL_LOCK_MASK                 (0x1U << TOPCRU_CRU_GPLL_CON1_PLL_LOCK_SHIFT)                /* 0x00000400 */
#define TOPCRU_CRU_GPLL_CON1_DSMPD_SHIFT                   (12U)
#define TOPCRU_CRU_GPLL_CON1_DSMPD_MASK                    (0x1U << TOPCRU_CRU_GPLL_CON1_DSMPD_SHIFT)                   /* 0x00001000 */
#define TOPCRU_CRU_GPLL_CON1_PLLPD0_SHIFT                  (13U)
#define TOPCRU_CRU_GPLL_CON1_PLLPD0_MASK                   (0x1U << TOPCRU_CRU_GPLL_CON1_PLLPD0_SHIFT)                  /* 0x00002000 */
#define TOPCRU_CRU_GPLL_CON1_PLLPD1_SHIFT                  (14U)
#define TOPCRU_CRU_GPLL_CON1_PLLPD1_MASK                   (0x1U << TOPCRU_CRU_GPLL_CON1_PLLPD1_SHIFT)                  /* 0x00004000 */
#define TOPCRU_CRU_GPLL_CON1_PLLPDSEL_SHIFT                (15U)
#define TOPCRU_CRU_GPLL_CON1_PLLPDSEL_MASK                 (0x1U << TOPCRU_CRU_GPLL_CON1_PLLPDSEL_SHIFT)                /* 0x00008000 */
/* CRU_GPLL_CON2 */
#define TOPCRU_CRU_GPLL_CON2_OFFSET                        (0x68U)
#define TOPCRU_CRU_GPLL_CON2_FRACDIV_SHIFT                 (0U)
#define TOPCRU_CRU_GPLL_CON2_FRACDIV_MASK                  (0xFFFFFFU << TOPCRU_CRU_GPLL_CON2_FRACDIV_SHIFT)            /* 0x00FFFFFF */
#define TOPCRU_CRU_GPLL_CON2_DACPD_SHIFT                   (24U)
#define TOPCRU_CRU_GPLL_CON2_DACPD_MASK                    (0x1U << TOPCRU_CRU_GPLL_CON2_DACPD_SHIFT)                   /* 0x01000000 */
#define TOPCRU_CRU_GPLL_CON2_FOUTPOSTDIVPD_SHIFT           (25U)
#define TOPCRU_CRU_GPLL_CON2_FOUTPOSTDIVPD_MASK            (0x1U << TOPCRU_CRU_GPLL_CON2_FOUTPOSTDIVPD_SHIFT)           /* 0x02000000 */
#define TOPCRU_CRU_GPLL_CON2_FOUTVCOPD_SHIFT               (26U)
#define TOPCRU_CRU_GPLL_CON2_FOUTVCOPD_MASK                (0x1U << TOPCRU_CRU_GPLL_CON2_FOUTVCOPD_SHIFT)               /* 0x04000000 */
#define TOPCRU_CRU_GPLL_CON2_FOUT4PHASEPD_SHIFT            (27U)
#define TOPCRU_CRU_GPLL_CON2_FOUT4PHASEPD_MASK             (0x1U << TOPCRU_CRU_GPLL_CON2_FOUT4PHASEPD_SHIFT)            /* 0x08000000 */
/* CRU_GPLL_CON3 */
#define TOPCRU_CRU_GPLL_CON3_OFFSET                        (0x6CU)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_BP_SHIFT                (0U)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_BP_MASK                 (0x1U << TOPCRU_CRU_GPLL_CON3_SSMOD_BP_SHIFT)                /* 0x00000001 */
#define TOPCRU_CRU_GPLL_CON3_SSMOD_DISABLE_SSCG_SHIFT      (1U)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_DISABLE_SSCG_MASK       (0x1U << TOPCRU_CRU_GPLL_CON3_SSMOD_DISABLE_SSCG_SHIFT)      /* 0x00000002 */
#define TOPCRU_CRU_GPLL_CON3_SSMOD_RESET_SHIFT             (2U)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_RESET_MASK              (0x1U << TOPCRU_CRU_GPLL_CON3_SSMOD_RESET_SHIFT)             /* 0x00000004 */
#define TOPCRU_CRU_GPLL_CON3_SSMOD_DOWNSPREAD_SHIFT        (3U)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_DOWNSPREAD_MASK         (0x1U << TOPCRU_CRU_GPLL_CON3_SSMOD_DOWNSPREAD_SHIFT)        /* 0x00000008 */
#define TOPCRU_CRU_GPLL_CON3_SSMOD_DIVVAL_SHIFT            (4U)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_DIVVAL_MASK             (0xFU << TOPCRU_CRU_GPLL_CON3_SSMOD_DIVVAL_SHIFT)            /* 0x000000F0 */
#define TOPCRU_CRU_GPLL_CON3_SSMOD_SPREAD_SHIFT            (8U)
#define TOPCRU_CRU_GPLL_CON3_SSMOD_SPREAD_MASK             (0x1FU << TOPCRU_CRU_GPLL_CON3_SSMOD_SPREAD_SHIFT)           /* 0x00001F00 */
/* CRU_GPLL_CON4 */
#define TOPCRU_CRU_GPLL_CON4_OFFSET                        (0x70U)
#define TOPCRU_CRU_GPLL_CON4_SSMOD_SEL_EXT_WAVE_SHIFT      (0U)
#define TOPCRU_CRU_GPLL_CON4_SSMOD_SEL_EXT_WAVE_MASK       (0x1U << TOPCRU_CRU_GPLL_CON4_SSMOD_SEL_EXT_WAVE_SHIFT)      /* 0x00000001 */
#define TOPCRU_CRU_GPLL_CON4_SSMOD_EXT_MAXADDR_SHIFT       (8U)
#define TOPCRU_CRU_GPLL_CON4_SSMOD_EXT_MAXADDR_MASK        (0xFFU << TOPCRU_CRU_GPLL_CON4_SSMOD_EXT_MAXADDR_SHIFT)      /* 0x0000FF00 */
/* CRU_SSCGTBL_CON00 */
#define TOPCRU_CRU_SSCGTBL_CON00_OFFSET                    (0x140U)
#define TOPCRU_CRU_SSCGTBL_CON00_CRU_SSCGTBL_CON00_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON00_CRU_SSCGTBL_CON00_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON00_CRU_SSCGTBL_CON00_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON01 */
#define TOPCRU_CRU_SSCGTBL_CON01_OFFSET                    (0x144U)
#define TOPCRU_CRU_SSCGTBL_CON01_CRU_SSCGTBL_CON01_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON01_CRU_SSCGTBL_CON01_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON01_CRU_SSCGTBL_CON01_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON02 */
#define TOPCRU_CRU_SSCGTBL_CON02_OFFSET                    (0x148U)
#define TOPCRU_CRU_SSCGTBL_CON02_CRU_SSCGTBL_CON02_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON02_CRU_SSCGTBL_CON02_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON02_CRU_SSCGTBL_CON02_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON03 */
#define TOPCRU_CRU_SSCGTBL_CON03_OFFSET                    (0x14CU)
#define TOPCRU_CRU_SSCGTBL_CON03_CRU_SSCGTBL_CON03_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON03_CRU_SSCGTBL_CON03_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON03_CRU_SSCGTBL_CON03_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON04 */
#define TOPCRU_CRU_SSCGTBL_CON04_OFFSET                    (0x150U)
#define TOPCRU_CRU_SSCGTBL_CON04_CRU_SSCGTBL_CON04_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON04_CRU_SSCGTBL_CON04_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON04_CRU_SSCGTBL_CON04_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON05 */
#define TOPCRU_CRU_SSCGTBL_CON05_OFFSET                    (0x154U)
#define TOPCRU_CRU_SSCGTBL_CON05_CRU_SSCGTBL_CON05_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON05_CRU_SSCGTBL_CON05_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON05_CRU_SSCGTBL_CON05_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON06 */
#define TOPCRU_CRU_SSCGTBL_CON06_OFFSET                    (0x158U)
#define TOPCRU_CRU_SSCGTBL_CON06_CRU_SSCGTBL_CON06_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON06_CRU_SSCGTBL_CON06_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON06_CRU_SSCGTBL_CON06_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON07 */
#define TOPCRU_CRU_SSCGTBL_CON07_OFFSET                    (0x15CU)
#define TOPCRU_CRU_SSCGTBL_CON07_CRU_SSCGTBL_CON07_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON07_CRU_SSCGTBL_CON07_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON07_CRU_SSCGTBL_CON07_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON08 */
#define TOPCRU_CRU_SSCGTBL_CON08_OFFSET                    (0x160U)
#define TOPCRU_CRU_SSCGTBL_CON08_CRU_SSCGTBL_CON08_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON08_CRU_SSCGTBL_CON08_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON08_CRU_SSCGTBL_CON08_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON09 */
#define TOPCRU_CRU_SSCGTBL_CON09_OFFSET                    (0x164U)
#define TOPCRU_CRU_SSCGTBL_CON09_CRU_SSCGTBL_CON09_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON09_CRU_SSCGTBL_CON09_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON09_CRU_SSCGTBL_CON09_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON10 */
#define TOPCRU_CRU_SSCGTBL_CON10_OFFSET                    (0x168U)
#define TOPCRU_CRU_SSCGTBL_CON10_CRU_SSCGTBL_CON10_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON10_CRU_SSCGTBL_CON10_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON10_CRU_SSCGTBL_CON10_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON11 */
#define TOPCRU_CRU_SSCGTBL_CON11_OFFSET                    (0x16CU)
#define TOPCRU_CRU_SSCGTBL_CON11_CRU_SSCGTBL_CON11_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON11_CRU_SSCGTBL_CON11_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON11_CRU_SSCGTBL_CON11_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON12 */
#define TOPCRU_CRU_SSCGTBL_CON12_OFFSET                    (0x170U)
#define TOPCRU_CRU_SSCGTBL_CON12_CRU_SSCGTBL_CON12_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON12_CRU_SSCGTBL_CON12_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON12_CRU_SSCGTBL_CON12_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON13 */
#define TOPCRU_CRU_SSCGTBL_CON13_OFFSET                    (0x174U)
#define TOPCRU_CRU_SSCGTBL_CON13_CRU_SSCGTBL_CON13_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON13_CRU_SSCGTBL_CON13_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON13_CRU_SSCGTBL_CON13_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON14 */
#define TOPCRU_CRU_SSCGTBL_CON14_OFFSET                    (0x178U)
#define TOPCRU_CRU_SSCGTBL_CON14_CRU_SSCGTBL_CON14_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON14_CRU_SSCGTBL_CON14_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON14_CRU_SSCGTBL_CON14_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON15 */
#define TOPCRU_CRU_SSCGTBL_CON15_OFFSET                    (0x17CU)
#define TOPCRU_CRU_SSCGTBL_CON15_CRU_SSCGTBL_CON15_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON15_CRU_SSCGTBL_CON15_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON15_CRU_SSCGTBL_CON15_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON16 */
#define TOPCRU_CRU_SSCGTBL_CON16_OFFSET                    (0x180U)
#define TOPCRU_CRU_SSCGTBL_CON16_CRU_SSCGTBL_CON16_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON16_CRU_SSCGTBL_CON16_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON16_CRU_SSCGTBL_CON16_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON17 */
#define TOPCRU_CRU_SSCGTBL_CON17_OFFSET                    (0x184U)
#define TOPCRU_CRU_SSCGTBL_CON17_CRU_SSCGTBL_CON17_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON17_CRU_SSCGTBL_CON17_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON17_CRU_SSCGTBL_CON17_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON18 */
#define TOPCRU_CRU_SSCGTBL_CON18_OFFSET                    (0x188U)
#define TOPCRU_CRU_SSCGTBL_CON18_CRU_SSCGTBL_CON18_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON18_CRU_SSCGTBL_CON18_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON18_CRU_SSCGTBL_CON18_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON19 */
#define TOPCRU_CRU_SSCGTBL_CON19_OFFSET                    (0x18CU)
#define TOPCRU_CRU_SSCGTBL_CON19_CRU_SSCGTBL_CON19_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON19_CRU_SSCGTBL_CON19_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON19_CRU_SSCGTBL_CON19_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON20 */
#define TOPCRU_CRU_SSCGTBL_CON20_OFFSET                    (0x190U)
#define TOPCRU_CRU_SSCGTBL_CON20_CRU_SSCGTBL_CON20_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON20_CRU_SSCGTBL_CON20_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON20_CRU_SSCGTBL_CON20_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON21 */
#define TOPCRU_CRU_SSCGTBL_CON21_OFFSET                    (0x194U)
#define TOPCRU_CRU_SSCGTBL_CON21_CRU_SSCGTBL_CON21_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON21_CRU_SSCGTBL_CON21_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON21_CRU_SSCGTBL_CON21_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON22 */
#define TOPCRU_CRU_SSCGTBL_CON22_OFFSET                    (0x198U)
#define TOPCRU_CRU_SSCGTBL_CON22_CRU_SSCGTBL_CON22_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON22_CRU_SSCGTBL_CON22_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON22_CRU_SSCGTBL_CON22_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON23 */
#define TOPCRU_CRU_SSCGTBL_CON23_OFFSET                    (0x19CU)
#define TOPCRU_CRU_SSCGTBL_CON23_CRU_SSCGTBL_CON23_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON23_CRU_SSCGTBL_CON23_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON23_CRU_SSCGTBL_CON23_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON24 */
#define TOPCRU_CRU_SSCGTBL_CON24_OFFSET                    (0x1A0U)
#define TOPCRU_CRU_SSCGTBL_CON24_CRU_SSCGTBL_CON24_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON24_CRU_SSCGTBL_CON24_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON24_CRU_SSCGTBL_CON24_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON25 */
#define TOPCRU_CRU_SSCGTBL_CON25_OFFSET                    (0x1A4U)
#define TOPCRU_CRU_SSCGTBL_CON25_CRU_SSCGTBL_CON25_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON25_CRU_SSCGTBL_CON25_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON25_CRU_SSCGTBL_CON25_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON26 */
#define TOPCRU_CRU_SSCGTBL_CON26_OFFSET                    (0x1A8U)
#define TOPCRU_CRU_SSCGTBL_CON26_CRU_SSCGTBL_CON26_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON26_CRU_SSCGTBL_CON26_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON26_CRU_SSCGTBL_CON26_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON27 */
#define TOPCRU_CRU_SSCGTBL_CON27_OFFSET                    (0x1ACU)
#define TOPCRU_CRU_SSCGTBL_CON27_CRU_SSCGTBL_CON27_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON27_CRU_SSCGTBL_CON27_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON27_CRU_SSCGTBL_CON27_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON28 */
#define TOPCRU_CRU_SSCGTBL_CON28_OFFSET                    (0x1B0U)
#define TOPCRU_CRU_SSCGTBL_CON28_CRU_SSCGTBL_CON28_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON28_CRU_SSCGTBL_CON28_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON28_CRU_SSCGTBL_CON28_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON29 */
#define TOPCRU_CRU_SSCGTBL_CON29_OFFSET                    (0x1B4U)
#define TOPCRU_CRU_SSCGTBL_CON29_CRU_SSCGTBL_CON29_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON29_CRU_SSCGTBL_CON29_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON29_CRU_SSCGTBL_CON29_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON30 */
#define TOPCRU_CRU_SSCGTBL_CON30_OFFSET                    (0x1B8U)
#define TOPCRU_CRU_SSCGTBL_CON30_CRU_SSCGTBL_CON30_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON30_CRU_SSCGTBL_CON30_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON30_CRU_SSCGTBL_CON30_SHIFT) /* 0xFFFFFFFF */
/* CRU_SSCGTBL_CON31 */
#define TOPCRU_CRU_SSCGTBL_CON31_OFFSET                    (0x1BCU)
#define TOPCRU_CRU_SSCGTBL_CON31_CRU_SSCGTBL_CON31_SHIFT   (0U)
#define TOPCRU_CRU_SSCGTBL_CON31_CRU_SSCGTBL_CON31_MASK    (0xFFFFFFFFU << TOPCRU_CRU_SSCGTBL_CON31_CRU_SSCGTBL_CON31_SHIFT) /* 0xFFFFFFFF */
/* AUTOCS_ACLK_PERI_SRC_CON0 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_OFFSET            (0xD00U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_CLKIN_DIV_CON_SHIFT (0U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_CLKIN_DIV_CON_MASK (0x7U << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_CLKIN_DIV_CON_SHIFT) /* 0x00000007 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_CLKIN_DIV_EN_SHIFT (3U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_CLKIN_DIV_EN_MASK (0x1U << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_CLKIN_DIV_EN_SHIFT) /* 0x00000008 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_IDLE_TH_SHIFT     (4U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_IDLE_TH_MASK      (0xFFFU << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_IDLE_TH_SHIFT)   /* 0x0000FFF0 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_WAIT_TH_SHIFT     (16U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_WAIT_TH_MASK      (0xFFFFU << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON0_WAIT_TH_SHIFT)  /* 0xFFFF0000 */
/* AUTOCS_ACLK_PERI_SRC_CON1 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_OFFSET            (0xD04U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_AUTOCS_CTRL_SHIFT (0U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_AUTOCS_CTRL_MASK  (0xFFFU << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_AUTOCS_CTRL_SHIFT) /* 0x00000FFF */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_AUTOCS_EN_SHIFT   (12U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_AUTOCS_EN_MASK    (0x1U << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_AUTOCS_EN_SHIFT)   /* 0x00001000 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_SWITCH_EN_SHIFT   (13U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_SWITCH_EN_MASK    (0x1U << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_SWITCH_EN_SHIFT)   /* 0x00002000 */
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_CLKSEL_CFG_SHIFT  (14U)
#define TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_CLKSEL_CFG_MASK   (0x3U << TOPCRU_AUTOCS_ACLK_PERI_SRC_CON1_CLKSEL_CFG_SHIFT)  /* 0x0000C000 */
/* AUTOCS_HCLK_HPMCU_SRC_CON0 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_OFFSET           (0xD08U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_CLKIN_DIV_CON_SHIFT (0U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_CLKIN_DIV_CON_MASK (0x7U << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_CLKIN_DIV_CON_SHIFT) /* 0x00000007 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_CLKIN_DIV_EN_SHIFT (3U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_CLKIN_DIV_EN_MASK (0x1U << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_CLKIN_DIV_EN_SHIFT) /* 0x00000008 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_IDLE_TH_SHIFT    (4U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_IDLE_TH_MASK     (0xFFFU << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_IDLE_TH_SHIFT)  /* 0x0000FFF0 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_WAIT_TH_SHIFT    (16U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_WAIT_TH_MASK     (0xFFFFU << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON0_WAIT_TH_SHIFT) /* 0xFFFF0000 */
/* AUTOCS_HCLK_HPMCU_SRC_CON1 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_OFFSET           (0xD0CU)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_AUTOCS_CTRL_SHIFT (0U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_AUTOCS_CTRL_MASK (0xFFFU << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_AUTOCS_CTRL_SHIFT) /* 0x00000FFF */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_AUTOCS_EN_SHIFT  (12U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_AUTOCS_EN_MASK   (0x1U << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_AUTOCS_EN_SHIFT)  /* 0x00001000 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_SWITCH_EN_SHIFT  (13U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_SWITCH_EN_MASK   (0x1U << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_SWITCH_EN_SHIFT)  /* 0x00002000 */
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_CLKSEL_CFG_SHIFT (14U)
#define TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_CLKSEL_CFG_MASK  (0x3U << TOPCRU_AUTOCS_HCLK_HPMCU_SRC_CON1_CLKSEL_CFG_SHIFT) /* 0x0000C000 */
/* AUTOCS_LSCLK_PERI_SRC_CON0 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_OFFSET           (0xD10U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_CLKIN_DIV_CON_SHIFT (0U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_CLKIN_DIV_CON_MASK (0x7U << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_CLKIN_DIV_CON_SHIFT) /* 0x00000007 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_CLKIN_DIV_EN_SHIFT (3U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_CLKIN_DIV_EN_MASK (0x1U << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_CLKIN_DIV_EN_SHIFT) /* 0x00000008 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_IDLE_TH_SHIFT    (4U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_IDLE_TH_MASK     (0xFFFU << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_IDLE_TH_SHIFT)  /* 0x0000FFF0 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_WAIT_TH_SHIFT    (16U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_WAIT_TH_MASK     (0xFFFFU << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON0_WAIT_TH_SHIFT) /* 0xFFFF0000 */
/* AUTOCS_LSCLK_PERI_SRC_CON1 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_OFFSET           (0xD14U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_AUTOCS_CTRL_SHIFT (0U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_AUTOCS_CTRL_MASK (0xFFFU << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_AUTOCS_CTRL_SHIFT) /* 0x00000FFF */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_AUTOCS_EN_SHIFT  (12U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_AUTOCS_EN_MASK   (0x1U << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_AUTOCS_EN_SHIFT)  /* 0x00001000 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_SWITCH_EN_SHIFT  (13U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_SWITCH_EN_MASK   (0x1U << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_SWITCH_EN_SHIFT)  /* 0x00002000 */
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_CLKSEL_CFG_SHIFT (14U)
#define TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_CLKSEL_CFG_MASK  (0x3U << TOPCRU_AUTOCS_LSCLK_PERI_SRC_CON1_CLKSEL_CFG_SHIFT) /* 0x0000C000 */
/* AUTOCS_CON_ACLK_PERI_SRC */
#define TOPCRU_AUTOCS_CON_ACLK_PERI_SRC_OFFSET             (0xD18U)
#define TOPCRU_AUTOCS_CON_ACLK_PERI_SRC_AUTOCS_CON_ACLK_PERI_SRC_SHIFT (0U)
#define TOPCRU_AUTOCS_CON_ACLK_PERI_SRC_AUTOCS_CON_ACLK_PERI_SRC_MASK (0xFFFFU << TOPCRU_AUTOCS_CON_ACLK_PERI_SRC_AUTOCS_CON_ACLK_PERI_SRC_SHIFT) /* 0x0000FFFF */
/* AUTOCS_CON_HCLK_HPMCU_SRC */
#define TOPCRU_AUTOCS_CON_HCLK_HPMCU_SRC_OFFSET            (0xD1CU)
#define TOPCRU_AUTOCS_CON_HCLK_HPMCU_SRC_AUTOCS_CON_HCLK_HPMCU_SRC_SHIFT (0U)
#define TOPCRU_AUTOCS_CON_HCLK_HPMCU_SRC_AUTOCS_CON_HCLK_HPMCU_SRC_MASK (0xFFFFU << TOPCRU_AUTOCS_CON_HCLK_HPMCU_SRC_AUTOCS_CON_HCLK_HPMCU_SRC_SHIFT) /* 0x0000FFFF */
/* AUTOCS_CON_LSCLK_PERI_SRC */
#define TOPCRU_AUTOCS_CON_LSCLK_PERI_SRC_OFFSET            (0xD20U)
#define TOPCRU_AUTOCS_CON_LSCLK_PERI_SRC_AUTOCS_CON_LSCLK_PERI_SRC_SHIFT (0U)
#define TOPCRU_AUTOCS_CON_LSCLK_PERI_SRC_AUTOCS_CON_LSCLK_PERI_SRC_MASK (0xFFFFU << TOPCRU_AUTOCS_CON_LSCLK_PERI_SRC_AUTOCS_CON_LSCLK_PERI_SRC_SHIFT) /* 0x0000FFFF */
/* CRU_GLB_CNT_TH */
#define TOPCRU_CRU_GLB_CNT_TH_OFFSET                       (0xC00U)
#define TOPCRU_CRU_GLB_CNT_TH_GLOBAL_RESET_COUNTER_THRESHOLD_SHIFT (0U)
#define TOPCRU_CRU_GLB_CNT_TH_GLOBAL_RESET_COUNTER_THRESHOLD_MASK (0x3FFU << TOPCRU_CRU_GLB_CNT_TH_GLOBAL_RESET_COUNTER_THRESHOLD_SHIFT) /* 0x000003FF */
#define TOPCRU_CRU_GLB_CNT_TH_RESERVED0_SHIFT              (10U)
#define TOPCRU_CRU_GLB_CNT_TH_RESERVED0_MASK               (0x3FU << TOPCRU_CRU_GLB_CNT_TH_RESERVED0_SHIFT)             /* 0x0000FC00 */
#define TOPCRU_CRU_GLB_CNT_TH_RESERVED1_SHIFT              (16U)
#define TOPCRU_CRU_GLB_CNT_TH_RESERVED1_MASK               (0xFFFFU << TOPCRU_CRU_GLB_CNT_TH_RESERVED1_SHIFT)           /* 0xFFFF0000 */
/* CRU_GLBRST_ST */
#define TOPCRU_CRU_GLBRST_ST_OFFSET                        (0xC04U)
#define TOPCRU_CRU_GLBRST_ST_FST_GLB_RST_ST_SHIFT          (0U)
#define TOPCRU_CRU_GLBRST_ST_FST_GLB_RST_ST_MASK           (0x1U << TOPCRU_CRU_GLBRST_ST_FST_GLB_RST_ST_SHIFT)          /* 0x00000001 */
#define TOPCRU_CRU_GLBRST_ST_SND_GLB_RST_ST_SHIFT          (1U)
#define TOPCRU_CRU_GLBRST_ST_SND_GLB_RST_ST_MASK           (0x1U << TOPCRU_CRU_GLBRST_ST_SND_GLB_RST_ST_SHIFT)          /* 0x00000002 */
#define TOPCRU_CRU_GLBRST_ST_FST_GLB_TSADC_RST_ST_SHIFT    (2U)
#define TOPCRU_CRU_GLBRST_ST_FST_GLB_TSADC_RST_ST_MASK     (0x1U << TOPCRU_CRU_GLBRST_ST_FST_GLB_TSADC_RST_ST_SHIFT)    /* 0x00000004 */
#define TOPCRU_CRU_GLBRST_ST_SND_GLB_TSADC_RST_ST_SHIFT    (3U)
#define TOPCRU_CRU_GLBRST_ST_SND_GLB_TSADC_RST_ST_MASK     (0x1U << TOPCRU_CRU_GLBRST_ST_SND_GLB_TSADC_RST_ST_SHIFT)    /* 0x00000008 */
#define TOPCRU_CRU_GLBRST_ST_FST_GLB_WDT_RST_ST_SHIFT      (4U)
#define TOPCRU_CRU_GLBRST_ST_FST_GLB_WDT_RST_ST_MASK       (0x1U << TOPCRU_CRU_GLBRST_ST_FST_GLB_WDT_RST_ST_SHIFT)      /* 0x00000010 */
#define TOPCRU_CRU_GLBRST_ST_SND_GLB_WDT_RST_ST_SHIFT      (5U)
#define TOPCRU_CRU_GLBRST_ST_SND_GLB_WDT_RST_ST_MASK       (0x1U << TOPCRU_CRU_GLBRST_ST_SND_GLB_WDT_RST_ST_SHIFT)      /* 0x00000020 */
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT0_RST_ST_SHIFT         (6U)
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT0_RST_ST_MASK          (0x1U << TOPCRU_CRU_GLBRST_ST_GLB_WDT0_RST_ST_SHIFT)         /* 0x00000040 */
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT1_RST_ST_SHIFT         (7U)
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT1_RST_ST_MASK          (0x1U << TOPCRU_CRU_GLBRST_ST_GLB_WDT1_RST_ST_SHIFT)         /* 0x00000080 */
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT2_RST_ST_SHIFT         (8U)
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT2_RST_ST_MASK          (0x1U << TOPCRU_CRU_GLBRST_ST_GLB_WDT2_RST_ST_SHIFT)         /* 0x00000100 */
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT3_RST_ST_SHIFT         (9U)
#define TOPCRU_CRU_GLBRST_ST_GLB_WDT3_RST_ST_MASK          (0x1U << TOPCRU_CRU_GLBRST_ST_GLB_WDT3_RST_ST_SHIFT)         /* 0x00000200 */
#define TOPCRU_CRU_GLBRST_ST_WDT_S_SYS_RST_ST_SHIFT        (10U)
#define TOPCRU_CRU_GLBRST_ST_WDT_S_SYS_RST_ST_MASK         (0x1U << TOPCRU_CRU_GLBRST_ST_WDT_S_SYS_RST_ST_SHIFT)        /* 0x00000400 */
#define TOPCRU_CRU_GLBRST_ST_WDT_NS_SYS_RST_ST_SHIFT       (11U)
#define TOPCRU_CRU_GLBRST_ST_WDT_NS_SYS_RST_ST_MASK        (0x1U << TOPCRU_CRU_GLBRST_ST_WDT_NS_SYS_RST_ST_SHIFT)       /* 0x00000800 */
#define TOPCRU_CRU_GLBRST_ST_WDT_HPMCU_SYS_RST_ST_SHIFT    (12U)
#define TOPCRU_CRU_GLBRST_ST_WDT_HPMCU_SYS_RST_ST_MASK     (0x1U << TOPCRU_CRU_GLBRST_ST_WDT_HPMCU_SYS_RST_ST_SHIFT)    /* 0x00001000 */
#define TOPCRU_CRU_GLBRST_ST_WDT_3_SYS_RST_ST_SHIFT        (13U)
#define TOPCRU_CRU_GLBRST_ST_WDT_3_SYS_RST_ST_MASK         (0x1U << TOPCRU_CRU_GLBRST_ST_WDT_3_SYS_RST_ST_SHIFT)        /* 0x00002000 */
#define TOPCRU_CRU_GLBRST_ST_WDT_4_SYS_RST_ST_SHIFT        (14U)
#define TOPCRU_CRU_GLBRST_ST_WDT_4_SYS_RST_ST_MASK         (0x1U << TOPCRU_CRU_GLBRST_ST_WDT_4_SYS_RST_ST_SHIFT)        /* 0x00004000 */
#define TOPCRU_CRU_GLBRST_ST_WDT_5_SYS_RST_ST_SHIFT        (15U)
#define TOPCRU_CRU_GLBRST_ST_WDT_5_SYS_RST_ST_MASK         (0x1U << TOPCRU_CRU_GLBRST_ST_WDT_5_SYS_RST_ST_SHIFT)        /* 0x00008000 */
/* CRU_GLB_SRST_FST_VALUE */
#define TOPCRU_CRU_GLB_SRST_FST_VALUE_OFFSET               (0xC08U)
#define TOPCRU_CRU_GLB_SRST_FST_VALUE_GLB_SRST_FST_SHIFT   (0U)
#define TOPCRU_CRU_GLB_SRST_FST_VALUE_GLB_SRST_FST_MASK    (0xFFFFU << TOPCRU_CRU_GLB_SRST_FST_VALUE_GLB_SRST_FST_SHIFT) /* 0x0000FFFF */
/* CRU_GLB_SRST_SND_VALUE */
#define TOPCRU_CRU_GLB_SRST_SND_VALUE_OFFSET               (0xC0CU)
#define TOPCRU_CRU_GLB_SRST_SND_VALUE_GLB_SRST_SND_SHIFT   (0U)
#define TOPCRU_CRU_GLB_SRST_SND_VALUE_GLB_SRST_SND_MASK    (0xFFFFU << TOPCRU_CRU_GLB_SRST_SND_VALUE_GLB_SRST_SND_SHIFT) /* 0x0000FFFF */
/* CRU_GLB_RST_CON */
#define TOPCRU_CRU_GLB_RST_CON_OFFSET                      (0xC10U)
#define TOPCRU_CRU_GLB_RST_CON_TSADC_GLB_SRST_CTRL_SHIFT   (0U)
#define TOPCRU_CRU_GLB_RST_CON_TSADC_GLB_SRST_CTRL_MASK    (0x1U << TOPCRU_CRU_GLB_RST_CON_TSADC_GLB_SRST_CTRL_SHIFT)   /* 0x00000001 */
#define TOPCRU_CRU_GLB_RST_CON_TSADC_GLB_SRST_EN_SHIFT     (1U)
#define TOPCRU_CRU_GLB_RST_CON_TSADC_GLB_SRST_EN_MASK      (0x1U << TOPCRU_CRU_GLB_RST_CON_TSADC_GLB_SRST_EN_SHIFT)     /* 0x00000002 */
#define TOPCRU_CRU_GLB_RST_CON_PMU_SRST_GLB_CTRL_SHIFT     (2U)
#define TOPCRU_CRU_GLB_RST_CON_PMU_SRST_GLB_CTRL_MASK      (0x1U << TOPCRU_CRU_GLB_RST_CON_PMU_SRST_GLB_CTRL_SHIFT)     /* 0x00000004 */
#define TOPCRU_CRU_GLB_RST_CON_PMU_SRST_GLB_EN_SHIFT       (3U)
#define TOPCRU_CRU_GLB_RST_CON_PMU_SRST_GLB_EN_MASK        (0x1U << TOPCRU_CRU_GLB_RST_CON_PMU_SRST_GLB_EN_SHIFT)       /* 0x00000008 */
#define TOPCRU_CRU_GLB_RST_CON_PMU_SRST_WDT_EN_SHIFT       (4U)
#define TOPCRU_CRU_GLB_RST_CON_PMU_SRST_WDT_EN_MASK        (0x1U << TOPCRU_CRU_GLB_RST_CON_PMU_SRST_WDT_EN_SHIFT)       /* 0x00000010 */
#define TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_CTRL_SHIFT     (6U)
#define TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_CTRL_MASK      (0x1FU << TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_CTRL_SHIFT)    /* 0x000007C0 */
#define TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_EN_SHIFT       (11U)
#define TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_EN_MASK        (0x1FU << TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_EN_SHIFT)      /* 0x0000F800 */
/* CRU_GLB_RST_ST_NCLR */
#define TOPCRU_CRU_GLB_RST_ST_NCLR_OFFSET                  (0xC14U)
#define TOPCRU_CRU_GLB_RST_ST_NCLR                         (0x0U)
#define TOPCRU_CRU_GLB_RST_ST_NCLR_GLB_RST_ST_NCLR_SHIFT   (0U)
#define TOPCRU_CRU_GLB_RST_ST_NCLR_GLB_RST_ST_NCLR_MASK    (0xFFFFU << TOPCRU_CRU_GLB_RST_ST_NCLR_GLB_RST_ST_NCLR_SHIFT) /* 0x0000FFFF */
/* CLK_SAI_FRAC_DIV_HIGH */
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_OFFSET                (0xCC0U)
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_DENOMINATOR_HIGH_SHIFT (0U)
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_DENOMINATOR_HIGH_MASK (0xFFU << TOPCRU_CLK_SAI_FRAC_DIV_HIGH_DENOMINATOR_HIGH_SHIFT) /* 0x000000FF */
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_NOMINATOR_HIGH_SHIFT  (8U)
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_NOMINATOR_HIGH_MASK   (0xFFU << TOPCRU_CLK_SAI_FRAC_DIV_HIGH_NOMINATOR_HIGH_SHIFT) /* 0x0000FF00 */
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_DELTA_OVER_SEL_SHIFT  (16U)
#define TOPCRU_CLK_SAI_FRAC_DIV_HIGH_DELTA_OVER_SEL_MASK   (0x1U << TOPCRU_CLK_SAI_FRAC_DIV_HIGH_DELTA_OVER_SEL_SHIFT)  /* 0x00010000 */
/****************************************PMU0CRU*****************************************/
/* PMUCLKSEL_CON00 */
#define PMU0CRU_PMUCLKSEL_CON00_OFFSET                     (0x300U)
#define PMU0CRU_PMUCLKSEL_CON00_CLK_DEEPSLOW_SEL_SHIFT     (0U)
#define PMU0CRU_PMUCLKSEL_CON00_CLK_DEEPSLOW_SEL_MASK      (0x3U << PMU0CRU_PMUCLKSEL_CON00_CLK_DEEPSLOW_SEL_SHIFT)     /* 0x00000003 */
#define PMU0CRU_PMUCLKSEL_CON00_XIN_RC_DIV_SEL_SHIFT       (2U)
#define PMU0CRU_PMUCLKSEL_CON00_XIN_RC_DIV_SEL_MASK        (0x1U << PMU0CRU_PMUCLKSEL_CON00_XIN_RC_DIV_SEL_SHIFT)       /* 0x00000004 */
#define PMU0CRU_PMUCLKSEL_CON00_DBCLK_PMU_GPIO0_SEL_SHIFT  (3U)
#define PMU0CRU_PMUCLKSEL_CON00_DBCLK_PMU_GPIO0_SEL_MASK   (0x1U << PMU0CRU_PMUCLKSEL_CON00_DBCLK_PMU_GPIO0_SEL_SHIFT)  /* 0x00000008 */
#define PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_PMU_DIV_SHIFT  (4U)
#define PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_PMU_DIV_MASK   (0x7U << PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_PMU_DIV_SHIFT)  /* 0x00000070 */
#define PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_PMU_SEL_SHIFT  (8U)
#define PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_PMU_SEL_MASK   (0x7U << PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_PMU_SEL_SHIFT)  /* 0x00000700 */
#define PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_OUT_SEL_SHIFT  (13U)
#define PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_OUT_SEL_MASK   (0x7U << PMU0CRU_PMUCLKSEL_CON00_CLK_TESTOUT_OUT_SEL_SHIFT)  /* 0x0000E000 */
/* PMUCLKSEL_CON01 */
#define PMU0CRU_PMUCLKSEL_CON01_OFFSET                     (0x304U)
#define PMU0CRU_PMUCLKSEL_CON01_XIN_RC_DIV_DIV_SHIFT       (0U)
#define PMU0CRU_PMUCLKSEL_CON01_XIN_RC_DIV_DIV_MASK        (0xFFFFFFFFU << PMU0CRU_PMUCLKSEL_CON01_XIN_RC_DIV_DIV_SHIFT) /* 0xFFFFFFFF */
/* PMUCLKSEL_CON02 */
#define PMU0CRU_PMUCLKSEL_CON02_OFFSET                     (0x308U)
#define PMU0CRU_PMUCLKSEL_CON02_LSCLK_PMU_ROOT_DIV_SHIFT   (0U)
#define PMU0CRU_PMUCLKSEL_CON02_LSCLK_PMU_ROOT_DIV_MASK    (0x3U << PMU0CRU_PMUCLKSEL_CON02_LSCLK_PMU_ROOT_DIV_SHIFT)   /* 0x00000003 */
#define PMU0CRU_PMUCLKSEL_CON02_LSCLK_PMU_ROOT_SEL_SHIFT   (4U)
#define PMU0CRU_PMUCLKSEL_CON02_LSCLK_PMU_ROOT_SEL_MASK    (0x1U << PMU0CRU_PMUCLKSEL_CON02_LSCLK_PMU_ROOT_SEL_SHIFT)   /* 0x00000010 */
/* PMUGATE_CON00 */
#define PMU0CRU_PMUGATE_CON00_OFFSET                       (0x800U)
#define PMU0CRU_PMUGATE_CON00_LSCLK_PMU_ROOT_EN_SHIFT      (0U)
#define PMU0CRU_PMUGATE_CON00_LSCLK_PMU_ROOT_EN_MASK       (0x1U << PMU0CRU_PMUGATE_CON00_LSCLK_PMU_ROOT_EN_SHIFT)      /* 0x00000001 */
#define PMU0CRU_PMUGATE_CON00_CLK_PMU_EN_SHIFT             (1U)
#define PMU0CRU_PMUGATE_CON00_CLK_PMU_EN_MASK              (0x1U << PMU0CRU_PMUGATE_CON00_CLK_PMU_EN_SHIFT)             /* 0x00000002 */
#define PMU0CRU_PMUGATE_CON00_PCLK_PMU_EN_SHIFT            (2U)
#define PMU0CRU_PMUGATE_CON00_PCLK_PMU_EN_MASK             (0x1U << PMU0CRU_PMUGATE_CON00_PCLK_PMU_EN_SHIFT)            /* 0x00000004 */
#define PMU0CRU_PMUGATE_CON00_XIN_RC_DIV_EN_SHIFT          (3U)
#define PMU0CRU_PMUGATE_CON00_XIN_RC_DIV_EN_MASK           (0x1U << PMU0CRU_PMUGATE_CON00_XIN_RC_DIV_EN_SHIFT)          /* 0x00000008 */
#define PMU0CRU_PMUGATE_CON00_PCLK_PMU_GPIO0_EN_SHIFT      (4U)
#define PMU0CRU_PMUGATE_CON00_PCLK_PMU_GPIO0_EN_MASK       (0x1U << PMU0CRU_PMUGATE_CON00_PCLK_PMU_GPIO0_EN_SHIFT)      /* 0x00000010 */
#define PMU0CRU_PMUGATE_CON00_DBCLK_PMU_GPIO0_EN_SHIFT     (5U)
#define PMU0CRU_PMUGATE_CON00_DBCLK_PMU_GPIO0_EN_MASK      (0x1U << PMU0CRU_PMUGATE_CON00_DBCLK_PMU_GPIO0_EN_SHIFT)     /* 0x00000020 */
#define PMU0CRU_PMUGATE_CON00_CLK_DDR_FAIL_SAFE_EN_SHIFT   (8U)
#define PMU0CRU_PMUGATE_CON00_CLK_DDR_FAIL_SAFE_EN_MASK    (0x1U << PMU0CRU_PMUGATE_CON00_CLK_DDR_FAIL_SAFE_EN_SHIFT)   /* 0x00000100 */
#define PMU0CRU_PMUGATE_CON00_PCLK_PMU_HP_TIMER_EN_SHIFT   (9U)
#define PMU0CRU_PMUGATE_CON00_PCLK_PMU_HP_TIMER_EN_MASK    (0x1U << PMU0CRU_PMUGATE_CON00_PCLK_PMU_HP_TIMER_EN_SHIFT)   /* 0x00000200 */
#define PMU0CRU_PMUGATE_CON00_CLK_PMU_HP_TIMER_EN_SHIFT    (10U)
#define PMU0CRU_PMUGATE_CON00_CLK_PMU_HP_TIMER_EN_MASK     (0x1U << PMU0CRU_PMUGATE_CON00_CLK_PMU_HP_TIMER_EN_SHIFT)    /* 0x00000400 */
#define PMU0CRU_PMUGATE_CON00_CLK_PMU_32K_HP_TIMER_EN_SHIFT (11U)
#define PMU0CRU_PMUGATE_CON00_CLK_PMU_32K_HP_TIMER_EN_MASK (0x1U << PMU0CRU_PMUGATE_CON00_CLK_PMU_32K_HP_TIMER_EN_SHIFT) /* 0x00000800 */
#define PMU0CRU_PMUGATE_CON00_PCLK_I2C0_EN_SHIFT           (12U)
#define PMU0CRU_PMUGATE_CON00_PCLK_I2C0_EN_MASK            (0x1U << PMU0CRU_PMUGATE_CON00_PCLK_I2C0_EN_SHIFT)           /* 0x00001000 */
#define PMU0CRU_PMUGATE_CON00_CLK_I2C0_EN_SHIFT            (13U)
#define PMU0CRU_PMUGATE_CON00_CLK_I2C0_EN_MASK             (0x1U << PMU0CRU_PMUGATE_CON00_CLK_I2C0_EN_SHIFT)            /* 0x00002000 */
#define PMU0CRU_PMUGATE_CON00_PCLK_UART0_EN_SHIFT          (14U)
#define PMU0CRU_PMUGATE_CON00_PCLK_UART0_EN_MASK           (0x1U << PMU0CRU_PMUGATE_CON00_PCLK_UART0_EN_SHIFT)          /* 0x00004000 */
/* PMUGATE_CON01 */
#define PMU0CRU_PMUGATE_CON01_OFFSET                       (0x804U)
#define PMU0CRU_PMUGATE_CON01_PCLK_IOC_PMUIO0_EN_SHIFT     (0U)
#define PMU0CRU_PMUGATE_CON01_PCLK_IOC_PMUIO0_EN_MASK      (0x1U << PMU0CRU_PMUGATE_CON01_PCLK_IOC_PMUIO0_EN_SHIFT)     /* 0x00000001 */
#define PMU0CRU_PMUGATE_CON01_PCLK_CRU_PMU_EN_SHIFT        (1U)
#define PMU0CRU_PMUGATE_CON01_PCLK_CRU_PMU_EN_MASK         (0x1U << PMU0CRU_PMUGATE_CON01_PCLK_CRU_PMU_EN_SHIFT)        /* 0x00000002 */
#define PMU0CRU_PMUGATE_CON01_PCLK_PMU_GRF_EN_SHIFT        (2U)
#define PMU0CRU_PMUGATE_CON01_PCLK_PMU_GRF_EN_MASK         (0x1U << PMU0CRU_PMUGATE_CON01_PCLK_PMU_GRF_EN_SHIFT)        /* 0x00000004 */
#define PMU0CRU_PMUGATE_CON01_PCLK_PMU_SGRF_EN_SHIFT       (3U)
#define PMU0CRU_PMUGATE_CON01_PCLK_PMU_SGRF_EN_MASK        (0x1U << PMU0CRU_PMUGATE_CON01_PCLK_PMU_SGRF_EN_SHIFT)       /* 0x00000008 */
#define PMU0CRU_PMUGATE_CON01_CLK_REFOUT_EN_SHIFT          (4U)
#define PMU0CRU_PMUGATE_CON01_CLK_REFOUT_EN_MASK           (0x1U << PMU0CRU_PMUGATE_CON01_CLK_REFOUT_EN_SHIFT)          /* 0x00000010 */
#define PMU0CRU_PMUGATE_CON01_CLK_PREROLL_EN_SHIFT         (6U)
#define PMU0CRU_PMUGATE_CON01_CLK_PREROLL_EN_MASK          (0x1U << PMU0CRU_PMUGATE_CON01_CLK_PREROLL_EN_SHIFT)         /* 0x00000040 */
#define PMU0CRU_PMUGATE_CON01_CLK_PREROLL_32K_EN_SHIFT     (7U)
#define PMU0CRU_PMUGATE_CON01_CLK_PREROLL_32K_EN_MASK      (0x1U << PMU0CRU_PMUGATE_CON01_CLK_PREROLL_32K_EN_SHIFT)     /* 0x00000080 */
#define PMU0CRU_PMUGATE_CON01_HCLK_PMU_SRAM_EN_SHIFT       (8U)
#define PMU0CRU_PMUGATE_CON01_HCLK_PMU_SRAM_EN_MASK        (0x1U << PMU0CRU_PMUGATE_CON01_HCLK_PMU_SRAM_EN_SHIFT)       /* 0x00000100 */
#define PMU0CRU_PMUGATE_CON01_LSCLK_PMU_32K_EN_SHIFT       (12U)
#define PMU0CRU_PMUGATE_CON01_LSCLK_PMU_32K_EN_MASK        (0x1U << PMU0CRU_PMUGATE_CON01_LSCLK_PMU_32K_EN_SHIFT)       /* 0x00001000 */
#define PMU0CRU_PMUGATE_CON01_CLK_TESTOUT_PMU_EN_SHIFT     (14U)
#define PMU0CRU_PMUGATE_CON01_CLK_TESTOUT_PMU_EN_MASK      (0x1U << PMU0CRU_PMUGATE_CON01_CLK_TESTOUT_PMU_EN_SHIFT)     /* 0x00004000 */
#define PMU0CRU_PMUGATE_CON01_CLK_TESTOUT_OUT_EN_SHIFT     (15U)
#define PMU0CRU_PMUGATE_CON01_CLK_TESTOUT_OUT_EN_MASK      (0x1U << PMU0CRU_PMUGATE_CON01_CLK_TESTOUT_OUT_EN_SHIFT)     /* 0x00008000 */
/* PMUGATE_CON02 */
#define PMU0CRU_PMUGATE_CON02_OFFSET                       (0x808U)
#define PMU0CRU_PMUGATE_CON02_PCLK_PWM0_EN_SHIFT           (0U)
#define PMU0CRU_PMUGATE_CON02_PCLK_PWM0_EN_MASK            (0x1U << PMU0CRU_PMUGATE_CON02_PCLK_PWM0_EN_SHIFT)           /* 0x00000001 */
#define PMU0CRU_PMUGATE_CON02_CLK_PWM0_EN_SHIFT            (1U)
#define PMU0CRU_PMUGATE_CON02_CLK_PWM0_EN_MASK             (0x1U << PMU0CRU_PMUGATE_CON02_CLK_PWM0_EN_SHIFT)            /* 0x00000002 */
#define PMU0CRU_PMUGATE_CON02_CLK_OSC_PWM0_EN_SHIFT        (2U)
#define PMU0CRU_PMUGATE_CON02_CLK_OSC_PWM0_EN_MASK         (0x1U << PMU0CRU_PMUGATE_CON02_CLK_OSC_PWM0_EN_SHIFT)        /* 0x00000004 */
#define PMU0CRU_PMUGATE_CON02_CLK_RC_PWM0_EN_SHIFT         (3U)
#define PMU0CRU_PMUGATE_CON02_CLK_RC_PWM0_EN_MASK          (0x1U << PMU0CRU_PMUGATE_CON02_CLK_RC_PWM0_EN_SHIFT)         /* 0x00000008 */
#define PMU0CRU_PMUGATE_CON02_CLK_FREQ_PWM0_EN_SHIFT       (4U)
#define PMU0CRU_PMUGATE_CON02_CLK_FREQ_PWM0_EN_MASK        (0x1U << PMU0CRU_PMUGATE_CON02_CLK_FREQ_PWM0_EN_SHIFT)       /* 0x00000010 */
#define PMU0CRU_PMUGATE_CON02_CLK_COUNTER_PWM0_EN_SHIFT    (5U)
#define PMU0CRU_PMUGATE_CON02_CLK_COUNTER_PWM0_EN_MASK     (0x1U << PMU0CRU_PMUGATE_CON02_CLK_COUNTER_PWM0_EN_SHIFT)    /* 0x00000020 */
#define PMU0CRU_PMUGATE_CON02_XIN_OSC0_DFT_EN_SHIFT        (6U)
#define PMU0CRU_PMUGATE_CON02_XIN_OSC0_DFT_EN_MASK         (0x1U << PMU0CRU_PMUGATE_CON02_XIN_OSC0_DFT_EN_SHIFT)        /* 0x00000040 */
#define PMU0CRU_PMUGATE_CON02_CLK_RC_400K_DFT_EN_SHIFT     (7U)
#define PMU0CRU_PMUGATE_CON02_CLK_RC_400K_DFT_EN_MASK      (0x1U << PMU0CRU_PMUGATE_CON02_CLK_RC_400K_DFT_EN_SHIFT)     /* 0x00000080 */
#define PMU0CRU_PMUGATE_CON02_CLK_LPMCU_PMU_EN_SHIFT       (12U)
#define PMU0CRU_PMUGATE_CON02_CLK_LPMCU_PMU_EN_MASK        (0x1U << PMU0CRU_PMUGATE_CON02_CLK_LPMCU_PMU_EN_SHIFT)       /* 0x00001000 */
/* PMUSOFTRST_CON00 */
#define PMU0CRU_PMUSOFTRST_CON00_OFFSET                    (0xA00U)
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_PMU_GPIO0_SHIFT   (4U)
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_PMU_GPIO0_MASK    (0x1U << PMU0CRU_PMUSOFTRST_CON00_PRESETN_PMU_GPIO0_SHIFT)   /* 0x00000010 */
#define PMU0CRU_PMUSOFTRST_CON00_DBRESETN_PMU_GPIO0_SHIFT  (5U)
#define PMU0CRU_PMUSOFTRST_CON00_DBRESETN_PMU_GPIO0_MASK   (0x1U << PMU0CRU_PMUSOFTRST_CON00_DBRESETN_PMU_GPIO0_SHIFT)  /* 0x00000020 */
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_DDR_FAIL_SAFE_SHIFT (8U)
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_DDR_FAIL_SAFE_MASK (0x1U << PMU0CRU_PMUSOFTRST_CON00_RESETN_DDR_FAIL_SAFE_SHIFT) /* 0x00000100 */
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_PMU_HP_TIMER_SHIFT (9U)
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_PMU_HP_TIMER_MASK (0x1U << PMU0CRU_PMUSOFTRST_CON00_PRESETN_PMU_HP_TIMER_SHIFT) /* 0x00000200 */
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_PMU_HP_TIMER_SHIFT (10U)
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_PMU_HP_TIMER_MASK  (0x1U << PMU0CRU_PMUSOFTRST_CON00_RESETN_PMU_HP_TIMER_SHIFT) /* 0x00000400 */
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_PMU_32K_HP_TIMER_SHIFT (11U)
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_PMU_32K_HP_TIMER_MASK (0x1U << PMU0CRU_PMUSOFTRST_CON00_RESETN_PMU_32K_HP_TIMER_SHIFT) /* 0x00000800 */
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_I2C0_SHIFT        (12U)
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_I2C0_MASK         (0x1U << PMU0CRU_PMUSOFTRST_CON00_PRESETN_I2C0_SHIFT)        /* 0x00001000 */
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_I2C0_SHIFT         (13U)
#define PMU0CRU_PMUSOFTRST_CON00_RESETN_I2C0_MASK          (0x1U << PMU0CRU_PMUSOFTRST_CON00_RESETN_I2C0_SHIFT)         /* 0x00002000 */
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_UART0_SHIFT       (14U)
#define PMU0CRU_PMUSOFTRST_CON00_PRESETN_UART0_MASK        (0x1U << PMU0CRU_PMUSOFTRST_CON00_PRESETN_UART0_SHIFT)       /* 0x00004000 */
#define PMU0CRU_PMUSOFTRST_CON00_SRESETN_UART0_SHIFT       (15U)
#define PMU0CRU_PMUSOFTRST_CON00_SRESETN_UART0_MASK        (0x1U << PMU0CRU_PMUSOFTRST_CON00_SRESETN_UART0_SHIFT)       /* 0x00008000 */
/* PMUSOFTRST_CON01 */
#define PMU0CRU_PMUSOFTRST_CON01_OFFSET                    (0xA04U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_IOC_PMUIO0_SHIFT  (0U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_IOC_PMUIO0_MASK   (0x1U << PMU0CRU_PMUSOFTRST_CON01_PRESETN_IOC_PMUIO0_SHIFT)  /* 0x00000001 */
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_CRU_PMU_SHIFT     (1U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_CRU_PMU_MASK      (0x1U << PMU0CRU_PMUSOFTRST_CON01_PRESETN_CRU_PMU_SHIFT)     /* 0x00000002 */
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_GRF_SHIFT     (2U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_GRF_MASK      (0x1U << PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_GRF_SHIFT)     /* 0x00000004 */
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_SGRF_SHIFT    (3U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_SGRF_MASK     (0x1U << PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_SGRF_SHIFT)    /* 0x00000008 */
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_SGRF_REMAP_SHIFT (4U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_SGRF_REMAP_MASK (0x1U << PMU0CRU_PMUSOFTRST_CON01_PRESETN_PMU_SGRF_REMAP_SHIFT) /* 0x00000010 */
#define PMU0CRU_PMUSOFTRST_CON01_RESETN_PREROLL_SHIFT      (6U)
#define PMU0CRU_PMUSOFTRST_CON01_RESETN_PREROLL_MASK       (0x1U << PMU0CRU_PMUSOFTRST_CON01_RESETN_PREROLL_SHIFT)      /* 0x00000040 */
#define PMU0CRU_PMUSOFTRST_CON01_RESETN_PREROLL_32K_SHIFT  (7U)
#define PMU0CRU_PMUSOFTRST_CON01_RESETN_PREROLL_32K_MASK   (0x1U << PMU0CRU_PMUSOFTRST_CON01_RESETN_PREROLL_32K_SHIFT)  /* 0x00000080 */
#define PMU0CRU_PMUSOFTRST_CON01_HRESETN_PMU_SRAM_SHIFT    (8U)
#define PMU0CRU_PMUSOFTRST_CON01_HRESETN_PMU_SRAM_MASK     (0x1U << PMU0CRU_PMUSOFTRST_CON01_HRESETN_PMU_SRAM_SHIFT)    /* 0x00000100 */
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PWM0_SHIFT        (9U)
#define PMU0CRU_PMUSOFTRST_CON01_PRESETN_PWM0_MASK         (0x1U << PMU0CRU_PMUSOFTRST_CON01_PRESETN_PWM0_SHIFT)        /* 0x00000200 */
#define PMU0CRU_PMUSOFTRST_CON01_RESETN_PWM0_SHIFT         (10U)
#define PMU0CRU_PMUSOFTRST_CON01_RESETN_PWM0_MASK          (0x1U << PMU0CRU_PMUSOFTRST_CON01_RESETN_PWM0_SHIFT)         /* 0x00000400 */
/* PMUSOFTRST_CON02 */
#define PMU0CRU_PMUSOFTRST_CON02_OFFSET                    (0xA08U)
#define PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_SHIFT        (0U)
#define PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_MASK         (0x1U << PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_SHIFT)        /* 0x00000001 */
#define PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_PWRUP_SHIFT  (1U)
#define PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_PWRUP_MASK   (0x1U << PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_PWRUP_SHIFT)  /* 0x00000002 */
#define PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_CPU_SHIFT    (2U)
#define PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_CPU_MASK     (0x1U << PMU0CRU_PMUSOFTRST_CON02_RESETN_LPMCU_CPU_SHIFT)    /* 0x00000004 */
#define PMU0CRU_PMUSOFTRST_CON02_TRESETN_LPMCU_CPU_SHIFT   (3U)
#define PMU0CRU_PMUSOFTRST_CON02_TRESETN_LPMCU_CPU_MASK    (0x1U << PMU0CRU_PMUSOFTRST_CON02_TRESETN_LPMCU_CPU_SHIFT)   /* 0x00000008 */
/* AUTOCS_LSCLK_PMU_ROOT_CON0 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_OFFSET          (0xD00U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_CLKIN_DIV_CON_SHIFT (0U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_CLKIN_DIV_CON_MASK (0x7U << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_CLKIN_DIV_CON_SHIFT) /* 0x00000007 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_CLKIN_DIV_EN_SHIFT (3U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_CLKIN_DIV_EN_MASK (0x1U << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_CLKIN_DIV_EN_SHIFT) /* 0x00000008 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_IDLE_TH_SHIFT   (4U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_IDLE_TH_MASK    (0xFFFU << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_IDLE_TH_SHIFT) /* 0x0000FFF0 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_WAIT_TH_SHIFT   (16U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_WAIT_TH_MASK    (0xFFFFU << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON0_WAIT_TH_SHIFT) /* 0xFFFF0000 */
/* AUTOCS_LSCLK_PMU_ROOT_CON1 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_OFFSET          (0xD04U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_AUTOCS_CTRL_SHIFT (0U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_AUTOCS_CTRL_MASK (0xFFFU << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_AUTOCS_CTRL_SHIFT) /* 0x00000FFF */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_AUTOCS_EN_SHIFT (12U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_AUTOCS_EN_MASK  (0x1U << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_AUTOCS_EN_SHIFT) /* 0x00001000 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_SWITCH_EN_SHIFT (13U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_SWITCH_EN_MASK  (0x1U << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_SWITCH_EN_SHIFT) /* 0x00002000 */
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_CLKSEL_CFG_SHIFT (14U)
#define PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_CLKSEL_CFG_MASK (0x3U << PMU0CRU_AUTOCS_LSCLK_PMU_ROOT_CON1_CLKSEL_CFG_SHIFT) /* 0x0000C000 */
/****************************************PMU1CRU*****************************************/
/* PMU1CLKSEL_CON00 */
#define PMU1CRU_PMU1CLKSEL_CON00_OFFSET                    (0x300U)
#define PMU1CRU_PMU1CLKSEL_CON00_SCLK_SFC_2X_PMU1_SEL_SHIFT (8U)
#define PMU1CRU_PMU1CLKSEL_CON00_SCLK_SFC_2X_PMU1_SEL_MASK (0x1U << PMU1CRU_PMU1CLKSEL_CON00_SCLK_SFC_2X_PMU1_SEL_SHIFT) /* 0x00000100 */
/* PMU1GATE_CON00 */
#define PMU1CRU_PMU1GATE_CON00_OFFSET                      (0x800U)
#define PMU1CRU_PMU1GATE_CON00_PCLK_SPI2AHB_EN_SHIFT       (0U)
#define PMU1CRU_PMU1GATE_CON00_PCLK_SPI2AHB_EN_MASK        (0x1U << PMU1CRU_PMU1GATE_CON00_PCLK_SPI2AHB_EN_SHIFT)       /* 0x00000001 */
#define PMU1CRU_PMU1GATE_CON00_HCLK_SPI2AHB_EN_SHIFT       (1U)
#define PMU1CRU_PMU1GATE_CON00_HCLK_SPI2AHB_EN_MASK        (0x1U << PMU1CRU_PMU1GATE_CON00_HCLK_SPI2AHB_EN_SHIFT)       /* 0x00000002 */
#define PMU1CRU_PMU1GATE_CON00_SCLK_SPI2AHB_EN_SHIFT       (2U)
#define PMU1CRU_PMU1GATE_CON00_SCLK_SPI2AHB_EN_MASK        (0x1U << PMU1CRU_PMU1GATE_CON00_SCLK_SPI2AHB_EN_SHIFT)       /* 0x00000004 */
#define PMU1CRU_PMU1GATE_CON00_LSCLK_PMU_BIU_EN_SHIFT      (3U)
#define PMU1CRU_PMU1GATE_CON00_LSCLK_PMU_BIU_EN_MASK       (0x1U << PMU1CRU_PMU1GATE_CON00_LSCLK_PMU_BIU_EN_SHIFT)      /* 0x00000008 */
#define PMU1CRU_PMU1GATE_CON00_PCLK_WDT_LPMCU_EN_SHIFT     (9U)
#define PMU1CRU_PMU1GATE_CON00_PCLK_WDT_LPMCU_EN_MASK      (0x1U << PMU1CRU_PMU1GATE_CON00_PCLK_WDT_LPMCU_EN_SHIFT)     /* 0x00000200 */
#define PMU1CRU_PMU1GATE_CON00_TCLK_WDT_LPMCU_EN_SHIFT     (10U)
#define PMU1CRU_PMU1GATE_CON00_TCLK_WDT_LPMCU_EN_MASK      (0x1U << PMU1CRU_PMU1GATE_CON00_TCLK_WDT_LPMCU_EN_SHIFT)     /* 0x00000400 */
#define PMU1CRU_PMU1GATE_CON00_HCLK_SFC_PMU1_EN_SHIFT      (12U)
#define PMU1CRU_PMU1GATE_CON00_HCLK_SFC_PMU1_EN_MASK       (0x1U << PMU1CRU_PMU1GATE_CON00_HCLK_SFC_PMU1_EN_SHIFT)      /* 0x00001000 */
#define PMU1CRU_PMU1GATE_CON00_HCLK_SFC_XIP_PMU1_EN_SHIFT  (13U)
#define PMU1CRU_PMU1GATE_CON00_HCLK_SFC_XIP_PMU1_EN_MASK   (0x1U << PMU1CRU_PMU1GATE_CON00_HCLK_SFC_XIP_PMU1_EN_SHIFT)  /* 0x00002000 */
#define PMU1CRU_PMU1GATE_CON00_SCLK_SFC_2X_PMU1_EN_SHIFT   (14U)
#define PMU1CRU_PMU1GATE_CON00_SCLK_SFC_2X_PMU1_EN_MASK    (0x1U << PMU1CRU_PMU1GATE_CON00_SCLK_SFC_2X_PMU1_EN_SHIFT)   /* 0x00004000 */
/* PMU1GATE_CON01 */
#define PMU1CRU_PMU1GATE_CON01_OFFSET                      (0x804U)
#define PMU1CRU_PMU1GATE_CON01_CLK_LPMCU_EN_SHIFT          (0U)
#define PMU1CRU_PMU1GATE_CON01_CLK_LPMCU_EN_MASK           (0x1U << PMU1CRU_PMU1GATE_CON01_CLK_LPMCU_EN_SHIFT)          /* 0x00000001 */
#define PMU1CRU_PMU1GATE_CON01_CLK_LPMCU_RTC_EN_SHIFT      (4U)
#define PMU1CRU_PMU1GATE_CON01_CLK_LPMCU_RTC_EN_MASK       (0x1U << PMU1CRU_PMU1GATE_CON01_CLK_LPMCU_RTC_EN_SHIFT)      /* 0x00000010 */
#define PMU1CRU_PMU1GATE_CON01_PCLK_LPMCU_MAILBOX_EN_SHIFT (8U)
#define PMU1CRU_PMU1GATE_CON01_PCLK_LPMCU_MAILBOX_EN_MASK  (0x1U << PMU1CRU_PMU1GATE_CON01_PCLK_LPMCU_MAILBOX_EN_SHIFT) /* 0x00000100 */
#define PMU1CRU_PMU1GATE_CON01_PCLK_IOC_PMUIO1_EN_SHIFT    (9U)
#define PMU1CRU_PMU1GATE_CON01_PCLK_IOC_PMUIO1_EN_MASK     (0x1U << PMU1CRU_PMU1GATE_CON01_PCLK_IOC_PMUIO1_EN_SHIFT)    /* 0x00000200 */
#define PMU1CRU_PMU1GATE_CON01_PCLK_CRU_PMU1_EN_SHIFT      (10U)
#define PMU1CRU_PMU1GATE_CON01_PCLK_CRU_PMU1_EN_MASK       (0x1U << PMU1CRU_PMU1GATE_CON01_PCLK_CRU_PMU1_EN_SHIFT)      /* 0x00000400 */
#define PMU1CRU_PMU1GATE_CON01_XIN_OSC0_DFT_EN_SHIFT       (11U)
#define PMU1CRU_PMU1GATE_CON01_XIN_OSC0_DFT_EN_MASK        (0x1U << PMU1CRU_PMU1GATE_CON01_XIN_OSC0_DFT_EN_SHIFT)       /* 0x00000800 */
/* PMU1SOFTRST_CON00 */
#define PMU1CRU_PMU1SOFTRST_CON00_OFFSET                   (0xA00U)
#define PMU1CRU_PMU1SOFTRST_CON00_PRESETN_SPI2AHB_SHIFT    (0U)
#define PMU1CRU_PMU1SOFTRST_CON00_PRESETN_SPI2AHB_MASK     (0x1U << PMU1CRU_PMU1SOFTRST_CON00_PRESETN_SPI2AHB_SHIFT)    /* 0x00000001 */
#define PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SPI2AHB_SHIFT    (1U)
#define PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SPI2AHB_MASK     (0x1U << PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SPI2AHB_SHIFT)    /* 0x00000002 */
#define PMU1CRU_PMU1SOFTRST_CON00_SRESETN_SPI2AHB_SHIFT    (2U)
#define PMU1CRU_PMU1SOFTRST_CON00_SRESETN_SPI2AHB_MASK     (0x1U << PMU1CRU_PMU1SOFTRST_CON00_SRESETN_SPI2AHB_SHIFT)    /* 0x00000004 */
#define PMU1CRU_PMU1SOFTRST_CON00_LSRESETN_PMU_BIU_SHIFT   (3U)
#define PMU1CRU_PMU1SOFTRST_CON00_LSRESETN_PMU_BIU_MASK    (0x1U << PMU1CRU_PMU1SOFTRST_CON00_LSRESETN_PMU_BIU_SHIFT)   /* 0x00000008 */
#define PMU1CRU_PMU1SOFTRST_CON00_PRESETN_WDT_LPMCU_SHIFT  (9U)
#define PMU1CRU_PMU1SOFTRST_CON00_PRESETN_WDT_LPMCU_MASK   (0x1U << PMU1CRU_PMU1SOFTRST_CON00_PRESETN_WDT_LPMCU_SHIFT)  /* 0x00000200 */
#define PMU1CRU_PMU1SOFTRST_CON00_TRESETN_WDT_LPMCU_SHIFT  (10U)
#define PMU1CRU_PMU1SOFTRST_CON00_TRESETN_WDT_LPMCU_MASK   (0x1U << PMU1CRU_PMU1SOFTRST_CON00_TRESETN_WDT_LPMCU_SHIFT)  /* 0x00000400 */
#define PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SFC_PMU1_SHIFT   (12U)
#define PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SFC_PMU1_MASK    (0x1U << PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SFC_PMU1_SHIFT)   /* 0x00001000 */
#define PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SFC_XIP_PMU1_SHIFT (13U)
#define PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SFC_XIP_PMU1_MASK (0x1U << PMU1CRU_PMU1SOFTRST_CON00_HRESETN_SFC_XIP_PMU1_SHIFT) /* 0x00002000 */
#define PMU1CRU_PMU1SOFTRST_CON00_SRESETN_SFC_2X_PMU1_SHIFT (14U)
#define PMU1CRU_PMU1SOFTRST_CON00_SRESETN_SFC_2X_PMU1_MASK (0x1U << PMU1CRU_PMU1SOFTRST_CON00_SRESETN_SFC_2X_PMU1_SHIFT) /* 0x00004000 */
/* PMU1SOFTRST_CON01 */
#define PMU1CRU_PMU1SOFTRST_CON01_OFFSET                   (0xA04U)
#define PMU1CRU_PMU1SOFTRST_CON01_PRESETN_LPMCU_MAILBOX_SHIFT (8U)
#define PMU1CRU_PMU1SOFTRST_CON01_PRESETN_LPMCU_MAILBOX_MASK (0x1U << PMU1CRU_PMU1SOFTRST_CON01_PRESETN_LPMCU_MAILBOX_SHIFT) /* 0x00000100 */
#define PMU1CRU_PMU1SOFTRST_CON01_PRESETN_IOC_PMUIO1_SHIFT (9U)
#define PMU1CRU_PMU1SOFTRST_CON01_PRESETN_IOC_PMUIO1_MASK  (0x1U << PMU1CRU_PMU1SOFTRST_CON01_PRESETN_IOC_PMUIO1_SHIFT) /* 0x00000200 */
#define PMU1CRU_PMU1SOFTRST_CON01_PRESETN_CRU_PMU1_SHIFT   (10U)
#define PMU1CRU_PMU1SOFTRST_CON01_PRESETN_CRU_PMU1_MASK    (0x1U << PMU1CRU_PMU1SOFTRST_CON01_PRESETN_CRU_PMU1_SHIFT)   /* 0x00000400 */
/* PMU1CRU_IP_CON */
#define PMU1CRU_PMU1CRU_IP_CON_OFFSET                      (0xC08U)
#define PMU1CRU_PMU1CRU_IP_CON_CLK_LPMCU_JTAG_EN_SHIFT     (0U)
#define PMU1CRU_PMU1CRU_IP_CON_CLK_LPMCU_JTAG_EN_MASK      (0x1U << PMU1CRU_PMU1CRU_IP_CON_CLK_LPMCU_JTAG_EN_SHIFT)     /* 0x00000001 */
/****************************************GRF_VEPU****************************************/
/* MEM_CON_SPRA */
#define GRF_VEPU_MEM_CON_SPRA_OFFSET                       (0x0U)
#define GRF_VEPU_MEM_CON_SPRA_RTSEL_SVT_SHIFT              (0U)
#define GRF_VEPU_MEM_CON_SPRA_RTSEL_SVT_MASK               (0x3U << GRF_VEPU_MEM_CON_SPRA_RTSEL_SVT_SHIFT)              /* 0x00000003 */
#define GRF_VEPU_MEM_CON_SPRA_WTSEL_SVT_SHIFT              (2U)
#define GRF_VEPU_MEM_CON_SPRA_WTSEL_SVT_MASK               (0x3U << GRF_VEPU_MEM_CON_SPRA_WTSEL_SVT_SHIFT)              /* 0x0000000C */
#define GRF_VEPU_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT   (4U)
#define GRF_VEPU_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_MASK    (0x3U << GRF_VEPU_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT)   /* 0x00000030 */
#define GRF_VEPU_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT   (6U)
#define GRF_VEPU_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_MASK    (0x3U << GRF_VEPU_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT)   /* 0x000000C0 */
#define GRF_VEPU_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT         (8U)
#define GRF_VEPU_MEM_CON_SPRA_RTSEL_HVT_MUX4_MASK          (0x3U << GRF_VEPU_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT)         /* 0x00000300 */
#define GRF_VEPU_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT         (10U)
#define GRF_VEPU_MEM_CON_SPRA_WTSEL_HVT_MUX4_MASK          (0x3U << GRF_VEPU_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT)         /* 0x00000C00 */
/* MEM_CON_DPRA */
#define GRF_VEPU_MEM_CON_DPRA_OFFSET                       (0x4U)
#define GRF_VEPU_MEM_CON_DPRA_PTSEL_SHIFT                  (0U)
#define GRF_VEPU_MEM_CON_DPRA_PTSEL_MASK                   (0x3U << GRF_VEPU_MEM_CON_DPRA_PTSEL_SHIFT)                  /* 0x00000003 */
#define GRF_VEPU_MEM_CON_DPRA_WTSEL_SHIFT                  (2U)
#define GRF_VEPU_MEM_CON_DPRA_WTSEL_MASK                   (0x3U << GRF_VEPU_MEM_CON_DPRA_WTSEL_SHIFT)                  /* 0x0000000C */
#define GRF_VEPU_MEM_CON_DPRA_RTSEL_SHIFT                  (4U)
#define GRF_VEPU_MEM_CON_DPRA_RTSEL_MASK                   (0x3U << GRF_VEPU_MEM_CON_DPRA_RTSEL_SHIFT)                  /* 0x00000030 */
/****************************************GRF_NPU*****************************************/
/* MEM_CON_SPRA */
#define GRF_NPU_MEM_CON_SPRA_OFFSET                        (0x0U)
#define GRF_NPU_MEM_CON_SPRA_RTSEL_SVT_SHIFT               (0U)
#define GRF_NPU_MEM_CON_SPRA_RTSEL_SVT_MASK                (0x3U << GRF_NPU_MEM_CON_SPRA_RTSEL_SVT_SHIFT)               /* 0x00000003 */
#define GRF_NPU_MEM_CON_SPRA_WTSEL_SVT_SHIFT               (2U)
#define GRF_NPU_MEM_CON_SPRA_WTSEL_SVT_MASK                (0x3U << GRF_NPU_MEM_CON_SPRA_WTSEL_SVT_SHIFT)               /* 0x0000000C */
#define GRF_NPU_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT    (4U)
#define GRF_NPU_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_MASK     (0x3U << GRF_NPU_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT)    /* 0x00000030 */
#define GRF_NPU_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT    (6U)
#define GRF_NPU_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_MASK     (0x3U << GRF_NPU_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT)    /* 0x000000C0 */
#define GRF_NPU_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT          (8U)
#define GRF_NPU_MEM_CON_SPRA_RTSEL_HVT_MUX4_MASK           (0x3U << GRF_NPU_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT)          /* 0x00000300 */
#define GRF_NPU_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT          (10U)
#define GRF_NPU_MEM_CON_SPRA_WTSEL_HVT_MUX4_MASK           (0x3U << GRF_NPU_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT)          /* 0x00000C00 */
/*****************************************GRF_VI*****************************************/
/* MEM_CON_SPRA */
#define GRF_VI_MEM_CON_SPRA_OFFSET                         (0x0U)
#define GRF_VI_MEM_CON_SPRA_RTSEL_SVT_SHIFT                (0U)
#define GRF_VI_MEM_CON_SPRA_RTSEL_SVT_MASK                 (0x3U << GRF_VI_MEM_CON_SPRA_RTSEL_SVT_SHIFT)                /* 0x00000003 */
#define GRF_VI_MEM_CON_SPRA_WTSEL_SVT_SHIFT                (2U)
#define GRF_VI_MEM_CON_SPRA_WTSEL_SVT_MASK                 (0x3U << GRF_VI_MEM_CON_SPRA_WTSEL_SVT_SHIFT)                /* 0x0000000C */
#define GRF_VI_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT     (4U)
#define GRF_VI_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_MASK      (0x3U << GRF_VI_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT)     /* 0x00000030 */
#define GRF_VI_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT     (6U)
#define GRF_VI_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_MASK      (0x3U << GRF_VI_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT)     /* 0x000000C0 */
#define GRF_VI_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT           (8U)
#define GRF_VI_MEM_CON_SPRA_RTSEL_HVT_MUX4_MASK            (0x3U << GRF_VI_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT)           /* 0x00000300 */
#define GRF_VI_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT           (10U)
#define GRF_VI_MEM_CON_SPRA_WTSEL_HVT_MUX4_MASK            (0x3U << GRF_VI_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT)           /* 0x00000C00 */
/* MEM_CON_DPRA */
#define GRF_VI_MEM_CON_DPRA_OFFSET                         (0x4U)
#define GRF_VI_MEM_CON_DPRA_PTSEL_SHIFT                    (0U)
#define GRF_VI_MEM_CON_DPRA_PTSEL_MASK                     (0x3U << GRF_VI_MEM_CON_DPRA_PTSEL_SHIFT)                    /* 0x00000003 */
#define GRF_VI_MEM_CON_DPRA_WTSEL_SHIFT                    (2U)
#define GRF_VI_MEM_CON_DPRA_WTSEL_MASK                     (0x3U << GRF_VI_MEM_CON_DPRA_WTSEL_SHIFT)                    /* 0x0000000C */
#define GRF_VI_MEM_CON_DPRA_RTSEL_SHIFT                    (4U)
#define GRF_VI_MEM_CON_DPRA_RTSEL_MASK                     (0x3U << GRF_VI_MEM_CON_DPRA_RTSEL_SHIFT)                    /* 0x00000030 */
/* VI_HPROT2_CON */
#define GRF_VI_VI_HPROT2_CON_OFFSET                        (0xCU)
#define GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_EMMC_SHIFT (0U)
#define GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_EMMC_MASK (0x1U << GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_EMMC_SHIFT) /* 0x00000001 */
#define GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_SDCARD_SHIFT (1U)
#define GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_SDCARD_MASK (0x1U << GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_SDCARD_SHIFT) /* 0x00000002 */
#define GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_FSPI_SHIFT (2U)
#define GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_FSPI_MASK (0x1U << GRF_VI_VI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_FSPI_SHIFT) /* 0x00000004 */
/* STATUS */
#define GRF_VI_STATUS_OFFSET                               (0x10U)
#define GRF_VI_STATUS_ISP_PRELIGHT_TRIG_SHIFT              (1U)
#define GRF_VI_STATUS_ISP_PRELIGHT_TRIG_MASK               (0x1U << GRF_VI_STATUS_ISP_PRELIGHT_TRIG_SHIFT)              /* 0x00000002 */
#define GRF_VI_STATUS_ISP_FLASH_TRIG_SHIFT                 (2U)
#define GRF_VI_STATUS_ISP_FLASH_TRIG_MASK                  (0x1U << GRF_VI_STATUS_ISP_FLASH_TRIG_SHIFT)                 /* 0x00000004 */
#define GRF_VI_STATUS_FSPI_IDLE_SHIFT                      (6U)
#define GRF_VI_STATUS_FSPI_IDLE_MASK                       (0x1U << GRF_VI_STATUS_FSPI_IDLE_SHIFT)                      /* 0x00000040 */
/* CSIPHY_CON */
#define GRF_VI_CSIPHY_CON_OFFSET                           (0x14U)
#define GRF_VI_CSIPHY_CON_FORCE_RX_MODE_SHIFT              (0U)
#define GRF_VI_CSIPHY_CON_FORCE_RX_MODE_MASK               (0xFU << GRF_VI_CSIPHY_CON_FORCE_RX_MODE_SHIFT)              /* 0x0000000F */
#define GRF_VI_CSIPHY_CON_LANE_ENABLE_SHIFT                (4U)
#define GRF_VI_CSIPHY_CON_LANE_ENABLE_MASK                 (0xFU << GRF_VI_CSIPHY_CON_LANE_ENABLE_SHIFT)                /* 0x000000F0 */
#define GRF_VI_CSIPHY_CON_ENABLE_CK_SHIFT                  (8U)
#define GRF_VI_CSIPHY_CON_ENABLE_CK_MASK                   (0x1U << GRF_VI_CSIPHY_CON_ENABLE_CK_SHIFT)                  /* 0x00000100 */
#define GRF_VI_CSIPHY_CON_RXBYTECLKHS_0_INV_SEL_SHIFT      (9U)
#define GRF_VI_CSIPHY_CON_RXBYTECLKHS_0_INV_SEL_MASK       (0x1U << GRF_VI_CSIPHY_CON_RXBYTECLKHS_0_INV_SEL_SHIFT)      /* 0x00000200 */
#define GRF_VI_CSIPHY_CON_ENABLE_CK_1_SHIFT                (10U)
#define GRF_VI_CSIPHY_CON_ENABLE_CK_1_MASK                 (0x1U << GRF_VI_CSIPHY_CON_ENABLE_CK_1_SHIFT)                /* 0x00000400 */
#define GRF_VI_CSIPHY_CON_RXBYTECLKHS_1_INV_SEL_SHIFT      (11U)
#define GRF_VI_CSIPHY_CON_RXBYTECLKHS_1_INV_SEL_MASK       (0x1U << GRF_VI_CSIPHY_CON_RXBYTECLKHS_1_INV_SEL_SHIFT)      /* 0x00000800 */
/* CSIPHY_STATUS */
#define GRF_VI_CSIPHY_STATUS_OFFSET                        (0x18U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOT_SHIFT    (0U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOT_MASK     (0xFU << GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOT_SHIFT)    /* 0x0000000F */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_RXSKEWCALHS_SHIFT      (4U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_RXSKEWCALHS_MASK       (0xFU << GRF_VI_CSIPHY_STATUS_CSIPHY_RXSKEWCALHS_SHIFT)      /* 0x000000F0 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_0_SHIFT (8U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_0_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_0_SHIFT) /* 0x00000100 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_0_SHIFT (9U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_0_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_0_SHIFT) /* 0x00000200 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_1_SHIFT (10U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_1_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_1_SHIFT) /* 0x00000400 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_1_SHIFT (11U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_1_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_1_SHIFT) /* 0x00000800 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_2_SHIFT (12U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_2_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_2_SHIFT) /* 0x00001000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_2_SHIFT (13U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_2_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_2_SHIFT) /* 0x00002000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_3_SHIFT (14U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_3_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP0_3_SHIFT) /* 0x00004000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_3_SHIFT (15U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_3_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ERRCONTENTIONLP1_3_SHIFT) /* 0x00008000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOTCLK_SHIFT (16U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOTCLK_MASK  (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOTCLK_SHIFT) /* 0x00010000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOTCLK_1_SHIFT (17U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOTCLK_1_MASK (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_ULPSACTIVENOTCLK_1_SHIFT) /* 0x00020000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_DIRECTION_SHIFT        (18U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_DIRECTION_MASK         (0x1U << GRF_VI_CSIPHY_STATUS_CSIPHY_DIRECTION_SHIFT)        /* 0x00040000 */
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERR_CONTROL_SHIFT      (19U)
#define GRF_VI_CSIPHY_STATUS_CSIPHY_ERR_CONTROL_MASK       (0xFU << GRF_VI_CSIPHY_STATUS_CSIPHY_ERR_CONTROL_SHIFT)      /* 0x00780000 */
/* MISC_CON */
#define GRF_VI_MISC_CON_OFFSET                             (0x20U)
#define GRF_VI_MISC_CON_CSI2HOST0_PPI_SEL_2LANE_SHIFT      (0U)
#define GRF_VI_MISC_CON_CSI2HOST0_PPI_SEL_2LANE_MASK       (0x1U << GRF_VI_MISC_CON_CSI2HOST0_PPI_SEL_2LANE_SHIFT)      /* 0x00000001 */
#define GRF_VI_MISC_CON_ISP_FL_TRIG_SHIFT                  (3U)
#define GRF_VI_MISC_CON_ISP_FL_TRIG_MASK                   (0x1U << GRF_VI_MISC_CON_ISP_FL_TRIG_SHIFT)                  /* 0x00000008 */
#define GRF_VI_MISC_CON_SDMMC_DETN_IO_POL_SHIFT            (8U)
#define GRF_VI_MISC_CON_SDMMC_DETN_IO_POL_MASK             (0x1U << GRF_VI_MISC_CON_SDMMC_DETN_IO_POL_SHIFT)            /* 0x00000100 */
/* SDMMC_DET_CNT */
#define GRF_VI_SDMMC_DET_CNT_OFFSET                        (0x24U)
#define GRF_VI_SDMMC_DET_CNT_DETECT_CNT_SHIFT              (0U)
#define GRF_VI_SDMMC_DET_CNT_DETECT_CNT_MASK               (0x3FFFFFU << GRF_VI_SDMMC_DET_CNT_DETECT_CNT_SHIFT)         /* 0x003FFFFF */
/* SDMMC_SIG_DETECT_CON */
#define GRF_VI_SDMMC_SIG_DETECT_CON_OFFSET                 (0x28U)
#define GRF_VI_SDMMC_SIG_DETECT_CON_SIG_DETECT_RISE_EN_SHIFT (0U)
#define GRF_VI_SDMMC_SIG_DETECT_CON_SIG_DETECT_RISE_EN_MASK (0x1U << GRF_VI_SDMMC_SIG_DETECT_CON_SIG_DETECT_RISE_EN_SHIFT) /* 0x00000001 */
#define GRF_VI_SDMMC_SIG_DETECT_CON_SIG_DETECT_FALL_EN_SHIFT (1U)
#define GRF_VI_SDMMC_SIG_DETECT_CON_SIG_DETECT_FALL_EN_MASK (0x1U << GRF_VI_SDMMC_SIG_DETECT_CON_SIG_DETECT_FALL_EN_SHIFT) /* 0x00000002 */
/* SDMMC_SIG_DETECT_STATUS */
#define GRF_VI_SDMMC_SIG_DETECT_STATUS_OFFSET              (0x2CU)
#define GRF_VI_SDMMC_SIG_DETECT_STATUS_SIG_DETECT_RISE_IRQ_SHIFT (0U)
#define GRF_VI_SDMMC_SIG_DETECT_STATUS_SIG_DETECT_RISE_IRQ_MASK (0x1U << GRF_VI_SDMMC_SIG_DETECT_STATUS_SIG_DETECT_RISE_IRQ_SHIFT) /* 0x00000001 */
#define GRF_VI_SDMMC_SIG_DETECT_STATUS_SIG_DETECT_FALL_IRQ_SHIFT (1U)
#define GRF_VI_SDMMC_SIG_DETECT_STATUS_SIG_DETECT_FALL_IRQ_MASK (0x1U << GRF_VI_SDMMC_SIG_DETECT_STATUS_SIG_DETECT_FALL_IRQ_SHIFT) /* 0x00000002 */
/* SDMMC_STATUS_CLR */
#define GRF_VI_SDMMC_STATUS_CLR_OFFSET                     (0x30U)
#define GRF_VI_SDMMC_STATUS_CLR_SIG_DETECT_RISE_CLR_SHIFT  (0U)
#define GRF_VI_SDMMC_STATUS_CLR_SIG_DETECT_RISE_CLR_MASK   (0x1U << GRF_VI_SDMMC_STATUS_CLR_SIG_DETECT_RISE_CLR_SHIFT)  /* 0x00000001 */
#define GRF_VI_SDMMC_STATUS_CLR_SIG_DETECT_FALL_CLR_SHIFT  (1U)
#define GRF_VI_SDMMC_STATUS_CLR_SIG_DETECT_FALL_CLR_MASK   (0x1U << GRF_VI_SDMMC_STATUS_CLR_SIG_DETECT_FALL_CLR_SHIFT)  /* 0x00000002 */
/****************************************GRF_CPU*****************************************/
/* CON0 */
#define GRF_CPU_CON0_OFFSET                                (0x0U)
#define GRF_CPU_CON0_VINITHI_SHIFT                         (0U)
#define GRF_CPU_CON0_VINITHI_MASK                          (0x1U << GRF_CPU_CON0_VINITHI_SHIFT)                         /* 0x00000001 */
#define GRF_CPU_CON0_CFGEND_SHIFT                          (1U)
#define GRF_CPU_CON0_CFGEND_MASK                           (0x1U << GRF_CPU_CON0_CFGEND_SHIFT)                          /* 0x00000002 */
#define GRF_CPU_CON0_CFGTE_SHIFT                           (2U)
#define GRF_CPU_CON0_CFGTE_MASK                            (0x1U << GRF_CPU_CON0_CFGTE_SHIFT)                           /* 0x00000004 */
#define GRF_CPU_CON0_L1RSTDISABLE_SHIFT                    (3U)
#define GRF_CPU_CON0_L1RSTDISABLE_MASK                     (0x1U << GRF_CPU_CON0_L1RSTDISABLE_SHIFT)                    /* 0x00000008 */
#define GRF_CPU_CON0_L2RSTDISABLE_SHIFT                    (4U)
#define GRF_CPU_CON0_L2RSTDISABLE_MASK                     (0x1U << GRF_CPU_CON0_L2RSTDISABLE_SHIFT)                    /* 0x00000010 */
#define GRF_CPU_CON0_EVENTI_SHIFT                          (5U)
#define GRF_CPU_CON0_EVENTI_MASK                           (0x1U << GRF_CPU_CON0_EVENTI_SHIFT)                          /* 0x00000020 */
#define GRF_CPU_CON0_DBGROMADDRV_SHIFT                     (6U)
#define GRF_CPU_CON0_DBGROMADDRV_MASK                      (0x1U << GRF_CPU_CON0_DBGROMADDRV_SHIFT)                     /* 0x00000040 */
#define GRF_CPU_CON0_DBGSELFADDRV_SHIFT                    (7U)
#define GRF_CPU_CON0_DBGSELFADDRV_MASK                     (0x1U << GRF_CPU_CON0_DBGSELFADDRV_SHIFT)                    /* 0x00000080 */
#define GRF_CPU_CON0_EVENTO_CLEAR_SHIFT                    (8U)
#define GRF_CPU_CON0_EVENTO_CLEAR_MASK                     (0x1U << GRF_CPU_CON0_EVENTO_CLEAR_SHIFT)                    /* 0x00000100 */
#define GRF_CPU_CON0_CORE_SRST_WFIEN_SHIFT                 (9U)
#define GRF_CPU_CON0_CORE_SRST_WFIEN_MASK                  (0x1U << GRF_CPU_CON0_CORE_SRST_WFIEN_SHIFT)                 /* 0x00000200 */
#define GRF_CPU_CON0_COREPO_SRST_WFIEN_SHIFT               (10U)
#define GRF_CPU_CON0_COREPO_SRST_WFIEN_MASK                (0x1U << GRF_CPU_CON0_COREPO_SRST_WFIEN_SHIFT)               /* 0x00000400 */
/* MEM_CFG_UHDSPRA */
#define GRF_CPU_MEM_CFG_UHDSPRA_OFFSET                     (0x4U)
#define GRF_CPU_MEM_CFG_UHDSPRA_MEM_CFG_UHDSPRA_SHIFT      (0U)
#define GRF_CPU_MEM_CFG_UHDSPRA_MEM_CFG_UHDSPRA_MASK       (0x7FFU << GRF_CPU_MEM_CFG_UHDSPRA_MEM_CFG_UHDSPRA_SHIFT)    /* 0x000007FF */
/* STATUS */
#define GRF_CPU_STATUS_OFFSET                              (0x8U)
#define GRF_CPU_STATUS_STANDBYWFE_SHIFT                    (0U)
#define GRF_CPU_STATUS_STANDBYWFE_MASK                     (0x1U << GRF_CPU_STATUS_STANDBYWFE_SHIFT)                    /* 0x00000001 */
#define GRF_CPU_STATUS_STANDBYWFI_SHIFT                    (1U)
#define GRF_CPU_STATUS_STANDBYWFI_MASK                     (0x1U << GRF_CPU_STATUS_STANDBYWFI_SHIFT)                    /* 0x00000002 */
#define GRF_CPU_STATUS_SMPNAMP_SHIFT                       (2U)
#define GRF_CPU_STATUS_SMPNAMP_MASK                        (0x1U << GRF_CPU_STATUS_SMPNAMP_SHIFT)                       /* 0x00000004 */
#define GRF_CPU_STATUS_STANDBYWFIL2_SHIFT                  (3U)
#define GRF_CPU_STATUS_STANDBYWFIL2_MASK                   (0x1U << GRF_CPU_STATUS_STANDBYWFIL2_SHIFT)                  /* 0x00000008 */
#define GRF_CPU_STATUS_EVENTO_RISING_EDGE_SHIFT            (4U)
#define GRF_CPU_STATUS_EVENTO_RISING_EDGE_MASK             (0x1U << GRF_CPU_STATUS_EVENTO_RISING_EDGE_SHIFT)            /* 0x00000010 */
#define GRF_CPU_STATUS_ST_JTAGTOP_SHIFT                    (5U)
#define GRF_CPU_STATUS_ST_JTAGTOP_MASK                     (0x1U << GRF_CPU_STATUS_ST_JTAGTOP_SHIFT)                    /* 0x00000020 */
#define GRF_CPU_STATUS_ST_JTAGNSW_SHIFT                    (6U)
#define GRF_CPU_STATUS_ST_JTAGNSW_MASK                     (0x1U << GRF_CPU_STATUS_ST_JTAGNSW_SHIFT)                    /* 0x00000040 */
/****************************************GRF_DDR*****************************************/
/* CON0 */
#define GRF_DDR_CON0_OFFSET                                (0x0U)
#define GRF_DDR_CON0_LPR_FULL2CRITICAL_SHIFT               (3U)
#define GRF_DDR_CON0_LPR_FULL2CRITICAL_MASK                (0x1U << GRF_DDR_CON0_LPR_FULL2CRITICAL_SHIFT)               /* 0x00000008 */
#define GRF_DDR_CON0_HPR_FULL2CRITICAL_SHIFT               (4U)
#define GRF_DDR_CON0_HPR_FULL2CRITICAL_MASK                (0x1U << GRF_DDR_CON0_HPR_FULL2CRITICAL_SHIFT)               /* 0x00000010 */
#define GRF_DDR_CON0_WR_FULL2CRITICAL_SHIFT                (5U)
#define GRF_DDR_CON0_WR_FULL2CRITICAL_MASK                 (0x1U << GRF_DDR_CON0_WR_FULL2CRITICAL_SHIFT)                /* 0x00000020 */
#define GRF_DDR_CON0_RRBTOKEN_EMPTY2CRITICAL_SHIFT         (6U)
#define GRF_DDR_CON0_RRBTOKEN_EMPTY2CRITICAL_MASK          (0x1U << GRF_DDR_CON0_RRBTOKEN_EMPTY2CRITICAL_SHIFT)         /* 0x00000040 */
#define GRF_DDR_CON0_WR_MASK_BY_HPR_SHIFT                  (7U)
#define GRF_DDR_CON0_WR_MASK_BY_HPR_MASK                   (0x1U << GRF_DDR_CON0_WR_MASK_BY_HPR_SHIFT)                  /* 0x00000080 */
#define GRF_DDR_CON0_HPR_ENTRY_NUM_SHIFT                   (8U)
#define GRF_DDR_CON0_HPR_ENTRY_NUM_MASK                    (0x3FU << GRF_DDR_CON0_HPR_ENTRY_NUM_SHIFT)                  /* 0x00003F00 */
#define GRF_DDR_CON0_DDRMON_EXTERNAL_TRIGGER_EN_SHIFT      (14U)
#define GRF_DDR_CON0_DDRMON_EXTERNAL_TRIGGER_EN_MASK       (0x1U << GRF_DDR_CON0_DDRMON_EXTERNAL_TRIGGER_EN_SHIFT)      /* 0x00004000 */
#define GRF_DDR_CON0_DDRC_MEM_GATE_EN_SHIFT                (15U)
#define GRF_DDR_CON0_DDRC_MEM_GATE_EN_MASK                 (0x1U << GRF_DDR_CON0_DDRC_MEM_GATE_EN_SHIFT)                /* 0x00008000 */
/* CON1 */
#define GRF_DDR_CON1_OFFSET                                (0x4U)
#define GRF_DDR_CON1_DDRC_AXI_CG_EN_SHIFT                  (0U)
#define GRF_DDR_CON1_DDRC_AXI_CG_EN_MASK                   (0x7U << GRF_DDR_CON1_DDRC_AXI_CG_EN_SHIFT)                  /* 0x00000007 */
#define GRF_DDR_CON1_DDRC_CORE_CG_EN_SHIFT                 (3U)
#define GRF_DDR_CON1_DDRC_CORE_CG_EN_MASK                  (0x1U << GRF_DDR_CON1_DDRC_CORE_CG_EN_SHIFT)                 /* 0x00000008 */
#define GRF_DDR_CON1_DDRC_APB_CG_EN_SHIFT                  (4U)
#define GRF_DDR_CON1_DDRC_APB_CG_EN_MASK                   (0x7U << GRF_DDR_CON1_DDRC_APB_CG_EN_SHIFT)                  /* 0x00000070 */
#define GRF_DDR_CON1_DDRC_SYSCREQ_CG_EN_SHIFT              (7U)
#define GRF_DDR_CON1_DDRC_SYSCREQ_CG_EN_MASK               (0x1U << GRF_DDR_CON1_DDRC_SYSCREQ_CG_EN_SHIFT)              /* 0x00000080 */
#define GRF_DDR_CON1_DDRC_PDSRLP_CG_EN_SHIFT               (8U)
#define GRF_DDR_CON1_DDRC_PDSRLP_CG_EN_MASK                (0x1U << GRF_DDR_CON1_DDRC_PDSRLP_CG_EN_SHIFT)               /* 0x00000100 */
#define GRF_DDR_CON1_DDRC_ACLK_AUTO_GATE_ENABLE_SHIFT      (9U)
#define GRF_DDR_CON1_DDRC_ACLK_AUTO_GATE_ENABLE_MASK       (0x1U << GRF_DDR_CON1_DDRC_ACLK_AUTO_GATE_ENABLE_SHIFT)      /* 0x00000200 */
#define GRF_DDR_CON1_BIU_CLK_AUTO_GATE_ENABLE_SHIFT        (10U)
#define GRF_DDR_CON1_BIU_CLK_AUTO_GATE_ENABLE_MASK         (0x1U << GRF_DDR_CON1_BIU_CLK_AUTO_GATE_ENABLE_SHIFT)        /* 0x00000400 */
#define GRF_DDR_CON1_SELFREF_TYPE2_EN_SHIFT                (11U)
#define GRF_DDR_CON1_SELFREF_TYPE2_EN_MASK                 (0x1U << GRF_DDR_CON1_SELFREF_TYPE2_EN_SHIFT)                /* 0x00000800 */
#define GRF_DDR_CON1_DDRC_AUTO_SR_DLY_SHIFT                (12U)
#define GRF_DDR_CON1_DDRC_AUTO_SR_DLY_MASK                 (0xFU << GRF_DDR_CON1_DDRC_AUTO_SR_DLY_SHIFT)                /* 0x0000F000 */
/* CON2 */
#define GRF_DDR_CON2_OFFSET                                (0x8U)
#define GRF_DDR_CON2_PRESS_CG_EN_SHIFT                     (0U)
#define GRF_DDR_CON2_PRESS_CG_EN_MASK                      (0x7U << GRF_DDR_CON2_PRESS_CG_EN_SHIFT)                     /* 0x00000007 */
#define GRF_DDR_CON2_PA_RMASK_SHIFT                        (4U)
#define GRF_DDR_CON2_PA_RMASK_MASK                         (0x3FU << GRF_DDR_CON2_PA_RMASK_SHIFT)                       /* 0x000003F0 */
#define GRF_DDR_CON2_PA_WMASK_SHIFT                        (12U)
#define GRF_DDR_CON2_PA_WMASK_MASK                         (0x7U << GRF_DDR_CON2_PA_WMASK_SHIFT)                        /* 0x00007000 */
/* CON3 */
#define GRF_DDR_CON3_OFFSET                                (0xCU)
#define GRF_DDR_CON3_ARPOISON_SHIFT                        (0U)
#define GRF_DDR_CON3_ARPOISON_MASK                         (0x7U << GRF_DDR_CON3_ARPOISON_SHIFT)                        /* 0x00000007 */
#define GRF_DDR_CON3_AWPOISON_SHIFT                        (4U)
#define GRF_DDR_CON3_AWPOISON_MASK                         (0x7U << GRF_DDR_CON3_AWPOISON_SHIFT)                        /* 0x00000070 */
#define GRF_DDR_CON3_ARAUTOPRE_SHIFT                       (8U)
#define GRF_DDR_CON3_ARAUTOPRE_MASK                        (0x7U << GRF_DDR_CON3_ARAUTOPRE_SHIFT)                       /* 0x00000700 */
#define GRF_DDR_CON3_AWAUTOPRE_SHIFT                       (12U)
#define GRF_DDR_CON3_AWAUTOPRE_MASK                        (0x7U << GRF_DDR_CON3_AWAUTOPRE_SHIFT)                       /* 0x00007000 */
/* CON4 */
#define GRF_DDR_CON4_OFFSET                                (0x10U)
#define GRF_DDR_CON4_PRESS_EN_SHIFT                        (0U)
#define GRF_DDR_CON4_PRESS_EN_MASK                         (0xFU << GRF_DDR_CON4_PRESS_EN_SHIFT)                        /* 0x0000000F */
#define GRF_DDR_CON4_PRESS_RDWR_ORDER_SHIFT                (4U)
#define GRF_DDR_CON4_PRESS_RDWR_ORDER_MASK                 (0xFU << GRF_DDR_CON4_PRESS_RDWR_ORDER_SHIFT)                /* 0x000000F0 */
#define GRF_DDR_CON4_PRESS_POLICY_SHIFT                    (8U)
#define GRF_DDR_CON4_PRESS_POLICY_MASK                     (0x3U << GRF_DDR_CON4_PRESS_POLICY_SHIFT)                    /* 0x00000300 */
#define GRF_DDR_CON4_PRESS_QUEUE_WAIT_LIMIT_SHIFT          (10U)
#define GRF_DDR_CON4_PRESS_QUEUE_WAIT_LIMIT_MASK           (0x1FU << GRF_DDR_CON4_PRESS_QUEUE_WAIT_LIMIT_SHIFT)         /* 0x00007C00 */
/* CON5 */
#define GRF_DDR_CON5_OFFSET                                (0x14U)
#define GRF_DDR_CON5_DDRC_SLVERR_ENABLE_SHIFT              (0U)
#define GRF_DDR_CON5_DDRC_SLVERR_ENABLE_MASK               (0x1U << GRF_DDR_CON5_DDRC_SLVERR_ENABLE_SHIFT)              /* 0x00000001 */
#define GRF_DDR_CON5_DFI_INIT_COMPLETE_SEL_SHIFT           (1U)
#define GRF_DDR_CON5_DFI_INIT_COMPLETE_SEL_MASK            (0x1U << GRF_DDR_CON5_DFI_INIT_COMPLETE_SEL_SHIFT)           /* 0x00000002 */
#define GRF_DDR_CON5_DFI_INIT_COMPLETE_SHIFT               (2U)
#define GRF_DDR_CON5_DFI_INIT_COMPLETE_MASK                (0x1U << GRF_DDR_CON5_DFI_INIT_COMPLETE_SHIFT)               /* 0x00000004 */
#define GRF_DDR_CON5_CSYSREQ_DDRC_PMU_SHIFT                (3U)
#define GRF_DDR_CON5_CSYSREQ_DDRC_PMU_MASK                 (0x1U << GRF_DDR_CON5_CSYSREQ_DDRC_PMU_SHIFT)                /* 0x00000008 */
#define GRF_DDR_CON5_CSYSREQ_ACLK_CPU_SHIFT                (4U)
#define GRF_DDR_CON5_CSYSREQ_ACLK_CPU_MASK                 (0x1U << GRF_DDR_CON5_CSYSREQ_ACLK_CPU_SHIFT)                /* 0x00000010 */
#define GRF_DDR_CON5_CSYSREQ_ACLK_NPVD_SHIFT               (5U)
#define GRF_DDR_CON5_CSYSREQ_ACLK_NPVD_MASK                (0x1U << GRF_DDR_CON5_CSYSREQ_ACLK_NPVD_SHIFT)               /* 0x00000020 */
#define GRF_DDR_CON5_CSYSREQ_ACLK_VI_SHIFT                 (6U)
#define GRF_DDR_CON5_CSYSREQ_ACLK_VI_MASK                  (0x1U << GRF_DDR_CON5_CSYSREQ_ACLK_VI_SHIFT)                 /* 0x00000040 */
#define GRF_DDR_CON5_AWQOS_URGENT_EN_SHIFT                 (8U)
#define GRF_DDR_CON5_AWQOS_URGENT_EN_MASK                  (0x7U << GRF_DDR_CON5_AWQOS_URGENT_EN_SHIFT)                 /* 0x00000700 */
#define GRF_DDR_CON5_ARQOS_URGENT_EN_SHIFT                 (12U)
#define GRF_DDR_CON5_ARQOS_URGENT_EN_MASK                  (0x7U << GRF_DDR_CON5_ARQOS_URGENT_EN_SHIFT)                 /* 0x00007000 */
/* CON6 */
#define GRF_DDR_CON6_OFFSET                                (0x18U)
#define GRF_DDR_CON6_PRESS_AR_TH_SHIFT                     (0U)
#define GRF_DDR_CON6_PRESS_AR_TH_MASK                      (0xFFFFU << GRF_DDR_CON6_PRESS_AR_TH_SHIFT)                  /* 0x0000FFFF */
/* CON7 */
#define GRF_DDR_CON7_OFFSET                                (0x1CU)
#define GRF_DDR_CON7_PRESS_AW_TH_SHIFT                     (0U)
#define GRF_DDR_CON7_PRESS_AW_TH_MASK                      (0xFFFFU << GRF_DDR_CON7_PRESS_AW_TH_SHIFT)                  /* 0x0000FFFF */
/* CON8 */
#define GRF_DDR_CON8_OFFSET                                (0x20U)
#define GRF_DDR_CON8_DFI_PHYMSTR_CS_STATE_SHIFT            (0U)
#define GRF_DDR_CON8_DFI_PHYMSTR_CS_STATE_MASK             (0x1U << GRF_DDR_CON8_DFI_PHYMSTR_CS_STATE_SHIFT)            /* 0x00000001 */
#define GRF_DDR_CON8_DFI_PHYMSTR_STATE_SEL_SHIFT           (1U)
#define GRF_DDR_CON8_DFI_PHYMSTR_STATE_SEL_MASK            (0x1U << GRF_DDR_CON8_DFI_PHYMSTR_STATE_SEL_SHIFT)           /* 0x00000002 */
#define GRF_DDR_CON8_DFI_PHYMST_TYPE_SHIFT                 (2U)
#define GRF_DDR_CON8_DFI_PHYMST_TYPE_MASK                  (0x3U << GRF_DDR_CON8_DFI_PHYMST_TYPE_SHIFT)                 /* 0x0000000C */
#define GRF_DDR_CON8_CSYSREQ_ACLK_CPU_PMU_SHIFT            (4U)
#define GRF_DDR_CON8_CSYSREQ_ACLK_CPU_PMU_MASK             (0x1U << GRF_DDR_CON8_CSYSREQ_ACLK_CPU_PMU_SHIFT)            /* 0x00000010 */
#define GRF_DDR_CON8_CSYSREQ_ACLK_NPVD_PMU_SHIFT           (5U)
#define GRF_DDR_CON8_CSYSREQ_ACLK_NPVD_PMU_MASK            (0x1U << GRF_DDR_CON8_CSYSREQ_ACLK_NPVD_PMU_SHIFT)           /* 0x00000020 */
#define GRF_DDR_CON8_CSYSREQ_ACLK_VI_PMU_SHIFT             (6U)
#define GRF_DDR_CON8_CSYSREQ_ACLK_VI_PMU_MASK              (0x1U << GRF_DDR_CON8_CSYSREQ_ACLK_VI_PMU_SHIFT)             /* 0x00000040 */
#define GRF_DDR_CON8_CSYSREQ_ACLK_CPU_GRF_SHIFT            (8U)
#define GRF_DDR_CON8_CSYSREQ_ACLK_CPU_GRF_MASK             (0x1U << GRF_DDR_CON8_CSYSREQ_ACLK_CPU_GRF_SHIFT)            /* 0x00000100 */
#define GRF_DDR_CON8_CSYSREQ_ACLK_NPVD_GRF_SHIFT           (9U)
#define GRF_DDR_CON8_CSYSREQ_ACLK_NPVD_GRF_MASK            (0x1U << GRF_DDR_CON8_CSYSREQ_ACLK_NPVD_GRF_SHIFT)           /* 0x00000200 */
#define GRF_DDR_CON8_CSYSREQ_ACLK_VI_GRF_SHIFT             (10U)
#define GRF_DDR_CON8_CSYSREQ_ACLK_VI_GRF_MASK              (0x1U << GRF_DDR_CON8_CSYSREQ_ACLK_VI_GRF_SHIFT)             /* 0x00000400 */
/* CON9 */
#define GRF_DDR_CON9_OFFSET                                (0x24U)
#define GRF_DDR_CON9_GRF_DFI_SCRAMBLE_CON0_SHIFT           (0U)
#define GRF_DDR_CON9_GRF_DFI_SCRAMBLE_CON0_MASK            (0xFFFFU << GRF_DDR_CON9_GRF_DFI_SCRAMBLE_CON0_SHIFT)        /* 0x0000FFFF */
/* CON10 */
#define GRF_DDR_CON10_OFFSET                               (0x28U)
#define GRF_DDR_CON10_GRF_DFI_SCRAMBLE_CON1_SHIFT          (0U)
#define GRF_DDR_CON10_GRF_DFI_SCRAMBLE_CON1_MASK           (0xFFFFU << GRF_DDR_CON10_GRF_DFI_SCRAMBLE_CON1_SHIFT)       /* 0x0000FFFF */
/* CON11 */
#define GRF_DDR_CON11_OFFSET                               (0x2CU)
#define GRF_DDR_CON11_DFI_SCRAMBLE_KEY0_SHIFT              (0U)
#define GRF_DDR_CON11_DFI_SCRAMBLE_KEY0_MASK               (0xFFFFU << GRF_DDR_CON11_DFI_SCRAMBLE_KEY0_SHIFT)           /* 0x0000FFFF */
/* CON12 */
#define GRF_DDR_CON12_OFFSET                               (0x30U)
#define GRF_DDR_CON12_DFI_SCRAMBLE_KEY1_SHIFT              (0U)
#define GRF_DDR_CON12_DFI_SCRAMBLE_KEY1_MASK               (0xFFFFU << GRF_DDR_CON12_DFI_SCRAMBLE_KEY1_SHIFT)           /* 0x0000FFFF */
/* CON13 */
#define GRF_DDR_CON13_OFFSET                               (0x34U)
#define GRF_DDR_CON13_DFI_SCRAMBLE_KEY2_SHIFT              (0U)
#define GRF_DDR_CON13_DFI_SCRAMBLE_KEY2_MASK               (0xFFFFU << GRF_DDR_CON13_DFI_SCRAMBLE_KEY2_SHIFT)           /* 0x0000FFFF */
/* CON14 */
#define GRF_DDR_CON14_OFFSET                               (0x38U)
#define GRF_DDR_CON14_DFI_SCRAMBLE_KEY3_SHIFT              (0U)
#define GRF_DDR_CON14_DFI_SCRAMBLE_KEY3_MASK               (0xFFFFU << GRF_DDR_CON14_DFI_SCRAMBLE_KEY3_SHIFT)           /* 0x0000FFFF */
/* PROBE_CTRL */
#define GRF_DDR_PROBE_CTRL_OFFSET                          (0x80U)
#define GRF_DDR_PROBE_CTRL_CPU_PORT_MAINSTATSUSPEND_SHIFT  (0U)
#define GRF_DDR_PROBE_CTRL_CPU_PORT_MAINSTATSUSPEND_MASK   (0x1U << GRF_DDR_PROBE_CTRL_CPU_PORT_MAINSTATSUSPEND_SHIFT)  /* 0x00000001 */
#define GRF_DDR_PROBE_CTRL_VI_PORT_MAINSTATSUSPEND_SHIFT   (1U)
#define GRF_DDR_PROBE_CTRL_VI_PORT_MAINSTATSUSPEND_MASK    (0x1U << GRF_DDR_PROBE_CTRL_VI_PORT_MAINSTATSUSPEND_SHIFT)   /* 0x00000002 */
#define GRF_DDR_PROBE_CTRL_NPVD_PORT_MAINSTATSUSPEND_SHIFT (2U)
#define GRF_DDR_PROBE_CTRL_NPVD_PORT_MAINSTATSUSPEND_MASK  (0x3U << GRF_DDR_PROBE_CTRL_NPVD_PORT_MAINSTATSUSPEND_SHIFT) /* 0x0000000C */
/* STATUS8 */
#define GRF_DDR_STATUS8_OFFSET                             (0x120U)
#define GRF_DDR_STATUS8                                    (0x0U)
#define GRF_DDR_STATUS8_CSYSREQ_DDRC_SHIFT                 (0U)
#define GRF_DDR_STATUS8_CSYSREQ_DDRC_MASK                  (0x1U << GRF_DDR_STATUS8_CSYSREQ_DDRC_SHIFT)                 /* 0x00000001 */
#define GRF_DDR_STATUS8_CSYSACK_DDRC_SHIFT                 (1U)
#define GRF_DDR_STATUS8_CSYSACK_DDRC_MASK                  (0x1U << GRF_DDR_STATUS8_CSYSACK_DDRC_SHIFT)                 /* 0x00000002 */
#define GRF_DDR_STATUS8_CACTIVE_DDRC_SHIFT                 (2U)
#define GRF_DDR_STATUS8_CACTIVE_DDRC_MASK                  (0x1U << GRF_DDR_STATUS8_CACTIVE_DDRC_SHIFT)                 /* 0x00000004 */
#define GRF_DDR_STATUS8_MSCH_PWR_IDLE_SHIFT                (3U)
#define GRF_DDR_STATUS8_MSCH_PWR_IDLE_MASK                 (0x1U << GRF_DDR_STATUS8_MSCH_PWR_IDLE_SHIFT)                /* 0x00000008 */
#define GRF_DDR_STATUS8_MSCH_PWR_IDLEACK_SHIFT             (4U)
#define GRF_DDR_STATUS8_MSCH_PWR_IDLEACK_MASK              (0x1U << GRF_DDR_STATUS8_MSCH_PWR_IDLEACK_SHIFT)             /* 0x00000010 */
#define GRF_DDR_STATUS8_MSCH_PWR_IDLEREQ_SHIFT             (5U)
#define GRF_DDR_STATUS8_MSCH_PWR_IDLEREQ_MASK              (0x1U << GRF_DDR_STATUS8_MSCH_PWR_IDLEREQ_SHIFT)             /* 0x00000020 */
#define GRF_DDR_STATUS8_DDR_APB_PWR_IDLE_SHIFT             (6U)
#define GRF_DDR_STATUS8_DDR_APB_PWR_IDLE_MASK              (0x1U << GRF_DDR_STATUS8_DDR_APB_PWR_IDLE_SHIFT)             /* 0x00000040 */
#define GRF_DDR_STATUS8_DDR_APB_PWR_IDLEACK_SHIFT          (7U)
#define GRF_DDR_STATUS8_DDR_APB_PWR_IDLEACK_MASK           (0x1U << GRF_DDR_STATUS8_DDR_APB_PWR_IDLEACK_SHIFT)          /* 0x00000080 */
#define GRF_DDR_STATUS8_DDR_APB_PWR_IDLEREQ_SHIFT          (8U)
#define GRF_DDR_STATUS8_DDR_APB_PWR_IDLEREQ_MASK           (0x1U << GRF_DDR_STATUS8_DDR_APB_PWR_IDLEREQ_SHIFT)          /* 0x00000100 */
#define GRF_DDR_STATUS8_DFI_INIT_COMPLETE_SHIFT            (9U)
#define GRF_DDR_STATUS8_DFI_INIT_COMPLETE_MASK             (0x1U << GRF_DDR_STATUS8_DFI_INIT_COMPLETE_SHIFT)            /* 0x00000200 */
#define GRF_DDR_STATUS8_DDRC_REG_SELFREF_TYPE_SHIFT        (10U)
#define GRF_DDR_STATUS8_DDRC_REG_SELFREF_TYPE_MASK         (0x3U << GRF_DDR_STATUS8_DDRC_REG_SELFREF_TYPE_SHIFT)        /* 0x00000C00 */
#define GRF_DDR_STATUS8_DTP_DDRFWLKDCPWRACTIVE_SHIFT       (12U)
#define GRF_DDR_STATUS8_DTP_DDRFWLKDCPWRACTIVE_MASK        (0x1U << GRF_DDR_STATUS8_DTP_DDRFWLKDCPWRACTIVE_SHIFT)       /* 0x00001000 */
#define GRF_DDR_STATUS8_DDRC_IDLE_ST_SHIFT                 (13U)
#define GRF_DDR_STATUS8_DDRC_IDLE_ST_MASK                  (0x1U << GRF_DDR_STATUS8_DDRC_IDLE_ST_SHIFT)                 /* 0x00002000 */
#define GRF_DDR_STATUS8_VI_MAINTRACEALARM_SHIFT            (14U)
#define GRF_DDR_STATUS8_VI_MAINTRACEALARM_MASK             (0x1U << GRF_DDR_STATUS8_VI_MAINTRACEALARM_SHIFT)            /* 0x00004000 */
#define GRF_DDR_STATUS8_VI_MAINSTATALARM_SHIFT             (15U)
#define GRF_DDR_STATUS8_VI_MAINSTATALARM_MASK              (0x1U << GRF_DDR_STATUS8_VI_MAINSTATALARM_SHIFT)             /* 0x00008000 */
#define GRF_DDR_STATUS8_NPVD_MAINTRACEALARM_SHIFT          (16U)
#define GRF_DDR_STATUS8_NPVD_MAINTRACEALARM_MASK           (0x1U << GRF_DDR_STATUS8_NPVD_MAINTRACEALARM_SHIFT)          /* 0x00010000 */
#define GRF_DDR_STATUS8_NPVD_MAINSTATALARM_SHIFT           (17U)
#define GRF_DDR_STATUS8_NPVD_MAINSTATALARM_MASK            (0x1U << GRF_DDR_STATUS8_NPVD_MAINSTATALARM_SHIFT)           /* 0x00020000 */
#define GRF_DDR_STATUS8_CPU_MAINTRACEALARM_SHIFT           (18U)
#define GRF_DDR_STATUS8_CPU_MAINTRACEALARM_MASK            (0x1U << GRF_DDR_STATUS8_CPU_MAINTRACEALARM_SHIFT)           /* 0x00040000 */
#define GRF_DDR_STATUS8_CPU_MAINSTATALARM_SHIFT            (19U)
#define GRF_DDR_STATUS8_CPU_MAINSTATALARM_MASK             (0x1U << GRF_DDR_STATUS8_CPU_MAINSTATALARM_SHIFT)            /* 0x00080000 */
#define GRF_DDR_STATUS8_CSYSACK_ACLK_CPU_SHIFT             (20U)
#define GRF_DDR_STATUS8_CSYSACK_ACLK_CPU_MASK              (0x1U << GRF_DDR_STATUS8_CSYSACK_ACLK_CPU_SHIFT)             /* 0x00100000 */
#define GRF_DDR_STATUS8_CSYSACK_ACLK_NPVD_SHIFT            (21U)
#define GRF_DDR_STATUS8_CSYSACK_ACLK_NPVD_MASK             (0x1U << GRF_DDR_STATUS8_CSYSACK_ACLK_NPVD_SHIFT)            /* 0x00200000 */
#define GRF_DDR_STATUS8_CSYSACK_ACLK_VI_SHIFT              (22U)
#define GRF_DDR_STATUS8_CSYSACK_ACLK_VI_MASK               (0x1U << GRF_DDR_STATUS8_CSYSACK_ACLK_VI_SHIFT)              /* 0x00400000 */
#define GRF_DDR_STATUS8_CACTIVE_ACLK_CPU_SHIFT             (23U)
#define GRF_DDR_STATUS8_CACTIVE_ACLK_CPU_MASK              (0x1U << GRF_DDR_STATUS8_CACTIVE_ACLK_CPU_SHIFT)             /* 0x00800000 */
#define GRF_DDR_STATUS8_CACTIVE_ACLK_NPVD_SHIFT            (24U)
#define GRF_DDR_STATUS8_CACTIVE_ACLK_NPVD_MASK             (0x1U << GRF_DDR_STATUS8_CACTIVE_ACLK_NPVD_SHIFT)            /* 0x01000000 */
#define GRF_DDR_STATUS8_CACTIVE_ACLK_VI_SHIFT              (25U)
#define GRF_DDR_STATUS8_CACTIVE_ACLK_VI_MASK               (0x1U << GRF_DDR_STATUS8_CACTIVE_ACLK_VI_SHIFT)              /* 0x02000000 */
/* STATUS9 */
#define GRF_DDR_STATUS9_OFFSET                             (0x124U)
#define GRF_DDR_STATUS9                                    (0x0U)
#define GRF_DDR_STATUS9_LPR_CREDIT_CNT_SHIFT               (0U)
#define GRF_DDR_STATUS9_LPR_CREDIT_CNT_MASK                (0x7FU << GRF_DDR_STATUS9_LPR_CREDIT_CNT_SHIFT)              /* 0x0000007F */
#define GRF_DDR_STATUS9_HPR_CREDIT_CNT_SHIFT               (8U)
#define GRF_DDR_STATUS9_HPR_CREDIT_CNT_MASK                (0x7FU << GRF_DDR_STATUS9_HPR_CREDIT_CNT_SHIFT)              /* 0x00007F00 */
#define GRF_DDR_STATUS9_WR_CREDIT_CNT_SHIFT                (16U)
#define GRF_DDR_STATUS9_WR_CREDIT_CNT_MASK                 (0x7FU << GRF_DDR_STATUS9_WR_CREDIT_CNT_SHIFT)               /* 0x007F0000 */
/****************************************GRF_SYS*****************************************/
/* PERI_CON0 */
#define GRF_SYS_PERI_CON0_OFFSET                           (0x0U)
#define GRF_SYS_PERI_CON0_UART0_RTS_INV_SHIFT              (0U)
#define GRF_SYS_PERI_CON0_UART0_RTS_INV_MASK               (0x1U << GRF_SYS_PERI_CON0_UART0_RTS_INV_SHIFT)              /* 0x00000001 */
#define GRF_SYS_PERI_CON0_UART0_CTS_INV_SHIFT              (1U)
#define GRF_SYS_PERI_CON0_UART0_CTS_INV_MASK               (0x1U << GRF_SYS_PERI_CON0_UART0_CTS_INV_SHIFT)              /* 0x00000002 */
#define GRF_SYS_PERI_CON0_UART0_DMA_RX_SINGLE_BYPASS_SHIFT (2U)
#define GRF_SYS_PERI_CON0_UART0_DMA_RX_SINGLE_BYPASS_MASK  (0x1U << GRF_SYS_PERI_CON0_UART0_DMA_RX_SINGLE_BYPASS_SHIFT) /* 0x00000004 */
#define GRF_SYS_PERI_CON0_UART1_RTS_INV_SHIFT              (4U)
#define GRF_SYS_PERI_CON0_UART1_RTS_INV_MASK               (0x1U << GRF_SYS_PERI_CON0_UART1_RTS_INV_SHIFT)              /* 0x00000010 */
#define GRF_SYS_PERI_CON0_UART1_CTS_INV_SHIFT              (5U)
#define GRF_SYS_PERI_CON0_UART1_CTS_INV_MASK               (0x1U << GRF_SYS_PERI_CON0_UART1_CTS_INV_SHIFT)              /* 0x00000020 */
#define GRF_SYS_PERI_CON0_UART1_DMA_RX_SINGLE_BYPASS_SHIFT (6U)
#define GRF_SYS_PERI_CON0_UART1_DMA_RX_SINGLE_BYPASS_MASK  (0x1U << GRF_SYS_PERI_CON0_UART1_DMA_RX_SINGLE_BYPASS_SHIFT) /* 0x00000040 */
/* PERI_CON1 */
#define GRF_SYS_PERI_CON1_OFFSET                           (0x4U)
#define GRF_SYS_PERI_CON1_I2C4_DMA_REQACK_RX_CLR_SHIFT     (0U)
#define GRF_SYS_PERI_CON1_I2C4_DMA_REQACK_RX_CLR_MASK      (0x1U << GRF_SYS_PERI_CON1_I2C4_DMA_REQACK_RX_CLR_SHIFT)     /* 0x00000001 */
#define GRF_SYS_PERI_CON1_I2C4_DMA_REQACK_TX_CLR_SHIFT     (1U)
#define GRF_SYS_PERI_CON1_I2C4_DMA_REQACK_TX_CLR_MASK      (0x1U << GRF_SYS_PERI_CON1_I2C4_DMA_REQACK_TX_CLR_SHIFT)     /* 0x00000002 */
#define GRF_SYS_PERI_CON1_SAI_DMA_REQACK_RX_CLR_SHIFT      (2U)
#define GRF_SYS_PERI_CON1_SAI_DMA_REQACK_RX_CLR_MASK       (0x1U << GRF_SYS_PERI_CON1_SAI_DMA_REQACK_RX_CLR_SHIFT)      /* 0x00000004 */
#define GRF_SYS_PERI_CON1_SAI_DMA_REQACK_TX_CLR_SHIFT      (3U)
#define GRF_SYS_PERI_CON1_SAI_DMA_REQACK_TX_CLR_MASK       (0x1U << GRF_SYS_PERI_CON1_SAI_DMA_REQACK_TX_CLR_SHIFT)      /* 0x00000008 */
#define GRF_SYS_PERI_CON1_SPI0_DMA_REQACK_RX_CLR_SHIFT     (4U)
#define GRF_SYS_PERI_CON1_SPI0_DMA_REQACK_RX_CLR_MASK      (0x1U << GRF_SYS_PERI_CON1_SPI0_DMA_REQACK_RX_CLR_SHIFT)     /* 0x00000010 */
#define GRF_SYS_PERI_CON1_SPI0_DMA_REQACK_TX_CLR_SHIFT     (5U)
#define GRF_SYS_PERI_CON1_SPI0_DMA_REQACK_TX_CLR_MASK      (0x1U << GRF_SYS_PERI_CON1_SPI0_DMA_REQACK_TX_CLR_SHIFT)     /* 0x00000020 */
#define GRF_SYS_PERI_CON1_UART1_DMA_REQACK_RX_CLR_SHIFT    (6U)
#define GRF_SYS_PERI_CON1_UART1_DMA_REQACK_RX_CLR_MASK     (0x1U << GRF_SYS_PERI_CON1_UART1_DMA_REQACK_RX_CLR_SHIFT)    /* 0x00000040 */
#define GRF_SYS_PERI_CON1_UART1_DMA_REQACK_TX_CLR_SHIFT    (7U)
#define GRF_SYS_PERI_CON1_UART1_DMA_REQACK_TX_CLR_MASK     (0x1U << GRF_SYS_PERI_CON1_UART1_DMA_REQACK_TX_CLR_SHIFT)    /* 0x00000080 */
#define GRF_SYS_PERI_CON1_UART2_DMA_REQACK_RX_CLR_SHIFT    (8U)
#define GRF_SYS_PERI_CON1_UART2_DMA_REQACK_RX_CLR_MASK     (0x1U << GRF_SYS_PERI_CON1_UART2_DMA_REQACK_RX_CLR_SHIFT)    /* 0x00000100 */
#define GRF_SYS_PERI_CON1_UART2_DMA_REQACK_TX_CLR_SHIFT    (9U)
#define GRF_SYS_PERI_CON1_UART2_DMA_REQACK_TX_CLR_MASK     (0x1U << GRF_SYS_PERI_CON1_UART2_DMA_REQACK_TX_CLR_SHIFT)    /* 0x00000200 */
/* PERI_CON2 */
#define GRF_SYS_PERI_CON2_OFFSET                           (0x8U)
#define GRF_SYS_PERI_CON2_WDT_NS_GLB_RESET_EN_SHIFT        (0U)
#define GRF_SYS_PERI_CON2_WDT_NS_GLB_RESET_EN_MASK         (0x1U << GRF_SYS_PERI_CON2_WDT_NS_GLB_RESET_EN_SHIFT)        /* 0x00000001 */
#define GRF_SYS_PERI_CON2_WDT_NS_PAUSE_EN_SHIFT            (1U)
#define GRF_SYS_PERI_CON2_WDT_NS_PAUSE_EN_MASK             (0x1U << GRF_SYS_PERI_CON2_WDT_NS_PAUSE_EN_SHIFT)            /* 0x00000002 */
#define GRF_SYS_PERI_CON2_ACLK_CRYPTO_EXGATE_SHIFT         (4U)
#define GRF_SYS_PERI_CON2_ACLK_CRYPTO_EXGATE_MASK          (0x1U << GRF_SYS_PERI_CON2_ACLK_CRYPTO_EXGATE_SHIFT)         /* 0x00000010 */
#define GRF_SYS_PERI_CON2_HCLK_RK_RNG_EXGATE_SHIFT         (5U)
#define GRF_SYS_PERI_CON2_HCLK_RK_RNG_EXGATE_MASK          (0x1U << GRF_SYS_PERI_CON2_HCLK_RK_RNG_EXGATE_SHIFT)         /* 0x00000020 */
#define GRF_SYS_PERI_CON2_CLK_CORE_CRYPTO_EXGATE_SHIFT     (6U)
#define GRF_SYS_PERI_CON2_CLK_CORE_CRYPTO_EXGATE_MASK      (0x1U << GRF_SYS_PERI_CON2_CLK_CORE_CRYPTO_EXGATE_SHIFT)     /* 0x00000040 */
#define GRF_SYS_PERI_CON2_CLK_PKA_CRYPTO_EXGATE_SHIFT      (7U)
#define GRF_SYS_PERI_CON2_CLK_PKA_CRYPTO_EXGATE_MASK       (0x1U << GRF_SYS_PERI_CON2_CLK_PKA_CRYPTO_EXGATE_SHIFT)      /* 0x00000080 */
#define GRF_SYS_PERI_CON2_I2C4_DMA_EN_SHIFT                (8U)
#define GRF_SYS_PERI_CON2_I2C4_DMA_EN_MASK                 (0x1U << GRF_SYS_PERI_CON2_I2C4_DMA_EN_SHIFT)                /* 0x00000100 */
/* PERI_HPROT2_CON */
#define GRF_SYS_PERI_HPROT2_CON_OFFSET                     (0xCU)
#define GRF_SYS_PERI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_HPMCU_SHIFT (0U)
#define GRF_SYS_PERI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_HPMCU_MASK (0x1U << GRF_SYS_PERI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_HPMCU_SHIFT) /* 0x00000001 */
#define GRF_SYS_PERI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_SDIO_SHIFT (1U)
#define GRF_SYS_PERI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_SDIO_MASK (0x1U << GRF_SYS_PERI_HPROT2_CON_GRF_CON_BUFFERABLE_HPROT2_SDIO_SHIFT) /* 0x00000002 */
/* PERI_STATUS */
#define GRF_SYS_PERI_STATUS_OFFSET                         (0x10U)
#define GRF_SYS_PERI_STATUS_TIMER0_EN_STATUS_SHIFT         (0U)
#define GRF_SYS_PERI_STATUS_TIMER0_EN_STATUS_MASK          (0x1U << GRF_SYS_PERI_STATUS_TIMER0_EN_STATUS_SHIFT)         /* 0x00000001 */
#define GRF_SYS_PERI_STATUS_TIMER1_EN_STATUS_SHIFT         (1U)
#define GRF_SYS_PERI_STATUS_TIMER1_EN_STATUS_MASK          (0x1U << GRF_SYS_PERI_STATUS_TIMER1_EN_STATUS_SHIFT)         /* 0x00000002 */
#define GRF_SYS_PERI_STATUS_TIMER2_EN_STATUS_SHIFT         (2U)
#define GRF_SYS_PERI_STATUS_TIMER2_EN_STATUS_MASK          (0x1U << GRF_SYS_PERI_STATUS_TIMER2_EN_STATUS_SHIFT)         /* 0x00000004 */
#define GRF_SYS_PERI_STATUS_TIMER3_EN_STATUS_SHIFT         (3U)
#define GRF_SYS_PERI_STATUS_TIMER3_EN_STATUS_MASK          (0x1U << GRF_SYS_PERI_STATUS_TIMER3_EN_STATUS_SHIFT)         /* 0x00000008 */
#define GRF_SYS_PERI_STATUS_TIMER4_EN_STATUS_SHIFT         (4U)
#define GRF_SYS_PERI_STATUS_TIMER4_EN_STATUS_MASK          (0x1U << GRF_SYS_PERI_STATUS_TIMER4_EN_STATUS_SHIFT)         /* 0x00000010 */
#define GRF_SYS_PERI_STATUS_TIMER5_EN_STATUS_SHIFT         (5U)
#define GRF_SYS_PERI_STATUS_TIMER5_EN_STATUS_MASK          (0x1U << GRF_SYS_PERI_STATUS_TIMER5_EN_STATUS_SHIFT)         /* 0x00000020 */
#define GRF_SYS_PERI_STATUS_STIMER0_EN_STATUS_SHIFT        (6U)
#define GRF_SYS_PERI_STATUS_STIMER0_EN_STATUS_MASK         (0x1U << GRF_SYS_PERI_STATUS_STIMER0_EN_STATUS_SHIFT)        /* 0x00000040 */
#define GRF_SYS_PERI_STATUS_STIMER1_EN_STATUS_SHIFT        (7U)
#define GRF_SYS_PERI_STATUS_STIMER1_EN_STATUS_MASK         (0x1U << GRF_SYS_PERI_STATUS_STIMER1_EN_STATUS_SHIFT)        /* 0x00000080 */
#define GRF_SYS_PERI_STATUS_RTC_DIG_LDO_RDY_SHIFT          (8U)
#define GRF_SYS_PERI_STATUS_RTC_DIG_LDO_RDY_MASK           (0x1U << GRF_SYS_PERI_STATUS_RTC_DIG_LDO_RDY_SHIFT)          /* 0x00000100 */
#define GRF_SYS_PERI_STATUS_RTC_EXT_OFF_O_SHIFT            (9U)
#define GRF_SYS_PERI_STATUS_RTC_EXT_OFF_O_MASK             (0x1U << GRF_SYS_PERI_STATUS_RTC_EXT_OFF_O_SHIFT)            /* 0x00000200 */
#define GRF_SYS_PERI_STATUS_ACODEC_ADC_MASTER_EN_SHIFT     (20U)
#define GRF_SYS_PERI_STATUS_ACODEC_ADC_MASTER_EN_MASK      (0x1U << GRF_SYS_PERI_STATUS_ACODEC_ADC_MASTER_EN_SHIFT)     /* 0x00100000 */
#define GRF_SYS_PERI_STATUS_ACODEC_DAC_MASTER_EN_SHIFT     (21U)
#define GRF_SYS_PERI_STATUS_ACODEC_DAC_MASTER_EN_MASK      (0x1U << GRF_SYS_PERI_STATUS_ACODEC_DAC_MASTER_EN_SHIFT)     /* 0x00200000 */
#define GRF_SYS_PERI_STATUS_TOPCNS_SBPI_BUSY_SHIFT         (22U)
#define GRF_SYS_PERI_STATUS_TOPCNS_SBPI_BUSY_MASK          (0x1U << GRF_SYS_PERI_STATUS_TOPCNS_SBPI_BUSY_SHIFT)         /* 0x00400000 */
#define GRF_SYS_PERI_STATUS_TOPCNS_USER_BUSY_SHIFT         (23U)
#define GRF_SYS_PERI_STATUS_TOPCNS_USER_BUSY_MASK          (0x1U << GRF_SYS_PERI_STATUS_TOPCNS_USER_BUSY_SHIFT)         /* 0x00800000 */
#define GRF_SYS_PERI_STATUS_TOPCS_SBPI_BUSY_SHIFT          (24U)
#define GRF_SYS_PERI_STATUS_TOPCS_SBPI_BUSY_MASK           (0x1U << GRF_SYS_PERI_STATUS_TOPCS_SBPI_BUSY_SHIFT)          /* 0x01000000 */
#define GRF_SYS_PERI_STATUS_TOPCS_USER_BUSY_SHIFT          (25U)
#define GRF_SYS_PERI_STATUS_TOPCS_USER_BUSY_MASK           (0x1U << GRF_SYS_PERI_STATUS_TOPCS_USER_BUSY_SHIFT)          /* 0x02000000 */
/* AUDIO_CON0 */
#define GRF_SYS_AUDIO_CON0_OFFSET                          (0x20U)
#define GRF_SYS_AUDIO_CON0_GRF_SAIEN_SCLKLRCK_SOURCE_CTRL_SHIFT (0U)
#define GRF_SYS_AUDIO_CON0_GRF_SAIEN_SCLKLRCK_SOURCE_CTRL_MASK (0x7U << GRF_SYS_AUDIO_CON0_GRF_SAIEN_SCLKLRCK_SOURCE_CTRL_SHIFT) /* 0x00000007 */
#define GRF_SYS_AUDIO_CON0_GRF_SAIEN_SDI0_SOURCE_CTRL_SHIFT (3U)
#define GRF_SYS_AUDIO_CON0_GRF_SAIEN_SDI0_SOURCE_CTRL_MASK (0x3U << GRF_SYS_AUDIO_CON0_GRF_SAIEN_SDI0_SOURCE_CTRL_SHIFT) /* 0x00000018 */
#define GRF_SYS_AUDIO_CON0_GRF_ADCEN_SCLKLRCK_SOURCE_CTRL_SHIFT (5U)
#define GRF_SYS_AUDIO_CON0_GRF_ADCEN_SCLKLRCK_SOURCE_CTRL_MASK (0x3U << GRF_SYS_AUDIO_CON0_GRF_ADCEN_SCLKLRCK_SOURCE_CTRL_SHIFT) /* 0x00000060 */
#define GRF_SYS_AUDIO_CON0_GRF_DACEN_SCLKLRCK_SOURCE_CTRL_SHIFT (7U)
#define GRF_SYS_AUDIO_CON0_GRF_DACEN_SCLKLRCK_SOURCE_CTRL_MASK (0x3U << GRF_SYS_AUDIO_CON0_GRF_DACEN_SCLKLRCK_SOURCE_CTRL_SHIFT) /* 0x00000180 */
#define GRF_SYS_AUDIO_CON0_GRF_DACEN_SDO0_SOURCE_CTRL_SHIFT (9U)
#define GRF_SYS_AUDIO_CON0_GRF_DACEN_SDO0_SOURCE_CTRL_MASK (0x3U << GRF_SYS_AUDIO_CON0_GRF_DACEN_SDO0_SOURCE_CTRL_SHIFT) /* 0x00000600 */
/* AUDIO_CON1 */
#define GRF_SYS_AUDIO_CON1_OFFSET                          (0x24U)
#define GRF_SYS_AUDIO_CON1_GRF_IOEN_SCLKLRCK_SOURCE_CTRL_SHIFT (0U)
#define GRF_SYS_AUDIO_CON1_GRF_IOEN_SCLKLRCK_SOURCE_CTRL_MASK (0x7U << GRF_SYS_AUDIO_CON1_GRF_IOEN_SCLKLRCK_SOURCE_CTRL_SHIFT) /* 0x00000007 */
#define GRF_SYS_AUDIO_CON1_GRF_FORCE_IOEN_SCLKLRCK_SHIFT   (3U)
#define GRF_SYS_AUDIO_CON1_GRF_FORCE_IOEN_SCLKLRCK_MASK    (0x1U << GRF_SYS_AUDIO_CON1_GRF_FORCE_IOEN_SCLKLRCK_SHIFT)   /* 0x00000008 */
#define GRF_SYS_AUDIO_CON1_GRF_FORCE_VALUE_SCLKLRCK_SHIFT  (4U)
#define GRF_SYS_AUDIO_CON1_GRF_FORCE_VALUE_SCLKLRCK_MASK   (0x1U << GRF_SYS_AUDIO_CON1_GRF_FORCE_VALUE_SCLKLRCK_SHIFT)  /* 0x00000010 */
#define GRF_SYS_AUDIO_CON1_GRF_IOEN_SDO0_SOURCE_CTRL_SHIFT (5U)
#define GRF_SYS_AUDIO_CON1_GRF_IOEN_SDO0_SOURCE_CTRL_MASK  (0x3U << GRF_SYS_AUDIO_CON1_GRF_IOEN_SDO0_SOURCE_CTRL_SHIFT) /* 0x00000060 */
#define GRF_SYS_AUDIO_CON1_GRF_CON_ACODEC_AD2DA_LOOP_SHIFT (7U)
#define GRF_SYS_AUDIO_CON1_GRF_CON_ACODEC_AD2DA_LOOP_MASK  (0x1U << GRF_SYS_AUDIO_CON1_GRF_CON_ACODEC_AD2DA_LOOP_SHIFT) /* 0x00000080 */
#define GRF_SYS_AUDIO_CON1_GRF_CON_ACODEC_RX_SCLK_LRCK_OUT_EN_SHIFT (8U)
#define GRF_SYS_AUDIO_CON1_GRF_CON_ACODEC_RX_SCLK_LRCK_OUT_EN_MASK (0x1U << GRF_SYS_AUDIO_CON1_GRF_CON_ACODEC_RX_SCLK_LRCK_OUT_EN_SHIFT) /* 0x00000100 */
/* USBOTG_CON0 */
#define GRF_SYS_USBOTG_CON0_OFFSET                         (0x30U)
#define GRF_SYS_USBOTG_CON0_USBOTG_BUS_FILTER_BYPASS_SHIFT (0U)
#define GRF_SYS_USBOTG_CON0_USBOTG_BUS_FILTER_BYPASS_MASK  (0xFU << GRF_SYS_USBOTG_CON0_USBOTG_BUS_FILTER_BYPASS_SHIFT) /* 0x0000000F */
#define GRF_SYS_USBOTG_CON0_USBOTG_HUB_PORT_OVERCURRENT_SHIFT (4U)
#define GRF_SYS_USBOTG_CON0_USBOTG_HUB_PORT_OVERCURRENT_MASK (0x1U << GRF_SYS_USBOTG_CON0_USBOTG_HUB_PORT_OVERCURRENT_SHIFT) /* 0x00000010 */
#define GRF_SYS_USBOTG_CON0_USBOTG_HUB_PORT_PERM_ATTACH_SHIFT (6U)
#define GRF_SYS_USBOTG_CON0_USBOTG_HUB_PORT_PERM_ATTACH_MASK (0x1U << GRF_SYS_USBOTG_CON0_USBOTG_HUB_PORT_PERM_ATTACH_SHIFT) /* 0x00000040 */
#define GRF_SYS_USBOTG_CON0_USBOTG_FLADJ_30MHZ_REG_SHIFT   (8U)
#define GRF_SYS_USBOTG_CON0_USBOTG_FLADJ_30MHZ_REG_MASK    (0x3FU << GRF_SYS_USBOTG_CON0_USBOTG_FLADJ_30MHZ_REG_SHIFT)  /* 0x00003F00 */
#define GRF_SYS_USBOTG_CON0_USBOTG_HOST_PORT_POWER_CONTROL_PRESENT_SHIFT (14U)
#define GRF_SYS_USBOTG_CON0_USBOTG_HOST_PORT_POWER_CONTROL_PRESENT_MASK (0x1U << GRF_SYS_USBOTG_CON0_USBOTG_HOST_PORT_POWER_CONTROL_PRESENT_SHIFT) /* 0x00004000 */
#define GRF_SYS_USBOTG_CON0_USBOTG_HOST_U2_PORT_DISABLE_SHIFT (15U)
#define GRF_SYS_USBOTG_CON0_USBOTG_HOST_U2_PORT_DISABLE_MASK (0x1U << GRF_SYS_USBOTG_CON0_USBOTG_HOST_U2_PORT_DISABLE_SHIFT) /* 0x00008000 */
/* USBOTG_CON1 */
#define GRF_SYS_USBOTG_CON1_OFFSET                         (0x34U)
#define GRF_SYS_USBOTG_CON1_USBOTG_PME_EN_SHIFT            (0U)
#define GRF_SYS_USBOTG_CON1_USBOTG_PME_EN_MASK             (0x1U << GRF_SYS_USBOTG_CON1_USBOTG_PME_EN_SHIFT)            /* 0x00000001 */
#define GRF_SYS_USBOTG_CON1_USBOTG_HOST_LEGACY_SMI_PCI_CMD_SHIFT (4U)
#define GRF_SYS_USBOTG_CON1_USBOTG_HOST_LEGACY_SMI_PCI_CMD_MASK (0x1U << GRF_SYS_USBOTG_CON1_USBOTG_HOST_LEGACY_SMI_PCI_CMD_SHIFT) /* 0x00000010 */
#define GRF_SYS_USBOTG_CON1_USBOTG_HOST_LEGACY_SMI_BAR_SHIFT (5U)
#define GRF_SYS_USBOTG_CON1_USBOTG_HOST_LEGACY_SMI_BAR_MASK (0x1U << GRF_SYS_USBOTG_CON1_USBOTG_HOST_LEGACY_SMI_BAR_SHIFT) /* 0x00000020 */
#define GRF_SYS_USBOTG_CON1_USBOTG_HOST_NUM_U2_PORT_SHIFT  (8U)
#define GRF_SYS_USBOTG_CON1_USBOTG_HOST_NUM_U2_PORT_MASK   (0xFU << GRF_SYS_USBOTG_CON1_USBOTG_HOST_NUM_U2_PORT_SHIFT)  /* 0x00000F00 */
/* USBOTG_STATUS0 */
#define GRF_SYS_USBOTG_STATUS0_OFFSET                      (0x40U)
#define GRF_SYS_USBOTG_STATUS0                             (0x0U)
#define GRF_SYS_USBOTG_STATUS0_USBOTG_LOGIC_ANALYZER_TRACE_31_0_SHIFT (0U)
#define GRF_SYS_USBOTG_STATUS0_USBOTG_LOGIC_ANALYZER_TRACE_31_0_MASK (0xFFFFFFFFU << GRF_SYS_USBOTG_STATUS0_USBOTG_LOGIC_ANALYZER_TRACE_31_0_SHIFT) /* 0xFFFFFFFF */
/* USBOTG_STATUS1 */
#define GRF_SYS_USBOTG_STATUS1_OFFSET                      (0x44U)
#define GRF_SYS_USBOTG_STATUS1                             (0x0U)
#define GRF_SYS_USBOTG_STATUS1_USBOTG_LOGIC_ANALYZER_TRACE_63_32_SHIFT (0U)
#define GRF_SYS_USBOTG_STATUS1_USBOTG_LOGIC_ANALYZER_TRACE_63_32_MASK (0xFFFFFFFFU << GRF_SYS_USBOTG_STATUS1_USBOTG_LOGIC_ANALYZER_TRACE_63_32_SHIFT) /* 0xFFFFFFFF */
/* USBOTG_STATUS2 */
#define GRF_SYS_USBOTG_STATUS2_OFFSET                      (0x48U)
#define GRF_SYS_USBOTG_STATUS2                             (0x0U)
#define GRF_SYS_USBOTG_STATUS2_USBOTG_HOST_CURRENT_BELT_SHIFT (0U)
#define GRF_SYS_USBOTG_STATUS2_USBOTG_HOST_CURRENT_BELT_MASK (0xFFFU << GRF_SYS_USBOTG_STATUS2_USBOTG_HOST_CURRENT_BELT_SHIFT) /* 0x00000FFF */
/* USBPHY_CON0 */
#define GRF_SYS_USBPHY_CON0_OFFSET                         (0x50U)
#define GRF_SYS_USBPHY_CON0_USBPHY_SW_EN_SHIFT             (0U)
#define GRF_SYS_USBPHY_CON0_USBPHY_SW_EN_MASK              (0x1U << GRF_SYS_USBPHY_CON0_USBPHY_SW_EN_SHIFT)             /* 0x00000001 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_SUSPEND_N_SHIFT    (1U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_SUSPEND_N_MASK     (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_SUSPEND_N_SHIFT)    /* 0x00000002 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_OPMODE_SHIFT       (2U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_OPMODE_MASK        (0x3U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_OPMODE_SHIFT)       /* 0x0000000C */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_XCVRSELECT_SHIFT   (4U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_XCVRSELECT_MASK    (0x3U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_XCVRSELECT_SHIFT)   /* 0x00000030 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_TERMSELECT_SHIFT   (6U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_TERMSELECT_MASK    (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_TERMSELECT_SHIFT)   /* 0x00000040 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DPPULLDOWN_SHIFT   (7U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DPPULLDOWN_MASK    (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DPPULLDOWN_SHIFT)   /* 0x00000080 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DMPULLDOWN_SHIFT   (8U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DMPULLDOWN_MASK    (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DMPULLDOWN_SHIFT)   /* 0x00000100 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDDIG_SEL_SHIFT    (9U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDDIG_SEL_MASK     (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDDIG_SEL_SHIFT)    /* 0x00000200 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDDIG_SHIFT        (10U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDDIG_MASK         (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDDIG_SHIFT)        /* 0x00000400 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDPULLUP_SHIFT     (11U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDPULLUP_MASK      (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_IDPULLUP_SHIFT)     /* 0x00000800 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_CHRGVBUS_SHIFT     (12U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_CHRGVBUS_MASK      (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_CHRGVBUS_SHIFT)     /* 0x00001000 */
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DISCHRGVBUS_SHIFT  (13U)
#define GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DISCHRGVBUS_MASK   (0x1U << GRF_SYS_USBPHY_CON0_USBOTG_UTMI_DISCHRGVBUS_SHIFT)  /* 0x00002000 */
/* USBPHY_CON1 */
#define GRF_SYS_USBPHY_CON1_OFFSET                         (0x54U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_SUSPEND_N_SEL_SHIFT (0U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_SUSPEND_N_SEL_MASK (0x3U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_SUSPEND_N_SEL_SHIFT) /* 0x00000003 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_SUSPEND_N_SHIFT    (2U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_SUSPEND_N_MASK     (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_SUSPEND_N_SHIFT)    /* 0x00000004 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_XVER_OWN_SHIFT  (6U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_XVER_OWN_MASK   (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_XVER_OWN_SHIFT)  /* 0x00000040 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_OE_SHIFT        (7U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_OE_MASK         (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_OE_SHIFT)        /* 0x00000080 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_DATA_SHIFT      (8U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_DATA_MASK       (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_DATA_SHIFT)      /* 0x00000100 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_SE0_SHIFT       (9U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_SE0_MASK        (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_FS_SE0_SHIFT)       /* 0x00000200 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_DRVVBUS_SEL_SHIFT  (10U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_DRVVBUS_SEL_MASK   (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_DRVVBUS_SEL_SHIFT)  /* 0x00000400 */
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_DRVVBUS_SHIFT      (11U)
#define GRF_SYS_USBPHY_CON1_USBOTG_UTMI_DRVVBUS_MASK       (0x1U << GRF_SYS_USBPHY_CON1_USBOTG_UTMI_DRVVBUS_SHIFT)      /* 0x00000800 */
/* USBPHY_CON2 */
#define GRF_SYS_USBPHY_CON2_OFFSET                         (0x58U)
#define GRF_SYS_USBPHY_CON2_USBPHY_IDP_SINK_EN_SHIFT       (7U)
#define GRF_SYS_USBPHY_CON2_USBPHY_IDP_SINK_EN_MASK        (0x1U << GRF_SYS_USBPHY_CON2_USBPHY_IDP_SINK_EN_SHIFT)       /* 0x00000080 */
#define GRF_SYS_USBPHY_CON2_USBPHY_IDM_SINK_EN_SHIFT       (8U)
#define GRF_SYS_USBPHY_CON2_USBPHY_IDM_SINK_EN_MASK        (0x1U << GRF_SYS_USBPHY_CON2_USBPHY_IDM_SINK_EN_SHIFT)       /* 0x00000100 */
#define GRF_SYS_USBPHY_CON2_USBPHY_IDP_SRC_EN_SHIFT        (9U)
#define GRF_SYS_USBPHY_CON2_USBPHY_IDP_SRC_EN_MASK         (0x1U << GRF_SYS_USBPHY_CON2_USBPHY_IDP_SRC_EN_SHIFT)        /* 0x00000200 */
#define GRF_SYS_USBPHY_CON2_USBPHY_RDM_PDWN_EN_SHIFT       (10U)
#define GRF_SYS_USBPHY_CON2_USBPHY_RDM_PDWN_EN_MASK        (0x1U << GRF_SYS_USBPHY_CON2_USBPHY_RDM_PDWN_EN_SHIFT)       /* 0x00000400 */
#define GRF_SYS_USBPHY_CON2_USBPHY_VDP_SRC_EN_SHIFT        (11U)
#define GRF_SYS_USBPHY_CON2_USBPHY_VDP_SRC_EN_MASK         (0x1U << GRF_SYS_USBPHY_CON2_USBPHY_VDP_SRC_EN_SHIFT)        /* 0x00000800 */
#define GRF_SYS_USBPHY_CON2_USBPHY_VDM_SRC_EN_SHIFT        (12U)
#define GRF_SYS_USBPHY_CON2_USBPHY_VDM_SRC_EN_MASK         (0x1U << GRF_SYS_USBPHY_CON2_USBPHY_VDM_SRC_EN_SHIFT)        /* 0x00001000 */
#define GRF_SYS_USBPHY_CON2_USBOTG_UTMI_BVALID_SHIFT       (14U)
#define GRF_SYS_USBPHY_CON2_USBOTG_UTMI_BVALID_MASK        (0x1U << GRF_SYS_USBPHY_CON2_USBOTG_UTMI_BVALID_SHIFT)       /* 0x00004000 */
#define GRF_SYS_USBPHY_CON2_USBOTG_UTMI_BVALID_SEL_SHIFT   (15U)
#define GRF_SYS_USBPHY_CON2_USBOTG_UTMI_BVALID_SEL_MASK    (0x1U << GRF_SYS_USBPHY_CON2_USBOTG_UTMI_BVALID_SEL_SHIFT)   /* 0x00008000 */
/* USBPHY_CON3 */
#define GRF_SYS_USBPHY_CON3_OFFSET                         (0x5CU)
#define GRF_SYS_USBPHY_CON3_USBPHY_CHARGE_DET_BYP_SHIFT    (0U)
#define GRF_SYS_USBPHY_CON3_USBPHY_CHARGE_DET_BYP_MASK     (0x1U << GRF_SYS_USBPHY_CON3_USBPHY_CHARGE_DET_BYP_SHIFT)    /* 0x00000001 */
#define GRF_SYS_USBPHY_CON3_USBPHY_FORCE_DP_ATTACHED_SHIFT (1U)
#define GRF_SYS_USBPHY_CON3_USBPHY_FORCE_DP_ATTACHED_MASK  (0x1U << GRF_SYS_USBPHY_CON3_USBPHY_FORCE_DP_ATTACHED_SHIFT) /* 0x00000002 */
#define GRF_SYS_USBPHY_CON3_USBPHY_FORCE_CP_DET_SHIFT      (2U)
#define GRF_SYS_USBPHY_CON3_USBPHY_FORCE_CP_DET_MASK       (0x1U << GRF_SYS_USBPHY_CON3_USBPHY_FORCE_CP_DET_SHIFT)      /* 0x00000004 */
#define GRF_SYS_USBPHY_CON3_USBPHY_FORCE_DCP_DET_SHIFT     (3U)
#define GRF_SYS_USBPHY_CON3_USBPHY_FORCE_DCP_DET_MASK      (0x1U << GRF_SYS_USBPHY_CON3_USBPHY_FORCE_DCP_DET_SHIFT)     /* 0x00000008 */
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLUP_EN_SHIFT  (4U)
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLUP_EN_MASK   (0x1U << GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLUP_EN_SHIFT)  /* 0x00000010 */
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLDOWN_EN_SHIFT (5U)
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLDOWN_EN_MASK (0x1U << GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLDOWN_EN_SHIFT) /* 0x00000020 */
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLUP_TUNE_SHIFT (6U)
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLUP_TUNE_MASK (0x3U << GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLUP_TUNE_SHIFT) /* 0x000000C0 */
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLDOWN_TUNE_SHIFT (8U)
#define GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLDOWN_TUNE_MASK (0x3U << GRF_SYS_USBPHY_CON3_USBPHY_BC_WEAKPULLDOWN_TUNE_SHIFT) /* 0x00000300 */
/* USBPHY_STATUS */
#define GRF_SYS_USBPHY_STATUS_OFFSET                       (0x60U)
#define GRF_SYS_USBPHY_STATUS                              (0x0U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VPI_SHIFT        (0U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VPI_MASK         (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VPI_SHIFT)        /* 0x00000001 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VMI_SHIFT        (1U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VMI_MASK         (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VMI_SHIFT)        /* 0x00000002 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VBUSVALID_SHIFT  (2U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VBUSVALID_MASK   (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_VBUSVALID_SHIFT)  /* 0x00000004 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_SESSEND_SHIFT    (3U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_SESSEND_MASK     (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_SESSEND_SHIFT)    /* 0x00000008 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_LINESTATE_SHIFT  (4U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_LINESTATE_MASK   (0x3U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_LINESTATE_SHIFT)  /* 0x00000030 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_IDDIG_SHIFT      (6U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_IDDIG_MASK       (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_IDDIG_SHIFT)      /* 0x00000040 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_HOSTDISCONNECT_SHIFT (7U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_HOSTDISCONNECT_MASK (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_HOSTDISCONNECT_SHIFT) /* 0x00000080 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_FS_XVER_OWN_SHIFT (8U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_FS_XVER_OWN_MASK (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_FS_XVER_OWN_SHIFT) /* 0x00000100 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_BVALID_SHIFT     (9U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_BVALID_MASK      (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_BVALID_SHIFT)     /* 0x00000200 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_AVALID_SHIFT     (10U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_AVALID_MASK      (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_UTMI_AVALID_SHIFT)     /* 0x00000400 */
#define GRF_SYS_USBPHY_STATUS_USBOTG_PHY_LS_FS_RCV_SHIFT   (11U)
#define GRF_SYS_USBPHY_STATUS_USBOTG_PHY_LS_FS_RCV_MASK    (0x1U << GRF_SYS_USBPHY_STATUS_USBOTG_PHY_LS_FS_RCV_SHIFT)   /* 0x00000800 */
#define GRF_SYS_USBPHY_STATUS_USBPHY_DCP_DETECTED_SHIFT    (12U)
#define GRF_SYS_USBPHY_STATUS_USBPHY_DCP_DETECTED_MASK     (0x1U << GRF_SYS_USBPHY_STATUS_USBPHY_DCP_DETECTED_SHIFT)    /* 0x00001000 */
#define GRF_SYS_USBPHY_STATUS_USBPHY_CP_DETECTED_SHIFT     (13U)
#define GRF_SYS_USBPHY_STATUS_USBPHY_CP_DETECTED_MASK      (0x1U << GRF_SYS_USBPHY_STATUS_USBPHY_CP_DETECTED_SHIFT)     /* 0x00002000 */
#define GRF_SYS_USBPHY_STATUS_USBPHY_DP_ATTACHED_SHIFT     (14U)
#define GRF_SYS_USBPHY_STATUS_USBPHY_DP_ATTACHED_MASK      (0x1U << GRF_SYS_USBPHY_STATUS_USBPHY_DP_ATTACHED_SHIFT)     /* 0x00004000 */
#define GRF_SYS_USBPHY_STATUS_USBPHY_DM_FLOATDET_SHIFT     (15U)
#define GRF_SYS_USBPHY_STATUS_USBPHY_DM_FLOATDET_MASK      (0x1U << GRF_SYS_USBPHY_STATUS_USBPHY_DM_FLOATDET_SHIFT)     /* 0x00008000 */
#define GRF_SYS_USBPHY_STATUS_USBPHY_DP_FLOATDET_SHIFT     (16U)
#define GRF_SYS_USBPHY_STATUS_USBPHY_DP_FLOATDET_MASK      (0x1U << GRF_SYS_USBPHY_STATUS_USBPHY_DP_FLOATDET_SHIFT)     /* 0x00010000 */
/* SARADC_CON */
#define GRF_SYS_SARADC_CON_OFFSET                          (0x70U)
#define GRF_SYS_SARADC_CON_SARADC_ANA_REG_SHIFT            (0U)
#define GRF_SYS_SARADC_CON_SARADC_ANA_REG_MASK             (0xFFFFU << GRF_SYS_SARADC_CON_SARADC_ANA_REG_SHIFT)         /* 0x0000FFFF */
/* TSADC_CON */
#define GRF_SYS_TSADC_CON_OFFSET                           (0x74U)
#define GRF_SYS_TSADC_CON_TSADC_ANA_REG_SHIFT              (0U)
#define GRF_SYS_TSADC_CON_TSADC_ANA_REG_MASK               (0xFFU << GRF_SYS_TSADC_CON_TSADC_ANA_REG_SHIFT)             /* 0x000000FF */
#define GRF_SYS_TSADC_CON_TSADC_TSEN_EN_SHIFT              (8U)
#define GRF_SYS_TSADC_CON_TSADC_TSEN_EN_MASK               (0x1U << GRF_SYS_TSADC_CON_TSADC_TSEN_EN_SHIFT)              /* 0x00000100 */
#define GRF_SYS_TSADC_CON_TSADC_PDZ_SHIFT                  (9U)
#define GRF_SYS_TSADC_CON_TSADC_PDZ_MASK                   (0x1U << GRF_SYS_TSADC_CON_TSADC_PDZ_SHIFT)                  /* 0x00000200 */
#define GRF_SYS_TSADC_CON_TSADC_PDZ_SEL_SHIFT              (10U)
#define GRF_SYS_TSADC_CON_TSADC_PDZ_SEL_MASK               (0x1U << GRF_SYS_TSADC_CON_TSADC_PDZ_SEL_SHIFT)              /* 0x00000400 */
/* OTP_CON */
#define GRF_SYS_OTP_CON_OFFSET                             (0x78U)
#define GRF_SYS_OTP_CON_OTPC_NS_LOCK_SHIFT                 (0U)
#define GRF_SYS_OTP_CON_OTPC_NS_LOCK_MASK                  (0x1U << GRF_SYS_OTP_CON_OTPC_NS_LOCK_SHIFT)                 /* 0x00000001 */
#define GRF_SYS_OTP_CON_OTPC_S_LOCK_SHIFT                  (1U)
#define GRF_SYS_OTP_CON_OTPC_S_LOCK_MASK                   (0x1U << GRF_SYS_OTP_CON_OTPC_S_LOCK_SHIFT)                  /* 0x00000002 */
/* MEM_CON_SPRA */
#define GRF_SYS_MEM_CON_SPRA_OFFSET                        (0x80U)
#define GRF_SYS_MEM_CON_SPRA_RTSEL_SVT_SHIFT               (0U)
#define GRF_SYS_MEM_CON_SPRA_RTSEL_SVT_MASK                (0x3U << GRF_SYS_MEM_CON_SPRA_RTSEL_SVT_SHIFT)               /* 0x00000003 */
#define GRF_SYS_MEM_CON_SPRA_WTSEL_SVT_SHIFT               (2U)
#define GRF_SYS_MEM_CON_SPRA_WTSEL_SVT_MASK                (0x3U << GRF_SYS_MEM_CON_SPRA_WTSEL_SVT_SHIFT)               /* 0x0000000C */
#define GRF_SYS_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT    (4U)
#define GRF_SYS_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_MASK     (0x3U << GRF_SYS_MEM_CON_SPRA_RTSEL_HVT_MUX1ORMUX2_SHIFT)    /* 0x00000030 */
#define GRF_SYS_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT    (6U)
#define GRF_SYS_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_MASK     (0x3U << GRF_SYS_MEM_CON_SPRA_WTSEL_HVT_MUX1ORMUX2_SHIFT)    /* 0x000000C0 */
#define GRF_SYS_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT          (8U)
#define GRF_SYS_MEM_CON_SPRA_RTSEL_HVT_MUX4_MASK           (0x3U << GRF_SYS_MEM_CON_SPRA_RTSEL_HVT_MUX4_SHIFT)          /* 0x00000300 */
#define GRF_SYS_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT          (10U)
#define GRF_SYS_MEM_CON_SPRA_WTSEL_HVT_MUX4_MASK           (0x3U << GRF_SYS_MEM_CON_SPRA_WTSEL_HVT_MUX4_SHIFT)          /* 0x00000C00 */
/* MEM_CON_DPRA */
#define GRF_SYS_MEM_CON_DPRA_OFFSET                        (0x84U)
#define GRF_SYS_MEM_CON_DPRA_PTSEL_SHIFT                   (0U)
#define GRF_SYS_MEM_CON_DPRA_PTSEL_MASK                    (0x3U << GRF_SYS_MEM_CON_DPRA_PTSEL_SHIFT)                   /* 0x00000003 */
#define GRF_SYS_MEM_CON_DPRA_WTSEL_SHIFT                   (2U)
#define GRF_SYS_MEM_CON_DPRA_WTSEL_MASK                    (0x3U << GRF_SYS_MEM_CON_DPRA_WTSEL_SHIFT)                   /* 0x0000000C */
#define GRF_SYS_MEM_CON_DPRA_RTSEL_SHIFT                   (4U)
#define GRF_SYS_MEM_CON_DPRA_RTSEL_MASK                    (0x3U << GRF_SYS_MEM_CON_DPRA_RTSEL_SHIFT)                   /* 0x00000030 */
/* MEM_CON_ROM */
#define GRF_SYS_MEM_CON_ROM_OFFSET                         (0x88U)
#define GRF_SYS_MEM_CON_ROM_PERI_MEM_CFG_ROM_SHIFT         (0U)
#define GRF_SYS_MEM_CON_ROM_PERI_MEM_CFG_ROM_MASK          (0x3FU << GRF_SYS_MEM_CON_ROM_PERI_MEM_CFG_ROM_SHIFT)        /* 0x0000003F */
/* MEM_CON_GATE */
#define GRF_SYS_MEM_CON_GATE_OFFSET                        (0x8CU)
#define GRF_SYS_MEM_CON_GATE_USBOTG_MEM_GAT_EN_SHIFT       (0U)
#define GRF_SYS_MEM_CON_GATE_USBOTG_MEM_GAT_EN_MASK        (0x1U << GRF_SYS_MEM_CON_GATE_USBOTG_MEM_GAT_EN_SHIFT)       /* 0x00000001 */
#define GRF_SYS_MEM_CON_GATE_CRYPTO_MEM_GAT_EN_SHIFT       (1U)
#define GRF_SYS_MEM_CON_GATE_CRYPTO_MEM_GAT_EN_MASK        (0x1U << GRF_SYS_MEM_CON_GATE_CRYPTO_MEM_GAT_EN_SHIFT)       /* 0x00000002 */
#define GRF_SYS_MEM_CON_GATE_ROM_MEM_GAT_EN_SHIFT          (2U)
#define GRF_SYS_MEM_CON_GATE_ROM_MEM_GAT_EN_MASK           (0x1U << GRF_SYS_MEM_CON_GATE_ROM_MEM_GAT_EN_SHIFT)          /* 0x00000004 */
#define GRF_SYS_MEM_CON_GATE_GMAC_MEM_GAT_EN_SHIFT         (3U)
#define GRF_SYS_MEM_CON_GATE_GMAC_MEM_GAT_EN_MASK          (0x1U << GRF_SYS_MEM_CON_GATE_GMAC_MEM_GAT_EN_SHIFT)         /* 0x00000008 */
#define GRF_SYS_MEM_CON_GATE_DECOM_MEM_GAT_EN_SHIFT        (4U)
#define GRF_SYS_MEM_CON_GATE_DECOM_MEM_GAT_EN_MASK         (0x1U << GRF_SYS_MEM_CON_GATE_DECOM_MEM_GAT_EN_SHIFT)        /* 0x00000010 */
#define GRF_SYS_MEM_CON_GATE_DMA2DDR_MEM_GAT_EN_SHIFT      (5U)
#define GRF_SYS_MEM_CON_GATE_DMA2DDR_MEM_GAT_EN_MASK       (0x1U << GRF_SYS_MEM_CON_GATE_DMA2DDR_MEM_GAT_EN_SHIFT)      /* 0x00000020 */
#define GRF_SYS_MEM_CON_GATE_RKCACHE_MEM_GAT_EN_SHIFT      (6U)
#define GRF_SYS_MEM_CON_GATE_RKCACHE_MEM_GAT_EN_MASK       (0x1U << GRF_SYS_MEM_CON_GATE_RKCACHE_MEM_GAT_EN_SHIFT)      /* 0x00000040 */
/* BIU_CON0 */
#define GRF_SYS_BIU_CON0_OFFSET                            (0x90U)
#define GRF_SYS_BIU_CON0_BIU_STALL_CTRL_SHIFT              (0U)
#define GRF_SYS_BIU_CON0_BIU_STALL_CTRL_MASK               (0x7FFFU << GRF_SYS_BIU_CON0_BIU_STALL_CTRL_SHIFT)           /* 0x00007FFF */
/* BIU_STATUS0 */
#define GRF_SYS_BIU_STATUS0_OFFSET                         (0x98U)
#define GRF_SYS_BIU_STATUS0                                (0x0U)
#define GRF_SYS_BIU_STATUS0_NOPENDINGTRANS_SHIFT           (0U)
#define GRF_SYS_BIU_STATUS0_NOPENDINGTRANS_MASK            (0x3FFFFFU << GRF_SYS_BIU_STATUS0_NOPENDINGTRANS_SHIFT)      /* 0x003FFFFF */
/* BIU_STATUS1 */
#define GRF_SYS_BIU_STATUS1_OFFSET                         (0x9CU)
#define GRF_SYS_BIU_STATUS1                                (0x0U)
#define GRF_SYS_BIU_STATUS1_ACTIVE_STATUS_SHIFT            (0U)
#define GRF_SYS_BIU_STATUS1_ACTIVE_STATUS_MASK             (0x7FFFU << GRF_SYS_BIU_STATUS1_ACTIVE_STATUS_SHIFT)         /* 0x00007FFF */
/* GMAC_CON0 */
#define GRF_SYS_GMAC_CON0_OFFSET                           (0xA0U)
#define GRF_SYS_GMAC_CON0_SBD_FLOWCTRL_SHIFT               (0U)
#define GRF_SYS_GMAC_CON0_SBD_FLOWCTRL_MASK                (0x1U << GRF_SYS_GMAC_CON0_SBD_FLOWCTRL_SHIFT)               /* 0x00000001 */
/* GMAC_CLK_CON */
#define GRF_SYS_GMAC_CLK_CON_OFFSET                        (0xA4U)
#define GRF_SYS_GMAC_CLK_CON_RMII_MODE_SHIFT               (0U)
#define GRF_SYS_GMAC_CLK_CON_RMII_MODE_MASK                (0x1U << GRF_SYS_GMAC_CLK_CON_RMII_MODE_SHIFT)               /* 0x00000001 */
#define GRF_SYS_GMAC_CLK_CON_RMII_GATE_EN_SHIFT            (1U)
#define GRF_SYS_GMAC_CLK_CON_RMII_GATE_EN_MASK             (0x1U << GRF_SYS_GMAC_CLK_CON_RMII_GATE_EN_SHIFT)            /* 0x00000002 */
#define GRF_SYS_GMAC_CLK_CON_RMII_TXCLK_SEL_SHIFT          (2U)
#define GRF_SYS_GMAC_CLK_CON_RMII_TXCLK_SEL_MASK           (0x3U << GRF_SYS_GMAC_CLK_CON_RMII_TXCLK_SEL_SHIFT)          /* 0x0000000C */
#define GRF_SYS_GMAC_CLK_CON_CLK_GMAC_50_O_SEL_SHIFT       (4U)
#define GRF_SYS_GMAC_CLK_CON_CLK_GMAC_50_O_SEL_MASK        (0x1U << GRF_SYS_GMAC_CLK_CON_CLK_GMAC_50_O_SEL_SHIFT)       /* 0x00000010 */
/* GMAC_ST */
#define GRF_SYS_GMAC_ST_OFFSET                             (0xA8U)
#define GRF_SYS_GMAC_ST                                    (0x0U)
#define GRF_SYS_GMAC_ST_MAC_SPEED_O_SHIFT                  (0U)
#define GRF_SYS_GMAC_ST_MAC_SPEED_O_MASK                   (0x3U << GRF_SYS_GMAC_ST_MAC_SPEED_O_SHIFT)                  /* 0x00000003 */
#define GRF_SYS_GMAC_ST_MACPHY_LNK_LED_SHIFT               (2U)
#define GRF_SYS_GMAC_ST_MACPHY_LNK_LED_MASK                (0x1U << GRF_SYS_GMAC_ST_MACPHY_LNK_LED_SHIFT)               /* 0x00000004 */
#define GRF_SYS_GMAC_ST_MACPHY_SPD_LED_SHIFT               (3U)
#define GRF_SYS_GMAC_ST_MACPHY_SPD_LED_MASK                (0x1U << GRF_SYS_GMAC_ST_MACPHY_SPD_LED_SHIFT)               /* 0x00000008 */
#define GRF_SYS_GMAC_ST_MACPHY_DPX_LED_SHIFT               (4U)
#define GRF_SYS_GMAC_ST_MACPHY_DPX_LED_MASK                (0x1U << GRF_SYS_GMAC_ST_MACPHY_DPX_LED_SHIFT)               /* 0x00000010 */
/* MACPHY_CON0 */
#define GRF_SYS_MACPHY_CON0_OFFSET                         (0xB0U)
#define GRF_SYS_MACPHY_CON0_SMI_SEL_SHIFT                  (0U)
#define GRF_SYS_MACPHY_CON0_SMI_SEL_MASK                   (0x1U << GRF_SYS_MACPHY_CON0_SMI_SEL_SHIFT)                  /* 0x00000001 */
#define GRF_SYS_MACPHY_CON0_SHUTDOWN_SHIFT                 (1U)
#define GRF_SYS_MACPHY_CON0_SHUTDOWN_MASK                  (0x1U << GRF_SYS_MACPHY_CON0_SHUTDOWN_SHIFT)                 /* 0x00000002 */
#define GRF_SYS_MACPHY_CON0_MODE_SHIFT                     (2U)
#define GRF_SYS_MACPHY_CON0_MODE_MASK                      (0x3U << GRF_SYS_MACPHY_CON0_MODE_SHIFT)                     /* 0x0000000C */
#define GRF_SYS_MACPHY_CON0_FXEN_SHIFT                     (4U)
#define GRF_SYS_MACPHY_CON0_FXEN_MASK                      (0x1U << GRF_SYS_MACPHY_CON0_FXEN_SHIFT)                     /* 0x00000010 */
#define GRF_SYS_MACPHY_CON0_XMII_SEL_SHIFT                 (5U)
#define GRF_SYS_MACPHY_CON0_XMII_SEL_MASK                  (0x3U << GRF_SYS_MACPHY_CON0_XMII_SEL_SHIFT)                 /* 0x00000060 */
#define GRF_SYS_MACPHY_CON0_CLKSEL_SHIFT                   (7U)
#define GRF_SYS_MACPHY_CON0_CLKSEL_MASK                    (0x7U << GRF_SYS_MACPHY_CON0_CLKSEL_SHIFT)                   /* 0x00000380 */
#define GRF_SYS_MACPHY_CON0_ID_SHIFT                       (10U)
#define GRF_SYS_MACPHY_CON0_ID_MASK                        (0x1FU << GRF_SYS_MACPHY_CON0_ID_SHIFT)                      /* 0x00007C00 */
#define GRF_SYS_MACPHY_CON0_LED_POL_SHIFT                  (15U)
#define GRF_SYS_MACPHY_CON0_LED_POL_MASK                   (0x1U << GRF_SYS_MACPHY_CON0_LED_POL_SHIFT)                  /* 0x00008000 */
/* MACPHY_CON1 */
#define GRF_SYS_MACPHY_CON1_OFFSET                         (0xB4U)
#define GRF_SYS_MACPHY_CON1_BGS_SHIFT                      (0U)
#define GRF_SYS_MACPHY_CON1_BGS_MASK                       (0xFU << GRF_SYS_MACPHY_CON1_BGS_SHIFT)                      /* 0x0000000F */
/* USBOTG_SIG_DETECT_CON */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OFFSET               (0x100U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_LINESTATE_IRQ_EN_SHIFT (0U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_LINESTATE_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_LINESTATE_IRQ_EN_SHIFT) /* 0x00000001 */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_BVALID_RISE_IRQ_EN_SHIFT (2U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_BVALID_RISE_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_BVALID_RISE_IRQ_EN_SHIFT) /* 0x00000004 */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_BVALID_FALL_IRQ_EN_SHIFT (3U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_BVALID_FALL_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_BVALID_FALL_IRQ_EN_SHIFT) /* 0x00000008 */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_ID_RISE_IRQ_EN_SHIFT (4U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_ID_RISE_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_ID_RISE_IRQ_EN_SHIFT) /* 0x00000010 */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_ID_FALL_IRQ_EN_SHIFT (5U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_ID_FALL_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_ID_FALL_IRQ_EN_SHIFT) /* 0x00000020 */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_DISCONNECT_RISE_IRQ_EN_SHIFT (6U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_DISCONNECT_RISE_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_DISCONNECT_RISE_IRQ_EN_SHIFT) /* 0x00000040 */
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_DISCONNECT_FALL_IRQ_EN_SHIFT (7U)
#define GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_DISCONNECT_FALL_IRQ_EN_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CON_OTG_DISCONNECT_FALL_IRQ_EN_SHIFT) /* 0x00000080 */
/* USBOTG_SIG_DETECT_STATUS */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OFFSET            (0x104U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS                   (0x0U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_LINESTATE_IRQ_SHIFT (0U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_LINESTATE_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_LINESTATE_IRQ_SHIFT) /* 0x00000001 */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_RISE_IRQ_SHIFT (2U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_RISE_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_RISE_IRQ_SHIFT) /* 0x00000004 */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_FALL_IRQ_SHIFT (3U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_FALL_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_FALL_IRQ_SHIFT) /* 0x00000008 */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_ID_RISE_IRQ_SHIFT (4U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_ID_RISE_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_ID_RISE_IRQ_SHIFT) /* 0x00000010 */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_ID_FALL_IRQ_SHIFT (5U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_ID_FALL_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_ID_FALL_IRQ_SHIFT) /* 0x00000020 */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_DISCONNECT_RISE_IRQ_SHIFT (6U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_DISCONNECT_RISE_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_DISCONNECT_RISE_IRQ_SHIFT) /* 0x00000040 */
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_DISCONNECT_FALL_IRQ_SHIFT (7U)
#define GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_DISCONNECT_FALL_IRQ_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_STATUS_OTG_DISCONNECT_FALL_IRQ_SHIFT) /* 0x00000080 */
/* USBOTG_SIG_DETECT_CLR */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OFFSET               (0x108U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_LINESTATE_IRQ_CLR_SHIFT (0U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_LINESTATE_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_LINESTATE_IRQ_CLR_SHIFT) /* 0x00000001 */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_BVALID_RISE_IRQ_CLR_SHIFT (2U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_BVALID_RISE_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_BVALID_RISE_IRQ_CLR_SHIFT) /* 0x00000004 */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_BVALID_FALL_IRQ_CLR_SHIFT (3U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_BVALID_FALL_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_BVALID_FALL_IRQ_CLR_SHIFT) /* 0x00000008 */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_ID_RISE_IRQ_CLR_SHIFT (4U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_ID_RISE_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_ID_RISE_IRQ_CLR_SHIFT) /* 0x00000010 */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_ID_FALL_IRQ_CLR_SHIFT (5U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_ID_FALL_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_ID_FALL_IRQ_CLR_SHIFT) /* 0x00000020 */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_DISCONNECT_RISE_IRQ_CLR_SHIFT (6U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_DISCONNECT_RISE_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_DISCONNECT_RISE_IRQ_CLR_SHIFT) /* 0x00000040 */
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_DISCONNECT_FALL_IRQ_CLR_SHIFT (7U)
#define GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_DISCONNECT_FALL_IRQ_CLR_MASK (0x1U << GRF_SYS_USBOTG_SIG_DETECT_CLR_OTG_DISCONNECT_FALL_IRQ_CLR_SHIFT) /* 0x00000080 */
/* USBOTG_LINESTATE_DETECT_CON */
#define GRF_SYS_USBOTG_LINESTATE_DETECT_CON_OFFSET         (0x110U)
#define GRF_SYS_USBOTG_LINESTATE_DETECT_CON_LINESTATE_DETECT_CON_SHIFT (0U)
#define GRF_SYS_USBOTG_LINESTATE_DETECT_CON_LINESTATE_DETECT_CON_MASK (0xFFFFFU << GRF_SYS_USBOTG_LINESTATE_DETECT_CON_LINESTATE_DETECT_CON_SHIFT) /* 0x000FFFFF */
/* USBOTG_DISCONNECT_DETECT_CON */
#define GRF_SYS_USBOTG_DISCONNECT_DETECT_CON_OFFSET        (0x114U)
#define GRF_SYS_USBOTG_DISCONNECT_DETECT_CON_DISCONNECT_DETECT_CON_SHIFT (0U)
#define GRF_SYS_USBOTG_DISCONNECT_DETECT_CON_DISCONNECT_DETECT_CON_MASK (0xFFFFFU << GRF_SYS_USBOTG_DISCONNECT_DETECT_CON_DISCONNECT_DETECT_CON_SHIFT) /* 0x000FFFFF */
/* USBOTG_BVALID_DETECT_CON */
#define GRF_SYS_USBOTG_BVALID_DETECT_CON_OFFSET            (0x118U)
#define GRF_SYS_USBOTG_BVALID_DETECT_CON_BVALID_DETECT_CON_SHIFT (0U)
#define GRF_SYS_USBOTG_BVALID_DETECT_CON_BVALID_DETECT_CON_MASK (0xFFFFFU << GRF_SYS_USBOTG_BVALID_DETECT_CON_BVALID_DETECT_CON_SHIFT) /* 0x000FFFFF */
/* USBOTG_ID_DETECT_CON */
#define GRF_SYS_USBOTG_ID_DETECT_CON_OFFSET                (0x11CU)
#define GRF_SYS_USBOTG_ID_DETECT_CON_ID_DETECT_CON_SHIFT   (0U)
#define GRF_SYS_USBOTG_ID_DETECT_CON_ID_DETECT_CON_MASK    (0xFFFFFFFU << GRF_SYS_USBOTG_ID_DETECT_CON_ID_DETECT_CON_SHIFT) /* 0x0FFFFFFF */
/* CACHE_PERI_ADDR_START */
#define GRF_SYS_CACHE_PERI_ADDR_START_OFFSET               (0x200U)
#define GRF_SYS_CACHE_PERI_ADDR_START_PERI_ADDR_START_SHIFT (0U)
#define GRF_SYS_CACHE_PERI_ADDR_START_PERI_ADDR_START_MASK (0xFFFFFU << GRF_SYS_CACHE_PERI_ADDR_START_PERI_ADDR_START_SHIFT) /* 0x000FFFFF */
/* CACHE_PERI_ADDR_END */
#define GRF_SYS_CACHE_PERI_ADDR_END_OFFSET                 (0x204U)
#define GRF_SYS_CACHE_PERI_ADDR_END_PERIPHRAL_ADDR_END_SHIFT (0U)
#define GRF_SYS_CACHE_PERI_ADDR_END_PERIPHRAL_ADDR_END_MASK (0xFFFFFU << GRF_SYS_CACHE_PERI_ADDR_END_PERIPHRAL_ADDR_END_SHIFT) /* 0x000FFFFF */
/* HPMCU_CODE_ADDR_START */
#define GRF_SYS_HPMCU_CODE_ADDR_START_OFFSET               (0x208U)
#define GRF_SYS_HPMCU_CODE_ADDR_START_HPMCU_CODE_ADDR_START_SHIFT (0U)
#define GRF_SYS_HPMCU_CODE_ADDR_START_HPMCU_CODE_ADDR_START_MASK (0xFFFFFU << GRF_SYS_HPMCU_CODE_ADDR_START_HPMCU_CODE_ADDR_START_SHIFT) /* 0x000FFFFF */
/* HPMCU_SRAM_ADDR_START */
#define GRF_SYS_HPMCU_SRAM_ADDR_START_OFFSET               (0x20CU)
#define GRF_SYS_HPMCU_SRAM_ADDR_START_HPMCU_SRAM_ADDR_START_SHIFT (0U)
#define GRF_SYS_HPMCU_SRAM_ADDR_START_HPMCU_SRAM_ADDR_START_MASK (0xFFFFFU << GRF_SYS_HPMCU_SRAM_ADDR_START_HPMCU_SRAM_ADDR_START_SHIFT) /* 0x000FFFFF */
/* HPMCU_EXSRAM_ADDR_START */
#define GRF_SYS_HPMCU_EXSRAM_ADDR_START_OFFSET             (0x210U)
#define GRF_SYS_HPMCU_EXSRAM_ADDR_START_HPMCU_EXSRAM_ADDR_START_SHIFT (0U)
#define GRF_SYS_HPMCU_EXSRAM_ADDR_START_HPMCU_EXSRAM_ADDR_START_MASK (0xFFFFFU << GRF_SYS_HPMCU_EXSRAM_ADDR_START_HPMCU_EXSRAM_ADDR_START_SHIFT) /* 0x000FFFFF */
/* HPMCU_CACHE_MISC */
#define GRF_SYS_HPMCU_CACHE_MISC_OFFSET                    (0x214U)
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_FLUSH_REQ_SHIFT     (0U)
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_FLUSH_REQ_MASK      (0x1U << GRF_SYS_HPMCU_CACHE_MISC_CACHE_FLUSH_REQ_SHIFT)     /* 0x00000001 */
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_SLV_MEMATTR_SHIFT   (1U)
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_SLV_MEMATTR_MASK    (0x3U << GRF_SYS_HPMCU_CACHE_MISC_CACHE_SLV_MEMATTR_SHIFT)   /* 0x00000006 */
#define GRF_SYS_HPMCU_CACHE_MISC_SCR1_SOFT_IRQ_SHIFT       (3U)
#define GRF_SYS_HPMCU_CACHE_MISC_SCR1_SOFT_IRQ_MASK        (0x1U << GRF_SYS_HPMCU_CACHE_MISC_SCR1_SOFT_IRQ_SHIFT)       /* 0x00000008 */
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_CODE_EN_SHIFT (4U)
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_CODE_EN_MASK (0x1U << GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_CODE_EN_SHIFT) /* 0x00000010 */
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_SRAM_EN_SHIFT (5U)
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_SRAM_EN_MASK (0x1U << GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_SRAM_EN_SHIFT) /* 0x00000020 */
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_EXRAM_EN_SHIFT (6U)
#define GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_EXRAM_EN_MASK (0x1U << GRF_SYS_HPMCU_CACHE_MISC_CACHE_MAINTAIN_EXRAM_EN_SHIFT) /* 0x00000040 */
/* HPMCU_CACHE_STATUS */
#define GRF_SYS_HPMCU_CACHE_STATUS_OFFSET                  (0x218U)
#define GRF_SYS_HPMCU_CACHE_STATUS_CACHE_IDLE_SHIFT        (0U)
#define GRF_SYS_HPMCU_CACHE_STATUS_CACHE_IDLE_MASK         (0x1U << GRF_SYS_HPMCU_CACHE_STATUS_CACHE_IDLE_SHIFT)        /* 0x00000001 */
#define GRF_SYS_HPMCU_CACHE_STATUS_CACHE_FLUSH_ACK_SHIFT   (1U)
#define GRF_SYS_HPMCU_CACHE_STATUS_CACHE_FLUSH_ACK_MASK    (0x1U << GRF_SYS_HPMCU_CACHE_STATUS_CACHE_FLUSH_ACK_SHIFT)   /* 0x00000002 */
#define GRF_SYS_HPMCU_CACHE_STATUS_WFI_HALTED_SHIFT        (2U)
#define GRF_SYS_HPMCU_CACHE_STATUS_WFI_HALTED_MASK         (0x1U << GRF_SYS_HPMCU_CACHE_STATUS_WFI_HALTED_SHIFT)        /* 0x00000004 */
#define GRF_SYS_HPMCU_CACHE_STATUS_NDM_RST_N_OUT_SHIFT     (3U)
#define GRF_SYS_HPMCU_CACHE_STATUS_NDM_RST_N_OUT_MASK      (0x1U << GRF_SYS_HPMCU_CACHE_STATUS_NDM_RST_N_OUT_SHIFT)     /* 0x00000008 */
/* CHIP_ID */
#define GRF_SYS_CHIP_ID_OFFSET                             (0x800U)
#define GRF_SYS_CHIP_ID_CHIP_ID_SHIFT                      (0U)
#define GRF_SYS_CHIP_ID_CHIP_ID_MASK                       (0xFFFFFFFFU << GRF_SYS_CHIP_ID_CHIP_ID_SHIFT)               /* 0xFFFFFFFF */
/* CHIP_VERSION */
#define GRF_SYS_CHIP_VERSION_OFFSET                        (0x804U)
#define GRF_SYS_CHIP_VERSION_CHIP_VERSION_SHIFT            (0U)
#define GRF_SYS_CHIP_VERSION_CHIP_VERSION_MASK             (0xFFFFFFFFU << GRF_SYS_CHIP_VERSION_CHIP_VERSION_SHIFT)     /* 0xFFFFFFFF */
/****************************************GRF_PMU*****************************************/
/* SOC_CON0 */
#define GRF_PMU_SOC_CON0_OFFSET                            (0x0U)
#define GRF_PMU_SOC_CON0_TSADC_SHUT_RESET_TRIGGER_EN_SHIFT (0U)
#define GRF_PMU_SOC_CON0_TSADC_SHUT_RESET_TRIGGER_EN_MASK  (0x1U << GRF_PMU_SOC_CON0_TSADC_SHUT_RESET_TRIGGER_EN_SHIFT) /* 0x00000001 */
#define GRF_PMU_SOC_CON0_WDT_SHUT_RESET_TRIGGER_EN_SHIFT   (1U)
#define GRF_PMU_SOC_CON0_WDT_SHUT_RESET_TRIGGER_EN_MASK    (0x1U << GRF_PMU_SOC_CON0_WDT_SHUT_RESET_TRIGGER_EN_SHIFT)   /* 0x00000002 */
#define GRF_PMU_SOC_CON0_DDRIO_RET_EN_SHIFT                (2U)
#define GRF_PMU_SOC_CON0_DDRIO_RET_EN_MASK                 (0x1U << GRF_PMU_SOC_CON0_DDRIO_RET_EN_SHIFT)                /* 0x00000004 */
#define GRF_PMU_SOC_CON0_SREF_ENTER_EN_SHIFT               (3U)
#define GRF_PMU_SOC_CON0_SREF_ENTER_EN_MASK                (0x1U << GRF_PMU_SOC_CON0_SREF_ENTER_EN_SHIFT)               /* 0x00000008 */
#define GRF_PMU_SOC_CON0_DDRC_GATING_EN_SHIFT              (4U)
#define GRF_PMU_SOC_CON0_DDRC_GATING_EN_MASK               (0x1U << GRF_PMU_SOC_CON0_DDRC_GATING_EN_SHIFT)              /* 0x00000010 */
#define GRF_PMU_SOC_CON0_DDR_IO_RET_DE_REQ_SHIFT           (5U)
#define GRF_PMU_SOC_CON0_DDR_IO_RET_DE_REQ_MASK            (0x1U << GRF_PMU_SOC_CON0_DDR_IO_RET_DE_REQ_SHIFT)           /* 0x00000020 */
#define GRF_PMU_SOC_CON0_DDR_IO_RET_CFG_SHIFT              (6U)
#define GRF_PMU_SOC_CON0_DDR_IO_RET_CFG_MASK               (0x1U << GRF_PMU_SOC_CON0_DDR_IO_RET_CFG_SHIFT)              /* 0x00000040 */
#define GRF_PMU_SOC_CON0_DDR_IO_RET_OEN_CFG_SHIFT          (7U)
#define GRF_PMU_SOC_CON0_DDR_IO_RET_OEN_CFG_MASK           (0x1U << GRF_PMU_SOC_CON0_DDR_IO_RET_OEN_CFG_SHIFT)          /* 0x00000080 */
#define GRF_PMU_SOC_CON0_UPCTL_C_SYSREQ_CFG_SHIFT          (8U)
#define GRF_PMU_SOC_CON0_UPCTL_C_SYSREQ_CFG_MASK           (0x1U << GRF_PMU_SOC_CON0_UPCTL_C_SYSREQ_CFG_SHIFT)          /* 0x00000100 */
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_BUFFEREN_SEL_SHIFT (9U)
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_BUFFEREN_SEL_MASK  (0x1U << GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_BUFFEREN_SEL_SHIFT) /* 0x00000200 */
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_BUFFEREN_CORE_SHIFT (10U)
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_BUFFEREN_CORE_MASK (0x1U << GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_BUFFEREN_CORE_SHIFT) /* 0x00000400 */
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_IO_HZ_SEL_SHIFT    (11U)
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_IO_HZ_SEL_MASK     (0x1U << GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_IO_HZ_SEL_SHIFT)    /* 0x00000800 */
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_IO_HZ_SHIFT        (12U)
#define GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_IO_HZ_MASK         (0x1U << GRF_PMU_SOC_CON0_GRF_CON_DDRPHY_IO_HZ_SHIFT)        /* 0x00001000 */
#define GRF_PMU_SOC_CON0_GRF_CON_RC_OSC_SEL_SHIFT          (14U)
#define GRF_PMU_SOC_CON0_GRF_CON_RC_OSC_SEL_MASK           (0x1U << GRF_PMU_SOC_CON0_GRF_CON_RC_OSC_SEL_SHIFT)          /* 0x00004000 */
#define GRF_PMU_SOC_CON0_GRF_CON_RTC_CLAMP_EN_SHIFT        (15U)
#define GRF_PMU_SOC_CON0_GRF_CON_RTC_CLAMP_EN_MASK         (0x1U << GRF_PMU_SOC_CON0_GRF_CON_RTC_CLAMP_EN_SHIFT)        /* 0x00008000 */
/* SOC_CON1 */
#define GRF_PMU_SOC_CON1_OFFSET                            (0x4U)
#define GRF_PMU_SOC_CON1_GRF_CON_SLEEP0_SRC_SEL_SHIFT      (0U)
#define GRF_PMU_SOC_CON1_GRF_CON_SLEEP0_SRC_SEL_MASK       (0x3U << GRF_PMU_SOC_CON1_GRF_CON_SLEEP0_SRC_SEL_SHIFT)      /* 0x00000003 */
#define GRF_PMU_SOC_CON1_GRF_CON_SLEEP1_SRC_SEL_SHIFT      (2U)
#define GRF_PMU_SOC_CON1_GRF_CON_SLEEP1_SRC_SEL_MASK       (0x3U << GRF_PMU_SOC_CON1_GRF_CON_SLEEP1_SRC_SEL_SHIFT)      /* 0x0000000C */
#define GRF_PMU_SOC_CON1_GRF_CON_PMIC_SLEEP0_POL_SHIFT     (4U)
#define GRF_PMU_SOC_CON1_GRF_CON_PMIC_SLEEP0_POL_MASK      (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PMIC_SLEEP0_POL_SHIFT)     /* 0x00000010 */
#define GRF_PMU_SOC_CON1_GRF_CON_PMIC_SLEEP1_POL_SHIFT     (5U)
#define GRF_PMU_SOC_CON1_GRF_CON_PMIC_SLEEP1_POL_MASK      (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PMIC_SLEEP1_POL_SHIFT)     /* 0x00000020 */
#define GRF_PMU_SOC_CON1_GRF_CON_PWM0_SWITCH_ENA_SHIFT     (6U)
#define GRF_PMU_SOC_CON1_GRF_CON_PWM0_SWITCH_ENA_MASK      (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PWM0_SWITCH_ENA_SHIFT)     /* 0x00000040 */
#define GRF_PMU_SOC_CON1_GRF_CON_PWM1_SWITCH_ENA_SHIFT     (7U)
#define GRF_PMU_SOC_CON1_GRF_CON_PWM1_SWITCH_ENA_MASK      (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PWM1_SWITCH_ENA_SHIFT)     /* 0x00000080 */
#define GRF_PMU_SOC_CON1_GRF_CON_CLK_32_IOE_SHIFT          (8U)
#define GRF_PMU_SOC_CON1_GRF_CON_CLK_32_IOE_MASK           (0x1U << GRF_PMU_SOC_CON1_GRF_CON_CLK_32_IOE_SHIFT)          /* 0x00000100 */
#define GRF_PMU_SOC_CON1_GRF_CON_UART2_CTS_INV_SHIFT       (9U)
#define GRF_PMU_SOC_CON1_GRF_CON_UART2_CTS_INV_MASK        (0x1U << GRF_PMU_SOC_CON1_GRF_CON_UART2_CTS_INV_SHIFT)       /* 0x00000200 */
#define GRF_PMU_SOC_CON1_GRF_CON_UART2_RTS_INV_SHIFT       (10U)
#define GRF_PMU_SOC_CON1_GRF_CON_UART2_RTS_INV_MASK        (0x1U << GRF_PMU_SOC_CON1_GRF_CON_UART2_RTS_INV_SHIFT)       /* 0x00000400 */
#define GRF_PMU_SOC_CON1_GRF_CON_UART2_DMA_RX_SINGLE_BYPASS_SHIFT (11U)
#define GRF_PMU_SOC_CON1_GRF_CON_UART2_DMA_RX_SINGLE_BYPASS_MASK (0x1U << GRF_PMU_SOC_CON1_GRF_CON_UART2_DMA_RX_SINGLE_BYPASS_SHIFT) /* 0x00000800 */
#define GRF_PMU_SOC_CON1_GRF_CON_PMUMEM_CLKGAT_DISABLE_SHIFT (12U)
#define GRF_PMU_SOC_CON1_GRF_CON_PMUMEM_CLKGAT_DISABLE_MASK (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PMUMEM_CLKGAT_DISABLE_SHIFT) /* 0x00001000 */
#define GRF_PMU_SOC_CON1_GRF_CON_WDT_NS_PMU_PAUSE_EN_SHIFT (13U)
#define GRF_PMU_SOC_CON1_GRF_CON_WDT_NS_PMU_PAUSE_EN_MASK  (0x1U << GRF_PMU_SOC_CON1_GRF_CON_WDT_NS_PMU_PAUSE_EN_SHIFT) /* 0x00002000 */
#define GRF_PMU_SOC_CON1_GRF_CON_PMUMCU_SOFT_IRQ_SHIFT     (14U)
#define GRF_PMU_SOC_CON1_GRF_CON_PMUMCU_SOFT_IRQ_MASK      (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PMUMCU_SOFT_IRQ_SHIFT)     /* 0x00004000 */
#define GRF_PMU_SOC_CON1_GRF_CON_PSRAM_SPI_CLK_IOE__SHIFT  (15U)
#define GRF_PMU_SOC_CON1_GRF_CON_PSRAM_SPI_CLK_IOE__MASK   (0x1U << GRF_PMU_SOC_CON1_GRF_CON_PSRAM_SPI_CLK_IOE__SHIFT)  /* 0x00008000 */
/* SOC_CON2 */
#define GRF_PMU_SOC_CON2_OFFSET                            (0x8U)
#define GRF_PMU_SOC_CON2_GRF_CON_LDO_LEAKAGE_SEL_SHIFT     (0U)
#define GRF_PMU_SOC_CON2_GRF_CON_LDO_LEAKAGE_SEL_MASK      (0x1U << GRF_PMU_SOC_CON2_GRF_CON_LDO_LEAKAGE_SEL_SHIFT)     /* 0x00000001 */
#define GRF_PMU_SOC_CON2_GRF_CON_LDO_LEAKAGE_EN_SHIFT      (1U)
#define GRF_PMU_SOC_CON2_GRF_CON_LDO_LEAKAGE_EN_MASK       (0x1U << GRF_PMU_SOC_CON2_GRF_CON_LDO_LEAKAGE_EN_SHIFT)      /* 0x00000002 */
#define GRF_PMU_SOC_CON2_GRF_CON_PREROLL_EN_SHIFT          (2U)
#define GRF_PMU_SOC_CON2_GRF_CON_PREROLL_EN_MASK           (0x1U << GRF_PMU_SOC_CON2_GRF_CON_PREROLL_EN_SHIFT)          /* 0x00000004 */
#define GRF_PMU_SOC_CON2_GRF_CON_PREROLL_ON_TIMEOUT_CLR_SHIFT (3U)
#define GRF_PMU_SOC_CON2_GRF_CON_PREROLL_ON_TIMEOUT_CLR_MASK (0x1U << GRF_PMU_SOC_CON2_GRF_CON_PREROLL_ON_TIMEOUT_CLR_SHIFT) /* 0x00000008 */
#define GRF_PMU_SOC_CON2_GRF_CON_PMU1_PWRDN_BYPASS_SHIFT   (4U)
#define GRF_PMU_SOC_CON2_GRF_CON_PMU1_PWRDN_BYPASS_MASK    (0x1U << GRF_PMU_SOC_CON2_GRF_CON_PMU1_PWRDN_BYPASS_SHIFT)   /* 0x00000010 */
#define GRF_PMU_SOC_CON2_GRF_CON_PMUIO1_PWRDN_BYPASS_SHIFT (5U)
#define GRF_PMU_SOC_CON2_GRF_CON_PMUIO1_PWRDN_BYPASS_MASK  (0x1U << GRF_PMU_SOC_CON2_GRF_CON_PMUIO1_PWRDN_BYPASS_SHIFT) /* 0x00000020 */
#define GRF_PMU_SOC_CON2_GRF_CON_PREROLL_MODE_SHIFT        (6U)
#define GRF_PMU_SOC_CON2_GRF_CON_PREROLL_MODE_MASK         (0x1U << GRF_PMU_SOC_CON2_GRF_CON_PREROLL_MODE_SHIFT)        /* 0x00000040 */
#define GRF_PMU_SOC_CON2_GRF_CON_PWR_CTRL0_IOEN_SHIFT      (8U)
#define GRF_PMU_SOC_CON2_GRF_CON_PWR_CTRL0_IOEN_MASK       (0x1U << GRF_PMU_SOC_CON2_GRF_CON_PWR_CTRL0_IOEN_SHIFT)      /* 0x00000100 */
#define GRF_PMU_SOC_CON2_GRF_CON_SPI2AHB_HPROT2_SHIFT      (9U)
#define GRF_PMU_SOC_CON2_GRF_CON_SPI2AHB_HPROT2_MASK       (0x1U << GRF_PMU_SOC_CON2_GRF_CON_SPI2AHB_HPROT2_SHIFT)      /* 0x00000200 */
#define GRF_PMU_SOC_CON2_GRF_CON_BUFFERABLE_HPROT2_FSPI_PMU_SHIFT (10U)
#define GRF_PMU_SOC_CON2_GRF_CON_BUFFERABLE_HPROT2_FSPI_PMU_MASK (0x1U << GRF_PMU_SOC_CON2_GRF_CON_BUFFERABLE_HPROT2_FSPI_PMU_SHIFT) /* 0x00000400 */
#define GRF_PMU_SOC_CON2_GRF_CON_LPMCU_HPROT2_SHIFT        (11U)
#define GRF_PMU_SOC_CON2_GRF_CON_LPMCU_HPROT2_MASK         (0x1U << GRF_PMU_SOC_CON2_GRF_CON_LPMCU_HPROT2_SHIFT)        /* 0x00000800 */
#define GRF_PMU_SOC_CON2_GRF_CON_UART0_DMA_REQACK_RX_CLR_SHIFT (12U)
#define GRF_PMU_SOC_CON2_GRF_CON_UART0_DMA_REQACK_RX_CLR_MASK (0x1U << GRF_PMU_SOC_CON2_GRF_CON_UART0_DMA_REQACK_RX_CLR_SHIFT) /* 0x00001000 */
#define GRF_PMU_SOC_CON2_GRF_CON_UART0_DMA_REQACK_TX_CLR_SHIFT (13U)
#define GRF_PMU_SOC_CON2_GRF_CON_UART0_DMA_REQACK_TX_CLR_MASK (0x1U << GRF_PMU_SOC_CON2_GRF_CON_UART0_DMA_REQACK_TX_CLR_SHIFT) /* 0x00002000 */
/* SOC_CON3 */
#define GRF_PMU_SOC_CON3_OFFSET                            (0xCU)
#define GRF_PMU_SOC_CON3_NPOR_OUT2CHIP_PULSE_WIDTH_SHIFT   (0U)
#define GRF_PMU_SOC_CON3_NPOR_OUT2CHIP_PULSE_WIDTH_MASK    (0x7FFFU << GRF_PMU_SOC_CON3_NPOR_OUT2CHIP_PULSE_WIDTH_SHIFT) /* 0x00007FFF */
/* SOC_CON4 */
#define GRF_PMU_SOC_CON4_OFFSET                            (0x10U)
#define GRF_PMU_SOC_CON4_PMUCRU_SOFTRST_HOLD0_SHIFT        (0U)
#define GRF_PMU_SOC_CON4_PMUCRU_SOFTRST_HOLD0_MASK         (0xFFFFU << GRF_PMU_SOC_CON4_PMUCRU_SOFTRST_HOLD0_SHIFT)     /* 0x0000FFFF */
/* SOC_CON5 */
#define GRF_PMU_SOC_CON5_OFFSET                            (0x14U)
#define GRF_PMU_SOC_CON5_PMUCRU_SOFTRST_HOLD1_SHIFT        (0U)
#define GRF_PMU_SOC_CON5_PMUCRU_SOFTRST_HOLD1_MASK         (0xFFFFU << GRF_PMU_SOC_CON5_PMUCRU_SOFTRST_HOLD1_SHIFT)     /* 0x0000FFFF */
/* SOC_CON6 */
#define GRF_PMU_SOC_CON6_OFFSET                            (0x18U)
#define GRF_PMU_SOC_CON6_PMUCRU_SOFTRST_HOLD2_SHIFT        (0U)
#define GRF_PMU_SOC_CON6_PMUCRU_SOFTRST_HOLD2_MASK         (0xFFFFU << GRF_PMU_SOC_CON6_PMUCRU_SOFTRST_HOLD2_SHIFT)     /* 0x0000FFFF */
/* SOC_CON7 */
#define GRF_PMU_SOC_CON7_OFFSET                            (0x1CU)
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_EN_SHIFT            (0U)
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_EN_MASK             (0x1U << GRF_PMU_SOC_CON7_GRF_CON_RCOSC_EN_SHIFT)            /* 0x00000001 */
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_SM_EN_SHIFT         (1U)
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_SM_EN_MASK          (0x1U << GRF_PMU_SOC_CON7_GRF_CON_RCOSC_SM_EN_SHIFT)         /* 0x00000002 */
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_PWM_SHIFT      (2U)
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_PWM_MASK       (0x1U << GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_PWM_SHIFT)      /* 0x00000004 */
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_COARSE_SHIFT   (4U)
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_COARSE_MASK    (0x7U << GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_COARSE_SHIFT)   /* 0x00000070 */
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_FINE_SHIFT     (8U)
#define GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_FINE_MASK      (0xFFU << GRF_PMU_SOC_CON7_GRF_CON_RCOSC_TRIM_FINE_SHIFT)    /* 0x0000FF00 */
/* SOC_CON8 */
#define GRF_PMU_SOC_CON8_OFFSET                            (0x20U)
#define GRF_PMU_SOC_CON8_GRF_CON_PMUIO1_PWR_UP_CNT_THRESH_SHIFT (0U)
#define GRF_PMU_SOC_CON8_GRF_CON_PMUIO1_PWR_UP_CNT_THRESH_MASK (0xFFFFFFFFU << GRF_PMU_SOC_CON8_GRF_CON_PMUIO1_PWR_UP_CNT_THRESH_SHIFT) /* 0xFFFFFFFF */
/* SOC_CON9 */
#define GRF_PMU_SOC_CON9_OFFSET                            (0x24U)
#define GRF_PMU_SOC_CON9_GRF_CON_PREROLL_ON_TIMEOUT_THRESH_SHIFT (0U)
#define GRF_PMU_SOC_CON9_GRF_CON_PREROLL_ON_TIMEOUT_THRESH_MASK (0xFFFFFFFFU << GRF_PMU_SOC_CON9_GRF_CON_PREROLL_ON_TIMEOUT_THRESH_SHIFT) /* 0xFFFFFFFF */
/* SOC_CON10 */
#define GRF_PMU_SOC_CON10_OFFSET                           (0x28U)
#define GRF_PMU_SOC_CON10_GRF_CON_PREROLL_OFF_TIMEOUT_THRESH_SHIFT (0U)
#define GRF_PMU_SOC_CON10_GRF_CON_PREROLL_OFF_TIMEOUT_THRESH_MASK (0xFFFFFFFFU << GRF_PMU_SOC_CON10_GRF_CON_PREROLL_OFF_TIMEOUT_THRESH_SHIFT) /* 0xFFFFFFFF */
/* SOC_STATUS0 */
#define GRF_PMU_SOC_STATUS0_OFFSET                         (0x30U)
#define GRF_PMU_SOC_STATUS0                                (0x3000U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFI_SHIFT        (0U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFI_MASK         (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFI_SHIFT)        /* 0x00000001 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFE_SHIFT        (1U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFE_MASK         (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFE_SHIFT)        /* 0x00000002 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFIL2_SHIFT      (2U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFIL2_MASK       (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_STANDBYWFIL2_SHIFT)      /* 0x00000004 */
#define GRF_PMU_SOC_STATUS0_RKNPOR_POWERGOOD_SHIFT         (3U)
#define GRF_PMU_SOC_STATUS0_RKNPOR_POWERGOOD_MASK          (0x1U << GRF_PMU_SOC_STATUS0_RKNPOR_POWERGOOD_SHIFT)         /* 0x00000008 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_ON_SHIFT        (4U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_ON_MASK         (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_ON_SHIFT)        /* 0x00000010 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_OFF_SHIFT       (5U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_OFF_MASK        (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_OFF_SHIFT)       /* 0x00000020 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_ON_TIMEOUT_SHIFT (6U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_ON_TIMEOUT_MASK (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_ON_TIMEOUT_SHIFT) /* 0x00000040 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_PMU_MCU_RST_N_OUT_SHIFT (8U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_PMU_MCU_RST_N_OUT_MASK  (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_PMU_MCU_RST_N_OUT_SHIFT) /* 0x00000100 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_PMU_MCU_WFI_HALTED_SHIFT (9U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_PMU_MCU_WFI_HALTED_MASK (0x1U << GRF_PMU_SOC_STATUS0_GRF_ST_PMU_MCU_WFI_HALTED_SHIFT) /* 0x00000200 */
#define GRF_PMU_SOC_STATUS0_SPI2AHB_IDLE_SHIFT             (12U)
#define GRF_PMU_SOC_STATUS0_SPI2AHB_IDLE_MASK              (0x1U << GRF_PMU_SOC_STATUS0_SPI2AHB_IDLE_SHIFT)             /* 0x00001000 */
#define GRF_PMU_SOC_STATUS0_FSPI_IDLE_SHIFT                (13U)
#define GRF_PMU_SOC_STATUS0_FSPI_IDLE_MASK                 (0x1U << GRF_PMU_SOC_STATUS0_FSPI_IDLE_SHIFT)                /* 0x00002000 */
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_STATE_SHIFT     (16U)
#define GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_STATE_MASK      (0xFU << GRF_PMU_SOC_STATUS0_GRF_ST_PREROLL_STATE_SHIFT)     /* 0x000F0000 */
/* MEN_CON */
#define GRF_PMU_MEN_CON_OFFSET                             (0x40U)
#define GRF_PMU_MEN_CON_GRF_CON_PMU_MEM_CFG_UHDSPRA_SHIFT  (0U)
#define GRF_PMU_MEN_CON_GRF_CON_PMU_MEM_CFG_UHDSPRA_MASK   (0xFU << GRF_PMU_MEN_CON_GRF_CON_PMU_MEM_CFG_UHDSPRA_SHIFT)  /* 0x0000000F */
/* SOC_SPECIAL0 */
#define GRF_PMU_SOC_SPECIAL0_OFFSET                        (0x50U)
#define GRF_PMU_SOC_SPECIAL0_GRF_CON_SOC_TEST_MODE_EN_SHIFT (0U)
#define GRF_PMU_SOC_SPECIAL0_GRF_CON_SOC_TEST_MODE_EN_MASK (0x1U << GRF_PMU_SOC_SPECIAL0_GRF_CON_SOC_TEST_MODE_EN_SHIFT) /* 0x00000001 */
/* SOC_PREROLL_INT_CON */
#define GRF_PMU_SOC_PREROLL_INT_CON_OFFSET                 (0x60U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_CLR_SHIFT (0U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_CLR_MASK (0x1U << GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_CLR_SHIFT) /* 0x00000001 */
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_OFF_CLR_SHIFT (1U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_OFF_CLR_MASK (0x1U << GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_OFF_CLR_SHIFT) /* 0x00000002 */
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_TIMEOUT_CLR_SHIFT (2U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_TIMEOUT_CLR_MASK (0x1U << GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_TIMEOUT_CLR_SHIFT) /* 0x00000004 */
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_EN_SHIFT (4U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_EN_MASK (0x1U << GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_EN_SHIFT) /* 0x00000010 */
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_OFF_EN_SHIFT (5U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_OFF_EN_MASK (0x1U << GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_OFF_EN_SHIFT) /* 0x00000020 */
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_TIMEOUT_EN_SHIFT (6U)
#define GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_TIMEOUT_EN_MASK (0x1U << GRF_PMU_SOC_PREROLL_INT_CON_INT_PREROLL_ON_TIMEOUT_EN_SHIFT) /* 0x00000040 */
/* OS_REG0 */
#define GRF_PMU_OS_REG0_OFFSET                             (0x200U)
#define GRF_PMU_OS_REG0_PMU_OS_REG0_SHIFT                  (0U)
#define GRF_PMU_OS_REG0_PMU_OS_REG0_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG0_PMU_OS_REG0_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG1 */
#define GRF_PMU_OS_REG1_OFFSET                             (0x204U)
#define GRF_PMU_OS_REG1_PMU_OS_REG1_SHIFT                  (0U)
#define GRF_PMU_OS_REG1_PMU_OS_REG1_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG1_PMU_OS_REG1_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG2 */
#define GRF_PMU_OS_REG2_OFFSET                             (0x208U)
#define GRF_PMU_OS_REG2_PMU_OS_REG2_SHIFT                  (0U)
#define GRF_PMU_OS_REG2_PMU_OS_REG2_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG2_PMU_OS_REG2_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG3 */
#define GRF_PMU_OS_REG3_OFFSET                             (0x20CU)
#define GRF_PMU_OS_REG3_PMU_OS_REG3_SHIFT                  (0U)
#define GRF_PMU_OS_REG3_PMU_OS_REG3_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG3_PMU_OS_REG3_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG4 */
#define GRF_PMU_OS_REG4_OFFSET                             (0x210U)
#define GRF_PMU_OS_REG4_PMU_OS_REG4_SHIFT                  (0U)
#define GRF_PMU_OS_REG4_PMU_OS_REG4_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG4_PMU_OS_REG4_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG5 */
#define GRF_PMU_OS_REG5_OFFSET                             (0x214U)
#define GRF_PMU_OS_REG5_PMU_OS_REG5_SHIFT                  (0U)
#define GRF_PMU_OS_REG5_PMU_OS_REG5_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG5_PMU_OS_REG5_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG6 */
#define GRF_PMU_OS_REG6_OFFSET                             (0x218U)
#define GRF_PMU_OS_REG6_PMU_OS_REG6_SHIFT                  (0U)
#define GRF_PMU_OS_REG6_PMU_OS_REG6_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG6_PMU_OS_REG6_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG7 */
#define GRF_PMU_OS_REG7_OFFSET                             (0x21CU)
#define GRF_PMU_OS_REG7_PMU_OS_REG7_SHIFT                  (0U)
#define GRF_PMU_OS_REG7_PMU_OS_REG7_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG7_PMU_OS_REG7_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG8 */
#define GRF_PMU_OS_REG8_OFFSET                             (0x220U)
#define GRF_PMU_OS_REG8_PMU_OS_REG8_SHIFT                  (0U)
#define GRF_PMU_OS_REG8_PMU_OS_REG8_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG8_PMU_OS_REG8_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG9 */
#define GRF_PMU_OS_REG9_OFFSET                             (0x224U)
#define GRF_PMU_OS_REG9_PMU_OS_REG9_SHIFT                  (0U)
#define GRF_PMU_OS_REG9_PMU_OS_REG9_MASK                   (0xFFFFFFFFU << GRF_PMU_OS_REG9_PMU_OS_REG9_SHIFT)           /* 0xFFFFFFFF */
/* OS_REG10 */
#define GRF_PMU_OS_REG10_OFFSET                            (0x228U)
#define GRF_PMU_OS_REG10_PMU_OS_REG10_SHIFT                (0U)
#define GRF_PMU_OS_REG10_PMU_OS_REG10_MASK                 (0xFFFFFFFFU << GRF_PMU_OS_REG10_PMU_OS_REG10_SHIFT)         /* 0xFFFFFFFF */
/* OS_REG11 */
#define GRF_PMU_OS_REG11_OFFSET                            (0x22CU)
#define GRF_PMU_OS_REG11_PMU_OS_REG11_SHIFT                (0U)
#define GRF_PMU_OS_REG11_PMU_OS_REG11_MASK                 (0xFFFFFFFFU << GRF_PMU_OS_REG11_PMU_OS_REG11_SHIFT)         /* 0xFFFFFFFF */
/* RESET_FUNCTION_STATUS */
#define GRF_PMU_RESET_FUNCTION_STATUS_OFFSET               (0x230U)
#define GRF_PMU_RESET_FUNCTION_STATUS                      (0x0U)
#define GRF_PMU_RESET_FUNCTION_STATUS_FIRST_RESET_SRC_SHIFT (0U)
#define GRF_PMU_RESET_FUNCTION_STATUS_FIRST_RESET_SRC_MASK (0x1U << GRF_PMU_RESET_FUNCTION_STATUS_FIRST_RESET_SRC_SHIFT) /* 0x00000001 */
#define GRF_PMU_RESET_FUNCTION_STATUS_WDT_RESET_SRC_SHIFT  (1U)
#define GRF_PMU_RESET_FUNCTION_STATUS_WDT_RESET_SRC_MASK   (0x1U << GRF_PMU_RESET_FUNCTION_STATUS_WDT_RESET_SRC_SHIFT)  /* 0x00000002 */
#define GRF_PMU_RESET_FUNCTION_STATUS_TSADC_SHUT_RESET_SRC_SHIFT (2U)
#define GRF_PMU_RESET_FUNCTION_STATUS_TSADC_SHUT_RESET_SRC_MASK (0x1U << GRF_PMU_RESET_FUNCTION_STATUS_TSADC_SHUT_RESET_SRC_SHIFT) /* 0x00000004 */
#define GRF_PMU_RESET_FUNCTION_STATUS_DDR_FAIL_SAFE_SRC_SHIFT (3U)
#define GRF_PMU_RESET_FUNCTION_STATUS_DDR_FAIL_SAFE_SRC_MASK (0x1U << GRF_PMU_RESET_FUNCTION_STATUS_DDR_FAIL_SAFE_SRC_SHIFT) /* 0x00000008 */
/* RESET_FUNCTION_CLR */
#define GRF_PMU_RESET_FUNCTION_CLR_OFFSET                  (0x234U)
#define GRF_PMU_RESET_FUNCTION_CLR_FIRST_RESET_SRC_CLR_SHIFT (0U)
#define GRF_PMU_RESET_FUNCTION_CLR_FIRST_RESET_SRC_CLR_MASK (0x1U << GRF_PMU_RESET_FUNCTION_CLR_FIRST_RESET_SRC_CLR_SHIFT) /* 0x00000001 */
#define GRF_PMU_RESET_FUNCTION_CLR_WDT_RESET_SRC_CLR_SHIFT (1U)
#define GRF_PMU_RESET_FUNCTION_CLR_WDT_RESET_SRC_CLR_MASK  (0x1U << GRF_PMU_RESET_FUNCTION_CLR_WDT_RESET_SRC_CLR_SHIFT) /* 0x00000002 */
#define GRF_PMU_RESET_FUNCTION_CLR_TSADC_SHUT_RESET_SRC_CLR_SHIFT (2U)
#define GRF_PMU_RESET_FUNCTION_CLR_TSADC_SHUT_RESET_SRC_CLR_MASK (0x1U << GRF_PMU_RESET_FUNCTION_CLR_TSADC_SHUT_RESET_SRC_CLR_SHIFT) /* 0x00000004 */
#define GRF_PMU_RESET_FUNCTION_CLR_DDR_FAIL_SAFE_SRC_CLR_SHIFT (3U)
#define GRF_PMU_RESET_FUNCTION_CLR_DDR_FAIL_SAFE_SRC_CLR_MASK (0x1U << GRF_PMU_RESET_FUNCTION_CLR_DDR_FAIL_SAFE_SRC_CLR_SHIFT) /* 0x00000008 */
/***************************************GPIO1_IOC****************************************/
/* GPIO1A_IOMUX_SEL_0 */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_OFFSET                (0x20U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A0_SEL_SHIFT     (0U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A0_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A0_SEL_SHIFT)     /* 0x0000000F */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A1_SEL_SHIFT     (4U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A1_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A1_SEL_SHIFT)     /* 0x000000F0 */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A2_SEL_SHIFT     (8U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A2_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A2_SEL_SHIFT)     /* 0x00000F00 */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A3_SEL_SHIFT     (12U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A3_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_0_GPIO1A3_SEL_SHIFT)     /* 0x0000F000 */
/* GPIO1A_IOMUX_SEL_1_0 */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_OFFSET              (0x24U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_GPIO1A4_SEL_SHIFT   (0U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_GPIO1A4_SEL_MASK    (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_GPIO1A4_SEL_SHIFT)   /* 0x0000000F */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_GPIO1A5_SEL_SHIFT   (4U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_GPIO1A5_SEL_MASK    (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_1_0_GPIO1A5_SEL_SHIFT)   /* 0x000000F0 */
/* GPIO1A_IOMUX_SEL_1_1 */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_OFFSET              (0x10024U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_GPIO1A6_SEL_SHIFT   (8U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_GPIO1A6_SEL_MASK    (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_GPIO1A6_SEL_SHIFT)   /* 0x00000F00 */
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_GPIO1A7_SEL_SHIFT   (12U)
#define GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_GPIO1A7_SEL_MASK    (0xFU << GPIO1_IOC_GPIO1A_IOMUX_SEL_1_1_GPIO1A7_SEL_SHIFT)   /* 0x0000F000 */
/* GPIO1B_IOMUX_SEL_0 */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_OFFSET                (0x10028U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B0_SEL_SHIFT     (0U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B0_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B0_SEL_SHIFT)     /* 0x0000000F */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B1_SEL_SHIFT     (4U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B1_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B1_SEL_SHIFT)     /* 0x000000F0 */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B2_SEL_SHIFT     (8U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B2_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B2_SEL_SHIFT)     /* 0x00000F00 */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B3_SEL_SHIFT     (12U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B3_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_0_GPIO1B3_SEL_SHIFT)     /* 0x0000F000 */
/* GPIO1B_IOMUX_SEL_1 */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_OFFSET                (0x1002CU)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B4_SEL_SHIFT     (0U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B4_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B4_SEL_SHIFT)     /* 0x0000000F */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B5_SEL_SHIFT     (4U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B5_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B5_SEL_SHIFT)     /* 0x000000F0 */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B6_SEL_SHIFT     (8U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B6_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B6_SEL_SHIFT)     /* 0x00000F00 */
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B7_SEL_SHIFT     (12U)
#define GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B7_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1B_IOMUX_SEL_1_GPIO1B7_SEL_SHIFT)     /* 0x0000F000 */
/* GPIO1C_IOMUX_SEL_0 */
#define GPIO1_IOC_GPIO1C_IOMUX_SEL_0_OFFSET                (0x10030U)
#define GPIO1_IOC_GPIO1C_IOMUX_SEL_0_GPIO1C0_SEL_SHIFT     (0U)
#define GPIO1_IOC_GPIO1C_IOMUX_SEL_0_GPIO1C0_SEL_MASK      (0xFU << GPIO1_IOC_GPIO1C_IOMUX_SEL_0_GPIO1C0_SEL_SHIFT)     /* 0x0000000F */
/* GPIO1A_DS_0 */
#define GPIO1_IOC_GPIO1A_DS_0_OFFSET                       (0x140U)
#define GPIO1_IOC_GPIO1A_DS_0_GPIO1A0_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_DS_0_GPIO1A0_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_0_GPIO1A0_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1A_DS_0_GPIO1A1_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1A_DS_0_GPIO1A1_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_0_GPIO1A1_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1A_DS_1 */
#define GPIO1_IOC_GPIO1A_DS_1_OFFSET                       (0x144U)
#define GPIO1_IOC_GPIO1A_DS_1_GPIO1A2_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_DS_1_GPIO1A2_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_1_GPIO1A2_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1A_DS_1_GPIO1A3_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1A_DS_1_GPIO1A3_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_1_GPIO1A3_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1A_DS_2 */
#define GPIO1_IOC_GPIO1A_DS_2_OFFSET                       (0x148U)
#define GPIO1_IOC_GPIO1A_DS_2_GPIO1A4_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_DS_2_GPIO1A4_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_2_GPIO1A4_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1A_DS_2_GPIO1A5_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1A_DS_2_GPIO1A5_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_2_GPIO1A5_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1A_DS_3 */
#define GPIO1_IOC_GPIO1A_DS_3_OFFSET                       (0x1014CU)
#define GPIO1_IOC_GPIO1A_DS_3_GPIO1A6_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_DS_3_GPIO1A6_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_3_GPIO1A6_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1A_DS_3_GPIO1A7_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1A_DS_3_GPIO1A7_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1A_DS_3_GPIO1A7_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1B_DS_0 */
#define GPIO1_IOC_GPIO1B_DS_0_OFFSET                       (0x10150U)
#define GPIO1_IOC_GPIO1B_DS_0_GPIO1B0_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1B_DS_0_GPIO1B0_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_0_GPIO1B0_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1B_DS_0_GPIO1B1_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1B_DS_0_GPIO1B1_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_0_GPIO1B1_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1B_DS_1 */
#define GPIO1_IOC_GPIO1B_DS_1_OFFSET                       (0x10154U)
#define GPIO1_IOC_GPIO1B_DS_1_GPIO1B2_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1B_DS_1_GPIO1B2_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_1_GPIO1B2_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1B_DS_1_GPIO1B3_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1B_DS_1_GPIO1B3_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_1_GPIO1B3_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1B_DS_2 */
#define GPIO1_IOC_GPIO1B_DS_2_OFFSET                       (0x10158U)
#define GPIO1_IOC_GPIO1B_DS_2_GPIO1B4_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1B_DS_2_GPIO1B4_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_2_GPIO1B4_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1B_DS_2_GPIO1B5_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1B_DS_2_GPIO1B5_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_2_GPIO1B5_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1B_DS_3 */
#define GPIO1_IOC_GPIO1B_DS_3_OFFSET                       (0x1015CU)
#define GPIO1_IOC_GPIO1B_DS_3_GPIO1B6_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1B_DS_3_GPIO1B6_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_3_GPIO1B6_DS_SHIFT)            /* 0x0000003F */
#define GPIO1_IOC_GPIO1B_DS_3_GPIO1B7_DS_SHIFT             (8U)
#define GPIO1_IOC_GPIO1B_DS_3_GPIO1B7_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1B_DS_3_GPIO1B7_DS_SHIFT)            /* 0x00003F00 */
/* GPIO1C_DS_0 */
#define GPIO1_IOC_GPIO1C_DS_0_OFFSET                       (0x10160U)
#define GPIO1_IOC_GPIO1C_DS_0_GPIO1C0_DS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1C_DS_0_GPIO1C0_DS_MASK              (0x3FU << GPIO1_IOC_GPIO1C_DS_0_GPIO1C0_DS_SHIFT)            /* 0x0000003F */
/* GPIO1A_PULL_0 */
#define GPIO1_IOC_GPIO1A_PULL_0_OFFSET                     (0x210U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A0_PULL_SHIFT         (0U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A0_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_0_GPIO1A0_PULL_SHIFT)         /* 0x00000003 */
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A1_PULL_SHIFT         (2U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A1_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_0_GPIO1A1_PULL_SHIFT)         /* 0x0000000C */
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A2_PULL_SHIFT         (4U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A2_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_0_GPIO1A2_PULL_SHIFT)         /* 0x00000030 */
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A3_PULL_SHIFT         (6U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A3_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_0_GPIO1A3_PULL_SHIFT)         /* 0x000000C0 */
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A4_PULL_SHIFT         (8U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A4_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_0_GPIO1A4_PULL_SHIFT)         /* 0x00000300 */
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A5_PULL_SHIFT         (10U)
#define GPIO1_IOC_GPIO1A_PULL_0_GPIO1A5_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_0_GPIO1A5_PULL_SHIFT)         /* 0x00000C00 */
/* GPIO1A_PULL_1 */
#define GPIO1_IOC_GPIO1A_PULL_1_OFFSET                     (0x10210U)
#define GPIO1_IOC_GPIO1A_PULL_1_GPIO1A6_PULL_SHIFT         (12U)
#define GPIO1_IOC_GPIO1A_PULL_1_GPIO1A6_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_1_GPIO1A6_PULL_SHIFT)         /* 0x00003000 */
#define GPIO1_IOC_GPIO1A_PULL_1_GPIO1A7_PULL_SHIFT         (14U)
#define GPIO1_IOC_GPIO1A_PULL_1_GPIO1A7_PULL_MASK          (0x3U << GPIO1_IOC_GPIO1A_PULL_1_GPIO1A7_PULL_SHIFT)         /* 0x0000C000 */
/* GPIO1B_PULL */
#define GPIO1_IOC_GPIO1B_PULL_OFFSET                       (0x10214U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B0_PULL_SHIFT           (0U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B0_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B0_PULL_SHIFT)           /* 0x00000003 */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B1_PULL_SHIFT           (2U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B1_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B1_PULL_SHIFT)           /* 0x0000000C */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B2_PULL_SHIFT           (4U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B2_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B2_PULL_SHIFT)           /* 0x00000030 */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B3_PULL_SHIFT           (6U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B3_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B3_PULL_SHIFT)           /* 0x000000C0 */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B4_PULL_SHIFT           (8U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B4_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B4_PULL_SHIFT)           /* 0x00000300 */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B5_PULL_SHIFT           (10U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B5_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B5_PULL_SHIFT)           /* 0x00000C00 */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B6_PULL_SHIFT           (12U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B6_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B6_PULL_SHIFT)           /* 0x00003000 */
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B7_PULL_SHIFT           (14U)
#define GPIO1_IOC_GPIO1B_PULL_GPIO1B7_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1B_PULL_GPIO1B7_PULL_SHIFT)           /* 0x0000C000 */
/* GPIO1C_PULL */
#define GPIO1_IOC_GPIO1C_PULL_OFFSET                       (0x10218U)
#define GPIO1_IOC_GPIO1C_PULL_GPIO1C0_PULL_SHIFT           (0U)
#define GPIO1_IOC_GPIO1C_PULL_GPIO1C0_PULL_MASK            (0x3U << GPIO1_IOC_GPIO1C_PULL_GPIO1C0_PULL_SHIFT)           /* 0x00000003 */
/* GPIO1A_IE_0 */
#define GPIO1_IOC_GPIO1A_IE_0_OFFSET                       (0x310U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A0_IE_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A0_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_0_GPIO1A0_IE_SHIFT)             /* 0x00000001 */
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A1_IE_SHIFT             (1U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A1_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_0_GPIO1A1_IE_SHIFT)             /* 0x00000002 */
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A2_IE_SHIFT             (2U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A2_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_0_GPIO1A2_IE_SHIFT)             /* 0x00000004 */
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A3_IE_SHIFT             (3U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A3_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_0_GPIO1A3_IE_SHIFT)             /* 0x00000008 */
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A4_IE_SHIFT             (4U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A4_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_0_GPIO1A4_IE_SHIFT)             /* 0x00000010 */
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A5_IE_SHIFT             (5U)
#define GPIO1_IOC_GPIO1A_IE_0_GPIO1A5_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_0_GPIO1A5_IE_SHIFT)             /* 0x00000020 */
/* GPIO1A_IE_1 */
#define GPIO1_IOC_GPIO1A_IE_1_OFFSET                       (0x10310U)
#define GPIO1_IOC_GPIO1A_IE_1_GPIO1A6_IE_SHIFT             (6U)
#define GPIO1_IOC_GPIO1A_IE_1_GPIO1A6_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_1_GPIO1A6_IE_SHIFT)             /* 0x00000040 */
#define GPIO1_IOC_GPIO1A_IE_1_GPIO1A7_IE_SHIFT             (7U)
#define GPIO1_IOC_GPIO1A_IE_1_GPIO1A7_IE_MASK              (0x1U << GPIO1_IOC_GPIO1A_IE_1_GPIO1A7_IE_SHIFT)             /* 0x00000080 */
/* GPIO1B_IE */
#define GPIO1_IOC_GPIO1B_IE_OFFSET                         (0x10314U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B0_IE_SHIFT               (0U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B0_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B0_IE_SHIFT)               /* 0x00000001 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B1_IE_SHIFT               (1U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B1_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B1_IE_SHIFT)               /* 0x00000002 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B2_IE_SHIFT               (2U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B2_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B2_IE_SHIFT)               /* 0x00000004 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B3_IE_SHIFT               (3U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B3_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B3_IE_SHIFT)               /* 0x00000008 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B4_IE_SHIFT               (4U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B4_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B4_IE_SHIFT)               /* 0x00000010 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B5_IE_SHIFT               (5U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B5_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B5_IE_SHIFT)               /* 0x00000020 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B6_IE_SHIFT               (6U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B6_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B6_IE_SHIFT)               /* 0x00000040 */
#define GPIO1_IOC_GPIO1B_IE_GPIO1B7_IE_SHIFT               (7U)
#define GPIO1_IOC_GPIO1B_IE_GPIO1B7_IE_MASK                (0x1U << GPIO1_IOC_GPIO1B_IE_GPIO1B7_IE_SHIFT)               /* 0x00000080 */
/* GPIO1C_IE */
#define GPIO1_IOC_GPIO1C_IE_OFFSET                         (0x10318U)
#define GPIO1_IOC_GPIO1C_IE_GPIO1C0_IE_SHIFT               (0U)
#define GPIO1_IOC_GPIO1C_IE_GPIO1C0_IE_MASK                (0x1U << GPIO1_IOC_GPIO1C_IE_GPIO1C0_IE_SHIFT)               /* 0x00000001 */
/* GPIO1A_SMT_0 */
#define GPIO1_IOC_GPIO1A_SMT_0_OFFSET                      (0x410U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A0_SMT_SHIFT           (0U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A0_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_0_GPIO1A0_SMT_SHIFT)           /* 0x00000001 */
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A1_SMT_SHIFT           (1U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A1_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_0_GPIO1A1_SMT_SHIFT)           /* 0x00000002 */
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A2_SMT_SHIFT           (2U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A2_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_0_GPIO1A2_SMT_SHIFT)           /* 0x00000004 */
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A3_SMT_SHIFT           (3U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A3_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_0_GPIO1A3_SMT_SHIFT)           /* 0x00000008 */
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A4_SMT_SHIFT           (4U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A4_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_0_GPIO1A4_SMT_SHIFT)           /* 0x00000010 */
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A5_SMT_SHIFT           (5U)
#define GPIO1_IOC_GPIO1A_SMT_0_GPIO1A5_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_0_GPIO1A5_SMT_SHIFT)           /* 0x00000020 */
/* GPIO1A_SMT_1 */
#define GPIO1_IOC_GPIO1A_SMT_1_OFFSET                      (0x10410U)
#define GPIO1_IOC_GPIO1A_SMT_1_GPIO1A6_SMT_SHIFT           (6U)
#define GPIO1_IOC_GPIO1A_SMT_1_GPIO1A6_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_1_GPIO1A6_SMT_SHIFT)           /* 0x00000040 */
#define GPIO1_IOC_GPIO1A_SMT_1_GPIO1A7_SMT_SHIFT           (7U)
#define GPIO1_IOC_GPIO1A_SMT_1_GPIO1A7_SMT_MASK            (0x1U << GPIO1_IOC_GPIO1A_SMT_1_GPIO1A7_SMT_SHIFT)           /* 0x00000080 */
/* GPIO1B_SMT */
#define GPIO1_IOC_GPIO1B_SMT_OFFSET                        (0x10414U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B0_SMT_SHIFT             (0U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B0_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B0_SMT_SHIFT)             /* 0x00000001 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B1_SMT_SHIFT             (1U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B1_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B1_SMT_SHIFT)             /* 0x00000002 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B2_SMT_SHIFT             (2U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B2_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B2_SMT_SHIFT)             /* 0x00000004 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B3_SMT_SHIFT             (3U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B3_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B3_SMT_SHIFT)             /* 0x00000008 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B4_SMT_SHIFT             (4U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B4_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B4_SMT_SHIFT)             /* 0x00000010 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B5_SMT_SHIFT             (5U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B5_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B5_SMT_SHIFT)             /* 0x00000020 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B6_SMT_SHIFT             (6U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B6_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B6_SMT_SHIFT)             /* 0x00000040 */
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B7_SMT_SHIFT             (7U)
#define GPIO1_IOC_GPIO1B_SMT_GPIO1B7_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1B_SMT_GPIO1B7_SMT_SHIFT)             /* 0x00000080 */
/* GPIO1C_SMT */
#define GPIO1_IOC_GPIO1C_SMT_OFFSET                        (0x10418U)
#define GPIO1_IOC_GPIO1C_SMT_GPIO1C0_SMT_SHIFT             (0U)
#define GPIO1_IOC_GPIO1C_SMT_GPIO1C0_SMT_MASK              (0x1U << GPIO1_IOC_GPIO1C_SMT_GPIO1C0_SMT_SHIFT)             /* 0x00000001 */
/* GPIO1A_SUS_0 */
#define GPIO1_IOC_GPIO1A_SUS_0_OFFSET                      (0x510U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A0_SUS_SHIFT           (0U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A0_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_0_GPIO1A0_SUS_SHIFT)           /* 0x00000001 */
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A1_SUS_SHIFT           (1U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A1_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_0_GPIO1A1_SUS_SHIFT)           /* 0x00000002 */
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A2_SUS_SHIFT           (2U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A2_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_0_GPIO1A2_SUS_SHIFT)           /* 0x00000004 */
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A3_SUS_SHIFT           (3U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A3_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_0_GPIO1A3_SUS_SHIFT)           /* 0x00000008 */
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A4_SUS_SHIFT           (4U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A4_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_0_GPIO1A4_SUS_SHIFT)           /* 0x00000010 */
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A5_SUS_SHIFT           (5U)
#define GPIO1_IOC_GPIO1A_SUS_0_GPIO1A5_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_0_GPIO1A5_SUS_SHIFT)           /* 0x00000020 */
/* GPIO1A_SUS_1 */
#define GPIO1_IOC_GPIO1A_SUS_1_OFFSET                      (0x10510U)
#define GPIO1_IOC_GPIO1A_SUS_1_GPIO1A6_SUS_SHIFT           (6U)
#define GPIO1_IOC_GPIO1A_SUS_1_GPIO1A6_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_1_GPIO1A6_SUS_SHIFT)           /* 0x00000040 */
#define GPIO1_IOC_GPIO1A_SUS_1_GPIO1A7_SUS_SHIFT           (7U)
#define GPIO1_IOC_GPIO1A_SUS_1_GPIO1A7_SUS_MASK            (0x1U << GPIO1_IOC_GPIO1A_SUS_1_GPIO1A7_SUS_SHIFT)           /* 0x00000080 */
/* GPIO1B_SUS */
#define GPIO1_IOC_GPIO1B_SUS_OFFSET                        (0x10514U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B0_SUS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B0_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B0_SUS_SHIFT)             /* 0x00000001 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B1_SUS_SHIFT             (1U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B1_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B1_SUS_SHIFT)             /* 0x00000002 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B2_SUS_SHIFT             (2U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B2_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B2_SUS_SHIFT)             /* 0x00000004 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B3_SUS_SHIFT             (3U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B3_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B3_SUS_SHIFT)             /* 0x00000008 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B4_SUS_SHIFT             (4U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B4_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B4_SUS_SHIFT)             /* 0x00000010 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B5_SUS_SHIFT             (5U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B5_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B5_SUS_SHIFT)             /* 0x00000020 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B6_SUS_SHIFT             (6U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B6_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B6_SUS_SHIFT)             /* 0x00000040 */
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B7_SUS_SHIFT             (7U)
#define GPIO1_IOC_GPIO1B_SUS_GPIO1B7_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1B_SUS_GPIO1B7_SUS_SHIFT)             /* 0x00000080 */
/* GPIO1C_SUS */
#define GPIO1_IOC_GPIO1C_SUS_OFFSET                        (0x10518U)
#define GPIO1_IOC_GPIO1C_SUS_GPIO1C0_SUS_SHIFT             (0U)
#define GPIO1_IOC_GPIO1C_SUS_GPIO1C0_SUS_MASK              (0x1U << GPIO1_IOC_GPIO1C_SUS_GPIO1C0_SUS_SHIFT)             /* 0x00000001 */
/* GPIO1A_SL_0 */
#define GPIO1_IOC_GPIO1A_SL_0_OFFSET                       (0x610U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A0_SL_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A0_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_0_GPIO1A0_SL_SHIFT)             /* 0x00000003 */
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A1_SL_SHIFT             (2U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A1_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_0_GPIO1A1_SL_SHIFT)             /* 0x0000000C */
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A2_SL_SHIFT             (4U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A2_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_0_GPIO1A2_SL_SHIFT)             /* 0x00000030 */
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A3_SL_SHIFT             (6U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A3_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_0_GPIO1A3_SL_SHIFT)             /* 0x000000C0 */
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A4_SL_SHIFT             (8U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A4_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_0_GPIO1A4_SL_SHIFT)             /* 0x00000300 */
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A5_SL_SHIFT             (10U)
#define GPIO1_IOC_GPIO1A_SL_0_GPIO1A5_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_0_GPIO1A5_SL_SHIFT)             /* 0x00000C00 */
/* GPIO1A_SL_1 */
#define GPIO1_IOC_GPIO1A_SL_1_OFFSET                       (0x10610U)
#define GPIO1_IOC_GPIO1A_SL_1_GPIO1A6_SL_SHIFT             (12U)
#define GPIO1_IOC_GPIO1A_SL_1_GPIO1A6_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_1_GPIO1A6_SL_SHIFT)             /* 0x00003000 */
#define GPIO1_IOC_GPIO1A_SL_1_GPIO1A7_SL_SHIFT             (14U)
#define GPIO1_IOC_GPIO1A_SL_1_GPIO1A7_SL_MASK              (0x3U << GPIO1_IOC_GPIO1A_SL_1_GPIO1A7_SL_SHIFT)             /* 0x0000C000 */
/* GPIO1B_SL */
#define GPIO1_IOC_GPIO1B_SL_OFFSET                         (0x10614U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B0_SL_SHIFT               (0U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B0_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B0_SL_SHIFT)               /* 0x00000003 */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B1_SL_SHIFT               (2U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B1_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B1_SL_SHIFT)               /* 0x0000000C */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B2_SL_SHIFT               (4U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B2_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B2_SL_SHIFT)               /* 0x00000030 */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B3_SL_SHIFT               (6U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B3_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B3_SL_SHIFT)               /* 0x000000C0 */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B4_SL_SHIFT               (8U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B4_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B4_SL_SHIFT)               /* 0x00000300 */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B5_SL_SHIFT               (10U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B5_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B5_SL_SHIFT)               /* 0x00000C00 */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B6_SL_SHIFT               (12U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B6_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B6_SL_SHIFT)               /* 0x00003000 */
#define GPIO1_IOC_GPIO1B_SL_GPIO1B7_SL_SHIFT               (14U)
#define GPIO1_IOC_GPIO1B_SL_GPIO1B7_SL_MASK                (0x3U << GPIO1_IOC_GPIO1B_SL_GPIO1B7_SL_SHIFT)               /* 0x0000C000 */
/* GPIO1C_SL */
#define GPIO1_IOC_GPIO1C_SL_OFFSET                         (0x10618U)
#define GPIO1_IOC_GPIO1C_SL_GPIO1C0_SL_SHIFT               (0U)
#define GPIO1_IOC_GPIO1C_SL_GPIO1C0_SL_MASK                (0x3U << GPIO1_IOC_GPIO1C_SL_GPIO1C0_SL_SHIFT)               /* 0x00000003 */
/* GPIO1A_OD_0 */
#define GPIO1_IOC_GPIO1A_OD_0_OFFSET                       (0x710U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A0_OD_SHIFT             (0U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A0_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_0_GPIO1A0_OD_SHIFT)             /* 0x00000001 */
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A1_OD_SHIFT             (1U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A1_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_0_GPIO1A1_OD_SHIFT)             /* 0x00000002 */
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A2_OD_SHIFT             (2U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A2_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_0_GPIO1A2_OD_SHIFT)             /* 0x00000004 */
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A3_OD_SHIFT             (3U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A3_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_0_GPIO1A3_OD_SHIFT)             /* 0x00000008 */
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A4_OD_SHIFT             (4U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A4_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_0_GPIO1A4_OD_SHIFT)             /* 0x00000010 */
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A5_OD_SHIFT             (5U)
#define GPIO1_IOC_GPIO1A_OD_0_GPIO1A5_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_0_GPIO1A5_OD_SHIFT)             /* 0x00000020 */
/* GPIO1A_OD_1 */
#define GPIO1_IOC_GPIO1A_OD_1_OFFSET                       (0x10710U)
#define GPIO1_IOC_GPIO1A_OD_1_GPIO1A6_OD_SHIFT             (6U)
#define GPIO1_IOC_GPIO1A_OD_1_GPIO1A6_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_1_GPIO1A6_OD_SHIFT)             /* 0x00000040 */
#define GPIO1_IOC_GPIO1A_OD_1_GPIO1A7_OD_SHIFT             (7U)
#define GPIO1_IOC_GPIO1A_OD_1_GPIO1A7_OD_MASK              (0x1U << GPIO1_IOC_GPIO1A_OD_1_GPIO1A7_OD_SHIFT)             /* 0x00000080 */
/* GPIO1B_OD */
#define GPIO1_IOC_GPIO1B_OD_OFFSET                         (0x10714U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B0_OD_SHIFT               (0U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B0_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B0_OD_SHIFT)               /* 0x00000001 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B1_OD_SHIFT               (1U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B1_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B1_OD_SHIFT)               /* 0x00000002 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B2_OD_SHIFT               (2U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B2_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B2_OD_SHIFT)               /* 0x00000004 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B3_OD_SHIFT               (3U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B3_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B3_OD_SHIFT)               /* 0x00000008 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B4_OD_SHIFT               (4U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B4_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B4_OD_SHIFT)               /* 0x00000010 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B5_OD_SHIFT               (5U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B5_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B5_OD_SHIFT)               /* 0x00000020 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B6_OD_SHIFT               (6U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B6_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B6_OD_SHIFT)               /* 0x00000040 */
#define GPIO1_IOC_GPIO1B_OD_GPIO1B7_OD_SHIFT               (7U)
#define GPIO1_IOC_GPIO1B_OD_GPIO1B7_OD_MASK                (0x1U << GPIO1_IOC_GPIO1B_OD_GPIO1B7_OD_SHIFT)               /* 0x00000080 */
/* GPIO1C_OD */
#define GPIO1_IOC_GPIO1C_OD_OFFSET                         (0x10718U)
#define GPIO1_IOC_GPIO1C_OD_GPIO1C0_OD_SHIFT               (0U)
#define GPIO1_IOC_GPIO1C_OD_GPIO1C0_OD_MASK                (0x1U << GPIO1_IOC_GPIO1C_OD_GPIO1C0_OD_SHIFT)               /* 0x00000001 */
/* IO_VSEL_VCCIO3 */
#define GPIO1_IOC_IO_VSEL_VCCIO3_OFFSET                    (0x800U)
#define GPIO1_IOC_IO_VSEL_VCCIO3_POC_VCCIO3_IDDQ_SHIFT     (0U)
#define GPIO1_IOC_IO_VSEL_VCCIO3_POC_VCCIO3_IDDQ_MASK      (0x1U << GPIO1_IOC_IO_VSEL_VCCIO3_POC_VCCIO3_IDDQ_SHIFT)     /* 0x00000001 */
#define GPIO1_IOC_IO_VSEL_VCCIO3_POC_VCCIO3_VD_SHIFT       (15U)
#define GPIO1_IOC_IO_VSEL_VCCIO3_POC_VCCIO3_VD_MASK        (0x1U << GPIO1_IOC_IO_VSEL_VCCIO3_POC_VCCIO3_VD_SHIFT)       /* 0x00008000 */
/* IO_VSEL_VCCIO4 */
#define GPIO1_IOC_IO_VSEL_VCCIO4_OFFSET                    (0x10800U)
#define GPIO1_IOC_IO_VSEL_VCCIO4_POC_VCCIO4_IDDQ_SHIFT     (0U)
#define GPIO1_IOC_IO_VSEL_VCCIO4_POC_VCCIO4_IDDQ_MASK      (0x1U << GPIO1_IOC_IO_VSEL_VCCIO4_POC_VCCIO4_IDDQ_SHIFT)     /* 0x00000001 */
#define GPIO1_IOC_IO_VSEL_VCCIO4_POC_VCCIO4_VD_SHIFT       (15U)
#define GPIO1_IOC_IO_VSEL_VCCIO4_POC_VCCIO4_VD_MASK        (0x1U << GPIO1_IOC_IO_VSEL_VCCIO4_POC_VCCIO4_VD_SHIFT)       /* 0x00008000 */
/* IO_VSEL_VCCIO7 */
#define GPIO1_IOC_IO_VSEL_VCCIO7_OFFSET                    (0x10804U)
#define GPIO1_IOC_IO_VSEL_VCCIO7_POC_VCCIO7_IDDQ_SHIFT     (0U)
#define GPIO1_IOC_IO_VSEL_VCCIO7_POC_VCCIO7_IDDQ_MASK      (0x1U << GPIO1_IOC_IO_VSEL_VCCIO7_POC_VCCIO7_IDDQ_SHIFT)     /* 0x00000001 */
#define GPIO1_IOC_IO_VSEL_VCCIO7_POC_VCCIO7_VD_SHIFT       (15U)
#define GPIO1_IOC_IO_VSEL_VCCIO7_POC_VCCIO7_VD_MASK        (0x1U << GPIO1_IOC_IO_VSEL_VCCIO7_POC_VCCIO7_VD_SHIFT)       /* 0x00008000 */
/* MISC_CON */
#define GPIO1_IOC_MISC_CON_OFFSET                          (0x10808U)
#define GPIO1_IOC_MISC_CON_FSPI_CLK_IOE__SHIFT             (0U)
#define GPIO1_IOC_MISC_CON_FSPI_CLK_IOE__MASK              (0x1U << GPIO1_IOC_MISC_CON_FSPI_CLK_IOE__SHIFT)             /* 0x00000001 */
/* SDCARD_IO_CON */
#define GPIO1_IOC_SDCARD_IO_CON_OFFSET                     (0x1080CU)
#define GPIO1_IOC_SDCARD_IO_CON_SDCARD_DECTN_DLY_SHIFT     (0U)
#define GPIO1_IOC_SDCARD_IO_CON_SDCARD_DECTN_DLY_MASK      (0xFFFFFFFFU << GPIO1_IOC_SDCARD_IO_CON_SDCARD_DECTN_DLY_SHIFT) /* 0xFFFFFFFF */
/* JTAG_M2_CON */
#define GPIO1_IOC_JTAG_M2_CON_OFFSET                       (0x10810U)
#define GPIO1_IOC_JTAG_M2_CON_FORCE_JTAG_SHIFT             (0U)
#define GPIO1_IOC_JTAG_M2_CON_FORCE_JTAG_MASK              (0x1U << GPIO1_IOC_JTAG_M2_CON_FORCE_JTAG_SHIFT)             /* 0x00000001 */
/***************************************GPIO2_IOC****************************************/
/* GPIO2A_IOMUX_SEL_0 */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_OFFSET                (0x40U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A0_SEL_SHIFT     (0U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A0_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A0_SEL_SHIFT)     /* 0x0000000F */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A1_SEL_SHIFT     (4U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A1_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A1_SEL_SHIFT)     /* 0x000000F0 */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A2_SEL_SHIFT     (8U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A2_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A2_SEL_SHIFT)     /* 0x00000F00 */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A3_SEL_SHIFT     (12U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A3_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_0_GPIO2A3_SEL_SHIFT)     /* 0x0000F000 */
/* GPIO2A_IOMUX_SEL_1 */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_OFFSET                (0x44U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A4_SEL_SHIFT     (0U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A4_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A4_SEL_SHIFT)     /* 0x0000000F */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A5_SEL_SHIFT     (4U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A5_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A5_SEL_SHIFT)     /* 0x000000F0 */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A6_SEL_SHIFT     (8U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A6_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A6_SEL_SHIFT)     /* 0x00000F00 */
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A7_SEL_SHIFT     (12U)
#define GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A7_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2A_IOMUX_SEL_1_GPIO2A7_SEL_SHIFT)     /* 0x0000F000 */
/* GPIO2B_IOMUX_SEL_0 */
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_OFFSET                (0x48U)
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B0_SEL_SHIFT     (0U)
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B0_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B0_SEL_SHIFT)     /* 0x0000000F */
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B1_SEL_SHIFT     (4U)
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B1_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B1_SEL_SHIFT)     /* 0x000000F0 */
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B2_SEL_SHIFT     (8U)
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B2_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B2_SEL_SHIFT)     /* 0x00000F00 */
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B3_SEL_SHIFT     (12U)
#define GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B3_SEL_MASK      (0xFU << GPIO2_IOC_GPIO2B_IOMUX_SEL_0_GPIO2B3_SEL_SHIFT)     /* 0x0000F000 */
/* GPIO2A_DS_0 */
#define GPIO2_IOC_GPIO2A_DS_0_OFFSET                       (0x180U)
#define GPIO2_IOC_GPIO2A_DS_0_GPIO2A0_DS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2A_DS_0_GPIO2A0_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_0_GPIO2A0_DS_SHIFT)            /* 0x0000003F */
#define GPIO2_IOC_GPIO2A_DS_0_GPIO2A1_DS_SHIFT             (8U)
#define GPIO2_IOC_GPIO2A_DS_0_GPIO2A1_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_0_GPIO2A1_DS_SHIFT)            /* 0x00003F00 */
/* GPIO2A_DS_1 */
#define GPIO2_IOC_GPIO2A_DS_1_OFFSET                       (0x184U)
#define GPIO2_IOC_GPIO2A_DS_1_GPIO2A2_DS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2A_DS_1_GPIO2A2_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_1_GPIO2A2_DS_SHIFT)            /* 0x0000003F */
#define GPIO2_IOC_GPIO2A_DS_1_GPIO2A3_DS_SHIFT             (8U)
#define GPIO2_IOC_GPIO2A_DS_1_GPIO2A3_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_1_GPIO2A3_DS_SHIFT)            /* 0x00003F00 */
/* GPIO2A_DS_2 */
#define GPIO2_IOC_GPIO2A_DS_2_OFFSET                       (0x188U)
#define GPIO2_IOC_GPIO2A_DS_2_GPIO2A4_DS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2A_DS_2_GPIO2A4_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_2_GPIO2A4_DS_SHIFT)            /* 0x0000003F */
#define GPIO2_IOC_GPIO2A_DS_2_GPIO2A5_DS_SHIFT             (8U)
#define GPIO2_IOC_GPIO2A_DS_2_GPIO2A5_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_2_GPIO2A5_DS_SHIFT)            /* 0x00003F00 */
/* GPIO2A_DS_3 */
#define GPIO2_IOC_GPIO2A_DS_3_OFFSET                       (0x18CU)
#define GPIO2_IOC_GPIO2A_DS_3_GPIO2A6_DS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2A_DS_3_GPIO2A6_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_3_GPIO2A6_DS_SHIFT)            /* 0x0000003F */
#define GPIO2_IOC_GPIO2A_DS_3_GPIO2A7_DS_SHIFT             (8U)
#define GPIO2_IOC_GPIO2A_DS_3_GPIO2A7_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2A_DS_3_GPIO2A7_DS_SHIFT)            /* 0x00003F00 */
/* GPIO2B_DS_0 */
#define GPIO2_IOC_GPIO2B_DS_0_OFFSET                       (0x190U)
#define GPIO2_IOC_GPIO2B_DS_0_GPIO2B0_DS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2B_DS_0_GPIO2B0_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2B_DS_0_GPIO2B0_DS_SHIFT)            /* 0x0000003F */
#define GPIO2_IOC_GPIO2B_DS_0_GPIO2B1_DS_SHIFT             (8U)
#define GPIO2_IOC_GPIO2B_DS_0_GPIO2B1_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2B_DS_0_GPIO2B1_DS_SHIFT)            /* 0x00003F00 */
/* GPIO2B_DS_1 */
#define GPIO2_IOC_GPIO2B_DS_1_OFFSET                       (0x194U)
#define GPIO2_IOC_GPIO2B_DS_1_GPIO2B2_DS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2B_DS_1_GPIO2B2_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2B_DS_1_GPIO2B2_DS_SHIFT)            /* 0x0000003F */
#define GPIO2_IOC_GPIO2B_DS_1_GPIO2B3_DS_SHIFT             (8U)
#define GPIO2_IOC_GPIO2B_DS_1_GPIO2B3_DS_MASK              (0x3FU << GPIO2_IOC_GPIO2B_DS_1_GPIO2B3_DS_SHIFT)            /* 0x00003F00 */
/* GPIO2A_PULL */
#define GPIO2_IOC_GPIO2A_PULL_OFFSET                       (0x220U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A0_PULL_SHIFT           (0U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A0_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A0_PULL_SHIFT)           /* 0x00000003 */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A1_PULL_SHIFT           (2U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A1_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A1_PULL_SHIFT)           /* 0x0000000C */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A2_PULL_SHIFT           (4U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A2_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A2_PULL_SHIFT)           /* 0x00000030 */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A3_PULL_SHIFT           (6U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A3_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A3_PULL_SHIFT)           /* 0x000000C0 */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A4_PULL_SHIFT           (8U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A4_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A4_PULL_SHIFT)           /* 0x00000300 */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A5_PULL_SHIFT           (10U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A5_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A5_PULL_SHIFT)           /* 0x00000C00 */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A6_PULL_SHIFT           (12U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A6_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A6_PULL_SHIFT)           /* 0x00003000 */
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A7_PULL_SHIFT           (14U)
#define GPIO2_IOC_GPIO2A_PULL_GPIO2A7_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2A_PULL_GPIO2A7_PULL_SHIFT)           /* 0x0000C000 */
/* GPIO2B_PULL */
#define GPIO2_IOC_GPIO2B_PULL_OFFSET                       (0x224U)
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B0_PULL_SHIFT           (0U)
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B0_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2B_PULL_GPIO2B0_PULL_SHIFT)           /* 0x00000003 */
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B1_PULL_SHIFT           (2U)
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B1_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2B_PULL_GPIO2B1_PULL_SHIFT)           /* 0x0000000C */
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B2_PULL_SHIFT           (4U)
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B2_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2B_PULL_GPIO2B2_PULL_SHIFT)           /* 0x00000030 */
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B3_PULL_SHIFT           (6U)
#define GPIO2_IOC_GPIO2B_PULL_GPIO2B3_PULL_MASK            (0x3U << GPIO2_IOC_GPIO2B_PULL_GPIO2B3_PULL_SHIFT)           /* 0x000000C0 */
/* GPIO2A_IE */
#define GPIO2_IOC_GPIO2A_IE_OFFSET                         (0x320U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A0_IE_SHIFT               (0U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A0_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A0_IE_SHIFT)               /* 0x00000001 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A1_IE_SHIFT               (1U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A1_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A1_IE_SHIFT)               /* 0x00000002 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A2_IE_SHIFT               (2U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A2_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A2_IE_SHIFT)               /* 0x00000004 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A3_IE_SHIFT               (3U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A3_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A3_IE_SHIFT)               /* 0x00000008 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A4_IE_SHIFT               (4U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A4_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A4_IE_SHIFT)               /* 0x00000010 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A5_IE_SHIFT               (5U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A5_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A5_IE_SHIFT)               /* 0x00000020 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A6_IE_SHIFT               (6U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A6_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A6_IE_SHIFT)               /* 0x00000040 */
#define GPIO2_IOC_GPIO2A_IE_GPIO2A7_IE_SHIFT               (7U)
#define GPIO2_IOC_GPIO2A_IE_GPIO2A7_IE_MASK                (0x1U << GPIO2_IOC_GPIO2A_IE_GPIO2A7_IE_SHIFT)               /* 0x00000080 */
/* GPIO2B_IE */
#define GPIO2_IOC_GPIO2B_IE_OFFSET                         (0x324U)
#define GPIO2_IOC_GPIO2B_IE_GPIO2B0_IE_SHIFT               (0U)
#define GPIO2_IOC_GPIO2B_IE_GPIO2B0_IE_MASK                (0x1U << GPIO2_IOC_GPIO2B_IE_GPIO2B0_IE_SHIFT)               /* 0x00000001 */
#define GPIO2_IOC_GPIO2B_IE_GPIO2B1_IE_SHIFT               (1U)
#define GPIO2_IOC_GPIO2B_IE_GPIO2B1_IE_MASK                (0x1U << GPIO2_IOC_GPIO2B_IE_GPIO2B1_IE_SHIFT)               /* 0x00000002 */
#define GPIO2_IOC_GPIO2B_IE_GPIO2B2_IE_SHIFT               (2U)
#define GPIO2_IOC_GPIO2B_IE_GPIO2B2_IE_MASK                (0x1U << GPIO2_IOC_GPIO2B_IE_GPIO2B2_IE_SHIFT)               /* 0x00000004 */
#define GPIO2_IOC_GPIO2B_IE_GPIO2B3_IE_SHIFT               (3U)
#define GPIO2_IOC_GPIO2B_IE_GPIO2B3_IE_MASK                (0x1U << GPIO2_IOC_GPIO2B_IE_GPIO2B3_IE_SHIFT)               /* 0x00000008 */
/* GPIO2A_SMT */
#define GPIO2_IOC_GPIO2A_SMT_OFFSET                        (0x420U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A0_SMT_SHIFT             (0U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A0_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A0_SMT_SHIFT)             /* 0x00000001 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A1_SMT_SHIFT             (1U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A1_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A1_SMT_SHIFT)             /* 0x00000002 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A2_SMT_SHIFT             (2U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A2_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A2_SMT_SHIFT)             /* 0x00000004 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A3_SMT_SHIFT             (3U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A3_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A3_SMT_SHIFT)             /* 0x00000008 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A4_SMT_SHIFT             (4U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A4_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A4_SMT_SHIFT)             /* 0x00000010 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A5_SMT_SHIFT             (5U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A5_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A5_SMT_SHIFT)             /* 0x00000020 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A6_SMT_SHIFT             (6U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A6_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A6_SMT_SHIFT)             /* 0x00000040 */
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A7_SMT_SHIFT             (7U)
#define GPIO2_IOC_GPIO2A_SMT_GPIO2A7_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2A_SMT_GPIO2A7_SMT_SHIFT)             /* 0x00000080 */
/* GPIO2B_SMT */
#define GPIO2_IOC_GPIO2B_SMT_OFFSET                        (0x424U)
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B0_SMT_SHIFT             (0U)
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B0_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2B_SMT_GPIO2B0_SMT_SHIFT)             /* 0x00000001 */
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B1_SMT_SHIFT             (1U)
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B1_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2B_SMT_GPIO2B1_SMT_SHIFT)             /* 0x00000002 */
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B2_SMT_SHIFT             (2U)
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B2_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2B_SMT_GPIO2B2_SMT_SHIFT)             /* 0x00000004 */
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B3_SMT_SHIFT             (3U)
#define GPIO2_IOC_GPIO2B_SMT_GPIO2B3_SMT_MASK              (0x1U << GPIO2_IOC_GPIO2B_SMT_GPIO2B3_SMT_SHIFT)             /* 0x00000008 */
/* GPIO2A_SUS */
#define GPIO2_IOC_GPIO2A_SUS_OFFSET                        (0x520U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A0_SUS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A0_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A0_SUS_SHIFT)             /* 0x00000001 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A1_SUS_SHIFT             (1U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A1_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A1_SUS_SHIFT)             /* 0x00000002 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A2_SUS_SHIFT             (2U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A2_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A2_SUS_SHIFT)             /* 0x00000004 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A3_SUS_SHIFT             (3U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A3_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A3_SUS_SHIFT)             /* 0x00000008 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A4_SUS_SHIFT             (4U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A4_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A4_SUS_SHIFT)             /* 0x00000010 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A5_SUS_SHIFT             (5U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A5_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A5_SUS_SHIFT)             /* 0x00000020 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A6_SUS_SHIFT             (6U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A6_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A6_SUS_SHIFT)             /* 0x00000040 */
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A7_SUS_SHIFT             (7U)
#define GPIO2_IOC_GPIO2A_SUS_GPIO2A7_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2A_SUS_GPIO2A7_SUS_SHIFT)             /* 0x00000080 */
/* GPIO2B_SUS */
#define GPIO2_IOC_GPIO2B_SUS_OFFSET                        (0x524U)
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B0_SUS_SHIFT             (0U)
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B0_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2B_SUS_GPIO2B0_SUS_SHIFT)             /* 0x00000001 */
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B1_SUS_SHIFT             (1U)
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B1_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2B_SUS_GPIO2B1_SUS_SHIFT)             /* 0x00000002 */
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B2_SUS_SHIFT             (2U)
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B2_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2B_SUS_GPIO2B2_SUS_SHIFT)             /* 0x00000004 */
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B3_SUS_SHIFT             (3U)
#define GPIO2_IOC_GPIO2B_SUS_GPIO2B3_SUS_MASK              (0x1U << GPIO2_IOC_GPIO2B_SUS_GPIO2B3_SUS_SHIFT)             /* 0x00000008 */
/* GPIO2A_SL */
#define GPIO2_IOC_GPIO2A_SL_OFFSET                         (0x620U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A0_SL_SHIFT               (0U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A0_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A0_SL_SHIFT)               /* 0x00000003 */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A1_SL_SHIFT               (2U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A1_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A1_SL_SHIFT)               /* 0x0000000C */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A2_SL_SHIFT               (4U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A2_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A2_SL_SHIFT)               /* 0x00000030 */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A3_SL_SHIFT               (6U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A3_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A3_SL_SHIFT)               /* 0x000000C0 */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A4_SL_SHIFT               (8U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A4_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A4_SL_SHIFT)               /* 0x00000300 */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A5_SL_SHIFT               (10U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A5_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A5_SL_SHIFT)               /* 0x00000C00 */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A6_SL_SHIFT               (12U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A6_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A6_SL_SHIFT)               /* 0x00003000 */
#define GPIO2_IOC_GPIO2A_SL_GPIO2A7_SL_SHIFT               (14U)
#define GPIO2_IOC_GPIO2A_SL_GPIO2A7_SL_MASK                (0x3U << GPIO2_IOC_GPIO2A_SL_GPIO2A7_SL_SHIFT)               /* 0x0000C000 */
/* GPIO2B_SL */
#define GPIO2_IOC_GPIO2B_SL_OFFSET                         (0x624U)
#define GPIO2_IOC_GPIO2B_SL_GPIO2B0_SL_SHIFT               (0U)
#define GPIO2_IOC_GPIO2B_SL_GPIO2B0_SL_MASK                (0x3U << GPIO2_IOC_GPIO2B_SL_GPIO2B0_SL_SHIFT)               /* 0x00000003 */
#define GPIO2_IOC_GPIO2B_SL_GPIO2B1_SL_SHIFT               (2U)
#define GPIO2_IOC_GPIO2B_SL_GPIO2B1_SL_MASK                (0x3U << GPIO2_IOC_GPIO2B_SL_GPIO2B1_SL_SHIFT)               /* 0x0000000C */
#define GPIO2_IOC_GPIO2B_SL_GPIO2B2_SL_SHIFT               (4U)
#define GPIO2_IOC_GPIO2B_SL_GPIO2B2_SL_MASK                (0x3U << GPIO2_IOC_GPIO2B_SL_GPIO2B2_SL_SHIFT)               /* 0x00000030 */
#define GPIO2_IOC_GPIO2B_SL_GPIO2B3_SL_SHIFT               (6U)
#define GPIO2_IOC_GPIO2B_SL_GPIO2B3_SL_MASK                (0x3U << GPIO2_IOC_GPIO2B_SL_GPIO2B3_SL_SHIFT)               /* 0x000000C0 */
/* GPIO2A_OD */
#define GPIO2_IOC_GPIO2A_OD_OFFSET                         (0x720U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A0_OD_SHIFT               (0U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A0_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A0_OD_SHIFT)               /* 0x00000001 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A1_OD_SHIFT               (1U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A1_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A1_OD_SHIFT)               /* 0x00000002 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A2_OD_SHIFT               (2U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A2_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A2_OD_SHIFT)               /* 0x00000004 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A3_OD_SHIFT               (3U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A3_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A3_OD_SHIFT)               /* 0x00000008 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A4_OD_SHIFT               (4U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A4_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A4_OD_SHIFT)               /* 0x00000010 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A5_OD_SHIFT               (5U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A5_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A5_OD_SHIFT)               /* 0x00000020 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A6_OD_SHIFT               (6U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A6_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A6_OD_SHIFT)               /* 0x00000040 */
#define GPIO2_IOC_GPIO2A_OD_GPIO2A7_OD_SHIFT               (7U)
#define GPIO2_IOC_GPIO2A_OD_GPIO2A7_OD_MASK                (0x1U << GPIO2_IOC_GPIO2A_OD_GPIO2A7_OD_SHIFT)               /* 0x00000080 */
/* GPIO2B_OD */
#define GPIO2_IOC_GPIO2B_OD_OFFSET                         (0x724U)
#define GPIO2_IOC_GPIO2B_OD_GPIO2B0_OD_SHIFT               (0U)
#define GPIO2_IOC_GPIO2B_OD_GPIO2B0_OD_MASK                (0x1U << GPIO2_IOC_GPIO2B_OD_GPIO2B0_OD_SHIFT)               /* 0x00000001 */
#define GPIO2_IOC_GPIO2B_OD_GPIO2B1_OD_SHIFT               (1U)
#define GPIO2_IOC_GPIO2B_OD_GPIO2B1_OD_MASK                (0x1U << GPIO2_IOC_GPIO2B_OD_GPIO2B1_OD_SHIFT)               /* 0x00000002 */
#define GPIO2_IOC_GPIO2B_OD_GPIO2B2_OD_SHIFT               (2U)
#define GPIO2_IOC_GPIO2B_OD_GPIO2B2_OD_MASK                (0x1U << GPIO2_IOC_GPIO2B_OD_GPIO2B2_OD_SHIFT)               /* 0x00000004 */
#define GPIO2_IOC_GPIO2B_OD_GPIO2B3_OD_SHIFT               (3U)
#define GPIO2_IOC_GPIO2B_OD_GPIO2B3_OD_MASK                (0x1U << GPIO2_IOC_GPIO2B_OD_GPIO2B3_OD_SHIFT)               /* 0x00000008 */
/* IO_VSEL_VCCIO6 */
#define GPIO2_IOC_IO_VSEL_VCCIO6_OFFSET                    (0x800U)
#define GPIO2_IOC_IO_VSEL_VCCIO6_POC_VCCIO6_IDDQ_SHIFT     (0U)
#define GPIO2_IOC_IO_VSEL_VCCIO6_POC_VCCIO6_IDDQ_MASK      (0x1U << GPIO2_IOC_IO_VSEL_VCCIO6_POC_VCCIO6_IDDQ_SHIFT)     /* 0x00000001 */
#define GPIO2_IOC_IO_VSEL_VCCIO6_POC_VCCIO6_VD_SHIFT       (15U)
#define GPIO2_IOC_IO_VSEL_VCCIO6_POC_VCCIO6_VD_MASK        (0x1U << GPIO2_IOC_IO_VSEL_VCCIO6_POC_VCCIO6_VD_SHIFT)       /* 0x00008000 */
/* MISC_CON */
#define GPIO2_IOC_MISC_CON_OFFSET                          (0x804U)
#define GPIO2_IOC_MISC_CON_SAI_MCLK_IOE__SHIFT             (0U)
#define GPIO2_IOC_MISC_CON_SAI_MCLK_IOE__MASK              (0x1U << GPIO2_IOC_MISC_CON_SAI_MCLK_IOE__SHIFT)             /* 0x00000001 */
#define GPIO2_IOC_MISC_CON_NPU_AVS_ENABLE_SHIFT            (15U)
#define GPIO2_IOC_MISC_CON_NPU_AVS_ENABLE_MASK             (0x1U << GPIO2_IOC_MISC_CON_NPU_AVS_ENABLE_SHIFT)            /* 0x00008000 */
/* SARADC_CON0 */
#define GPIO2_IOC_SARADC_CON0_OFFSET                       (0x80CU)
#define GPIO2_IOC_SARADC_CON0_SARADC_ANA_REG_SHIFT         (0U)
#define GPIO2_IOC_SARADC_CON0_SARADC_ANA_REG_MASK          (0xFFFFU << GPIO2_IOC_SARADC_CON0_SARADC_ANA_REG_SHIFT)      /* 0x0000FFFF */
/* SARADC_CON1 */
#define GPIO2_IOC_SARADC_CON1_OFFSET                       (0x810U)
#define GPIO2_IOC_SARADC_CON1_SARADC_PD_SEL_SHIFT          (0U)
#define GPIO2_IOC_SARADC_CON1_SARADC_PD_SEL_MASK           (0x1U << GPIO2_IOC_SARADC_CON1_SARADC_PD_SEL_SHIFT)          /* 0x00000001 */
#define GPIO2_IOC_SARADC_CON1_SARADC_PD_SHIFT              (1U)
#define GPIO2_IOC_SARADC_CON1_SARADC_PD_MASK               (0x1U << GPIO2_IOC_SARADC_CON1_SARADC_PD_SHIFT)              /* 0x00000002 */
/**************************************GPIO0_A_IOC***************************************/
/* GPIO0A_IOMUX_SEL_0 */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_OFFSET              (0x0U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A0_SEL_SHIFT   (0U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A0_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A0_SEL_SHIFT)   /* 0x0000000F */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A1_SEL_SHIFT   (4U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A1_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A1_SEL_SHIFT)   /* 0x000000F0 */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A2_SEL_SHIFT   (8U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A2_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A2_SEL_SHIFT)   /* 0x00000F00 */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A3_SEL_SHIFT   (12U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A3_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_0_GPIO0A3_SEL_SHIFT)   /* 0x0000F000 */
/* GPIO0A_IOMUX_SEL_1 */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_OFFSET              (0x4U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A4_SEL_SHIFT   (0U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A4_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A4_SEL_SHIFT)   /* 0x0000000F */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A5_SEL_SHIFT   (4U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A5_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A5_SEL_SHIFT)   /* 0x000000F0 */
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A6_SEL_SHIFT   (8U)
#define GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A6_SEL_MASK    (0xFU << GPIO0_A_IOC_GPIO0A_IOMUX_SEL_1_GPIO0A6_SEL_SHIFT)   /* 0x00000F00 */
/* GPIO0A_DS_0 */
#define GPIO0_A_IOC_GPIO0A_DS_0_OFFSET                     (0x100U)
#define GPIO0_A_IOC_GPIO0A_DS_0_GPIO0A0_DS_SHIFT           (0U)
#define GPIO0_A_IOC_GPIO0A_DS_0_GPIO0A0_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_0_GPIO0A0_DS_SHIFT)          /* 0x0000003F */
#define GPIO0_A_IOC_GPIO0A_DS_0_GPIO0A1_DS_SHIFT           (8U)
#define GPIO0_A_IOC_GPIO0A_DS_0_GPIO0A1_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_0_GPIO0A1_DS_SHIFT)          /* 0x00003F00 */
/* GPIO0A_DS_1 */
#define GPIO0_A_IOC_GPIO0A_DS_1_OFFSET                     (0x104U)
#define GPIO0_A_IOC_GPIO0A_DS_1_GPIO0A2_DS_SHIFT           (0U)
#define GPIO0_A_IOC_GPIO0A_DS_1_GPIO0A2_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_1_GPIO0A2_DS_SHIFT)          /* 0x0000003F */
#define GPIO0_A_IOC_GPIO0A_DS_1_GPIO0A3_DS_SHIFT           (8U)
#define GPIO0_A_IOC_GPIO0A_DS_1_GPIO0A3_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_1_GPIO0A3_DS_SHIFT)          /* 0x00003F00 */
/* GPIO0A_DS_2 */
#define GPIO0_A_IOC_GPIO0A_DS_2_OFFSET                     (0x108U)
#define GPIO0_A_IOC_GPIO0A_DS_2_GPIO0A4_DS_SHIFT           (0U)
#define GPIO0_A_IOC_GPIO0A_DS_2_GPIO0A4_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_2_GPIO0A4_DS_SHIFT)          /* 0x0000003F */
#define GPIO0_A_IOC_GPIO0A_DS_2_GPIO0A5_DS_SHIFT           (8U)
#define GPIO0_A_IOC_GPIO0A_DS_2_GPIO0A5_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_2_GPIO0A5_DS_SHIFT)          /* 0x00003F00 */
/* GPIO0A_DS_3 */
#define GPIO0_A_IOC_GPIO0A_DS_3_OFFSET                     (0x10CU)
#define GPIO0_A_IOC_GPIO0A_DS_3_GPIO0A6_DS_SHIFT           (0U)
#define GPIO0_A_IOC_GPIO0A_DS_3_GPIO0A6_DS_MASK            (0x3FU << GPIO0_A_IOC_GPIO0A_DS_3_GPIO0A6_DS_SHIFT)          /* 0x0000003F */
/* GPIO0A_PULL */
#define GPIO0_A_IOC_GPIO0A_PULL_OFFSET                     (0x200U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A0_PULL_SHIFT         (0U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A0_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A0_PULL_SHIFT)         /* 0x00000003 */
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A1_PULL_SHIFT         (2U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A1_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A1_PULL_SHIFT)         /* 0x0000000C */
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A2_PULL_SHIFT         (4U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A2_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A2_PULL_SHIFT)         /* 0x00000030 */
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A3_PULL_SHIFT         (6U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A3_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A3_PULL_SHIFT)         /* 0x000000C0 */
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A4_PULL_SHIFT         (8U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A4_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A4_PULL_SHIFT)         /* 0x00000300 */
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A5_PULL_SHIFT         (10U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A5_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A5_PULL_SHIFT)         /* 0x00000C00 */
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A6_PULL_SHIFT         (12U)
#define GPIO0_A_IOC_GPIO0A_PULL_GPIO0A6_PULL_MASK          (0x3U << GPIO0_A_IOC_GPIO0A_PULL_GPIO0A6_PULL_SHIFT)         /* 0x00003000 */
/* GPIO0A_IE */
#define GPIO0_A_IOC_GPIO0A_IE_OFFSET                       (0x300U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A0_IE_SHIFT             (0U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A0_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A0_IE_SHIFT)             /* 0x00000001 */
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A1_IE_SHIFT             (1U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A1_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A1_IE_SHIFT)             /* 0x00000002 */
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A2_IE_SHIFT             (2U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A2_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A2_IE_SHIFT)             /* 0x00000004 */
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A3_IE_SHIFT             (3U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A3_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A3_IE_SHIFT)             /* 0x00000008 */
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A4_IE_SHIFT             (4U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A4_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A4_IE_SHIFT)             /* 0x00000010 */
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A5_IE_SHIFT             (5U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A5_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A5_IE_SHIFT)             /* 0x00000020 */
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A6_IE_SHIFT             (6U)
#define GPIO0_A_IOC_GPIO0A_IE_GPIO0A6_IE_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_IE_GPIO0A6_IE_SHIFT)             /* 0x00000040 */
/* GPIO0A_SMT */
#define GPIO0_A_IOC_GPIO0A_SMT_OFFSET                      (0x400U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A0_SMT_SHIFT           (0U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A0_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A0_SMT_SHIFT)           /* 0x00000001 */
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A1_SMT_SHIFT           (1U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A1_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A1_SMT_SHIFT)           /* 0x00000002 */
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A2_SMT_SHIFT           (2U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A2_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A2_SMT_SHIFT)           /* 0x00000004 */
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A3_SMT_SHIFT           (3U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A3_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A3_SMT_SHIFT)           /* 0x00000008 */
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A4_SMT_SHIFT           (4U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A4_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A4_SMT_SHIFT)           /* 0x00000010 */
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A5_SMT_SHIFT           (5U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A5_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A5_SMT_SHIFT)           /* 0x00000020 */
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A6_SMT_SHIFT           (6U)
#define GPIO0_A_IOC_GPIO0A_SMT_GPIO0A6_SMT_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SMT_GPIO0A6_SMT_SHIFT)           /* 0x00000040 */
/* GPIO0A_SUS */
#define GPIO0_A_IOC_GPIO0A_SUS_OFFSET                      (0x500U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A0_SUS_SHIFT           (0U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A0_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A0_SUS_SHIFT)           /* 0x00000001 */
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A1_SUS_SHIFT           (1U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A1_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A1_SUS_SHIFT)           /* 0x00000002 */
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A2_SUS_SHIFT           (2U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A2_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A2_SUS_SHIFT)           /* 0x00000004 */
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A3_SUS_SHIFT           (3U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A3_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A3_SUS_SHIFT)           /* 0x00000008 */
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A4_SUS_SHIFT           (4U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A4_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A4_SUS_SHIFT)           /* 0x00000010 */
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A5_SUS_SHIFT           (5U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A5_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A5_SUS_SHIFT)           /* 0x00000020 */
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A6_SUS_SHIFT           (6U)
#define GPIO0_A_IOC_GPIO0A_SUS_GPIO0A6_SUS_MASK            (0x1U << GPIO0_A_IOC_GPIO0A_SUS_GPIO0A6_SUS_SHIFT)           /* 0x00000040 */
/* GPIO0A_SL */
#define GPIO0_A_IOC_GPIO0A_SL_OFFSET                       (0x600U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A0_SL_SHIFT             (0U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A0_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A0_SL_SHIFT)             /* 0x00000003 */
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A1_SL_SHIFT             (2U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A1_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A1_SL_SHIFT)             /* 0x0000000C */
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A2_SL_SHIFT             (4U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A2_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A2_SL_SHIFT)             /* 0x00000030 */
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A3_SL_SHIFT             (6U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A3_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A3_SL_SHIFT)             /* 0x000000C0 */
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A4_SL_SHIFT             (8U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A4_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A4_SL_SHIFT)             /* 0x00000300 */
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A5_SL_SHIFT             (10U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A5_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A5_SL_SHIFT)             /* 0x00000C00 */
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A6_SL_SHIFT             (12U)
#define GPIO0_A_IOC_GPIO0A_SL_GPIO0A6_SL_MASK              (0x3U << GPIO0_A_IOC_GPIO0A_SL_GPIO0A6_SL_SHIFT)             /* 0x00003000 */
/* GPIO0A_OD */
#define GPIO0_A_IOC_GPIO0A_OD_OFFSET                       (0x700U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A0_OD_SHIFT             (0U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A0_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A0_OD_SHIFT)             /* 0x00000001 */
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A1_OD_SHIFT             (1U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A1_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A1_OD_SHIFT)             /* 0x00000002 */
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A2_OD_SHIFT             (2U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A2_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A2_OD_SHIFT)             /* 0x00000004 */
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A3_OD_SHIFT             (3U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A3_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A3_OD_SHIFT)             /* 0x00000008 */
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A4_OD_SHIFT             (4U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A4_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A4_OD_SHIFT)             /* 0x00000010 */
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A5_OD_SHIFT             (5U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A5_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A5_OD_SHIFT)             /* 0x00000020 */
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A6_OD_SHIFT             (6U)
#define GPIO0_A_IOC_GPIO0A_OD_GPIO0A6_OD_MASK              (0x1U << GPIO0_A_IOC_GPIO0A_OD_GPIO0A6_OD_SHIFT)             /* 0x00000040 */
/* IO_VSEL */
#define GPIO0_A_IOC_IO_VSEL_OFFSET                         (0x800U)
#define GPIO0_A_IOC_IO_VSEL_POC_PMUIO0_IDDQ_SHIFT          (0U)
#define GPIO0_A_IOC_IO_VSEL_POC_PMUIO0_IDDQ_MASK           (0x1U << GPIO0_A_IOC_IO_VSEL_POC_PMUIO0_IDDQ_SHIFT)          /* 0x00000001 */
/* GRF_JTAG_CON0 */
#define GPIO0_A_IOC_GRF_JTAG_CON0_OFFSET                   (0x804U)
#define GPIO0_A_IOC_GRF_JTAG_CON0_GRF_CON_FORCE_JTAG_UART_M0_SHIFT (3U)
#define GPIO0_A_IOC_GRF_JTAG_CON0_GRF_CON_FORCE_JTAG_UART_M0_MASK (0x1U << GPIO0_A_IOC_GRF_JTAG_CON0_GRF_CON_FORCE_JTAG_UART_M0_SHIFT) /* 0x00000008 */
/* GRF_JTAG_CON1 */
#define GPIO0_A_IOC_GRF_JTAG_CON1_OFFSET                   (0x808U)
#define GPIO0_A_IOC_GRF_JTAG_CON1_UART0_M0_RX_LOW_DLY_SHIFT (0U)
#define GPIO0_A_IOC_GRF_JTAG_CON1_UART0_M0_RX_LOW_DLY_MASK (0xFFFFFFFFU << GPIO0_A_IOC_GRF_JTAG_CON1_UART0_M0_RX_LOW_DLY_SHIFT) /* 0xFFFFFFFF */
/* XIN_CON */
#define GPIO0_A_IOC_XIN_CON_OFFSET                         (0x900U)
#define GPIO0_A_IOC_XIN_CON_XIN_CON_DS_SHIFT               (0U)
#define GPIO0_A_IOC_XIN_CON_XIN_CON_DS_MASK                (0xFU << GPIO0_A_IOC_XIN_CON_XIN_CON_DS_SHIFT)               /* 0x0000000F */
/**************************************GPIO0_B_IOC***************************************/
/* GPIO0B_IOMUX_SEL_0 */
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_OFFSET              (0x8U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B0_SEL_SHIFT   (0U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B0_SEL_MASK    (0xFU << GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B0_SEL_SHIFT)   /* 0x0000000F */
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B1_SEL_SHIFT   (4U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B1_SEL_MASK    (0xFU << GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B1_SEL_SHIFT)   /* 0x000000F0 */
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B2_SEL_SHIFT   (8U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B2_SEL_MASK    (0xFU << GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B2_SEL_SHIFT)   /* 0x00000F00 */
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B3_SEL_SHIFT   (12U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B3_SEL_MASK    (0xFU << GPIO0_B_IOC_GPIO0B_IOMUX_SEL_0_GPIO0B3_SEL_SHIFT)   /* 0x0000F000 */
/* GPIO0B_IOMUX_SEL_1 */
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_OFFSET              (0xCU)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_GPIO0B4_SEL_SHIFT   (0U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_GPIO0B4_SEL_MASK    (0xFU << GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_GPIO0B4_SEL_SHIFT)   /* 0x0000000F */
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_GPIO0B5_SEL_SHIFT   (4U)
#define GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_GPIO0B5_SEL_MASK    (0xFU << GPIO0_B_IOC_GPIO0B_IOMUX_SEL_1_GPIO0B5_SEL_SHIFT)   /* 0x000000F0 */
/* GPIO0B_DS_0 */
#define GPIO0_B_IOC_GPIO0B_DS_0_OFFSET                     (0x110U)
#define GPIO0_B_IOC_GPIO0B_DS_0_GPIO0B0_DS_SHIFT           (0U)
#define GPIO0_B_IOC_GPIO0B_DS_0_GPIO0B0_DS_MASK            (0x3FU << GPIO0_B_IOC_GPIO0B_DS_0_GPIO0B0_DS_SHIFT)          /* 0x0000003F */
#define GPIO0_B_IOC_GPIO0B_DS_0_GPIO0B1_DS_SHIFT           (8U)
#define GPIO0_B_IOC_GPIO0B_DS_0_GPIO0B1_DS_MASK            (0x3FU << GPIO0_B_IOC_GPIO0B_DS_0_GPIO0B1_DS_SHIFT)          /* 0x00003F00 */
/* GPIO0B_DS_1 */
#define GPIO0_B_IOC_GPIO0B_DS_1_OFFSET                     (0x114U)
#define GPIO0_B_IOC_GPIO0B_DS_1_GPIO0B2_DS_SHIFT           (0U)
#define GPIO0_B_IOC_GPIO0B_DS_1_GPIO0B2_DS_MASK            (0x3FU << GPIO0_B_IOC_GPIO0B_DS_1_GPIO0B2_DS_SHIFT)          /* 0x0000003F */
#define GPIO0_B_IOC_GPIO0B_DS_1_GPIO0B3_DS_SHIFT           (8U)
#define GPIO0_B_IOC_GPIO0B_DS_1_GPIO0B3_DS_MASK            (0x3FU << GPIO0_B_IOC_GPIO0B_DS_1_GPIO0B3_DS_SHIFT)          /* 0x00003F00 */
/* GPIO0B_DS_2 */
#define GPIO0_B_IOC_GPIO0B_DS_2_OFFSET                     (0x118U)
#define GPIO0_B_IOC_GPIO0B_DS_2_GPIO0B4_DS_SHIFT           (0U)
#define GPIO0_B_IOC_GPIO0B_DS_2_GPIO0B4_DS_MASK            (0x3FU << GPIO0_B_IOC_GPIO0B_DS_2_GPIO0B4_DS_SHIFT)          /* 0x0000003F */
#define GPIO0_B_IOC_GPIO0B_DS_2_GPIO0B5_DS_SHIFT           (8U)
#define GPIO0_B_IOC_GPIO0B_DS_2_GPIO0B5_DS_MASK            (0x3FU << GPIO0_B_IOC_GPIO0B_DS_2_GPIO0B5_DS_SHIFT)          /* 0x00003F00 */
/* GPIO0B_PULL */
#define GPIO0_B_IOC_GPIO0B_PULL_OFFSET                     (0x204U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B0_PULL_SHIFT         (0U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B0_PULL_MASK          (0x3U << GPIO0_B_IOC_GPIO0B_PULL_GPIO0B0_PULL_SHIFT)         /* 0x00000003 */
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B1_PULL_SHIFT         (2U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B1_PULL_MASK          (0x3U << GPIO0_B_IOC_GPIO0B_PULL_GPIO0B1_PULL_SHIFT)         /* 0x0000000C */
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B3_PULL_SHIFT         (6U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B3_PULL_MASK          (0x3U << GPIO0_B_IOC_GPIO0B_PULL_GPIO0B3_PULL_SHIFT)         /* 0x000000C0 */
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B2_PULL_SHIFT         (4U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B2_PULL_MASK          (0x3U << GPIO0_B_IOC_GPIO0B_PULL_GPIO0B2_PULL_SHIFT)         /* 0x00000030 */
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B4_PULL_SHIFT         (8U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B4_PULL_MASK          (0x3U << GPIO0_B_IOC_GPIO0B_PULL_GPIO0B4_PULL_SHIFT)         /* 0x00000300 */
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B5_PULL_SHIFT         (10U)
#define GPIO0_B_IOC_GPIO0B_PULL_GPIO0B5_PULL_MASK          (0x3U << GPIO0_B_IOC_GPIO0B_PULL_GPIO0B5_PULL_SHIFT)         /* 0x00000C00 */
/* GPIO0B_IE */
#define GPIO0_B_IOC_GPIO0B_IE_OFFSET                       (0x304U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B0_IE_SHIFT             (0U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B0_IE_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_IE_GPIO0B0_IE_SHIFT)             /* 0x00000001 */
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B1_IE_SHIFT             (1U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B1_IE_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_IE_GPIO0B1_IE_SHIFT)             /* 0x00000002 */
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B2_IE_SHIFT             (2U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B2_IE_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_IE_GPIO0B2_IE_SHIFT)             /* 0x00000004 */
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B3_IE_SHIFT             (3U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B3_IE_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_IE_GPIO0B3_IE_SHIFT)             /* 0x00000008 */
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B4_IE_SHIFT             (4U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B4_IE_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_IE_GPIO0B4_IE_SHIFT)             /* 0x00000010 */
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B5_IE_SHIFT             (5U)
#define GPIO0_B_IOC_GPIO0B_IE_GPIO0B5_IE_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_IE_GPIO0B5_IE_SHIFT)             /* 0x00000020 */
/* GPIO0B_SMT */
#define GPIO0_B_IOC_GPIO0B_SMT_OFFSET                      (0x404U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B0_SMT_SHIFT           (0U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B0_SMT_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SMT_GPIO0B0_SMT_SHIFT)           /* 0x00000001 */
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B1_SMT_SHIFT           (1U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B1_SMT_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SMT_GPIO0B1_SMT_SHIFT)           /* 0x00000002 */
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B2_SMT_SHIFT           (2U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B2_SMT_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SMT_GPIO0B2_SMT_SHIFT)           /* 0x00000004 */
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B3_SMT_SHIFT           (3U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B3_SMT_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SMT_GPIO0B3_SMT_SHIFT)           /* 0x00000008 */
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B4_SMT_SHIFT           (4U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B4_SMT_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SMT_GPIO0B4_SMT_SHIFT)           /* 0x00000010 */
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B5_SMT_SHIFT           (5U)
#define GPIO0_B_IOC_GPIO0B_SMT_GPIO0B5_SMT_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SMT_GPIO0B5_SMT_SHIFT)           /* 0x00000020 */
/* GPIO0B_SUS */
#define GPIO0_B_IOC_GPIO0B_SUS_OFFSET                      (0x504U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B0_SUS_SHIFT           (0U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B0_SUS_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SUS_GPIO0B0_SUS_SHIFT)           /* 0x00000001 */
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B1_SUS_SHIFT           (1U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B1_SUS_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SUS_GPIO0B1_SUS_SHIFT)           /* 0x00000002 */
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B2_SUS_SHIFT           (2U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B2_SUS_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SUS_GPIO0B2_SUS_SHIFT)           /* 0x00000004 */
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B3_SUS_SHIFT           (3U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B3_SUS_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SUS_GPIO0B3_SUS_SHIFT)           /* 0x00000008 */
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B4_SUS_SHIFT           (4U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B4_SUS_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SUS_GPIO0B4_SUS_SHIFT)           /* 0x00000010 */
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B5_SUS_SHIFT           (5U)
#define GPIO0_B_IOC_GPIO0B_SUS_GPIO0B5_SUS_MASK            (0x1U << GPIO0_B_IOC_GPIO0B_SUS_GPIO0B5_SUS_SHIFT)           /* 0x00000020 */
/* GPIO0B_SL */
#define GPIO0_B_IOC_GPIO0B_SL_OFFSET                       (0x604U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B0_SL_SHIFT             (0U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B0_SL_MASK              (0x3U << GPIO0_B_IOC_GPIO0B_SL_GPIO0B0_SL_SHIFT)             /* 0x00000003 */
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B1_SL_SHIFT             (2U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B1_SL_MASK              (0x3U << GPIO0_B_IOC_GPIO0B_SL_GPIO0B1_SL_SHIFT)             /* 0x0000000C */
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B2_SL_SHIFT             (4U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B2_SL_MASK              (0x3U << GPIO0_B_IOC_GPIO0B_SL_GPIO0B2_SL_SHIFT)             /* 0x00000030 */
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B3_SL_SHIFT             (6U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B3_SL_MASK              (0x3U << GPIO0_B_IOC_GPIO0B_SL_GPIO0B3_SL_SHIFT)             /* 0x000000C0 */
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B4_SL_SHIFT             (8U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B4_SL_MASK              (0x3U << GPIO0_B_IOC_GPIO0B_SL_GPIO0B4_SL_SHIFT)             /* 0x00000300 */
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B5_SL_SHIFT             (10U)
#define GPIO0_B_IOC_GPIO0B_SL_GPIO0B5_SL_MASK              (0x3U << GPIO0_B_IOC_GPIO0B_SL_GPIO0B5_SL_SHIFT)             /* 0x00000C00 */
/* GPIO0B_OD */
#define GPIO0_B_IOC_GPIO0B_OD_OFFSET                       (0x704U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B0_OD_SHIFT             (0U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B0_OD_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_OD_GPIO0B0_OD_SHIFT)             /* 0x00000001 */
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B1_OD_SHIFT             (1U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B1_OD_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_OD_GPIO0B1_OD_SHIFT)             /* 0x00000002 */
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B2_OD_SHIFT             (2U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B2_OD_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_OD_GPIO0B2_OD_SHIFT)             /* 0x00000004 */
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B3_OD_SHIFT             (3U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B3_OD_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_OD_GPIO0B3_OD_SHIFT)             /* 0x00000008 */
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B4_OD_SHIFT             (4U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B4_OD_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_OD_GPIO0B4_OD_SHIFT)             /* 0x00000010 */
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B5_OD_SHIFT             (5U)
#define GPIO0_B_IOC_GPIO0B_OD_GPIO0B5_OD_MASK              (0x1U << GPIO0_B_IOC_GPIO0B_OD_GPIO0B5_OD_SHIFT)             /* 0x00000020 */
/* IO_VSEL */
#define GPIO0_B_IOC_IO_VSEL_OFFSET                         (0x800U)
#define GPIO0_B_IOC_IO_VSEL_POC_PMUIO1_IDDQ_SHIFT          (0U)
#define GPIO0_B_IOC_IO_VSEL_POC_PMUIO1_IDDQ_MASK           (0x1U << GPIO0_B_IOC_IO_VSEL_POC_PMUIO1_IDDQ_SHIFT)          /* 0x00000001 */
#define GPIO0_B_IOC_IO_VSEL_POC_PMUIO1_VD_SHIFT            (15U)
#define GPIO0_B_IOC_IO_VSEL_POC_PMUIO1_VD_MASK             (0x1U << GPIO0_B_IOC_IO_VSEL_POC_PMUIO1_VD_SHIFT)            /* 0x00008000 */
/* GRF_JTAG_CON0 */
#define GPIO0_B_IOC_GRF_JTAG_CON0_OFFSET                   (0x804U)
#define GPIO0_B_IOC_GRF_JTAG_CON0_GRF_CON_JTAG_SEL_SHIFT   (0U)
#define GPIO0_B_IOC_GRF_JTAG_CON0_GRF_CON_JTAG_SEL_MASK    (0x7U << GPIO0_B_IOC_GRF_JTAG_CON0_GRF_CON_JTAG_SEL_SHIFT)   /* 0x00000007 */
#define GPIO0_B_IOC_GRF_JTAG_CON0_GRF_CON_FORCE_JTAG_UART_M1_SHIFT (3U)
#define GPIO0_B_IOC_GRF_JTAG_CON0_GRF_CON_FORCE_JTAG_UART_M1_MASK (0x1U << GPIO0_B_IOC_GRF_JTAG_CON0_GRF_CON_FORCE_JTAG_UART_M1_SHIFT) /* 0x00000008 */
/* GRF_JTAG_CON1 */
#define GPIO0_B_IOC_GRF_JTAG_CON1_OFFSET                   (0x808U)
#define GPIO0_B_IOC_GRF_JTAG_CON1_UART0_M1_RX_LOW_DLY_SHIFT (0U)
#define GPIO0_B_IOC_GRF_JTAG_CON1_UART0_M1_RX_LOW_DLY_MASK (0xFFFFFFFFU << GPIO0_B_IOC_GRF_JTAG_CON1_UART0_M1_RX_LOW_DLY_SHIFT) /* 0xFFFFFFFF */
/******************************************GPIO******************************************/
/* SWPORT_DR_L */
#define GPIO_SWPORT_DR_L_OFFSET                            (0x0U)
#define GPIO_SWPORT_DR_L_SWPORT_DR_LOW_SHIFT               (0U)
#define GPIO_SWPORT_DR_L_SWPORT_DR_LOW_MASK                (0xFFFFU << GPIO_SWPORT_DR_L_SWPORT_DR_LOW_SHIFT)            /* 0x0000FFFF */
/* SWPORT_DR_H */
#define GPIO_SWPORT_DR_H_OFFSET                            (0x4U)
#define GPIO_SWPORT_DR_H_SWPORT_DR_HIGH_SHIFT              (0U)
#define GPIO_SWPORT_DR_H_SWPORT_DR_HIGH_MASK               (0xFFFFU << GPIO_SWPORT_DR_H_SWPORT_DR_HIGH_SHIFT)           /* 0x0000FFFF */
/* SWPORT_DDR_L */
#define GPIO_SWPORT_DDR_L_OFFSET                           (0x8U)
#define GPIO_SWPORT_DDR_L_SWPORT_DDR_LOW_SHIFT             (0U)
#define GPIO_SWPORT_DDR_L_SWPORT_DDR_LOW_MASK              (0xFFFFU << GPIO_SWPORT_DDR_L_SWPORT_DDR_LOW_SHIFT)          /* 0x0000FFFF */
/* SWPORT_DDR_H */
#define GPIO_SWPORT_DDR_H_OFFSET                           (0xCU)
#define GPIO_SWPORT_DDR_H_SWPORT_DDR_HIGH_SHIFT            (0U)
#define GPIO_SWPORT_DDR_H_SWPORT_DDR_HIGH_MASK             (0xFFFFU << GPIO_SWPORT_DDR_H_SWPORT_DDR_HIGH_SHIFT)         /* 0x0000FFFF */
/* INT_EN_L */
#define GPIO_INT_EN_L_OFFSET                               (0x10U)
#define GPIO_INT_EN_L_INT_EN_LOW_SHIFT                     (0U)
#define GPIO_INT_EN_L_INT_EN_LOW_MASK                      (0xFFFFU << GPIO_INT_EN_L_INT_EN_LOW_SHIFT)                  /* 0x0000FFFF */
/* INT_EN_H */
#define GPIO_INT_EN_H_OFFSET                               (0x14U)
#define GPIO_INT_EN_H_INT_EN_HIGH_SHIFT                    (0U)
#define GPIO_INT_EN_H_INT_EN_HIGH_MASK                     (0xFFFFU << GPIO_INT_EN_H_INT_EN_HIGH_SHIFT)                 /* 0x0000FFFF */
/* INT_MASK_L */
#define GPIO_INT_MASK_L_OFFSET                             (0x18U)
#define GPIO_INT_MASK_L_INT_MASK_LOW_SHIFT                 (0U)
#define GPIO_INT_MASK_L_INT_MASK_LOW_MASK                  (0xFFFFU << GPIO_INT_MASK_L_INT_MASK_LOW_SHIFT)              /* 0x0000FFFF */
/* INT_MASK_H */
#define GPIO_INT_MASK_H_OFFSET                             (0x1CU)
#define GPIO_INT_MASK_H_INT_MASK_HIGH_SHIFT                (0U)
#define GPIO_INT_MASK_H_INT_MASK_HIGH_MASK                 (0xFFFFU << GPIO_INT_MASK_H_INT_MASK_HIGH_SHIFT)             /* 0x0000FFFF */
/* INT_TYPE_L */
#define GPIO_INT_TYPE_L_OFFSET                             (0x20U)
#define GPIO_INT_TYPE_L_INT_TYPE_LOW_SHIFT                 (0U)
#define GPIO_INT_TYPE_L_INT_TYPE_LOW_MASK                  (0xFFFFU << GPIO_INT_TYPE_L_INT_TYPE_LOW_SHIFT)              /* 0x0000FFFF */
/* INT_TYPE_H */
#define GPIO_INT_TYPE_H_OFFSET                             (0x24U)
#define GPIO_INT_TYPE_H_INT_TYPE_HIGH_SHIFT                (0U)
#define GPIO_INT_TYPE_H_INT_TYPE_HIGH_MASK                 (0xFFFFU << GPIO_INT_TYPE_H_INT_TYPE_HIGH_SHIFT)             /* 0x0000FFFF */
/* INT_POLARITY_L */
#define GPIO_INT_POLARITY_L_OFFSET                         (0x28U)
#define GPIO_INT_POLARITY_L_INT_POLARITY_LOW_SHIFT         (0U)
#define GPIO_INT_POLARITY_L_INT_POLARITY_LOW_MASK          (0xFFFFU << GPIO_INT_POLARITY_L_INT_POLARITY_LOW_SHIFT)      /* 0x0000FFFF */
/* INT_POLARITY_H */
#define GPIO_INT_POLARITY_H_OFFSET                         (0x2CU)
#define GPIO_INT_POLARITY_H_INT_POLARITY_HIGH_SHIFT        (0U)
#define GPIO_INT_POLARITY_H_INT_POLARITY_HIGH_MASK         (0xFFFFU << GPIO_INT_POLARITY_H_INT_POLARITY_HIGH_SHIFT)     /* 0x0000FFFF */
/* INT_BOTHEDGE_L */
#define GPIO_INT_BOTHEDGE_L_OFFSET                         (0x30U)
#define GPIO_INT_BOTHEDGE_L_INT_BOTHEDGE_LOW_SHIFT         (0U)
#define GPIO_INT_BOTHEDGE_L_INT_BOTHEDGE_LOW_MASK          (0xFFFFU << GPIO_INT_BOTHEDGE_L_INT_BOTHEDGE_LOW_SHIFT)      /* 0x0000FFFF */
/* INT_BOTHEDGE_H */
#define GPIO_INT_BOTHEDGE_H_OFFSET                         (0x34U)
#define GPIO_INT_BOTHEDGE_H_INT_BOTHEDGE_HIGH_SHIFT        (0U)
#define GPIO_INT_BOTHEDGE_H_INT_BOTHEDGE_HIGH_MASK         (0xFFFFU << GPIO_INT_BOTHEDGE_H_INT_BOTHEDGE_HIGH_SHIFT)     /* 0x0000FFFF */
/* DEBOUNCE_L */
#define GPIO_DEBOUNCE_L_OFFSET                             (0x38U)
#define GPIO_DEBOUNCE_L_DEBOUNCE_LOW_SHIFT                 (0U)
#define GPIO_DEBOUNCE_L_DEBOUNCE_LOW_MASK                  (0xFFFFU << GPIO_DEBOUNCE_L_DEBOUNCE_LOW_SHIFT)              /* 0x0000FFFF */
/* DEBOUNCE_H */
#define GPIO_DEBOUNCE_H_OFFSET                             (0x3CU)
#define GPIO_DEBOUNCE_H_DEBOUNCE_HIGH_SHIFT                (0U)
#define GPIO_DEBOUNCE_H_DEBOUNCE_HIGH_MASK                 (0xFFFFU << GPIO_DEBOUNCE_H_DEBOUNCE_HIGH_SHIFT)             /* 0x0000FFFF */
/* DBCLK_DIV_EN_L */
#define GPIO_DBCLK_DIV_EN_L_OFFSET                         (0x40U)
#define GPIO_DBCLK_DIV_EN_L_DBCLK_DIV_EN_LOW_SHIFT         (0U)
#define GPIO_DBCLK_DIV_EN_L_DBCLK_DIV_EN_LOW_MASK          (0xFFFFU << GPIO_DBCLK_DIV_EN_L_DBCLK_DIV_EN_LOW_SHIFT)      /* 0x0000FFFF */
/* DBCLK_DIV_EN_H */
#define GPIO_DBCLK_DIV_EN_H_OFFSET                         (0x44U)
#define GPIO_DBCLK_DIV_EN_H_DBCLK_DIV_EN_HIGH_SHIFT        (0U)
#define GPIO_DBCLK_DIV_EN_H_DBCLK_DIV_EN_HIGH_MASK         (0xFFFFU << GPIO_DBCLK_DIV_EN_H_DBCLK_DIV_EN_HIGH_SHIFT)     /* 0x0000FFFF */
/* DBCLK_DIV_CON */
#define GPIO_DBCLK_DIV_CON_OFFSET                          (0x48U)
#define GPIO_DBCLK_DIV_CON_DBCLK_DIV_CON_SHIFT             (0U)
#define GPIO_DBCLK_DIV_CON_DBCLK_DIV_CON_MASK              (0xFFFFFFU << GPIO_DBCLK_DIV_CON_DBCLK_DIV_CON_SHIFT)        /* 0x00FFFFFF */
/* INT_STATUS */
#define GPIO_INT_STATUS_OFFSET                             (0x50U)
#define GPIO_INT_STATUS                                    (0x0U)
#define GPIO_INT_STATUS_INT_STATUS_SHIFT                   (0U)
#define GPIO_INT_STATUS_INT_STATUS_MASK                    (0xFFFFFFFFU << GPIO_INT_STATUS_INT_STATUS_SHIFT)            /* 0xFFFFFFFF */
/* INT_RAWSTATUS */
#define GPIO_INT_RAWSTATUS_OFFSET                          (0x58U)
#define GPIO_INT_RAWSTATUS                                 (0x0U)
#define GPIO_INT_RAWSTATUS_INT_RAWSTATUS_SHIFT             (0U)
#define GPIO_INT_RAWSTATUS_INT_RAWSTATUS_MASK              (0xFFFFFFFFU << GPIO_INT_RAWSTATUS_INT_RAWSTATUS_SHIFT)      /* 0xFFFFFFFF */
/* PORT_EOI_L */
#define GPIO_PORT_EOI_L_OFFSET                             (0x60U)
#define GPIO_PORT_EOI_L_PORT_EOI_LOW_SHIFT                 (0U)
#define GPIO_PORT_EOI_L_PORT_EOI_LOW_MASK                  (0xFFFFU << GPIO_PORT_EOI_L_PORT_EOI_LOW_SHIFT)              /* 0x0000FFFF */
/* PORT_EOI_H */
#define GPIO_PORT_EOI_H_OFFSET                             (0x64U)
#define GPIO_PORT_EOI_H_PORT_EOI_HIGH_SHIFT                (0U)
#define GPIO_PORT_EOI_H_PORT_EOI_HIGH_MASK                 (0xFFFFU << GPIO_PORT_EOI_H_PORT_EOI_HIGH_SHIFT)             /* 0x0000FFFF */
/* EXT_PORT */
#define GPIO_EXT_PORT_OFFSET                               (0x70U)
#define GPIO_EXT_PORT                                      (0x0U)
#define GPIO_EXT_PORT_EXT_PORT_SHIFT                       (0U)
#define GPIO_EXT_PORT_EXT_PORT_MASK                        (0xFFFFFFFFU << GPIO_EXT_PORT_EXT_PORT_SHIFT)                /* 0xFFFFFFFF */
/* VER_ID */
#define GPIO_VER_ID_OFFSET                                 (0x78U)
#define GPIO_VER_ID                                        (0x10219C8U)
#define GPIO_VER_ID_VER_ID_SHIFT                           (0U)
#define GPIO_VER_ID_VER_ID_MASK                            (0xFFFFFFFFU << GPIO_VER_ID_VER_ID_SHIFT)                    /* 0xFFFFFFFF */
/* STORE_ST_L */
#define GPIO_STORE_ST_L_OFFSET                             (0x80U)
#define GPIO_STORE_ST_L_STORE_ST_L_SHIFT                   (0U)
#define GPIO_STORE_ST_L_STORE_ST_L_MASK                    (0xFFFFU << GPIO_STORE_ST_L_STORE_ST_L_SHIFT)                /* 0x0000FFFF */
/* STORE_ST_H */
#define GPIO_STORE_ST_H_OFFSET                             (0x84U)
#define GPIO_STORE_ST_H_STORE_ST_H_SHIFT                   (0U)
#define GPIO_STORE_ST_H_STORE_ST_H_MASK                    (0xFFFFU << GPIO_STORE_ST_H_STORE_ST_H_SHIFT)                /* 0x0000FFFF */
/* GPIO_REG_GROUP_L */
#define GPIO_GPIO_REG_GROUP_L_OFFSET                       (0x100U)
#define GPIO_GPIO_REG_GROUP_L_GPIO_REG_GROUP_LOW_SHIFT     (0U)
#define GPIO_GPIO_REG_GROUP_L_GPIO_REG_GROUP_LOW_MASK      (0xFFFFU << GPIO_GPIO_REG_GROUP_L_GPIO_REG_GROUP_LOW_SHIFT)  /* 0x0000FFFF */
/* GPIO_REG_GROUP_H */
#define GPIO_GPIO_REG_GROUP_H_OFFSET                       (0x104U)
#define GPIO_GPIO_REG_GROUP_H_GPIO_REG_GROUP_HIGH_SHIFT    (0U)
#define GPIO_GPIO_REG_GROUP_H_GPIO_REG_GROUP_HIGH_MASK     (0xFFFFU << GPIO_GPIO_REG_GROUP_H_GPIO_REG_GROUP_HIGH_SHIFT) /* 0x0000FFFF */
/* GPIO_VIRTUAL_EN */
#define GPIO_GPIO_VIRTUAL_EN_OFFSET                        (0x108U)
#define GPIO_GPIO_VIRTUAL_EN_GPIO_VIRTUAL_EN_SHIFT         (0U)
#define GPIO_GPIO_VIRTUAL_EN_GPIO_VIRTUAL_EN_MASK          (0x1U << GPIO_GPIO_VIRTUAL_EN_GPIO_VIRTUAL_EN_SHIFT)         /* 0x00000001 */
/* GPIO_REG_GROUP1_L */
#define GPIO_GPIO_REG_GROUP1_L_OFFSET                      (0x110U)
#define GPIO_GPIO_REG_GROUP1_L_GPIO_REG_GROUP1_LOW_SHIFT   (0U)
#define GPIO_GPIO_REG_GROUP1_L_GPIO_REG_GROUP1_LOW_MASK    (0xFFFFU << GPIO_GPIO_REG_GROUP1_L_GPIO_REG_GROUP1_LOW_SHIFT) /* 0x0000FFFF */
/* GPIO_REG_GROUP1_H */
#define GPIO_GPIO_REG_GROUP1_H_OFFSET                      (0x114U)
#define GPIO_GPIO_REG_GROUP1_H_GPIO_REG_GROUP1_HIGH_SHIFT  (0U)
#define GPIO_GPIO_REG_GROUP1_H_GPIO_REG_GROUP1_HIGH_MASK   (0xFFFFU << GPIO_GPIO_REG_GROUP1_H_GPIO_REG_GROUP1_HIGH_SHIFT) /* 0x0000FFFF */
/* GPIO_REG_GROUP2_L */
#define GPIO_GPIO_REG_GROUP2_L_OFFSET                      (0x118U)
#define GPIO_GPIO_REG_GROUP2_L_GPIO_REG_GROUP2_LOW_SHIFT   (0U)
#define GPIO_GPIO_REG_GROUP2_L_GPIO_REG_GROUP2_LOW_MASK    (0xFFFFU << GPIO_GPIO_REG_GROUP2_L_GPIO_REG_GROUP2_LOW_SHIFT) /* 0x0000FFFF */
/* GPIO_REG_GROUP2_H */
#define GPIO_GPIO_REG_GROUP2_H_OFFSET                      (0x11CU)
#define GPIO_GPIO_REG_GROUP2_H_GPIO_REG_GROUP2_HIGH_SHIFT  (0U)
#define GPIO_GPIO_REG_GROUP2_H_GPIO_REG_GROUP2_HIGH_MASK   (0xFFFFU << GPIO_GPIO_REG_GROUP2_H_GPIO_REG_GROUP2_HIGH_SHIFT) /* 0x0000FFFF */
/* GPIO_REG_GROUP3_L */
#define GPIO_GPIO_REG_GROUP3_L_OFFSET                      (0x120U)
#define GPIO_GPIO_REG_GROUP3_L_GPIO_REG_GROUP3_LOW_SHIFT   (0U)
#define GPIO_GPIO_REG_GROUP3_L_GPIO_REG_GROUP3_LOW_MASK    (0xFFFFU << GPIO_GPIO_REG_GROUP3_L_GPIO_REG_GROUP3_LOW_SHIFT) /* 0x0000FFFF */
/* GPIO_REG_GROUP3_H */
#define GPIO_GPIO_REG_GROUP3_H_OFFSET                      (0x124U)
#define GPIO_GPIO_REG_GROUP3_H_GPIO_REG_GROUP3_HIGH_SHIFT  (0U)
#define GPIO_GPIO_REG_GROUP3_H_GPIO_REG_GROUP3_HIGH_MASK   (0xFFFFU << GPIO_GPIO_REG_GROUP3_H_GPIO_REG_GROUP3_HIGH_SHIFT) /* 0x0000FFFF */
/******************************************I2C*******************************************/
/* CON */
#define I2C_CON_OFFSET                                     (0x0U)
#define I2C_CON_I2C_EN_SHIFT                               (0U)
#define I2C_CON_I2C_EN_MASK                                (0x1U << I2C_CON_I2C_EN_SHIFT)                               /* 0x00000001 */
#define I2C_CON_I2C_MODE_SHIFT                             (1U)
#define I2C_CON_I2C_MODE_MASK                              (0x3U << I2C_CON_I2C_MODE_SHIFT)                             /* 0x00000006 */
#define I2C_CON_START_SHIFT                                (3U)
#define I2C_CON_START_MASK                                 (0x1U << I2C_CON_START_SHIFT)                                /* 0x00000008 */
#define I2C_CON_STOP_SHIFT                                 (4U)
#define I2C_CON_STOP_MASK                                  (0x1U << I2C_CON_STOP_SHIFT)                                 /* 0x00000010 */
#define I2C_CON_ACK_SHIFT                                  (5U)
#define I2C_CON_ACK_MASK                                   (0x1U << I2C_CON_ACK_SHIFT)                                  /* 0x00000020 */
#define I2C_CON_ACT2NAK_SHIFT                              (6U)
#define I2C_CON_ACT2NAK_MASK                               (0x1U << I2C_CON_ACT2NAK_SHIFT)                              /* 0x00000040 */
#define I2C_CON_DATA_UPD_ST_SHIFT                          (8U)
#define I2C_CON_DATA_UPD_ST_MASK                           (0xFU << I2C_CON_DATA_UPD_ST_SHIFT)                          /* 0x00000F00 */
#define I2C_CON_START_SETUP_SHIFT                          (12U)
#define I2C_CON_START_SETUP_MASK                           (0x3U << I2C_CON_START_SETUP_SHIFT)                          /* 0x00003000 */
#define I2C_CON_STOP_SETUP_SHIFT                           (14U)
#define I2C_CON_STOP_SETUP_MASK                            (0x3U << I2C_CON_STOP_SETUP_SHIFT)                           /* 0x0000C000 */
#define I2C_CON_VERSION_SHIFT                              (16U)
#define I2C_CON_VERSION_MASK                               (0xFFFFU << I2C_CON_VERSION_SHIFT)                           /* 0xFFFF0000 */
/* CLKDIV */
#define I2C_CLKDIV_OFFSET                                  (0x4U)
#define I2C_CLKDIV_CLKDIVL_SHIFT                           (0U)
#define I2C_CLKDIV_CLKDIVL_MASK                            (0xFFFFU << I2C_CLKDIV_CLKDIVL_SHIFT)                        /* 0x0000FFFF */
#define I2C_CLKDIV_CLKDIVH_SHIFT                           (16U)
#define I2C_CLKDIV_CLKDIVH_MASK                            (0xFFFFU << I2C_CLKDIV_CLKDIVH_SHIFT)                        /* 0xFFFF0000 */
/* MRXADDR */
#define I2C_MRXADDR_OFFSET                                 (0x8U)
#define I2C_MRXADDR_SADDR_SHIFT                            (0U)
#define I2C_MRXADDR_SADDR_MASK                             (0xFFFFFFU << I2C_MRXADDR_SADDR_SHIFT)                       /* 0x00FFFFFF */
#define I2C_MRXADDR_ADDLVLD_SHIFT                          (24U)
#define I2C_MRXADDR_ADDLVLD_MASK                           (0x1U << I2C_MRXADDR_ADDLVLD_SHIFT)                          /* 0x01000000 */
#define I2C_MRXADDR_ADDMVLD_SHIFT                          (25U)
#define I2C_MRXADDR_ADDMVLD_MASK                           (0x1U << I2C_MRXADDR_ADDMVLD_SHIFT)                          /* 0x02000000 */
#define I2C_MRXADDR_ADDHVLD_SHIFT                          (26U)
#define I2C_MRXADDR_ADDHVLD_MASK                           (0x1U << I2C_MRXADDR_ADDHVLD_SHIFT)                          /* 0x04000000 */
/* MRXRADDR */
#define I2C_MRXRADDR_OFFSET                                (0xCU)
#define I2C_MRXRADDR_SRADDR_SHIFT                          (0U)
#define I2C_MRXRADDR_SRADDR_MASK                           (0xFFFFFFU << I2C_MRXRADDR_SRADDR_SHIFT)                     /* 0x00FFFFFF */
#define I2C_MRXRADDR_SRADDLVLD_SHIFT                       (24U)
#define I2C_MRXRADDR_SRADDLVLD_MASK                        (0x1U << I2C_MRXRADDR_SRADDLVLD_SHIFT)                       /* 0x01000000 */
#define I2C_MRXRADDR_SRADDMVLD_SHIFT                       (25U)
#define I2C_MRXRADDR_SRADDMVLD_MASK                        (0x1U << I2C_MRXRADDR_SRADDMVLD_SHIFT)                       /* 0x02000000 */
#define I2C_MRXRADDR_SRADDHVLD_SHIFT                       (26U)
#define I2C_MRXRADDR_SRADDHVLD_MASK                        (0x1U << I2C_MRXRADDR_SRADDHVLD_SHIFT)                       /* 0x04000000 */
/* MTXCNT */
#define I2C_MTXCNT_OFFSET                                  (0x10U)
#define I2C_MTXCNT_MTXCNT_SHIFT                            (0U)
#define I2C_MTXCNT_MTXCNT_MASK                             (0x3FU << I2C_MTXCNT_MTXCNT_SHIFT)                           /* 0x0000003F */
/* MRXCNT */
#define I2C_MRXCNT_OFFSET                                  (0x14U)
#define I2C_MRXCNT_MRXCNT_SHIFT                            (0U)
#define I2C_MRXCNT_MRXCNT_MASK                             (0x3FU << I2C_MRXCNT_MRXCNT_SHIFT)                           /* 0x0000003F */
/* IEN */
#define I2C_IEN_OFFSET                                     (0x18U)
#define I2C_IEN_BTFIEN_SHIFT                               (0U)
#define I2C_IEN_BTFIEN_MASK                                (0x1U << I2C_IEN_BTFIEN_SHIFT)                               /* 0x00000001 */
#define I2C_IEN_BRFIEN_SHIFT                               (1U)
#define I2C_IEN_BRFIEN_MASK                                (0x1U << I2C_IEN_BRFIEN_SHIFT)                               /* 0x00000002 */
#define I2C_IEN_MBTFIEN_SHIFT                              (2U)
#define I2C_IEN_MBTFIEN_MASK                               (0x1U << I2C_IEN_MBTFIEN_SHIFT)                              /* 0x00000004 */
#define I2C_IEN_MBRFIEN_SHIFT                              (3U)
#define I2C_IEN_MBRFIEN_MASK                               (0x1U << I2C_IEN_MBRFIEN_SHIFT)                              /* 0x00000008 */
#define I2C_IEN_STARTIEN_SHIFT                             (4U)
#define I2C_IEN_STARTIEN_MASK                              (0x1U << I2C_IEN_STARTIEN_SHIFT)                             /* 0x00000010 */
#define I2C_IEN_STOPIEN_SHIFT                              (5U)
#define I2C_IEN_STOPIEN_MASK                               (0x1U << I2C_IEN_STOPIEN_SHIFT)                              /* 0x00000020 */
#define I2C_IEN_NAKRCVIEN_SHIFT                            (6U)
#define I2C_IEN_NAKRCVIEN_MASK                             (0x1U << I2C_IEN_NAKRCVIEN_SHIFT)                            /* 0x00000040 */
#define I2C_IEN_SLAVEHDSCLEN_SHIFT                         (7U)
#define I2C_IEN_SLAVEHDSCLEN_MASK                          (0x1U << I2C_IEN_SLAVEHDSCLEN_SHIFT)                         /* 0x00000080 */
#define I2C_IEN_SLAVEHDSDAEN_SHIFT                         (8U)
#define I2C_IEN_SLAVEHDSDAEN_MASK                          (0x1U << I2C_IEN_SLAVEHDSDAEN_SHIFT)                         /* 0x00000100 */
/* IPD */
#define I2C_IPD_OFFSET                                     (0x1CU)
#define I2C_IPD_BTFIPD_SHIFT                               (0U)
#define I2C_IPD_BTFIPD_MASK                                (0x1U << I2C_IPD_BTFIPD_SHIFT)                               /* 0x00000001 */
#define I2C_IPD_BRFIPD_SHIFT                               (1U)
#define I2C_IPD_BRFIPD_MASK                                (0x1U << I2C_IPD_BRFIPD_SHIFT)                               /* 0x00000002 */
#define I2C_IPD_MBTFIPD_SHIFT                              (2U)
#define I2C_IPD_MBTFIPD_MASK                               (0x1U << I2C_IPD_MBTFIPD_SHIFT)                              /* 0x00000004 */
#define I2C_IPD_MBRFIPD_SHIFT                              (3U)
#define I2C_IPD_MBRFIPD_MASK                               (0x1U << I2C_IPD_MBRFIPD_SHIFT)                              /* 0x00000008 */
#define I2C_IPD_STARTIPD_SHIFT                             (4U)
#define I2C_IPD_STARTIPD_MASK                              (0x1U << I2C_IPD_STARTIPD_SHIFT)                             /* 0x00000010 */
#define I2C_IPD_STOPIPD_SHIFT                              (5U)
#define I2C_IPD_STOPIPD_MASK                               (0x1U << I2C_IPD_STOPIPD_SHIFT)                              /* 0x00000020 */
#define I2C_IPD_NAKRCVIPD_SHIFT                            (6U)
#define I2C_IPD_NAKRCVIPD_MASK                             (0x1U << I2C_IPD_NAKRCVIPD_SHIFT)                            /* 0x00000040 */
#define I2C_IPD_SLAVEHDSCLIPD_SHIFT                        (7U)
#define I2C_IPD_SLAVEHDSCLIPD_MASK                         (0x1U << I2C_IPD_SLAVEHDSCLIPD_SHIFT)                        /* 0x00000080 */
#define I2C_IPD_SLAVEHDSDAIPD_SHIFT                        (8U)
#define I2C_IPD_SLAVEHDSDAIPD_MASK                         (0x1U << I2C_IPD_SLAVEHDSDAIPD_SHIFT)                        /* 0x00000100 */
/* FCNT */
#define I2C_FCNT_OFFSET                                    (0x20U)
#define I2C_FCNT                                           (0x0U)
#define I2C_FCNT_FCNT_SHIFT                                (0U)
#define I2C_FCNT_FCNT_MASK                                 (0x3FU << I2C_FCNT_FCNT_SHIFT)                               /* 0x0000003F */
/* SCL_OE_DB */
#define I2C_SCL_OE_DB_OFFSET                               (0x24U)
#define I2C_SCL_OE_DB_SCL_OE_DB_SHIFT                      (0U)
#define I2C_SCL_OE_DB_SCL_OE_DB_MASK                       (0xFFU << I2C_SCL_OE_DB_SCL_OE_DB_SHIFT)                     /* 0x000000FF */
/* TXDATA0 */
#define I2C_TXDATA0_OFFSET                                 (0x100U)
#define I2C_TXDATA0_TXDATA0_SHIFT                          (0U)
#define I2C_TXDATA0_TXDATA0_MASK                           (0xFFFFFFFFU << I2C_TXDATA0_TXDATA0_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA1 */
#define I2C_TXDATA1_OFFSET                                 (0x104U)
#define I2C_TXDATA1_TXDATA1_SHIFT                          (0U)
#define I2C_TXDATA1_TXDATA1_MASK                           (0xFFFFFFFFU << I2C_TXDATA1_TXDATA1_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA2 */
#define I2C_TXDATA2_OFFSET                                 (0x108U)
#define I2C_TXDATA2_TXDATA2_SHIFT                          (0U)
#define I2C_TXDATA2_TXDATA2_MASK                           (0xFFFFFFFFU << I2C_TXDATA2_TXDATA2_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA3 */
#define I2C_TXDATA3_OFFSET                                 (0x10CU)
#define I2C_TXDATA3_TXDATA3_SHIFT                          (0U)
#define I2C_TXDATA3_TXDATA3_MASK                           (0xFFFFFFFFU << I2C_TXDATA3_TXDATA3_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA4 */
#define I2C_TXDATA4_OFFSET                                 (0x110U)
#define I2C_TXDATA4_TXDATA4_SHIFT                          (0U)
#define I2C_TXDATA4_TXDATA4_MASK                           (0xFFFFFFFFU << I2C_TXDATA4_TXDATA4_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA5 */
#define I2C_TXDATA5_OFFSET                                 (0x114U)
#define I2C_TXDATA5_TXDATA5_SHIFT                          (0U)
#define I2C_TXDATA5_TXDATA5_MASK                           (0xFFFFFFFFU << I2C_TXDATA5_TXDATA5_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA6 */
#define I2C_TXDATA6_OFFSET                                 (0x118U)
#define I2C_TXDATA6_TXDATA6_SHIFT                          (0U)
#define I2C_TXDATA6_TXDATA6_MASK                           (0xFFFFFFFFU << I2C_TXDATA6_TXDATA6_SHIFT)                   /* 0xFFFFFFFF */
/* TXDATA7 */
#define I2C_TXDATA7_OFFSET                                 (0x11CU)
#define I2C_TXDATA7_TXDATA7_SHIFT                          (0U)
#define I2C_TXDATA7_TXDATA7_MASK                           (0xFFFFFFFFU << I2C_TXDATA7_TXDATA7_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA0 */
#define I2C_RXDATA0_OFFSET                                 (0x200U)
#define I2C_RXDATA0                                        (0x0U)
#define I2C_RXDATA0_RXDATA0_SHIFT                          (0U)
#define I2C_RXDATA0_RXDATA0_MASK                           (0xFFFFFFFFU << I2C_RXDATA0_RXDATA0_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA1 */
#define I2C_RXDATA1_OFFSET                                 (0x204U)
#define I2C_RXDATA1                                        (0x0U)
#define I2C_RXDATA1_RXDATA1_SHIFT                          (0U)
#define I2C_RXDATA1_RXDATA1_MASK                           (0xFFFFFFFFU << I2C_RXDATA1_RXDATA1_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA2 */
#define I2C_RXDATA2_OFFSET                                 (0x208U)
#define I2C_RXDATA2                                        (0x0U)
#define I2C_RXDATA2_RXDATA2_SHIFT                          (0U)
#define I2C_RXDATA2_RXDATA2_MASK                           (0xFFFFFFFFU << I2C_RXDATA2_RXDATA2_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA3 */
#define I2C_RXDATA3_OFFSET                                 (0x20CU)
#define I2C_RXDATA3                                        (0x0U)
#define I2C_RXDATA3_RXDATA3_SHIFT                          (0U)
#define I2C_RXDATA3_RXDATA3_MASK                           (0xFFFFFFFFU << I2C_RXDATA3_RXDATA3_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA4 */
#define I2C_RXDATA4_OFFSET                                 (0x210U)
#define I2C_RXDATA4                                        (0x0U)
#define I2C_RXDATA4_RXDATA4_SHIFT                          (0U)
#define I2C_RXDATA4_RXDATA4_MASK                           (0xFFFFFFFFU << I2C_RXDATA4_RXDATA4_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA5 */
#define I2C_RXDATA5_OFFSET                                 (0x214U)
#define I2C_RXDATA5                                        (0x0U)
#define I2C_RXDATA5_RXDATA5_SHIFT                          (0U)
#define I2C_RXDATA5_RXDATA5_MASK                           (0xFFFFFFFFU << I2C_RXDATA5_RXDATA5_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA6 */
#define I2C_RXDATA6_OFFSET                                 (0x218U)
#define I2C_RXDATA6                                        (0x0U)
#define I2C_RXDATA6_RXDATA6_SHIFT                          (0U)
#define I2C_RXDATA6_RXDATA6_MASK                           (0xFFFFFFFFU << I2C_RXDATA6_RXDATA6_SHIFT)                   /* 0xFFFFFFFF */
/* RXDATA7 */
#define I2C_RXDATA7_OFFSET                                 (0x21CU)
#define I2C_RXDATA7                                        (0x0U)
#define I2C_RXDATA7_RXDATA7_SHIFT                          (0U)
#define I2C_RXDATA7_RXDATA7_MASK                           (0xFFFFFFFFU << I2C_RXDATA7_RXDATA7_SHIFT)                   /* 0xFFFFFFFF */
/* ST */
#define I2C_ST_OFFSET                                      (0x220U)
#define I2C_ST                                             (0x0U)
#define I2C_ST_SDA_ST_SHIFT                                (0U)
#define I2C_ST_SDA_ST_MASK                                 (0x1U << I2C_ST_SDA_ST_SHIFT)                                /* 0x00000001 */
#define I2C_ST_SCL_ST_SHIFT                                (1U)
#define I2C_ST_SCL_ST_MASK                                 (0x1U << I2C_ST_SCL_ST_SHIFT)                                /* 0x00000002 */
/* DBGCTRL */
#define I2C_DBGCTRL_OFFSET                                 (0x224U)
#define I2C_DBGCTRL_FLT_F_SHIFT                            (0U)
#define I2C_DBGCTRL_FLT_F_MASK                             (0xFU << I2C_DBGCTRL_FLT_F_SHIFT)                            /* 0x0000000F */
#define I2C_DBGCTRL_FLT_R_SHIFT                            (4U)
#define I2C_DBGCTRL_FLT_R_MASK                             (0xFU << I2C_DBGCTRL_FLT_R_SHIFT)                            /* 0x000000F0 */
#define I2C_DBGCTRL_SLV_HOLD_SCL_TH_SHIFT                  (8U)
#define I2C_DBGCTRL_SLV_HOLD_SCL_TH_MASK                   (0xFU << I2C_DBGCTRL_SLV_HOLD_SCL_TH_SHIFT)                  /* 0x00000F00 */
#define I2C_DBGCTRL_FLT_EN_SHIFT                           (12U)
#define I2C_DBGCTRL_FLT_EN_MASK                            (0x1U << I2C_DBGCTRL_FLT_EN_SHIFT)                           /* 0x00001000 */
#define I2C_DBGCTRL_NAK_RELEASE_SCL_SHIFT                  (13U)
#define I2C_DBGCTRL_NAK_RELEASE_SCL_MASK                   (0x1U << I2C_DBGCTRL_NAK_RELEASE_SCL_SHIFT)                  /* 0x00002000 */
#define I2C_DBGCTRL_H0_CHECK_SCL_SHIFT                     (14U)
#define I2C_DBGCTRL_H0_CHECK_SCL_MASK                      (0x1U << I2C_DBGCTRL_H0_CHECK_SCL_SHIFT)                     /* 0x00004000 */
/* CON1 */
#define I2C_CON1_OFFSET                                    (0x228U)
#define I2C_CON1_AUTO_STOP_SHIFT                           (0U)
#define I2C_CON1_AUTO_STOP_MASK                            (0x1U << I2C_CON1_AUTO_STOP_SHIFT)                           /* 0x00000001 */
#define I2C_CON1_AUTO_STOP_TX_END_SHIFT                    (1U)
#define I2C_CON1_AUTO_STOP_TX_END_MASK                     (0x1U << I2C_CON1_AUTO_STOP_TX_END_SHIFT)                    /* 0x00000002 */
#define I2C_CON1_AUTO_STOP_NAK_SHIFT                       (2U)
#define I2C_CON1_AUTO_STOP_NAK_MASK                        (0x1U << I2C_CON1_AUTO_STOP_NAK_SHIFT)                       /* 0x00000004 */
/******************************************UART******************************************/
/* RBR */
#define UART_RBR_OFFSET                                    (0x0U)
#define UART_RBR                                           (0x0U)
#define UART_RBR_DATA_INPUT_SHIFT                          (0U)
#define UART_RBR_DATA_INPUT_MASK                           (0xFFU << UART_RBR_DATA_INPUT_SHIFT)                         /* 0x000000FF */
/* DLL */
#define UART_DLL_OFFSET                                    (0x0U)
#define UART_DLL_BAUD_RATE_DIVISOR_L_SHIFT                 (0U)
#define UART_DLL_BAUD_RATE_DIVISOR_L_MASK                  (0xFFU << UART_DLL_BAUD_RATE_DIVISOR_L_SHIFT)                /* 0x000000FF */
/* THR */
#define UART_THR_OFFSET                                    (0x0U)
#define UART_THR_DATA_OUTPUT_SHIFT                         (0U)
#define UART_THR_DATA_OUTPUT_MASK                          (0xFFU << UART_THR_DATA_OUTPUT_SHIFT)                        /* 0x000000FF */
/* DLH */
#define UART_DLH_OFFSET                                    (0x4U)
#define UART_DLH_BAUD_RATE_DIVISOR_H_SHIFT                 (0U)
#define UART_DLH_BAUD_RATE_DIVISOR_H_MASK                  (0xFFU << UART_DLH_BAUD_RATE_DIVISOR_H_SHIFT)                /* 0x000000FF */
/* IER */
#define UART_IER_OFFSET                                    (0x4U)
#define UART_IER_RECEIVE_DATA_AVAILABLE_INT_EN_SHIFT       (0U)
#define UART_IER_RECEIVE_DATA_AVAILABLE_INT_EN_MASK        (0x1U << UART_IER_RECEIVE_DATA_AVAILABLE_INT_EN_SHIFT)       /* 0x00000001 */
#define UART_IER_TRANS_HOLD_EMPTY_INT_EN_SHIFT             (1U)
#define UART_IER_TRANS_HOLD_EMPTY_INT_EN_MASK              (0x1U << UART_IER_TRANS_HOLD_EMPTY_INT_EN_SHIFT)             /* 0x00000002 */
#define UART_IER_RECEIVE_LINE_STATUS_INT_EN_SHIFT          (2U)
#define UART_IER_RECEIVE_LINE_STATUS_INT_EN_MASK           (0x1U << UART_IER_RECEIVE_LINE_STATUS_INT_EN_SHIFT)          /* 0x00000004 */
#define UART_IER_MODEM_STATUS_INT_EN_SHIFT                 (3U)
#define UART_IER_MODEM_STATUS_INT_EN_MASK                  (0x1U << UART_IER_MODEM_STATUS_INT_EN_SHIFT)                 /* 0x00000008 */
#define UART_IER_PROG_THRE_INT_EN_SHIFT                    (7U)
#define UART_IER_PROG_THRE_INT_EN_MASK                     (0x1U << UART_IER_PROG_THRE_INT_EN_SHIFT)                    /* 0x00000080 */
/* FCR */
#define UART_FCR_OFFSET                                    (0x8U)
#define UART_FCR_FIFO_EN_SHIFT                             (0U)
#define UART_FCR_FIFO_EN_MASK                              (0x1U << UART_FCR_FIFO_EN_SHIFT)                             /* 0x00000001 */
#define UART_FCR_RCVR_FIFO_RESET_SHIFT                     (1U)
#define UART_FCR_RCVR_FIFO_RESET_MASK                      (0x1U << UART_FCR_RCVR_FIFO_RESET_SHIFT)                     /* 0x00000002 */
#define UART_FCR_XMIT_FIFO_RESET_SHIFT                     (2U)
#define UART_FCR_XMIT_FIFO_RESET_MASK                      (0x1U << UART_FCR_XMIT_FIFO_RESET_SHIFT)                     /* 0x00000004 */
#define UART_FCR_DMA_MODE_SHIFT                            (3U)
#define UART_FCR_DMA_MODE_MASK                             (0x1U << UART_FCR_DMA_MODE_SHIFT)                            /* 0x00000008 */
#define UART_FCR_TX_EMPTY_TRIGGER_SHIFT                    (4U)
#define UART_FCR_TX_EMPTY_TRIGGER_MASK                     (0x3U << UART_FCR_TX_EMPTY_TRIGGER_SHIFT)                    /* 0x00000030 */
#define UART_FCR_RCVR_TRIGGER_SHIFT                        (6U)
#define UART_FCR_RCVR_TRIGGER_MASK                         (0x3U << UART_FCR_RCVR_TRIGGER_SHIFT)                        /* 0x000000C0 */
/* IIR */
#define UART_IIR_OFFSET                                    (0x8U)
#define UART_IIR                                           (0x1U)
#define UART_IIR_INT_ID_SHIFT                              (0U)
#define UART_IIR_INT_ID_MASK                               (0xFU << UART_IIR_INT_ID_SHIFT)                              /* 0x0000000F */
#define UART_IIR_FIFOS_EN_SHIFT                            (6U)
#define UART_IIR_FIFOS_EN_MASK                             (0x3U << UART_IIR_FIFOS_EN_SHIFT)                            /* 0x000000C0 */
/* LCR */
#define UART_LCR_OFFSET                                    (0xCU)
#define UART_LCR_DATA_LENGTH_SEL_SHIFT                     (0U)
#define UART_LCR_DATA_LENGTH_SEL_MASK                      (0x3U << UART_LCR_DATA_LENGTH_SEL_SHIFT)                     /* 0x00000003 */
#define UART_LCR_STOP_BITS_NUM_SHIFT                       (2U)
#define UART_LCR_STOP_BITS_NUM_MASK                        (0x1U << UART_LCR_STOP_BITS_NUM_SHIFT)                       /* 0x00000004 */
#define UART_LCR_PARITY_EN_SHIFT                           (3U)
#define UART_LCR_PARITY_EN_MASK                            (0x1U << UART_LCR_PARITY_EN_SHIFT)                           /* 0x00000008 */
#define UART_LCR_EVEN_PARITY_SEL_SHIFT                     (4U)
#define UART_LCR_EVEN_PARITY_SEL_MASK                      (0x1U << UART_LCR_EVEN_PARITY_SEL_SHIFT)                     /* 0x00000010 */
#define UART_LCR_BREAK_CTRL_SHIFT                          (6U)
#define UART_LCR_BREAK_CTRL_MASK                           (0x1U << UART_LCR_BREAK_CTRL_SHIFT)                          /* 0x00000040 */
#define UART_LCR_DIV_LAT_ACCESS_SHIFT                      (7U)
#define UART_LCR_DIV_LAT_ACCESS_MASK                       (0x1U << UART_LCR_DIV_LAT_ACCESS_SHIFT)                      /* 0x00000080 */
/* MCR */
#define UART_MCR_OFFSET                                    (0x10U)
#define UART_MCR_DATA_TERMINAL_READY_SHIFT                 (0U)
#define UART_MCR_DATA_TERMINAL_READY_MASK                  (0x1U << UART_MCR_DATA_TERMINAL_READY_SHIFT)                 /* 0x00000001 */
#define UART_MCR_REQ_TO_SEND_SHIFT                         (1U)
#define UART_MCR_REQ_TO_SEND_MASK                          (0x1U << UART_MCR_REQ_TO_SEND_SHIFT)                         /* 0x00000002 */
#define UART_MCR_OUT1_SHIFT                                (2U)
#define UART_MCR_OUT1_MASK                                 (0x1U << UART_MCR_OUT1_SHIFT)                                /* 0x00000004 */
#define UART_MCR_OUT2_SHIFT                                (3U)
#define UART_MCR_OUT2_MASK                                 (0x1U << UART_MCR_OUT2_SHIFT)                                /* 0x00000008 */
#define UART_MCR_LOOPBACK_SHIFT                            (4U)
#define UART_MCR_LOOPBACK_MASK                             (0x1U << UART_MCR_LOOPBACK_SHIFT)                            /* 0x00000010 */
#define UART_MCR_AUTO_FLOW_CTRL_EN_SHIFT                   (5U)
#define UART_MCR_AUTO_FLOW_CTRL_EN_MASK                    (0x1U << UART_MCR_AUTO_FLOW_CTRL_EN_SHIFT)                   /* 0x00000020 */
#define UART_MCR_SIR_MODE_EN_SHIFT                         (6U)
#define UART_MCR_SIR_MODE_EN_MASK                          (0x1U << UART_MCR_SIR_MODE_EN_SHIFT)                         /* 0x00000040 */
/* LSR */
#define UART_LSR_OFFSET                                    (0x14U)
#define UART_LSR                                           (0x60U)
#define UART_LSR_DATA_READY_SHIFT                          (0U)
#define UART_LSR_DATA_READY_MASK                           (0x1U << UART_LSR_DATA_READY_SHIFT)                          /* 0x00000001 */
#define UART_LSR_OVERRUN_ERROR_SHIFT                       (1U)
#define UART_LSR_OVERRUN_ERROR_MASK                        (0x1U << UART_LSR_OVERRUN_ERROR_SHIFT)                       /* 0x00000002 */
#define UART_LSR_PARITY_EROR_SHIFT                         (2U)
#define UART_LSR_PARITY_EROR_MASK                          (0x1U << UART_LSR_PARITY_EROR_SHIFT)                         /* 0x00000004 */
#define UART_LSR_FRAMING_ERROR_SHIFT                       (3U)
#define UART_LSR_FRAMING_ERROR_MASK                        (0x1U << UART_LSR_FRAMING_ERROR_SHIFT)                       /* 0x00000008 */
#define UART_LSR_BREAK_INT_SHIFT                           (4U)
#define UART_LSR_BREAK_INT_MASK                            (0x1U << UART_LSR_BREAK_INT_SHIFT)                           /* 0x00000010 */
#define UART_LSR_TRANS_HOLD_REG_EMPTY_SHIFT                (5U)
#define UART_LSR_TRANS_HOLD_REG_EMPTY_MASK                 (0x1U << UART_LSR_TRANS_HOLD_REG_EMPTY_SHIFT)                /* 0x00000020 */
#define UART_LSR_TRANS_EMPTY_SHIFT                         (6U)
#define UART_LSR_TRANS_EMPTY_MASK                          (0x1U << UART_LSR_TRANS_EMPTY_SHIFT)                         /* 0x00000040 */
#define UART_LSR_RECEIVER_FIFO_ERROR_SHIFT                 (7U)
#define UART_LSR_RECEIVER_FIFO_ERROR_MASK                  (0x1U << UART_LSR_RECEIVER_FIFO_ERROR_SHIFT)                 /* 0x00000080 */
/* MSR */
#define UART_MSR_OFFSET                                    (0x18U)
#define UART_MSR                                           (0x0U)
#define UART_MSR_DELTA_CLEAR_TO_SEND_SHIFT                 (0U)
#define UART_MSR_DELTA_CLEAR_TO_SEND_MASK                  (0x1U << UART_MSR_DELTA_CLEAR_TO_SEND_SHIFT)                 /* 0x00000001 */
#define UART_MSR_DELTA_DATA_SET_READY_SHIFT                (1U)
#define UART_MSR_DELTA_DATA_SET_READY_MASK                 (0x1U << UART_MSR_DELTA_DATA_SET_READY_SHIFT)                /* 0x00000002 */
#define UART_MSR_TRAILING_EDGE_RING_INDICATOR_SHIFT        (2U)
#define UART_MSR_TRAILING_EDGE_RING_INDICATOR_MASK         (0x1U << UART_MSR_TRAILING_EDGE_RING_INDICATOR_SHIFT)        /* 0x00000004 */
#define UART_MSR_DELTA_DATA_CARRIER_DETECT_SHIFT           (3U)
#define UART_MSR_DELTA_DATA_CARRIER_DETECT_MASK            (0x1U << UART_MSR_DELTA_DATA_CARRIER_DETECT_SHIFT)           /* 0x00000008 */
#define UART_MSR_CLEAR_TO_SEND_SHIFT                       (4U)
#define UART_MSR_CLEAR_TO_SEND_MASK                        (0x1U << UART_MSR_CLEAR_TO_SEND_SHIFT)                       /* 0x00000010 */
#define UART_MSR_DATA_SET_READY_SHIFT                      (5U)
#define UART_MSR_DATA_SET_READY_MASK                       (0x1U << UART_MSR_DATA_SET_READY_SHIFT)                      /* 0x00000020 */
#define UART_MSR_RING_INDICATOR_SHIFT                      (6U)
#define UART_MSR_RING_INDICATOR_MASK                       (0x1U << UART_MSR_RING_INDICATOR_SHIFT)                      /* 0x00000040 */
#define UART_MSR_DATA_CARRIOR_DETECT_SHIFT                 (7U)
#define UART_MSR_DATA_CARRIOR_DETECT_MASK                  (0x1U << UART_MSR_DATA_CARRIOR_DETECT_SHIFT)                 /* 0x00000080 */
/* SCR */
#define UART_SCR_OFFSET                                    (0x1CU)
#define UART_SCR_TEMP_STORE_SPACE_SHIFT                    (0U)
#define UART_SCR_TEMP_STORE_SPACE_MASK                     (0xFFU << UART_SCR_TEMP_STORE_SPACE_SHIFT)                   /* 0x000000FF */
/* SRBR */
#define UART_SRBR_OFFSET                                   (0x30U)
#define UART_SRBR                                          (0x0U)
#define UART_SRBR_SHADOW_RBR_SHIFT                         (0U)
#define UART_SRBR_SHADOW_RBR_MASK                          (0xFFU << UART_SRBR_SHADOW_RBR_SHIFT)                        /* 0x000000FF */
/* STHR */
#define UART_STHR_OFFSET                                   (0x30U)
#define UART_STHR_SHADOW_THR_SHIFT                         (0U)
#define UART_STHR_SHADOW_THR_MASK                          (0xFFU << UART_STHR_SHADOW_THR_SHIFT)                        /* 0x000000FF */
/* FAR */
#define UART_FAR_OFFSET                                    (0x70U)
#define UART_FAR_FIFO_ACCESS_TEST_EN_SHIFT                 (0U)
#define UART_FAR_FIFO_ACCESS_TEST_EN_MASK                  (0x1U << UART_FAR_FIFO_ACCESS_TEST_EN_SHIFT)                 /* 0x00000001 */
/* TFR */
#define UART_TFR_OFFSET                                    (0x74U)
#define UART_TFR                                           (0x0U)
#define UART_TFR_TRANS_FIFO_READ_SHIFT                     (0U)
#define UART_TFR_TRANS_FIFO_READ_MASK                      (0xFFU << UART_TFR_TRANS_FIFO_READ_SHIFT)                    /* 0x000000FF */
/* RFW */
#define UART_RFW_OFFSET                                    (0x78U)
#define UART_RFW_RECEIVE_FIFO_WRITE_SHIFT                  (0U)
#define UART_RFW_RECEIVE_FIFO_WRITE_MASK                   (0xFFU << UART_RFW_RECEIVE_FIFO_WRITE_SHIFT)                 /* 0x000000FF */
#define UART_RFW_RECEIVE_FIFO_PARITY_ERROR_SHIFT           (8U)
#define UART_RFW_RECEIVE_FIFO_PARITY_ERROR_MASK            (0x1U << UART_RFW_RECEIVE_FIFO_PARITY_ERROR_SHIFT)           /* 0x00000100 */
#define UART_RFW_RECEIVE_FIFO_FRAMING_ERROR_SHIFT          (9U)
#define UART_RFW_RECEIVE_FIFO_FRAMING_ERROR_MASK           (0x1U << UART_RFW_RECEIVE_FIFO_FRAMING_ERROR_SHIFT)          /* 0x00000200 */
/* USR */
#define UART_USR_OFFSET                                    (0x7CU)
#define UART_USR                                           (0x6U)
#define UART_USR_UART_BUSY_SHIFT                           (0U)
#define UART_USR_UART_BUSY_MASK                            (0x1U << UART_USR_UART_BUSY_SHIFT)                           /* 0x00000001 */
#define UART_USR_TRANS_FIFO_NOT_FULL_SHIFT                 (1U)
#define UART_USR_TRANS_FIFO_NOT_FULL_MASK                  (0x1U << UART_USR_TRANS_FIFO_NOT_FULL_SHIFT)                 /* 0x00000002 */
#define UART_USR_TRASN_FIFO_EMPTY_SHIFT                    (2U)
#define UART_USR_TRASN_FIFO_EMPTY_MASK                     (0x1U << UART_USR_TRASN_FIFO_EMPTY_SHIFT)                    /* 0x00000004 */
#define UART_USR_RECEIVE_FIFO_NOT_EMPTY_SHIFT              (3U)
#define UART_USR_RECEIVE_FIFO_NOT_EMPTY_MASK               (0x1U << UART_USR_RECEIVE_FIFO_NOT_EMPTY_SHIFT)              /* 0x00000008 */
#define UART_USR_RECEIVE_FIFO_FULL_SHIFT                   (4U)
#define UART_USR_RECEIVE_FIFO_FULL_MASK                    (0x1U << UART_USR_RECEIVE_FIFO_FULL_SHIFT)                   /* 0x00000010 */
/* TFL */
#define UART_TFL_OFFSET                                    (0x80U)
#define UART_TFL                                           (0x0U)
#define UART_TFL_TRANS_FIFO_LEVEL_SHIFT                    (0U)
#define UART_TFL_TRANS_FIFO_LEVEL_MASK                     (0x3FU << UART_TFL_TRANS_FIFO_LEVEL_SHIFT)                   /* 0x0000003F */
/* RFL */
#define UART_RFL_OFFSET                                    (0x84U)
#define UART_RFL                                           (0x0U)
#define UART_RFL_RECEIVE_FIFO_LEVEL_SHIFT                  (0U)
#define UART_RFL_RECEIVE_FIFO_LEVEL_MASK                   (0x3FU << UART_RFL_RECEIVE_FIFO_LEVEL_SHIFT)                 /* 0x0000003F */
/* SRR */
#define UART_SRR_OFFSET                                    (0x88U)
#define UART_SRR_UART_RESET_SHIFT                          (0U)
#define UART_SRR_UART_RESET_MASK                           (0x1U << UART_SRR_UART_RESET_SHIFT)                          /* 0x00000001 */
#define UART_SRR_RCVR_FIFO_RESET_SHIFT                     (1U)
#define UART_SRR_RCVR_FIFO_RESET_MASK                      (0x1U << UART_SRR_RCVR_FIFO_RESET_SHIFT)                     /* 0x00000002 */
#define UART_SRR_XMIT_FIFO_RESET_SHIFT                     (2U)
#define UART_SRR_XMIT_FIFO_RESET_MASK                      (0x1U << UART_SRR_XMIT_FIFO_RESET_SHIFT)                     /* 0x00000004 */
/* SRTS */
#define UART_SRTS_OFFSET                                   (0x8CU)
#define UART_SRTS_SHADOW_REQ_TO_SEND_SHIFT                 (0U)
#define UART_SRTS_SHADOW_REQ_TO_SEND_MASK                  (0x1U << UART_SRTS_SHADOW_REQ_TO_SEND_SHIFT)                 /* 0x00000001 */
/* SBCR */
#define UART_SBCR_OFFSET                                   (0x90U)
#define UART_SBCR_SHADOW_BREAK_CTRL_SHIFT                  (0U)
#define UART_SBCR_SHADOW_BREAK_CTRL_MASK                   (0x1U << UART_SBCR_SHADOW_BREAK_CTRL_SHIFT)                  /* 0x00000001 */
/* SDMAM */
#define UART_SDMAM_OFFSET                                  (0x94U)
#define UART_SDMAM_SHADOW_DMA_MODE_SHIFT                   (0U)
#define UART_SDMAM_SHADOW_DMA_MODE_MASK                    (0x1U << UART_SDMAM_SHADOW_DMA_MODE_SHIFT)                   /* 0x00000001 */
/* SFE */
#define UART_SFE_OFFSET                                    (0x98U)
#define UART_SFE_SHADOW_FIFO_EN_SHIFT                      (0U)
#define UART_SFE_SHADOW_FIFO_EN_MASK                       (0x1U << UART_SFE_SHADOW_FIFO_EN_SHIFT)                      /* 0x00000001 */
/* SRT */
#define UART_SRT_OFFSET                                    (0x9CU)
#define UART_SRT_SHADOW_RCVR_TRIGGER_SHIFT                 (0U)
#define UART_SRT_SHADOW_RCVR_TRIGGER_MASK                  (0x3U << UART_SRT_SHADOW_RCVR_TRIGGER_SHIFT)                 /* 0x00000003 */
/* STET */
#define UART_STET_OFFSET                                   (0xA0U)
#define UART_STET_SHADOW_TX_EMPTY_TRIGGER_SHIFT            (0U)
#define UART_STET_SHADOW_TX_EMPTY_TRIGGER_MASK             (0x3U << UART_STET_SHADOW_TX_EMPTY_TRIGGER_SHIFT)            /* 0x00000003 */
/* HTX */
#define UART_HTX_OFFSET                                    (0xA4U)
#define UART_HTX_HALT_TX_EN_SHIFT                          (0U)
#define UART_HTX_HALT_TX_EN_MASK                           (0x1U << UART_HTX_HALT_TX_EN_SHIFT)                          /* 0x00000001 */
/* DMASA */
#define UART_DMASA_OFFSET                                  (0xA8U)
#define UART_DMASA_DMA_SOFTWARE_ACK_SHIFT                  (0U)
#define UART_DMASA_DMA_SOFTWARE_ACK_MASK                   (0x1U << UART_DMASA_DMA_SOFTWARE_ACK_SHIFT)                  /* 0x00000001 */
/* CPR */
#define UART_CPR_OFFSET                                    (0xF4U)
#define UART_CPR                                           (0x0U)
#define UART_CPR_APB_DATA_WIDTH_SHIFT                      (0U)
#define UART_CPR_APB_DATA_WIDTH_MASK                       (0x3U << UART_CPR_APB_DATA_WIDTH_SHIFT)                      /* 0x00000003 */
#define UART_CPR_AFCE_MODE_SHIFT                           (4U)
#define UART_CPR_AFCE_MODE_MASK                            (0x1U << UART_CPR_AFCE_MODE_SHIFT)                           /* 0x00000010 */
#define UART_CPR_THRE_MODE_SHIFT                           (5U)
#define UART_CPR_THRE_MODE_MASK                            (0x1U << UART_CPR_THRE_MODE_SHIFT)                           /* 0x00000020 */
#define UART_CPR_SIR_MODE_SHIFT                            (6U)
#define UART_CPR_SIR_MODE_MASK                             (0x1U << UART_CPR_SIR_MODE_SHIFT)                            /* 0x00000040 */
#define UART_CPR_SIR_LP_MODE_SHIFT                         (7U)
#define UART_CPR_SIR_LP_MODE_MASK                          (0x1U << UART_CPR_SIR_LP_MODE_SHIFT)                         /* 0x00000080 */
#define UART_CPR_NEW_FEAT_SHIFT                            (8U)
#define UART_CPR_NEW_FEAT_MASK                             (0x1U << UART_CPR_NEW_FEAT_SHIFT)                            /* 0x00000100 */
#define UART_CPR_FIFO_ACCESS_SHIFT                         (9U)
#define UART_CPR_FIFO_ACCESS_MASK                          (0x1U << UART_CPR_FIFO_ACCESS_SHIFT)                         /* 0x00000200 */
#define UART_CPR_FIFO_STAT_SHIFT                           (10U)
#define UART_CPR_FIFO_STAT_MASK                            (0x1U << UART_CPR_FIFO_STAT_SHIFT)                           /* 0x00000400 */
#define UART_CPR_SHADOW_SHIFT                              (11U)
#define UART_CPR_SHADOW_MASK                               (0x1U << UART_CPR_SHADOW_SHIFT)                              /* 0x00000800 */
#define UART_CPR_UART_ADD_ENCODED_PARAMS_SHIFT             (12U)
#define UART_CPR_UART_ADD_ENCODED_PARAMS_MASK              (0x1U << UART_CPR_UART_ADD_ENCODED_PARAMS_SHIFT)             /* 0x00001000 */
#define UART_CPR_DMA_EXTRA_SHIFT                           (13U)
#define UART_CPR_DMA_EXTRA_MASK                            (0x1U << UART_CPR_DMA_EXTRA_SHIFT)                           /* 0x00002000 */
#define UART_CPR_FIFO_MODE_SHIFT                           (16U)
#define UART_CPR_FIFO_MODE_MASK                            (0xFFU << UART_CPR_FIFO_MODE_SHIFT)                          /* 0x00FF0000 */
/* UCV */
#define UART_UCV_OFFSET                                    (0xF8U)
#define UART_UCV                                           (0x330372AU)
#define UART_UCV_VER_SHIFT                                 (0U)
#define UART_UCV_VER_MASK                                  (0xFFFFFFFFU << UART_UCV_VER_SHIFT)                          /* 0xFFFFFFFF */
/* CTR */
#define UART_CTR_OFFSET                                    (0xFCU)
#define UART_CTR                                           (0x44570110U)
#define UART_CTR_PERIPHERAL_ID_SHIFT                       (0U)
#define UART_CTR_PERIPHERAL_ID_MASK                        (0xFFFFFFFFU << UART_CTR_PERIPHERAL_ID_SHIFT)                /* 0xFFFFFFFF */
/******************************************PWM*******************************************/
/* VERSION_ID */
#define PWM_VERSION_ID_OFFSET                              (0x0U)
#define PWM_VERSION_ID                                     (0x4000044U)
#define PWM_VERSION_ID_CHANNEL_NUM_SUPPORT_SHIFT           (0U)
#define PWM_VERSION_ID_CHANNEL_NUM_SUPPORT_MASK            (0xFU << PWM_VERSION_ID_CHANNEL_NUM_SUPPORT_SHIFT)           /* 0x0000000F */
#define PWM_VERSION_ID_CHANNEL_INDEX_SHIFT                 (4U)
#define PWM_VERSION_ID_CHANNEL_INDEX_MASK                  (0xFU << PWM_VERSION_ID_CHANNEL_INDEX_SHIFT)                 /* 0x000000F0 */
#define PWM_VERSION_ID_IR_TRANS_SUPPORT_SHIFT              (8U)
#define PWM_VERSION_ID_IR_TRANS_SUPPORT_MASK               (0x1U << PWM_VERSION_ID_IR_TRANS_SUPPORT_SHIFT)              /* 0x00000100 */
#define PWM_VERSION_ID_POWER_KEY_SUPPORT_SHIFT             (9U)
#define PWM_VERSION_ID_POWER_KEY_SUPPORT_MASK              (0x1U << PWM_VERSION_ID_POWER_KEY_SUPPORT_SHIFT)             /* 0x00000200 */
#define PWM_VERSION_ID_FREQ_METER_SUPPORT_SHIFT            (10U)
#define PWM_VERSION_ID_FREQ_METER_SUPPORT_MASK             (0x1U << PWM_VERSION_ID_FREQ_METER_SUPPORT_SHIFT)            /* 0x00000400 */
#define PWM_VERSION_ID_COUNTER_SUPPORT_SHIFT               (11U)
#define PWM_VERSION_ID_COUNTER_SUPPORT_MASK                (0x1U << PWM_VERSION_ID_COUNTER_SUPPORT_SHIFT)               /* 0x00000800 */
#define PWM_VERSION_ID_WAVE_SUPPORT_SHIFT                  (12U)
#define PWM_VERSION_ID_WAVE_SUPPORT_MASK                   (0x1U << PWM_VERSION_ID_WAVE_SUPPORT_SHIFT)                  /* 0x00001000 */
#define PWM_VERSION_ID_FILTER_SUPPORT_SHIFT                (13U)
#define PWM_VERSION_ID_FILTER_SUPPORT_MASK                 (0x1U << PWM_VERSION_ID_FILTER_SUPPORT_SHIFT)                /* 0x00002000 */
#define PWM_VERSION_ID_BIPHASIC_COUNTER_SUPPORT_SHIFT      (14U)
#define PWM_VERSION_ID_BIPHASIC_COUNTER_SUPPORT_MASK       (0x1U << PWM_VERSION_ID_BIPHASIC_COUNTER_SUPPORT_SHIFT)      /* 0x00004000 */
#define PWM_VERSION_ID_MINOR_VERSION_SHIFT                 (16U)
#define PWM_VERSION_ID_MINOR_VERSION_MASK                  (0xFFU << PWM_VERSION_ID_MINOR_VERSION_SHIFT)                /* 0x00FF0000 */
#define PWM_VERSION_ID_MAIN_VERSION_SHIFT                  (24U)
#define PWM_VERSION_ID_MAIN_VERSION_MASK                   (0xFFU << PWM_VERSION_ID_MAIN_VERSION_SHIFT)                 /* 0xFF000000 */
/* ENABLE */
#define PWM_ENABLE_OFFSET                                  (0x4U)
#define PWM_ENABLE_PWM_CLK_EN_SHIFT                        (0U)
#define PWM_ENABLE_PWM_CLK_EN_MASK                         (0x1U << PWM_ENABLE_PWM_CLK_EN_SHIFT)                        /* 0x00000001 */
#define PWM_ENABLE_PWM_EN_SHIFT                            (1U)
#define PWM_ENABLE_PWM_EN_MASK                             (0x1U << PWM_ENABLE_PWM_EN_SHIFT)                            /* 0x00000002 */
#define PWM_ENABLE_PWM_CTRL_UPDATE_EN_SHIFT                (2U)
#define PWM_ENABLE_PWM_CTRL_UPDATE_EN_MASK                 (0x1U << PWM_ENABLE_PWM_CTRL_UPDATE_EN_SHIFT)                /* 0x00000004 */
#define PWM_ENABLE_FORCE_CLK_EN_SHIFT                      (3U)
#define PWM_ENABLE_FORCE_CLK_EN_MASK                       (0x1U << PWM_ENABLE_FORCE_CLK_EN_SHIFT)                      /* 0x00000008 */
#define PWM_ENABLE_PWM_GLOBAL_JOIN_EN_SHIFT                (4U)
#define PWM_ENABLE_PWM_GLOBAL_JOIN_EN_MASK                 (0x1U << PWM_ENABLE_PWM_GLOBAL_JOIN_EN_SHIFT)                /* 0x00000010 */
#define PWM_ENABLE_PWM_CNT_RD_EN_SHIFT                     (5U)
#define PWM_ENABLE_PWM_CNT_RD_EN_MASK                      (0x1U << PWM_ENABLE_PWM_CNT_RD_EN_SHIFT)                     /* 0x00000020 */
/* CLK_CTRL */
#define PWM_CLK_CTRL_OFFSET                                (0x8U)
#define PWM_CLK_CTRL_PRESCALE_SHIFT                        (0U)
#define PWM_CLK_CTRL_PRESCALE_MASK                         (0x7U << PWM_CLK_CTRL_PRESCALE_SHIFT)                        /* 0x00000007 */
#define PWM_CLK_CTRL_SCALE_SHIFT                           (4U)
#define PWM_CLK_CTRL_SCALE_MASK                            (0x1FFU << PWM_CLK_CTRL_SCALE_SHIFT)                         /* 0x00001FF0 */
#define PWM_CLK_CTRL_CLK_SRC_SEL_SHIFT                     (13U)
#define PWM_CLK_CTRL_CLK_SRC_SEL_MASK                      (0x3U << PWM_CLK_CTRL_CLK_SRC_SEL_SHIFT)                     /* 0x00006000 */
#define PWM_CLK_CTRL_CLK_GLOBAL_SEL_SHIFT                  (15U)
#define PWM_CLK_CTRL_CLK_GLOBAL_SEL_MASK                   (0x1U << PWM_CLK_CTRL_CLK_GLOBAL_SEL_SHIFT)                  /* 0x00008000 */
/* CTRL */
#define PWM_CTRL_OFFSET                                    (0xCU)
#define PWM_CTRL_PWM_MODE_SHIFT                            (0U)
#define PWM_CTRL_PWM_MODE_MASK                             (0x3U << PWM_CTRL_PWM_MODE_SHIFT)                            /* 0x00000003 */
#define PWM_CTRL_DUTY_POL_SHIFT                            (2U)
#define PWM_CTRL_DUTY_POL_MASK                             (0x1U << PWM_CTRL_DUTY_POL_SHIFT)                            /* 0x00000004 */
#define PWM_CTRL_INACTIVE_POL_SHIFT                        (3U)
#define PWM_CTRL_INACTIVE_POL_MASK                         (0x1U << PWM_CTRL_INACTIVE_POL_SHIFT)                        /* 0x00000008 */
#define PWM_CTRL_OUTPUT_MODE_SHIFT                         (4U)
#define PWM_CTRL_OUTPUT_MODE_MASK                          (0x1U << PWM_CTRL_OUTPUT_MODE_SHIFT)                         /* 0x00000010 */
#define PWM_CTRL_ALIGNED_VLD_N_SHIFT                       (5U)
#define PWM_CTRL_ALIGNED_VLD_N_MASK                        (0x1U << PWM_CTRL_ALIGNED_VLD_N_SHIFT)                       /* 0x00000020 */
#define PWM_CTRL_PWM_IN_SEL_SHIFT                          (6U)
#define PWM_CTRL_PWM_IN_SEL_MASK                           (0x7U << PWM_CTRL_PWM_IN_SEL_SHIFT)                          /* 0x000001C0 */
/* PERIOD */
#define PWM_PERIOD_OFFSET                                  (0x10U)
#define PWM_PERIOD_PERIOD_SHIFT                            (0U)
#define PWM_PERIOD_PERIOD_MASK                             (0xFFFFFFFFU << PWM_PERIOD_PERIOD_SHIFT)                     /* 0xFFFFFFFF */
/* DUTY */
#define PWM_DUTY_OFFSET                                    (0x14U)
#define PWM_DUTY_DUTY_SHIFT                                (0U)
#define PWM_DUTY_DUTY_MASK                                 (0xFFFFFFFFU << PWM_DUTY_DUTY_SHIFT)                         /* 0xFFFFFFFF */
/* OFFSET */
#define PWM_OFFSET_OFFSET                                  (0x18U)
#define PWM_OFFSET_CHANNEL_OUTPUT_OFFSET_SHIFT             (0U)
#define PWM_OFFSET_CHANNEL_OUTPUT_OFFSET_MASK              (0xFFFFFFFFU << PWM_OFFSET_CHANNEL_OUTPUT_OFFSET_SHIFT)      /* 0xFFFFFFFF */
/* RPT */
#define PWM_RPT_OFFSET                                     (0x1CU)
#define PWM_RPT_RPT_FIRST_DIMENSIONAL_SHIFT                (0U)
#define PWM_RPT_RPT_FIRST_DIMENSIONAL_MASK                 (0xFFFFU << PWM_RPT_RPT_FIRST_DIMENSIONAL_SHIFT)             /* 0x0000FFFF */
#define PWM_RPT_RPT_SECOND_DIMENSIONAL_SHIFT               (16U)
#define PWM_RPT_RPT_SECOND_DIMENSIONAL_MASK                (0xFFFFU << PWM_RPT_RPT_SECOND_DIMENSIONAL_SHIFT)            /* 0xFFFF0000 */
/* FILTER_CTRL */
#define PWM_FILTER_CTRL_OFFSET                             (0x20U)
#define PWM_FILTER_CTRL_FILTER_ENABLE_SHIFT                (0U)
#define PWM_FILTER_CTRL_FILTER_ENABLE_MASK                 (0x1U << PWM_FILTER_CTRL_FILTER_ENABLE_SHIFT)                /* 0x00000001 */
#define PWM_FILTER_CTRL_FILTER_NUMBER_SHIFT                (4U)
#define PWM_FILTER_CTRL_FILTER_NUMBER_MASK                 (0x3FU << PWM_FILTER_CTRL_FILTER_NUMBER_SHIFT)               /* 0x000003F0 */
/* CNT */
#define PWM_CNT_OFFSET                                     (0x24U)
#define PWM_CNT                                            (0x0U)
#define PWM_CNT_CNT_SHIFT                                  (0U)
#define PWM_CNT_CNT_MASK                                   (0xFFFFFFFFU << PWM_CNT_CNT_SHIFT)                           /* 0xFFFFFFFF */
/* ENABLE_DELAY */
#define PWM_ENABLE_DELAY_OFFSET                            (0x28U)
#define PWM_ENABLE_DELAY_PWM_ENABLE_DELAY_SHIFT            (0U)
#define PWM_ENABLE_DELAY_PWM_ENABLE_DELAY_MASK             (0xFFFFU << PWM_ENABLE_DELAY_PWM_ENABLE_DELAY_SHIFT)         /* 0x0000FFFF */
/* HPC */
#define PWM_HPC_OFFSET                                     (0x2CU)
#define PWM_HPC_HPC_SHIFT                                  (0U)
#define PWM_HPC_HPC_MASK                                   (0xFFFFFFFFU << PWM_HPC_HPC_SHIFT)                           /* 0xFFFFFFFF */
/* LPC */
#define PWM_LPC_OFFSET                                     (0x30U)
#define PWM_LPC_LPC_SHIFT                                  (0U)
#define PWM_LPC_LPC_MASK                                   (0xFFFFFFFFU << PWM_LPC_LPC_SHIFT)                           /* 0xFFFFFFFF */
/* INTSTS */
#define PWM_INTSTS_OFFSET                                  (0x70U)
#define PWM_INTSTS_CAP_LPC_INTSTS_SHIFT                    (0U)
#define PWM_INTSTS_CAP_LPC_INTSTS_MASK                     (0x1U << PWM_INTSTS_CAP_LPC_INTSTS_SHIFT)                    /* 0x00000001 */
#define PWM_INTSTS_CAP_HPC_INTSTS_SHIFT                    (1U)
#define PWM_INTSTS_CAP_HPC_INTSTS_MASK                     (0x1U << PWM_INTSTS_CAP_HPC_INTSTS_SHIFT)                    /* 0x00000002 */
#define PWM_INTSTS_ONESHOT_END_INTSTS_SHIFT                (2U)
#define PWM_INTSTS_ONESHOT_END_INTSTS_MASK                 (0x1U << PWM_INTSTS_ONESHOT_END_INTSTS_SHIFT)                /* 0x00000004 */
#define PWM_INTSTS_RELOAD_INTSTS_SHIFT                     (3U)
#define PWM_INTSTS_RELOAD_INTSTS_MASK                      (0x1U << PWM_INTSTS_RELOAD_INTSTS_SHIFT)                     /* 0x00000008 */
#define PWM_INTSTS_FREQ_INTSTS_SHIFT                       (4U)
#define PWM_INTSTS_FREQ_INTSTS_MASK                        (0x1U << PWM_INTSTS_FREQ_INTSTS_SHIFT)                       /* 0x00000010 */
#define PWM_INTSTS_PWR_INTSTS_SHIFT                        (5U)
#define PWM_INTSTS_PWR_INTSTS_MASK                         (0x1U << PWM_INTSTS_PWR_INTSTS_SHIFT)                        /* 0x00000020 */
/* INT_EN */
#define PWM_INT_EN_OFFSET                                  (0x74U)
#define PWM_INT_EN_CAP_LPC_INT_EN_SHIFT                    (0U)
#define PWM_INT_EN_CAP_LPC_INT_EN_MASK                     (0x1U << PWM_INT_EN_CAP_LPC_INT_EN_SHIFT)                    /* 0x00000001 */
#define PWM_INT_EN_CAP_HPC_INT_EN_SHIFT                    (1U)
#define PWM_INT_EN_CAP_HPC_INT_EN_MASK                     (0x1U << PWM_INT_EN_CAP_HPC_INT_EN_SHIFT)                    /* 0x00000002 */
#define PWM_INT_EN_ONESHOT_END_INT_EN_SHIFT                (2U)
#define PWM_INT_EN_ONESHOT_END_INT_EN_MASK                 (0x1U << PWM_INT_EN_ONESHOT_END_INT_EN_SHIFT)                /* 0x00000004 */
#define PWM_INT_EN_RELOAD_INT_EN_SHIFT                     (3U)
#define PWM_INT_EN_RELOAD_INT_EN_MASK                      (0x1U << PWM_INT_EN_RELOAD_INT_EN_SHIFT)                     /* 0x00000008 */
#define PWM_INT_EN_FREQ_INT_EN_SHIFT                       (4U)
#define PWM_INT_EN_FREQ_INT_EN_MASK                        (0x1U << PWM_INT_EN_FREQ_INT_EN_SHIFT)                       /* 0x00000010 */
#define PWM_INT_EN_PWR_INT_EN_SHIFT                        (5U)
#define PWM_INT_EN_PWR_INT_EN_MASK                         (0x1U << PWM_INT_EN_PWR_INT_EN_SHIFT)                        /* 0x00000020 */
/* GLOBAL_ARBITER */
#define PWM_GLOBAL_ARBITER_OFFSET                          (0xC0U)
#define PWM_GLOBAL_ARBITER_GLOBAL_GRANT_SHIFT              (0U)
#define PWM_GLOBAL_ARBITER_GLOBAL_GRANT_MASK               (0xFFU << PWM_GLOBAL_ARBITER_GLOBAL_GRANT_SHIFT)             /* 0x000000FF */
#define PWM_GLOBAL_ARBITER_GLOBAL_READ_LOCK_SHIFT          (16U)
#define PWM_GLOBAL_ARBITER_GLOBAL_READ_LOCK_MASK           (0xFFU << PWM_GLOBAL_ARBITER_GLOBAL_READ_LOCK_SHIFT)         /* 0x00FF0000 */
/* GLOBAL_CTRL */
#define PWM_GLOBAL_CTRL_OFFSET                             (0xC4U)
#define PWM_GLOBAL_CTRL_GLOBAL_PWM_EN_SHIFT                (0U)
#define PWM_GLOBAL_CTRL_GLOBAL_PWM_EN_MASK                 (0x1U << PWM_GLOBAL_CTRL_GLOBAL_PWM_EN_SHIFT)                /* 0x00000001 */
#define PWM_GLOBAL_CTRL_GLOBAL_PWM_CTRL_UPDATE_EN_SHIFT    (1U)
#define PWM_GLOBAL_CTRL_GLOBAL_PWM_CTRL_UPDATE_EN_MASK     (0x1U << PWM_GLOBAL_CTRL_GLOBAL_PWM_CTRL_UPDATE_EN_SHIFT)    /* 0x00000002 */
/* PWRMATCH_ARBITER */
#define PWM_PWRMATCH_ARBITER_OFFSET                        (0x100U)
#define PWM_PWRMATCH_ARBITER_PWRKEY_GRANT_SHIFT            (0U)
#define PWM_PWRMATCH_ARBITER_PWRKEY_GRANT_MASK             (0xFFU << PWM_PWRMATCH_ARBITER_PWRKEY_GRANT_SHIFT)           /* 0x000000FF */
#define PWM_PWRMATCH_ARBITER_PWRKEY_READ_LOCK_SHIFT        (16U)
#define PWM_PWRMATCH_ARBITER_PWRKEY_READ_LOCK_MASK         (0xFFU << PWM_PWRMATCH_ARBITER_PWRKEY_READ_LOCK_SHIFT)       /* 0x00FF0000 */
/* PWRMATCH_CTRL */
#define PWM_PWRMATCH_CTRL_OFFSET                           (0x104U)
#define PWM_PWRMATCH_CTRL_PWRKEY_ENABLE_SHIFT              (0U)
#define PWM_PWRMATCH_CTRL_PWRKEY_ENABLE_MASK               (0x1U << PWM_PWRMATCH_CTRL_PWRKEY_ENABLE_SHIFT)              /* 0x00000001 */
#define PWM_PWRMATCH_CTRL_PWRKEY_POLARITY_SHIFT            (1U)
#define PWM_PWRMATCH_CTRL_PWRKEY_POLARITY_MASK             (0x1U << PWM_PWRMATCH_CTRL_PWRKEY_POLARITY_SHIFT)            /* 0x00000002 */
#define PWM_PWRMATCH_CTRL_PWRKEY_CAPTURE_CTRL_SHIFT        (2U)
#define PWM_PWRMATCH_CTRL_PWRKEY_CAPTURE_CTRL_MASK         (0x1U << PWM_PWRMATCH_CTRL_PWRKEY_CAPTURE_CTRL_SHIFT)        /* 0x00000004 */
#define PWM_PWRMATCH_CTRL_PWRKEY_INT_CTRL_SHIFT            (3U)
#define PWM_PWRMATCH_CTRL_PWRKEY_INT_CTRL_MASK             (0x1U << PWM_PWRMATCH_CTRL_PWRKEY_INT_CTRL_SHIFT)            /* 0x00000008 */
/* PWRMATCH_LPRE */
#define PWM_PWRMATCH_LPRE_OFFSET                           (0x108U)
#define PWM_PWRMATCH_LPRE_CNT_MIN_SHIFT                    (0U)
#define PWM_PWRMATCH_LPRE_CNT_MIN_MASK                     (0xFFFFU << PWM_PWRMATCH_LPRE_CNT_MIN_SHIFT)                 /* 0x0000FFFF */
#define PWM_PWRMATCH_LPRE_CNT_MAX_SHIFT                    (16U)
#define PWM_PWRMATCH_LPRE_CNT_MAX_MASK                     (0xFFFFU << PWM_PWRMATCH_LPRE_CNT_MAX_SHIFT)                 /* 0xFFFF0000 */
/* PWRMATCH_HPRE */
#define PWM_PWRMATCH_HPRE_OFFSET                           (0x10CU)
#define PWM_PWRMATCH_HPRE_CNT_MIN_SHIFT                    (0U)
#define PWM_PWRMATCH_HPRE_CNT_MIN_MASK                     (0xFFFFU << PWM_PWRMATCH_HPRE_CNT_MIN_SHIFT)                 /* 0x0000FFFF */
#define PWM_PWRMATCH_HPRE_CNT_MAX_SHIFT                    (16U)
#define PWM_PWRMATCH_HPRE_CNT_MAX_MASK                     (0xFFFFU << PWM_PWRMATCH_HPRE_CNT_MAX_SHIFT)                 /* 0xFFFF0000 */
/* PWRMATCH_LD */
#define PWM_PWRMATCH_LD_OFFSET                             (0x110U)
#define PWM_PWRMATCH_LD_CNT_MIN_SHIFT                      (0U)
#define PWM_PWRMATCH_LD_CNT_MIN_MASK                       (0xFFFFU << PWM_PWRMATCH_LD_CNT_MIN_SHIFT)                   /* 0x0000FFFF */
#define PWM_PWRMATCH_LD_CNT_MAX_SHIFT                      (16U)
#define PWM_PWRMATCH_LD_CNT_MAX_MASK                       (0xFFFFU << PWM_PWRMATCH_LD_CNT_MAX_SHIFT)                   /* 0xFFFF0000 */
/* PWRMATCH_HD_ZERO */
#define PWM_PWRMATCH_HD_ZERO_OFFSET                        (0x114U)
#define PWM_PWRMATCH_HD_ZERO_CNT_MIN_SHIFT                 (0U)
#define PWM_PWRMATCH_HD_ZERO_CNT_MIN_MASK                  (0xFFFFU << PWM_PWRMATCH_HD_ZERO_CNT_MIN_SHIFT)              /* 0x0000FFFF */
#define PWM_PWRMATCH_HD_ZERO_CNT_MAX_SHIFT                 (16U)
#define PWM_PWRMATCH_HD_ZERO_CNT_MAX_MASK                  (0xFFFFU << PWM_PWRMATCH_HD_ZERO_CNT_MAX_SHIFT)              /* 0xFFFF0000 */
/* PWRMATCH_HD_ONE */
#define PWM_PWRMATCH_HD_ONE_OFFSET                         (0x118U)
#define PWM_PWRMATCH_HD_ONE_CNT_MIN_SHIFT                  (0U)
#define PWM_PWRMATCH_HD_ONE_CNT_MIN_MASK                   (0xFFFFU << PWM_PWRMATCH_HD_ONE_CNT_MIN_SHIFT)               /* 0x0000FFFF */
#define PWM_PWRMATCH_HD_ONE_CNT_MAX_SHIFT                  (16U)
#define PWM_PWRMATCH_HD_ONE_CNT_MAX_MASK                   (0xFFFFU << PWM_PWRMATCH_HD_ONE_CNT_MAX_SHIFT)               /* 0xFFFF0000 */
/* PWRMATCH_VALUE0 */
#define PWM_PWRMATCH_VALUE0_OFFSET                         (0x11CU)
#define PWM_PWRMATCH_VALUE0_PWRKEY_MATCH_VALUE0_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE0_PWRKEY_MATCH_VALUE0_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE0_PWRKEY_MATCH_VALUE0_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE1 */
#define PWM_PWRMATCH_VALUE1_OFFSET                         (0x120U)
#define PWM_PWRMATCH_VALUE1_PWRKEY_MATCH_VALUE1_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE1_PWRKEY_MATCH_VALUE1_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE1_PWRKEY_MATCH_VALUE1_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE2 */
#define PWM_PWRMATCH_VALUE2_OFFSET                         (0x124U)
#define PWM_PWRMATCH_VALUE2_PWRKEY_MATCH_VALUE2_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE2_PWRKEY_MATCH_VALUE2_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE2_PWRKEY_MATCH_VALUE2_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE3 */
#define PWM_PWRMATCH_VALUE3_OFFSET                         (0x128U)
#define PWM_PWRMATCH_VALUE3_PWRKEY_MATCH_VALUE3_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE3_PWRKEY_MATCH_VALUE3_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE3_PWRKEY_MATCH_VALUE3_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE4 */
#define PWM_PWRMATCH_VALUE4_OFFSET                         (0x12CU)
#define PWM_PWRMATCH_VALUE4_PWRKEY_MATCH_VALUE4_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE4_PWRKEY_MATCH_VALUE4_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE4_PWRKEY_MATCH_VALUE4_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE5 */
#define PWM_PWRMATCH_VALUE5_OFFSET                         (0x130U)
#define PWM_PWRMATCH_VALUE5_PWRKEY_MATCH_VALUE5_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE5_PWRKEY_MATCH_VALUE5_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE5_PWRKEY_MATCH_VALUE5_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE6 */
#define PWM_PWRMATCH_VALUE6_OFFSET                         (0x134U)
#define PWM_PWRMATCH_VALUE6_PWRKEY_MATCH_VALUE6_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE6_PWRKEY_MATCH_VALUE6_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE6_PWRKEY_MATCH_VALUE6_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE7 */
#define PWM_PWRMATCH_VALUE7_OFFSET                         (0x138U)
#define PWM_PWRMATCH_VALUE7_PWRKEY_MATCH_VALUE7_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE7_PWRKEY_MATCH_VALUE7_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE7_PWRKEY_MATCH_VALUE7_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE8 */
#define PWM_PWRMATCH_VALUE8_OFFSET                         (0x13CU)
#define PWM_PWRMATCH_VALUE8_PWRKEY_MATCH_VALUE8_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE8_PWRKEY_MATCH_VALUE8_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE8_PWRKEY_MATCH_VALUE8_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE9 */
#define PWM_PWRMATCH_VALUE9_OFFSET                         (0x140U)
#define PWM_PWRMATCH_VALUE9_PWRKEY_MATCH_VALUE9_SHIFT      (0U)
#define PWM_PWRMATCH_VALUE9_PWRKEY_MATCH_VALUE9_MASK       (0xFFFFFFFFU << PWM_PWRMATCH_VALUE9_PWRKEY_MATCH_VALUE9_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE10 */
#define PWM_PWRMATCH_VALUE10_OFFSET                        (0x144U)
#define PWM_PWRMATCH_VALUE10_PWRKEY_MATCH_VALUE10_SHIFT    (0U)
#define PWM_PWRMATCH_VALUE10_PWRKEY_MATCH_VALUE10_MASK     (0xFFFFFFFFU << PWM_PWRMATCH_VALUE10_PWRKEY_MATCH_VALUE10_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE11 */
#define PWM_PWRMATCH_VALUE11_OFFSET                        (0x148U)
#define PWM_PWRMATCH_VALUE11_PWRKEY_MATCH_VALUE11_SHIFT    (0U)
#define PWM_PWRMATCH_VALUE11_PWRKEY_MATCH_VALUE11_MASK     (0xFFFFFFFFU << PWM_PWRMATCH_VALUE11_PWRKEY_MATCH_VALUE11_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE12 */
#define PWM_PWRMATCH_VALUE12_OFFSET                        (0x14CU)
#define PWM_PWRMATCH_VALUE12_PWRKEY_MATCH_VALUE12_SHIFT    (0U)
#define PWM_PWRMATCH_VALUE12_PWRKEY_MATCH_VALUE12_MASK     (0xFFFFFFFFU << PWM_PWRMATCH_VALUE12_PWRKEY_MATCH_VALUE12_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE13 */
#define PWM_PWRMATCH_VALUE13_OFFSET                        (0x150U)
#define PWM_PWRMATCH_VALUE13_PWRKEY_MATCH_VALUE13_SHIFT    (0U)
#define PWM_PWRMATCH_VALUE13_PWRKEY_MATCH_VALUE13_MASK     (0xFFFFFFFFU << PWM_PWRMATCH_VALUE13_PWRKEY_MATCH_VALUE13_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE14 */
#define PWM_PWRMATCH_VALUE14_OFFSET                        (0x154U)
#define PWM_PWRMATCH_VALUE14_PWRKEY_MATCH_VALUE14_SHIFT    (0U)
#define PWM_PWRMATCH_VALUE14_PWRKEY_MATCH_VALUE14_MASK     (0xFFFFFFFFU << PWM_PWRMATCH_VALUE14_PWRKEY_MATCH_VALUE14_SHIFT) /* 0xFFFFFFFF */
/* PWRMATCH_VALUE15 */
#define PWM_PWRMATCH_VALUE15_OFFSET                        (0x158U)
#define PWM_PWRMATCH_VALUE15_PWRKEY_MATCH_VALUE15_SHIFT    (0U)
#define PWM_PWRMATCH_VALUE15_PWRKEY_MATCH_VALUE15_MASK     (0xFFFFFFFFU << PWM_PWRMATCH_VALUE15_PWRKEY_MATCH_VALUE15_SHIFT) /* 0xFFFFFFFF */
/* PWRCAPTURE_VALUE */
#define PWM_PWRCAPTURE_VALUE_OFFSET                        (0x15CU)
#define PWM_PWRCAPTURE_VALUE                               (0x0U)
#define PWM_PWRCAPTURE_VALUE_PWRKEY_CAPTURE_VALUE_SHIFT    (0U)
#define PWM_PWRCAPTURE_VALUE_PWRKEY_CAPTURE_VALUE_MASK     (0xFFFFFFFFU << PWM_PWRCAPTURE_VALUE_PWRKEY_CAPTURE_VALUE_SHIFT) /* 0xFFFFFFFF */
/* FREQ_ARBITER */
#define PWM_FREQ_ARBITER_OFFSET                            (0x1C0U)
#define PWM_FREQ_ARBITER_FREQ_GRANT_SHIFT                  (0U)
#define PWM_FREQ_ARBITER_FREQ_GRANT_MASK                   (0xFFU << PWM_FREQ_ARBITER_FREQ_GRANT_SHIFT)                 /* 0x000000FF */
#define PWM_FREQ_ARBITER_FREQ_READ_LOCK_SHIFT              (16U)
#define PWM_FREQ_ARBITER_FREQ_READ_LOCK_MASK               (0xFFU << PWM_FREQ_ARBITER_FREQ_READ_LOCK_SHIFT)             /* 0x00FF0000 */
/* FREQ_CTRL */
#define PWM_FREQ_CTRL_OFFSET                               (0x1C4U)
#define PWM_FREQ_CTRL_FREQ_EN_SHIFT                        (0U)
#define PWM_FREQ_CTRL_FREQ_EN_MASK                         (0x1U << PWM_FREQ_CTRL_FREQ_EN_SHIFT)                        /* 0x00000001 */
#define PWM_FREQ_CTRL_FREQ_CLK_SEL_SHIFT                   (1U)
#define PWM_FREQ_CTRL_FREQ_CLK_SEL_MASK                    (0x1U << PWM_FREQ_CTRL_FREQ_CLK_SEL_SHIFT)                   /* 0x00000002 */
#define PWM_FREQ_CTRL_FREQ_CHANNEL_SEL_SHIFT               (3U)
#define PWM_FREQ_CTRL_FREQ_CHANNEL_SEL_MASK                (0x7U << PWM_FREQ_CTRL_FREQ_CHANNEL_SEL_SHIFT)               /* 0x00000038 */
#define PWM_FREQ_CTRL_FREQ_TIMER_CLK_SWITCH_MODE_SHIFT     (6U)
#define PWM_FREQ_CTRL_FREQ_TIMER_CLK_SWITCH_MODE_MASK      (0x1U << PWM_FREQ_CTRL_FREQ_TIMER_CLK_SWITCH_MODE_SHIFT)     /* 0x00000040 */
#define PWM_FREQ_CTRL_FREQ_TIMER_CLK_SEL_SHIFT             (7U)
#define PWM_FREQ_CTRL_FREQ_TIMER_CLK_SEL_MASK              (0x1U << PWM_FREQ_CTRL_FREQ_TIMER_CLK_SEL_SHIFT)             /* 0x00000080 */
/* FREQ_TIMER_VALUE */
#define PWM_FREQ_TIMER_VALUE_OFFSET                        (0x1C8U)
#define PWM_FREQ_TIMER_VALUE_FREQ_TIMER_VALUE_SHIFT        (0U)
#define PWM_FREQ_TIMER_VALUE_FREQ_TIMER_VALUE_MASK         (0xFFFFFFFFU << PWM_FREQ_TIMER_VALUE_FREQ_TIMER_VALUE_SHIFT) /* 0xFFFFFFFF */
/* FREQ_RESULT_VALUE */
#define PWM_FREQ_RESULT_VALUE_OFFSET                       (0x1CCU)
#define PWM_FREQ_RESULT_VALUE_FREQ_RESULT_VALUE_SHIFT      (0U)
#define PWM_FREQ_RESULT_VALUE_FREQ_RESULT_VALUE_MASK       (0xFFFFFFFFU << PWM_FREQ_RESULT_VALUE_FREQ_RESULT_VALUE_SHIFT) /* 0xFFFFFFFF */
/* COUNTER_ARBITER */
#define PWM_COUNTER_ARBITER_OFFSET                         (0x200U)
#define PWM_COUNTER_ARBITER_COUNTER_GRANT_SHIFT            (0U)
#define PWM_COUNTER_ARBITER_COUNTER_GRANT_MASK             (0xFFU << PWM_COUNTER_ARBITER_COUNTER_GRANT_SHIFT)           /* 0x000000FF */
#define PWM_COUNTER_ARBITER_COUNTER_READ_LOCK_SHIFT        (16U)
#define PWM_COUNTER_ARBITER_COUNTER_READ_LOCK_MASK         (0xFFU << PWM_COUNTER_ARBITER_COUNTER_READ_LOCK_SHIFT)       /* 0x00FF0000 */
/* COUNTER_CTRL */
#define PWM_COUNTER_CTRL_OFFSET                            (0x204U)
#define PWM_COUNTER_CTRL_COUNTER_EN_SHIFT                  (0U)
#define PWM_COUNTER_CTRL_COUNTER_EN_MASK                   (0x1U << PWM_COUNTER_CTRL_COUNTER_EN_SHIFT)                  /* 0x00000001 */
#define PWM_COUNTER_CTRL_COUNTER_CLK_SEL_SHIFT             (1U)
#define PWM_COUNTER_CTRL_COUNTER_CLK_SEL_MASK              (0x3U << PWM_COUNTER_CTRL_COUNTER_CLK_SEL_SHIFT)             /* 0x00000006 */
#define PWM_COUNTER_CTRL_COUNTER_CHANNEL_SEL_SHIFT         (3U)
#define PWM_COUNTER_CTRL_COUNTER_CHANNEL_SEL_MASK          (0x7U << PWM_COUNTER_CTRL_COUNTER_CHANNEL_SEL_SHIFT)         /* 0x00000038 */
#define PWM_COUNTER_CTRL_COUNTER_CLR_SHIFT                 (6U)
#define PWM_COUNTER_CTRL_COUNTER_CLR_MASK                  (0x1U << PWM_COUNTER_CTRL_COUNTER_CLR_SHIFT)                 /* 0x00000040 */
/* COUNTER_LOW */
#define PWM_COUNTER_LOW_OFFSET                             (0x208U)
#define PWM_COUNTER_LOW                                    (0x0U)
#define PWM_COUNTER_LOW_COUNTER_LOW_BITS_SHIFT             (0U)
#define PWM_COUNTER_LOW_COUNTER_LOW_BITS_MASK              (0xFFFFFFFFU << PWM_COUNTER_LOW_COUNTER_LOW_BITS_SHIFT)      /* 0xFFFFFFFF */
/* COUNTER_HIGH */
#define PWM_COUNTER_HIGH_OFFSET                            (0x20CU)
#define PWM_COUNTER_HIGH                                   (0x0U)
#define PWM_COUNTER_HIGH_COUNTER_HIGH_BITS_SHIFT           (0U)
#define PWM_COUNTER_HIGH_COUNTER_HIGH_BITS_MASK            (0xFFFFFFFFU << PWM_COUNTER_HIGH_COUNTER_HIGH_BITS_SHIFT)    /* 0xFFFFFFFF */
/****************************************SPI2AHB*****************************************/
/* SPI_CTRL */
#define SPI2AHB_SPI_CTRL_OFFSET                            (0x0U)
#define SPI2AHB_SPI_CTRL_FBM_SHIFT                         (0U)
#define SPI2AHB_SPI_CTRL_FBM_MASK                          (0x1U << SPI2AHB_SPI_CTRL_FBM_SHIFT)                         /* 0x00000001 */
#define SPI2AHB_SPI_CTRL_DDR_EN_SHIFT                      (1U)
#define SPI2AHB_SPI_CTRL_DDR_EN_MASK                       (0x1U << SPI2AHB_SPI_CTRL_DDR_EN_SHIFT)                      /* 0x00000002 */
#define SPI2AHB_SPI_CTRL_CS_MODE_SHIFT                     (2U)
#define SPI2AHB_SPI_CTRL_CS_MODE_MASK                      (0x1U << SPI2AHB_SPI_CTRL_CS_MODE_SHIFT)                     /* 0x00000004 */
#define SPI2AHB_SPI_CTRL_SPIM_SHIFT                        (3U)
#define SPI2AHB_SPI_CTRL_SPIM_MASK                         (0x1U << SPI2AHB_SPI_CTRL_SPIM_SHIFT)                        /* 0x00000008 */
#define SPI2AHB_SPI_CTRL_DATB_SHIFT                        (4U)
#define SPI2AHB_SPI_CTRL_DATB_MASK                         (0x3U << SPI2AHB_SPI_CTRL_DATB_SHIFT)                        /* 0x00000030 */
/* DMA_CTRL */
#define SPI2AHB_DMA_CTRL_OFFSET                            (0x4U)
#define SPI2AHB_DMA_CTRL_BURST_TYPE_SHIFT                  (0U)
#define SPI2AHB_DMA_CTRL_BURST_TYPE_MASK                   (0x3U << SPI2AHB_DMA_CTRL_BURST_TYPE_SHIFT)                  /* 0x00000003 */
/* SPI_EN */
#define SPI2AHB_SPI_EN_OFFSET                              (0xCU)
#define SPI2AHB_SPI_EN_SPI_EN_SHIFT                        (0U)
#define SPI2AHB_SPI_EN_SPI_EN_MASK                         (0x1U << SPI2AHB_SPI_EN_SPI_EN_SHIFT)                        /* 0x00000001 */
/* FRAME_ADDR */
#define SPI2AHB_FRAME_ADDR_OFFSET                          (0x10U)
#define SPI2AHB_FRAME_ADDR_FRAME_ADDR_SHIFT                (0U)
#define SPI2AHB_FRAME_ADDR_FRAME_ADDR_MASK                 (0xFFFFFFFFU << SPI2AHB_FRAME_ADDR_FRAME_ADDR_SHIFT)         /* 0xFFFFFFFF */
/* FRAME_BNUM */
#define SPI2AHB_FRAME_BNUM_OFFSET                          (0x14U)
#define SPI2AHB_FRAME_BNUM_FRAME_BNUM_SHIFT                (0U)
#define SPI2AHB_FRAME_BNUM_FRAME_BNUM_MASK                 (0xFFFFFFFFU << SPI2AHB_FRAME_BNUM_FRAME_BNUM_SHIFT)         /* 0xFFFFFFFF */
/* FRAME_TME_EN */
#define SPI2AHB_FRAME_TME_EN_OFFSET                        (0x28U)
#define SPI2AHB_FRAME_TME_EN_FRAME_TME_EN_SHIFT            (0U)
#define SPI2AHB_FRAME_TME_EN_FRAME_TME_EN_MASK             (0x1U << SPI2AHB_FRAME_TME_EN_FRAME_TME_EN_SHIFT)            /* 0x00000001 */
/* FRAME_TME */
#define SPI2AHB_FRAME_TME_OFFSET                           (0x2CU)
#define SPI2AHB_FRAME_TME_FRAME_TME_SHIFT                  (0U)
#define SPI2AHB_FRAME_TME_FRAME_TME_MASK                   (0xFFFFFFFFU << SPI2AHB_FRAME_TME_FRAME_TME_SHIFT)           /* 0xFFFFFFFF */
/* SW_RST */
#define SPI2AHB_SW_RST_OFFSET                              (0x30U)
#define SPI2AHB_SW_RST_SW_RST_SHIFT                        (0U)
#define SPI2AHB_SW_RST_SW_RST_MASK                         (0x1U << SPI2AHB_SW_RST_SW_RST_SHIFT)                        /* 0x00000001 */
/* CSN */
#define SPI2AHB_CSN_OFFSET                                 (0x38U)
#define SPI2AHB_CSN                                        (0x1U)
#define SPI2AHB_CSN_CSN_SHIFT                              (0U)
#define SPI2AHB_CSN_CSN_MASK                               (0x1U << SPI2AHB_CSN_CSN_SHIFT)                              /* 0x00000001 */
#define SPI2AHB_CSN_CSN_SRC_SHIFT                          (1U)
#define SPI2AHB_CSN_CSN_SRC_MASK                           (0x1U << SPI2AHB_CSN_CSN_SRC_SHIFT)                          /* 0x00000002 */
/* IMR */
#define SPI2AHB_IMR_OFFSET                                 (0x40U)
#define SPI2AHB_IMR_FRAME_ENDM_SHIFT                       (0U)
#define SPI2AHB_IMR_FRAME_ENDM_MASK                        (0x1U << SPI2AHB_IMR_FRAME_ENDM_SHIFT)                       /* 0x00000001 */
#define SPI2AHB_IMR_PFIFO_ERM_SHIFT                        (1U)
#define SPI2AHB_IMR_PFIFO_ERM_MASK                         (0x1U << SPI2AHB_IMR_PFIFO_ERM_SHIFT)                        /* 0x00000002 */
#define SPI2AHB_IMR_RFIFO_ERM_SHIFT                        (2U)
#define SPI2AHB_IMR_RFIFO_ERM_MASK                         (0x1U << SPI2AHB_IMR_RFIFO_ERM_SHIFT)                        /* 0x00000004 */
#define SPI2AHB_IMR_RFIFO_FWM_SHIFT                        (3U)
#define SPI2AHB_IMR_RFIFO_FWM_MASK                         (0x1U << SPI2AHB_IMR_RFIFO_FWM_SHIFT)                        /* 0x00000008 */
#define SPI2AHB_IMR_FRAME_ERRM_SHIFT                       (4U)
#define SPI2AHB_IMR_FRAME_ERRM_MASK                        (0x1U << SPI2AHB_IMR_FRAME_ERRM_SHIFT)                       /* 0x00000010 */
#define SPI2AHB_IMR_DMA_ERRM_SHIFT                         (5U)
#define SPI2AHB_IMR_DMA_ERRM_MASK                          (0x1U << SPI2AHB_IMR_DMA_ERRM_SHIFT)                         /* 0x00000020 */
#define SPI2AHB_IMR_FRAME_MISM_SHIFT                       (6U)
#define SPI2AHB_IMR_FRAME_MISM_MASK                        (0x1U << SPI2AHB_IMR_FRAME_MISM_SHIFT)                       /* 0x00000040 */
#define SPI2AHB_IMR_TIMEOUTM_SHIFT                         (7U)
#define SPI2AHB_IMR_TIMEOUTM_MASK                          (0x1U << SPI2AHB_IMR_TIMEOUTM_SHIFT)                         /* 0x00000080 */
/* ICLR */
#define SPI2AHB_ICLR_OFFSET                                (0x44U)
#define SPI2AHB_ICLR_FRAME_ENDC_SHIFT                      (0U)
#define SPI2AHB_ICLR_FRAME_ENDC_MASK                       (0x1U << SPI2AHB_ICLR_FRAME_ENDC_SHIFT)                      /* 0x00000001 */
#define SPI2AHB_ICLR_PFIFO_ERC_SHIFT                       (1U)
#define SPI2AHB_ICLR_PFIFO_ERC_MASK                        (0x1U << SPI2AHB_ICLR_PFIFO_ERC_SHIFT)                       /* 0x00000002 */
#define SPI2AHB_ICLR_RFIFO_ERC_SHIFT                       (2U)
#define SPI2AHB_ICLR_RFIFO_ERC_MASK                        (0x1U << SPI2AHB_ICLR_RFIFO_ERC_SHIFT)                       /* 0x00000004 */
#define SPI2AHB_ICLR_RFIFO_FWC_SHIFT                       (3U)
#define SPI2AHB_ICLR_RFIFO_FWC_MASK                        (0x1U << SPI2AHB_ICLR_RFIFO_FWC_SHIFT)                       /* 0x00000008 */
#define SPI2AHB_ICLR_FRAME_ERRC_SHIFT                      (4U)
#define SPI2AHB_ICLR_FRAME_ERRC_MASK                       (0x1U << SPI2AHB_ICLR_FRAME_ERRC_SHIFT)                      /* 0x00000010 */
#define SPI2AHB_ICLR_DMA_ERRC_SHIFT                        (5U)
#define SPI2AHB_ICLR_DMA_ERRC_MASK                         (0x1U << SPI2AHB_ICLR_DMA_ERRC_SHIFT)                        /* 0x00000020 */
#define SPI2AHB_ICLR_FRAME_MISC_SHIFT                      (6U)
#define SPI2AHB_ICLR_FRAME_MISC_MASK                       (0x1U << SPI2AHB_ICLR_FRAME_MISC_SHIFT)                      /* 0x00000040 */
#define SPI2AHB_ICLR_TIMEOUTC_SHIFT                        (7U)
#define SPI2AHB_ICLR_TIMEOUTC_MASK                         (0x1U << SPI2AHB_ICLR_TIMEOUTC_SHIFT)                        /* 0x00000080 */
/* ISR */
#define SPI2AHB_ISR_OFFSET                                 (0x48U)
#define SPI2AHB_ISR                                        (0x0U)
#define SPI2AHB_ISR_FRAME_ENDS_SHIFT                       (0U)
#define SPI2AHB_ISR_FRAME_ENDS_MASK                        (0x1U << SPI2AHB_ISR_FRAME_ENDS_SHIFT)                       /* 0x00000001 */
#define SPI2AHB_ISR_PFIFO_ERS_SHIFT                        (1U)
#define SPI2AHB_ISR_PFIFO_ERS_MASK                         (0x1U << SPI2AHB_ISR_PFIFO_ERS_SHIFT)                        /* 0x00000002 */
#define SPI2AHB_ISR_RFIFO_ERS_SHIFT                        (2U)
#define SPI2AHB_ISR_RFIFO_ERS_MASK                         (0x1U << SPI2AHB_ISR_RFIFO_ERS_SHIFT)                        /* 0x00000004 */
#define SPI2AHB_ISR_RFIFO_FWS_SHIFT                        (3U)
#define SPI2AHB_ISR_RFIFO_FWS_MASK                         (0x1U << SPI2AHB_ISR_RFIFO_FWS_SHIFT)                        /* 0x00000008 */
#define SPI2AHB_ISR_FRAME_ERRS_SHIFT                       (4U)
#define SPI2AHB_ISR_FRAME_ERRS_MASK                        (0x1U << SPI2AHB_ISR_FRAME_ERRS_SHIFT)                       /* 0x00000010 */
#define SPI2AHB_ISR_DMA_ERRS_SHIFT                         (5U)
#define SPI2AHB_ISR_DMA_ERRS_MASK                          (0x1U << SPI2AHB_ISR_DMA_ERRS_SHIFT)                         /* 0x00000020 */
#define SPI2AHB_ISR_FRAME_MISS_SHIFT                       (6U)
#define SPI2AHB_ISR_FRAME_MISS_MASK                        (0x1U << SPI2AHB_ISR_FRAME_MISS_SHIFT)                       /* 0x00000040 */
#define SPI2AHB_ISR_TIMEOUTS_SHIFT                         (7U)
#define SPI2AHB_ISR_TIMEOUTS_MASK                          (0x1U << SPI2AHB_ISR_TIMEOUTS_SHIFT)                         /* 0x00000080 */
/* RISR */
#define SPI2AHB_RISR_OFFSET                                (0x4CU)
#define SPI2AHB_RISR                                       (0x0U)
#define SPI2AHB_RISR_FRAME_ENDS_SHIFT                      (0U)
#define SPI2AHB_RISR_FRAME_ENDS_MASK                       (0x1U << SPI2AHB_RISR_FRAME_ENDS_SHIFT)                      /* 0x00000001 */
#define SPI2AHB_RISR_PFIFO_ERS_SHIFT                       (1U)
#define SPI2AHB_RISR_PFIFO_ERS_MASK                        (0x1U << SPI2AHB_RISR_PFIFO_ERS_SHIFT)                       /* 0x00000002 */
#define SPI2AHB_RISR_RFIFO_ERS_SHIFT                       (2U)
#define SPI2AHB_RISR_RFIFO_ERS_MASK                        (0x1U << SPI2AHB_RISR_RFIFO_ERS_SHIFT)                       /* 0x00000004 */
#define SPI2AHB_RISR_RFIFO_FWS_SHIFT                       (3U)
#define SPI2AHB_RISR_RFIFO_FWS_MASK                        (0x1U << SPI2AHB_RISR_RFIFO_FWS_SHIFT)                       /* 0x00000008 */
#define SPI2AHB_RISR_FRAME_ERRS_SHIFT                      (4U)
#define SPI2AHB_RISR_FRAME_ERRS_MASK                       (0x1U << SPI2AHB_RISR_FRAME_ERRS_SHIFT)                      /* 0x00000010 */
#define SPI2AHB_RISR_DMA_ERRS_SHIFT                        (5U)
#define SPI2AHB_RISR_DMA_ERRS_MASK                         (0x1U << SPI2AHB_RISR_DMA_ERRS_SHIFT)                        /* 0x00000020 */
#define SPI2AHB_RISR_FRAME_MISS_SHIFT                      (6U)
#define SPI2AHB_RISR_FRAME_MISS_MASK                       (0x1U << SPI2AHB_RISR_FRAME_MISS_SHIFT)                      /* 0x00000040 */
#define SPI2AHB_RISR_TIMEOUTS_SHIFT                        (7U)
#define SPI2AHB_RISR_TIMEOUTS_MASK                         (0x1U << SPI2AHB_RISR_TIMEOUTS_SHIFT)                        /* 0x00000080 */
/* VER */
#define SPI2AHB_VER_OFFSET                                 (0x50U)
#define SPI2AHB_VER                                        (0x1U)
#define SPI2AHB_VER_VER_SHIFT                              (0U)
#define SPI2AHB_VER_VER_MASK                               (0xFFFFU << SPI2AHB_VER_VER_SHIFT)                           /* 0x0000FFFF */
/* MONITOR */
#define SPI2AHB_MONITOR_OFFSET                             (0x54U)
#define SPI2AHB_MONITOR                                    (0x1U)
#define SPI2AHB_MONITOR_RX_FIFO_EMPTY_SHIFT                (0U)
#define SPI2AHB_MONITOR_RX_FIFO_EMPTY_MASK                 (0x1U << SPI2AHB_MONITOR_RX_FIFO_EMPTY_SHIFT)                /* 0x00000001 */
#define SPI2AHB_MONITOR_SCLK_CNT_STATE_SHIFT               (1U)
#define SPI2AHB_MONITOR_SCLK_CNT_STATE_MASK                (0x1U << SPI2AHB_MONITOR_SCLK_CNT_STATE_SHIFT)               /* 0x00000002 */
#define SPI2AHB_MONITOR_FSM_STATE_SHIFT                    (4U)
#define SPI2AHB_MONITOR_FSM_STATE_MASK                     (0x3U << SPI2AHB_MONITOR_FSM_STATE_SHIFT)                    /* 0x00000030 */
#define SPI2AHB_MONITOR_IDLE_STATE_SHIFT                   (8U)
#define SPI2AHB_MONITOR_IDLE_STATE_MASK                    (0x1U << SPI2AHB_MONITOR_IDLE_STATE_SHIFT)                   /* 0x00000100 */
/* FRAME_CNT_MONITOR */
#define SPI2AHB_FRAME_CNT_MONITOR_OFFSET                   (0x58U)
#define SPI2AHB_FRAME_CNT_MONITOR                          (0x0U)
#define SPI2AHB_FRAME_CNT_MONITOR_FRAME_CNT_MONITOR_SHIFT  (0U)
#define SPI2AHB_FRAME_CNT_MONITOR_FRAME_CNT_MONITOR_MASK   (0xFFFFFFFFU << SPI2AHB_FRAME_CNT_MONITOR_FRAME_CNT_MONITOR_SHIFT) /* 0xFFFFFFFF */
/******************************************MBOX******************************************/
/* A2B_INTEN */
#define MBOX_A2B_INTEN_OFFSET                              (0x0U)
#define MBOX_A2B_INTEN_A2B_INTENX_SHIFT                    (0U)
#define MBOX_A2B_INTEN_A2B_INTENX_MASK                     (0x1U << MBOX_A2B_INTEN_A2B_INTENX_SHIFT)                    /* 0x00000001 */
#define MBOX_A2B_INTEN_A2B_TRIG_MODEX_SHIFT                (8U)
#define MBOX_A2B_INTEN_A2B_TRIG_MODEX_MASK                 (0x1U << MBOX_A2B_INTEN_A2B_TRIG_MODEX_SHIFT)                /* 0x00000100 */
/* A2B_STATUS */
#define MBOX_A2B_STATUS_OFFSET                             (0x4U)
#define MBOX_A2B_STATUS_A2B_INTX_SHIFT                     (0U)
#define MBOX_A2B_STATUS_A2B_INTX_MASK                      (0x1U << MBOX_A2B_STATUS_A2B_INTX_SHIFT)                     /* 0x00000001 */
/* A2B_CMD */
#define MBOX_A2B_CMD_OFFSET                                (0x8U)
#define MBOX_A2B_CMD_COMMAND_SHIFT                         (0U)
#define MBOX_A2B_CMD_COMMAND_MASK                          (0xFFFFFFFFU << MBOX_A2B_CMD_COMMAND_SHIFT)                  /* 0xFFFFFFFF */
/* A2B_DATA */
#define MBOX_A2B_DATA_OFFSET                               (0xCU)
#define MBOX_A2B_DATA_DATA_SHIFT                           (0U)
#define MBOX_A2B_DATA_DATA_MASK                            (0xFFFFFFFFU << MBOX_A2B_DATA_DATA_SHIFT)                    /* 0xFFFFFFFF */
/* B2A_INTEN */
#define MBOX_B2A_INTEN_OFFSET                              (0x10U)
#define MBOX_B2A_INTEN_B2A_INTENX_SHIFT                    (0U)
#define MBOX_B2A_INTEN_B2A_INTENX_MASK                     (0x1U << MBOX_B2A_INTEN_B2A_INTENX_SHIFT)                    /* 0x00000001 */
#define MBOX_B2A_INTEN_B2A_TRIG_MODEX_SHIFT                (8U)
#define MBOX_B2A_INTEN_B2A_TRIG_MODEX_MASK                 (0x1U << MBOX_B2A_INTEN_B2A_TRIG_MODEX_SHIFT)                /* 0x00000100 */
/* B2A_STATUS */
#define MBOX_B2A_STATUS_OFFSET                             (0x14U)
#define MBOX_B2A_STATUS_B2A_INTX_SHIFT                     (0U)
#define MBOX_B2A_STATUS_B2A_INTX_MASK                      (0x1U << MBOX_B2A_STATUS_B2A_INTX_SHIFT)                     /* 0x00000001 */
/* B2A_CMD */
#define MBOX_B2A_CMD_OFFSET                                (0x18U)
#define MBOX_B2A_CMD_COMMAND_SHIFT                         (0U)
#define MBOX_B2A_CMD_COMMAND_MASK                          (0xFFFFFFFFU << MBOX_B2A_CMD_COMMAND_SHIFT)                  /* 0xFFFFFFFF */
/* B2A_DATA */
#define MBOX_B2A_DATA_OFFSET                               (0x1CU)
#define MBOX_B2A_DATA_DATA_SHIFT                           (0U)
#define MBOX_B2A_DATA_DATA_MASK                            (0xFFFFFFFFU << MBOX_B2A_DATA_DATA_SHIFT)                    /* 0xFFFFFFFF */
/******************************************FSPI******************************************/
/* CTRL0 */
#define FSPI_CTRL0_OFFSET                                  (0x0U)
#define FSPI_CTRL0_SPIM_SHIFT                              (0U)
#define FSPI_CTRL0_SPIM_MASK                               (0x1U << FSPI_CTRL0_SPIM_SHIFT)                              /* 0x00000001 */
#define FSPI_CTRL0_SHIFTPHASE_SHIFT                        (1U)
#define FSPI_CTRL0_SHIFTPHASE_MASK                         (0x1U << FSPI_CTRL0_SHIFTPHASE_SHIFT)                        /* 0x00000002 */
#define FSPI_CTRL0_DTR_MODE_SHIFT                          (2U)
#define FSPI_CTRL0_DTR_MODE_MASK                           (0x1U << FSPI_CTRL0_DTR_MODE_SHIFT)                          /* 0x00000004 */
#define FSPI_CTRL0_DAT_ORDER_SHIFT                         (3U)
#define FSPI_CTRL0_DAT_ORDER_MASK                          (0x1U << FSPI_CTRL0_DAT_ORDER_SHIFT)                         /* 0x00000008 */
#define FSPI_CTRL0_IDLE_CYCLE_SHIFT                        (4U)
#define FSPI_CTRL0_IDLE_CYCLE_MASK                         (0xFU << FSPI_CTRL0_IDLE_CYCLE_SHIFT)                        /* 0x000000F0 */
#define FSPI_CTRL0_CMDB_SHIFT                              (8U)
#define FSPI_CTRL0_CMDB_MASK                               (0x3U << FSPI_CTRL0_CMDB_SHIFT)                              /* 0x00000300 */
#define FSPI_CTRL0_ADRB_SHIFT                              (10U)
#define FSPI_CTRL0_ADRB_MASK                               (0x3U << FSPI_CTRL0_ADRB_SHIFT)                              /* 0x00000C00 */
#define FSPI_CTRL0_DATB_SHIFT                              (12U)
#define FSPI_CTRL0_DATB_MASK                               (0x3U << FSPI_CTRL0_DATB_SHIFT)                              /* 0x00003000 */
#define FSPI_CTRL0_CMDB16_SHIFT                            (14U)
#define FSPI_CTRL0_CMDB16_MASK                             (0x1U << FSPI_CTRL0_CMDB16_SHIFT)                            /* 0x00004000 */
#define FSPI_CTRL0_ADRB16_SHIFT                            (15U)
#define FSPI_CTRL0_ADRB16_MASK                             (0x1U << FSPI_CTRL0_ADRB16_SHIFT)                            /* 0x00008000 */
#define FSPI_CTRL0_DATAB16_SHIFT                           (16U)
#define FSPI_CTRL0_DATAB16_MASK                            (0x1U << FSPI_CTRL0_DATAB16_SHIFT)                           /* 0x00010000 */
#define FSPI_CTRL0_DQS_MODE_SHIFT                          (17U)
#define FSPI_CTRL0_DQS_MODE_MASK                           (0x1U << FSPI_CTRL0_DQS_MODE_SHIFT)                          /* 0x00020000 */
#define FSPI_CTRL0_DTR_RDC_SEL_SHIFT                       (18U)
#define FSPI_CTRL0_DTR_RDC_SEL_MASK                        (0x1U << FSPI_CTRL0_DTR_RDC_SEL_SHIFT)                       /* 0x00040000 */
#define FSPI_CTRL0_CMD_STR_SHIFT                           (19U)
#define FSPI_CTRL0_CMD_STR_MASK                            (0x1U << FSPI_CTRL0_CMD_STR_SHIFT)                           /* 0x00080000 */
#define FSPI_CTRL0_ADDR_STR_SHIFT                          (20U)
#define FSPI_CTRL0_ADDR_STR_MASK                           (0x1U << FSPI_CTRL0_ADDR_STR_SHIFT)                          /* 0x00100000 */
#define FSPI_CTRL0_DBL_DUMM_CTRL_SHIFT                     (21U)
#define FSPI_CTRL0_DBL_DUMM_CTRL_MASK                      (0x1U << FSPI_CTRL0_DBL_DUMM_CTRL_SHIFT)                     /* 0x00200000 */
#define FSPI_CTRL0_DUMM_OVLP_SHIFT                         (22U)
#define FSPI_CTRL0_DUMM_OVLP_MASK                          (0x3U << FSPI_CTRL0_DUMM_OVLP_SHIFT)                         /* 0x00C00000 */
#define FSPI_CTRL0_INTRNL_CLK_MODE_SHIFT                   (24U)
#define FSPI_CTRL0_INTRNL_CLK_MODE_MASK                    (0x1U << FSPI_CTRL0_INTRNL_CLK_MODE_SHIFT)                   /* 0x01000000 */
#define FSPI_CTRL0_HYPER_ADDR_EN_SHIFT                     (25U)
#define FSPI_CTRL0_HYPER_ADDR_EN_MASK                      (0x1U << FSPI_CTRL0_HYPER_ADDR_EN_SHIFT)                     /* 0x02000000 */
#define FSPI_CTRL0_HYPER_RSVD_EN_SHIFT                     (26U)
#define FSPI_CTRL0_HYPER_RSVD_EN_MASK                      (0x1U << FSPI_CTRL0_HYPER_RSVD_EN_SHIFT)                     /* 0x04000000 */
#define FSPI_CTRL0_CMD_CTRL_SHIFT                          (27U)
#define FSPI_CTRL0_CMD_CTRL_MASK                           (0x3U << FSPI_CTRL0_CMD_CTRL_SHIFT)                          /* 0x18000000 */
#define FSPI_CTRL0_WP_EN_SHIFT                             (29U)
#define FSPI_CTRL0_WP_EN_MASK                              (0x1U << FSPI_CTRL0_WP_EN_SHIFT)                             /* 0x20000000 */
/* IMR */
#define FSPI_IMR_OFFSET                                    (0x4U)
#define FSPI_IMR_RXFM_SHIFT                                (0U)
#define FSPI_IMR_RXFM_MASK                                 (0x1U << FSPI_IMR_RXFM_SHIFT)                                /* 0x00000001 */
#define FSPI_IMR_RXUM_SHIFT                                (1U)
#define FSPI_IMR_RXUM_MASK                                 (0x1U << FSPI_IMR_RXUM_SHIFT)                                /* 0x00000002 */
#define FSPI_IMR_TXOM_SHIFT                                (2U)
#define FSPI_IMR_TXOM_MASK                                 (0x1U << FSPI_IMR_TXOM_SHIFT)                                /* 0x00000004 */
#define FSPI_IMR_TXEM_SHIFT                                (3U)
#define FSPI_IMR_TXEM_MASK                                 (0x1U << FSPI_IMR_TXEM_SHIFT)                                /* 0x00000008 */
#define FSPI_IMR_TRANSM_SHIFT                              (4U)
#define FSPI_IMR_TRANSM_MASK                               (0x1U << FSPI_IMR_TRANSM_SHIFT)                              /* 0x00000010 */
#define FSPI_IMR_AHBM_SHIFT                                (5U)
#define FSPI_IMR_AHBM_MASK                                 (0x1U << FSPI_IMR_AHBM_SHIFT)                                /* 0x00000020 */
#define FSPI_IMR_NSPIM_SHIFT                               (6U)
#define FSPI_IMR_NSPIM_MASK                                (0x1U << FSPI_IMR_NSPIM_SHIFT)                               /* 0x00000040 */
#define FSPI_IMR_DMAM_SHIFT                                (7U)
#define FSPI_IMR_DMAM_MASK                                 (0x1U << FSPI_IMR_DMAM_SHIFT)                                /* 0x00000080 */
#define FSPI_IMR_ILLGL_WRM_SHIFT                           (9U)
#define FSPI_IMR_ILLGL_WRM_MASK                            (0x1U << FSPI_IMR_ILLGL_WRM_SHIFT)                           /* 0x00000200 */
#define FSPI_IMR_NODE_ILLGLM_SHIFT                         (10U)
#define FSPI_IMR_NODE_ILLGLM_MASK                          (0x1U << FSPI_IMR_NODE_ILLGLM_SHIFT)                         /* 0x00000400 */
#define FSPI_IMR_CHAINM_SHIFT                              (11U)
#define FSPI_IMR_CHAINM_MASK                               (0x1U << FSPI_IMR_CHAINM_SHIFT)                              /* 0x00000800 */
/* ICLR */
#define FSPI_ICLR_OFFSET                                   (0x8U)
#define FSPI_ICLR_RXFC_SHIFT                               (0U)
#define FSPI_ICLR_RXFC_MASK                                (0x1U << FSPI_ICLR_RXFC_SHIFT)                               /* 0x00000001 */
#define FSPI_ICLR_RXUC_SHIFT                               (1U)
#define FSPI_ICLR_RXUC_MASK                                (0x1U << FSPI_ICLR_RXUC_SHIFT)                               /* 0x00000002 */
#define FSPI_ICLR_TXOC_SHIFT                               (2U)
#define FSPI_ICLR_TXOC_MASK                                (0x1U << FSPI_ICLR_TXOC_SHIFT)                               /* 0x00000004 */
#define FSPI_ICLR_TXEC_SHIFT                               (3U)
#define FSPI_ICLR_TXEC_MASK                                (0x1U << FSPI_ICLR_TXEC_SHIFT)                               /* 0x00000008 */
#define FSPI_ICLR_TRANSC_SHIFT                             (4U)
#define FSPI_ICLR_TRANSC_MASK                              (0x1U << FSPI_ICLR_TRANSC_SHIFT)                             /* 0x00000010 */
#define FSPI_ICLR_AHBC_SHIFT                               (5U)
#define FSPI_ICLR_AHBC_MASK                                (0x1U << FSPI_ICLR_AHBC_SHIFT)                               /* 0x00000020 */
#define FSPI_ICLR_NSPIC_SHIFT                              (6U)
#define FSPI_ICLR_NSPIC_MASK                               (0x1U << FSPI_ICLR_NSPIC_SHIFT)                              /* 0x00000040 */
#define FSPI_ICLR_DMAC_SHIFT                               (7U)
#define FSPI_ICLR_DMAC_MASK                                (0x1U << FSPI_ICLR_DMAC_SHIFT)                               /* 0x00000080 */
#define FSPI_ICLR_ILLGL_WRC_SHIFT                          (9U)
#define FSPI_ICLR_ILLGL_WRC_MASK                           (0x1U << FSPI_ICLR_ILLGL_WRC_SHIFT)                          /* 0x00000200 */
#define FSPI_ICLR_NODE_ILLGLM_SHIFT                        (10U)
#define FSPI_ICLR_NODE_ILLGLM_MASK                         (0x1U << FSPI_ICLR_NODE_ILLGLM_SHIFT)                        /* 0x00000400 */
#define FSPI_ICLR_CHAINC_SHIFT                             (11U)
#define FSPI_ICLR_CHAINC_MASK                              (0x1U << FSPI_ICLR_CHAINC_SHIFT)                             /* 0x00000800 */
/* FTLR */
#define FSPI_FTLR_OFFSET                                   (0xCU)
#define FSPI_FTLR_TXFTLR_SHIFT                             (0U)
#define FSPI_FTLR_TXFTLR_MASK                              (0xFFU << FSPI_FTLR_TXFTLR_SHIFT)                            /* 0x000000FF */
#define FSPI_FTLR_RXFTLR_SHIFT                             (8U)
#define FSPI_FTLR_RXFTLR_MASK                              (0xFFU << FSPI_FTLR_RXFTLR_SHIFT)                            /* 0x0000FF00 */
/* RCVR */
#define FSPI_RCVR_OFFSET                                   (0x10U)
#define FSPI_RCVR_RCVR_SHIFT                               (0U)
#define FSPI_RCVR_RCVR_MASK                                (0x1U << FSPI_RCVR_RCVR_SHIFT)                               /* 0x00000001 */
/* AX0 */
#define FSPI_AX0_OFFSET                                    (0x14U)
#define FSPI_AX0_AX_SHIFT                                  (0U)
#define FSPI_AX0_AX_MASK                                   (0xFFU << FSPI_AX0_AX_SHIFT)                                 /* 0x000000FF */
/* ABIT0 */
#define FSPI_ABIT0_OFFSET                                  (0x18U)
#define FSPI_ABIT0_ABIT_SHIFT                              (0U)
#define FSPI_ABIT0_ABIT_MASK                               (0x3FU << FSPI_ABIT0_ABIT_SHIFT)                             /* 0x0000003F */
/* ISR */
#define FSPI_ISR_OFFSET                                    (0x1CU)
#define FSPI_ISR_RXFS_SHIFT                                (0U)
#define FSPI_ISR_RXFS_MASK                                 (0x1U << FSPI_ISR_RXFS_SHIFT)                                /* 0x00000001 */
#define FSPI_ISR_RXUS_SHIFT                                (1U)
#define FSPI_ISR_RXUS_MASK                                 (0x1U << FSPI_ISR_RXUS_SHIFT)                                /* 0x00000002 */
#define FSPI_ISR_TXOS_SHIFT                                (2U)
#define FSPI_ISR_TXOS_MASK                                 (0x1U << FSPI_ISR_TXOS_SHIFT)                                /* 0x00000004 */
#define FSPI_ISR_TXES_SHIFT                                (3U)
#define FSPI_ISR_TXES_MASK                                 (0x1U << FSPI_ISR_TXES_SHIFT)                                /* 0x00000008 */
#define FSPI_ISR_TRANSS_SHIFT                              (4U)
#define FSPI_ISR_TRANSS_MASK                               (0x1U << FSPI_ISR_TRANSS_SHIFT)                              /* 0x00000010 */
#define FSPI_ISR_AHBS_SHIFT                                (5U)
#define FSPI_ISR_AHBS_MASK                                 (0x1U << FSPI_ISR_AHBS_SHIFT)                                /* 0x00000020 */
#define FSPI_ISR_NSPIS_SHIFT                               (6U)
#define FSPI_ISR_NSPIS_MASK                                (0x1U << FSPI_ISR_NSPIS_SHIFT)                               /* 0x00000040 */
#define FSPI_ISR_DMAS_SHIFT                                (7U)
#define FSPI_ISR_DMAS_MASK                                 (0x1U << FSPI_ISR_DMAS_SHIFT)                                /* 0x00000080 */
#define FSPI_ISR_NODE_ILLGLS_SHIFT                         (10U)
#define FSPI_ISR_NODE_ILLGLS_MASK                          (0x1U << FSPI_ISR_NODE_ILLGLS_SHIFT)                         /* 0x00000400 */
#define FSPI_ISR_CHAINS_SHIFT                              (11U)
#define FSPI_ISR_CHAINS_MASK                               (0x1U << FSPI_ISR_CHAINS_SHIFT)                              /* 0x00000800 */
/* FSR */
#define FSPI_FSR_OFFSET                                    (0x20U)
#define FSPI_FSR_TXFS_SHIFT                                (0U)
#define FSPI_FSR_TXFS_MASK                                 (0x1U << FSPI_FSR_TXFS_SHIFT)                                /* 0x00000001 */
#define FSPI_FSR_TXES_SHIFT                                (1U)
#define FSPI_FSR_TXES_MASK                                 (0x1U << FSPI_FSR_TXES_SHIFT)                                /* 0x00000002 */
#define FSPI_FSR_RXES_SHIFT                                (2U)
#define FSPI_FSR_RXES_MASK                                 (0x1U << FSPI_FSR_RXES_SHIFT)                                /* 0x00000004 */
#define FSPI_FSR_RXFS_SHIFT                                (3U)
#define FSPI_FSR_RXFS_MASK                                 (0x1U << FSPI_FSR_RXFS_SHIFT)                                /* 0x00000008 */
#define FSPI_FSR_TXWLVL_SHIFT                              (8U)
#define FSPI_FSR_TXWLVL_MASK                               (0x7FU << FSPI_FSR_TXWLVL_SHIFT)                             /* 0x00007F00 */
#define FSPI_FSR_RXWLVL_SHIFT                              (16U)
#define FSPI_FSR_RXWLVL_MASK                               (0x1FU << FSPI_FSR_RXWLVL_SHIFT)                             /* 0x001F0000 */
/* SR */
#define FSPI_SR_OFFSET                                     (0x24U)
#define FSPI_SR                                            (0x0U)
#define FSPI_SR_SR_SHIFT                                   (0U)
#define FSPI_SR_SR_MASK                                    (0x1U << FSPI_SR_SR_SHIFT)                                   /* 0x00000001 */
/* RISR */
#define FSPI_RISR_OFFSET                                   (0x28U)
#define FSPI_RISR_RXFS_SHIFT                               (0U)
#define FSPI_RISR_RXFS_MASK                                (0x1U << FSPI_RISR_RXFS_SHIFT)                               /* 0x00000001 */
#define FSPI_RISR_RXUS_SHIFT                               (1U)
#define FSPI_RISR_RXUS_MASK                                (0x1U << FSPI_RISR_RXUS_SHIFT)                               /* 0x00000002 */
#define FSPI_RISR_TXOS_SHIFT                               (2U)
#define FSPI_RISR_TXOS_MASK                                (0x1U << FSPI_RISR_TXOS_SHIFT)                               /* 0x00000004 */
#define FSPI_RISR_TXES_SHIFT                               (3U)
#define FSPI_RISR_TXES_MASK                                (0x1U << FSPI_RISR_TXES_SHIFT)                               /* 0x00000008 */
#define FSPI_RISR_TRANSS_SHIFT                             (4U)
#define FSPI_RISR_TRANSS_MASK                              (0x1U << FSPI_RISR_TRANSS_SHIFT)                             /* 0x00000010 */
#define FSPI_RISR_AHBS_SHIFT                               (5U)
#define FSPI_RISR_AHBS_MASK                                (0x1U << FSPI_RISR_AHBS_SHIFT)                               /* 0x00000020 */
#define FSPI_RISR_NSPIS_SHIFT                              (6U)
#define FSPI_RISR_NSPIS_MASK                               (0x1U << FSPI_RISR_NSPIS_SHIFT)                              /* 0x00000040 */
#define FSPI_RISR_DMAS_SHIFT                               (7U)
#define FSPI_RISR_DMAS_MASK                                (0x1U << FSPI_RISR_DMAS_SHIFT)                               /* 0x00000080 */
#define FSPI_RISR_ILLGL_WRS_SHIFT                          (9U)
#define FSPI_RISR_ILLGL_WRS_MASK                           (0x1U << FSPI_RISR_ILLGL_WRS_SHIFT)                          /* 0x00000200 */
/* VER */
#define FSPI_VER_OFFSET                                    (0x2CU)
#define FSPI_VER_VER_SHIFT                                 (0U)
#define FSPI_VER_VER_MASK                                  (0xFFFFU << FSPI_VER_VER_SHIFT)                              /* 0x0000FFFF */
#define FSPI_VER_RSVD_SHIFT                                (16U)
#define FSPI_VER_RSVD_MASK                                 (0x1U << FSPI_VER_RSVD_SHIFT)                                /* 0x00010000 */
#define FSPI_VER_X4_CAP_SHIFT                              (17U)
#define FSPI_VER_X4_CAP_MASK                               (0x1U << FSPI_VER_X4_CAP_SHIFT)                              /* 0x00020000 */
#define FSPI_VER_X8_CAP_SHIFT                              (18U)
#define FSPI_VER_X8_CAP_MASK                               (0x1U << FSPI_VER_X8_CAP_SHIFT)                              /* 0x00040000 */
#define FSPI_VER_X16_CAP_SHIFT                             (19U)
#define FSPI_VER_X16_CAP_MASK                              (0x1U << FSPI_VER_X16_CAP_SHIFT)                             /* 0x00080000 */
#define FSPI_VER_INDRT_TX_FIFO_DEPTH_SHIFT                 (20U)
#define FSPI_VER_INDRT_TX_FIFO_DEPTH_MASK                  (0x1U << FSPI_VER_INDRT_TX_FIFO_DEPTH_SHIFT)                 /* 0x00100000 */
/* QOP */
#define FSPI_QOP_OFFSET                                    (0x30U)
#define FSPI_QOP_SO123_SHIFT                               (0U)
#define FSPI_QOP_SO123_MASK                                (0x1U << FSPI_QOP_SO123_SHIFT)                               /* 0x00000001 */
#define FSPI_QOP_SO123BP_SHIFT                             (1U)
#define FSPI_QOP_SO123BP_MASK                              (0x1U << FSPI_QOP_SO123BP_SHIFT)                             /* 0x00000002 */
/* EXT_CTRL */
#define FSPI_EXT_CTRL_OFFSET                               (0x34U)
#define FSPI_EXT_CTRL_CS_DESEL_CTRL_SHIFT                  (0U)
#define FSPI_EXT_CTRL_CS_DESEL_CTRL_MASK                   (0xFU << FSPI_EXT_CTRL_CS_DESEL_CTRL_SHIFT)                  /* 0x0000000F */
#define FSPI_EXT_CTRL_SWITCH_IO_DUMM_CNT_SHIFT             (4U)
#define FSPI_EXT_CTRL_SWITCH_IO_DUMM_CNT_MASK              (0xFU << FSPI_EXT_CTRL_SWITCH_IO_DUMM_CNT_SHIFT)             /* 0x000000F0 */
#define FSPI_EXT_CTRL_SWITCH_IO_O2I_CNT_SHIFT              (8U)
#define FSPI_EXT_CTRL_SWITCH_IO_O2I_CNT_MASK               (0xFU << FSPI_EXT_CTRL_SWITCH_IO_O2I_CNT_SHIFT)              /* 0x00000F00 */
#define FSPI_EXT_CTRL_TRANS_INT_MODE_SHIFT                 (13U)
#define FSPI_EXT_CTRL_TRANS_INT_MODE_MASK                  (0x1U << FSPI_EXT_CTRL_TRANS_INT_MODE_SHIFT)                 /* 0x00002000 */
#define FSPI_EXT_CTRL_SR_GEN_MODE_SHIFT                    (14U)
#define FSPI_EXT_CTRL_SR_GEN_MODE_MASK                     (0x1U << FSPI_EXT_CTRL_SR_GEN_MODE_SHIFT)                    /* 0x00004000 */
#define FSPI_EXT_CTRL_CSS_EXT_SHIFT                        (15U)
#define FSPI_EXT_CTRL_CSS_EXT_MASK                         (0x1U << FSPI_EXT_CTRL_CSS_EXT_SHIFT)                        /* 0x00008000 */
#define FSPI_EXT_CTRL_CSH_EXT_SHIFT                        (16U)
#define FSPI_EXT_CTRL_CSH_EXT_MASK                         (0x3U << FSPI_EXT_CTRL_CSH_EXT_SHIFT)                        /* 0x00030000 */
#define FSPI_EXT_CTRL_CSH_CLK_EN_SHIFT                     (19U)
#define FSPI_EXT_CTRL_CSH_CLK_EN_MASK                      (0x1U << FSPI_EXT_CTRL_CSH_CLK_EN_SHIFT)                     /* 0x00080000 */
#define FSPI_EXT_CTRL_DQS_SMP_MODE_SHIFT                   (20U)
#define FSPI_EXT_CTRL_DQS_SMP_MODE_MASK                    (0x3U << FSPI_EXT_CTRL_DQS_SMP_MODE_SHIFT)                   /* 0x00300000 */
/* DLL_CTRL0 */
#define FSPI_DLL_CTRL0_OFFSET                              (0x3CU)
#define FSPI_DLL_CTRL0_SMP_DLL_CFG_SHIFT                   (0U)
#define FSPI_DLL_CTRL0_SMP_DLL_CFG_MASK                    (0x1FFU << FSPI_DLL_CTRL0_SMP_DLL_CFG_SHIFT)                 /* 0x000001FF */
#define FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT                  (15U)
#define FSPI_DLL_CTRL0_SCLK_SMP_SEL_MASK                   (0x1U << FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT)                  /* 0x00008000 */
#define FSPI_DLL_CTRL0_DQS1_DLL_CFG_SHIFT                  (16U)
#define FSPI_DLL_CTRL0_DQS1_DLL_CFG_MASK                   (0x1FFU << FSPI_DLL_CTRL0_DQS1_DLL_CFG_SHIFT)                /* 0x01FF0000 */
#define FSPI_DLL_CTRL0_DQS1_SMP_SEL_SHIFT                  (31U)
#define FSPI_DLL_CTRL0_DQS1_SMP_SEL_MASK                   (0x1U << FSPI_DLL_CTRL0_DQS1_SMP_SEL_SHIFT)                  /* 0x80000000 */
/* HRDYMASK */
#define FSPI_HRDYMASK_OFFSET                               (0x40U)
#define FSPI_HRDYMASK_XIP_HUP_REQ_SHIFT                    (16U)
#define FSPI_HRDYMASK_XIP_HUP_REQ_MASK                     (0x1U << FSPI_HRDYMASK_XIP_HUP_REQ_SHIFT)                    /* 0x00010000 */
#define FSPI_HRDYMASK_XIP_HUP_RLS_REQ_SHIFT                (17U)
#define FSPI_HRDYMASK_XIP_HUP_RLS_REQ_MASK                 (0x1U << FSPI_HRDYMASK_XIP_HUP_RLS_REQ_SHIFT)                /* 0x00020000 */
#define FSPI_HRDYMASK_XIP_HUP_STATE_SHIFT                  (18U)
#define FSPI_HRDYMASK_XIP_HUP_STATE_MASK                   (0x1U << FSPI_HRDYMASK_XIP_HUP_STATE_SHIFT)                  /* 0x00040000 */
/* EXT_AX */
#define FSPI_EXT_AX_OFFSET                                 (0x44U)
#define FSPI_EXT_AX_AX_CANCEL_PAT_SHIFT                    (0U)
#define FSPI_EXT_AX_AX_CANCEL_PAT_MASK                     (0xFFU << FSPI_EXT_AX_AX_CANCEL_PAT_SHIFT)                   /* 0x000000FF */
#define FSPI_EXT_AX_AX_SETUP_PAT_SHIFT                     (8U)
#define FSPI_EXT_AX_AX_SETUP_PAT_MASK                      (0xFFU << FSPI_EXT_AX_AX_SETUP_PAT_SHIFT)                    /* 0x0000FF00 */
/* SCLK_INATM_CNT */
#define FSPI_SCLK_INATM_CNT_OFFSET                         (0x48U)
#define FSPI_SCLK_INATM_CNT_SCLK_INATM_CNT_SHIFT           (0U)
#define FSPI_SCLK_INATM_CNT_SCLK_INATM_CNT_MASK            (0xFFFFFFFFU << FSPI_SCLK_INATM_CNT_SCLK_INATM_CNT_SHIFT)    /* 0xFFFFFFFF */
/* XMMC_WCMD0 */
#define FSPI_XMMC_WCMD0_OFFSET                             (0x50U)
#define FSPI_XMMC_WCMD0_CMD_SHIFT                          (0U)
#define FSPI_XMMC_WCMD0_CMD_MASK                           (0xFFU << FSPI_XMMC_WCMD0_CMD_SHIFT)                         /* 0x000000FF */
#define FSPI_XMMC_WCMD0_DUMM_SHIFT                         (8U)
#define FSPI_XMMC_WCMD0_DUMM_MASK                          (0xFU << FSPI_XMMC_WCMD0_DUMM_SHIFT)                         /* 0x00000F00 */
#define FSPI_XMMC_WCMD0_CONT_SHIFT                         (13U)
#define FSPI_XMMC_WCMD0_CONT_MASK                          (0x1U << FSPI_XMMC_WCMD0_CONT_SHIFT)                         /* 0x00002000 */
#define FSPI_XMMC_WCMD0_ADDRB_SHIFT                        (14U)
#define FSPI_XMMC_WCMD0_ADDRB_MASK                         (0x3U << FSPI_XMMC_WCMD0_ADDRB_SHIFT)                        /* 0x0000C000 */
#define FSPI_XMMC_WCMD0_CMDF_EXT_SHIFT                     (16U)
#define FSPI_XMMC_WCMD0_CMDF_EXT_MASK                      (0xFFFFU << FSPI_XMMC_WCMD0_CMDF_EXT_SHIFT)                  /* 0xFFFF0000 */
/* XMMC_RCMD0 */
#define FSPI_XMMC_RCMD0_OFFSET                             (0x54U)
#define FSPI_XMMC_RCMD0_CMD_SHIFT                          (0U)
#define FSPI_XMMC_RCMD0_CMD_MASK                           (0xFFU << FSPI_XMMC_RCMD0_CMD_SHIFT)                         /* 0x000000FF */
#define FSPI_XMMC_RCMD0_DUMM_SHIFT                         (8U)
#define FSPI_XMMC_RCMD0_DUMM_MASK                          (0xFU << FSPI_XMMC_RCMD0_DUMM_SHIFT)                         /* 0x00000F00 */
#define FSPI_XMMC_RCMD0_CONT_SHIFT                         (13U)
#define FSPI_XMMC_RCMD0_CONT_MASK                          (0x1U << FSPI_XMMC_RCMD0_CONT_SHIFT)                         /* 0x00002000 */
#define FSPI_XMMC_RCMD0_ADDRB_SHIFT                        (14U)
#define FSPI_XMMC_RCMD0_ADDRB_MASK                         (0x3U << FSPI_XMMC_RCMD0_ADDRB_SHIFT)                        /* 0x0000C000 */
#define FSPI_XMMC_RCMD0_CMDF_EXT_SHIFT                     (16U)
#define FSPI_XMMC_RCMD0_CMDF_EXT_MASK                      (0xFFFFU << FSPI_XMMC_RCMD0_CMDF_EXT_SHIFT)                  /* 0xFFFF0000 */
/* XMMC_CTRL */
#define FSPI_XMMC_CTRL_OFFSET                              (0x58U)
#define FSPI_XMMC_CTRL_DEV_HWEN_SHIFT                      (5U)
#define FSPI_XMMC_CTRL_DEV_HWEN_MASK                       (0x1U << FSPI_XMMC_CTRL_DEV_HWEN_SHIFT)                      /* 0x00000020 */
#define FSPI_XMMC_CTRL_PFT_EN_SHIFT                        (6U)
#define FSPI_XMMC_CTRL_PFT_EN_MASK                         (0x1U << FSPI_XMMC_CTRL_PFT_EN_SHIFT)                        /* 0x00000040 */
/* MODE */
#define FSPI_MODE_OFFSET                                   (0x5CU)
#define FSPI_MODE_XMMC_MODE_EN_SHIFT                       (0U)
#define FSPI_MODE_XMMC_MODE_EN_MASK                        (0x1U << FSPI_MODE_XMMC_MODE_EN_SHIFT)                       /* 0x00000001 */
/* DEVRGN */
#define FSPI_DEVRGN_OFFSET                                 (0x60U)
#define FSPI_DEVRGN_RSIZE_SHIFT                            (0U)
#define FSPI_DEVRGN_RSIZE_MASK                             (0x1FU << FSPI_DEVRGN_RSIZE_SHIFT)                           /* 0x0000001F */
#define FSPI_DEVRGN_DEC_CTRL_SHIFT                         (8U)
#define FSPI_DEVRGN_DEC_CTRL_MASK                          (0x3U << FSPI_DEVRGN_DEC_CTRL_SHIFT)                         /* 0x00000300 */
/* DEVSIZE0 */
#define FSPI_DEVSIZE0_OFFSET                               (0x64U)
#define FSPI_DEVSIZE0_DSIZE_SHIFT                          (0U)
#define FSPI_DEVSIZE0_DSIZE_MASK                           (0x1FU << FSPI_DEVSIZE0_DSIZE_SHIFT)                         /* 0x0000001F */
/* TME0 */
#define FSPI_TME0_OFFSET                                   (0x68U)
#define FSPI_TME0_SCLK_INATM_EN_SHIFT                      (1U)
#define FSPI_TME0_SCLK_INATM_EN_MASK                       (0x1U << FSPI_TME0_SCLK_INATM_EN_SHIFT)                      /* 0x00000002 */
/* RX_FULL_WTMRK */
#define FSPI_RX_FULL_WTMRK_OFFSET                          (0x70U)
#define FSPI_RX_FULL_WTMRK_XIP_RX_FULL_WTMRK_SHIFT         (0U)
#define FSPI_RX_FULL_WTMRK_XIP_RX_FULL_WTMRK_MASK          (0xFFU << FSPI_RX_FULL_WTMRK_XIP_RX_FULL_WTMRK_SHIFT)        /* 0x000000FF */
#define FSPI_RX_FULL_WTMRK_INDRT_RX_FULL_WTMRK_SHIFT       (16U)
#define FSPI_RX_FULL_WTMRK_INDRT_RX_FULL_WTMRK_MASK        (0xFFU << FSPI_RX_FULL_WTMRK_INDRT_RX_FULL_WTMRK_SHIFT)      /* 0x00FF0000 */
/* DUMM_CTRL */
#define FSPI_DUMM_CTRL_OFFSET                              (0x74U)
#define FSPI_DUMM_CTRL_DUMM_SEL_SHIFT                      (0U)
#define FSPI_DUMM_CTRL_DUMM_SEL_MASK                       (0x1U << FSPI_DUMM_CTRL_DUMM_SEL_SHIFT)                      /* 0x00000001 */
#define FSPI_DUMM_CTRL_DUMM_EXT_SHIFT                      (1U)
#define FSPI_DUMM_CTRL_DUMM_EXT_MASK                       (0x7FU << FSPI_DUMM_CTRL_DUMM_EXT_SHIFT)                     /* 0x000000FE */
/* CMD_EXT */
#define FSPI_CMD_EXT_OFFSET                                (0x78U)
#define FSPI_CMD_EXT_CMDF_EXT_SHIFT                        (0U)
#define FSPI_CMD_EXT_CMDF_EXT_MASK                         (0xFFFFU << FSPI_CMD_EXT_CMDF_EXT_SHIFT)                     /* 0x0000FFFF */
/* TRC_CTRL */
#define FSPI_TRC_CTRL_OFFSET                               (0x7CU)
#define FSPI_TRC_CTRL_TRC_EN_SHIFT                         (0U)
#define FSPI_TRC_CTRL_TRC_EN_MASK                          (0x1U << FSPI_TRC_CTRL_TRC_EN_SHIFT)                         /* 0x00000001 */
#define FSPI_TRC_CTRL_TRC_CYCLE_SHIFT                      (1U)
#define FSPI_TRC_CTRL_TRC_CYCLE_MASK                       (0xFFFFU << FSPI_TRC_CTRL_TRC_CYCLE_SHIFT)                   /* 0x0001FFFE */
/* DMATR */
#define FSPI_DMATR_OFFSET                                  (0x80U)
#define FSPI_DMATR_DMATR_SHIFT                             (0U)
#define FSPI_DMATR_DMATR_MASK                              (0x1U << FSPI_DMATR_DMATR_SHIFT)                             /* 0x00000001 */
/* DMAADDR */
#define FSPI_DMAADDR_OFFSET                                (0x84U)
#define FSPI_DMAADDR_DMAADDR_SHIFT                         (0U)
#define FSPI_DMAADDR_DMAADDR_MASK                          (0xFFFFFFFFU << FSPI_DMAADDR_DMAADDR_SHIFT)                  /* 0xFFFFFFFF */
/* LEN_CTRL */
#define FSPI_LEN_CTRL_OFFSET                               (0x88U)
#define FSPI_LEN_CTRL_TRB_SEL_SHIFT                        (0U)
#define FSPI_LEN_CTRL_TRB_SEL_MASK                         (0x1U << FSPI_LEN_CTRL_TRB_SEL_SHIFT)                        /* 0x00000001 */
/* LEN_EXT */
#define FSPI_LEN_EXT_OFFSET                                (0x8CU)
#define FSPI_LEN_EXT_TRB_EXT_SHIFT                         (0U)
#define FSPI_LEN_EXT_TRB_EXT_MASK                          (0xFFFFFFFFU << FSPI_LEN_EXT_TRB_EXT_SHIFT)                  /* 0xFFFFFFFF */
/* XMMCSR */
#define FSPI_XMMCSR_OFFSET                                 (0x94U)
#define FSPI_XMMCSR_SLOPOVER0_SHIFT                        (0U)
#define FSPI_XMMCSR_SLOPOVER0_MASK                         (0x1U << FSPI_XMMCSR_SLOPOVER0_SHIFT)                        /* 0x00000001 */
#define FSPI_XMMCSR_SLOPOVER1_SHIFT                        (1U)
#define FSPI_XMMCSR_SLOPOVER1_MASK                         (0x1U << FSPI_XMMCSR_SLOPOVER1_SHIFT)                        /* 0x00000002 */
/* HYPER_RSVD_ADDR */
#define FSPI_HYPER_RSVD_ADDR_OFFSET                        (0x98U)
#define FSPI_HYPER_RSVD_ADDR_HYPER_RSVD_ADDR_SHIFT         (0U)
#define FSPI_HYPER_RSVD_ADDR_HYPER_RSVD_ADDR_MASK          (0x1FFFU << FSPI_HYPER_RSVD_ADDR_HYPER_RSVD_ADDR_SHIFT)      /* 0x00001FFF */
/* VDMC0 */
#define FSPI_VDMC0_OFFSET                                  (0x9CU)
#define FSPI_VDMC0_MID_SHIFT                               (0U)
#define FSPI_VDMC0_MID_MASK                                (0xFU << FSPI_VDMC0_MID_SHIFT)                               /* 0x0000000F */
/* DBG_IO_CTRL */
#define FSPI_DBG_IO_CTRL_OFFSET                            (0xA0U)
#define FSPI_DBG_IO_CTRL_DBG_OUT0_MUX_SHIFT                (0U)
#define FSPI_DBG_IO_CTRL_DBG_OUT0_MUX_MASK                 (0x3U << FSPI_DBG_IO_CTRL_DBG_OUT0_MUX_SHIFT)                /* 0x00000003 */
#define FSPI_DBG_IO_CTRL_DBG_OUT1_MUX_SHIFT                (2U)
#define FSPI_DBG_IO_CTRL_DBG_OUT1_MUX_MASK                 (0x3U << FSPI_DBG_IO_CTRL_DBG_OUT1_MUX_SHIFT)                /* 0x0000000C */
#define FSPI_DBG_IO_CTRL_DLL_DBG_EN_SHIFT                  (16U)
#define FSPI_DBG_IO_CTRL_DLL_DBG_EN_MASK                   (0x1U << FSPI_DBG_IO_CTRL_DLL_DBG_EN_SHIFT)                  /* 0x00010000 */
/* DEV_RSTN */
#define FSPI_DEV_RSTN_OFFSET                               (0xA4U)
#define FSPI_DEV_RSTN_DEV_RSTN_SHIFT                       (0U)
#define FSPI_DEV_RSTN_DEV_RSTN_MASK                        (0xFU << FSPI_DEV_RSTN_DEV_RSTN_SHIFT)                       /* 0x0000000F */
/* SLF_DQS_CTRL */
#define FSPI_SLF_DQS_CTRL_OFFSET                           (0xA8U)
#define FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN0_SHIFT            (0U)
#define FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN0_MASK             (0x1U << FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN0_SHIFT)            /* 0x00000001 */
#define FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN1_SHIFT            (16U)
#define FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN1_MASK             (0x1U << FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN1_SHIFT)            /* 0x00010000 */
/* TRNS_TR_CTRL */
#define FSPI_TRNS_TR_CTRL_OFFSET                           (0xACU)
#define FSPI_TRNS_TR_CTRL_TRNS_TR_MODE_SHIFT               (0U)
#define FSPI_TRNS_TR_CTRL_TRNS_TR_MODE_MASK                (0x1U << FSPI_TRNS_TR_CTRL_TRNS_TR_MODE_SHIFT)               /* 0x00000001 */
#define FSPI_TRNS_TR_CTRL_TX_FIFO_DNUM_SHIFT               (8U)
#define FSPI_TRNS_TR_CTRL_TX_FIFO_DNUM_MASK                (0x7FU << FSPI_TRNS_TR_CTRL_TX_FIFO_DNUM_SHIFT)              /* 0x00007F00 */
/* TRNS_TR */
#define FSPI_TRNS_TR_OFFSET                                (0xB0U)
#define FSPI_TRNS_TR_TRNS_TR_SHIFT                         (0U)
#define FSPI_TRNS_TR_TRNS_TR_MASK                          (0x1U << FSPI_TRNS_TR_TRNS_TR_SHIFT)                         /* 0x00000001 */
/* DMA_CHAIN_TR */
#define FSPI_DMA_CHAIN_TR_OFFSET                           (0xB4U)
#define FSPI_DMA_CHAIN_TR_DMA_CHAIN_TR_SHIFT               (0U)
#define FSPI_DMA_CHAIN_TR_DMA_CHAIN_TR_MASK                (0x1U << FSPI_DMA_CHAIN_TR_DMA_CHAIN_TR_SHIFT)               /* 0x00000001 */
/* DMA_HEAD_ADDR */
#define FSPI_DMA_HEAD_ADDR_OFFSET                          (0xB8U)
#define FSPI_DMA_HEAD_ADDR_DMA_HEAD_ADDR_SHIFT             (0U)
#define FSPI_DMA_HEAD_ADDR_DMA_HEAD_ADDR_MASK              (0xFFFFFFFFU << FSPI_DMA_HEAD_ADDR_DMA_HEAD_ADDR_SHIFT)      /* 0xFFFFFFFF */
/* DMA_CTRL */
#define FSPI_DMA_CTRL_OFFSET                               (0xBCU)
#define FSPI_DMA_CTRL_DMA_BURST_TYPE_SHIFT                 (0U)
#define FSPI_DMA_CTRL_DMA_BURST_TYPE_MASK                  (0x3U << FSPI_DMA_CTRL_DMA_BURST_TYPE_SHIFT)                 /* 0x00000003 */
/* CMD */
#define FSPI_CMD_OFFSET                                    (0x100U)
#define FSPI_CMD_CMD_SHIFT                                 (0U)
#define FSPI_CMD_CMD_MASK                                  (0xFFU << FSPI_CMD_CMD_SHIFT)                                /* 0x000000FF */
#define FSPI_CMD_DUMM_SHIFT                                (8U)
#define FSPI_CMD_DUMM_MASK                                 (0xFU << FSPI_CMD_DUMM_SHIFT)                                /* 0x00000F00 */
#define FSPI_CMD_WR_SHIFT                                  (12U)
#define FSPI_CMD_WR_MASK                                   (0x1U << FSPI_CMD_WR_SHIFT)                                  /* 0x00001000 */
#define FSPI_CMD_CONT_SHIFT                                (13U)
#define FSPI_CMD_CONT_MASK                                 (0x1U << FSPI_CMD_CONT_SHIFT)                                /* 0x00002000 */
#define FSPI_CMD_ADDRB_SHIFT                               (14U)
#define FSPI_CMD_ADDRB_MASK                                (0x3U << FSPI_CMD_ADDRB_SHIFT)                               /* 0x0000C000 */
#define FSPI_CMD_TRB_SHIFT                                 (16U)
#define FSPI_CMD_TRB_MASK                                  (0x3FFFU << FSPI_CMD_TRB_SHIFT)                              /* 0x3FFF0000 */
#define FSPI_CMD_CS_SHIFT                                  (30U)
#define FSPI_CMD_CS_MASK                                   (0x3U << FSPI_CMD_CS_SHIFT)                                  /* 0xC0000000 */
/* ADDR */
#define FSPI_ADDR_OFFSET                                   (0x104U)
#define FSPI_ADDR_ADDR_SHIFT                               (0U)
#define FSPI_ADDR_ADDR_MASK                                (0xFFFFFFFFU << FSPI_ADDR_ADDR_SHIFT)                        /* 0xFFFFFFFF */
/* DATA */
#define FSPI_DATA_OFFSET                                   (0x108U)
#define FSPI_DATA_DATA_SHIFT                               (0U)
#define FSPI_DATA_DATA_MASK                                (0xFFFFFFFFU << FSPI_DATA_DATA_SHIFT)                        /* 0xFFFFFFFF */
/* CTRL1 */
#define FSPI_CTRL1_OFFSET                                  (0x200U)
#define FSPI_CTRL1_SPIM_SHIFT                              (0U)
#define FSPI_CTRL1_SPIM_MASK                               (0x1U << FSPI_CTRL1_SPIM_SHIFT)                              /* 0x00000001 */
#define FSPI_CTRL1_SHIFTPHASE_SHIFT                        (1U)
#define FSPI_CTRL1_SHIFTPHASE_MASK                         (0x1U << FSPI_CTRL1_SHIFTPHASE_SHIFT)                        /* 0x00000002 */
#define FSPI_CTRL1_DTR_MODE_SHIFT                          (2U)
#define FSPI_CTRL1_DTR_MODE_MASK                           (0x1U << FSPI_CTRL1_DTR_MODE_SHIFT)                          /* 0x00000004 */
#define FSPI_CTRL1_DAT_ORDER_SHIFT                         (3U)
#define FSPI_CTRL1_DAT_ORDER_MASK                          (0x1U << FSPI_CTRL1_DAT_ORDER_SHIFT)                         /* 0x00000008 */
#define FSPI_CTRL1_IDLE_CYCLE_SHIFT                        (4U)
#define FSPI_CTRL1_IDLE_CYCLE_MASK                         (0xFU << FSPI_CTRL1_IDLE_CYCLE_SHIFT)                        /* 0x000000F0 */
#define FSPI_CTRL1_CMDB_SHIFT                              (8U)
#define FSPI_CTRL1_CMDB_MASK                               (0x3U << FSPI_CTRL1_CMDB_SHIFT)                              /* 0x00000300 */
#define FSPI_CTRL1_ADRB_SHIFT                              (10U)
#define FSPI_CTRL1_ADRB_MASK                               (0x3U << FSPI_CTRL1_ADRB_SHIFT)                              /* 0x00000C00 */
#define FSPI_CTRL1_DATB_SHIFT                              (12U)
#define FSPI_CTRL1_DATB_MASK                               (0x3U << FSPI_CTRL1_DATB_SHIFT)                              /* 0x00003000 */
#define FSPI_CTRL1_CMDB16_SHIFT                            (14U)
#define FSPI_CTRL1_CMDB16_MASK                             (0x1U << FSPI_CTRL1_CMDB16_SHIFT)                            /* 0x00004000 */
#define FSPI_CTRL1_ADRB16_SHIFT                            (15U)
#define FSPI_CTRL1_ADRB16_MASK                             (0x1U << FSPI_CTRL1_ADRB16_SHIFT)                            /* 0x00008000 */
#define FSPI_CTRL1_DATAB16_SHIFT                           (16U)
#define FSPI_CTRL1_DATAB16_MASK                            (0x1U << FSPI_CTRL1_DATAB16_SHIFT)                           /* 0x00010000 */
#define FSPI_CTRL1_DQS_MODE_SHIFT                          (17U)
#define FSPI_CTRL1_DQS_MODE_MASK                           (0x1U << FSPI_CTRL1_DQS_MODE_SHIFT)                          /* 0x00020000 */
#define FSPI_CTRL1_DTR_RDC_SEL_SHIFT                       (18U)
#define FSPI_CTRL1_DTR_RDC_SEL_MASK                        (0x1U << FSPI_CTRL1_DTR_RDC_SEL_SHIFT)                       /* 0x00040000 */
#define FSPI_CTRL1_CMD_STR_SHIFT                           (19U)
#define FSPI_CTRL1_CMD_STR_MASK                            (0x1U << FSPI_CTRL1_CMD_STR_SHIFT)                           /* 0x00080000 */
#define FSPI_CTRL1_ADDR_STR_SHIFT                          (20U)
#define FSPI_CTRL1_ADDR_STR_MASK                           (0x1U << FSPI_CTRL1_ADDR_STR_SHIFT)                          /* 0x00100000 */
#define FSPI_CTRL1_DBL_DUMM_CTRL_SHIFT                     (21U)
#define FSPI_CTRL1_DBL_DUMM_CTRL_MASK                      (0x1U << FSPI_CTRL1_DBL_DUMM_CTRL_SHIFT)                     /* 0x00200000 */
#define FSPI_CTRL1_DUMM_OVLP_SHIFT                         (22U)
#define FSPI_CTRL1_DUMM_OVLP_MASK                          (0x3U << FSPI_CTRL1_DUMM_OVLP_SHIFT)                         /* 0x00C00000 */
#define FSPI_CTRL1_INTRNL_CLK_MODE_SHIFT                   (24U)
#define FSPI_CTRL1_INTRNL_CLK_MODE_MASK                    (0x1U << FSPI_CTRL1_INTRNL_CLK_MODE_SHIFT)                   /* 0x01000000 */
#define FSPI_CTRL1_HYPER_ADDR_EN_SHIFT                     (25U)
#define FSPI_CTRL1_HYPER_ADDR_EN_MASK                      (0x1U << FSPI_CTRL1_HYPER_ADDR_EN_SHIFT)                     /* 0x02000000 */
#define FSPI_CTRL1_HYPER_RSVD_EN_SHIFT                     (26U)
#define FSPI_CTRL1_HYPER_RSVD_EN_MASK                      (0x1U << FSPI_CTRL1_HYPER_RSVD_EN_SHIFT)                     /* 0x04000000 */
#define FSPI_CTRL1_CMD_CTRL_SHIFT                          (27U)
#define FSPI_CTRL1_CMD_CTRL_MASK                           (0x3U << FSPI_CTRL1_CMD_CTRL_SHIFT)                          /* 0x18000000 */
#define FSPI_CTRL1_WP_EN_SHIFT                             (29U)
#define FSPI_CTRL1_WP_EN_MASK                              (0x1U << FSPI_CTRL1_WP_EN_SHIFT)                             /* 0x20000000 */
/* AX1 */
#define FSPI_AX1_OFFSET                                    (0x214U)
#define FSPI_AX1_AX_SHIFT                                  (0U)
#define FSPI_AX1_AX_MASK                                   (0xFFU << FSPI_AX1_AX_SHIFT)                                 /* 0x000000FF */
/* ABIT1 */
#define FSPI_ABIT1_OFFSET                                  (0x218U)
#define FSPI_ABIT1_ABIT_SHIFT                              (0U)
#define FSPI_ABIT1_ABIT_MASK                               (0x3FU << FSPI_ABIT1_ABIT_SHIFT)                             /* 0x0000003F */
/* DLL_CTRL1 */
#define FSPI_DLL_CTRL1_OFFSET                              (0x23CU)
#define FSPI_DLL_CTRL1_SMP_DLL_CFG_SHIFT                   (0U)
#define FSPI_DLL_CTRL1_SMP_DLL_CFG_MASK                    (0x1FFU << FSPI_DLL_CTRL1_SMP_DLL_CFG_SHIFT)                 /* 0x000001FF */
#define FSPI_DLL_CTRL1_SCLK_SMP_SEL_SHIFT                  (15U)
#define FSPI_DLL_CTRL1_SCLK_SMP_SEL_MASK                   (0x1U << FSPI_DLL_CTRL1_SCLK_SMP_SEL_SHIFT)                  /* 0x00008000 */
#define FSPI_DLL_CTRL1_DQS1_DLL_CFG_SHIFT                  (16U)
#define FSPI_DLL_CTRL1_DQS1_DLL_CFG_MASK                   (0x1FFU << FSPI_DLL_CTRL1_DQS1_DLL_CFG_SHIFT)                /* 0x01FF0000 */
#define FSPI_DLL_CTRL1_DQS1_SMP_SEL_SHIFT                  (31U)
#define FSPI_DLL_CTRL1_DQS1_SMP_SEL_MASK                   (0x1U << FSPI_DLL_CTRL1_DQS1_SMP_SEL_SHIFT)                  /* 0x80000000 */
/* XMMC_WCMD1 */
#define FSPI_XMMC_WCMD1_OFFSET                             (0x250U)
#define FSPI_XMMC_WCMD1_CMD_SHIFT                          (0U)
#define FSPI_XMMC_WCMD1_CMD_MASK                           (0xFFU << FSPI_XMMC_WCMD1_CMD_SHIFT)                         /* 0x000000FF */
#define FSPI_XMMC_WCMD1_DUMM_SHIFT                         (8U)
#define FSPI_XMMC_WCMD1_DUMM_MASK                          (0xFU << FSPI_XMMC_WCMD1_DUMM_SHIFT)                         /* 0x00000F00 */
#define FSPI_XMMC_WCMD1_CONT_SHIFT                         (13U)
#define FSPI_XMMC_WCMD1_CONT_MASK                          (0x1U << FSPI_XMMC_WCMD1_CONT_SHIFT)                         /* 0x00002000 */
#define FSPI_XMMC_WCMD1_ADDRB_SHIFT                        (14U)
#define FSPI_XMMC_WCMD1_ADDRB_MASK                         (0x3U << FSPI_XMMC_WCMD1_ADDRB_SHIFT)                        /* 0x0000C000 */
#define FSPI_XMMC_WCMD1_CMDF_EXT_SHIFT                     (16U)
#define FSPI_XMMC_WCMD1_CMDF_EXT_MASK                      (0xFFFFU << FSPI_XMMC_WCMD1_CMDF_EXT_SHIFT)                  /* 0xFFFF0000 */
/* XMMC_RCMD1 */
#define FSPI_XMMC_RCMD1_OFFSET                             (0x254U)
#define FSPI_XMMC_RCMD1_CMD_SHIFT                          (0U)
#define FSPI_XMMC_RCMD1_CMD_MASK                           (0xFFU << FSPI_XMMC_RCMD1_CMD_SHIFT)                         /* 0x000000FF */
#define FSPI_XMMC_RCMD1_DUMM_SHIFT                         (8U)
#define FSPI_XMMC_RCMD1_DUMM_MASK                          (0xFU << FSPI_XMMC_RCMD1_DUMM_SHIFT)                         /* 0x00000F00 */
#define FSPI_XMMC_RCMD1_CONT_SHIFT                         (13U)
#define FSPI_XMMC_RCMD1_CONT_MASK                          (0x1U << FSPI_XMMC_RCMD1_CONT_SHIFT)                         /* 0x00002000 */
#define FSPI_XMMC_RCMD1_ADDRB_SHIFT                        (14U)
#define FSPI_XMMC_RCMD1_ADDRB_MASK                         (0x3U << FSPI_XMMC_RCMD1_ADDRB_SHIFT)                        /* 0x0000C000 */
#define FSPI_XMMC_RCMD1_CMDF_EXT_SHIFT                     (16U)
#define FSPI_XMMC_RCMD1_CMDF_EXT_MASK                      (0xFFFFU << FSPI_XMMC_RCMD1_CMDF_EXT_SHIFT)                  /* 0xFFFF0000 */
/* DEVSIZE1 */
#define FSPI_DEVSIZE1_OFFSET                               (0x264U)
#define FSPI_DEVSIZE1_DSIZE_SHIFT                          (0U)
#define FSPI_DEVSIZE1_DSIZE_MASK                           (0x1FU << FSPI_DEVSIZE1_DSIZE_SHIFT)                         /* 0x0000001F */
/* TME1 */
#define FSPI_TME1_OFFSET                                   (0x268U)
#define FSPI_TME1_SCLK_INATM_EN_SHIFT                      (1U)
#define FSPI_TME1_SCLK_INATM_EN_MASK                       (0x1U << FSPI_TME1_SCLK_INATM_EN_SHIFT)                      /* 0x00000002 */
/* VDMC1 */
#define FSPI_VDMC1_OFFSET                                  (0x29CU)
#define FSPI_VDMC1_MID_SHIFT                               (0U)
#define FSPI_VDMC1_MID_MASK                                (0xFU << FSPI_VDMC1_MID_SHIFT)                               /* 0x0000000F */
/*****************************************ICACHE*****************************************/
/* CACHE_CTRL */
#define ICACHE_CACHE_CTRL_OFFSET                           (0x0U)
#define ICACHE_CACHE_CTRL_CACHE_EN_SHIFT                   (0U)
#define ICACHE_CACHE_CTRL_CACHE_EN_MASK                    (0x1U << ICACHE_CACHE_CTRL_CACHE_EN_SHIFT)                   /* 0x00000001 */
#define ICACHE_CACHE_CTRL_CACHE_WT_EN_SHIFT                (1U)
#define ICACHE_CACHE_CTRL_CACHE_WT_EN_MASK                 (0x1U << ICACHE_CACHE_CTRL_CACHE_WT_EN_SHIFT)                /* 0x00000002 */
#define ICACHE_CACHE_CTRL_CACHE_HB_EN_SHIFT                (2U)
#define ICACHE_CACHE_CTRL_CACHE_HB_EN_MASK                 (0x1U << ICACHE_CACHE_CTRL_CACHE_HB_EN_SHIFT)                /* 0x00000004 */
#define ICACHE_CACHE_CTRL_CACHE_STB_EN_SHIFT               (3U)
#define ICACHE_CACHE_CTRL_CACHE_STB_EN_MASK                (0x1U << ICACHE_CACHE_CTRL_CACHE_STB_EN_SHIFT)               /* 0x00000008 */
#define ICACHE_CACHE_CTRL_CACHE_FLUSH_SHIFT                (4U)
#define ICACHE_CACHE_CTRL_CACHE_FLUSH_MASK                 (0x1U << ICACHE_CACHE_CTRL_CACHE_FLUSH_SHIFT)                /* 0x00000010 */
#define ICACHE_CACHE_CTRL_CACHE_PMU_EN_SHIFT               (5U)
#define ICACHE_CACHE_CTRL_CACHE_PMU_EN_MASK                (0x1U << ICACHE_CACHE_CTRL_CACHE_PMU_EN_SHIFT)               /* 0x00000020 */
#define ICACHE_CACHE_CTRL_CACHE_BYPASS_SHIFT               (6U)
#define ICACHE_CACHE_CTRL_CACHE_BYPASS_MASK                (0x1U << ICACHE_CACHE_CTRL_CACHE_BYPASS_SHIFT)               /* 0x00000040 */
#define ICACHE_CACHE_CTRL_STB_TIMEOUT_EN_SHIFT             (7U)
#define ICACHE_CACHE_CTRL_STB_TIMEOUT_EN_MASK              (0x1U << ICACHE_CACHE_CTRL_STB_TIMEOUT_EN_SHIFT)             /* 0x00000080 */
#define ICACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_SHIFT         (8U)
#define ICACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_MASK          (0x7U << ICACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_SHIFT)         /* 0x00000700 */
#define ICACHE_CACHE_CTRL_CACHE_MPU_MODE_SHIFT             (12U)
#define ICACHE_CACHE_CTRL_CACHE_MPU_MODE_MASK              (0x1U << ICACHE_CACHE_CTRL_CACHE_MPU_MODE_SHIFT)             /* 0x00001000 */
#define ICACHE_CACHE_CTRL_CACHE_PF_EN_SHIFT                (13U)
#define ICACHE_CACHE_CTRL_CACHE_PF_EN_MASK                 (0x1U << ICACHE_CACHE_CTRL_CACHE_PF_EN_SHIFT)                /* 0x00002000 */
/* CACHE_MAINTAIN0 */
#define ICACHE_CACHE_MAINTAIN0_OFFSET                      (0x4U)
#define ICACHE_CACHE_MAINTAIN0_CACHE_M_VALID_SHIFT         (0U)
#define ICACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK          (0x1U << ICACHE_CACHE_MAINTAIN0_CACHE_M_VALID_SHIFT)         /* 0x00000001 */
#define ICACHE_CACHE_MAINTAIN0_CACHE_M_CMD_SHIFT           (1U)
#define ICACHE_CACHE_MAINTAIN0_CACHE_M_CMD_MASK            (0x3U << ICACHE_CACHE_MAINTAIN0_CACHE_M_CMD_SHIFT)           /* 0x00000006 */
#define ICACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_SHIFT          (5U)
#define ICACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_MASK           (0x7FFFFFFU << ICACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_SHIFT)    /* 0xFFFFFFE0 */
/* CACHE_MAINTAIN1 */
#define ICACHE_CACHE_MAINTAIN1_OFFSET                      (0x8U)
#define ICACHE_CACHE_MAINTAIN1_CACHE_M_OFFSET_SHIFT        (0U)
#define ICACHE_CACHE_MAINTAIN1_CACHE_M_OFFSET_MASK         (0xFFFFU << ICACHE_CACHE_MAINTAIN1_CACHE_M_OFFSET_SHIFT)     /* 0x0000FFFF */
/* STB_TIMEOUT_CTRL */
#define ICACHE_STB_TIMEOUT_CTRL_OFFSET                     (0xCU)
#define ICACHE_STB_TIMEOUT_CTRL_STB_TIMEOUT_VALUE_SHIFT    (0U)
#define ICACHE_STB_TIMEOUT_CTRL_STB_TIMEOUT_VALUE_MASK     (0x7FFFFU << ICACHE_STB_TIMEOUT_CTRL_STB_TIMEOUT_VALUE_SHIFT) /* 0x0007FFFF */
/* CACHE_INT_EN */
#define ICACHE_CACHE_INT_EN_OFFSET                         (0x20U)
#define ICACHE_CACHE_INT_EN_ERR_RECORD_EN_SHIFT            (0U)
#define ICACHE_CACHE_INT_EN_ERR_RECORD_EN_MASK             (0x1U << ICACHE_CACHE_INT_EN_ERR_RECORD_EN_SHIFT)            /* 0x00000001 */
/* CACHE_INT_ST */
#define ICACHE_CACHE_INT_ST_OFFSET                         (0x24U)
#define ICACHE_CACHE_INT_ST_AHB_ERROR_STATUS_SHIFT         (0U)
#define ICACHE_CACHE_INT_ST_AHB_ERROR_STATUS_MASK          (0x1U << ICACHE_CACHE_INT_ST_AHB_ERROR_STATUS_SHIFT)         /* 0x00000001 */
/* CACHE_ERR_HADDR */
#define ICACHE_CACHE_ERR_HADDR_OFFSET                      (0x28U)
#define ICACHE_CACHE_ERR_HADDR                             (0x0U)
#define ICACHE_CACHE_ERR_HADDR_STATUS_HADDR_SHIFT          (0U)
#define ICACHE_CACHE_ERR_HADDR_STATUS_HADDR_MASK           (0x1U << ICACHE_CACHE_ERR_HADDR_STATUS_HADDR_SHIFT)          /* 0x00000001 */
/* CACHE_STATUS */
#define ICACHE_CACHE_STATUS_OFFSET                         (0x30U)
#define ICACHE_CACHE_STATUS                                (0x0U)
#define ICACHE_CACHE_STATUS_CACHE_INIT_FINISH_SHIFT        (0U)
#define ICACHE_CACHE_STATUS_CACHE_INIT_FINISH_MASK         (0x1U << ICACHE_CACHE_STATUS_CACHE_INIT_FINISH_SHIFT)        /* 0x00000001 */
#define ICACHE_CACHE_STATUS_CACHE_M_BUSY_SHIFT             (1U)
#define ICACHE_CACHE_STATUS_CACHE_M_BUSY_MASK              (0x1U << ICACHE_CACHE_STATUS_CACHE_M_BUSY_SHIFT)             /* 0x00000002 */
#define ICACHE_CACHE_STATUS_CACHE_FLUSH_DONE_SHIFT         (2U)
#define ICACHE_CACHE_STATUS_CACHE_FLUSH_DONE_MASK          (0x1U << ICACHE_CACHE_STATUS_CACHE_FLUSH_DONE_SHIFT)         /* 0x00000004 */
/* PMU_RD_NUM_CNT */
#define ICACHE_PMU_RD_NUM_CNT_OFFSET                       (0x40U)
#define ICACHE_PMU_RD_NUM_CNT                              (0x0U)
#define ICACHE_PMU_RD_NUM_CNT_PMU_RD_NUM_CNT_SHIFT         (0U)
#define ICACHE_PMU_RD_NUM_CNT_PMU_RD_NUM_CNT_MASK          (0xFFFFFFFFU << ICACHE_PMU_RD_NUM_CNT_PMU_RD_NUM_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_WR_NUM_CNT */
#define ICACHE_PMU_WR_NUM_CNT_OFFSET                       (0x44U)
#define ICACHE_PMU_WR_NUM_CNT                              (0x0U)
#define ICACHE_PMU_WR_NUM_CNT_PMU_WR_NUM_CNT_SHIFT         (0U)
#define ICACHE_PMU_WR_NUM_CNT_PMU_WR_NUM_CNT_MASK          (0xFFFFFFFFU << ICACHE_PMU_WR_NUM_CNT_PMU_WR_NUM_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_SRAM_RD_HIT_CNT */
#define ICACHE_PMU_SRAM_RD_HIT_CNT_OFFSET                  (0x48U)
#define ICACHE_PMU_SRAM_RD_HIT_CNT                         (0x0U)
#define ICACHE_PMU_SRAM_RD_HIT_CNT_PMU_SRAM_RD_HIT_CNT_SHIFT (0U)
#define ICACHE_PMU_SRAM_RD_HIT_CNT_PMU_SRAM_RD_HIT_CNT_MASK (0xFFFFFFFFU << ICACHE_PMU_SRAM_RD_HIT_CNT_PMU_SRAM_RD_HIT_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_HB_RD_HIT_CNT */
#define ICACHE_PMU_HB_RD_HIT_CNT_OFFSET                    (0x4CU)
#define ICACHE_PMU_HB_RD_HIT_CNT                           (0x0U)
#define ICACHE_PMU_HB_RD_HIT_CNT_PMU_HB_RD_HIT_CNT_SHIFT   (0U)
#define ICACHE_PMU_HB_RD_HIT_CNT_PMU_HB_RD_HIT_CNT_MASK    (0xFFFFFFFFU << ICACHE_PMU_HB_RD_HIT_CNT_PMU_HB_RD_HIT_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_STB_RD_HIT_CNT */
#define ICACHE_PMU_STB_RD_HIT_CNT_OFFSET                   (0x50U)
#define ICACHE_PMU_STB_RD_HIT_CNT                          (0x0U)
#define ICACHE_PMU_STB_RD_HIT_CNT_PMU_STB_RD_HIT_CNT_SHIFT (0U)
#define ICACHE_PMU_STB_RD_HIT_CNT_PMU_STB_RD_HIT_CNT_MASK  (0xFFFFFFFFU << ICACHE_PMU_STB_RD_HIT_CNT_PMU_STB_RD_HIT_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_RD_HIT_CNT */
#define ICACHE_PMU_RD_HIT_CNT_OFFSET                       (0x54U)
#define ICACHE_PMU_RD_HIT_CNT                              (0x0U)
#define ICACHE_PMU_RD_HIT_CNT_PMU_RD_HIT_CNT_SHIFT         (0U)
#define ICACHE_PMU_RD_HIT_CNT_PMU_RD_HIT_CNT_MASK          (0xFFFFFFFFU << ICACHE_PMU_RD_HIT_CNT_PMU_RD_HIT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_WR_HIT_CNT */
#define ICACHE_PMU_WR_HIT_CNT_OFFSET                       (0x58U)
#define ICACHE_PMU_WR_HIT_CNT                              (0x0U)
#define ICACHE_PMU_WR_HIT_CNT_PMU_WR_HIT_CNT_SHIFT         (0U)
#define ICACHE_PMU_WR_HIT_CNT_PMU_WR_HIT_CNT_MASK          (0xFFFFFFFFU << ICACHE_PMU_WR_HIT_CNT_PMU_WR_HIT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_RD_MISS_PENALTY_CNT */
#define ICACHE_PMU_RD_MISS_PENALTY_CNT_OFFSET              (0x5CU)
#define ICACHE_PMU_RD_MISS_PENALTY_CNT                     (0x0U)
#define ICACHE_PMU_RD_MISS_PENALTY_CNT_PMU_RD_MISS_PENALTY_CNT_SHIFT (0U)
#define ICACHE_PMU_RD_MISS_PENALTY_CNT_PMU_RD_MISS_PENALTY_CNT_MASK (0xFFFFFFFFU << ICACHE_PMU_RD_MISS_PENALTY_CNT_PMU_RD_MISS_PENALTY_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_WR_MISS_PENALTY_CNT */
#define ICACHE_PMU_WR_MISS_PENALTY_CNT_OFFSET              (0x60U)
#define ICACHE_PMU_WR_MISS_PENALTY_CNT                     (0x0U)
#define ICACHE_PMU_WR_MISS_PENALTY_CNT_PMU_WR_MISS_PENALTY_CNT_SHIFT (0U)
#define ICACHE_PMU_WR_MISS_PENALTY_CNT_PMU_WR_MISS_PENALTY_CNT_MASK (0xFFFFFFFFU << ICACHE_PMU_WR_MISS_PENALTY_CNT_PMU_WR_MISS_PENALTY_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_RD_LAT_CNT */
#define ICACHE_PMU_RD_LAT_CNT_OFFSET                       (0x64U)
#define ICACHE_PMU_RD_LAT_CNT                              (0x0U)
#define ICACHE_PMU_RD_LAT_CNT_PMU_RD_LAT_CNT_SHIFT         (0U)
#define ICACHE_PMU_RD_LAT_CNT_PMU_RD_LAT_CNT_MASK          (0xFFFFFFFFU << ICACHE_PMU_RD_LAT_CNT_PMU_RD_LAT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_WR_LAT_CNT */
#define ICACHE_PMU_WR_LAT_CNT_OFFSET                       (0x68U)
#define ICACHE_PMU_WR_LAT_CNT                              (0x0U)
#define ICACHE_PMU_WR_LAT_CNT_PMU_RD_LAT_CNT_SHIFT         (0U)
#define ICACHE_PMU_WR_LAT_CNT_PMU_RD_LAT_CNT_MASK          (0xFFFFFFFFU << ICACHE_PMU_WR_LAT_CNT_PMU_RD_LAT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* REVISION */
#define ICACHE_REVISION_OFFSET                             (0xF0U)
#define ICACHE_REVISION                                    (0x100U)
#define ICACHE_REVISION_REVISION_SHIFT                     (0U)
#define ICACHE_REVISION_REVISION_MASK                      (0xFFFFFFFFU << ICACHE_REVISION_REVISION_SHIFT)              /* 0xFFFFFFFF */
/*****************************************DCACHE*****************************************/
/* CACHE_CTRL */
#define DCACHE_CACHE_CTRL_OFFSET                           (0x0U)
#define DCACHE_CACHE_CTRL_CACHE_EN_SHIFT                   (0U)
#define DCACHE_CACHE_CTRL_CACHE_EN_MASK                    (0x1U << DCACHE_CACHE_CTRL_CACHE_EN_SHIFT)                   /* 0x00000001 */
#define DCACHE_CACHE_CTRL_CACHE_WT_EN_SHIFT                (1U)
#define DCACHE_CACHE_CTRL_CACHE_WT_EN_MASK                 (0x1U << DCACHE_CACHE_CTRL_CACHE_WT_EN_SHIFT)                /* 0x00000002 */
#define DCACHE_CACHE_CTRL_CACHE_HB_EN_SHIFT                (2U)
#define DCACHE_CACHE_CTRL_CACHE_HB_EN_MASK                 (0x1U << DCACHE_CACHE_CTRL_CACHE_HB_EN_SHIFT)                /* 0x00000004 */
#define DCACHE_CACHE_CTRL_CACHE_STB_EN_SHIFT               (3U)
#define DCACHE_CACHE_CTRL_CACHE_STB_EN_MASK                (0x1U << DCACHE_CACHE_CTRL_CACHE_STB_EN_SHIFT)               /* 0x00000008 */
#define DCACHE_CACHE_CTRL_CACHE_FLUSH_SHIFT                (4U)
#define DCACHE_CACHE_CTRL_CACHE_FLUSH_MASK                 (0x1U << DCACHE_CACHE_CTRL_CACHE_FLUSH_SHIFT)                /* 0x00000010 */
#define DCACHE_CACHE_CTRL_CACHE_PMU_EN_SHIFT               (5U)
#define DCACHE_CACHE_CTRL_CACHE_PMU_EN_MASK                (0x1U << DCACHE_CACHE_CTRL_CACHE_PMU_EN_SHIFT)               /* 0x00000020 */
#define DCACHE_CACHE_CTRL_CACHE_BYPASS_SHIFT               (6U)
#define DCACHE_CACHE_CTRL_CACHE_BYPASS_MASK                (0x1U << DCACHE_CACHE_CTRL_CACHE_BYPASS_SHIFT)               /* 0x00000040 */
#define DCACHE_CACHE_CTRL_STB_TIMEOUT_EN_SHIFT             (7U)
#define DCACHE_CACHE_CTRL_STB_TIMEOUT_EN_MASK              (0x1U << DCACHE_CACHE_CTRL_STB_TIMEOUT_EN_SHIFT)             /* 0x00000080 */
#define DCACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_SHIFT         (8U)
#define DCACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_MASK          (0x7U << DCACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_SHIFT)         /* 0x00000700 */
#define DCACHE_CACHE_CTRL_CACHE_MPU_MODE_SHIFT             (12U)
#define DCACHE_CACHE_CTRL_CACHE_MPU_MODE_MASK              (0x1U << DCACHE_CACHE_CTRL_CACHE_MPU_MODE_SHIFT)             /* 0x00001000 */
#define DCACHE_CACHE_CTRL_CACHE_PF_EN_SHIFT                (13U)
#define DCACHE_CACHE_CTRL_CACHE_PF_EN_MASK                 (0x1U << DCACHE_CACHE_CTRL_CACHE_PF_EN_SHIFT)                /* 0x00002000 */
/* CACHE_MAINTAIN0 */
#define DCACHE_CACHE_MAINTAIN0_OFFSET                      (0x4U)
#define DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_SHIFT         (0U)
#define DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_MASK          (0x1U << DCACHE_CACHE_MAINTAIN0_CACHE_M_VALID_SHIFT)         /* 0x00000001 */
#define DCACHE_CACHE_MAINTAIN0_CACHE_M_CMD_SHIFT           (1U)
#define DCACHE_CACHE_MAINTAIN0_CACHE_M_CMD_MASK            (0x3U << DCACHE_CACHE_MAINTAIN0_CACHE_M_CMD_SHIFT)           /* 0x00000006 */
#define DCACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_SHIFT          (5U)
#define DCACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_MASK           (0x7FFFFFFU << DCACHE_CACHE_MAINTAIN0_CACHE_M_ADDR_SHIFT)    /* 0xFFFFFFE0 */
/* CACHE_MAINTAIN1 */
#define DCACHE_CACHE_MAINTAIN1_OFFSET                      (0x8U)
#define DCACHE_CACHE_MAINTAIN1_CACHE_M_OFFSET_SHIFT        (0U)
#define DCACHE_CACHE_MAINTAIN1_CACHE_M_OFFSET_MASK         (0xFFFFU << DCACHE_CACHE_MAINTAIN1_CACHE_M_OFFSET_SHIFT)     /* 0x0000FFFF */
/* STB_TIMEOUT_CTRL */
#define DCACHE_STB_TIMEOUT_CTRL_OFFSET                     (0xCU)
#define DCACHE_STB_TIMEOUT_CTRL_STB_TIMEOUT_VALUE_SHIFT    (0U)
#define DCACHE_STB_TIMEOUT_CTRL_STB_TIMEOUT_VALUE_MASK     (0x7FFFFU << DCACHE_STB_TIMEOUT_CTRL_STB_TIMEOUT_VALUE_SHIFT) /* 0x0007FFFF */
/* CACHE_INT_EN */
#define DCACHE_CACHE_INT_EN_OFFSET                         (0x20U)
#define DCACHE_CACHE_INT_EN_ERR_RECORD_EN_SHIFT            (0U)
#define DCACHE_CACHE_INT_EN_ERR_RECORD_EN_MASK             (0x1U << DCACHE_CACHE_INT_EN_ERR_RECORD_EN_SHIFT)            /* 0x00000001 */
/* CACHE_INT_ST */
#define DCACHE_CACHE_INT_ST_OFFSET                         (0x24U)
#define DCACHE_CACHE_INT_ST_AHB_ERROR_STATUS_SHIFT         (0U)
#define DCACHE_CACHE_INT_ST_AHB_ERROR_STATUS_MASK          (0x1U << DCACHE_CACHE_INT_ST_AHB_ERROR_STATUS_SHIFT)         /* 0x00000001 */
/* CACHE_ERR_HADDR */
#define DCACHE_CACHE_ERR_HADDR_OFFSET                      (0x28U)
#define DCACHE_CACHE_ERR_HADDR                             (0x0U)
#define DCACHE_CACHE_ERR_HADDR_STATUS_HADDR_SHIFT          (0U)
#define DCACHE_CACHE_ERR_HADDR_STATUS_HADDR_MASK           (0x1U << DCACHE_CACHE_ERR_HADDR_STATUS_HADDR_SHIFT)          /* 0x00000001 */
/* CACHE_STATUS */
#define DCACHE_CACHE_STATUS_OFFSET                         (0x30U)
#define DCACHE_CACHE_STATUS                                (0x0U)
#define DCACHE_CACHE_STATUS_CACHE_INIT_FINISH_SHIFT        (0U)
#define DCACHE_CACHE_STATUS_CACHE_INIT_FINISH_MASK         (0x1U << DCACHE_CACHE_STATUS_CACHE_INIT_FINISH_SHIFT)        /* 0x00000001 */
#define DCACHE_CACHE_STATUS_CACHE_M_BUSY_SHIFT             (1U)
#define DCACHE_CACHE_STATUS_CACHE_M_BUSY_MASK              (0x1U << DCACHE_CACHE_STATUS_CACHE_M_BUSY_SHIFT)             /* 0x00000002 */
#define DCACHE_CACHE_STATUS_CACHE_FLUSH_DONE_SHIFT         (2U)
#define DCACHE_CACHE_STATUS_CACHE_FLUSH_DONE_MASK          (0x1U << DCACHE_CACHE_STATUS_CACHE_FLUSH_DONE_SHIFT)         /* 0x00000004 */
/* PMU_RD_NUM_CNT */
#define DCACHE_PMU_RD_NUM_CNT_OFFSET                       (0x40U)
#define DCACHE_PMU_RD_NUM_CNT                              (0x0U)
#define DCACHE_PMU_RD_NUM_CNT_PMU_RD_NUM_CNT_SHIFT         (0U)
#define DCACHE_PMU_RD_NUM_CNT_PMU_RD_NUM_CNT_MASK          (0xFFFFFFFFU << DCACHE_PMU_RD_NUM_CNT_PMU_RD_NUM_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_WR_NUM_CNT */
#define DCACHE_PMU_WR_NUM_CNT_OFFSET                       (0x44U)
#define DCACHE_PMU_WR_NUM_CNT                              (0x0U)
#define DCACHE_PMU_WR_NUM_CNT_PMU_WR_NUM_CNT_SHIFT         (0U)
#define DCACHE_PMU_WR_NUM_CNT_PMU_WR_NUM_CNT_MASK          (0xFFFFFFFFU << DCACHE_PMU_WR_NUM_CNT_PMU_WR_NUM_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_SRAM_RD_HIT_CNT */
#define DCACHE_PMU_SRAM_RD_HIT_CNT_OFFSET                  (0x48U)
#define DCACHE_PMU_SRAM_RD_HIT_CNT                         (0x0U)
#define DCACHE_PMU_SRAM_RD_HIT_CNT_PMU_SRAM_RD_HIT_CNT_SHIFT (0U)
#define DCACHE_PMU_SRAM_RD_HIT_CNT_PMU_SRAM_RD_HIT_CNT_MASK (0xFFFFFFFFU << DCACHE_PMU_SRAM_RD_HIT_CNT_PMU_SRAM_RD_HIT_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_HB_RD_HIT_CNT */
#define DCACHE_PMU_HB_RD_HIT_CNT_OFFSET                    (0x4CU)
#define DCACHE_PMU_HB_RD_HIT_CNT                           (0x0U)
#define DCACHE_PMU_HB_RD_HIT_CNT_PMU_HB_RD_HIT_CNT_SHIFT   (0U)
#define DCACHE_PMU_HB_RD_HIT_CNT_PMU_HB_RD_HIT_CNT_MASK    (0xFFFFFFFFU << DCACHE_PMU_HB_RD_HIT_CNT_PMU_HB_RD_HIT_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_STB_RD_HIT_CNT */
#define DCACHE_PMU_STB_RD_HIT_CNT_OFFSET                   (0x50U)
#define DCACHE_PMU_STB_RD_HIT_CNT                          (0x0U)
#define DCACHE_PMU_STB_RD_HIT_CNT_PMU_STB_RD_HIT_CNT_SHIFT (0U)
#define DCACHE_PMU_STB_RD_HIT_CNT_PMU_STB_RD_HIT_CNT_MASK  (0xFFFFFFFFU << DCACHE_PMU_STB_RD_HIT_CNT_PMU_STB_RD_HIT_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_RD_HIT_CNT */
#define DCACHE_PMU_RD_HIT_CNT_OFFSET                       (0x54U)
#define DCACHE_PMU_RD_HIT_CNT                              (0x0U)
#define DCACHE_PMU_RD_HIT_CNT_PMU_RD_HIT_CNT_SHIFT         (0U)
#define DCACHE_PMU_RD_HIT_CNT_PMU_RD_HIT_CNT_MASK          (0xFFFFFFFFU << DCACHE_PMU_RD_HIT_CNT_PMU_RD_HIT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_WR_HIT_CNT */
#define DCACHE_PMU_WR_HIT_CNT_OFFSET                       (0x58U)
#define DCACHE_PMU_WR_HIT_CNT                              (0x0U)
#define DCACHE_PMU_WR_HIT_CNT_PMU_WR_HIT_CNT_SHIFT         (0U)
#define DCACHE_PMU_WR_HIT_CNT_PMU_WR_HIT_CNT_MASK          (0xFFFFFFFFU << DCACHE_PMU_WR_HIT_CNT_PMU_WR_HIT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_RD_MISS_PENALTY_CNT */
#define DCACHE_PMU_RD_MISS_PENALTY_CNT_OFFSET              (0x5CU)
#define DCACHE_PMU_RD_MISS_PENALTY_CNT                     (0x0U)
#define DCACHE_PMU_RD_MISS_PENALTY_CNT_PMU_RD_MISS_PENALTY_CNT_SHIFT (0U)
#define DCACHE_PMU_RD_MISS_PENALTY_CNT_PMU_RD_MISS_PENALTY_CNT_MASK (0xFFFFFFFFU << DCACHE_PMU_RD_MISS_PENALTY_CNT_PMU_RD_MISS_PENALTY_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_WR_MISS_PENALTY_CNT */
#define DCACHE_PMU_WR_MISS_PENALTY_CNT_OFFSET              (0x60U)
#define DCACHE_PMU_WR_MISS_PENALTY_CNT                     (0x0U)
#define DCACHE_PMU_WR_MISS_PENALTY_CNT_PMU_WR_MISS_PENALTY_CNT_SHIFT (0U)
#define DCACHE_PMU_WR_MISS_PENALTY_CNT_PMU_WR_MISS_PENALTY_CNT_MASK (0xFFFFFFFFU << DCACHE_PMU_WR_MISS_PENALTY_CNT_PMU_WR_MISS_PENALTY_CNT_SHIFT) /* 0xFFFFFFFF */
/* PMU_RD_LAT_CNT */
#define DCACHE_PMU_RD_LAT_CNT_OFFSET                       (0x64U)
#define DCACHE_PMU_RD_LAT_CNT                              (0x0U)
#define DCACHE_PMU_RD_LAT_CNT_PMU_RD_LAT_CNT_SHIFT         (0U)
#define DCACHE_PMU_RD_LAT_CNT_PMU_RD_LAT_CNT_MASK          (0xFFFFFFFFU << DCACHE_PMU_RD_LAT_CNT_PMU_RD_LAT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* PMU_WR_LAT_CNT */
#define DCACHE_PMU_WR_LAT_CNT_OFFSET                       (0x68U)
#define DCACHE_PMU_WR_LAT_CNT                              (0x0U)
#define DCACHE_PMU_WR_LAT_CNT_PMU_RD_LAT_CNT_SHIFT         (0U)
#define DCACHE_PMU_WR_LAT_CNT_PMU_RD_LAT_CNT_MASK          (0xFFFFFFFFU << DCACHE_PMU_WR_LAT_CNT_PMU_RD_LAT_CNT_SHIFT)  /* 0xFFFFFFFF */
/* REVISION */
#define DCACHE_REVISION_OFFSET                             (0xF0U)
#define DCACHE_REVISION                                    (0x100U)
#define DCACHE_REVISION_REVISION_SHIFT                     (0U)
#define DCACHE_REVISION_REVISION_MASK                      (0xFFFFFFFFU << DCACHE_REVISION_REVISION_SHIFT)              /* 0xFFFFFFFF */
/****************************************SPINLOCK****************************************/
/* STATUS_0 */
#define SPINLOCK_STATUS_0_OFFSET                           (0x0U)
#define SPINLOCK_STATUS_0_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_0_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_0_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_1 */
#define SPINLOCK_STATUS_1_OFFSET                           (0x4U)
#define SPINLOCK_STATUS_1_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_1_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_1_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_2 */
#define SPINLOCK_STATUS_2_OFFSET                           (0x8U)
#define SPINLOCK_STATUS_2_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_2_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_2_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_3 */
#define SPINLOCK_STATUS_3_OFFSET                           (0xCU)
#define SPINLOCK_STATUS_3_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_3_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_3_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_4 */
#define SPINLOCK_STATUS_4_OFFSET                           (0x10U)
#define SPINLOCK_STATUS_4_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_4_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_4_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_5 */
#define SPINLOCK_STATUS_5_OFFSET                           (0x14U)
#define SPINLOCK_STATUS_5_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_5_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_5_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_6 */
#define SPINLOCK_STATUS_6_OFFSET                           (0x18U)
#define SPINLOCK_STATUS_6_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_6_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_6_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_7 */
#define SPINLOCK_STATUS_7_OFFSET                           (0x1CU)
#define SPINLOCK_STATUS_7_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_7_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_7_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_8 */
#define SPINLOCK_STATUS_8_OFFSET                           (0x20U)
#define SPINLOCK_STATUS_8_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_8_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_8_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_9 */
#define SPINLOCK_STATUS_9_OFFSET                           (0x24U)
#define SPINLOCK_STATUS_9_SPINLOCK_STATUS_SHIFT            (0U)
#define SPINLOCK_STATUS_9_SPINLOCK_STATUS_MASK             (0xFU << SPINLOCK_STATUS_9_SPINLOCK_STATUS_SHIFT)            /* 0x0000000F */
/* STATUS_10 */
#define SPINLOCK_STATUS_10_OFFSET                          (0x28U)
#define SPINLOCK_STATUS_10_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_10_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_10_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_11 */
#define SPINLOCK_STATUS_11_OFFSET                          (0x2CU)
#define SPINLOCK_STATUS_11_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_11_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_11_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_12 */
#define SPINLOCK_STATUS_12_OFFSET                          (0x30U)
#define SPINLOCK_STATUS_12_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_12_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_12_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_13 */
#define SPINLOCK_STATUS_13_OFFSET                          (0x34U)
#define SPINLOCK_STATUS_13_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_13_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_13_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_14 */
#define SPINLOCK_STATUS_14_OFFSET                          (0x38U)
#define SPINLOCK_STATUS_14_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_14_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_14_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_15 */
#define SPINLOCK_STATUS_15_OFFSET                          (0x3CU)
#define SPINLOCK_STATUS_15_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_15_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_15_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_16 */
#define SPINLOCK_STATUS_16_OFFSET                          (0x40U)
#define SPINLOCK_STATUS_16_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_16_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_16_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_17 */
#define SPINLOCK_STATUS_17_OFFSET                          (0x44U)
#define SPINLOCK_STATUS_17_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_17_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_17_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_18 */
#define SPINLOCK_STATUS_18_OFFSET                          (0x48U)
#define SPINLOCK_STATUS_18_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_18_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_18_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_19 */
#define SPINLOCK_STATUS_19_OFFSET                          (0x4CU)
#define SPINLOCK_STATUS_19_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_19_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_19_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_20 */
#define SPINLOCK_STATUS_20_OFFSET                          (0x50U)
#define SPINLOCK_STATUS_20_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_20_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_20_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_21 */
#define SPINLOCK_STATUS_21_OFFSET                          (0x54U)
#define SPINLOCK_STATUS_21_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_21_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_21_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_22 */
#define SPINLOCK_STATUS_22_OFFSET                          (0x58U)
#define SPINLOCK_STATUS_22_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_22_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_22_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_23 */
#define SPINLOCK_STATUS_23_OFFSET                          (0x5CU)
#define SPINLOCK_STATUS_23_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_23_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_23_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_24 */
#define SPINLOCK_STATUS_24_OFFSET                          (0x60U)
#define SPINLOCK_STATUS_24_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_24_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_24_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_25 */
#define SPINLOCK_STATUS_25_OFFSET                          (0x64U)
#define SPINLOCK_STATUS_25_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_25_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_25_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_26 */
#define SPINLOCK_STATUS_26_OFFSET                          (0x68U)
#define SPINLOCK_STATUS_26_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_26_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_26_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_27 */
#define SPINLOCK_STATUS_27_OFFSET                          (0x6CU)
#define SPINLOCK_STATUS_27_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_27_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_27_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_28 */
#define SPINLOCK_STATUS_28_OFFSET                          (0x70U)
#define SPINLOCK_STATUS_28_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_28_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_28_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_29 */
#define SPINLOCK_STATUS_29_OFFSET                          (0x74U)
#define SPINLOCK_STATUS_29_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_29_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_29_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_30 */
#define SPINLOCK_STATUS_30_OFFSET                          (0x78U)
#define SPINLOCK_STATUS_30_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_30_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_30_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_31 */
#define SPINLOCK_STATUS_31_OFFSET                          (0x7CU)
#define SPINLOCK_STATUS_31_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_31_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_31_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_32 */
#define SPINLOCK_STATUS_32_OFFSET                          (0x80U)
#define SPINLOCK_STATUS_32_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_32_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_32_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_33 */
#define SPINLOCK_STATUS_33_OFFSET                          (0x84U)
#define SPINLOCK_STATUS_33_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_33_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_33_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_34 */
#define SPINLOCK_STATUS_34_OFFSET                          (0x88U)
#define SPINLOCK_STATUS_34_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_34_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_34_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_35 */
#define SPINLOCK_STATUS_35_OFFSET                          (0x8CU)
#define SPINLOCK_STATUS_35_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_35_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_35_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_36 */
#define SPINLOCK_STATUS_36_OFFSET                          (0x90U)
#define SPINLOCK_STATUS_36_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_36_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_36_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_37 */
#define SPINLOCK_STATUS_37_OFFSET                          (0x94U)
#define SPINLOCK_STATUS_37_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_37_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_37_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_38 */
#define SPINLOCK_STATUS_38_OFFSET                          (0x98U)
#define SPINLOCK_STATUS_38_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_38_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_38_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_39 */
#define SPINLOCK_STATUS_39_OFFSET                          (0x9CU)
#define SPINLOCK_STATUS_39_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_39_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_39_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_40 */
#define SPINLOCK_STATUS_40_OFFSET                          (0xA0U)
#define SPINLOCK_STATUS_40_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_40_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_40_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_41 */
#define SPINLOCK_STATUS_41_OFFSET                          (0xA4U)
#define SPINLOCK_STATUS_41_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_41_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_41_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_42 */
#define SPINLOCK_STATUS_42_OFFSET                          (0xA8U)
#define SPINLOCK_STATUS_42_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_42_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_42_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_43 */
#define SPINLOCK_STATUS_43_OFFSET                          (0xACU)
#define SPINLOCK_STATUS_43_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_43_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_43_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_44 */
#define SPINLOCK_STATUS_44_OFFSET                          (0xB0U)
#define SPINLOCK_STATUS_44_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_44_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_44_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_45 */
#define SPINLOCK_STATUS_45_OFFSET                          (0xB4U)
#define SPINLOCK_STATUS_45_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_45_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_45_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_46 */
#define SPINLOCK_STATUS_46_OFFSET                          (0xB8U)
#define SPINLOCK_STATUS_46_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_46_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_46_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_47 */
#define SPINLOCK_STATUS_47_OFFSET                          (0xBCU)
#define SPINLOCK_STATUS_47_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_47_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_47_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_48 */
#define SPINLOCK_STATUS_48_OFFSET                          (0xC0U)
#define SPINLOCK_STATUS_48_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_48_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_48_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_49 */
#define SPINLOCK_STATUS_49_OFFSET                          (0xC4U)
#define SPINLOCK_STATUS_49_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_49_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_49_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_50 */
#define SPINLOCK_STATUS_50_OFFSET                          (0xC8U)
#define SPINLOCK_STATUS_50_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_50_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_50_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_51 */
#define SPINLOCK_STATUS_51_OFFSET                          (0xCCU)
#define SPINLOCK_STATUS_51_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_51_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_51_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_52 */
#define SPINLOCK_STATUS_52_OFFSET                          (0xD0U)
#define SPINLOCK_STATUS_52_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_52_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_52_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_53 */
#define SPINLOCK_STATUS_53_OFFSET                          (0xD4U)
#define SPINLOCK_STATUS_53_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_53_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_53_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_54 */
#define SPINLOCK_STATUS_54_OFFSET                          (0xD8U)
#define SPINLOCK_STATUS_54_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_54_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_54_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_55 */
#define SPINLOCK_STATUS_55_OFFSET                          (0xDCU)
#define SPINLOCK_STATUS_55_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_55_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_55_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_56 */
#define SPINLOCK_STATUS_56_OFFSET                          (0xE0U)
#define SPINLOCK_STATUS_56_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_56_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_56_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_57 */
#define SPINLOCK_STATUS_57_OFFSET                          (0xE4U)
#define SPINLOCK_STATUS_57_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_57_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_57_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_58 */
#define SPINLOCK_STATUS_58_OFFSET                          (0xE8U)
#define SPINLOCK_STATUS_58_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_58_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_58_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_59 */
#define SPINLOCK_STATUS_59_OFFSET                          (0xECU)
#define SPINLOCK_STATUS_59_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_59_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_59_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_60 */
#define SPINLOCK_STATUS_60_OFFSET                          (0xF0U)
#define SPINLOCK_STATUS_60_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_60_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_60_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_61 */
#define SPINLOCK_STATUS_61_OFFSET                          (0xF4U)
#define SPINLOCK_STATUS_61_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_61_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_61_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_62 */
#define SPINLOCK_STATUS_62_OFFSET                          (0xF8U)
#define SPINLOCK_STATUS_62_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_62_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_62_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/* STATUS_63 */
#define SPINLOCK_STATUS_63_OFFSET                          (0xFCU)
#define SPINLOCK_STATUS_63_SPINLOCK_STATUS_SHIFT           (0U)
#define SPINLOCK_STATUS_63_SPINLOCK_STATUS_MASK            (0xFU << SPINLOCK_STATUS_63_SPINLOCK_STATUS_SHIFT)           /* 0x0000000F */
/*****************************************TIMER******************************************/
/* LOAD_COUNT0 */
#define TIMER_LOAD_COUNT0_OFFSET                           (0x0U)
#define TIMER_LOAD_COUNT0_COUNT0_SHIFT                     (0U)
#define TIMER_LOAD_COUNT0_COUNT0_MASK                      (0xFFFFFFFFU << TIMER_LOAD_COUNT0_COUNT0_SHIFT)              /* 0xFFFFFFFF */
/* LOAD_COUNT1 */
#define TIMER_LOAD_COUNT1_OFFSET                           (0x4U)
#define TIMER_LOAD_COUNT1_COUNT1_SHIFT                     (0U)
#define TIMER_LOAD_COUNT1_COUNT1_MASK                      (0xFFFFFFFFU << TIMER_LOAD_COUNT1_COUNT1_SHIFT)              /* 0xFFFFFFFF */
/* CURRENT_VALUE0 */
#define TIMER_CURRENT_VALUE0_OFFSET                        (0x8U)
#define TIMER_CURRENT_VALUE0                               (0x0U)
#define TIMER_CURRENT_VALUE0_CURRENT_VALUE0_SHIFT          (0U)
#define TIMER_CURRENT_VALUE0_CURRENT_VALUE0_MASK           (0xFFFFFFFFU << TIMER_CURRENT_VALUE0_CURRENT_VALUE0_SHIFT)   /* 0xFFFFFFFF */
/* CURRENT_VALUE1 */
#define TIMER_CURRENT_VALUE1_OFFSET                        (0xCU)
#define TIMER_CURRENT_VALUE1                               (0x0U)
#define TIMER_CURRENT_VALUE1_CURRENT_VALUE1_SHIFT          (0U)
#define TIMER_CURRENT_VALUE1_CURRENT_VALUE1_MASK           (0xFFFFFFFFU << TIMER_CURRENT_VALUE1_CURRENT_VALUE1_SHIFT)   /* 0xFFFFFFFF */
/* CONTROLREG */
#define TIMER_CONTROLREG_OFFSET                            (0x10U)
#define TIMER_CONTROLREG_TIMER_ENABLE_SHIFT                (0U)
#define TIMER_CONTROLREG_TIMER_ENABLE_MASK                 (0x1U << TIMER_CONTROLREG_TIMER_ENABLE_SHIFT)                /* 0x00000001 */
#define TIMER_CONTROLREG_TIMER_MODE_SHIFT                  (1U)
#define TIMER_CONTROLREG_TIMER_MODE_MASK                   (0x1U << TIMER_CONTROLREG_TIMER_MODE_SHIFT)                  /* 0x00000002 */
#define TIMER_CONTROLREG_TIMER_INT_MASK_SHIFT              (2U)
#define TIMER_CONTROLREG_TIMER_INT_MASK_MASK               (0x1U << TIMER_CONTROLREG_TIMER_INT_MASK_SHIFT)              /* 0x00000004 */
/* INTSTATUS */
#define TIMER_INTSTATUS_OFFSET                             (0x18U)
#define TIMER_INTSTATUS_INT_PD_SHIFT                       (0U)
#define TIMER_INTSTATUS_INT_PD_MASK                        (0x1U << TIMER_INTSTATUS_INT_PD_SHIFT)                       /* 0x00000001 */
/******************************************SPI*******************************************/
/* CTRLR0 */
#define SPI_CTRLR0_OFFSET                                  (0x0U)
#define SPI_CTRLR0_DFS_SHIFT                               (0U)
#define SPI_CTRLR0_DFS_MASK                                (0x3U << SPI_CTRLR0_DFS_SHIFT)                               /* 0x00000003 */
#define SPI_CTRLR0_CFS_SHIFT                               (2U)
#define SPI_CTRLR0_CFS_MASK                                (0xFU << SPI_CTRLR0_CFS_SHIFT)                               /* 0x0000003C */
#define SPI_CTRLR0_SCPH_SHIFT                              (6U)
#define SPI_CTRLR0_SCPH_MASK                               (0x1U << SPI_CTRLR0_SCPH_SHIFT)                              /* 0x00000040 */
#define SPI_CTRLR0_SCPOL_SHIFT                             (7U)
#define SPI_CTRLR0_SCPOL_MASK                              (0x1U << SPI_CTRLR0_SCPOL_SHIFT)                             /* 0x00000080 */
#define SPI_CTRLR0_CSM_SHIFT                               (8U)
#define SPI_CTRLR0_CSM_MASK                                (0x3U << SPI_CTRLR0_CSM_SHIFT)                               /* 0x00000300 */
#define SPI_CTRLR0_SSD_SHIFT                               (10U)
#define SPI_CTRLR0_SSD_MASK                                (0x1U << SPI_CTRLR0_SSD_SHIFT)                               /* 0x00000400 */
#define SPI_CTRLR0_EM_SHIFT                                (11U)
#define SPI_CTRLR0_EM_MASK                                 (0x1U << SPI_CTRLR0_EM_SHIFT)                                /* 0x00000800 */
#define SPI_CTRLR0_FBM_SHIFT                               (12U)
#define SPI_CTRLR0_FBM_MASK                                (0x1U << SPI_CTRLR0_FBM_SHIFT)                               /* 0x00001000 */
#define SPI_CTRLR0_BHT_SHIFT                               (13U)
#define SPI_CTRLR0_BHT_MASK                                (0x1U << SPI_CTRLR0_BHT_SHIFT)                               /* 0x00002000 */
#define SPI_CTRLR0_RSD_SHIFT                               (14U)
#define SPI_CTRLR0_RSD_MASK                                (0x3U << SPI_CTRLR0_RSD_SHIFT)                               /* 0x0000C000 */
#define SPI_CTRLR0_FRF_SHIFT                               (16U)
#define SPI_CTRLR0_FRF_MASK                                (0x3U << SPI_CTRLR0_FRF_SHIFT)                               /* 0x00030000 */
#define SPI_CTRLR0_XFM_SHIFT                               (18U)
#define SPI_CTRLR0_XFM_MASK                                (0x3U << SPI_CTRLR0_XFM_SHIFT)                               /* 0x000C0000 */
#define SPI_CTRLR0_OPM_SHIFT                               (20U)
#define SPI_CTRLR0_OPM_MASK                                (0x1U << SPI_CTRLR0_OPM_SHIFT)                               /* 0x00100000 */
#define SPI_CTRLR0_MTM_SHIFT                               (21U)
#define SPI_CTRLR0_MTM_MASK                                (0x1U << SPI_CTRLR0_MTM_SHIFT)                               /* 0x00200000 */
#define SPI_CTRLR0_SM_SHIFT                                (22U)
#define SPI_CTRLR0_SM_MASK                                 (0x1U << SPI_CTRLR0_SM_SHIFT)                                /* 0x00400000 */
#define SPI_CTRLR0_SOI_SHIFT                               (23U)
#define SPI_CTRLR0_SOI_MASK                                (0x3U << SPI_CTRLR0_SOI_SHIFT)                               /* 0x01800000 */
#define SPI_CTRLR0_LBK_SHIFT                               (25U)
#define SPI_CTRLR0_LBK_MASK                                (0x1U << SPI_CTRLR0_LBK_SHIFT)                               /* 0x02000000 */
/* CTRLR1 */
#define SPI_CTRLR1_OFFSET                                  (0x4U)
#define SPI_CTRLR1_NDM_SHIFT                               (0U)
#define SPI_CTRLR1_NDM_MASK                                (0xFFFFFFFFU << SPI_CTRLR1_NDM_SHIFT)                        /* 0xFFFFFFFF */
/* ENR */
#define SPI_ENR_OFFSET                                     (0x8U)
#define SPI_ENR_ENR_SHIFT                                  (0U)
#define SPI_ENR_ENR_MASK                                   (0x1U << SPI_ENR_ENR_SHIFT)                                  /* 0x00000001 */
/* SER */
#define SPI_SER_OFFSET                                     (0xCU)
#define SPI_SER_SER_SHIFT                                  (0U)
#define SPI_SER_SER_MASK                                   (0x3U << SPI_SER_SER_SHIFT)                                  /* 0x00000003 */
/* BAUDR */
#define SPI_BAUDR_OFFSET                                   (0x10U)
#define SPI_BAUDR_BAUDR_SHIFT                              (0U)
#define SPI_BAUDR_BAUDR_MASK                               (0xFFFFU << SPI_BAUDR_BAUDR_SHIFT)                           /* 0x0000FFFF */
/* TXFTLR */
#define SPI_TXFTLR_OFFSET                                  (0x14U)
#define SPI_TXFTLR_XFTLR_SHIFT                             (0U)
#define SPI_TXFTLR_XFTLR_MASK                              (0x3FU << SPI_TXFTLR_XFTLR_SHIFT)                            /* 0x0000003F */
/* RXFTLR */
#define SPI_RXFTLR_OFFSET                                  (0x18U)
#define SPI_RXFTLR_RXFTLR_SHIFT                            (0U)
#define SPI_RXFTLR_RXFTLR_MASK                             (0x3FU << SPI_RXFTLR_RXFTLR_SHIFT)                           /* 0x0000003F */
/* TXFLR */
#define SPI_TXFLR_OFFSET                                   (0x1CU)
#define SPI_TXFLR                                          (0x0U)
#define SPI_TXFLR_TXFLR_SHIFT                              (0U)
#define SPI_TXFLR_TXFLR_MASK                               (0x7FU << SPI_TXFLR_TXFLR_SHIFT)                             /* 0x0000007F */
/* RXFLR */
#define SPI_RXFLR_OFFSET                                   (0x20U)
#define SPI_RXFLR                                          (0x0U)
#define SPI_RXFLR_RXFLR_SHIFT                              (0U)
#define SPI_RXFLR_RXFLR_MASK                               (0x7FU << SPI_RXFLR_RXFLR_SHIFT)                             /* 0x0000007F */
/* SR */
#define SPI_SR_OFFSET                                      (0x24U)
#define SPI_SR                                             (0x4CU)
#define SPI_SR_BSF_SHIFT                                   (0U)
#define SPI_SR_BSF_MASK                                    (0x1U << SPI_SR_BSF_SHIFT)                                   /* 0x00000001 */
#define SPI_SR_TFF_SHIFT                                   (1U)
#define SPI_SR_TFF_MASK                                    (0x1U << SPI_SR_TFF_SHIFT)                                   /* 0x00000002 */
#define SPI_SR_TFE_SHIFT                                   (2U)
#define SPI_SR_TFE_MASK                                    (0x1U << SPI_SR_TFE_SHIFT)                                   /* 0x00000004 */
#define SPI_SR_RFE_SHIFT                                   (3U)
#define SPI_SR_RFE_MASK                                    (0x1U << SPI_SR_RFE_SHIFT)                                   /* 0x00000008 */
#define SPI_SR_RFF_SHIFT                                   (4U)
#define SPI_SR_RFF_MASK                                    (0x1U << SPI_SR_RFF_SHIFT)                                   /* 0x00000010 */
#define SPI_SR_STB_SHIFT                                   (5U)
#define SPI_SR_STB_MASK                                    (0x1U << SPI_SR_STB_SHIFT)                                   /* 0x00000020 */
#define SPI_SR_SSI_SHIFT                                   (6U)
#define SPI_SR_SSI_MASK                                    (0x1U << SPI_SR_SSI_SHIFT)                                   /* 0x00000040 */
/* IPR */
#define SPI_IPR_OFFSET                                     (0x28U)
#define SPI_IPR_IPR_SHIFT                                  (0U)
#define SPI_IPR_IPR_MASK                                   (0x1U << SPI_IPR_IPR_SHIFT)                                  /* 0x00000001 */
/* IMR */
#define SPI_IMR_OFFSET                                     (0x2CU)
#define SPI_IMR_TFEIM_SHIFT                                (0U)
#define SPI_IMR_TFEIM_MASK                                 (0x1U << SPI_IMR_TFEIM_SHIFT)                                /* 0x00000001 */
#define SPI_IMR_TFOIM_SHIFT                                (1U)
#define SPI_IMR_TFOIM_MASK                                 (0x1U << SPI_IMR_TFOIM_SHIFT)                                /* 0x00000002 */
#define SPI_IMR_RFUIM_SHIFT                                (2U)
#define SPI_IMR_RFUIM_MASK                                 (0x1U << SPI_IMR_RFUIM_SHIFT)                                /* 0x00000004 */
#define SPI_IMR_RFOIM_SHIFT                                (3U)
#define SPI_IMR_RFOIM_MASK                                 (0x1U << SPI_IMR_RFOIM_SHIFT)                                /* 0x00000008 */
#define SPI_IMR_RFFIM_SHIFT                                (4U)
#define SPI_IMR_RFFIM_MASK                                 (0x1U << SPI_IMR_RFFIM_SHIFT)                                /* 0x00000010 */
#define SPI_IMR_TOIM_SHIFT                                 (5U)
#define SPI_IMR_TOIM_MASK                                  (0x1U << SPI_IMR_TOIM_SHIFT)                                 /* 0x00000020 */
#define SPI_IMR_SSPIM_SHIFT                                (6U)
#define SPI_IMR_SSPIM_MASK                                 (0x1U << SPI_IMR_SSPIM_SHIFT)                                /* 0x00000040 */
#define SPI_IMR_TXFIM_SHIFT                                (7U)
#define SPI_IMR_TXFIM_MASK                                 (0x1U << SPI_IMR_TXFIM_SHIFT)                                /* 0x00000080 */
/* ISR */
#define SPI_ISR_OFFSET                                     (0x30U)
#define SPI_ISR_TFEIS_SHIFT                                (0U)
#define SPI_ISR_TFEIS_MASK                                 (0x1U << SPI_ISR_TFEIS_SHIFT)                                /* 0x00000001 */
#define SPI_ISR_TFOIS_SHIFT                                (1U)
#define SPI_ISR_TFOIS_MASK                                 (0x1U << SPI_ISR_TFOIS_SHIFT)                                /* 0x00000002 */
#define SPI_ISR_RFUIS_SHIFT                                (2U)
#define SPI_ISR_RFUIS_MASK                                 (0x1U << SPI_ISR_RFUIS_SHIFT)                                /* 0x00000004 */
#define SPI_ISR_RFOIS_SHIFT                                (3U)
#define SPI_ISR_RFOIS_MASK                                 (0x1U << SPI_ISR_RFOIS_SHIFT)                                /* 0x00000008 */
#define SPI_ISR_RFFIS_SHIFT                                (4U)
#define SPI_ISR_RFFIS_MASK                                 (0x1U << SPI_ISR_RFFIS_SHIFT)                                /* 0x00000010 */
#define SPI_ISR_TOIS_SHIFT                                 (5U)
#define SPI_ISR_TOIS_MASK                                  (0x1U << SPI_ISR_TOIS_SHIFT)                                 /* 0x00000020 */
#define SPI_ISR_SSPIS_SHIFT                                (6U)
#define SPI_ISR_SSPIS_MASK                                 (0x1U << SPI_ISR_SSPIS_SHIFT)                                /* 0x00000040 */
#define SPI_ISR_TXFIS_SHIFT                                (7U)
#define SPI_ISR_TXFIS_MASK                                 (0x1U << SPI_ISR_TXFIS_SHIFT)                                /* 0x00000080 */
/* RISR */
#define SPI_RISR_OFFSET                                    (0x34U)
#define SPI_RISR_TFERIS_SHIFT                              (0U)
#define SPI_RISR_TFERIS_MASK                               (0x1U << SPI_RISR_TFERIS_SHIFT)                              /* 0x00000001 */
#define SPI_RISR_TFORIS_SHIFT                              (1U)
#define SPI_RISR_TFORIS_MASK                               (0x1U << SPI_RISR_TFORIS_SHIFT)                              /* 0x00000002 */
#define SPI_RISR_RFURIS_SHIFT                              (2U)
#define SPI_RISR_RFURIS_MASK                               (0x1U << SPI_RISR_RFURIS_SHIFT)                              /* 0x00000004 */
#define SPI_RISR_RFORIS_SHIFT                              (3U)
#define SPI_RISR_RFORIS_MASK                               (0x1U << SPI_RISR_RFORIS_SHIFT)                              /* 0x00000008 */
#define SPI_RISR_RFFRIS_SHIFT                              (4U)
#define SPI_RISR_RFFRIS_MASK                               (0x1U << SPI_RISR_RFFRIS_SHIFT)                              /* 0x00000010 */
#define SPI_RISR_TORIS_SHIFT                               (5U)
#define SPI_RISR_TORIS_MASK                                (0x1U << SPI_RISR_TORIS_SHIFT)                               /* 0x00000020 */
#define SPI_RISR_SSPRIS_SHIFT                              (6U)
#define SPI_RISR_SSPRIS_MASK                               (0x1U << SPI_RISR_SSPRIS_SHIFT)                              /* 0x00000040 */
#define SPI_RISR_TXFRIS_SHIFT                              (7U)
#define SPI_RISR_TXFRIS_MASK                               (0x1U << SPI_RISR_TXFRIS_SHIFT)                              /* 0x00000080 */
/* ICR */
#define SPI_ICR_OFFSET                                     (0x38U)
#define SPI_ICR_CCI_SHIFT                                  (0U)
#define SPI_ICR_CCI_MASK                                   (0x1U << SPI_ICR_CCI_SHIFT)                                  /* 0x00000001 */
#define SPI_ICR_CRFUI_SHIFT                                (1U)
#define SPI_ICR_CRFUI_MASK                                 (0x1U << SPI_ICR_CRFUI_SHIFT)                                /* 0x00000002 */
#define SPI_ICR_CRFOI_SHIFT                                (2U)
#define SPI_ICR_CRFOI_MASK                                 (0x1U << SPI_ICR_CRFOI_SHIFT)                                /* 0x00000004 */
#define SPI_ICR_CTFOI_SHIFT                                (3U)
#define SPI_ICR_CTFOI_MASK                                 (0x1U << SPI_ICR_CTFOI_SHIFT)                                /* 0x00000008 */
#define SPI_ICR_CTOI_SHIFT                                 (4U)
#define SPI_ICR_CTOI_MASK                                  (0x1U << SPI_ICR_CTOI_SHIFT)                                 /* 0x00000010 */
#define SPI_ICR_CSSPI_SHIFT                                (5U)
#define SPI_ICR_CSSPI_MASK                                 (0x1U << SPI_ICR_CSSPI_SHIFT)                                /* 0x00000020 */
#define SPI_ICR_CTXFI_SHIFT                                (6U)
#define SPI_ICR_CTXFI_MASK                                 (0x1U << SPI_ICR_CTXFI_SHIFT)                                /* 0x00000040 */
/* DMACR */
#define SPI_DMACR_OFFSET                                   (0x3CU)
#define SPI_DMACR_RDE_SHIFT                                (0U)
#define SPI_DMACR_RDE_MASK                                 (0x1U << SPI_DMACR_RDE_SHIFT)                                /* 0x00000001 */
#define SPI_DMACR_TDE_SHIFT                                (1U)
#define SPI_DMACR_TDE_MASK                                 (0x1U << SPI_DMACR_TDE_SHIFT)                                /* 0x00000002 */
/* DMATDLR */
#define SPI_DMATDLR_OFFSET                                 (0x40U)
#define SPI_DMATDLR_TDL_SHIFT                              (0U)
#define SPI_DMATDLR_TDL_MASK                               (0x3FU << SPI_DMATDLR_TDL_SHIFT)                             /* 0x0000003F */
/* DMARDLR */
#define SPI_DMARDLR_OFFSET                                 (0x44U)
#define SPI_DMARDLR_RDL_SHIFT                              (0U)
#define SPI_DMARDLR_RDL_MASK                               (0x3FU << SPI_DMARDLR_RDL_SHIFT)                             /* 0x0000003F */
/* VERSION */
#define SPI_VERSION_OFFSET                                 (0x48U)
#define SPI_VERSION                                        (0x110002U)
#define SPI_VERSION_VERSION_SHIFT                          (0U)
#define SPI_VERSION_VERSION_MASK                           (0xFFFFFFFFU << SPI_VERSION_VERSION_SHIFT)                   /* 0xFFFFFFFF */
/* TIMEOUT */
#define SPI_TIMEOUT_OFFSET                                 (0x4CU)
#define SPI_TIMEOUT_TOV_SHIFT                              (0U)
#define SPI_TIMEOUT_TOV_MASK                               (0xFFFFU << SPI_TIMEOUT_TOV_SHIFT)                           /* 0x0000FFFF */
#define SPI_TIMEOUT_TOE_SHIFT                              (16U)
#define SPI_TIMEOUT_TOE_MASK                               (0x1U << SPI_TIMEOUT_TOE_SHIFT)                              /* 0x00010000 */
/* BYPASS */
#define SPI_BYPASS_OFFSET                                  (0x50U)
#define SPI_BYPASS_BYEN_SHIFT                              (0U)
#define SPI_BYPASS_BYEN_MASK                               (0x1U << SPI_BYPASS_BYEN_SHIFT)                              /* 0x00000001 */
#define SPI_BYPASS_FBM_SHIFT                               (1U)
#define SPI_BYPASS_FBM_MASK                                (0x1U << SPI_BYPASS_FBM_SHIFT)                               /* 0x00000002 */
#define SPI_BYPASS_END_SHIFT                               (2U)
#define SPI_BYPASS_END_MASK                                (0x1U << SPI_BYPASS_END_SHIFT)                               /* 0x00000004 */
#define SPI_BYPASS_RXCP_SHIFT                              (3U)
#define SPI_BYPASS_RXCP_MASK                               (0x1U << SPI_BYPASS_RXCP_SHIFT)                              /* 0x00000008 */
#define SPI_BYPASS_TXCP_SHIFT                              (4U)
#define SPI_BYPASS_TXCP_MASK                               (0x1U << SPI_BYPASS_TXCP_SHIFT)                              /* 0x00000010 */
/* TXDR */
#define SPI_TXDR_OFFSET                                    (0x400U)
#define SPI_TXDR_TXDR_SHIFT                                (0U)
#define SPI_TXDR_TXDR_MASK                                 (0xFFFFU << SPI_TXDR_TXDR_SHIFT)                             /* 0x0000FFFF */
/* RXDR */
#define SPI_RXDR_OFFSET                                    (0x800U)
#define SPI_RXDR                                           (0x0U)
#define SPI_RXDR_RXDR_SHIFT                                (0U)
#define SPI_RXDR_RXDR_MASK                                 (0xFFFFU << SPI_RXDR_RXDR_SHIFT)                             /* 0x0000FFFF */
/*****************************************SARADC*****************************************/
/* CONV_CON */
#define SARADC_CONV_CON_OFFSET                             (0x0U)
#define SARADC_CONV_CON_CHANNEL_SEL_SHIFT                  (0U)
#define SARADC_CONV_CON_CHANNEL_SEL_MASK                   (0xFU << SARADC_CONV_CON_CHANNEL_SEL_SHIFT)                  /* 0x0000000F */
#define SARADC_CONV_CON_START_ADC_SHIFT                    (4U)
#define SARADC_CONV_CON_START_ADC_MASK                     (0x1U << SARADC_CONV_CON_START_ADC_SHIFT)                    /* 0x00000010 */
#define SARADC_CONV_CON_SINGLE_PD_MODE_SHIFT               (5U)
#define SARADC_CONV_CON_SINGLE_PD_MODE_MASK                (0x1U << SARADC_CONV_CON_SINGLE_PD_MODE_SHIFT)               /* 0x00000020 */
#define SARADC_CONV_CON_AUTO_CHANNEL_MODE_SHIFT            (6U)
#define SARADC_CONV_CON_AUTO_CHANNEL_MODE_MASK             (0x1U << SARADC_CONV_CON_AUTO_CHANNEL_MODE_SHIFT)            /* 0x00000040 */
#define SARADC_CONV_CON_END_CONV_SHIFT                     (7U)
#define SARADC_CONV_CON_END_CONV_MASK                      (0x1U << SARADC_CONV_CON_END_CONV_SHIFT)                     /* 0x00000080 */
#define SARADC_CONV_CON_AS_PD_MODE_SHIFT                   (8U)
#define SARADC_CONV_CON_AS_PD_MODE_MASK                    (0x1U << SARADC_CONV_CON_AS_PD_MODE_SHIFT)                   /* 0x00000100 */
#define SARADC_CONV_CON_INT_LOCK_SHIFT                     (9U)
#define SARADC_CONV_CON_INT_LOCK_MASK                      (0x1U << SARADC_CONV_CON_INT_LOCK_SHIFT)                     /* 0x00000200 */
/* T_PD_SOC */
#define SARADC_T_PD_SOC_OFFSET                             (0x4U)
#define SARADC_T_PD_SOC_T_PD_SOC_SHIFT                     (0U)
#define SARADC_T_PD_SOC_T_PD_SOC_MASK                      (0xFFU << SARADC_T_PD_SOC_T_PD_SOC_SHIFT)                    /* 0x000000FF */
/* T_AS_SOC */
#define SARADC_T_AS_SOC_OFFSET                             (0x8U)
#define SARADC_T_AS_SOC_T_AS_SOC_SHIFT                     (0U)
#define SARADC_T_AS_SOC_T_AS_SOC_MASK                      (0xFFFFFFFFU << SARADC_T_AS_SOC_T_AS_SOC_SHIFT)              /* 0xFFFFFFFF */
/* T_DAS_SOC */
#define SARADC_T_DAS_SOC_OFFSET                            (0xCU)
#define SARADC_T_DAS_SOC_T_DAS_SOC_SHIFT                   (0U)
#define SARADC_T_DAS_SOC_T_DAS_SOC_MASK                    (0xFFFFFFFFU << SARADC_T_DAS_SOC_T_DAS_SOC_SHIFT)            /* 0xFFFFFFFF */
/* T_SEL_SOC */
#define SARADC_T_SEL_SOC_OFFSET                            (0x10U)
#define SARADC_T_SEL_SOC_T_SEL_SOC_SHIFT                   (0U)
#define SARADC_T_SEL_SOC_T_SEL_SOC_MASK                    (0xFFFFU << SARADC_T_SEL_SOC_T_SEL_SOC_SHIFT)                /* 0x0000FFFF */
/* HIGH_COMP0 */
#define SARADC_HIGH_COMP0_OFFSET                           (0x14U)
#define SARADC_HIGH_COMP0_HIGH_COMP0_SHIFT                 (0U)
#define SARADC_HIGH_COMP0_HIGH_COMP0_MASK                  (0xFFFU << SARADC_HIGH_COMP0_HIGH_COMP0_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP1 */
#define SARADC_HIGH_COMP1_OFFSET                           (0x18U)
#define SARADC_HIGH_COMP1_HIGH_COMP1_SHIFT                 (0U)
#define SARADC_HIGH_COMP1_HIGH_COMP1_MASK                  (0xFFFU << SARADC_HIGH_COMP1_HIGH_COMP1_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP2 */
#define SARADC_HIGH_COMP2_OFFSET                           (0x1CU)
#define SARADC_HIGH_COMP2_HIGH_COMP2_SHIFT                 (0U)
#define SARADC_HIGH_COMP2_HIGH_COMP2_MASK                  (0xFFFU << SARADC_HIGH_COMP2_HIGH_COMP2_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP3 */
#define SARADC_HIGH_COMP3_OFFSET                           (0x20U)
#define SARADC_HIGH_COMP3_HIGH_COMP3_SHIFT                 (0U)
#define SARADC_HIGH_COMP3_HIGH_COMP3_MASK                  (0xFFFU << SARADC_HIGH_COMP3_HIGH_COMP3_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP4 */
#define SARADC_HIGH_COMP4_OFFSET                           (0x24U)
#define SARADC_HIGH_COMP4_HIGH_COMP4_SHIFT                 (0U)
#define SARADC_HIGH_COMP4_HIGH_COMP4_MASK                  (0xFFFU << SARADC_HIGH_COMP4_HIGH_COMP4_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP5 */
#define SARADC_HIGH_COMP5_OFFSET                           (0x28U)
#define SARADC_HIGH_COMP5_HIGH_COMP5_SHIFT                 (0U)
#define SARADC_HIGH_COMP5_HIGH_COMP5_MASK                  (0xFFFU << SARADC_HIGH_COMP5_HIGH_COMP5_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP7 */
#define SARADC_HIGH_COMP7_OFFSET                           (0x30U)
#define SARADC_HIGH_COMP7_HIGH_COMP7_SHIFT                 (0U)
#define SARADC_HIGH_COMP7_HIGH_COMP7_MASK                  (0xFFFU << SARADC_HIGH_COMP7_HIGH_COMP7_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP8 */
#define SARADC_HIGH_COMP8_OFFSET                           (0x34U)
#define SARADC_HIGH_COMP8_HIGH_COMP8_SHIFT                 (0U)
#define SARADC_HIGH_COMP8_HIGH_COMP8_MASK                  (0xFFFU << SARADC_HIGH_COMP8_HIGH_COMP8_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP9 */
#define SARADC_HIGH_COMP9_OFFSET                           (0x38U)
#define SARADC_HIGH_COMP9_HIGH_COMP9_SHIFT                 (0U)
#define SARADC_HIGH_COMP9_HIGH_COMP9_MASK                  (0xFFFU << SARADC_HIGH_COMP9_HIGH_COMP9_SHIFT)               /* 0x00000FFF */
/* HIGH_COMP10 */
#define SARADC_HIGH_COMP10_OFFSET                          (0x3CU)
#define SARADC_HIGH_COMP10_HIGH_COMP10_SHIFT               (0U)
#define SARADC_HIGH_COMP10_HIGH_COMP10_MASK                (0xFFFU << SARADC_HIGH_COMP10_HIGH_COMP10_SHIFT)             /* 0x00000FFF */
/* HIGH_COMP11 */
#define SARADC_HIGH_COMP11_OFFSET                          (0x40U)
#define SARADC_HIGH_COMP11_HIGH_COMP11_SHIFT               (0U)
#define SARADC_HIGH_COMP11_HIGH_COMP11_MASK                (0xFFFU << SARADC_HIGH_COMP11_HIGH_COMP11_SHIFT)             /* 0x00000FFF */
/* HIGH_COMP12 */
#define SARADC_HIGH_COMP12_OFFSET                          (0x44U)
#define SARADC_HIGH_COMP12_HIGH_COMP12_SHIFT               (0U)
#define SARADC_HIGH_COMP12_HIGH_COMP12_MASK                (0xFFFU << SARADC_HIGH_COMP12_HIGH_COMP12_SHIFT)             /* 0x00000FFF */
/* HIGH_COMP13 */
#define SARADC_HIGH_COMP13_OFFSET                          (0x48U)
#define SARADC_HIGH_COMP13_HIGH_COMP13_SHIFT               (0U)
#define SARADC_HIGH_COMP13_HIGH_COMP13_MASK                (0xFFFU << SARADC_HIGH_COMP13_HIGH_COMP13_SHIFT)             /* 0x00000FFF */
/* HIGH_COMP14 */
#define SARADC_HIGH_COMP14_OFFSET                          (0x4CU)
#define SARADC_HIGH_COMP14_HIGH_COMP14_SHIFT               (0U)
#define SARADC_HIGH_COMP14_HIGH_COMP14_MASK                (0xFFFU << SARADC_HIGH_COMP14_HIGH_COMP14_SHIFT)             /* 0x00000FFF */
/* HIGH_COMP15 */
#define SARADC_HIGH_COMP15_OFFSET                          (0x50U)
#define SARADC_HIGH_COMP15_HIGH_COMP15_SHIFT               (0U)
#define SARADC_HIGH_COMP15_HIGH_COMP15_MASK                (0xFFFU << SARADC_HIGH_COMP15_HIGH_COMP15_SHIFT)             /* 0x00000FFF */
/* LOW_COMP0 */
#define SARADC_LOW_COMP0_OFFSET                            (0x54U)
#define SARADC_LOW_COMP0_LOW_COMP0_SHIFT                   (0U)
#define SARADC_LOW_COMP0_LOW_COMP0_MASK                    (0xFFFU << SARADC_LOW_COMP0_LOW_COMP0_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP1 */
#define SARADC_LOW_COMP1_OFFSET                            (0x58U)
#define SARADC_LOW_COMP1_LOW_COMP1_SHIFT                   (0U)
#define SARADC_LOW_COMP1_LOW_COMP1_MASK                    (0xFFFU << SARADC_LOW_COMP1_LOW_COMP1_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP2 */
#define SARADC_LOW_COMP2_OFFSET                            (0x5CU)
#define SARADC_LOW_COMP2_LOW_COMP2_SHIFT                   (0U)
#define SARADC_LOW_COMP2_LOW_COMP2_MASK                    (0xFFFU << SARADC_LOW_COMP2_LOW_COMP2_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP3 */
#define SARADC_LOW_COMP3_OFFSET                            (0x60U)
#define SARADC_LOW_COMP3_LOW_COMP3_SHIFT                   (0U)
#define SARADC_LOW_COMP3_LOW_COMP3_MASK                    (0xFFFU << SARADC_LOW_COMP3_LOW_COMP3_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP4 */
#define SARADC_LOW_COMP4_OFFSET                            (0x64U)
#define SARADC_LOW_COMP4_LOW_COMP4_SHIFT                   (0U)
#define SARADC_LOW_COMP4_LOW_COMP4_MASK                    (0xFFFU << SARADC_LOW_COMP4_LOW_COMP4_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP5 */
#define SARADC_LOW_COMP5_OFFSET                            (0x68U)
#define SARADC_LOW_COMP5_LOW_COMP5_SHIFT                   (0U)
#define SARADC_LOW_COMP5_LOW_COMP5_MASK                    (0xFFFU << SARADC_LOW_COMP5_LOW_COMP5_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP6 */
#define SARADC_LOW_COMP6_OFFSET                            (0x6CU)
#define SARADC_LOW_COMP6_LOW_COMP6_SHIFT                   (0U)
#define SARADC_LOW_COMP6_LOW_COMP6_MASK                    (0xFFFU << SARADC_LOW_COMP6_LOW_COMP6_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP7 */
#define SARADC_LOW_COMP7_OFFSET                            (0x70U)
#define SARADC_LOW_COMP7_LOW_COMP7_SHIFT                   (0U)
#define SARADC_LOW_COMP7_LOW_COMP7_MASK                    (0xFFFU << SARADC_LOW_COMP7_LOW_COMP7_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP8 */
#define SARADC_LOW_COMP8_OFFSET                            (0x74U)
#define SARADC_LOW_COMP8_LOW_COMP8_SHIFT                   (0U)
#define SARADC_LOW_COMP8_LOW_COMP8_MASK                    (0xFFFU << SARADC_LOW_COMP8_LOW_COMP8_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP9 */
#define SARADC_LOW_COMP9_OFFSET                            (0x78U)
#define SARADC_LOW_COMP9_LOW_COMP9_SHIFT                   (0U)
#define SARADC_LOW_COMP9_LOW_COMP9_MASK                    (0xFFFU << SARADC_LOW_COMP9_LOW_COMP9_SHIFT)                 /* 0x00000FFF */
/* LOW_COMP10 */
#define SARADC_LOW_COMP10_OFFSET                           (0x7CU)
#define SARADC_LOW_COMP10_LOW_COMP10_SHIFT                 (0U)
#define SARADC_LOW_COMP10_LOW_COMP10_MASK                  (0x1U << SARADC_LOW_COMP10_LOW_COMP10_SHIFT)                 /* 0x00000001 */
/* LOW_COMP11 */
#define SARADC_LOW_COMP11_OFFSET                           (0x80U)
#define SARADC_LOW_COMP11_LOW_COMP11_SHIFT                 (0U)
#define SARADC_LOW_COMP11_LOW_COMP11_MASK                  (0xFFFU << SARADC_LOW_COMP11_LOW_COMP11_SHIFT)               /* 0x00000FFF */
/* LOW_COMP12 */
#define SARADC_LOW_COMP12_OFFSET                           (0x84U)
#define SARADC_LOW_COMP12_LOW_COMP12_SHIFT                 (0U)
#define SARADC_LOW_COMP12_LOW_COMP12_MASK                  (0xFFFU << SARADC_LOW_COMP12_LOW_COMP12_SHIFT)               /* 0x00000FFF */
/* LOW_COMP13 */
#define SARADC_LOW_COMP13_OFFSET                           (0x88U)
#define SARADC_LOW_COMP13_LOW_COMP13_SHIFT                 (0U)
#define SARADC_LOW_COMP13_LOW_COMP13_MASK                  (0x1U << SARADC_LOW_COMP13_LOW_COMP13_SHIFT)                 /* 0x00000001 */
/* LOW_COMP14 */
#define SARADC_LOW_COMP14_OFFSET                           (0x8CU)
#define SARADC_LOW_COMP14_LOW_COMP14_SHIFT                 (0U)
#define SARADC_LOW_COMP14_LOW_COMP14_MASK                  (0xFFFU << SARADC_LOW_COMP14_LOW_COMP14_SHIFT)               /* 0x00000FFF */
/* LOW_COMP15 */
#define SARADC_LOW_COMP15_OFFSET                           (0x90U)
#define SARADC_LOW_COMP15_LOW_COMP15_SHIFT                 (0U)
#define SARADC_LOW_COMP15_LOW_COMP15_MASK                  (0xFFFU << SARADC_LOW_COMP15_LOW_COMP15_SHIFT)               /* 0x00000FFF */
/* DEBOUNCE */
#define SARADC_DEBOUNCE_OFFSET                             (0x94U)
#define SARADC_DEBOUNCE_DEBOUNCE_SHIFT                     (0U)
#define SARADC_DEBOUNCE_DEBOUNCE_MASK                      (0xFFU << SARADC_DEBOUNCE_DEBOUNCE_SHIFT)                    /* 0x000000FF */
/* HT_INT_EN */
#define SARADC_HT_INT_EN_OFFSET                            (0x98U)
#define SARADC_HT_INT_EN_HT_INT_EN0_SHIFT                  (0U)
#define SARADC_HT_INT_EN_HT_INT_EN0_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN0_SHIFT)                  /* 0x00000001 */
#define SARADC_HT_INT_EN_HT_INT_EN1_SHIFT                  (1U)
#define SARADC_HT_INT_EN_HT_INT_EN1_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN1_SHIFT)                  /* 0x00000002 */
#define SARADC_HT_INT_EN_HT_INT_EN2_SHIFT                  (2U)
#define SARADC_HT_INT_EN_HT_INT_EN2_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN2_SHIFT)                  /* 0x00000004 */
#define SARADC_HT_INT_EN_HT_INT_EN3_SHIFT                  (3U)
#define SARADC_HT_INT_EN_HT_INT_EN3_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN3_SHIFT)                  /* 0x00000008 */
#define SARADC_HT_INT_EN_HT_INT_EN4_SHIFT                  (4U)
#define SARADC_HT_INT_EN_HT_INT_EN4_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN4_SHIFT)                  /* 0x00000010 */
#define SARADC_HT_INT_EN_HT_INT_EN5_SHIFT                  (5U)
#define SARADC_HT_INT_EN_HT_INT_EN5_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN5_SHIFT)                  /* 0x00000020 */
#define SARADC_HT_INT_EN_HT_INT_EN6_SHIFT                  (6U)
#define SARADC_HT_INT_EN_HT_INT_EN6_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN6_SHIFT)                  /* 0x00000040 */
#define SARADC_HT_INT_EN_HT_INT_EN7_SHIFT                  (7U)
#define SARADC_HT_INT_EN_HT_INT_EN7_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN7_SHIFT)                  /* 0x00000080 */
#define SARADC_HT_INT_EN_HT_INT_EN8_SHIFT                  (8U)
#define SARADC_HT_INT_EN_HT_INT_EN8_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN8_SHIFT)                  /* 0x00000100 */
#define SARADC_HT_INT_EN_HT_INT_EN9_SHIFT                  (9U)
#define SARADC_HT_INT_EN_HT_INT_EN9_MASK                   (0x1U << SARADC_HT_INT_EN_HT_INT_EN9_SHIFT)                  /* 0x00000200 */
#define SARADC_HT_INT_EN_HT_INT_EN10_SHIFT                 (10U)
#define SARADC_HT_INT_EN_HT_INT_EN10_MASK                  (0x1U << SARADC_HT_INT_EN_HT_INT_EN10_SHIFT)                 /* 0x00000400 */
#define SARADC_HT_INT_EN_HT_INT_EN11_SHIFT                 (11U)
#define SARADC_HT_INT_EN_HT_INT_EN11_MASK                  (0x1U << SARADC_HT_INT_EN_HT_INT_EN11_SHIFT)                 /* 0x00000800 */
#define SARADC_HT_INT_EN_HT_INT_EN12_SHIFT                 (12U)
#define SARADC_HT_INT_EN_HT_INT_EN12_MASK                  (0x1U << SARADC_HT_INT_EN_HT_INT_EN12_SHIFT)                 /* 0x00001000 */
#define SARADC_HT_INT_EN_HT_INT_EN13_SHIFT                 (13U)
#define SARADC_HT_INT_EN_HT_INT_EN13_MASK                  (0x1U << SARADC_HT_INT_EN_HT_INT_EN13_SHIFT)                 /* 0x00002000 */
#define SARADC_HT_INT_EN_HT_INT_EN14_SHIFT                 (14U)
#define SARADC_HT_INT_EN_HT_INT_EN14_MASK                  (0x1U << SARADC_HT_INT_EN_HT_INT_EN14_SHIFT)                 /* 0x00004000 */
#define SARADC_HT_INT_EN_HT_INT_EN15_SHIFT                 (15U)
#define SARADC_HT_INT_EN_HT_INT_EN15_MASK                  (0x1U << SARADC_HT_INT_EN_HT_INT_EN15_SHIFT)                 /* 0x00008000 */
/* LT_INT_EN */
#define SARADC_LT_INT_EN_OFFSET                            (0x9CU)
#define SARADC_LT_INT_EN_LT_INT_EN0_SHIFT                  (0U)
#define SARADC_LT_INT_EN_LT_INT_EN0_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN0_SHIFT)                  /* 0x00000001 */
#define SARADC_LT_INT_EN_LT_INT_EN1_SHIFT                  (1U)
#define SARADC_LT_INT_EN_LT_INT_EN1_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN1_SHIFT)                  /* 0x00000002 */
#define SARADC_LT_INT_EN_LT_INT_EN2_SHIFT                  (2U)
#define SARADC_LT_INT_EN_LT_INT_EN2_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN2_SHIFT)                  /* 0x00000004 */
#define SARADC_LT_INT_EN_LT_INT_EN3_SHIFT                  (3U)
#define SARADC_LT_INT_EN_LT_INT_EN3_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN3_SHIFT)                  /* 0x00000008 */
#define SARADC_LT_INT_EN_LT_INT_EN4_SHIFT                  (4U)
#define SARADC_LT_INT_EN_LT_INT_EN4_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN4_SHIFT)                  /* 0x00000010 */
#define SARADC_LT_INT_EN_LT_INT_EN5_SHIFT                  (5U)
#define SARADC_LT_INT_EN_LT_INT_EN5_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN5_SHIFT)                  /* 0x00000020 */
#define SARADC_LT_INT_EN_LT_INT_EN6_SHIFT                  (6U)
#define SARADC_LT_INT_EN_LT_INT_EN6_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN6_SHIFT)                  /* 0x00000040 */
#define SARADC_LT_INT_EN_LT_INT_EN7_SHIFT                  (7U)
#define SARADC_LT_INT_EN_LT_INT_EN7_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN7_SHIFT)                  /* 0x00000080 */
#define SARADC_LT_INT_EN_LT_INT_EN8_SHIFT                  (8U)
#define SARADC_LT_INT_EN_LT_INT_EN8_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN8_SHIFT)                  /* 0x00000100 */
#define SARADC_LT_INT_EN_LT_INT_EN9_SHIFT                  (9U)
#define SARADC_LT_INT_EN_LT_INT_EN9_MASK                   (0x1U << SARADC_LT_INT_EN_LT_INT_EN9_SHIFT)                  /* 0x00000200 */
#define SARADC_LT_INT_EN_LT_INT_EN10_SHIFT                 (10U)
#define SARADC_LT_INT_EN_LT_INT_EN10_MASK                  (0x1U << SARADC_LT_INT_EN_LT_INT_EN10_SHIFT)                 /* 0x00000400 */
#define SARADC_LT_INT_EN_LT_INT_EN11_SHIFT                 (11U)
#define SARADC_LT_INT_EN_LT_INT_EN11_MASK                  (0x1U << SARADC_LT_INT_EN_LT_INT_EN11_SHIFT)                 /* 0x00000800 */
#define SARADC_LT_INT_EN_LT_INT_EN12_SHIFT                 (12U)
#define SARADC_LT_INT_EN_LT_INT_EN12_MASK                  (0x1U << SARADC_LT_INT_EN_LT_INT_EN12_SHIFT)                 /* 0x00001000 */
#define SARADC_LT_INT_EN_LT_INT_EN13_SHIFT                 (13U)
#define SARADC_LT_INT_EN_LT_INT_EN13_MASK                  (0x1U << SARADC_LT_INT_EN_LT_INT_EN13_SHIFT)                 /* 0x00002000 */
#define SARADC_LT_INT_EN_LT_INT_EN14_SHIFT                 (14U)
#define SARADC_LT_INT_EN_LT_INT_EN14_MASK                  (0x1U << SARADC_LT_INT_EN_LT_INT_EN14_SHIFT)                 /* 0x00004000 */
#define SARADC_LT_INT_EN_LT_INT_EN15_SHIFT                 (15U)
#define SARADC_LT_INT_EN_LT_INT_EN15_MASK                  (0x1U << SARADC_LT_INT_EN_LT_INT_EN15_SHIFT)                 /* 0x00008000 */
/* MT_INT_EN */
#define SARADC_MT_INT_EN_OFFSET                            (0x100U)
#define SARADC_MT_INT_EN_MT_INT_EN0_SHIFT                  (0U)
#define SARADC_MT_INT_EN_MT_INT_EN0_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN0_SHIFT)                  /* 0x00000001 */
#define SARADC_MT_INT_EN_MT_INT_EN1_SHIFT                  (1U)
#define SARADC_MT_INT_EN_MT_INT_EN1_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN1_SHIFT)                  /* 0x00000002 */
#define SARADC_MT_INT_EN_MT_INT_EN2_SHIFT                  (2U)
#define SARADC_MT_INT_EN_MT_INT_EN2_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN2_SHIFT)                  /* 0x00000004 */
#define SARADC_MT_INT_EN_MT_INT_EN3_SHIFT                  (3U)
#define SARADC_MT_INT_EN_MT_INT_EN3_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN3_SHIFT)                  /* 0x00000008 */
#define SARADC_MT_INT_EN_MT_INT_EN4_SHIFT                  (4U)
#define SARADC_MT_INT_EN_MT_INT_EN4_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN4_SHIFT)                  /* 0x00000010 */
#define SARADC_MT_INT_EN_MT_INT_EN5_SHIFT                  (5U)
#define SARADC_MT_INT_EN_MT_INT_EN5_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN5_SHIFT)                  /* 0x00000020 */
#define SARADC_MT_INT_EN_MT_INT_EN6_SHIFT                  (6U)
#define SARADC_MT_INT_EN_MT_INT_EN6_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN6_SHIFT)                  /* 0x00000040 */
#define SARADC_MT_INT_EN_MT_INT_EN7_SHIFT                  (7U)
#define SARADC_MT_INT_EN_MT_INT_EN7_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN7_SHIFT)                  /* 0x00000080 */
#define SARADC_MT_INT_EN_MT_INT_EN8_SHIFT                  (8U)
#define SARADC_MT_INT_EN_MT_INT_EN8_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN8_SHIFT)                  /* 0x00000100 */
#define SARADC_MT_INT_EN_MT_INT_EN9_SHIFT                  (9U)
#define SARADC_MT_INT_EN_MT_INT_EN9_MASK                   (0x1U << SARADC_MT_INT_EN_MT_INT_EN9_SHIFT)                  /* 0x00000200 */
#define SARADC_MT_INT_EN_MT_INT_EN10_SHIFT                 (10U)
#define SARADC_MT_INT_EN_MT_INT_EN10_MASK                  (0x1U << SARADC_MT_INT_EN_MT_INT_EN10_SHIFT)                 /* 0x00000400 */
#define SARADC_MT_INT_EN_MT_INT_EN11_SHIFT                 (11U)
#define SARADC_MT_INT_EN_MT_INT_EN11_MASK                  (0x1U << SARADC_MT_INT_EN_MT_INT_EN11_SHIFT)                 /* 0x00000800 */
#define SARADC_MT_INT_EN_MT_INT_EN12_SHIFT                 (12U)
#define SARADC_MT_INT_EN_MT_INT_EN12_MASK                  (0x1U << SARADC_MT_INT_EN_MT_INT_EN12_SHIFT)                 /* 0x00001000 */
#define SARADC_MT_INT_EN_MT_INT_EN13_SHIFT                 (13U)
#define SARADC_MT_INT_EN_MT_INT_EN13_MASK                  (0x1U << SARADC_MT_INT_EN_MT_INT_EN13_SHIFT)                 /* 0x00002000 */
#define SARADC_MT_INT_EN_MT_INT_EN14_SHIFT                 (14U)
#define SARADC_MT_INT_EN_MT_INT_EN14_MASK                  (0x1U << SARADC_MT_INT_EN_MT_INT_EN14_SHIFT)                 /* 0x00004000 */
#define SARADC_MT_INT_EN_MT_INT_EN15_SHIFT                 (15U)
#define SARADC_MT_INT_EN_MT_INT_EN15_MASK                  (0x1U << SARADC_MT_INT_EN_MT_INT_EN15_SHIFT)                 /* 0x00008000 */
/* END_INT_EN */
#define SARADC_END_INT_EN_OFFSET                           (0x104U)
#define SARADC_END_INT_EN_END_INT_EN_SHIFT                 (0U)
#define SARADC_END_INT_EN_END_INT_EN_MASK                  (0x1U << SARADC_END_INT_EN_END_INT_EN_SHIFT)                 /* 0x00000001 */
/* ST_CON */
#define SARADC_ST_CON_OFFSET                               (0x108U)
#define SARADC_ST_CON_CCTRL_SHIFT                          (0U)
#define SARADC_ST_CON_CCTRL_MASK                           (0x7U << SARADC_ST_CON_CCTRL_SHIFT)                          /* 0x00000007 */
#define SARADC_ST_CON_ICTRL_SHIFT                          (3U)
#define SARADC_ST_CON_ICTRL_MASK                           (0x7U << SARADC_ST_CON_ICTRL_SHIFT)                          /* 0x00000038 */
/* STATUS */
#define SARADC_STATUS_OFFSET                               (0x10CU)
#define SARADC_STATUS                                      (0x2U)
#define SARADC_STATUS_CONV_ST_SHIFT                        (0U)
#define SARADC_STATUS_CONV_ST_MASK                         (0x1U << SARADC_STATUS_CONV_ST_SHIFT)                        /* 0x00000001 */
#define SARADC_STATUS_PD_SHIFT                             (1U)
#define SARADC_STATUS_PD_MASK                              (0x1U << SARADC_STATUS_PD_SHIFT)                             /* 0x00000002 */
#define SARADC_STATUS_SEL_SHIFT                            (2U)
#define SARADC_STATUS_SEL_MASK                             (0xFU << SARADC_STATUS_SEL_SHIFT)                            /* 0x0000003C */
/* END_INT_ST */
#define SARADC_END_INT_ST_OFFSET                           (0x110U)
#define SARADC_END_INT_ST_END_INT_ST_SHIFT                 (0U)
#define SARADC_END_INT_ST_END_INT_ST_MASK                  (0x1U << SARADC_END_INT_ST_END_INT_ST_SHIFT)                 /* 0x00000001 */
/* HT_INT_ST */
#define SARADC_HT_INT_ST_OFFSET                            (0x114U)
#define SARADC_HT_INT_ST_HT_INT_ST0_SHIFT                  (0U)
#define SARADC_HT_INT_ST_HT_INT_ST0_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST0_SHIFT)                  /* 0x00000001 */
#define SARADC_HT_INT_ST_HT_INT_ST1_SHIFT                  (1U)
#define SARADC_HT_INT_ST_HT_INT_ST1_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST1_SHIFT)                  /* 0x00000002 */
#define SARADC_HT_INT_ST_HT_INT_ST2_SHIFT                  (2U)
#define SARADC_HT_INT_ST_HT_INT_ST2_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST2_SHIFT)                  /* 0x00000004 */
#define SARADC_HT_INT_ST_HT_INT_ST3_SHIFT                  (3U)
#define SARADC_HT_INT_ST_HT_INT_ST3_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST3_SHIFT)                  /* 0x00000008 */
#define SARADC_HT_INT_ST_HT_INT_ST4_SHIFT                  (4U)
#define SARADC_HT_INT_ST_HT_INT_ST4_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST4_SHIFT)                  /* 0x00000010 */
#define SARADC_HT_INT_ST_HT_INT_ST5_SHIFT                  (5U)
#define SARADC_HT_INT_ST_HT_INT_ST5_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST5_SHIFT)                  /* 0x00000020 */
#define SARADC_HT_INT_ST_HT_INT_ST6_SHIFT                  (6U)
#define SARADC_HT_INT_ST_HT_INT_ST6_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST6_SHIFT)                  /* 0x00000040 */
#define SARADC_HT_INT_ST_HT_INT_ST7_SHIFT                  (7U)
#define SARADC_HT_INT_ST_HT_INT_ST7_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST7_SHIFT)                  /* 0x00000080 */
#define SARADC_HT_INT_ST_HT_INT_ST8_SHIFT                  (8U)
#define SARADC_HT_INT_ST_HT_INT_ST8_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST8_SHIFT)                  /* 0x00000100 */
#define SARADC_HT_INT_ST_HT_INT_ST9_SHIFT                  (9U)
#define SARADC_HT_INT_ST_HT_INT_ST9_MASK                   (0x1U << SARADC_HT_INT_ST_HT_INT_ST9_SHIFT)                  /* 0x00000200 */
#define SARADC_HT_INT_ST_HT_INT_ST10_SHIFT                 (10U)
#define SARADC_HT_INT_ST_HT_INT_ST10_MASK                  (0x1U << SARADC_HT_INT_ST_HT_INT_ST10_SHIFT)                 /* 0x00000400 */
#define SARADC_HT_INT_ST_HT_INT_ST11_SHIFT                 (11U)
#define SARADC_HT_INT_ST_HT_INT_ST11_MASK                  (0x1U << SARADC_HT_INT_ST_HT_INT_ST11_SHIFT)                 /* 0x00000800 */
#define SARADC_HT_INT_ST_HT_INT_ST12_SHIFT                 (12U)
#define SARADC_HT_INT_ST_HT_INT_ST12_MASK                  (0x1U << SARADC_HT_INT_ST_HT_INT_ST12_SHIFT)                 /* 0x00001000 */
#define SARADC_HT_INT_ST_HT_INT_ST13_SHIFT                 (13U)
#define SARADC_HT_INT_ST_HT_INT_ST13_MASK                  (0x1U << SARADC_HT_INT_ST_HT_INT_ST13_SHIFT)                 /* 0x00002000 */
#define SARADC_HT_INT_ST_HT_INT_ST14_SHIFT                 (14U)
#define SARADC_HT_INT_ST_HT_INT_ST14_MASK                  (0x1U << SARADC_HT_INT_ST_HT_INT_ST14_SHIFT)                 /* 0x00004000 */
#define SARADC_HT_INT_ST_HT_INT_ST15_SHIFT                 (15U)
#define SARADC_HT_INT_ST_HT_INT_ST15_MASK                  (0x1U << SARADC_HT_INT_ST_HT_INT_ST15_SHIFT)                 /* 0x00008000 */
/* LT_INT_ST */
#define SARADC_LT_INT_ST_OFFSET                            (0x118U)
#define SARADC_LT_INT_ST_LT_INT_ST0_SHIFT                  (0U)
#define SARADC_LT_INT_ST_LT_INT_ST0_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST0_SHIFT)                  /* 0x00000001 */
#define SARADC_LT_INT_ST_LT_INT_ST1_SHIFT                  (1U)
#define SARADC_LT_INT_ST_LT_INT_ST1_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST1_SHIFT)                  /* 0x00000002 */
#define SARADC_LT_INT_ST_LT_INT_ST2_SHIFT                  (2U)
#define SARADC_LT_INT_ST_LT_INT_ST2_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST2_SHIFT)                  /* 0x00000004 */
#define SARADC_LT_INT_ST_LT_INT_ST3_SHIFT                  (3U)
#define SARADC_LT_INT_ST_LT_INT_ST3_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST3_SHIFT)                  /* 0x00000008 */
#define SARADC_LT_INT_ST_LT_INT_ST4_SHIFT                  (4U)
#define SARADC_LT_INT_ST_LT_INT_ST4_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST4_SHIFT)                  /* 0x00000010 */
#define SARADC_LT_INT_ST_LT_INT_ST5_SHIFT                  (5U)
#define SARADC_LT_INT_ST_LT_INT_ST5_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST5_SHIFT)                  /* 0x00000020 */
#define SARADC_LT_INT_ST_LT_INT_ST6_SHIFT                  (6U)
#define SARADC_LT_INT_ST_LT_INT_ST6_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST6_SHIFT)                  /* 0x00000040 */
#define SARADC_LT_INT_ST_LT_INT_ST7_SHIFT                  (7U)
#define SARADC_LT_INT_ST_LT_INT_ST7_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST7_SHIFT)                  /* 0x00000080 */
#define SARADC_LT_INT_ST_LT_INT_ST8_SHIFT                  (8U)
#define SARADC_LT_INT_ST_LT_INT_ST8_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST8_SHIFT)                  /* 0x00000100 */
#define SARADC_LT_INT_ST_LT_INT_ST9_SHIFT                  (9U)
#define SARADC_LT_INT_ST_LT_INT_ST9_MASK                   (0x1U << SARADC_LT_INT_ST_LT_INT_ST9_SHIFT)                  /* 0x00000200 */
#define SARADC_LT_INT_ST_LT_INT_ST10_SHIFT                 (10U)
#define SARADC_LT_INT_ST_LT_INT_ST10_MASK                  (0x1U << SARADC_LT_INT_ST_LT_INT_ST10_SHIFT)                 /* 0x00000400 */
#define SARADC_LT_INT_ST_LT_INT_ST11_SHIFT                 (11U)
#define SARADC_LT_INT_ST_LT_INT_ST11_MASK                  (0x1U << SARADC_LT_INT_ST_LT_INT_ST11_SHIFT)                 /* 0x00000800 */
#define SARADC_LT_INT_ST_LT_INT_ST12_SHIFT                 (12U)
#define SARADC_LT_INT_ST_LT_INT_ST12_MASK                  (0x1U << SARADC_LT_INT_ST_LT_INT_ST12_SHIFT)                 /* 0x00001000 */
#define SARADC_LT_INT_ST_LT_INT_ST13_SHIFT                 (13U)
#define SARADC_LT_INT_ST_LT_INT_ST13_MASK                  (0x1U << SARADC_LT_INT_ST_LT_INT_ST13_SHIFT)                 /* 0x00002000 */
#define SARADC_LT_INT_ST_LT_INT_ST14_SHIFT                 (14U)
#define SARADC_LT_INT_ST_LT_INT_ST14_MASK                  (0x1U << SARADC_LT_INT_ST_LT_INT_ST14_SHIFT)                 /* 0x00004000 */
#define SARADC_LT_INT_ST_LT_INT_ST15_SHIFT                 (15U)
#define SARADC_LT_INT_ST_LT_INT_ST15_MASK                  (0x1U << SARADC_LT_INT_ST_LT_INT_ST15_SHIFT)                 /* 0x00008000 */
/* MT_INT_ST */
#define SARADC_MT_INT_ST_OFFSET                            (0x11CU)
#define SARADC_MT_INT_ST_MT_INT_ST0_SHIFT                  (0U)
#define SARADC_MT_INT_ST_MT_INT_ST0_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST0_SHIFT)                  /* 0x00000001 */
#define SARADC_MT_INT_ST_MT_INT_ST1_SHIFT                  (1U)
#define SARADC_MT_INT_ST_MT_INT_ST1_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST1_SHIFT)                  /* 0x00000002 */
#define SARADC_MT_INT_ST_MT_INT_ST2_SHIFT                  (2U)
#define SARADC_MT_INT_ST_MT_INT_ST2_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST2_SHIFT)                  /* 0x00000004 */
#define SARADC_MT_INT_ST_MT_INT_ST3_SHIFT                  (3U)
#define SARADC_MT_INT_ST_MT_INT_ST3_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST3_SHIFT)                  /* 0x00000008 */
#define SARADC_MT_INT_ST_MT_INT_ST4_SHIFT                  (4U)
#define SARADC_MT_INT_ST_MT_INT_ST4_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST4_SHIFT)                  /* 0x00000010 */
#define SARADC_MT_INT_ST_MT_INT_ST5_SHIFT                  (5U)
#define SARADC_MT_INT_ST_MT_INT_ST5_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST5_SHIFT)                  /* 0x00000020 */
#define SARADC_MT_INT_ST_MT_INT_ST6_SHIFT                  (6U)
#define SARADC_MT_INT_ST_MT_INT_ST6_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST6_SHIFT)                  /* 0x00000040 */
#define SARADC_MT_INT_ST_MT_INT_ST7_SHIFT                  (7U)
#define SARADC_MT_INT_ST_MT_INT_ST7_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST7_SHIFT)                  /* 0x00000080 */
#define SARADC_MT_INT_ST_MT_INT_ST8_SHIFT                  (8U)
#define SARADC_MT_INT_ST_MT_INT_ST8_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST8_SHIFT)                  /* 0x00000100 */
#define SARADC_MT_INT_ST_MT_INT_ST9_SHIFT                  (9U)
#define SARADC_MT_INT_ST_MT_INT_ST9_MASK                   (0x1U << SARADC_MT_INT_ST_MT_INT_ST9_SHIFT)                  /* 0x00000200 */
#define SARADC_MT_INT_ST_MT_INT_ST10_SHIFT                 (10U)
#define SARADC_MT_INT_ST_MT_INT_ST10_MASK                  (0x1U << SARADC_MT_INT_ST_MT_INT_ST10_SHIFT)                 /* 0x00000400 */
#define SARADC_MT_INT_ST_MT_INT_ST11_SHIFT                 (11U)
#define SARADC_MT_INT_ST_MT_INT_ST11_MASK                  (0x1U << SARADC_MT_INT_ST_MT_INT_ST11_SHIFT)                 /* 0x00000800 */
#define SARADC_MT_INT_ST_MT_INT_ST12_SHIFT                 (12U)
#define SARADC_MT_INT_ST_MT_INT_ST12_MASK                  (0x1U << SARADC_MT_INT_ST_MT_INT_ST12_SHIFT)                 /* 0x00001000 */
#define SARADC_MT_INT_ST_MT_INT_ST13_SHIFT                 (13U)
#define SARADC_MT_INT_ST_MT_INT_ST13_MASK                  (0x1U << SARADC_MT_INT_ST_MT_INT_ST13_SHIFT)                 /* 0x00002000 */
#define SARADC_MT_INT_ST_MT_INT_ST14_SHIFT                 (14U)
#define SARADC_MT_INT_ST_MT_INT_ST14_MASK                  (0x1U << SARADC_MT_INT_ST_MT_INT_ST14_SHIFT)                 /* 0x00004000 */
#define SARADC_MT_INT_ST_MT_INT_ST15_SHIFT                 (15U)
#define SARADC_MT_INT_ST_MT_INT_ST15_MASK                  (0x1U << SARADC_MT_INT_ST_MT_INT_ST15_SHIFT)                 /* 0x00008000 */
/* DATA0 */
#define SARADC_DATA0_OFFSET                                (0x120U)
#define SARADC_DATA0                                       (0x0U)
#define SARADC_DATA0_DATA0_SHIFT                           (0U)
#define SARADC_DATA0_DATA0_MASK                            (0xFFFU << SARADC_DATA0_DATA0_SHIFT)                         /* 0x00000FFF */
/* DATA1 */
#define SARADC_DATA1_OFFSET                                (0x124U)
#define SARADC_DATA1                                       (0x0U)
#define SARADC_DATA1_DATA1_SHIFT                           (0U)
#define SARADC_DATA1_DATA1_MASK                            (0xFFFU << SARADC_DATA1_DATA1_SHIFT)                         /* 0x00000FFF */
/* DATA2 */
#define SARADC_DATA2_OFFSET                                (0x128U)
#define SARADC_DATA2                                       (0x0U)
#define SARADC_DATA2_DATA2_SHIFT                           (0U)
#define SARADC_DATA2_DATA2_MASK                            (0xFFFU << SARADC_DATA2_DATA2_SHIFT)                         /* 0x00000FFF */
/* DATA3 */
#define SARADC_DATA3_OFFSET                                (0x12CU)
#define SARADC_DATA3                                       (0x0U)
#define SARADC_DATA3_DATA3_SHIFT                           (0U)
#define SARADC_DATA3_DATA3_MASK                            (0xFFFU << SARADC_DATA3_DATA3_SHIFT)                         /* 0x00000FFF */
/* DATA4 */
#define SARADC_DATA4_OFFSET                                (0x130U)
#define SARADC_DATA4                                       (0x0U)
#define SARADC_DATA4_DATA4_SHIFT                           (0U)
#define SARADC_DATA4_DATA4_MASK                            (0xFFFU << SARADC_DATA4_DATA4_SHIFT)                         /* 0x00000FFF */
/* DATA5 */
#define SARADC_DATA5_OFFSET                                (0x134U)
#define SARADC_DATA5                                       (0x0U)
#define SARADC_DATA5_DATA5_SHIFT                           (0U)
#define SARADC_DATA5_DATA5_MASK                            (0xFFFU << SARADC_DATA5_DATA5_SHIFT)                         /* 0x00000FFF */
/* DATA6 */
#define SARADC_DATA6_OFFSET                                (0x138U)
#define SARADC_DATA6                                       (0x0U)
#define SARADC_DATA6_DATA6_SHIFT                           (0U)
#define SARADC_DATA6_DATA6_MASK                            (0xFFFU << SARADC_DATA6_DATA6_SHIFT)                         /* 0x00000FFF */
/* DATA7 */
#define SARADC_DATA7_OFFSET                                (0x13CU)
#define SARADC_DATA7                                       (0x0U)
#define SARADC_DATA7_DATA7_SHIFT                           (0U)
#define SARADC_DATA7_DATA7_MASK                            (0xFFFU << SARADC_DATA7_DATA7_SHIFT)                         /* 0x00000FFF */
/* DATA8 */
#define SARADC_DATA8_OFFSET                                (0x140U)
#define SARADC_DATA8                                       (0x0U)
#define SARADC_DATA8_DATA8_SHIFT                           (0U)
#define SARADC_DATA8_DATA8_MASK                            (0xFFFU << SARADC_DATA8_DATA8_SHIFT)                         /* 0x00000FFF */
/* DATA9 */
#define SARADC_DATA9_OFFSET                                (0x144U)
#define SARADC_DATA9                                       (0x0U)
#define SARADC_DATA9_DATA9_SHIFT                           (0U)
#define SARADC_DATA9_DATA9_MASK                            (0xFFFU << SARADC_DATA9_DATA9_SHIFT)                         /* 0x00000FFF */
/* DATA10 */
#define SARADC_DATA10_OFFSET                               (0x148U)
#define SARADC_DATA10                                      (0x0U)
#define SARADC_DATA10_DATA10_SHIFT                         (0U)
#define SARADC_DATA10_DATA10_MASK                          (0xFFFU << SARADC_DATA10_DATA10_SHIFT)                       /* 0x00000FFF */
/* DATA11 */
#define SARADC_DATA11_OFFSET                               (0x14CU)
#define SARADC_DATA11                                      (0x0U)
#define SARADC_DATA11_DATA11_SHIFT                         (0U)
#define SARADC_DATA11_DATA11_MASK                          (0xFFFU << SARADC_DATA11_DATA11_SHIFT)                       /* 0x00000FFF */
/* DATA12 */
#define SARADC_DATA12_OFFSET                               (0x150U)
#define SARADC_DATA12                                      (0x0U)
#define SARADC_DATA12_DATA12_SHIFT                         (0U)
#define SARADC_DATA12_DATA12_MASK                          (0xFFFU << SARADC_DATA12_DATA12_SHIFT)                       /* 0x00000FFF */
/* DATA13 */
#define SARADC_DATA13_OFFSET                               (0x154U)
#define SARADC_DATA13                                      (0x0U)
#define SARADC_DATA13_DATA13_SHIFT                         (0U)
#define SARADC_DATA13_DATA13_MASK                          (0xFFFU << SARADC_DATA13_DATA13_SHIFT)                       /* 0x00000FFF */
/* DATA14 */
#define SARADC_DATA14_OFFSET                               (0x158U)
#define SARADC_DATA14                                      (0x0U)
#define SARADC_DATA14_DATA14_SHIFT                         (0U)
#define SARADC_DATA14_DATA14_MASK                          (0xFFFU << SARADC_DATA14_DATA14_SHIFT)                       /* 0x00000FFF */
/* DATA15 */
#define SARADC_DATA15_OFFSET                               (0x15CU)
#define SARADC_DATA15                                      (0x0U)
#define SARADC_DATA15_DATA15_SHIFT                         (0U)
#define SARADC_DATA15_DATA15_MASK                          (0xFFFU << SARADC_DATA15_DATA15_SHIFT)                       /* 0x00000FFF */
/* AUTO_CH_EN */
#define SARADC_AUTO_CH_EN_OFFSET                           (0x160U)
#define SARADC_AUTO_CH_EN_AUTO_CH_EN_SHIFT                 (0U)
#define SARADC_AUTO_CH_EN_AUTO_CH_EN_MASK                  (0xFFFFU << SARADC_AUTO_CH_EN_AUTO_CH_EN_SHIFT)              /* 0x0000FFFF */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __RV1103B_H */
