#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "serialcommswidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <QPainter>
#include <QPainterPath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // -------------------- Setup --------------------

    int boxWidth = 120;
    int scopeWidth = 800;
    int scopeHeight = 400;

    QFont buttonFont("Arial", 12, QFont::Bold);
    QFont labelFont("Arial", 12);

    QWidget* window = new QWidget(this);
    window->setStyleSheet("background-color: light grey");

    QGridLayout* totalLayout = new QGridLayout(window);
    totalLayout->setSizeConstraint(QGridLayout::SetMinimumSize);

    // -------------------- Serial Comms --------------------
    // -------------------- Scope Controls --------------------

    SerialCommsWidget* serialComms = new SerialCommsWidget(this);
    totalLayout->addWidget(serialComms, 0, 0);

    // -------------------- Oscilloscope --------------------

    QPixmap pixmap(scopeWidth, scopeHeight);
    pixmap.fill(QColor("black"));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::white);
    pen.setWidth(3);
    painter.setPen(pen);

    QPainterPath path;
    for(double x = 0; x < scopeWidth; x++) {
        path.lineTo(QPointF(x, 100*sin(0.03 * x)+scopeHeight/2));
    }
    painter.drawPath(path);

    pen.setColor(Qt::blue);
    painter.setPen(pen);
    QPainterPath path2;
    for(double x = 0; x < scopeWidth; x++) {
        path2.lineTo(QPointF(x, -180*(sin(0.05 * (x-100))/(0.05 * (x-100)))+scopeHeight/2));
    }
    painter.drawPath(path2);

    QLabel* oscopeLabel = new QLabel();
    oscopeLabel->setPixmap(pixmap);
    totalLayout->addWidget(oscopeLabel, 1, 0, 1, 2);


    window->setMinimumWidth(scopeWidth);
    window->setMinimumHeight(window->width());

    setCentralWidget(window);

    window->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
