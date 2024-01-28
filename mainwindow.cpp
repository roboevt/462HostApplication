#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <fstream>

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <QPainter>
#include <QPainterPath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), richarduino("/dev/ttyUSB1", B115200)
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

    QWidget* serialCommsBox = new QWidget();
    serialCommsBox->setMinimumWidth(boxWidth * 3);
    serialCommsBox->setStyleSheet("background-color: grey");
    totalLayout->addWidget(serialCommsBox, 0, 0);

    QVBoxLayout* serialCommsLayout = new QVBoxLayout(serialCommsBox);
    serialCommsLayout->setAlignment(Qt::AlignTop);

    QWidget* scopeControlsBox = new QWidget();
    scopeControlsBox->setMinimumWidth(boxWidth * 3);
    scopeControlsBox->setStyleSheet("background-color: grey");
    totalLayout->addWidget(scopeControlsBox, 0, 1);

    QVBoxLayout* scopeControlsLayout = new QVBoxLayout(scopeControlsBox);
    scopeControlsLayout->setAlignment(Qt::AlignTop);

    // -------------------- Serial Comms --------------------

    // ---------- Peek ----------
    QHBoxLayout* peekLayout = new QHBoxLayout();

    QPushButton* peekButton = new QPushButton("Peek");
    peekButton->setFixedWidth(boxWidth);
    peekButton->setFont(buttonFont);
    peekLayout->addWidget(peekButton);
    connect(peekButton, SIGNAL(clicked()), SLOT(peek()));

    peekInput = new QLineEdit();
    peekInput->setPlaceholderText("Address");
    peekInput->setFixedWidth(boxWidth);
    peekInput->setFont(labelFont);
    peekLayout->addWidget(peekInput);

    peekDataLabel = new QLabel();
    peekDataLabel->setMinimumWidth(boxWidth);
    peekDataLabel->setStyleSheet(labelStyle);
    peekDataLabel->setFont(labelFont);
    peekLayout->addWidget(peekDataLabel);

    serialCommsLayout->addLayout(peekLayout);

    // ---------- Poke ----------

    QHBoxLayout* pokeLayout = new QHBoxLayout();

    QPushButton* pokeButton = new QPushButton("Poke");
    pokeButton->setFixedWidth(boxWidth);
    pokeButton->setFont(buttonFont);
    pokeLayout->addWidget(pokeButton);
    connect(pokeButton, SIGNAL(clicked()), SLOT(poke()));

    pokeAddressInput = new QLineEdit();
    pokeAddressInput->setPlaceholderText("Address");
    pokeAddressInput->setFixedWidth(boxWidth);
    pokeAddressInput->setFont(labelFont);
    pokeLayout->addWidget(pokeAddressInput);

    pokeDataInput = new QLineEdit();
    pokeDataInput->setPlaceholderText("Data");
    pokeDataInput->setFont(labelFont);
    pokeLayout->addWidget(pokeDataInput);

    serialCommsLayout->addLayout(pokeLayout);

    // ---------- Version ----------

    QHBoxLayout* versionLayout = new QHBoxLayout();

    QPushButton* versionButton = new QPushButton("Version");
    versionButton->setFixedWidth(boxWidth);
    versionButton->setFont(buttonFont);
    versionLayout->addWidget(versionButton);
    connect(versionButton, SIGNAL(clicked()), SLOT(checkVersion()));

    versionLabel = new QLabel();
    versionLabel->setMinimumWidth(boxWidth);
    versionLabel->setStyleSheet(labelStyle);
    versionLabel->setFont(labelFont);
    versionLayout->addWidget(versionLabel);

    serialCommsLayout->addLayout(versionLayout);

    // ---------- Firmware ----------

    QHBoxLayout* firmwareLayout = new QHBoxLayout();

    QPushButton* firmwareButton = new QPushButton("Program");
    firmwareButton->setFixedWidth(boxWidth);
    firmwareButton->setFont(buttonFont);
    firmwareLayout->addWidget(firmwareButton);
    connect(firmwareButton, SIGNAL(clicked()), SLOT(uploadFirmware()));

    QPushButton* firmwareFileSelectButton = new QPushButton("Select File");
    firmwareFileSelectButton->setFixedWidth(boxWidth);
    firmwareFileSelectButton->setFont(buttonFont);
    firmwareLayout->addWidget(firmwareFileSelectButton);
    connect(firmwareFileSelectButton, SIGNAL(clicked()), SLOT(browseFirmware()));

    firmwareLabel = new QLabel();
    firmwareLabel->setStyleSheet(labelStyle);
    firmwareLabel->setMinimumWidth(boxWidth);
    firmwareLabel->setFont(labelFont);
    firmwareLayout->addWidget(firmwareLabel);

    serialCommsLayout->addLayout(firmwareLayout);

    // -------------------- Scope Controls --------------------

    // ---------- Gain ----------

    QHBoxLayout* gainLayout = new QHBoxLayout();

    QPushButton* gainButton = new QPushButton("Set Gain");
    gainButton->setFixedWidth(boxWidth);
    gainButton->setFont(buttonFont);
    gainLayout->addWidget(gainButton);
    connect(gainButton, SIGNAL(clicked()), SLOT(setGain()));

    gainInput = new QSpinBox();
    gainInput->setMinimumWidth(boxWidth);
    gainInput->setMinimum(1);
    gainInput->setMaximum(50);
    gainInput->setSuffix("x");
    gainLayout->addWidget(gainInput);

    scopeControlsLayout->addLayout(gainLayout);

    // ---------- Trigger ----------
    QHBoxLayout* triggerLayout = new QHBoxLayout();

    QPushButton* triggerButton = new QPushButton("Set Trigger");
    triggerButton->setFixedWidth(boxWidth);
    triggerButton->setFont(buttonFont);
    triggerLayout->addWidget(triggerButton);
    connect(triggerButton, SIGNAL(clicked()), SLOT(setTrigger()));

    triggerInput = new QComboBox();
    triggerInput->addItem("Rising Edge");
    triggerInput->addItem("Falling Edge");
    triggerInput->addItem("Level"); // Add level input
    triggerInput->setFont(labelFont);
    triggerLayout->addWidget(triggerInput);

    scopeControlsLayout->addLayout(triggerLayout);

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

