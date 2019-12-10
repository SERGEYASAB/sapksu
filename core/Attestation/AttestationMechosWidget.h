#ifndef ATTESTATIONMECHOSWIDGET_H
#define ATTESTATIONMECHOSWIDGET_H

#include "Common/PrefixHeader.h"

class CircutorWidget;
class AttestationMechosWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AttestationMechosWidget(QWidget *parent = nullptr);

private:
    QPushButton *supplyButton = nullptr;
    QPushButton *startButton = nullptr;
    QPushButton *loadTen1Button = nullptr;
    QPushButton *loadTen2Button = nullptr;
    QPushButton *loadMotorButton = nullptr;
    QPushButton *loadReactButton = nullptr;
    QVBoxLayout *goupLayout;
    QWidget *widget;
    CircutorWidget *circutorWidget = nullptr;

    QList<QPushButton *>loadButtons;
    bool stateOnStartButton = false;
    bool stateOnTen1Button = false;
    bool stateOnTen2Button = false;
    bool stateOnMotorButton = false;
    bool stateOnReactButton = false;

    void blockButtons(QPushButton *button);

    void supplyAction();
    void startAction();
    void loadTen1Action();
    void loadTen2Action();
    void loadMotorAction();
    void loadReactAction();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};
#endif // ATTESTATIONMECHOSWIDGET_H
