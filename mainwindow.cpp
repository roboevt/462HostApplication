#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <QTimer>
#include <QPainter>
#include <QPainterPath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Richarduino Host Application");

    // -------------------- Setup --------------------

    int boxWidth = 120;

    QWidget* window = new QWidget(this);
    window->setStyleSheet("background-color: light grey");

    QGridLayout* totalLayout = new QGridLayout(window);
    totalLayout->setSizeConstraint(QGridLayout::SetMinimumSize);

    // -------------------- Serial Comms --------------------
    // -------------------- Scope Controls --------------------

    serialComms = new SerialCommsWidget(this);
    totalLayout->addWidget(serialComms, 0, 0);

    // -------------------- Oscilloscope --------------------



    // pen.setColor(Qt::blue);
    // painter.setPen(pen);
    // QPainterPath path2;
    // for(double x = 0; x < scopeWidth; x++) {
    //     path2.lineTo(QPointF(x, -180*(sin(0.05 * (x-100))/(0.05 * (x-100)))+scopeHeight/2));
    // }
    // painter.drawPath(path2);

    oscopeLabel = new QLabel();
    // oscopeLabel->setPixmap(pixmap);
    totalLayout->addWidget(oscopeLabel, 1, 0, 1, 2);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    timer->start(93);  // we should read 4095 bytes every 92.8571 ms


    window->setMinimumWidth(scopeWidth);
    window->setMinimumHeight(window->width());

    setCentralWidget(window);

    window->show();
}

void MainWindow::updateDisplay() {
    serialComms->read();

    QPixmap pixmap = QPixmap(scopeWidth, scopeHeight);
    pixmap.fill(QColor("black"));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::white);
    pen.setWidth(3);
    painter.setPen(pen);

    QPainterPath path;
    for(double x = 0; x < scopeWidth; x++) {
        path.lineTo(QPointF(x, serialComms->samples[x]));
        // path.lineTo(QPointF(x, -180*(sin(0.05 * (x-100))/(0.05 * (x-100)))+800/2));
    }
    painter.drawPath(path);
    oscopeLabel->setPixmap(pixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}
