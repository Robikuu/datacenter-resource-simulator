#pragma once

#include "Server.h"

// tipVirtualizare e std::string. Rule of 3 mostenit de la Server!

class ServerVirtual : public Server {
  private:
    // private: detalii specifice serverului virtual/cloud,
    // nu sunt necesare in eventuale clase derivate.
    std::string tipVirtualizare;  // "KVM", "Xen", "VMware"
    float procentResurse;   // % din resursele hostului fizic (0-100)
    int limiteIOPS;         // limita de operatii I/O pe secunda

  public:
    explicit ServerVirtual(const std::string &nume = "VIRTUAL_DEFAULT", long long memorie = 1,
                           float frecventa = 0.01f, int cores = 1, bool online = false,
                           const std::string &idResursa = "VIRT_DEFAULT", double costPeOra = 0.0,
                           const std::string &virtualizare = "KVM", float procent = 100.0f,
                           int iops = 1000);

    ServerVirtual(const ServerVirtual &copie);
    ServerVirtual &operator=(const ServerVirtual &copie);
    ~ServerVirtual() noexcept override;

    // Override functii virtuale
    void afiseazaDetalii() const override;
    double calculeazaCost() const override;
    void afiseazaStatus() const override;

    // toString() din IObject
    std::string toString() const override;

    // Getters
    const std::string &getTipVirtualizare() const;
    float getProcentResurse() const;
    int getLimiteIOPS() const;

    // Setters
    void setTipVirtualizare(const std::string &tipNou);
    void setProcentResurse(float procent);
    void setLimiteIOPS(int iops);

    friend std::ostream &operator<<(std::ostream &os, const ServerVirtual &sv);
    friend std::istream &operator>>(std::istream &is, ServerVirtual &sv);
};
