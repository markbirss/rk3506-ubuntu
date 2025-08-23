/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    rk3568_evb1.c
  * @version V0.1
  * @brief   wfd vop2 board configs driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-08-15     Damon Ding      first implementation
  *
  ******************************************************************************
  */

#include <wfd.h>
#include <wfd_rockchip.h>

#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <drm/connector.h>
#include <drm/drm_modes.h>
#include <drm/media-bus-format.h>

#include "simple_panel.h"

/* Helper function(s) */
static const struct mode *wfd_timing_to_mode(const struct wfdmode_timing *timing)
{
    char *p = (char *)timing;
    if (p)
    {
        p -= offsetof(struct mode, timing);
    }
    return (const struct mode *)p;
}

static const struct wfd_keyval *wfd_get_ext_from_list(const struct wfd_keyval *ext_list, const char *key)
{
    while (ext_list)
    {
        if (!ext_list->key)
        {
            ext_list = NULL;
            break;
        }
        else if (strcmp(ext_list->key, key) == 0)
        {
            return ext_list;
        }
        ++ext_list;
    }

    return NULL;
}

static const struct wfd_keyval nc_port_ext[] =
{
    { /* marks end of list */
        .key = NULL,
        .i = 0,
        .p = NULL,
    },
};

struct pwm_bl_config edp_panel_backlight_config =
{
    .dev_name = "pwm1",
    .channel = 1,
    .duty = 8000,
    .period = 10000,
    .polarity = 0,
};

struct panel_delay_config edp_panel_delay_config =
{
    .prepare = 120,
    .unprepare = 120,
    .enable = 120,
    .disable = 120,
    .reset = 10,
    .init = 10,
};

struct panel_funcs edp_panel_funcs =
{
    .init = simple_panel_init,
    .prepare = simple_panel_prepare,
    .unprepare = simple_panel_unprepare,
    .enable = simple_panel_enable,
    .disable = simple_panel_disable,
};

struct panel edp_panel =
{
    .bus_format = MEDIA_BUS_FMT_RGB888_1X24,
    .bpc = 8,
    .funcs = &edp_panel_funcs,
};

struct panel_control_pin edp_panel_enable_pin =
{
    .bank_pin = BANK_PIN(GPIO_BANK0, 23),
    .bank_id = GPIO_BANK0,
    .pin_id = GPIO_PIN_C7,
    .active_state = PANEL_GPIO_ACTIVE_HIGH,
};

static const struct wfd_keyval edp_ext[] =
{
    {
        .key = WFD_OPT_OUTPUT_IF,
        .i = VOP_OUTPUT_IF_eDP0,
        .p = (struct connector[])
        {
            {
                .connector_type = DRM_MODE_CONNECTOR_eDP,
                .id = 0,
                .panel = &edp_panel,
            },
        },
    },
    {
        .key = WFD_OPT_PANEL_BACKLIGHT,
        .i = 0,
        .p = &edp_panel_backlight_config,
    },
    {
        .key = WFD_OPT_PANEL_ENABLE_PIN,
        .i = 0,
        .p = &edp_panel_enable_pin,
    },
    {
        .key = WFD_OPT_PANEL_DELAY_CONFIG,
        .i = 0,
        .p = &edp_panel_delay_config,
    },
    { /* marks end of list */
        .key = NULL,
        .i = 0,
        .p = NULL,
    },
};

static const struct mode edp_modes[] =
{
    {
        .timing = {
            .pixel_clock_kHz = 200000,
            .hpixels = 1536, .hbp = 48, .hsw = 16, .hfp = 12,
            .vlines  = 2048, .vbp =  8, .vsw =  4, .vfp =  8,
            .flags = WFDMODE_PREFERRED,
        },
        .ext_list = NULL,
    },
    {
        // marks end of list
        .timing = {.pixel_clock_kHz = 0},
    },
};

unsigned int vop2_pipeline_binding[ROCKCHIP_MAX_CRTC] =
{
    /* port 1 */
    (1 << ROCKCHIP_VOP2_CLUSTER0) | (1 << ROCKCHIP_VOP2_ESMART0) | (1 << ROCKCHIP_VOP2_SMART0),
    /* port 2 */
    (1 << ROCKCHIP_VOP2_CLUSTER1) | (1 << ROCKCHIP_VOP2_SMART1),
    /* port 3 */
    (1 << ROCKCHIP_VOP2_ESMART1)
};

