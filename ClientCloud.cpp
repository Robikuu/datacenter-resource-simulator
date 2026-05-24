#include "ClientCloud.h"

ClientCloud::ClientCloud(FabricaProviderAbstract *f) : fabrica(f) {}

void ClientCloud::setFabrica(FabricaProviderAbstract *f) { fabrica = f; }

FabricaProviderAbstract *ClientCloud::getFabrica() const { return fabrica; }

ResursaCloud *ClientCloud::creeazaServerDedicat() const {
  if (!fabrica)
    return nullptr;
  IFabricaServerDedicat *factoryDedicat = fabrica->getFabricaDedicat();
  if (!factoryDedicat)
    return nullptr;
  return factoryDedicat->creeaza();
}

ResursaCloud *ClientCloud::creeazaServerVirtual() const {
  if (!fabrica)
    return nullptr;
  IFabricaServerVirtual *factoryVirtual = fabrica->getFabricaVirtual();
  if (!factoryVirtual)
    return nullptr;
  return factoryVirtual->creeaza();
}
