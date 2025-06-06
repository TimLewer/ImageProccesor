#pragma once

#include <stdexcept>
#include <iostream>

class BMPFormatError : public std::runtime_error {
public:
    explicit BMPFormatError(const std::string& message) : std::runtime_error("BMP Format Error: " + message) {
        std::cerr << what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
};

class FilterError : public std::runtime_error {
public:
    explicit FilterError(const std::string& message) : std::runtime_error("Filter Error: " + message) {
        std::cerr << what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
};

class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& message) : std::runtime_error("File Error: " + message) {
        std::cerr << what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
};

class SystemError : public std::runtime_error {
public:
    explicit SystemError(const std::string& message) : std::runtime_error("File Error: " + message) {
        std::cerr << what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
};