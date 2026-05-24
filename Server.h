#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ResursaCloud.h"

// numeServer e std::string. frecventaCores e pointer raw deci Rule of 3 ramane obligatoriu!

class Server : public ResursaCloud {
  private:
    std::string numeServer;
    long long memorieRam;  // capacitatea in BYTES!!!!!!
    float *frecventaCores; // frecventa pentru fiecare core
    int numarCores;        // numar de coruri alocate serverului
    bool esteOnline;
    int numarVMAlocate;
    static const int MAX_CORES = 256;
    static int numarServere;

  public:
    explicit Server(const std::string &nume = "DEFAULT", long long memorie = 1,
                    float frecventa = 0.01f, int cores = 1, bool online = false,
                    const std::string &idResursa = "SRV_DEFAULT", double costPeOra = 0.0);

    // Rule of three obligatoriu pt frecventaCores!
    Server(const Server &copie);
    Server &operator=(const Server &copie);
    ~Server() noexcept override;

    friend std::ostream &operator<<(std::ostream &os, const Server &server);
    friend std::istream &operator>>(std::istream &is, Server &server);

    void setNumeServer(const std::string &numeNou);
    void setMemorieRam(long long memorieNoua);
    void setFreceventaCoreIndex(int coreIndex, float frecventa);
    void setNumarCores(int coresNoi);
    void setEsteOnline(bool statusOnline);

    float &operator[](int index);
    const float &operator[](int index) const;

    Server operator+(int coresAdunate) const;
    Server operator-(int coresScazute) const;
    Server &operator++();
    Server operator++(int);

    bool operator<(const Server &alt) const;
    bool operator>(const Server &alt) const;
    bool operator==(const Server &alt) const;

    const std::string &getNumeServer() const;
    long long getMemorieRam() const;
    double memorieRamToGB() const;
    float getFreceventaCoreIndex(int coreIndex) const;
    int getNumarCores() const;
    bool getEsteOnline() const;
    bool alocaVM();
    void elibereazaVM();
    int getNumarVMAlocate() const;

    void afiseazaDetalii() const override;
    double calculeazaCost() const override;
    void afiseazaStatus() const override;

    // toString() din IObject
    std::string toString() const override;
};

Server operator+(int coresAdunate, const Server &server);
Server operator-(int coresScazute, const Server &server);
