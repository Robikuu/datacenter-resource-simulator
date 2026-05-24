#pragma once

#include <iostream>
#include <string>

#include "IObject.h"

// Clasa abstracta pentru resurse cloud. Mosteneste IObject.
// idResursa e std::string deci nu e nevoie de Rule of 3 manual pt string.

class ResursaCloud : public IObject {
private:
  static int contorResurse;

protected:
  // protected ca sa fie vazute de Server, ServerDedicat, etc.
  std::string idResursa;
  double costPeOra;

public:
  explicit ResursaCloud(const std::string &id = "RES_DEFAULT", double cost = 0.0);
  ResursaCloud(const ResursaCloud &copie);
  ResursaCloud &operator=(const ResursaCloud &copie);

  // Destructor virtual obligatoriu pt delete prin pointer la baza!
  virtual ~ResursaCloud() noexcept;

  // Functii pur virtuale
  virtual void afiseazaDetalii() const = 0;
  virtual double calculeazaCost() const = 0;

  // Metoda concreta apelata si extinsa de derivate
  virtual void afiseazaStatus() const;

  const std::string &getIdResursa() const;
  double getCostPeOra() const;
  void setIdResursa(const std::string &idNou);
  void setCostPeOra(double costNou);
  static int getContorResurse();
};