static const struct wfd_keyval vop2_ext[] =
{
    {
        .key = WFD_OPT_WFD_PORT_NUMBER,
        .i = 3,
        .p = NULL,
    },
    {
        .key = WFD_OPT_WFD_PIPELINE_NUMBER,
        .i = 6,
        .p = NULL,
    },
    {
        .key = WFD_OPT_WFD_PIPELINE_BINDING,
        .i = 0,
        .p = vop2_pipeline_binding,
    },
    { NULL }  // marks end of list
};

static const struct wfd_keyval port_ext[] =
{
    { "device_ext_example", .i = 1 },
    { NULL }  // marks end of list
};

static const struct mode sample_timings[] =
{
    {
        // 800x480 @ 60 Hz
        .timing = {
            .pixel_clock_kHz =  29760,
            .hpixels =  800, .hfp = 24, .hsw = 72, .hbp = 96, //  992 total
            .vlines  =  480, .vfp =  3, .vsw = 10, .vbp =  7, //  500 total
            .flags = WFDMODE_INVERT_HSYNC,
        },
        .ext_list = (const struct wfd_keyval[])
        {
            { "ext_1_example", .i = 1 },
            { "ext_2_example", .i = 2 },
            { NULL }  // marks end of list
        },
    },
    {
        // 1024x768 @ 60 Hz (CVT)
        .timing = {
            .pixel_clock_kHz =  63500,
            .hpixels = 1024, .hfp = 48, .hsw = 104, .hbp = 152, // 1328 total
            .vlines  =  768, .vfp =  3, .vsw =  4, .vbp = 23, //  798 total
            .flags = WFDMODE_INVERT_VSYNC,
        },
        .ext_list = NULL,
    },
    {
        // 1280x1024 @ 60 Hz (CVT)
        .timing = {
            .pixel_clock_kHz = 109000,
            .hpixels = 1280, .hfp = 88, .hsw = 128, .hbp = 216, // 1712 total
            .vlines  = 1024, .vfp =  3, .vsw =  7, .vbp = 29, // 1063 total
            .flags = WFDMODE_INVERT_VSYNC,
        },
        .ext_list = NULL,
    },
    {
        // 1080p @ 60 Hz (1920x1080)
        .timing = {
            .pixel_clock_kHz = 148500,
            .hpixels = 1920, .hfp = 88, .hsw = 44, .hbp = 148, // 2200 total
            .vlines  = 1080, .vfp =  4, .vsw =  5, .vbp = 36, // 1125 total
            .flags = 0,
        },
        .ext_list = NULL,
    },
    {
        // 720p @ 60 Hz (1280x720)
        .timing = {
            .pixel_clock_kHz =  74250,
            .hpixels = 1280, .hfp = 110, .hsw = 40, .hbp = 220, // 1650 total
            .vlines  =  720, .vfp =  5, .vsw =  5, .vbp = 20, //  750 total
            .flags = 0,
        },
        .ext_list = NULL,
    },
    {
        // 800p @ 60 Hz (1280x800)
        .timing = {
            .pixel_clock_kHz =  71000,
            .hpixels = 1280, .hfp = 48, .hsw = 32, .hbp = 80, // 1440 total
            .vlines  =  800, .vfp = 3, .vsw =  6, .vbp = 14, //  823 total
            .flags = 0,
        },
        .ext_list = NULL,
    },
    {
        // marks end of list
        .timing = {.pixel_clock_kHz = 0},
    },
};

int wfddevice_cfg_create(struct wfddevice_cfg **device_cfg, int deviceid, const struct wfd_keyval *opts)
{
    int err = EOK;
    struct wfddevice_cfg *tmp_dev = NULL;

    if (!device_cfg)
        return ENODEV;

    if (*device_cfg)
        return EOK;

    switch (deviceid)
    {
    case 1:
        tmp_dev = dss_calloc(1, sizeof(*tmp_dev));
        if (!tmp_dev)
        {
            err = ENOMEM;
            goto end;
        }
        *tmp_dev = (struct wfddevice_cfg)
        {
            .ext_list = vop2_ext,
        };
        break;
    default:
        /* Invalid device_cfg id*/
        err = ENOENT;
        goto end;
    }

end:
    if (err)
    {
        dss_free(tmp_dev);
    }
    else
    {
        *device_cfg = tmp_dev;
    }

    return err;

}

