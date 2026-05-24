#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "IP.h"
#include "ExceptiiCloud.h"

class Server;

// numeInstanta e std::string. consumCPU e float* raw deci Rule of 3 ramane obligatoriu!

class MasinaVirtuala {
  private:
    std::string numeInstanta;
    int numarCores;
    long long memorieRam;
    IP ipAlocat;
    Server *serverGazda; // primit de la server, nu il detine VM-ul!
    bool estePornita;
    float *consumCPU;

    static const int MAX_CORES = 32;
    static int contorVMs;

    static int valideazaCores(int cores) {
        if (cores <= 0) {
            throw ExceptieValidare("Numar de cores prea mic in VM", std::to_string(cores), "valideazaCores VM");
        }
        if (cores > MAX_CORES) {
            throw ExceptieCapacitateDepasita("Numar de cores depaseste MAX in VM", MAX_CORES, cores);
        }
        return cores;
    }

    static long long valideazaMemorie(long long memorie) {
        if (memorie <= 0) {
            throw ExceptieValidare("Memorie RAM prea mica in VM", std::to_string(memorie), "valideazaMemorie VM");
        }
        return memorie;
    }

  public:
    // SERVERUL SE PASEAZA CA POINTER!!!!!!!!!!!!!!!!!!!!!!!
    explicit MasinaVirtuala(const std::string &nume = "VM_DEFAULT", int cores = 1,
                            long long memorie = 1, const IP &ip = IP(),
                            Server *server = nullptr, bool pornita = false);

    // Rule of three obligatoriu pt consumCPU!
    MasinaVirtuala(const MasinaVirtuala &copie);
    MasinaVirtuala &operator=(const MasinaVirtuala &copie);
    ~MasinaVirtuala() noexcept;

    void setNumeInstanta(const std::string &numeNou);
    void setNumarCores(int coresNoi);
    void setMemorieRam(long long memorieNoua);
    void setIpAlocat(const IP &ipNou);
    void setServerGazda(Server *serverNou);
    void setEstePornita(bool stareNoua);
    void setConsumCPUIndex(int index, float consumNou);

    const std::string &getNumeInstanta() const;
    int getNumarCores() const;
    long long getMemorieRam() const;
    const IP &getIpAlocat() const;
    Server *getServerGazda() const;
    bool getEstePornita() const;
    float getConsumCPUIndex(int index) const;
    const float *getConsumCPU() const;
    static int getContorVMs();

    float &operator[](int index);
    const float &operator[](int index) const;

    MasinaVirtuala &operator++();
    MasinaVirtuala operator++(int);

    bool operator<(const MasinaVirtuala &alt) const;
    bool operator>(const MasinaVirtuala &alt) const;
    bool operator==(const MasinaVirtuala &alt) const;

    MasinaVirtuala operator+(const MasinaVirtuala &alt) const;
    MasinaVirtuala operator-(const MasinaVirtuala &alt) const;
    MasinaVirtuala operator+(int coresExtra) const;
    MasinaVirtuala operator-(int coresScazute) const;

    friend std::ostream &operator<<(std::ostream &os, const MasinaVirtuala &vm);
    friend std::istream &operator>>(std::istream &is, MasinaVirtuala &vm);
};

MasinaVirtuala operator+(int val, const MasinaVirtuala &vm);
