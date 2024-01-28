#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

#include "Richarduino.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    void poke();
    void peek();
    void checkVersion();
    void browseFirmware();
    void uploadFirmware();

    void setGain();
    void setTrigger();

private:
    Ui::MainWindow *ui;

    Richarduino richarduino;

    QString labelStyle = "border:1px solid black";
    QString completedLabelStyle = "border:1px solid green";

    QLabel* peekDataLabel;
    QLabel* firmwareLabel;
    QLabel* versionLabel;

    QLineEdit* peekInput;
    QLineEdit* pokeAddressInput, *pokeDataInput;
    QSpinBox* gainInput;
    QComboBox* triggerInput;

    std::vector<uint32_t> readFirmwareFile(std::string firmwarePath);

};
#endif // MAINWINDOW_H
