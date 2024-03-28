#ifndef SERIALCOMMSWIDGET_H
#define SERIALCOMMSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <thread>

#include "Richarduino.h"

class SerialCommsWidget : public QWidget
{
    Q_OBJECT

    int defaultWidth = 120;
    QFont buttonFont;
    QFont labelFont;

    QWidget* boundingBox;

    QPushButton* connectButton;
    QLineEdit* connectInput;
    QComboBox* connectBaud;

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

    QComboBox* transferInput;

    QComboBox* gainInput;
    QComboBox* offsetInput;
    QComboBox* triggerInput;

    QString labelStyle = "border:1px solid black";
    QString completedLabelStyle = "border:1px solid green";

    Richarduino richarduino;

    std::vector<uint32_t> readFirmwareFile(std::string firmwarePath);

    std::jthread worker;

    enum class TriggerMode { disabled, risingEdge, fallingEdge } triggerMode;
    uint8_t triggerLevel = 127; // todo adjustable

public:
    SerialCommsWidget(QWidget* parent = nullptr);
    ~SerialCommsWidget();

    std::array<uint8_t, 4095> samples;


public Q_SLOTS:
    void connectToRicharduino();
    void poke();
    void peek();
    void checkVersion();
    void browseFirmware();
    void uploadFirmware();

    void powerOn();
    void transfer();
    void startScope();
    void stopScope();
    // void read();
    void read(int samples);
    void setGain();
    void setOffset();
    void setTrigger();

signals:
    void newSamplesAvailable();

};

#endif // SERIALCOMMSWIDGET_H

// richarduino("/dev/ttyUSB2", B115200)
