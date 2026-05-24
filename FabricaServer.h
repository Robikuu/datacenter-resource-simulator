#pragma once

#include "ServerDedicat.h"
#include "ServerVirtual.h"
#include <string>

// --- Abstract Product Interfaces ---

class IFabricaServerDedicat {
public:
  virtual ServerDedicat *creeaza() const = 0;
  virtual ~IFabricaServerDedicat() = default;
};

class IFabricaServerVirtual {
public:
  virtual ServerVirtual *creeaza() const = 0;
  virtual ~IFabricaServerVirtual() = default;
};

// --- Abstract Factory ---

class FabricaProviderAbstract {
public:
  virtual IFabricaServerDedicat *getFabricaDedicat() const = 0;
  virtual IFabricaServerVirtual *getFabricaVirtual() const = 0;
  virtual std::string getNumeProvider() const = 0;
  virtual ~FabricaProviderAbstract() = default;
};

// --- Concrete Implementations for Provider A (AWS-style) ---

class FabricaDedicatA : public IFabricaServerDedicat {
public:
  ServerDedicat *creeaza() const override;
};

class FabricaVirtualA : public IFabricaServerVirtual {
public:
  ServerVirtual *creeaza() const override;
};

class FabricaProviderA : public FabricaProviderAbstract {
private:
  FabricaDedicatA fabricaDedicat;
  FabricaVirtualA fabricaVirtual;

public:
  IFabricaServerDedicat *getFabricaDedicat() const override;
  IFabricaServerVirtual *getFabricaVirtual() const override;
  std::string getNumeProvider() const override;
};

// --- Concrete Implementations for Provider B (Azure-style) ---

class FabricaDedicatB : public IFabricaServerDedicat {
public:
  ServerDedicat *creeaza() const override;
};

class FabricaVirtualB : public IFabricaServerVirtual {
public:
  ServerVirtual *creeaza() const override;
};

class FabricaProviderB : public FabricaProviderAbstract {
private:
  FabricaDedicatB fabricaDedicat;
  FabricaVirtualB fabricaVirtual;

public:
  IFabricaServerDedicat *getFabricaDedicat() const override;
  IFabricaServerVirtual *getFabricaVirtual() const override;
  std::string getNumeProvider() const override;
};
