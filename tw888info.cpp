#include "tw888info.h"

TW888Info::TW888Info(QueryInfo *parent) : QueryInfo(parent)
{

}

TW888Info::TW888Info(const TW888Info& c)
{
    area_id = c.area_id;
    machine_id = c.machine_id;
    rksj = c.rksj;

    for(int i=0;i<3;i++)
    {
        vcp.u_rms[i]     = c.vcp.u_rms[i];
        vcp.u_cf[i]      = c.vcp.u_cf[i];
        vcp.u_cf[i]      = c.vcp.u_cf[i];
        vcp.u_cf[i]      = c.vcp.u_cf[i];
        vcp.u_bias[i]    = c.vcp.u_bias[i];
        vcp.i_rms[i]     = c.vcp.i_rms[i];
        vcp.i_cf[i]      = c.vcp.i_cf[i];
        vcp.i_wf[i]      = c.vcp.i_wf[i];
        vcp.i_thd[i]     = c.vcp.i_thd[i];
        vcp.p_act[i]     = c.vcp.p_act[i];
        vcp.p_pf[i]      = c.vcp.p_pf[i];
        vcp.p_z[i]       = c.vcp.p_z[i];
    }

    vcp.u_balance = c.vcp.u_balance;
    vcp.u_df = c.vcp.u_df;

    vcp.i_balance = c.vcp.i_balance;
    vcp.i_df = c.vcp.i_df;

    vcp.p_tp = c.vcp.p_tp;
    vcp.p_d = c.vcp.p_d;
    vcp.p_load = c.vcp.p_load;

    for(int i=0;i<3;i++)
    {
        harmonic.v_thd[i]    = c.harmonic.v_thd[i];
        harmonic.v_odd[i]    = c.harmonic.v_odd[i];
        harmonic.v_even[i]   = c.harmonic.v_even[i];
        harmonic.v_p_vel[i]  = c.harmonic.v_p_vel[i];
        harmonic.v_n_vel[i]  = c.harmonic.v_n_vel[i];
        harmonic.i_thd[i]    = c.harmonic.i_thd[i];
        harmonic.i_odd[i]    = c.harmonic.i_odd[i];
        harmonic.i_even[i]   = c.harmonic.i_even[i];
        harmonic.i_p_vel[i]  = c.harmonic.i_p_vel[i];
        harmonic.i_n_vel[i]  = c.harmonic.i_n_vel[i];
        harmonic.i_zero[i]   = c.harmonic.i_zero[i];
    }

    for(int i=0;i<12;i++)
    {
        harmonic.v1_nth[i]  = c.harmonic.v1_nth[i];
        harmonic.v2_nth[i]  = c.harmonic.v2_nth[i];
        harmonic.v3_nth[i]  = c.harmonic.v3_nth[i];
        harmonic.i1_nth[i]  = c.harmonic.i1_nth[i];
        harmonic.i2_nth[i]  = c.harmonic.i2_nth[i];
        harmonic.i3_nth[i]  = c.harmonic.i3_nth[i];
    }

    {
        diagnosis.rotor_health_level                 = c.diagnosis.rotor_health_level;
        diagnosis.rotor_health_index                 = c.diagnosis.rotor_health_index;
        diagnosis.static_eccentricity                = c.diagnosis.static_eccentricity;
        diagnosis.dynamic_eccentricity               = c.diagnosis.dynamic_eccentricity;
        diagnosis.windings_loose                     = c.diagnosis.windings_loose;
        diagnosis.turntoturnshort                    = c.diagnosis.turntoturnshort;
        diagnosis.noise_db                           = c.diagnosis.noise_db;
        diagnosis.estimated_number_of_rotor_bar      = c.diagnosis.estimated_number_of_rotor_bar;
        diagnosis.estimated_number_of_stator_slot    = c.diagnosis.estimated_number_of_stator_slot;
        diagnosis.runing_speed                       = c.diagnosis.runing_speed;
        diagnosis.runing_slip                        = c.diagnosis.runing_slip;
        diagnosis.pole_pass_frequency                = c.diagnosis.pole_pass_frequency;
        diagnosis.motor_running_efficiency           = c.diagnosis.motor_running_efficiency;
        diagnosis.output_power                       = c.diagnosis.output_power;
        diagnosis.demand_power                       = c.diagnosis.demand_power;
        diagnosis.output_torque                      = c.diagnosis.output_torque;
        diagnosis.imbanlance_or_unbanlance_index     = c.diagnosis.imbanlance_or_unbanlance_index;
        diagnosis.looseness                          = c.diagnosis.looseness;
        diagnosis.level_vdev_con                     = c.diagnosis.level_vdev_con;
        diagnosis.level_vbias_con                    = c.diagnosis.level_vbias_con;
        diagnosis.level_vcf_con                      = c.diagnosis.level_vcf_con;
        diagnosis.level_vharmoinc_con                = c.diagnosis.level_vharmoinc_con;
        diagnosis.level_pf_con                       = c.diagnosis.level_pf_con;
        diagnosis.level_load_con                     = c.diagnosis.level_load_con;
        diagnosis.level_ibalance_con                 = c.diagnosis.level_ibalance_con;
        diagnosis.level_ccf_con                      = c.diagnosis.level_ccf_con;
        diagnosis.level_charmonic_con                = c.diagnosis.level_charmonic_con;
        diagnosis.level_rotorhealth_con              = c.diagnosis.level_rotorhealth_con;
        diagnosis.level_statorhealth_con             = c.diagnosis.level_statorhealth_con;
        diagnosis.level_dynamic_or_static_ecc_con    = c.diagnosis.level_dynamic_or_static_ecc_con;
        diagnosis.level_unbalance_or_misalign_con    = c.diagnosis.level_unbalance_or_misalign_con;
        diagnosis.level_bearing_con                  = c.diagnosis.level_bearing_con;
    }

    {
        rollbearing.ftf_value1     = c.rollbearing.ftf_value1;
        rollbearing.bsf_value1     = c.rollbearing.bsf_value1;
        rollbearing.bpfo_value1    = c.rollbearing.bpfo_value1;
        rollbearing.bpfl_value1    = c.rollbearing.bpfl_value1;
        rollbearing.ftf_value2     = c.rollbearing.ftf_value2;
        rollbearing.bsf_value2     = c.rollbearing.bsf_value2;
        rollbearing.bpfo_value2    = c.rollbearing.bpfo_value2;
        rollbearing.bpfl_value2    = c.rollbearing.bpfl_value2;
        rollbearing.ftf_value3     = c.rollbearing.ftf_value3;
        rollbearing.bsf_value3     = c.rollbearing.bsf_value3;
        rollbearing.bpfo_value3    = c.rollbearing.bpfo_value3;
        rollbearing.bpfl_value3    = c.rollbearing.bpfl_value3;
        rollbearing.ftf_value4     = c.rollbearing.ftf_value4;
        rollbearing.bsf_value4     = c.rollbearing.bsf_value4;
        rollbearing.bpfo_value4    = c.rollbearing.bpfo_value4;
        rollbearing.bpfl_value4    = c.rollbearing.bpfl_value4;
    }

    {
        journalbearing.joub1_value  =   c.journalbearing.joub1_value;
        journalbearing.joub2_value  =   c.journalbearing.joub2_value;
        journalbearing.joub3_value  =   c.journalbearing.joub3_value;
    }

    {
        customparams.level_looseness = c.customparams.level_looseness;
    }
}

