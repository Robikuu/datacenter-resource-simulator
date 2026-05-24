#include "Server.h"
#include "ExceptiiCloud.h"
#include "Logger.h"

int Server::numarServere = 0;

Server::Server(const std::string &nume, long long memorie, float frecventa,
               int cores, bool online, const std::string &idResursa, double costPeOra)
    : ResursaCloud(idResursa, costPeOra),
      numeServer(nume.empty() ? "DEFAULT" : nume),
      memorieRam{memorie > 0 ? memorie : 1}, numarCores{cores > 0 ? cores : 1},
      esteOnline{online}, numarVMAlocate{0}, frecventaCores{nullptr} {
    std::cout << "[Constructor Server]" << std::endl;

    if (cores <= 0)
        throw ExceptieValidare(
            "Numar de cores invalid (" + std::to_string(cores) + ") in constructorul Server",
            std::to_string(cores), "constructor Server");

    if (cores > MAX_CORES)
        throw ExceptieCapacitateDepasita(
            "Un server simplu nu poate depasi " + std::to_string(MAX_CORES) +
            " cores (cerut: " + std::to_string(cores) + ")",
            MAX_CORES, cores, "constructor Server");

    this->frecventaCores = new float[this->numarCores];
    float frecvValid = frecventa > 0 ? frecventa : 0.01f;
    for (int i = 0; i < this->numarCores; i++) {
        this->frecventaCores[i] = frecvValid;
    }

    numarServere++;
    Logger::getInstance().log("Server creat: nume=" + this->numeServer +
                              ", cores=" + std::to_string(this->numarCores));
}

// Copiem frecventaCores, restul se copiaza singure automat
Server::Server(const Server &copie)
    : ResursaCloud(copie),
      numeServer(copie.numeServer),
      memorieRam{copie.memorieRam}, numarCores{copie.numarCores},
      esteOnline{copie.esteOnline}, numarVMAlocate{copie.numarVMAlocate},
      frecventaCores{nullptr} {

    this->frecventaCores = new float[this->numarCores];
    for (int i = 0; i < this->numarCores; i++) {
        this->frecventaCores[i] = copie.frecventaCores[i];
    }

    numarServere++;
}

// Assignment operator pt frecventaCores, restul se atribuie direct
Server &Server::operator=(const Server &copie) {
    if (this == &copie)
        return *this;

    float *frecventeNoi = new float[copie.numarCores];
    for (int i = 0; i < copie.numarCores; i++) {
        frecventeNoi[i] = copie.frecventaCores[i];
    }

    delete[] this->frecventaCores;

    this->numeServer = copie.numeServer;
    this->numarCores = copie.numarCores;
    this->frecventaCores = frecventeNoi;

    ResursaCloud::operator=(copie);

    this->memorieRam = copie.memorieRam;
    this->esteOnline = copie.esteOnline;
    this->numarVMAlocate = copie.numarVMAlocate;

    return *this;
}

// Destructor pt frecventaCores, numeServer se distruge singur
Server::~Server() noexcept {
    std::cout << "[Destructor Server] nume=" << this->numeServer << std::endl;
    Logger::getInstance().log("Server distrus: nume=" + this->numeServer);
    delete[] this->frecventaCores;
    frecventaCores = nullptr;
    numarServere--;
}

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "Nume Server: " << server.numeServer << "\n";
    os << "Memorie RAM (bytes): " << server.memorieRam << "\n";
    os << "Numar Cores: " << server.numarCores << "\n";
    os << "Frecventele per core (GHz): ";
    for (int i = 0; i < server.numarCores; i++) {
        os << server.frecventaCores[i];
        if (i < server.numarCores - 1)
            os << ", ";
    }
    os << "\n";
    os << std::boolalpha << "Este online: " << server.esteOnline << std::endl;
    return os;
}

