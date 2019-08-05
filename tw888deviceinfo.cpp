#include "tw888deviceinfo.h"

#include "constants.h"

#include "globalvariable.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlTableModel>

TW888DeviceInfo::TW888DeviceInfo(DeviceInfo *parent) : DeviceInfo(parent)
{
    area_id = 0;
    machine_id = 0;
}

void TW888DeviceInfo::init()
{
    DeviceInfo::init();
}

void TW888DeviceInfo::handlerReceiveMsg()
{
    int datalen = rec_buffer.size();
    int slen = rec_buffer.size() - MODBUS_HEAD_LEHGHT;
    int byteStep = 2;
    char cmd;
    char unit;
    char num;

    waitingCount = 0;
    //isReceiving = false;
    //qDebug() << "receive data lenght: " << datalen;
    if (datalen <=0)
    {
        //failed
        isReceiving = false;
        DeviceInfo::handlerReceiveMsg();
        return;
    }
    if (datalen <= 6)
    {
        //record wave
        isReceiving = false;
        DeviceInfo::handlerReceiveMsg();
        return;
    }
    //handle wave data
    cmd = rec_buffer[7];
    unit = rec_buffer[6];
    num = rec_buffer[1];


    if (datalen <=9) //modbus error
    {
        qDebug() << "modbus data error!";
    }
    else
    {
        if (cmd == 0x3)
        {
            //qDebug() << "modbus data h3!";
            char mq0 = rec_buffer[0];
            //char mq1 = rec_buffer[1];
            char mq2 = rec_buffer[2];
            char mq3 = rec_buffer[3];
            char mq5 = rec_buffer[5];
            char mq8 = rec_buffer[8];
            //qDebug() << "modbus data len: " << ((int)mq5);
            if (mq0 == 0x0 && mq2 == 0x0 && mq3 == 0x0 && mq5 == (mq8 + 3))
            {

            }
            else //modbus valid error!
            {
                isReceiving = false;
                DeviceInfo::handlerReceiveMsg();
                return;
            }
        }
        else if (cmd == 0x10)
        {
            qDebug() << "modbus data h10!";
        }

        if(unit == COM_R) //read measure data ok
        {
            qDebug() << "measure read!";
            byteStep = 2;
            if(num == TW888_R_0) //标识数据
            {
                QByteArray measure_buffer;
                measure_buffer.resize(2);
                for(int i=0;i<2;i++)
                {
                    measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep];
                    measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep + 1];

                    ushort valuedata;
                    memcpy(&valuedata,measure_buffer,2);

                    if(0 == i)
                    {
                        version_main = valuedata;
                    }
                    else
                    {
                        version_sub = valuedata;
                    }
                }

                for(int i=2;i<10;i++)
                {
                    measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep];
                    measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep + 1];

                    ushort valuedata;
                    memcpy(&valuedata,measure_buffer,2);

                    guid[i-2] = valuedata;
                }

                measure_buffer.resize(4);
                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 12*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 12*byteStep + 1];
                measure_buffer[3] = rec_buffer[MODBUS_HEAD_LEHGHT + 12*byteStep + 2];
                measure_buffer[2] = rec_buffer[MODBUS_HEAD_LEHGHT + 12*byteStep + 3];

                int valuedata_i;
                memcpy(&valuedata_i,measure_buffer,4);

                supportMotorNum = valuedata_i;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 16*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 16*byteStep + 1];
                measure_buffer[3] = rec_buffer[MODBUS_HEAD_LEHGHT + 16*byteStep + 2];
                measure_buffer[2] = rec_buffer[MODBUS_HEAD_LEHGHT + 16*byteStep + 3];

                memcpy(&valuedata_i,measure_buffer,4);

                area_id = valuedata_i;

                twinfo.area_id = area_id;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 18*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 18*byteStep + 1];
                measure_buffer[3] = rec_buffer[MODBUS_HEAD_LEHGHT + 18*byteStep + 2];
                measure_buffer[2] = rec_buffer[MODBUS_HEAD_LEHGHT + 18*byteStep + 3];

                memcpy(&valuedata_i,measure_buffer,4);

                machine_id = valuedata_i;
                twinfo.machine_id = machine_id;

                int year,month,date,hour,minute,second;

                measure_buffer.resize(2);
                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 20*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 20*byteStep + 1];

                ushort valuedata;
                memcpy(&valuedata,measure_buffer,2);

                year = valuedata;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 21*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 21*byteStep + 1];

                valuedata;
                memcpy(&valuedata,measure_buffer,2);

                month = valuedata;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 22*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 22*byteStep + 1];

                valuedata;
                memcpy(&valuedata,measure_buffer,2);

                date = valuedata;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 23*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 23*byteStep + 1];

                valuedata;
                memcpy(&valuedata,measure_buffer,2);

                hour = valuedata;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 24*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 24*byteStep + 1];

                valuedata;
                memcpy(&valuedata,measure_buffer,2);

                minute = valuedata;

                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 25*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 25*byteStep + 1];

                valuedata;
                memcpy(&valuedata,measure_buffer,2);

                second = valuedata;

                //QDate qd(year,month,date);
                //QTime qt(hour,minute,second);
                //QDateTime rksj_dt(qd,qt);

                //twinfo.rksj = rksj_dt.toString(GlobalVariable::dtFormat);

                QDateTime current_time = QDateTime::currentDateTime();
                QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);
                twinfo.rksj = StrCurrentTime;
            }
            else if(num == TW888_R_1) //标识数据
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                byteStep = 4;
                for(int i=0;i<43;i++)
                {
                    measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,4);
                    float measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);
                    //qDebug() << "measure: " << QString::number(measure_value,10,4);
                    if(i<3)
                    {
                        twinfo.vcp.u_rms[i] = measure_value;
                    }
                    else if(i<6)
                    {
                        twinfo.vcp.u_cf[i-3] = measure_value;
                    }
                    else if(i<9)
                    {
                        twinfo.vcp.u_wf[i-6] = measure_value;
                    }
                    else if(i<12)
                    {
                        twinfo.vcp.u_thd[i-9] = measure_value;
                    }
                    else if(i<15)
                    {
                        twinfo.vcp.u_bias[i-12] = measure_value;
                    }
                    else if(15 == i)
                    {
                        twinfo.vcp.u_balance = measure_value;
                    }
                    else if(16 == i)
                    {
                        twinfo.vcp.u_df = measure_value;
                    }
                    else if(i<20)
                    {
                        twinfo.vcp.i_rms[i-17] = measure_value;
                    }
                    else if(i<23)
                    {
                        twinfo.vcp.i_cf[i-20] = measure_value;
                    }
                    else if(i<26)
                    {
                        twinfo.vcp.i_wf[i-23] = measure_value;
                    }
                    else if(i<29)
                    {
                        twinfo.vcp.i_thd[i-26] = measure_value;
                    }
                    else if(i==29)
                    {
                        twinfo.vcp.i_balance = measure_value;
                    }
                    else if(i==30)
                    {
                        twinfo.vcp.i_df = measure_value;
                    }
                    else if(i<34)
                    {
                        twinfo.vcp.p_act[i-31] = measure_value;
                    }
                    else if(i==34)
                    {
                        twinfo.vcp.p_tp = measure_value;
                    }
                    else if(i<38)
                    {
                        twinfo.vcp.p_pf[i-36] = measure_value;
                    }
                    else if(i<41)
                    {
                        twinfo.vcp.p_z[i-38] = measure_value;
                    }
                    else if(i==41)
                    {
                        twinfo.vcp.p_d = measure_value;
                    }
                    else if(i==42)
                    {
                        twinfo.vcp.p_load = measure_value;
                    }
                }
            }
            else if(num == TW888_R_2)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                byteStep = 4;
                for(int i=0;i<54;i++)
                {
                    measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,4);
                    float measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);
                    //qDebug() << "measure: " << QString::number(measure_value,10,4);
                    if(i<3)
                    {
                        twinfo.harmonic.v_thd[i] = measure_value;
                    }
                    else if(i<6)
                    {
                        twinfo.harmonic.v_odd[i-3] = measure_value;
                    }
                    else if(i<9)
                    {
                        twinfo.harmonic.v_even[i-6] = measure_value;
                    }
                    else if(i<12)
                    {
                        twinfo.harmonic.v_p_vel[i-9] = measure_value;
                    }
                    else if(i<15)
                    {
                        twinfo.harmonic.v_n_vel[i-12] = measure_value;
                    }
                    else if(i<18)
                    {
                        twinfo.harmonic.v_zero[i-15] = measure_value;
                    }
                    else if(i<30)
                    {
                        twinfo.harmonic.v1_nth[i-18] = measure_value;
                    }
                    else if(i<42)
                    {
                        twinfo.harmonic.v2_nth[i-30] = measure_value;
                    }
                    else if(i<54)
                    {
                        twinfo.harmonic.v3_nth[i-42] = measure_value;
                    }
                }
            }
            else if(num == TW888_R_3)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                byteStep = 4;
                for(int i=0;i<54;i++)
                {
                    measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,4);
                    float measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);
                    //qDebug() << "measure: " << QString::number(measure_value,10,4);
                    if(i<3)
                    {
                        twinfo.harmonic.i_thd[i] = measure_value;
                    }
                    else if(i<6)
                    {
                        twinfo.harmonic.i_odd[i-3] = measure_value;
                    }
                    else if(i<9)
                    {
                        twinfo.harmonic.i_even[i-6] = measure_value;
                    }
                    else if(i<12)
                    {
                        twinfo.harmonic.i_p_vel[i-9] = measure_value;
                    }
                    else if(i<15)
                    {
                        twinfo.harmonic.i_n_vel[i-12] = measure_value;
                    }
                    else if(i<18)
                    {
                        twinfo.harmonic.i_zero[i-15] = measure_value;
                    }
                    else if(i<30)
                    {
                        twinfo.harmonic.i1_nth[i-18] = measure_value;
                    }
                    else if(i<42)
                    {
                        twinfo.harmonic.i2_nth[i-30] = measure_value;
                    }
                    else if(i<54)
                    {
                        twinfo.harmonic.i3_nth[i-42] = measure_value;
                    }
                }
            }
            else if(num == TW888_R_4)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                byteStep = 4;
                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 0*byteStep,4);
                float measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.rotor_health_level = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 1*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.rotor_health_index = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 2*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.static_eccentricity = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 3*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.dynamic_eccentricity = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 4*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.windings_loose = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 5*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.turntoturnshort = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 6*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.noise_db = measure_value;

                /////////////////////////////////
                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 7*byteStep,4);
                int measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.estimated_number_of_rotor_bar = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 8*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.estimated_number_of_stator_slot = measure_value_i;
                //////////////////////////////////////////////////////

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 9*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.runing_speed = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 10*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.runing_slip = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 11*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.pole_pass_frequency = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 13*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.motor_running_efficiency = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 14*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.output_power = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 15*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.demand_power = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 16*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.output_torque = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 17*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.imbanlance_or_unbanlance_index = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 18*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.looseness = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 21*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_vdev_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 22*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_vbias_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 23*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_vcf_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 24*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_vharmoinc_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 25*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_pf_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 26*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_load_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 27*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_ibalance_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 28*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_ccf_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 29*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_charmonic_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 30*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_rotorhealth_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 31*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_statorhealth_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 32*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_dynamic_or_static_ecc_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 33*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_unbalance_or_misalign_con = measure_value_i;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 34*byteStep,4);
                measure_value_i = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.diagnosis.level_bearing_con = measure_value_i;
            }
            else if(num == TW888_R_5)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                byteStep = 4;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 1*byteStep,4);
                float measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.ftf_value1 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 3*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bsf_value1 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 5*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfo_value1 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 7*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfl_value1 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 9*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.ftf_value2 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 11*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bsf_value2 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 13*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfo_value2 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 15*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfl_value2 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 17*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.ftf_value3 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 19*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bsf_value3 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 21*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfo_value3 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 23*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfl_value3 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 25*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.ftf_value4 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 27*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bsf_value4 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 29*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfo_value4 = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 31*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.rollbearing.bpfl_value4 = measure_value;

                ////////////////////////////////////////////////

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 33*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.journalbearing.joub1_value = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 35*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.journalbearing.joub2_value = measure_value;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 37*byteStep,4);
                measure_value = DeviceInfo::bufferTofloat(measure_buffer,LH_LL_HH_HL);

                twinfo.journalbearing.joub3_value = measure_value;
            }
            else if(num == TW888_R_6)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                byteStep = 4;

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 1*byteStep,4);
                int measure_value = DeviceInfo::bufferToint(measure_buffer,LH_LL_HH_HL);

                twinfo.customparams.level_looseness = measure_value;

                TW888Info* ptwinfo = new TW888Info(twinfo);

                QMutexLocker m_lock(&GlobalVariable::tw888chargesglobalMutex);
                if(GlobalVariable::tw888charges.contains(deviceCode))
                {
                    if(GlobalVariable::tw888charges[deviceCode].size()>=MAX_QUEUE_NUM)
                    {
                        GlobalVariable::tw888charges[deviceCode].dequeue();
                    }
                    GlobalVariable::tw888charges[deviceCode].enqueue(ptwinfo);
                }
                else
                {
                    QQueue<TW888Info*> chqueue;
                    chqueue.enqueue(ptwinfo);

                    GlobalVariable::tw888charges[deviceCode] = chqueue;
                }
            }
        }
    }

    isReceiving = false;
    DeviceInfo::handlerReceiveMsg();
}

