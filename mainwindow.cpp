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
    this->setWindowTitle("Eric Todd Richarduino Host Application");

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

    oscopeLabel = new QLabel();
    oscopeLabel->setMinimumSize(scopeWidth, scopeHeight);
    oscopeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QBoxLayout* scopeLayout = new QHBoxLayout();
    scopeLayout->addWidget(oscopeLabel);
    totalLayout->addLayout(scopeLayout, 1, 0, 1, 2);

    scopeTimer = new QTimer(this);
    connect(serialComms, SIGNAL(newSamplesAvailable()), this, SLOT(updateDisplay()));

    window->setMinimumWidth(scopeWidth);
    window->setMinimumHeight(window->width());

    setCentralWidget(window);

    window->show();
}

void MainWindow::updateDisplay() {
    scopeWidth = oscopeLabel->width();
    scopeHeight = oscopeLabel->height();

    // serialComms->read(numSamples);

    QPixmap pixmap = QPixmap(scopeWidth, scopeHeight);
    pixmap.fill(QColor("black"));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);


    QPen pen(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 18));
    painter.drawText(QPoint(5,23), "3.3V");
    painter.drawText(QPoint(5,scopeHeight-5), "-3.3V");


    painter.setWindow(0,0,scopeWidth,255);
    const int lineLevel = 255-serialComms->triggerLevel;
    painter.drawLine(0,lineLevel,scopeWidth,lineLevel);
    pen.setWidth(3);
    painter.setPen(pen);
    QPainterPath path(QPointF(-127, 127));
    for(int i = 0; i < numSamples; i++) {
        const float x = ((float)i / numSamples) * scopeWidth;
        const float y = 255 - serialComms->samples[i];
        path.lineTo(QPointF(x, y));
    }
    painter.drawPath(path);
    oscopeLabel->setPixmap(pixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}
