#pragma once

#include "ExceptiiCloud.h"
#include <string>

// Depozitar template cu parametru non-tip N.
// ATENTIE: Depozitar NU sterge obiectele la destructor!

template <typename T, int N> class Depozitar {
private:
  T *elemente[N];
  int numarElemente;

public:
  Depozitar() : numarElemente(0) {
    for (int i = 0; i < N; i++) {
      elemente[i] = nullptr;
    }
  }

  // Destructorul nu sterge elementele
  ~Depozitar() noexcept = default;

  // Adauga element, arunca ExceptieCapacitateDepasita daca e plin
  void adauga(T *element) {
    if (numarElemente >= N) {
      throw ExceptieCapacitateDepasita(
          "Depozitar plin: capacitate maxima " + std::to_string(N) +
              " atinsa (elemente curente: " + std::to_string(numarElemente) +
              ")",
          N, numarElemente + 1, "Depozitar::adauga");
    }
    elemente[numarElemente] = element;
    numarElemente++;
  }

  // Elimina element, arunca ExceptieValidare la index gresit
  void elimina(int index) {
    if (index < 0 || index >= numarElemente) {
      throw ExceptieValidare("Index invalid (" + std::to_string(index) +
                                 ") in Depozitar::elimina (elemente: " +
                                 std::to_string(numarElemente) + ")",
                             std::to_string(index), "Depozitar::elimina");
    }
    for (int i = index; i < numarElemente - 1; i++) {
      elemente[i] = elemente[i + 1];
    }
    elemente[numarElemente - 1] = nullptr;
    numarElemente--;
  }

  // Get element, arunca ExceptieValidare la index gresit
  T *get(int index) const {
    if (index < 0 || index >= numarElemente) {
      throw ExceptieValidare("Index invalid (" + std::to_string(index) +
                                 ") in Depozitar::get (elemente: " +
                                 std::to_string(numarElemente) + ")",
                             std::to_string(index), "Depozitar::get");
    }
    return elemente[index];
  }

  int getNr() const noexcept { return numarElemente; }
  bool estePlin() const noexcept { return numarElemente >= N; }
  bool esteGol() const noexcept { return numarElemente == 0; }

  // Pointer la array-ul de pointeri
  T **getElemente() { return elemente; }
  T *const *getElemente() const { return elemente; }

  // Copiaza intr-un container extern
  template <typename U> void copiazaIn(U &container) const {
    for (int i = 0; i < numarElemente; i++) {
      container.push_back(elemente[i]);
    }
  }
};