void MainWindow::peek() {
    uint32_t address = peekInput->text().toInt(nullptr, 16);
    int value = richarduino.peek(address);
    peekDataLabel->setText(QString::number(value, 16));
    peekDataLabel->setStyleSheet(completedLabelStyle);
}

void MainWindow::poke() {
    uint32_t address = pokeAddressInput->text().toInt(nullptr,16);
    uint32_t data = pokeDataInput->text().toInt(nullptr, 16);
    richarduino.poke(address,data);
    pokeDataInput->setStyleSheet(completedLabelStyle);
}

void MainWindow::checkVersion() {
    int version = richarduino.version();

    versionLabel->setText(QString::number(version));
    versionLabel->setStyleSheet(completedLabelStyle);
}

void MainWindow::browseFirmware() {
    QString firmwareFilePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open Text file"), "", tr("Bin Files (*.bin)"));
    firmwareLabel->setText(firmwareFilePath);
}

void MainWindow::uploadFirmware() {
    QString filePath = firmwareLabel->text();
    if(filePath != "") {
        std::vector<uint32_t> firmware = readFirmwareFile(filePath.toStdString());
        richarduino.program(firmware);
        firmwareLabel->setStyleSheet(completedLabelStyle);
    }
}

void MainWindow::setGain() {
    int gain = gainInput->value();
    std::cout << "Setting gain (" << gain << ") not yet implemented" << std::endl;
}

void MainWindow::setTrigger() {
    QString trigger = triggerInput->currentText();
    std::cout << "Setting trigger (" << trigger.toStdString() << ") not yet implemented" << std::endl;
}

std::vector<uint32_t> MainWindow::readFirmwareFile(std::string firmwarePath) {
    std::ifstream firmwareFile(firmwarePath);

    uint32_t address;
    firmwareFile >> std::hex >> address;
    if (address != 4096) {
        std::cout << "Invalid firmware file, first address must be 4096, was " +
                         std::to_string(address)
                  << std::endl;
        return {};
    }

    std::vector<uint32_t> firmware;

    std::cout << "Reading firmware from " << firmwarePath << ". Firmware: " << std::endl;

    hex(std::cout);
    while (firmwareFile.good()) {
        uint32_t instruction;
        if (firmwareFile >> address >> instruction) {
            firmware.push_back(instruction);
        }
    }
    dec(std::cout);

    return firmware;
}

MainWindow::~MainWindow()
{
    delete ui;
}
