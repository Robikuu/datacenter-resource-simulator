#pragma once

#include "FabricaServer.h"
#include "ResursaCloud.h"

// Primeste fabrica prin dependency injection si foloseste interfete abstracte
class ClientCloud {
private:
  FabricaProviderAbstract *fabrica; // injected, not owned

public:
  explicit ClientCloud(FabricaProviderAbstract *f);

  void setFabrica(FabricaProviderAbstract *f);
  FabricaProviderAbstract *getFabrica() const;

  ResursaCloud *creeazaServerDedicat() const;
  ResursaCloud *creeazaServerVirtual() const;
};
