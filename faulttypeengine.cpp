#include "faulttypeengine.h"

FaultTypeEngine::FaultTypeEngine(QObject *parent) : QObject(parent)
{
    faults_v.insert(1,tr("Unbalanced"));
    faults_v.insert(2,tr("Misalignment"));
    faults_v.insert(3,tr("Bent Shaft"));
    faults_v.insert(4,tr("Eccentric"));
    faults_v.insert(5,tr("Looseness"));
    faults_v.insert(6,tr("Antifriction bearing"));
    faults_v.insert(7,tr("Sliding bearing"));
    faults_v.insert(8,tr("Rub fault"));
    faults_v.insert(9,tr("Gear box failure"));
    faults_v.insert(10,tr("Sympathy"));

    faults_e.insert(1,tr("Voltage balance"));
    faults_e.insert(2,tr("Voltage nameplate deviation"));
    faults_e.insert(3,tr("Voltage spike coefficient"));
    faults_e.insert(4,tr("Voltage harmonics"));
    faults_e.insert(5,tr("Power factor"));
    faults_e.insert(6,tr("Load"));
    faults_e.insert(7,tr("Current Balance"));
    faults_e.insert(8,tr("Current spike coefficient"));
    faults_e.insert(9,tr("Current harmonics"));
    faults_e.insert(10,tr("Rotor health"));
    faults_e.insert(11,tr("Stator"));
    faults_e.insert(12,tr("Air gap"));
    faults_e.insert(13,tr("Rotor balance or alignment"));
    faults_e.insert(14,tr("Loosening"));
    faults_e.insert(15,tr("Bearing"));
}
