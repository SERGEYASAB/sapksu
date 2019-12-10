#include "Classifier.h"

#include "ParametrAliases/ParametrAliases.h"
#include "CommunicationProtocols/CommunicationProtocol.h"
#include "KsuTypes/KsuTypes.h"
#include "ParametrTemplates/ParametrTemplates.h"
#include "KsuSoftwareVersions/KsuSoftwareVersions.h"
#include "SAPParametrs/SAPParametrs.h"
#include "Users/Users.h"
#include "ReportManager/OperatorClickReport.h"
#include "SUTypes/SUTypes.h"

Classifier::Classifier(QObject *parent)
{
    users = new Users;
    ksuTypes = new KsuTypes;
    communicationProtocol = new CommunicationProtocol;
    parametrAliases = new ParametrAliases;
    parametrTemplates = new ParametrTemplates;
    ksuSoftwareVersions = new KsuSoftwareVersions;
    sapParametrs = new SAPParametrs;
    suTypes = new SUTypes;

    addTab(parametrAliases, "Псевдонимы (ALIASES)");
    addTab(ksuTypes, "Типы КСУ");
    addTab(ksuSoftwareVersions, "Версии ПО КСУ");
    addTab(communicationProtocol, "Протоколы обмена");
    addTab(parametrTemplates, tr("Шаблоны классификаторов"));
    addTab(sapParametrs, tr("Параметры САП"));
    addTab(users, "Пользователи");
    addTab(suTypes, "Типы СУ");

    connect(this, &Classifier::currentChanged, this, &Classifier::currentIndexChanged);

    resize(1024, 768);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Справочник протоколов"));
}

void Classifier::currentIndexChanged(int index)
{
    switch (index) {
        case 0:
            parametrAliases->selectData();
            break;

        case 1:
            ksuTypes->selectData();
            break;

        case 2:
            ksuSoftwareVersions->selectData();
            break;

        case 3:
            communicationProtocol->selectData();
            break;

        case 4:
            parametrTemplates->selectData();
            break;

        case 5:
            sapParametrs->selectData();
            break;

        case 6:
            users->selectData();
            break;

        case 7:
            suTypes->selectData();
            break;

        default:
            break;
    }
}

void Classifier::closeEvent(QCloseEvent *event)
{
    OperatorClickReport::instance()->write(QStringLiteral("Справочники КСУ: закрытие окна"));

    QWidget::closeEvent(event);
}
