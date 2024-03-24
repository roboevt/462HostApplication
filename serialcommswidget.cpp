#include "serialcommswidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <iostream>
#include <fstream>

SerialCommsWidget::SerialCommsWidget(QWidget* parent)
    : QWidget(parent), buttonFont("Arial", 12, QFont::Bold), labelFont("Arial", 12), richarduino(1, 921600) {
    this->setMinimumWidth(defaultWidth * 3);

    QGridLayout* totalLayout = new QGridLayout(this);
    totalLayout->setSizeConstraint(QGridLayout::SetMinimumSize);

    QWidget* serialCommsBox = new QWidget();
    serialCommsBox->setMinimumWidth(defaultWidth * 3);
    serialCommsBox->setStyleSheet("background-color: grey");
    totalLayout->addWidget(serialCommsBox, 0, 0);

    QVBoxLayout* serialCommsLayout = new QVBoxLayout(serialCommsBox);
    serialCommsLayout->setAlignment(Qt::AlignTop);

    QWidget* scopeControlsBox = new QWidget();
    scopeControlsBox->setMinimumWidth(defaultWidth * 3);
    scopeControlsBox->setStyleSheet("background-color: grey");
    totalLayout->addWidget(scopeControlsBox, 0, 1);

    QVBoxLayout* scopeControlsLayout = new QVBoxLayout(scopeControlsBox);
    scopeControlsLayout->setAlignment(Qt::AlignTop);

    // ---------- Connect ----------
    QHBoxLayout* connectLayout = new QHBoxLayout();

    connectButton = new QPushButton("Connect");
    connectButton->setFixedWidth(defaultWidth);
    connectButton->setFont(buttonFont);
    connectLayout->addWidget(connectButton);

    connectInput = new QLineEdit();
    connectInput->setPlaceholderText("Port Number");
    connectInput->setFixedWidth(defaultWidth);
    connectInput->setFont(labelFont);
    connectLayout->addWidget(connectInput);

    connectBaud = new QComboBox();
    connectBaud->setPlaceholderText("--Baud Rate--");
    connectBaud->addItem("115200", 115200);
    connectBaud->addItem("921600", 921600);
    connectBaud->setFont(labelFont);
    connectLayout->addWidget(connectBaud);

    if(richarduino.connected) {
        connectInput->setText(QString::number(richarduino.port));
        int index = connectBaud->findData(richarduino.baud);
        if ( index != -1 ) { // -1 for not found
            connectBaud->setCurrentIndex(index);
        }

    }

    serialCommsLayout->addLayout(connectLayout);

    // ---------- Peek ----------
    QHBoxLayout* peekLayout = new QHBoxLayout();

    peekButton = new QPushButton("Peek");
    peekButton->setFixedWidth(defaultWidth);
    peekButton->setFont(buttonFont);
    peekLayout->addWidget(peekButton);


    peekInput = new QLineEdit();
    peekInput->setPlaceholderText("Address");
    peekInput->setFixedWidth(defaultWidth);
    peekInput->setFont(labelFont);
    peekLayout->addWidget(peekInput);

    peekDataLabel = new QLabel();
    peekDataLabel->setMinimumWidth(defaultWidth);
    peekDataLabel->setStyleSheet(labelStyle);
    peekDataLabel->setFont(labelFont);
    peekLayout->addWidget(peekDataLabel);

    serialCommsLayout->addLayout(peekLayout);

    // ---------- Poke ----------

    QHBoxLayout* pokeLayout = new QHBoxLayout();

    pokeButton = new QPushButton("Poke");
    pokeButton->setFixedWidth(defaultWidth);
    pokeButton->setFont(buttonFont);
    pokeLayout->addWidget(pokeButton);


    pokeAddressInput = new QLineEdit();
    pokeAddressInput->setPlaceholderText("Address");
    pokeAddressInput->setFixedWidth(defaultWidth);
    pokeAddressInput->setFont(labelFont);
    pokeLayout->addWidget(pokeAddressInput);

    pokeDataInput = new QLineEdit();
    pokeDataInput->setPlaceholderText("Data");
    pokeDataInput->setFont(labelFont);
    pokeLayout->addWidget(pokeDataInput);

    serialCommsLayout->addLayout(pokeLayout);

    // ---------- Version ----------

    QHBoxLayout* versionLayout = new QHBoxLayout();

    versionButton = new QPushButton("Version");
    versionButton->setFixedWidth(defaultWidth);
    versionButton->setFont(buttonFont);
    versionLayout->addWidget(versionButton);

    versionLabel = new QLabel();
    versionLabel->setMinimumWidth(defaultWidth);
    versionLabel->setStyleSheet(labelStyle);
    versionLabel->setFont(labelFont);
    versionLayout->addWidget(versionLabel);

    serialCommsLayout->addLayout(versionLayout);

    // ---------- Firmware ----------

    QHBoxLayout* firmwareLayout = new QHBoxLayout();

    firmwareButton = new QPushButton("Program");
    firmwareButton->setFixedWidth(defaultWidth);
    firmwareButton->setFont(buttonFont);
    firmwareLayout->addWidget(firmwareButton);


    firmwareFileSelectButton = new QPushButton("Select File");
    firmwareFileSelectButton->setFixedWidth(defaultWidth);
    firmwareFileSelectButton->setFont(buttonFont);
    firmwareLayout->addWidget(firmwareFileSelectButton);


    firmwareLabel = new QLabel();
    firmwareLabel->setStyleSheet(labelStyle);
    firmwareLabel->setMinimumWidth(defaultWidth);
    firmwareLabel->setFont(labelFont);
    firmwareLayout->addWidget(firmwareLabel);

    serialCommsLayout->addLayout(firmwareLayout);

    // -------------------- Scope Controls --------------------

    // ---------- Power On ----------

    QHBoxLayout* powerLayout = new QHBoxLayout();
    powerLayout->setAlignment(Qt::AlignLeft);

    QPushButton* powerButton = new QPushButton("Power On");
    powerButton->setFixedWidth(defaultWidth);
    powerButton->setFont(buttonFont);
    powerLayout->addWidget(powerButton);
    connect(powerButton, SIGNAL(clicked()), SLOT(powerOn()));


    scopeControlsLayout->addLayout(powerLayout);

    // ---------- Transfer ----------

    QHBoxLayout* transferLayout = new QHBoxLayout();
    transferLayout->setAlignment(Qt::AlignLeft);

    QPushButton* uartTransferButton = new QPushButton("Transfer - UART");
    uartTransferButton->setFixedWidth(defaultWidth * 1.4);
    uartTransferButton->setFont(buttonFont);
    transferLayout->addWidget(uartTransferButton);
    connect(uartTransferButton, SIGNAL(clicked()), SLOT(uartTransfer()));

    QPushButton* vgaTransferButton = new QPushButton("Transfer - VGA");
    vgaTransferButton->setFixedWidth(defaultWidth * 1.4);
    vgaTransferButton->setFont(buttonFont);
    transferLayout->addWidget(vgaTransferButton);
    connect(vgaTransferButton, SIGNAL(clicked()), SLOT(vgaTransfer()));

    scopeControlsLayout->addLayout(transferLayout);

    // ---------- Read ----------

    QHBoxLayout* readLayout = new QHBoxLayout();
    readLayout->setAlignment(Qt::AlignLeft);

    QPushButton* readButton = new QPushButton("Read");
    readButton->setFixedWidth(defaultWidth);
    readButton->setFont(buttonFont);
    readLayout->addWidget(readButton);
    connect(readButton, SIGNAL(clicked()), SLOT(read()));

    scopeControlsLayout->addLayout(readLayout);


    // ---------- Gain ----------

    QHBoxLayout* gainLayout = new QHBoxLayout();

    QPushButton* gainButton = new QPushButton("Set Gain");
    gainButton->setFixedWidth(defaultWidth);
    gainButton->setFont(buttonFont);
    gainLayout->addWidget(gainButton);
    connect(gainButton, SIGNAL(clicked()), SLOT(setGain()));

    gainInput = new QSpinBox();
    gainInput->setMinimumWidth(defaultWidth);
    gainInput->setMinimum(1);
    gainInput->setMaximum(50);
    gainInput->setSuffix("x");
    gainLayout->addWidget(gainInput);

    scopeControlsLayout->addLayout(gainLayout);

    // ---------- Trigger ----------
    QHBoxLayout* triggerLayout = new QHBoxLayout();

    QPushButton* triggerButton = new QPushButton("Set Trigger");
    triggerButton->setFixedWidth(defaultWidth);
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

    connect(connectButton, SIGNAL(clicked()), SLOT(connectToRicharduino()));
    connect(peekButton, SIGNAL(clicked()), SLOT(peek()));
    connect(pokeButton, SIGNAL(clicked()), SLOT(poke()));
    connect(versionButton, SIGNAL(clicked()), SLOT(checkVersion()));
    connect(firmwareButton, SIGNAL(clicked()), SLOT(uploadFirmware()));
    connect(firmwareFileSelectButton, SIGNAL(clicked()), SLOT(browseFirmware()));

    read();
}

