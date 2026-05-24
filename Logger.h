#pragma once

#include <string>
#include <vector>

// Logger tip Meyers Singleton!
// Fara copy si assignment, n-ai voie sa il duplici!

class Logger {
private:
    std::vector<std::string> jurnal;
    int contorEvenimente;
    Logger() : contorEvenimente(0) {}

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance();

    void log(const std::string& mesaj);

    void afiseazaJurnal() const;

    void goleste();

    int getNrInregistrari() const noexcept;
};
