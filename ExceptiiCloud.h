#pragma once

// Exceptii custom pentru Cloud. Toate mostenesc std::runtime_error.

#include <sstream>
#include <stdexcept>
#include <string>

// ExceptieCloud: Exceptia de baza, are campul operatie.
class ExceptieCloud : public std::runtime_error {
private:
  std::string operatie; // ce operatie a crapat

public:
  explicit ExceptieCloud(const std::string &mesaj,
                         const std::string &op = "necunoscuta")
      : std::runtime_error(mesaj), operatie(op) {}

  ~ExceptieCloud() noexcept override = default;

  const char *what() const noexcept override {
    return std::runtime_error::what();
  }

  const std::string &getOperatie() const noexcept { return operatie; }
};

// ExceptieValidare: pentru date invalide (IP gresit, cores < 1, etc.)
class ExceptieValidare : public ExceptieCloud {
private:
  std::string valoareInvalida; // valoarea care nu e buna

public:
  explicit ExceptieValidare(const std::string &mesaj,
                            const std::string &valoare = "",
                            const std::string &op = "validare")
      : ExceptieCloud(mesaj, op), valoareInvalida(valoare) {}

  ~ExceptieValidare() noexcept override = default;

  const char *what() const noexcept override { return ExceptieCloud::what(); }

  const std::string &getValoareInvalida() const noexcept {
    return valoareInvalida;
  }
};

// ExceptieResursaIndisponibila: cand nu e online serverul sau e ocupat
class ExceptieResursaIndisponibila : public ExceptieCloud {
private:
  std::string idResursa; // ce resursa a crapat

public:
  explicit ExceptieResursaIndisponibila(const std::string &mesaj,
                                        const std::string &id = "",
                                        const std::string &op = "acces_resursa")
      : ExceptieCloud(mesaj, op), idResursa(id) {}

  ~ExceptieResursaIndisponibila() noexcept override = default;

  const char *what() const noexcept override { return ExceptieCloud::what(); }

  const std::string &getIdResursa() const noexcept { return idResursa; }
};

// ExceptieCapacitateDepasita: cand sari peste limita maxima
class ExceptieCapacitateDepasita : public ExceptieCloud {
private:
  int limita;        // limita maxima
  int valoareCeruta; // cat s-a cerut

public:
  explicit ExceptieCapacitateDepasita(const std::string &mesaj, int lim = 0,
                                      int ceruta = 0,
                                      const std::string &op = "alocare")
      : ExceptieCloud(mesaj, op), limita(lim), valoareCeruta(ceruta) {}

  ~ExceptieCapacitateDepasita() noexcept override = default;

  const char *what() const noexcept override { return ExceptieCloud::what(); }

  int getLimita() const noexcept { return limita; }
  int getValoareCeruta() const noexcept { return valoareCeruta; }
};
