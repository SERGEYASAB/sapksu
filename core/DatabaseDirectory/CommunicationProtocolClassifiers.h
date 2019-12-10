#ifndef COMMUNICATIONPROTOCOLCLASSIFIERS_H
#define COMMUNICATIONPROTOCOLCLASSIFIERS_H

#include "Common/PrefixHeader.h"

class CommunicationProtocolClassifiers : public QTabWidget
{
public:
    explicit CommunicationProtocolClassifiers(int comProtocolId, SapKsu::KSUProtocolType protocolType, QObject *parent = Q_NULLPTR);
};

#endif // COMMUNICATIONPROTOCOLCLASSIFIERS_H
