#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "Common/PrefixHeader.h"

class KsuTypes;
class CommunicationProtocol;
class ParametrAliases;
class ParametrTemplates;
class KsuSoftwareVersions;
class SAPParametrs;
class Users;
class SUTypes;

class Classifier : public QTabWidget
{
public:
    explicit Classifier(QObject *parent = Q_NULLPTR);

private slots:
    void currentIndexChanged(int index);

private:
    ParametrAliases *parametrAliases = nullptr;
    KsuTypes *ksuTypes = nullptr;
    KsuSoftwareVersions *ksuSoftwareVersions = nullptr;
    CommunicationProtocol *communicationProtocol = nullptr;
    ParametrTemplates *parametrTemplates = nullptr;
    SAPParametrs *sapParametrs = nullptr;
    Users *users = nullptr;
    SUTypes *suTypes = nullptr;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // CLASSIFIER_H
