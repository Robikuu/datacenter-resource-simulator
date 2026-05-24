#pragma once

#include "Server.h"

// tipProcesor e std::string. Rule of 3 mostenit de la Server!

class ServerDedicat : public Server {
  private:
    // private: detalii specifice serverului dedicat, nu sunt necesare
    // in eventuale clase derivate din ServerDedicat.
    std::string tipProcesor;   // "Xeon", "EPYC", "Threadripper"
    bool areRAID;
    int garantieLuni;    // garantie in luni

  public:
    explicit ServerDedicat(const std::string &nume = "DEDICAT_DEFAULT", long long memorie = 1,
                           float frecventa = 0.01f, int cores = 1, bool online = false,
                           const std::string &idResursa = "DED_DEFAULT", double costPeOra = 0.0,
                           const std::string &procesor = "Generic", bool raid = false,
                           int garantie = 12);

    ServerDedicat(const ServerDedicat &copie);
    ServerDedicat &operator=(const ServerDedicat &copie);
    ~ServerDedicat() noexcept override;

    // Override functii virtuale
    void afiseazaDetalii() const override;
    double calculeazaCost() const override;
    void afiseazaStatus() const override;

    // toString() din IObject
    std::string toString() const override;

    // Getters
    const std::string &getTipProcesor() const;
    bool getAreRAID() const;
    int getGarantieLuni() const;

    // Setters
    void setTipProcesor(const std::string &procesorNou);
    void setAreRAID(bool raid);
    void setGarantieLuni(int luni);

    friend std::ostream &operator<<(std::ostream &os, const ServerDedicat &sd);
    friend std::istream &operator>>(std::istream &is, ServerDedicat &sd);
};
