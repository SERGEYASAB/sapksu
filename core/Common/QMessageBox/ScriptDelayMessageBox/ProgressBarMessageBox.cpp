#include "ProgressBarMessageBox.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualFullTesting/ModbusKsuManualFullTestingWidget.h"
#include "ManualTesting/Ksu735/ManualFullTesting/Ksu735ManualFullTestingWidget.h"

void ProgressBarMessageBox::windowbehavior()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_bar);
    setLayout(layout);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    resize(500, 50);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(windowFlags() & Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle(tr("Проверка классификатора параметров"));
}

void ProgressBarMessageBox::setupProgressBar()
{
    m_bar = new QProgressBar;
    m_bar->setRange(0, 100);
    m_bar->setValue(0);
    m_bar->setAlignment(Qt::AlignCenter);
}

ProgressBarMessageBox::ProgressBarMessageBox(ModbusKsuManualFullTestingWidget *widget)
{
    setupProgressBar();
    connect(widget, &ModbusKsuManualFullTestingWidget::progressBarChanged, this, &ProgressBarMessageBox::setValue);
    windowbehavior();
}

ProgressBarMessageBox::ProgressBarMessageBox(Ksu735ManualFullTestingWidget *widget)
{
    setupProgressBar();
    connect(widget, &Ksu735ManualFullTestingWidget::progressBarChanged, this, &ProgressBarMessageBox::setValue);
    windowbehavior();
}

int ProgressBarMessageBox::value() const
{
    return m_bar->value();
}

void ProgressBarMessageBox::setValue(int value)
{
    m_bar->setValue(value);
    if (value == 100) {
        m_closeFlag = true;
        close();
    }
}


void ProgressBarMessageBox::closeEvent(QCloseEvent *event)
{
    if (!m_closeFlag)

        event->ignore();
    else
        QWidget::closeEvent(event);
}
