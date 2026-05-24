#include "ServerDedicat.h"

#include <iomanip>
#include <sstream>

ServerDedicat::ServerDedicat(const std::string &nume, long long memorie, float frecventa,
                             int cores, bool online, const std::string &idResursa,
                             double costPeOra, const std::string &procesor, bool raid,
                             int garantie)
    : Server(nume, memorie, frecventa, cores, online, idResursa, costPeOra),
      tipProcesor(procesor.empty() ? "Generic" : procesor), areRAID(raid),
      garantieLuni(garantie > 0 ? garantie : 1) {
    std::cout << "[Constructor ServerDedicat]" << std::endl;
}

// Copiem tipProcesor direct
ServerDedicat::ServerDedicat(const ServerDedicat &copie)
    : Server(copie), tipProcesor(copie.tipProcesor), areRAID(copie.areRAID),
      garantieLuni(copie.garantieLuni) {
}

// Atribuim tipProcesor direct
ServerDedicat &ServerDedicat::operator=(const ServerDedicat &copie) {
    if (this == &copie)
        return *this;

    Server::operator=(copie);

    this->tipProcesor = copie.tipProcesor;
    this->areRAID = copie.areRAID;
    this->garantieLuni = copie.garantieLuni;

    return *this;
}

// Destructorul nu mai face stergere manuala pe tipProcesor
ServerDedicat::~ServerDedicat() noexcept {
    std::cout << "[Destructor ServerDedicat] procesor=" << this->tipProcesor << std::endl;
}

void ServerDedicat::afiseazaDetalii() const {
    Server::afiseazaDetalii();
    std::cout << "--- Detalii Dedicat ---" << std::endl;
    std::cout << "Tip Procesor: " << this->tipProcesor << std::endl;
    std::cout << std::boolalpha << "Are RAID: " << this->areRAID << std::endl;
    std::cout << "Garantie (luni): " << this->garantieLuni << std::endl;
}

double ServerDedicat::calculeazaCost() const {
    double costBaza = Server::calculeazaCost() * 1.5;
    if (this->areRAID) {
        costBaza += 50.0;
    }
    return costBaza;
}

void ServerDedicat::afiseazaStatus() const {
    Server::afiseazaStatus();
    std::cout << "Procesor: " << this->tipProcesor
              << " | RAID: " << std::boolalpha << this->areRAID
              << " | Garantie: " << this->garantieLuni << " luni" << std::endl;
}

std::string ServerDedicat::toString() const {
    std::ostringstream oss;
    oss << Server::toString();
    // find() + substr() de la std::string
    std::string bazaStr = oss.str();
    std::string info = bazaStr.substr(0, bazaStr.size() - 1);
    info += ", procesor=" + this->tipProcesor +
            ", RAID=" + std::string(this->areRAID ? "da" : "nu") +
            ", garantie=" + std::to_string(this->garantieLuni) + " luni]";
    return info;
}

const std::string &ServerDedicat::getTipProcesor() const { return this->tipProcesor; }
bool ServerDedicat::getAreRAID() const { return this->areRAID; }
int ServerDedicat::getGarantieLuni() const { return this->garantieLuni; }

void ServerDedicat::setTipProcesor(const std::string &procesorNou) {
    if (procesorNou.empty())
        return;
    this->tipProcesor = procesorNou;
}

void ServerDedicat::setAreRAID(bool raid) { this->areRAID = raid; }

void ServerDedicat::setGarantieLuni(int luni) {
    this->garantieLuni = luni > 0 ? luni : 1;
}

std::ostream &operator<<(std::ostream &os, const ServerDedicat &sd) {
    os << static_cast<const Server &>(sd);
    os << "Tip Procesor: " << sd.tipProcesor << "\n";
    os << std::boolalpha << "Are RAID: " << sd.areRAID << "\n";
    os << "Garantie (luni): " << sd.garantieLuni << "\n";
    return os;
}

std::istream &operator>>(std::istream &is, ServerDedicat &sd) {
    is >> static_cast<Server &>(sd);
    if (!is)
        return is;

    std::string idRes;
    std::cout << "Introduceti ID Resursa: ";
    is >> idRes;
    if (!is)
        return is;
    sd.setIdResursa(idRes);

    std::cout << "Introduceti cost pe ora (RON, > 0): ";
    double cost;
    if (!(is >> cost))
        return is;
    sd.setCostPeOra(cost);

    std::string procesor;
    std::cout << "Introduceti tip procesor (Xeon/EPYC/Threadripper): ";
    is >> procesor;
    if (!is)
        return is;
    sd.tipProcesor = procesor;

    std::cout << "Are RAID? ('true' sau 'false'): ";
    if (!(is >> std::boolalpha >> sd.areRAID))
        return is;

    std::cout << "Garantie (luni, > 0): ";
    int garantie;
    if (!(is >> garantie))
        return is;
    sd.garantieLuni = garantie > 0 ? garantie : 1;

    return is;
}
