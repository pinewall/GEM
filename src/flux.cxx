#include <math.h>
const _ValueType missing_value = 0.0;
const _ValueType umin = 0.5;
const _ValueType zref = 10.0;
const _ValueType ztref = 2.0;

/*
    nMax  ! data vector length
    mask (nMax) ! ocn domain mask       0 <=> out of domain
    zbot (nMax) ! atm level height      (m)
    ubot (nMax) ! atm u wind            (m/s)
    vbot (nMax) ! atm v wind            (m/s)
    thbot(nMax) ! atm potential T       (K)
    qbot (nMax) ! atm specific humidity (kg/kg)
    rbot (nMax) ! atm air density       (kg/m^3)
    tbot (nMax) ! atm T                 (K) 
    us   (nMax) ! ocn u-velocity        (m/s)
    vs   (nMax) ! ocn v-velocity        (m/s)
    ts   (nMax) ! ocn temperature       (K)
   */
void ocn_surface_stress (int size, _ValueType * taux, _ValueType * tauy, const int * ocn_mask, const _ValueType * atm_level_height, const _ValueType * atm_uwind, const _ValueType * atm_vwind, const _ValueType * atm_potential_T, const _ValueType * atm_humidity, const _ValueType * atm_air_density, const _ValueType * atm_T, const _ValueType * ocn_u_velocity, const _ValueType * ocn_v_velocity, const _ValueType * ocn_T)
{
    _ValueType vmag;
    _ValueType rd;
    _ValueType rdn;
    _ValueType ustar;
    _ValueType tau;
    for (int i = 0; i < size; i ++)
    {
        if (ocn_mask[i] > 0.5)
        {
            vmag = sqrt ( (atm_uwind[i] - ocn_u_velocity[i]) * (atm_uwind[i] - ocn_u_velocity[i]) + (atm_vwind[i] - ocn_v_velocity[i]) * (atm_vwind[i] - ocn_v_velocity[i]) );
            vmag = (vmag > umin) ? vmag : umin;
            thvbot = atm_potential_T[i] * (1.0 + shr_const_zvir * atm_humidity[i]);
            // TODO
            rd = rdn / (1.0 + rdn / shr_const_karman * (alz - psimh));
            ustar = rd * vmag;
            tau = atm_air_density[i] * ustar * ustar;
            taux[i] = tau * (atm_uwind[i] - ocn_u_velocity[i]) / vmag;
            tauy[i] = tau * (atm_vwind[i] - ocn_v_velocity[i]) / vmag;
        }
        else
        {
            taux[i] = missing_value;
        }
    }
}
