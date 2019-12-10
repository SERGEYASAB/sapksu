#ifndef ATTESTATIONENERGOFORM_H
#define ATTESTATIONENERGOFORM_H

#include "Common/PrefixHeader.h"

class AttestationEnergoform : public QWidget
{
    Q_OBJECT

public:
    explicit AttestationEnergoform(QWidget *parent = 0);

private slots:
    void openEnergoForm();
};

#endif // ATTESTATIONENERGOFORM_H