std::istream &operator>>(std::istream &is, Server &server) {
    std::string numeNou;

    while (true) {
        std::cout << "Introduceti nume server: ";
        is >> numeNou;
        if (!is)
            return is;

        if (!numeNou.empty()) {
            server.numeServer = numeNou;
            break;
        }
        std::cout << "Numele serverului nu poate fi gol.\n";
    }

    std::cout << "Introduceti memorie RAM (bytes, > 0): ";
    long long memorieNoua;
    is >> memorieNoua;
    if (!is)
        return is;
    server.memorieRam = memorieNoua > 0 ? memorieNoua : 1;

    std::cout << "Introduceti numar de cores: ";
    int coresNoi;
    is >> coresNoi;
    if (!is)
        return is;
    if (coresNoi <= 0) {
        throw ExceptieValidare(
            "Numar de cores invalid (" + std::to_string(coresNoi) + ") la citire Server",
            std::to_string(coresNoi), "operator>> Server");
    }
    else if (coresNoi > Server::MAX_CORES) {
        throw ExceptieCapacitateDepasita(
            "Numarul de cores depaseste limita maxima de " + std::to_string(Server::MAX_CORES),
            Server::MAX_CORES, coresNoi, "operator>> Server");
    }
    else {
        server.numarCores = coresNoi;
    }

    delete[] server.frecventaCores;
    server.frecventaCores = new float[server.numarCores];

    for (int i = 0; i < server.numarCores; i++) {
        std::cout << "Introduceti frecventa pentru core " << i << " (GHz, > 0): ";
        float frecvantaCore;
        is >> frecvantaCore;
        if (!is)
            return is;
        server.frecventaCores[i] = frecvantaCore > 0.0f ? frecvantaCore : 0.01f;
    }

    std::cout << "Este online? ('true' sau 'false'): ";
    if (!(is >> std::boolalpha >> server.esteOnline)) {
        return is;
    }

    return is;
}

void Server::setNumeServer(const std::string &numeNou) {
    if (numeNou.empty())
        return;
    this->numeServer = numeNou;
}

void Server::setMemorieRam(long long memorieNoua) {
    this->memorieRam = memorieNoua > 0 ? memorieNoua : 1;
}

void Server::setFreceventaCoreIndex(int coreIndex, float frecventa) {
    if (coreIndex >= 0 && coreIndex < this->numarCores) {
        this->frecventaCores[coreIndex] = frecventa > 0.0f ? frecventa : 0.01f;
    }
}

void Server::setNumarCores(int coresNoi) {
    if (coresNoi <= 0) {
        throw ExceptieValidare(
            "Numar de cores invalid (" + std::to_string(coresNoi) + ") in Server::setNumarCores",
            std::to_string(coresNoi), "setNumarCores");
    }
    if (coresNoi > MAX_CORES) {
        throw ExceptieCapacitateDepasita(
            "Numar de cores (" + std::to_string(coresNoi) +
            ") depaseste limita de " + std::to_string(MAX_CORES) + " in Server::setNumarCores",
            MAX_CORES, coresNoi, "setNumarCores");
    }
    int coresValide = coresNoi;

    if (coresValide == this->numarCores) {
        return;
    }

    float *frecventeNoi = new float[coresValide];
    int deCopiat =
        (coresValide < this->numarCores) ? coresValide : this->numarCores;

    for (int i = 0; i < deCopiat; i++) {
        frecventeNoi[i] = this->frecventaCores[i];
    }
    for (int i = deCopiat; i < coresValide; i++) {
        frecventeNoi[i] = this->frecventaCores[this->numarCores - 1];
    }

    delete[] this->frecventaCores;
    this->frecventaCores = frecventeNoi;
    this->numarCores = coresValide;
}

void Server::setEsteOnline(bool statusOnline) { this->esteOnline = statusOnline; }

float &Server::operator[](int index) {
    if (index < 0 || index >= this->numarCores) {
        throw std::out_of_range("Indexul corului in Server::operator[]");
    }
    return this->frecventaCores[index];
}

const float &Server::operator[](int index) const {
    if (index < 0 || index >= this->numarCores) {
        throw std::out_of_range(
            "Indexul corului nu este in Server::operator[] const");
    }
    return this->frecventaCores[index];
}

Server Server::operator+(int coresAdunate) const {
    Server rezultat(*this);
    int coresNoi = this->numarCores + coresAdunate;
    if (coresNoi > MAX_CORES) {
        coresNoi = MAX_CORES;
    }
    else if (coresNoi < 1) {
        coresNoi = 1;
    }

    if (coresNoi != this->numarCores) {
        delete[] rezultat.frecventaCores;
        rezultat.frecventaCores = new float[coresNoi];
        for (int i = 0; i < coresNoi; i++) {
            rezultat.frecventaCores[i] =
                (i < this->numarCores)
                    ? this->frecventaCores[i]
                    : this->frecventaCores[this->numarCores - 1];
        }
        rezultat.numarCores = coresNoi;
    }

    return rezultat;
}

Server Server::operator-(int coresScazute) const { return operator+(-coresScazute); }

Server &Server::operator++() {
    if (this->numarCores < MAX_CORES) {
        float *buffer = new float[this->numarCores + 1];
        for (int i = 0; i < this->numarCores; i++) {
            buffer[i] = this->frecventaCores[i];
        }
        buffer[this->numarCores] = 0.01f;
        delete[] this->frecventaCores;
        this->frecventaCores = buffer;
        this->numarCores++;
    }
    return *this;
}

