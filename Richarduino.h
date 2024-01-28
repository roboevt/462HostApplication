#ifndef RICHARDUINO_H
#define RICHARDUINO_H

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstdio>
// #include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct Richarduino {
    int portFd;
    struct termios tty;
    int baud;

    bool isProgrammed = false;

    Richarduino(std::string location, int baud) : baud(baud) {
        // serial tty code adapted from SerialPort_RevB Connor Monohan 2021
        // Open serial port connection
        portFd = open(location.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (portFd < 0) {
            throw std::runtime_error("Failed to open serial port <" + location + ">");
        }
        // Edit settings
        if (tcgetattr(portFd, &tty) != 0) { /* handle error */
            abort();
        }
        // Set baud rate
        cfsetspeed(&tty, baud);

        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 10;
        tty.c_lflag = 0;  // Non canonical mode, seems to be important

        if (tcsetattr(portFd, TCSANOW, &tty) != 0) {
            throw std::runtime_error("Failed to set serial port attributes");
        }
    }

    ~Richarduino() { close(portFd); }

    void write(std::string data) {
        int num_written = ::write(portFd, data.c_str(), data.size());
        if (num_written < 0 || num_written < data.size()) {  // handle error
            std::cout << "Error: Wrote <" << num_written << "> bytes, expected <" << data.size()
                      << ">" << std::endl;
        }
    }

    void write(uint32_t data) {
        std::vector<char> dataBytes(4);
        // Convert data to bytes (big endian)
        dataBytes[0] = (data >> 24) & 0xFF;
        dataBytes[1] = (data >> 16) & 0xFF;
        dataBytes[2] = (data >> 8) & 0xFF;
        dataBytes[3] = data & 0xFF;
        write(std::string(dataBytes.begin(), dataBytes.end()));
    }

    std::vector<char> read(int n) {
        std::vector<char> data(n);
        int numRead = ::read(portFd, data.data(), n);

        if (numRead == n) {
            return data;
        }

        // Did not read expected number of bytes
        std::cout << "Error: Read <" << numRead << "> bytes, expected <" << n << ">" << std::endl;

        if (numRead > 0) {
            return std::vector<char>(data.begin(), data.begin() + numRead);
        }

        // No bytes read
        perror("Error reading from serial port");
        return std::vector<char>(n, '0');
    }

    int version() {
        if(!isProgrammed) {
            write("V");
            std::vector<char> response = read(1);
            return std::stoi(std::string(response.begin(), response.end()));
        } else {
            std::cout << "Error: Can not check version after programming device" << std::endl;
            return -1;
        }
    }

    void program(std::vector<uint32_t> program) {
        std::cout << "Uploading program..." << std::endl;

        int programLength = program.size() * 4;  // 4 bytes per instruction

        std::vector<char> programLengthBytes(4);
        // Convert program length to bytes (big endian)
        programLengthBytes[0] = (programLength >> 24) & 0xFF;
        programLengthBytes[1] = (programLength >> 16) & 0xFF;
        programLengthBytes[2] = (programLength >> 8) & 0xFF;
        programLengthBytes[3] = programLength & 0xFF;
        std::string programLengthString(programLengthBytes.begin(), programLengthBytes.end());

        // Program command
        write("P");
        write(programLengthString);

        // Program data
        for (uint32_t instruction : program) {
            write(instruction);
        }

        isProgrammed = true;
        std::cout << "Done uploading!" << std::endl;
    }

    void poke(uint32_t addr, uint32_t data) {
        write("W");
        write(addr);
        write(data);
    }

    int peek(uint32_t addr) {
        write("R");
        write(addr);

        std::vector<char> response = read(4);
        int responseInt = 0;
        // Convert response to int (big endian)
        responseInt |= (response[0] & 0xff) << 24;
        responseInt |= (response[1] & 0xff) << 16;
        responseInt |= (response[2] & 0xff) << 8;
        responseInt |= (response[3] & 0xff);

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Give time to respond
        return responseInt;
    }
};

#endif // RICHARDUINO_H
