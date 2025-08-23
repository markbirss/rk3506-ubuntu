/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef RK960_BH_H
#define RK960_BH_H

/* extern */ struct rk960_common;

int rk960_register_bh(struct rk960_common *hw_priv);
void rk960_unregister_bh(struct rk960_common *hw_priv);
void rk960_irq_handler(struct rk960_common *hw_priv);
void rk960_bh_wakeup(struct rk960_common *hw_priv);
int rk960_bh_suspend(struct rk960_common *hw_priv);
int rk960_bh_resume(struct rk960_common *hw_priv);
int rk960_bh_read_ctrl_reg(struct rk960_common *priv, u16 * ctrl_reg);
/* Must be called from BH thread. */
void rk960_enable_powersave(struct rk960_vif *priv, bool enable);
int wsm_release_tx_buffer(struct rk960_common *hw_priv, int count);
int wsm_release_vif_tx_buffer(struct rk960_common *hw_priv, int if_id,
			      int count);

#endif /* RK960_BH_H */
