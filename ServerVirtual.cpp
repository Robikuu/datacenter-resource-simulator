#include "ServerVirtual.h"

#include <iomanip>
#include <sstream>

ServerVirtual::ServerVirtual(const std::string &nume, long long memorie, float frecventa,
                             int cores, bool online, const std::string &idResursa,
                             double costPeOra, const std::string &virtualizare,
                             float procent, int iops)
    : Server(nume, memorie, frecventa, cores, online, idResursa, costPeOra),
      tipVirtualizare(virtualizare.empty() ? "KVM" : virtualizare),
      procentResurse(procent > 0.0f && procent <= 100.0f ? procent : 100.0f),
      limiteIOPS(iops > 0 ? iops : 1000) {
    std::cout << "[Constructor ServerVirtual]" << std::endl;
}

// Copiem tipVirtualizare direct
ServerVirtual::ServerVirtual(const ServerVirtual &copie)
    : Server(copie), tipVirtualizare(copie.tipVirtualizare),
      procentResurse(copie.procentResurse), limiteIOPS(copie.limiteIOPS) {
}

// Atribuim tipVirtualizare direct
ServerVirtual &ServerVirtual::operator=(const ServerVirtual &copie) {
    if (this == &copie)
        return *this;

    Server::operator=(copie);

    this->tipVirtualizare = copie.tipVirtualizare;
    this->procentResurse = copie.procentResurse;
    this->limiteIOPS = copie.limiteIOPS;

    return *this;
}

// Destructorul nu mai face stergere manuala pe tipVirtualizare
ServerVirtual::~ServerVirtual() noexcept {
    std::cout << "[Destructor ServerVirtual] virtualizare=" << this->tipVirtualizare << std::endl;
}

void ServerVirtual::afiseazaDetalii() const {
    Server::afiseazaDetalii();
    std::cout << "--- Detalii Virtual ---" << std::endl;
    std::cout << "Tip Virtualizare: " << this->tipVirtualizare << std::endl;
    std::cout << "Procent Resurse: " << this->procentResurse << "%" << std::endl;
    std::cout << "Limite IOPS: " << this->limiteIOPS << std::endl;
}

double ServerVirtual::calculeazaCost() const {
    return Server::calculeazaCost() * (this->procentResurse / 100.0);
}

void ServerVirtual::afiseazaStatus() const {
    Server::afiseazaStatus();
    std::cout << "Virtualizare: " << this->tipVirtualizare
              << " | Resurse: " << this->procentResurse << "%"
              << " | IOPS: " << this->limiteIOPS << std::endl;
}

std::string ServerVirtual::toString() const {
    std::ostringstream oss;
    oss << Server::toString();
    std::string bazaStr = oss.str();
    // substr() de la std::string
    std::string info = bazaStr.substr(0, bazaStr.size() - 1);
    info += ", virtualizare=" + this->tipVirtualizare +
            ", resurse=" + std::to_string(static_cast<int>(this->procentResurse)) + "%" +
            ", IOPS=" + std::to_string(this->limiteIOPS) + "]";
    return info;
}

const std::string &ServerVirtual::getTipVirtualizare() const { return this->tipVirtualizare; }
float ServerVirtual::getProcentResurse() const { return this->procentResurse; }
int ServerVirtual::getLimiteIOPS() const { return this->limiteIOPS; }

void ServerVirtual::setTipVirtualizare(const std::string &tipNou) {
    if (tipNou.empty())
        return;
    this->tipVirtualizare = tipNou;
}

void ServerVirtual::setProcentResurse(float procent) {
    this->procentResurse = (procent > 0.0f && procent <= 100.0f) ? procent : 100.0f;
}

void ServerVirtual::setLimiteIOPS(int iops) {
    this->limiteIOPS = iops > 0 ? iops : 1000;
}

std::ostream &operator<<(std::ostream &os, const ServerVirtual &sv) {
    os << static_cast<const Server &>(sv);
    os << "Tip Virtualizare: " << sv.tipVirtualizare << "\n";
    os << "Procent Resurse: " << sv.procentResurse << "%\n";
    os << "Limite IOPS: " << sv.limiteIOPS << "\n";
    return os;
}

std::istream &operator>>(std::istream &is, ServerVirtual &sv) {
    is >> static_cast<Server &>(sv);
    if (!is)
        return is;

    std::string idRes;
    std::cout << "Introduceti ID Resursa: ";
    is >> idRes;
    if (!is)
        return is;
    sv.setIdResursa(idRes);

    std::cout << "Introduceti cost pe ora (RON, > 0): ";
    double cost;
    if (!(is >> cost))
        return is;
    sv.setCostPeOra(cost);

    std::string virtualizare;
    std::cout << "Introduceti tip virtualizare (KVM/Xen/VMware): ";
    is >> virtualizare;
    if (!is)
        return is;
    sv.tipVirtualizare = virtualizare;

    std::cout << "Procent resurse (0-100): ";
    float procent;
    if (!(is >> procent))
        return is;
    sv.procentResurse = (procent > 0.0f && procent <= 100.0f) ? procent : 100.0f;

    std::cout << "Limite IOPS (> 0): ";
    int iops;
    if (!(is >> iops))
        return is;
    sv.limiteIOPS = iops > 0 ? iops : 1000;

    return is;
}
