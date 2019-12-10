#include "RegisterVariableValueStrategy.h"

void RegisterVariableValueStrategy::setStartBit(quint8 startBit)
{
    if (startBit > _endBit)
        return;

    _startBit = startBit;
}

void RegisterVariableValueStrategy::setEndBit(quint8 endBit)
{
    if (endBit >= _valueBitsCount || endBit < _startBit)
        return;

    _endBit = endBit;
}

RegisterVariableValueStrategy::~RegisterVariableValueStrategy() noexcept = default;

void RegisterVariableValueStrategy::setBytesCount(quint8 bytesCount)
{
    _valueBitsCount = bytesCount * 8;

    if (_endBit >= _valueBitsCount) {
        _endBit = _valueBitsCount - 1;
    }

    if (_startBit > _endBit) {
        _startBit = _endBit;
    }
}

void RegisterVariableValueStrategy::setBitRange(quint8 startBit, quint8 endBit)
{
    if (startBit > endBit || endBit >= _valueBitsCount)
        return;

    _startBit = startBit;
    _endBit = endBit;
}