void SerialCommsWidget::connectToRicharduino() {
    int port = connectInput->text().toInt();
    int baudrate = connectBaud->currentText().toInt();
    if(richarduino.connect(port, baudrate) != 1) {
        std::cout << "Unable to connect to richarduino on port " << port << " at baud " << baudrate << std::endl;
    }
}

void SerialCommsWidget::peek() {
    uint32_t address = peekInput->text().toUInt(nullptr, 16);
    int value = richarduino.peek(address);
    peekDataLabel->setText(QString::number(value, 16));
    peekDataLabel->setStyleSheet(completedLabelStyle);
}

void SerialCommsWidget::poke() {
    uint32_t address = pokeAddressInput->text().toUInt(nullptr,16);
    uint32_t data = pokeDataInput->text().toUInt(nullptr, 16);
    richarduino.poke(address,data);
    pokeDataInput->setStyleSheet(completedLabelStyle);
}

void SerialCommsWidget::checkVersion() {
    int version = richarduino.version();

    versionLabel->setText(QString::number(version));
    versionLabel->setStyleSheet(completedLabelStyle);
}

void SerialCommsWidget::browseFirmware() {
    QString firmwareFilePath = QFileDialog::getOpenFileName(this,
                                                            tr("Open Text file"), "", tr("Bin Files (*.bin)"));
    firmwareLabel->setText(firmwareFilePath);
}

