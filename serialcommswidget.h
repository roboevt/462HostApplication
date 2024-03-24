#ifndef SERIALCOMMSWIDGET_H
#define SERIALCOMMSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>

#include "Richarduino.h"

class SerialCommsWidget : public QWidget
{
    Q_OBJECT

    int defaultWidth = 120;
    QFont buttonFont;
    QFont labelFont;

    QWidget* boundingBox;

    QPushButton* connectButton;
    QPushButton* peekButton;
    QPushButton* pokeButton;
    QPushButton* versionButton;
    QPushButton* firmwareButton;
    QPushButton* firmwareFileSelectButton;

    QLabel* peekDataLabel;
    QLabel* firmwareLabel;
    QLabel* versionLabel;

    QLineEdit* peekInput;
    QLineEdit* pokeAddressInput, *pokeDataInput;
    QLineEdit* connectInput;

    QSpinBox* gainInput;
    QComboBox* triggerInput;

    QString labelStyle = "border:1px solid black";
    QString completedLabelStyle = "border:1px solid green";

    Richarduino richarduino;

    std::vector<uint32_t> readFirmwareFile(std::string firmwarePath);

public:
    SerialCommsWidget(QWidget* parent = nullptr);


public Q_SLOTS:
    void poke();
    void peek();
    void checkVersion();
    void browseFirmware();
    void uploadFirmware();

    void setGain();
    void setTrigger();

};

#endif // SERIALCOMMSWIDGET_H

// richarduino("/dev/ttyUSB2", B115200)