Server Server::operator++(int) {
    Server copie(*this);
    ++(*this);
    return copie;
}

bool Server::operator<(const Server &alt) const {
    if (this->memorieRam != alt.memorieRam) {
        return this->memorieRam < alt.memorieRam;
    }
    if (this->numarCores != alt.numarCores) {
        return this->numarCores < alt.numarCores;
    }
    if (this->esteOnline != alt.esteOnline) {
        return this->esteOnline < alt.esteOnline;
    }
    for (int i = 0; i < this->numarCores; i++) {
        if (std::round(this->frecventaCores[i] * 100.0f) !=
            std::round(alt.frecventaCores[i] * 100.0f)) {
            return this->frecventaCores[i] < alt.frecventaCores[i];
        }
    }
    return false;
}

bool Server::operator>(const Server &alt) const { return alt < *this; }

bool Server::operator==(const Server &alt) const {
    if (this->numeServer != alt.numeServer)
        return false;
    if (this->memorieRam != alt.memorieRam)
        return false;
    if (this->numarCores != alt.numarCores)
        return false;
    if (this->esteOnline != alt.esteOnline)
        return false;

    for (int i = 0; i < this->numarCores; i++) {
        if (std::round(this->frecventaCores[i] * 100.0f) !=
            std::round(alt.frecventaCores[i] * 100.0f)) {
            return false;
        }
    }

    return true;
}

const std::string &Server::getNumeServer() const { return this->numeServer; }

long long Server::getMemorieRam() const { return this->memorieRam; }

double Server::memorieRamToGB() const {
    return static_cast<double>(this->memorieRam) / (1024.0 * 1024.0 * 1024.0);
}

float Server::getFreceventaCoreIndex(int coreIndex) const {
    if (coreIndex >= 0 && coreIndex < this->numarCores) {
        return this->frecventaCores[coreIndex];
    }
    return 0.0f;
}

int Server::getNumarCores() const { return this->numarCores; }

bool Server::getEsteOnline() const { return this->esteOnline; }

bool Server::alocaVM() {
    if (!this->esteOnline) {
        return false;
    }
    this->numarVMAlocate++;
    return true;
}

void Server::elibereazaVM() {
    if (this->numarVMAlocate > 0) {
        this->numarVMAlocate--;
    }
}

int Server::getNumarVMAlocate() const { return this->numarVMAlocate; }

Server operator+(int coresAdunate, const Server &server) {
    return server + coresAdunate;
}

Server operator-(int coresScazute, const Server &server) {
    int coresNoi = coresScazute - server.getNumarCores();
    if (coresNoi < 1) {
        coresNoi = 1;
    }

    Server rezultat = server + (coresNoi - server.getNumarCores());
    return rezultat;
}

void Server::afiseazaDetalii() const {
    std::cout << "--- Detalii Server ---" << std::endl;
    std::cout << "ID Resursa: " << this->idResursa << std::endl;
    std::cout << "Cost pe ora: " << this->costPeOra << " RON" << std::endl;
    std::cout << "Nume Server: " << this->numeServer << std::endl;
    std::cout << "Memorie RAM (bytes): " << this->memorieRam << std::endl;
    std::cout << "Numar Cores: " << this->numarCores << std::endl;
    std::cout << "Frecventele per core (GHz): ";
    for (int i = 0; i < this->numarCores; i++) {
        std::cout << this->frecventaCores[i];
        if (i < this->numarCores - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << std::boolalpha << "Este online: " << this->esteOnline << std::endl;
    std::cout << "VM-uri alocate: " << this->numarVMAlocate << std::endl;
}

double Server::calculeazaCost() const {
    return this->costPeOra * this->numarCores;
}

void Server::afiseazaStatus() const {
    ResursaCloud::afiseazaStatus();
    std::cout << "Nume Server: " << this->numeServer << std::endl;
    std::cout << "Cores: " << this->numarCores << " | RAM: " << this->memorieRam << " bytes" << std::endl;
    std::cout << std::boolalpha << "Online: " << this->esteOnline
              << " | VM-uri alocate: " << this->numarVMAlocate << std::endl;
}

std::string Server::toString() const {
    std::ostringstream oss;
    oss << "Server[id=" << getId()
        << ", nume=" << this->numeServer
        << ", cores=" << this->numarCores
        << ", ram=" << std::fixed << std::setprecision(2) << memorieRamToGB() << " GB"
        << ", online=" << std::boolalpha << this->esteOnline << "]";
    return oss.str();
}
