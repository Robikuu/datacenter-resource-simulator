#include "Logger.h"

#include <iostream>

// Instantiat o singura data la primul apel
Logger& Logger::getInstance() {
    static Logger instanta;
    return instanta;
}

void Logger::log(const std::string& mesaj) {
    contorEvenimente++;
    std::string intrare = "[EVENT " + std::to_string(contorEvenimente) + "] " + mesaj;
    jurnal.push_back(intrare);
}

void Logger::afiseazaJurnal() const {
    std::cout << "\n===== JURNAL SISTEM (Logger Singleton) =====\n";
    if (jurnal.empty()) {
        std::cout << "(jurnalul este gol)\n";
    } else {
        for (const auto& intrare : jurnal) {
            std::cout << intrare << "\n";
        }
    }
    std::cout << "Total inregistrari: " << jurnal.size() << "\n";
    std::cout << "=============================================\n";
}

void Logger::goleste() {
    jurnal.clear();
}

int Logger::getNrInregistrari() const noexcept {
    return static_cast<int>(jurnal.size());
}