const struct wfd_keyval *wfddevice_cfg_get_extension(const struct wfddevice_cfg *device_cfg,
        const char *key)
{
    return wfd_get_ext_from_list(device_cfg->ext_list, key);
}

void wfddevice_cfg_destroy(struct wfddevice_cfg *device_cfg)
{
    free(device_cfg);
}

int wfdport_cfg_create(struct wfdport_cfg **port,
                       const struct wfddevice_cfg *device_cfg, int portid,
                       const struct wfd_keyval *opts)
{
    struct wfdport_cfg *tmp_port = NULL;
    struct port_cfg *port_cfg;
    int vp_id = portid - 1;
    int err = EOK;

    if (!device_cfg)
        return ENODEV;

    if (!portid)
    {
        return ENOENT;
    }

    port_cfg = dss_calloc(1, sizeof(*port_cfg));
    if (!port_cfg)
    {
        err = ENOMEM;
        goto end;
    }
    tmp_port = &port_cfg->base;
    tmp_port->id = portid;
    tmp_port->ext_list = nc_port_ext;

    switch (vp_id)
    {
    case ROCKCHIP_VP0:
        tmp_port->ext_list = edp_ext;
        tmp_port->first_mode = &edp_modes[0];
        printf("Display %d -> VP%d -> EDP\n", portid, vp_id);
        break;
    case ROCKCHIP_VP1:
        tmp_port->ext_list = port_ext;
        tmp_port->first_mode = &sample_timings[0];
        printf("Display %d -> VP%d -> SAMPLE\n", portid, vp_id);
        break;
    case ROCKCHIP_VP2:
        tmp_port->ext_list = port_ext;
        tmp_port->first_mode = &sample_timings[0];
        printf("Display %d -> VP%d -> SAMPLE\n", portid, vp_id);
        break;
    default:
        printf("Display %d -> Invalid bound VP%d. This port is not usable or not exist\n", portid, vp_id);
        err = ENOENT;
        break;
    }

end:
    if (err)
    {
        dss_free(port_cfg);
    }
    else
    {
        *port = tmp_port;
    }

    return err;
}

const struct wfd_keyval *wfdport_cfg_get_extension(const struct wfdport_cfg *port, const char *key)
{
    return wfd_get_ext_from_list(port->ext_list, key);
}

void wfdport_cfg_destroy(struct wfdport_cfg *port)
{
    struct port_cfg *port_cfg = wfdport_cfg_to_port_cfg(port);
    dss_free(port_cfg);
}

int wfdmode_cfg_list_create(struct wfdmode_cfg_list **mode_list,
                            const struct wfdport_cfg *port, const struct wfd_keyval *opts)
{
    int err = 0;
    struct wfdmode_cfg_list *tmp_mode_list = NULL;

    assert(port);

    tmp_mode_list = malloc(sizeof * tmp_mode_list);
    if (!tmp_mode_list)
    {
        err = ENOMEM;
        goto out;
    }
    *tmp_mode_list = (struct wfdmode_cfg_list)
    {
        .port = port,
        .ext_list = NULL,
    };
out:
    if (err)
    {
        free(tmp_mode_list);
    }
    else
    {
        *mode_list = tmp_mode_list;
    }
    return err;
}

const struct wfd_keyval *wfdmode_cfg_list_get_extension(const struct wfdmode_cfg_list *mode_list,
        const char *key)
{
    return wfd_get_ext_from_list(mode_list->ext_list, key);
}

void wfdmode_cfg_list_destroy(struct wfdmode_cfg_list *mode_list)
{
    free(mode_list);
}

const struct wfdmode_timing *wfdmode_cfg_list_get_next(const struct wfdmode_cfg_list *mode_list,
        const struct wfdmode_timing *prev_timing)
{
    assert(mode_list);

    const struct mode *m = mode_list->port->first_mode;
    if (prev_timing)
    {
        m = wfd_timing_to_mode(prev_timing) + 1;
    }

    if (m && m->timing.pixel_clock_kHz == 0)
    {
        m = NULL;  // end of list
    }
    return m ? &m->timing : NULL;
}

const struct wfd_keyval *wfdmode_cfg_get_extension(const struct wfdmode_timing *timing, const char *key)
{
    const struct wfd_keyval *ext = wfd_timing_to_mode(timing)->ext_list;
    return wfd_get_ext_from_list(ext, key);
}
