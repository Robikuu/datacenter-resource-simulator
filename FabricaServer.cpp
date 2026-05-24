#include "FabricaServer.h"

// --- Provider A (AWS-style) ---

// AWS bare metal
ServerDedicat *FabricaDedicatA::creeaza() const {
  return new ServerDedicat("AWS_Metal",               // nume
                           64LL * 1024 * 1024 * 1024, // memorie (64GB)
                           3.2f,                      // frecventa
                           32,                        // cores
                           true,                      // online
                           "AWS_DED",                 // idResursa
                           10.5,                      // costPeOra
                           "EPYC",                    // procesor
                           true,                      // raid
                           24                         // garantie
  );
}

// AWS masina virtuala
ServerVirtual *FabricaVirtualA::creeaza() const {
  return new ServerVirtual("AWS_EC2",                 // nume
                           16LL * 1024 * 1024 * 1024, // memorie (16GB)
                           3.0f,                      // frecventa
                           8,                         // cores
                           true,                      // online
                           "AWS_VIRT",                // idResursa
                           2.5,                       // costPeOra
                           "KVM",                     // virtualizare
                           80.0f,                     // procentResurse
                           5000                       // limiteIOPS
  );
}

IFabricaServerDedicat *FabricaProviderA::getFabricaDedicat() const {
  return const_cast<FabricaDedicatA *>(&fabricaDedicat);
}

IFabricaServerVirtual *FabricaProviderA::getFabricaVirtual() const {
  return const_cast<FabricaVirtualA *>(&fabricaVirtual);
}

std::string FabricaProviderA::getNumeProvider() const {
  return "Provider A (AWS-style)";
}

// --- Provider B (Azure-style) ---

// Azure bare metal
ServerDedicat *FabricaDedicatB::creeaza() const {
  return new ServerDedicat("Azure_BareMetal",          // nume
                           128LL * 1024 * 1024 * 1024, // memorie (128GB)
                           2.8f,                       // frecventa
                           16,                         // cores
                           true,                       // online
                           "AZU_DED",                  // idResursa
                           12.0,                       // costPeOra
                           "Xeon",                     // procesor
                           false,                      // raid
                           12                          // garantie
  );
}

// Azure masina virtuala
ServerVirtual *FabricaVirtualB::creeaza() const {
  return new ServerVirtual("Azure_VM",                // nume
                           32LL * 1024 * 1024 * 1024, // memorie (32GB)
                           2.5f,                      // frecventa
                           4,                         // cores
                           true,                      // online
                           "AZU_VIRT",                // idResursa
                           3.0,                       // costPeOra
                           "VMware",                  // virtualizare
                           50.0f,                     // procentResurse
                           2000                       // limiteIOPS
  );
}

IFabricaServerDedicat *FabricaProviderB::getFabricaDedicat() const {
  return const_cast<FabricaDedicatB *>(&fabricaDedicat);
}

IFabricaServerVirtual *FabricaProviderB::getFabricaVirtual() const {
  return const_cast<FabricaVirtualB *>(&fabricaVirtual);
}

std::string FabricaProviderB::getNumeProvider() const {
  return "Provider B (Azure-style)";
}
