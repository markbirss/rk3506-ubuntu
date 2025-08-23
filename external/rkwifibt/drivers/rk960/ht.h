/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_HT_H_INCLUDED
#define RK960_HT_H_INCLUDED

#include <net/mac80211.h>

struct rk960_ht_info {
	struct ieee80211_sta_ht_cap ht_cap;
	enum nl80211_channel_type channel_type;
	u16 operation_mode;
};

static inline int rk960_is_ht(const struct rk960_ht_info *ht_info)
{
	return ht_info->channel_type != NL80211_CHAN_NO_HT;
}

static inline int rk960_ht_greenfield(const struct rk960_ht_info *ht_info)
{
	return rk960_is_ht(ht_info) &&
	    (ht_info->ht_cap.cap & IEEE80211_HT_CAP_GRN_FLD) &&
	    !(ht_info->operation_mode & IEEE80211_HT_OP_MODE_NON_GF_STA_PRSNT);
}

static inline int rk960_ht_ampdu_density(const struct rk960_ht_info *ht_info)
{
	if (!rk960_is_ht(ht_info))
		return 0;
	return ht_info->ht_cap.ampdu_density;
}

#endif /* RK960_HT_H_INCLUDED */
