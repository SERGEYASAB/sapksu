#include "IRZFCWidget.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsNode/INode.h"
#include "Devices/Device.h"
#include "StartWindow/ActionStyledWidget.h"
#include "ReportManager/ParamsReport.h"

IRZFCWidget::IRZFCWidget(const IRZFCWidget::IRZFCWidgetMode &value, QWidget *parent) : QWidget(parent)
{
    m_mode = value;

    v1 = new Device(this);

    if (m_mode == IRZFCWidgetMode::Cut)
        v1->setHidden(true);

    m_voltageInputButton = new QPushButton("Применить");
    connect(m_voltageInputButton, &QPushButton::clicked, this, &IRZFCWidget::writeVoltageInput);

    m_voltageInputLineEdit = new QSpinBox(this);
    m_voltageInputLineEdit->setMinimum(100);
    m_voltageInputLineEdit->setMaximum(280);
    m_voltageInputLineEdit->setSingleStep(5);

    m_k1 = new QDoubleSpinBox(this);
    m_k1->setMinimum(0.95);
    m_k1->setMaximum(1.05);
    m_k1->setSingleStep(0.01);

    if (m_mode == IRZFCWidgetMode::Cut)
        m_k1->setHidden(true);

    m_k2 = new QDoubleSpinBox(this);
    m_k2->setMinimum(0.000);
    m_k2->setMaximum(1.000);
    m_k2->setSingleStep(0.005);

#ifdef SAP_KSU
    m_voltageInputLineEdit->setValue(220);
    m_k1->setValue(1.00);
    m_k2->setValue(0.510);
#endif
#ifdef SAP_SU
    m_voltageInputLineEdit->setValue(220);
    m_k1->setValue(1.00);
    m_k2->setValue(0.510);
#endif
#ifdef MECHOS
    m_voltageInputLineEdit->setValue(220);
    m_k1->setValue(1.03);
    m_k2->setValue(0.510);
#endif

#ifndef QT_DEBUG
    m_k2->setDisabled(true);
#endif

    if (m_mode == IRZFCWidgetMode::Cut)
        m_k2->setHidden(true);

    m_startAction = new QAction(QIcon(":/Images/startButton.png"), tr("Пуск"), this);
    m_stopAction = new QAction(QIcon(":/Images/stopButton.png"), tr("Стоп"), this);

    connect(m_startAction, &QAction::triggered, this, &IRZFCWidget::start);
    connect(m_stopAction, &QAction::triggered, this, &IRZFCWidget::stop);

    ActionStyledWidget *m_startStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                         m_startAction,
                                                                         new QLabel("Пуск"),
                                                                         QIcon(":/Images/startButton.png"),
                                                                         QSize(80, 80));

    ActionStyledWidget *m_stopStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                        m_stopAction,
                                                                        new QLabel("Стоп"),
                                                                        QIcon(":/Images/stopButton.png"),
                                                                        QSize(80, 80));

    QGridLayout *gridLayout = new QGridLayout;


    gridLayout->addWidget(new QLabel("Задание значения напряжения"), 0, 0);

    if (m_mode == IRZFCWidgetMode::Full)
        gridLayout->addWidget(new QLabel("Коэффициент коррекции по напряжению"), 1, 0);

    if (m_mode == IRZFCWidgetMode::Full)
        gridLayout->addWidget(new QLabel("Коэффициент пересчета"), 2, 0);

    gridLayout->addWidget(m_voltageInputLineEdit, 0, 1);

    if (m_mode == IRZFCWidgetMode::Full)
        gridLayout->addWidget(m_k1, 1, 1);

    if (m_mode == IRZFCWidgetMode::Full)
        gridLayout->addWidget(m_k2, 2, 1);

    gridLayout->addWidget(m_voltageInputButton, 3, 1);
    gridLayout->addWidget(m_startStylesheetWidget, 5, 0);
    gridLayout->addWidget(m_stopStylesheetWidget, 5, 1);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(gridLayout);
    vBoxLayout->addStretch();

    if (m_mode == IRZFCWidgetMode::Full) {

        QHBoxLayout *centralLayout = new QHBoxLayout;

        if (m_mode == IRZFCWidgetMode::Full)
            centralLayout->addWidget(v1);

        centralLayout->addLayout(vBoxLayout);
        setLayout(centralLayout);
    }

    if (m_mode == IRZFCWidgetMode::Cut) {
        setLayout(vBoxLayout);
    }

    configurationChanged();
}

void IRZFCWidget::configurationChanged()
{
    auto server = IOServer::instance();
    if (!server)
        return;

    auto nodes = server->getNodes();
    v1->setDataSourceNode(findNodeByName(nodes, "UZ1"));
}

void IRZFCWidget::writeVoltageInput()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(tr("Значение напряжения/коэффициентов было изменено\n"));
    msgBox.setInformativeText(tr("Изменить значение напряжения?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok) {

        if (!IOServer::instance())
            return;

        IVariable *voltage = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_SUPPLY"));

        if (!voltage)
            return;

        voltage->setValue(m_voltageInputLineEdit->text().toInt() * m_k1->value() * m_k2->value() * 100/200);
    }
}

void IRZFCWidget::start()
{
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения Пуска нажмите \"Ок\"")))
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_DRIVE"));
    if (!variable)
        return;

    variable->setValue(1);

    LoggerTaskManager2::getInstance()->log(QString("Включена нагрузки ПЧ: U = %1").arg(m_voltageInputLineEdit->text()), LoggerTaskManager2::Info);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Включена нагрузки ПЧ"),
                                    m_voltageInputLineEdit->text());
}

void IRZFCWidget::stop()
{
    if (!IOServer::instance())
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_DRIVE"));
    if (!variable)
        return;

    variable->setValue(0);

    LoggerTaskManager2::getInstance()->log(QString("Отключение нагрузки ПЧ"), LoggerTaskManager2::Info);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Отключение нагрузки ПЧ"),
                                    m_voltageInputLineEdit->text());

}

INode *IRZFCWidget::findNodeByName(QList<INode *> &container, const QString &nodeName) const
{
    for (auto node : container) {
        if (node->name() == nodeName) {
            return node;
        }
    }
    return nullptr;
}

void IRZFCWidget::setMode(const IRZFCWidgetMode &value)
{
    m_mode = value;
}

bool IRZFCWidget::showConfirmChangeValueMessage(const QString &text) const
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QStringLiteral("Подтверждение"));
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec() == QMessageBox::Ok;
}