void SerialCommsWidget::uploadFirmware() {
    QString filePath = firmwareLabel->text();
    if(filePath != "") {
        std::vector<uint32_t> firmware = readFirmwareFile(filePath.toStdString());
        richarduino.program(firmware);
        firmwareLabel->setStyleSheet(completedLabelStyle);
    }
}

void SerialCommsWidget::powerOn() {
    richarduino.poke(0xfffffff0, 1); // Set direction to ouput for pin 1
    richarduino.poke(0xfffffff4, 1); // Turn on pin
}

void SerialCommsWidget::uartTransfer() {
    richarduino.poke(0xffffffb8, 1); // Length
    richarduino.poke(0xffffffbc, 0xffffffe4); // Uart tx addr
    richarduino.poke(0xffffffb0, 1); // Go bit
}

void SerialCommsWidget::vgaTransfer() {
    richarduino.poke(0xffffffb8, 1024);  // length
    richarduino.poke(0xffffffbc, 0x20000); // start of framebuffer
    richarduino.poke(0xffffffb0, 1);  // Go bit
}

void SerialCommsWidget::read() {
    std::vector<char> data = richarduino.read(4095);

    // for(char c : data) {
    //     std::cout << std::dec << ((short)c & 0xff) << " ";
    // }
    // std::cout << std::endl << std::endl;

    for(int i = 0; i < data.size(); i++) {
        samples[i] = data[i];
    }
}

void SerialCommsWidget::setGain() {
    int gain = gainInput->value();
    std::cout << "Setting gain (" << gain << ") not yet implemented" << std::endl;
}

void SerialCommsWidget::setTrigger() {
    QString trigger = triggerInput->currentText();
    std::cout << "Setting trigger (" << trigger.toStdString() << ") not yet implemented" << std::endl;
}

std::vector<uint32_t> SerialCommsWidget::readFirmwareFile(std::string firmwarePath) {
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
