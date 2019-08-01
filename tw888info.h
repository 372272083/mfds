#ifndef TW888INFO_H
#define TW888INFO_H

#include "queryinfo.h"

class TW888Info : public QueryInfo
{
    Q_OBJECT
public:
    explicit TW888Info(QueryInfo *parent = 0);
    TW888Info(const TW888Info&);
    TW888Info& operator=(const TW888Info &other);

    int area_id;
    int machine_id;

    class VCP
    {
    public:
        float u_rms[3];
        float u_cf[3];
        float u_wf[3];
        float u_thd[3];
        float u_bias[3];
        float u_balance;
        float u_df;

        float i_rms[3];
        float i_cf[3];
        float i_wf[3];
        float i_thd[3];
        float i_balance;
        float i_df;

        float p_act[3];
        float p_tp;
        float p_pf[3];
        float p_z[3];
        float p_d;
        float p_load;
    };
    VCP vcp;

    class Harmonic
    {
    public:
        float v_thd[3];
        float v_odd[3];
        float v_even[3];
        float v_p_vel[3];
        float v_n_vel[3];
        float v_zero[3];
        float v1_nth[12];
        float v2_nth[12];
        float v3_nth[12];

        float i_thd[3];
        float i_odd[3];
        float i_even[3];
        float i_p_vel[3];
        float i_n_vel[3];
        float i_zero[3];
        float i1_nth[12];
        float i2_nth[12];
        float i3_nth[12];
    };
    Harmonic harmonic;

    class Diagnosis
    {
    public:
        float rotor_health_level;
        float rotor_health_index;
        float static_eccentricity;
        float dynamic_eccentricity;
        float windings_loose;
        float turntoturnshort;
        float noise_db;
        int estimated_number_of_rotor_bar;
        int estimated_number_of_stator_slot;
        float runing_speed;
        float runing_slip;
        float pole_pass_frequency;
        float motor_running_efficiency;
        float output_power;
        float demand_power;
        float output_torque;

        float imbanlance_or_unbanlance_index;
        float looseness;

        int level_vdev_con;
        int level_vbias_con;
        int level_vcf_con;
        int level_vharmoinc_con;

        int level_pf_con;
        int level_load_con;
        int level_ibalance_con;
        int level_ccf_con;
        int level_charmonic_con;
        int level_rotorhealth_con;
        int level_statorhealth_con;
        int level_dynamic_or_static_ecc_con;

        int level_unbalance_or_misalign_con;
        int level_bearing_con;
    };
    Diagnosis diagnosis;

    class RollingBearing
    {
    public:
        float ftf_value1;
        float bsf_value1;
        float bpfo_value1;
        float bpfl_value1;

        float ftf_value2;
        float bsf_value2;
        float bpfo_value2;
        float bpfl_value2;

        float ftf_value3;
        float bsf_value3;
        float bpfo_value3;
        float bpfl_value3;

        float ftf_value4;
        float bsf_value4;
        float bpfo_value4;
        float bpfl_value4;
    };
    RollingBearing rollbearing;

    class JournalBearing
    {
    public:
        float joub1_value;
        float joub2_value;
        float joub3_value;
    };
    JournalBearing journalbearing;

    class CustomParams
    {
    public:
        int level_looseness;
    };
    CustomParams customparams;

    virtual QString toString();

signals:

public slots:
};

#endif // TW888INFO_H
