#ifndef FLUX_H
#define FLUX_H

void ocn_surface_stress (int size, double * taux, double * tauy, const int * ocn_mask, double * atm_level_height, double * atm_uwind, double * atm_vwind, double * atm_air_density, double * atm_T, double * ocn_u_velocity, double * ocn_v_velocity, double * ocn_T);

#endif
