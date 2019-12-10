#ifndef KSUMODBUSPARAMETRSMAP_H
#define KSUMODBUSPARAMETRSMAP_H

#include "Common/PrefixHeader.h"

class KSUModbusParametrsModel;
class KSUModbusParamertDomItem;

class KSUModbusParametrsMap : public QWidget
{
    Q_OBJECT
public:
    explicit KSUModbusParametrsMap(const QString &xmlPath, QWidget *parent = nullptr);
    ~KSUModbusParametrsMap();

    inline QList<KSUModbusParamertDomItem *> parametrs() const noexcept { return m_parametrs; }

public slots:
    void okAction();

    void add();
    void edit();
    void remove();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool saveConfiguration();
    void showSaveFileErrorMessage();

    void traverseXml(const QDomNode &domNode);

    QList<KSUModbusParamertDomItem *> m_parametrs;

    QFile m_configFile;
    QDomDocument m_xmlConfig;
    QDomNode m_parametrsContainerNode;

    QTableView *m_parametrsTable;
    KSUModbusParametrsModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
//    KSU0x20ParametrsDelegate *m_parametrsDelegate;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    bool m_hasChanges = false;
};

#endif // KSUMODBUSPARAMETRSMAP_H
