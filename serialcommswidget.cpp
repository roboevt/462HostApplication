#include "serialcommswidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <iostream>
#include <fstream>
#include <map>

SerialCommsWidget::SerialCommsWidget(QWidget* parent)
    : QWidget(parent), buttonFont("Arial", 12, QFont::Bold), labelFont("Arial", 12), richarduino(1, 921600), triggerMode(TriggerMode::level) {
    std::fill(samples.begin(), samples.end(), 0);
    this->setMinimumWidth(defaultWidth * 3);

    QGridLayout* totalLayout = new QGridLayout(this);
    totalLayout->setSizeConstraint(QGridLayout::SetMinimumSize);
    totalLayout->setAlignment(Qt::AlignTop);

    QWidget* serialCommsBox = new QWidget();
    serialCommsBox->setMinimumWidth(defaultWidth * 3);
    serialCommsBox->setStyleSheet("background-color: grey");
    totalLayout->addWidget(serialCommsBox, 0, 0);

    QVBoxLayout* serialCommsLayout = new QVBoxLayout(serialCommsBox);
    serialCommsLayout->setAlignment(Qt::AlignTop);

    QWidget* scopeControlsBox = new QWidget();
    scopeControlsBox->setMinimumWidth(defaultWidth * 3 + 30);
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

    QPushButton* scopeStartButton = new QPushButton("Start Scope");
    scopeStartButton->setFixedWidth(defaultWidth);
    scopeStartButton->setFont(buttonFont);
    powerLayout->addWidget(scopeStartButton);
    connect(scopeStartButton, SIGNAL(clicked()), SLOT(startScope()));

    QPushButton* scopeStopButton = new QPushButton("Stop Scope");
    scopeStopButton->setFixedWidth(defaultWidth);
    scopeStopButton->setFont(buttonFont);
    powerLayout->addWidget(scopeStopButton);
    connect(scopeStopButton, SIGNAL(clicked()), SLOT(stopScope()));

    scopeControlsLayout->addLayout(powerLayout);

    connect(scopeStartButton, SIGNAL(clicked()), this, SLOT(startScope()));
    connect(scopeStopButton, SIGNAL(clicked()), this, SLOT(stopScope()));

    // ---------- Transfer ----------

    QHBoxLayout* transferLayout = new QHBoxLayout();
    transferLayout->setAlignment(Qt::AlignLeft);

    QPushButton* transferButton = new QPushButton("Transfer");
    transferButton->setFixedWidth(defaultWidth);
    transferButton->setFont(buttonFont);
    transferLayout->addWidget(transferButton);
    connect(transferButton, SIGNAL(clicked()), SLOT(transfer()));

    transferInput = new QComboBox();
    transferInput->setMinimumWidth(defaultWidth);
    transferInput->addItem("UART");
    transferInput->addItem("VGA");
    transferInput->addItem("Disable");
    transferInput->setFont(labelFont);
    transferLayout->addWidget(transferInput);
    scopeControlsLayout->addLayout(transferLayout);

    // ---------- Gain ----------

    QHBoxLayout* gainLayout = new QHBoxLayout();

    QPushButton* gainButton = new QPushButton("Set Gain");
    gainButton->setFixedWidth(defaultWidth);
    gainButton->setFont(buttonFont);
    gainLayout->addWidget(gainButton);
    connect(gainButton, SIGNAL(clicked()), SLOT(setGain()));

    gainInput = new QComboBox();
    gainInput->setMinimumWidth(defaultWidth);
    gainInput->addItem("0.25");
    gainInput->addItem("1");
    gainInput->addItem("10");
    gainInput->addItem("20");
    gainInput->addItem("30");
    gainInput->addItem("40");
    gainInput->addItem("60");
    gainInput->addItem("80");
    gainInput->addItem("120");
    gainInput->addItem("157");
    gainInput->setFont(labelFont);
    gainLayout->addWidget(gainInput);

    scopeControlsLayout->addLayout(gainLayout);

    // ---------- Offset ----------

    QHBoxLayout* offsetLayout = new QHBoxLayout();

    QPushButton* offsetButton = new QPushButton("Set Offset");
    offsetButton->setFixedWidth(defaultWidth);
    offsetButton->setFont(buttonFont);
    offsetLayout->addWidget(offsetButton);
    connect(offsetButton, SIGNAL(clicked()), SLOT(setOffset()));

    offsetInput = new QComboBox();
    offsetInput->setMinimumWidth(defaultWidth);
    offsetInput->addItem("-17.6");
    offsetInput->addItem("-16.7");
    offsetInput->addItem("-15.7");
    offsetInput->addItem("-14.7");
    offsetInput->addItem("-13.7");
    offsetInput->addItem("-12.7");
    offsetInput->addItem("-11.7");
    offsetInput->addItem("-10.6");
    offsetInput->addItem("-8.4");
    offsetInput->addItem("-7.3");
    offsetInput->addItem("-6.1");
    offsetInput->addItem("-4.9");
    offsetInput->addItem("-3.8");
    offsetInput->addItem("-2.5");
    offsetInput->addItem("-1.3");
    offsetInput->addItem("0");
    offsetInput->addItem("1.3");
    offsetInput->addItem("2.5");
    offsetInput->addItem("3.8");
    offsetInput->addItem("4.9");
    offsetInput->addItem("6.1");
    offsetInput->addItem("7.3");
    offsetInput->addItem("8.4");
    offsetInput->addItem("10.6");
    offsetInput->addItem("11.7");
    offsetInput->addItem("12.7");
    offsetInput->addItem("13.7");
    offsetInput->addItem("14.7");
    offsetInput->addItem("15.7");
    offsetInput->addItem("16.7");
    offsetInput->addItem("17.6");
    offsetInput->setFont(labelFont);
    offsetLayout->addWidget(offsetInput);

    scopeControlsLayout->addLayout(offsetLayout);

    // ---------- Trigger ----------
    QHBoxLayout* triggerLayout = new QHBoxLayout();

    QPushButton* triggerButton = new QPushButton("Set Trigger");
    triggerButton->setFixedWidth(defaultWidth);
    triggerButton->setFont(buttonFont);
    triggerLayout->addWidget(triggerButton);
    connect(triggerButton, SIGNAL(clicked()), SLOT(setTrigger()));

    triggerInput = new QComboBox();
    triggerInput->addItem("Level");
    triggerInput->addItem("Rising Edge");
    triggerInput->addItem("Falling Edge");
    triggerInput->addItem("Disabled");
    triggerInput->setFont(labelFont);
    triggerLayout->addWidget(triggerInput);

    levelInput = new QSlider();
    levelInput->setOrientation(Qt::Horizontal);
    levelInput->setMinimum(-33);
    levelInput->setMaximum(33);
    triggerLayout->addWidget(levelInput);
    connect(levelInput, SIGNAL(valueChanged(int)), this, SLOT(setLevel(int)));

    scopeControlsLayout->addLayout(triggerLayout);

    connect(connectButton, SIGNAL(clicked()), SLOT(connectToRicharduino()));
    connect(peekButton, SIGNAL(clicked()), SLOT(peek()));
    connect(pokeButton, SIGNAL(clicked()), SLOT(poke()));
    connect(versionButton, SIGNAL(clicked()), SLOT(checkVersion()));
    connect(firmwareButton, SIGNAL(clicked()), SLOT(uploadFirmware()));
    connect(firmwareFileSelectButton, SIGNAL(clicked()), SLOT(browseFirmware()));

    // read();
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

void SerialCommsWidget::startScope() {
    worker = std::jthread{ [this] (std::stop_token token) {
            while(!token.stop_requested()) {
                auto newSamples = richarduino.read(samples.size());

                if(triggerMode == TriggerMode::risingEdge) {
                    int triggerPos = 0;
                    for(int i = 1; i < newSamples.size(); i++) {
                        const int sample = (int)newSamples[i] & 0xff;
                        const int lastSample = (int)newSamples[i-1] & 0xff;
                        if(sample >= triggerLevel && lastSample < triggerLevel) {
                            triggerPos = i;
                            break;
                        }
                    }
                    // std::cout << "Found at pos " << triggerPos << std::endl;
                    std::copy(newSamples.begin() + triggerPos, newSamples.end(), samples.begin());
                } else  if(triggerMode == TriggerMode::fallingEdge) {
                    int triggerPos = 0;
                    for(int i = 1; i < newSamples.size(); i++) {
                        const int sample = (int)newSamples[i] & 0xff;
                        const int lastSample = (int)newSamples[i-1] & 0xff;
                        if(sample <= triggerLevel && lastSample > triggerLevel) {
                            triggerPos = i;
                            break;
                        }
                    }
                    // std::cout << "Found at pos " << triggerPos << std::endl;
                    std::copy(newSamples.begin() + triggerPos, newSamples.end(), samples.begin());
                } else if(triggerMode == TriggerMode::level) {
                    int triggerPos = 0;
                    for(int i = 1; i < newSamples.size(); i++) {
                        const int sample = (int)newSamples[i] & 0xff;
                        const int lastSample = (int)newSamples[i-1] & 0xff;
                        if(sample <= triggerLevel && lastSample > triggerLevel || sample >= triggerLevel && lastSample < triggerLevel) {
                            triggerPos = i;
                            break;
                        }
                    }
                    std::copy(newSamples.begin() + triggerPos, newSamples.end(), samples.begin());
                } else {
                    std::copy(newSamples.begin(), newSamples.end(), samples.begin());
                }
                // Request new frame to be drawn.
                emit newSamplesAvailable();
            }
    }};
}

void SerialCommsWidget::stopScope() {
    worker.request_stop();
}

void SerialCommsWidget::transfer() {
    QString transferMode = transferInput->currentText();
    if(transferMode == "UART") {
        richarduino.poke(0xffffffb8, 1); // Length
        richarduino.poke(0xffffffbc, 0xffffffe4); // Uart tx addr
        richarduino.poke(0xffffffb0, 1); // Go bit
    } else if(transferMode == "VGA") {
        richarduino.poke(0xffffffb8, 500000);  // length
        richarduino.poke(0xffffffbc, 0x200000); // start of framebuffer
        richarduino.poke(0xffffffb0, 1);  // Go bit
    } else if(transferMode == "Disable") {
        richarduino.poke(0xffffffb0, 0); // Go bit
    } else {
        std::cout << "Invalid transfer mode " << transferMode.toStdString() << std::endl;
    }
}

void SerialCommsWidget::read(int num) {
    std::vector<char> data = richarduino.read(num);

    // for(char c : data) {
    //     std::cout << std::dec << ((short)c & 0xff) << " ";
    // }
    // std::cout << std::endl << std::endl;

    for(int i = 0; i < data.size(); i++) {
        samples[i] = data[i];
    }
}

const static std::map<std::string, uint8_t> gainMap{{"1", 0b00000001}, {"10", 0b0000011}, {"20",0b0000101}, {"30",0b0000111}, {"40",0b0001001},{"60",0b0001011},
                                                    {"80",0b0001101},{"120",0b0001111},{"157",0b0010001},{"0.25",0b0010011},{".25",0b0010011}};
const static std::map<std::string, uint8_t> offsetMap{{"-17.6",0x3e},{"-16.7",0x3c},{"-15.7",0x3a},{"-14.7",0x38},{"-13.7",0x36},{"-12.7",0x34},{"-11.7",0x32},
                                                      {"-10.6",0x30},{"-8.4",0x2e},{"-7.3",0x2c},{"-6.1",0x2a},{"-4.9",0x28},{"-3.8",0x26},{"-2.5",0x24},{"-1.3",0x22},
                                                      {"0",0},{"1.3", 0x2},{"2.5",0x4},{"3.8",0x6},{"4.9",0x8},{"6.1",0xa},{"7.3",0xc},{"8.4",0xe},{"10.6",0x10},
                                                      {"11.7",0x12},{"12.7",0x14},{"13.7",0x16},{"14.7",0x18},{"15.7",0x1a},{"16.7",0x1c},{"17.6",0x1e}};

void SerialCommsWidget::setGain() {
    std::string gain = gainInput->currentText().toStdString();
    if(gainMap.count(gain) > 0) {
        richarduino.poke(0xffffffc4, gainMap.at(gain));
    }
}

void SerialCommsWidget::setOffset() {
    std::string offset = offsetInput->currentText().toStdString();
    if(offsetMap.count(offset) > 0) {
        richarduino.poke(0xffffffc4, offsetMap.at(offset));
    }
}

void SerialCommsWidget::setTrigger() {
    QString trigger = triggerInput->currentText();
    if(trigger == "Rising Edge") {
        triggerMode = TriggerMode::risingEdge;
        std::cout << "Trigger mode set to " << trigger.toStdString() << std::endl;
    } else if(trigger == "Falling Edge") {
        triggerMode = TriggerMode::fallingEdge;
        std::cout << "Trigger mode set to " << trigger.toStdString() << std::endl;
    } else if(trigger == "Level") {
        triggerMode = TriggerMode::level;
        std::cout << "Trigger mode set to " << trigger.toStdString() << std::endl;
    } else if(trigger == "Disabled") {
        triggerMode = TriggerMode::disabled;
        std::cout << "Trigger mode set to " << trigger.toStdString() << std::endl;
    } else {
        std::cout << "Setting trigger (" << trigger.toStdString() << ") not yet implemented" << std::endl;
    }
}

void SerialCommsWidget::setLevel(int levelx10) {
    const float voltageLevel = levelx10 / 10.0;
    this->triggerLevel = ((voltageLevel / 3.3) + 1.0) * 127;  // -3.3 to 3.3 -> 0 to 255
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

SerialCommsWidget::~SerialCommsWidget() {
    stopScope();
}
