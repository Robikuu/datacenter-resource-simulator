#include "ResursaCloud.h"
#include "Logger.h"

int ResursaCloud::contorResurse = 0;

ResursaCloud::ResursaCloud(const std::string &id, double cost)
    : IObject(), idResursa(id.empty() ? "RES_DEFAULT" : id),
      costPeOra(cost >= 0.0 ? cost : 0.0) {
    std::cout << "[Constructor ResursaCloud]" << std::endl;
    contorResurse++;
    Logger::getInstance().log("ResursaCloud creata: id=" + this->idResursa +
                              ", cost=" + std::to_string(this->costPeOra));
}

// string-ul se copiaza singur
ResursaCloud::ResursaCloud(const ResursaCloud &copie)
    : IObject(copie), idResursa(copie.idResursa), costPeOra(copie.costPeOra) {
    contorResurse++;
}

// atribuire automata pt string
ResursaCloud &ResursaCloud::operator=(const ResursaCloud &copie) {
    if (this == &copie)
        return *this;

    this->idResursa = copie.idResursa;
    this->costPeOra = copie.costPeOra;

    return *this;
}

// string-ul se sterge singur din memorie
ResursaCloud::~ResursaCloud() noexcept {
    std::cout << "[Destructor ResursaCloud] id=" << this->idResursa << std::endl;
    Logger::getInstance().log("ResursaCloud distrusa: id=" + this->idResursa);
    contorResurse--;
}

void ResursaCloud::afiseazaStatus() const {
    std::cout << "=== Status Resursa Cloud ===" << std::endl;
    std::cout << "ID Resursa: " << this->idResursa << std::endl;
    std::cout << "Cost pe ora: " << this->costPeOra << " RON" << std::endl;
}

const std::string &ResursaCloud::getIdResursa() const { return this->idResursa; }

double ResursaCloud::getCostPeOra() const { return this->costPeOra; }

void ResursaCloud::setIdResursa(const std::string &idNou) {
    if (idNou.empty())
        return;
    this->idResursa = idNou;
}

void ResursaCloud::setCostPeOra(double costNou) {
    this->costPeOra = costNou >= 0.0 ? costNou : 0.0;
}

int ResursaCloud::getContorResurse() { return contorResurse; }
