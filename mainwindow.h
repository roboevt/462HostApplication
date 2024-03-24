#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

#include "serialcommswidget.h"

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

private:
    Ui::MainWindow *ui;
    SerialCommsWidget* serialComms;

    QLabel* oscopeLabel;

    QString labelStyle = "border:1px solid black";
    QString completedLabelStyle = "border:1px solid green";

    int scopeWidth = 800;
    int scopeHeight = 400;

public Q_SLOTS:
    void updateDisplay();

};
#endif // MAINWINDOW_H
