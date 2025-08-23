/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef PM_H_INCLUDED
#define PM_H_INCLUDED

#ifdef CONFIG_WAKELOCK
#include <linux/wakelock.h>
#endif

/* ******************************************************************** */
/* mac80211 API								*/

#ifdef CONFIG_PM

/* extern */ struct rk960_common;
/* private */ struct rk960_suspend_state;

struct rk960_pm_state {
#ifdef CONFIG_WAKELOCK
	struct wake_lock wakelock;
#else
	struct timer_list stay_awake;
#endif
	struct platform_device *pm_dev;
	spinlock_t lock;
};

struct rk960_pm_state_vif {
	struct rk960_suspend_state *suspend_state;
};

int rk960_pm_init(struct rk960_pm_state *pm, struct rk960_common *priv);
void rk960_pm_deinit(struct rk960_pm_state *pm);
void rk960_pm_stay_awake(struct rk960_pm_state *pm, unsigned long tmo);
int rk960_wow_suspend(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan);
int rk960_wow_resume(struct ieee80211_hw *hw);
int rk960_can_suspend(struct rk960_common *priv);
int rk960_device_wakeup(struct rk960_common *hw_priv, int wake);
int rk960_suspend(struct rk960_common *hw_priv);
int rk960_resume(struct rk960_common *hw_priv);

#endif /* CONFIG_PM */

#endif
