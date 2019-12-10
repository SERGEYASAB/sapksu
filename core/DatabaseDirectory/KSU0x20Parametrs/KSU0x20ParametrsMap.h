#ifndef KSU0X20PARAMETRSMAP_H
#define KSU0X20PARAMETRSMAP_H

#include "Common/PrefixHeader.h"
#include "KSU0x20Parametr.h"

class KSU0x20ParametrsModel;
class KSU0x20ParametrsDelegate;
class CustomizableTableView;

class KSU0x20ParametrsMap : public QWidget
{
    Q_OBJECT
public:
    explicit KSU0x20ParametrsMap(const QString &xmlPath, QWidget *parent = nullptr);

    inline QVector<KSU0x20Parametr *> parametrs() const noexcept { return m_parametrs; }

public slots:
    void okAction();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool saveConfiguration();
    void showSaveFileErrorMessage();

    void traverseXml(const QDomNode &domNode);
    void fillXmlDocument(QDomDocument &document);

    QVector<KSU0x20Parametr *> m_parametrs;

    QFile m_configFile;
    QDomDocument m_xmlConfig;

    CustomizableTableView *m_parametrsTable = nullptr;
    KSU0x20ParametrsModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;
    KSU0x20ParametrsDelegate *m_parametrsDelegate = nullptr;

    QVBoxLayout *m_buttonBox = nullptr;
    QPushButton *m_okButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
};

#endif // KSU0X20PARAMETRSMAP_H
