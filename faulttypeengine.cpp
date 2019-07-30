#include "faulttypeengine.h"

FaultTypeEngine::FaultTypeEngine(QObject *parent) : QObject(parent)
{
    faults.insert(1,tr("Unbalanced"));
    faults.insert(2,tr("Misalignment"));
    faults.insert(3,tr("Bent Shaft"));
    faults.insert(4,tr("Eccentric"));
    faults.insert(5,tr("Looseness"));
    faults.insert(6,tr("Antifriction bearing"));
    faults.insert(7,tr("Sliding bearing"));
    faults.insert(8,tr("Rub fault"));
    faults.insert(9,tr("Gear box failure"));
    faults.insert(10,tr("Sympathy"));
}
