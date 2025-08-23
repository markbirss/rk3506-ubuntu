
#include "rtthread.h"
#include "hal_base.h"
#include "mhd_api.h"
#include "../../components/drivers/wlan/wlan_mgnt.h"

static void bcm_init(int argc, char **argv)
{
    mhd_gpio_config(0, GPIO_BANK3, 16);  //WL_REGON on GPIO_BANK3 with GPIO_PIN_C0
    mhd_gpio_config(1, GPIO_BANK3, 15);  //WL_HWOOB on GPIO_BANK3 with GPIO_PIN_B7
    mhd_set_country_code(MHD_MK_CNTRY('C', 'N', 0));
    mhd_module_init();
}
MSH_CMD_EXPORT(bcm_init, bcm wifi driver init);

static void bcm_wifi(int argc, char **argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_wifi_commands(argc, argv);
}
MSH_CMD_EXPORT(bcm_wifi, bcm wifi command);

static void wl(int argc, char **argv)
{
    //rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    mhd_wl_cmd(argc, argv);
}
MSH_CMD_EXPORT(wl, bcm wl command);

static void wifi_sta(int argc, char **argv)
{
    rt_wlan_set_mode("wlan0", RT_WLAN_STATION);
}
MSH_CMD_EXPORT(wifi_sta, wifi sta mode);

static void wifi_ap(int argc, char **argv)
{
    rt_wlan_set_mode("wlan1", RT_WLAN_AP);
}
MSH_CMD_EXPORT(wifi_ap, wifi ap mode);