QString TW888DeviceInfo::getComAddress()
{
    return "";
}

void TW888DeviceInfo::close()
{

}

void TW888DeviceInfo::handleSendMsg()
{
    static int count = 0; //multi-windows

    if(msgPriSendQueue.size()>MAX_QUEUE_NUM || msgSendQueue.size()>MAX_QUEUE_NUM)
    {
        return;
    }

    if (isReceiving)
    {
        return;
    }

    //int subwindow = count % measure_interval;
    //if (0 == subwindow)
    {
        //if(!isHaveSomeState(0,COM_R))
        {
            struct ModbusTCPMapInfo cmd;
            cmd.Num = TW888_R_0;
            cmd.Unit = COM_R;
            cmd.Addr = 0;
            cmd.Command = 0x4;
            cmd.Length = 26;
            cmd.ExpectLen = cmd.Length * 2 + 9;
            msgSendQueue.enqueue(cmd);

            struct ModbusTCPMapInfo cmd_1;
            cmd_1.Num = TW888_R_1;
            cmd_1.Unit = COM_R;
            cmd_1.Addr = 31;
            cmd_1.Command = 0x4;
            cmd_1.Length = 86;
            cmd_1.ExpectLen = cmd_1.Length * 2 + 9;
            msgSendQueue.enqueue(cmd_1);

            struct ModbusTCPMapInfo cmd_2;
            cmd_2.Num = TW888_R_2;
            cmd_2.Unit = COM_R;
            cmd_2.Addr = 117;
            cmd_2.Command = 0x4;
            cmd_2.Length = 108;
            cmd_2.ExpectLen = cmd_2.Length * 2 + 9;
            msgSendQueue.enqueue(cmd_2);

            struct ModbusTCPMapInfo cmd_3;
            cmd_3.Num = TW888_R_3;
            cmd_3.Unit = COM_R;
            cmd_3.Addr = 225;
            cmd_3.Command = 0x4;
            cmd_3.Length = 108;
            cmd_3.ExpectLen = cmd_3.Length * 2 + 9;
            msgSendQueue.enqueue(cmd_3);

            struct ModbusTCPMapInfo cmd_4;
            cmd_4.Num = TW888_R_4;
            cmd_4.Unit = COM_R;
            cmd_4.Addr = 333;
            cmd_4.Command = 0x4;
            cmd_4.Length = 70;
            cmd_4.ExpectLen = cmd_4.Length * 2 + 9;
            msgSendQueue.enqueue(cmd_4);

            struct ModbusTCPMapInfo cmd_5;
            cmd_5.Num = TW888_R_5;
            cmd_5.Unit = COM_R;
            cmd_5.Addr = 553;
            cmd_5.Command = 0x4;
            cmd_5.Length = 76;
            cmd_5.ExpectLen = cmd_5.Length * 2 + 9;
            msgSendQueue.enqueue(cmd_5);

            struct ModbusTCPMapInfo cmd_6;
            cmd_6.Num = TW888_R_6;
            cmd_6.Unit = COM_R;
            cmd_6.Addr = 799;
            cmd_6.Command = 0x4;
            cmd_6.Length = 2;
            cmd_6.ExpectLen = cmd_6.Length * 2 + 9;
            msgSendQueue.enqueue(cmd_6);
        }
    }
    count++;
}
