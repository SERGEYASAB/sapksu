#include "NumberVariableValueStrategy.h"

NumberVariableValueStrategy::NumberVariableValueStrategy() : NumberVariableValueStrategy(1.0, 0.0)
{

}

NumberVariableValueStrategy::NumberVariableValueStrategy(double coefficientK, double coefficientB) : VariableValueStrategy(),
    _coefficientK(coefficientK), _coefficientB(coefficientB)
{

}

void NumberVariableValueStrategy::setCoefficientK(double coefficientK)
{
    if (coefficientK == 0.0)
        return;

    _coefficientK = coefficientK;
}

void NumberVariableValueStrategy::setCoefficientB(double coefficientB)
{
    _coefficientB = coefficientB;
}
