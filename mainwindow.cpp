#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <QTimer>
#include <QPainter>
#include <QPainterPath>

#include <cmath>

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
    oscopeLabel->setMinimumSize(scopeWidth, scopeHeight);
    // QSizePolicy oscopeSizePolicy(QSizePolicy::Expanding);
    oscopeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // oscopeLabel->setPixmap(pixmap);
    QBoxLayout* scopeLayout = new QHBoxLayout();
    scopeLayout->addWidget(oscopeLabel);
    totalLayout->addLayout(scopeLayout, 1, 0, 1, 2);
    // totalLayout->addWidget(oscopeLabel, 1, 0, 1, 2, Qt::AlignAbsolute);

    scopeTimer = new QTimer(this);
    connect(scopeTimer, SIGNAL(timeout()), this, SLOT(updateDisplay()));

    const float idealInterval = (numSamples/44100.0f)*1000;
    std::cout << "Reading at an interval of " << idealInterval << "ms" << std::endl;
    scopeTimer->start(std::floor(idealInterval));  // we should read 4095 bytes every 92.8571 ms
    // scopeTimer->start(20);


    window->setMinimumWidth(scopeWidth);
    window->setMinimumHeight(window->width());

    setCentralWidget(window);

    window->show();
}

void MainWindow::updateDisplay() {
    scopeWidth = oscopeLabel->width();
    scopeHeight = oscopeLabel->height();

    serialComms->read(numSamples);

    QPixmap pixmap = QPixmap(scopeWidth, scopeHeight);
    pixmap.fill(QColor("black"));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::white);
    pen.setWidth(3);
    painter.setPen(pen);

    QPainterPath path;
    for(int i = 0; i < numSamples; i++) {
        const float x = ((float)i / numSamples) * scopeWidth;
        const float y = (serialComms->samples[i] - 0xff/2) * (scopeHeight/0xff) + (scopeHeight/2);
        path.lineTo(QPointF(x, y));
        // path.lineTo(QPointF(x, -180*(sin(0.05 * (x-100))/(0.05 * (x-100)))+800/2));
    }
    painter.drawPath(path);
    oscopeLabel->setPixmap(pixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}
