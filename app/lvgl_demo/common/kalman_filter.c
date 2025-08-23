#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "kalman_filter.h"

void KalmanInit(KalmanFilter *kf, double initial_state, double initial_error,
                double process_noise, double measurement_noise)
{
    kf->v = initial_state;
    kf->P = initial_error;
    kf->Q = process_noise;
    kf->R = measurement_noise;
}

void KalmanUpdate(KalmanFilter *kf, double measurement, int first)
{
    if (first)
        KalmanInit(kf, measurement, 1, 0.1, 0.1);

    kf->v += kf->P * kf->Q;
    kf->P += kf->P * kf->Q + kf->Q * kf->P;

    kf->K = kf->P / (kf->P + kf->R);
    kf->v = kf->v + kf->K * (measurement - kf->v);
    kf->P = (1 - kf->K) * kf->P;
}

