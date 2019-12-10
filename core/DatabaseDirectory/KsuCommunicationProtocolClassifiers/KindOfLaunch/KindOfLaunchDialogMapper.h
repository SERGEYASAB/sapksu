#ifndef KINDOFLAUNCHDIALOGMAPPER_H
#define KINDOFLAUNCHDIALOGMAPPER_H

#include "Common/PrefixHeader.h"
#include "DatabaseDirectory/InputDialogMapperInterface.h"

class KindOfLaunchDialogMapper : public InputDialogMapperInterface
{
public:
    explicit KindOfLaunchDialogMapper(int comProtocolCode, QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void save() override;

private:
    int m_comProtocolCode;
};

#endif // KINDOFLAUNCHDIALOGMAPPER_H
