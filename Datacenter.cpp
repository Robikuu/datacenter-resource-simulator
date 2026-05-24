#include "Datacenter.h"
#include "ExceptiiCloud.h"

#include <algorithm>

// Rule of 3 eliminat complet. String-urile si vectorul se ocupa de tot!

Datacenter::Datacenter(const std::string &nume, const std::string &loc,
                       int nrServere, const Server *listaServere)
    : numeDatacenter(nume.empty() ? "DC_DEFAULT" : nume),
      locatie(loc.empty() ? "LOCATIE_DEFAULT" : loc), servere() {

  if (nrServere > 0 && listaServere != nullptr) {
    servere.assign(listaServere, listaServere + nrServere);
  } else if (nrServere > 0) {
    servere.resize(static_cast<std::size_t>(nrServere));
  }
}

void Datacenter::adaugaServer(const Server &serverNou) {
  servere.push_back(serverNou);
}

Server &Datacenter::operator[](int index) {
  if (index < 0 || index >= static_cast<int>(servere.size())) {
    throw ExceptieValidare("Index invalid (" + std::to_string(index) +
                               ") in Datacenter::operator[] (numar servere: " +
                               std::to_string(servere.size()) + ")",
                           std::to_string(index), "operator[] Datacenter");
  }
  return servere[static_cast<std::size_t>(index)];
}

const Server &Datacenter::operator[](int index) const {
  if (index < 0 || index >= static_cast<int>(servere.size())) {
    throw ExceptieValidare(
        "Index invalid (" + std::to_string(index) +
            ") in Datacenter::operator[] const (numar servere: " +
            std::to_string(servere.size()) + ")",
        std::to_string(index), "operator[] const Datacenter");
  }
  return servere[static_cast<std::size_t>(index)];
}

Datacenter Datacenter::operator+(const Server &serverNou) const {
  Datacenter rezultat(*this);
  rezultat.adaugaServer(serverNou);
  return rezultat;
}

Datacenter &Datacenter::operator++() {
  this->adaugaServer(Server());
  return *this;
}

Datacenter Datacenter::operator++(int) {
  Datacenter copie(*this);
  ++(*this);
  return copie;
}

Datacenter Datacenter::operator-(const Server &serverDeScos) const {
  Datacenter rezultat(*this);
  if (rezultat.servere.empty()) {
    return rezultat;
  }

  auto it = std::find_if(
      rezultat.servere.begin(), rezultat.servere.end(),
      [&serverDeScos](const Server &s) { return s == serverDeScos; });

  if (it != rezultat.servere.end()) {
    rezultat.servere.erase(it);
  }

  return rezultat;
}

bool Datacenter::operator<(const Datacenter &dc) const {
  return servere.size() < dc.servere.size();
}

bool Datacenter::operator>(const Datacenter &dc) const { return dc < *this; }

bool Datacenter::operator==(const Datacenter &dc) const {
  if (this->numeDatacenter != dc.numeDatacenter)
    return false;
  if (this->locatie != dc.locatie)
    return false;
  if (this->servere.size() != dc.servere.size())
    return false;

  for (std::size_t i = 0; i < this->servere.size(); i++) {
    if (!(this->servere[i] == dc.servere[i])) {
      return false;
    }
  }

  return true;
}

std::ostream &operator<<(std::ostream &os, const Datacenter &dc) {
  os << "Nume Datacenter: " << dc.numeDatacenter << "\n";
  os << "Locatie: " << dc.locatie << "\n";
  os << "Numar servere: " << dc.servere.size() << "\n";

  for (std::size_t i = 0; i < dc.servere.size(); i++) {
    os << "--- Server [" << i << "] ---\n";
    os << dc.servere[i];
  }
  return os;
}

std::istream &operator>>(std::istream &is, Datacenter &dc) {
  std::string numeNou;
  std::string locatieNoua;
  int nrNou = 0;

  while (true) {
    std::cout << "Introduceti nume datacenter: ";
    is >> numeNou;
    if (!is) {
      return is;
    }
    if (!numeNou.empty()) {
      break;
    }
    std::cout << "Numele datacenter-ului nu poate fi gol.\n";
  }

  while (true) {
    std::cout << "Introduceti locatie datacenter: ";
    is >> locatieNoua;
    if (!is) {
      return is;
    }
    if (!locatieNoua.empty()) {
      break;
    }
    std::cout << "Locatia nu poate fi goala.\n";
  }

  while (true) {
    std::cout << "Introduceti numar servere (>=0): ";
    if (is >> nrNou) {
      if (nrNou >= 0) {
        break;
      }
      std::cout << "Numarul de servere nu poate fi negativ.\n";
    } else {
      if (is.eof()) {
        return is;
      }
      is.clear();
      is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Valoare invalida pentru numar servere.\n";
    }
  }

  std::vector<Server> servereNoi;
  for (int i = 0; i < nrNou; i++) {
    std::cout << "Introduceti date pentru serverul " << i << ":\n";
    Server s;
    if (!(is >> s)) {
      return is;
    }
    servereNoi.push_back(s);
  }

  dc.numeDatacenter = numeNou;
  dc.locatie = locatieNoua;
  dc.servere = servereNoi;

  return is;
}

const std::string &Datacenter::getNumeDatacenter() const {
  return this->numeDatacenter;
}
const std::string &Datacenter::getLocatie() const { return this->locatie; }
int Datacenter::getNumarServere() const {
  return static_cast<int>(this->servere.size());
}
const std::vector<Server> &Datacenter::getServere() const {
  return this->servere;
}

Datacenter operator+(const Server &server, const Datacenter &dc) {
  return dc + server;
}
