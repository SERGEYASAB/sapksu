#ifndef IVARIABLEBYINDEX_H
#define IVARIABLEBYINDEX_H

class IVariable;

class IVariableByIndex
{
public:
    virtual IVariable *variableByIndex(const QModelIndex &index) const = 0;
};

#endif // IVARIABLEBYINDEX_H