TW888Info& TW888Info::operator=(const TW888Info &c)
{
    area_id = c.area_id;
    machine_id = c.machine_id;
    rksj = c.rksj;

    for(int i=0;i<3;i++)
    {
        vcp.u_rms[i]     = c.vcp.u_rms[i];
        vcp.u_cf[i]      = c.vcp.u_cf[i];
        vcp.u_cf[i]      = c.vcp.u_cf[i];
        vcp.u_cf[i]      = c.vcp.u_cf[i];
        vcp.u_bias[i]    = c.vcp.u_bias[i];
        vcp.i_rms[i]     = c.vcp.i_rms[i];
        vcp.i_cf[i]      = c.vcp.i_cf[i];
        vcp.i_wf[i]      = c.vcp.i_wf[i];
        vcp.i_thd[i]     = c.vcp.i_thd[i];
        vcp.p_act[i]     = c.vcp.p_act[i];
        vcp.p_pf[i]      = c.vcp.p_pf[i];
        vcp.p_z[i]       = c.vcp.p_z[i];
    }

    vcp.u_balance = c.vcp.u_balance;
    vcp.u_df = c.vcp.u_df;

    vcp.i_balance = c.vcp.i_balance;
    vcp.i_df = c.vcp.i_df;

    vcp.p_tp = c.vcp.p_tp;
    vcp.p_d = c.vcp.p_d;
    vcp.p_load = c.vcp.p_load;

    for(int i=0;i<3;i++)
    {
        harmonic.v_thd[i]    = c.harmonic.v_thd[i];
        harmonic.v_odd[i]    = c.harmonic.v_odd[i];
        harmonic.v_even[i]   = c.harmonic.v_even[i];
        harmonic.v_p_vel[i]  = c.harmonic.v_p_vel[i];
        harmonic.v_n_vel[i]  = c.harmonic.v_n_vel[i];
        harmonic.i_thd[i]    = c.harmonic.i_thd[i];
        harmonic.i_odd[i]    = c.harmonic.i_odd[i];
        harmonic.i_even[i]   = c.harmonic.i_even[i];
        harmonic.i_p_vel[i]  = c.harmonic.i_p_vel[i];
        harmonic.i_n_vel[i]  = c.harmonic.i_n_vel[i];
        harmonic.i_zero[i]   = c.harmonic.i_zero[i];
    }

    for(int i=0;i<12;i++)
    {
        harmonic.v1_nth[i]  = c.harmonic.v1_nth[i];
        harmonic.v2_nth[i]  = c.harmonic.v2_nth[i];
        harmonic.v3_nth[i]  = c.harmonic.v3_nth[i];
        harmonic.i1_nth[i]  = c.harmonic.i1_nth[i];
        harmonic.i2_nth[i]  = c.harmonic.i2_nth[i];
        harmonic.i3_nth[i]  = c.harmonic.i3_nth[i];
    }

    {
        diagnosis.rotor_health_level                 = c.diagnosis.rotor_health_level;
        diagnosis.rotor_health_index                 = c.diagnosis.rotor_health_index;
        diagnosis.static_eccentricity                = c.diagnosis.static_eccentricity;
        diagnosis.dynamic_eccentricity               = c.diagnosis.dynamic_eccentricity;
        diagnosis.windings_loose                     = c.diagnosis.windings_loose;
        diagnosis.turntoturnshort                    = c.diagnosis.turntoturnshort;
        diagnosis.noise_db                           = c.diagnosis.noise_db;
        diagnosis.estimated_number_of_rotor_bar      = c.diagnosis.estimated_number_of_rotor_bar;
        diagnosis.estimated_number_of_stator_slot    = c.diagnosis.estimated_number_of_stator_slot;
        diagnosis.runing_speed                       = c.diagnosis.runing_speed;
        diagnosis.runing_slip                        = c.diagnosis.runing_slip;
        diagnosis.pole_pass_frequency                = c.diagnosis.pole_pass_frequency;
        diagnosis.motor_running_efficiency           = c.diagnosis.motor_running_efficiency;
        diagnosis.output_power                       = c.diagnosis.output_power;
        diagnosis.demand_power                       = c.diagnosis.demand_power;
        diagnosis.output_torque                      = c.diagnosis.output_torque;
        diagnosis.imbanlance_or_unbanlance_index     = c.diagnosis.imbanlance_or_unbanlance_index;
        diagnosis.looseness                          = c.diagnosis.looseness;
        diagnosis.level_vdev_con                     = c.diagnosis.level_vdev_con;
        diagnosis.level_vbias_con                    = c.diagnosis.level_vbias_con;
        diagnosis.level_vcf_con                      = c.diagnosis.level_vcf_con;
        diagnosis.level_vharmoinc_con                = c.diagnosis.level_vharmoinc_con;
        diagnosis.level_pf_con                       = c.diagnosis.level_pf_con;
        diagnosis.level_load_con                     = c.diagnosis.level_load_con;
        diagnosis.level_ibalance_con                 = c.diagnosis.level_ibalance_con;
        diagnosis.level_ccf_con                      = c.diagnosis.level_ccf_con;
        diagnosis.level_charmonic_con                = c.diagnosis.level_charmonic_con;
        diagnosis.level_rotorhealth_con              = c.diagnosis.level_rotorhealth_con;
        diagnosis.level_statorhealth_con             = c.diagnosis.level_statorhealth_con;
        diagnosis.level_dynamic_or_static_ecc_con    = c.diagnosis.level_dynamic_or_static_ecc_con;
        diagnosis.level_unbalance_or_misalign_con    = c.diagnosis.level_unbalance_or_misalign_con;
        diagnosis.level_bearing_con                  = c.diagnosis.level_bearing_con;
    }

    {
        rollbearing.ftf_value1     = c.rollbearing.ftf_value1;
        rollbearing.bsf_value1     = c.rollbearing.bsf_value1;
        rollbearing.bpfo_value1    = c.rollbearing.bpfo_value1;
        rollbearing.bpfl_value1    = c.rollbearing.bpfl_value1;
        rollbearing.ftf_value2     = c.rollbearing.ftf_value2;
        rollbearing.bsf_value2     = c.rollbearing.bsf_value2;
        rollbearing.bpfo_value2    = c.rollbearing.bpfo_value2;
        rollbearing.bpfl_value2    = c.rollbearing.bpfl_value2;
        rollbearing.ftf_value3     = c.rollbearing.ftf_value3;
        rollbearing.bsf_value3     = c.rollbearing.bsf_value3;
        rollbearing.bpfo_value3    = c.rollbearing.bpfo_value3;
        rollbearing.bpfl_value3    = c.rollbearing.bpfl_value3;
        rollbearing.ftf_value4     = c.rollbearing.ftf_value4;
        rollbearing.bsf_value4     = c.rollbearing.bsf_value4;
        rollbearing.bpfo_value4    = c.rollbearing.bpfo_value4;
        rollbearing.bpfl_value4    = c.rollbearing.bpfl_value4;
    }

    {
        journalbearing.joub1_value  =   c.journalbearing.joub1_value;
        journalbearing.joub2_value  =   c.journalbearing.joub2_value;
        journalbearing.joub3_value  =   c.journalbearing.joub3_value;
    }

    {
        customparams.level_looseness = c.customparams.level_looseness;
    }

    return *this;
}

QString TW888Info::toString()
{
    QString result;

    return result;
}
