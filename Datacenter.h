#pragma once

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "Server.h"

// servere este acum std::vector<Server>: O(1) la push_back, O(n) la cautare.
// Clasa are doar string si vector, deci Rule of 3 nu mai e necesar!

class Datacenter {
private:
  std::string numeDatacenter;
  std::string locatie;
  // std::vector<Server>: O(1) amortizat la push_back, O(n) cautare.
  std::vector<Server> servere;

public:
  explicit Datacenter(const std::string &nume = "DC_DEFAULT",
                      const std::string &loc = "LOCATIE_DEFAULT",
                      int nrServere = 0, const Server *listaServere = nullptr);

  // Rule of three eliminat complet, compilatorul face tot automat!

  void adaugaServer(const Server &serverNou);

  Server &operator[](int index);
  const Server &operator[](int index) const;

  Datacenter operator+(const Server &serverNou) const;
  Datacenter &operator++();
  Datacenter operator++(int);
  Datacenter operator-(const Server &serverDeScos) const;

  bool operator<(const Datacenter &dc) const;
  bool operator>(const Datacenter &dc) const;
  bool operator==(const Datacenter &dc) const;

  friend std::ostream &operator<<(std::ostream &os, const Datacenter &dc);
  friend std::istream &operator>>(std::istream &is, Datacenter &dc);

  const std::string &getNumeDatacenter() const;
  const std::string &getLocatie() const;
  int getNumarServere() const;
  const std::vector<Server> &getServere() const;
};

Datacenter operator+(const Server &server, const Datacenter &dc);
