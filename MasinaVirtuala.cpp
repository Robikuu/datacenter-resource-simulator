#include "MasinaVirtuala.h"
#include "ExceptiiCloud.h"
#include "Logger.h"

#include "Server.h"

int MasinaVirtuala::contorVMs = 0;

// SERVERUL SE PASEAZA CA POINTER!!!!!!!!!!!!!!!!!!!!!!!
MasinaVirtuala::MasinaVirtuala(const std::string &nume, int cores, long long memorie,
                               const IP &ip, Server *server, bool pornita)
    : numeInstanta(nume.empty() ? "VM_DEFAULT" : nume),
      numarCores(valideazaCores(cores)),
      memorieRam(valideazaMemorie(memorie)), ipAlocat(ip),
      serverGazda(nullptr), estePornita(pornita), consumCPU(nullptr) {
    if (cores > MAX_CORES)
        throw ExceptieCapacitateDepasita(
            "VM nu poate avea mai mult de " + std::to_string(MAX_CORES) +
            " cores (cerut: " + std::to_string(cores) + ")",
            MAX_CORES, cores, "constructor MasinaVirtuala");

    this->consumCPU = new float[this->numarCores];
    for (int i = 0; i < this->numarCores; i++) {
        this->consumCPU[i] = 0.0f;
    }

    if (server != nullptr && server->alocaVM()) {
        this->serverGazda = server;
    }

    contorVMs++;
    Logger::getInstance().log("MasinaVirtuala creata: nume=" + this->numeInstanta);
}

// Copy constructor pt consumCPU, numeInstanta se copiaza singur ca e string
MasinaVirtuala::MasinaVirtuala(const MasinaVirtuala &copie)
    : numeInstanta(copie.numeInstanta), numarCores(copie.numarCores),
      memorieRam(copie.memorieRam), ipAlocat(copie.ipAlocat),
      serverGazda(nullptr), estePornita(copie.estePornita),
      consumCPU(nullptr) {

    this->consumCPU = new float[this->numarCores];
    for (int i = 0; i < this->numarCores; i++) {
        this->consumCPU[i] = copie.consumCPU[i];
    }

    if (copie.serverGazda != nullptr && copie.serverGazda->alocaVM()) {
        this->serverGazda = copie.serverGazda;
    }

    contorVMs++;
}

// Assignment operator pt consumCPU, restul se copiaza singure
MasinaVirtuala &MasinaVirtuala::operator=(const MasinaVirtuala &copie) {
    if (this == &copie) {
        return *this;
    }

    float *consumNou = new float[copie.numarCores];
    for (int i = 0; i < copie.numarCores; i++) {
        consumNou[i] = copie.consumCPU[i];
    }

    IP ipNou(copie.ipAlocat);

    Server *serverVechi = this->serverGazda;
    Server *serverNou = copie.serverGazda;

    bool poateFixaServerNou = true;
    if (serverVechi != serverNou && serverNou != nullptr) {
        poateFixaServerNou = serverNou->alocaVM();
    }

    if (!poateFixaServerNou) {
        delete[] consumNou;
        return *this;
    }

    if (serverVechi != nullptr && serverVechi != serverNou) {
        serverVechi->elibereazaVM();
    }

    delete[] this->consumCPU;

    this->numeInstanta = copie.numeInstanta;
    this->numarCores = copie.numarCores;
    this->memorieRam = copie.memorieRam;
    this->ipAlocat = ipNou;
    this->estePornita = copie.estePornita;
    this->consumCPU = consumNou;

    if (serverVechi == serverNou) {
        this->serverGazda = serverVechi;
    }
    else {
        this->serverGazda = serverNou;
    }

    return *this;
}

// Destructor pt consumCPU, numeInstanta se sterge automat
MasinaVirtuala::~MasinaVirtuala() noexcept {
    if (this->serverGazda != nullptr) {
        this->serverGazda->elibereazaVM();
        this->serverGazda = nullptr;
    }

    delete[] this->consumCPU;
    this->consumCPU = nullptr;

    if (contorVMs > 0) {
        contorVMs--;
    }
}

void MasinaVirtuala::setNumeInstanta(const std::string &numeNou) {
    if (numeNou.empty())
        return;
    this->numeInstanta = numeNou;
}

void MasinaVirtuala::setNumarCores(int coresNoi) {
    int coresValide = valideazaCores(coresNoi);
    if (coresValide == this->numarCores) {
        return;
    }

    float *consumNou = new float[coresValide];
    for (int i = 0; i < coresValide; i++) {
        consumNou[i] = (i < this->numarCores) ? this->consumCPU[i] : 0.0f;
    }

    delete[] this->consumCPU;
    this->consumCPU = consumNou;
    this->numarCores = coresValide;
}

void MasinaVirtuala::setMemorieRam(long long memorieNoua) {
    this->memorieRam = valideazaMemorie(memorieNoua);
}

void MasinaVirtuala::setIpAlocat(const IP &ipNou) { this->ipAlocat = ipNou; }

void MasinaVirtuala::setServerGazda(Server *serverNou) {
    if (serverNou == this->serverGazda) {
        return;
    }

    if (serverNou != nullptr && !serverNou->alocaVM()) {
        return;
    }

    if (this->serverGazda != nullptr) {
        this->serverGazda->elibereazaVM();
    }

    this->serverGazda = serverNou;
}

void MasinaVirtuala::setEstePornita(bool stareNoua) { this->estePornita = stareNoua; }

void MasinaVirtuala::setConsumCPUIndex(int index, float consumNou) {
    if (index < 0 || index >= this->numarCores) {
        return;
    }
    this->consumCPU[index] = consumNou >= 0.0f ? consumNou : 0.0f;
}

const std::string &MasinaVirtuala::getNumeInstanta() const { return this->numeInstanta; }

int MasinaVirtuala::getNumarCores() const { return this->numarCores; }

long long MasinaVirtuala::getMemorieRam() const { return this->memorieRam; }

const IP &MasinaVirtuala::getIpAlocat() const { return this->ipAlocat; }

Server *MasinaVirtuala::getServerGazda() const { return this->serverGazda; }

bool MasinaVirtuala::getEstePornita() const { return this->estePornita; }

float MasinaVirtuala::getConsumCPUIndex(int index) const {
    if (index < 0 || index >= this->numarCores) {
        return 0.0f;
    }
    return this->consumCPU[index];
}

const float *MasinaVirtuala::getConsumCPU() const { return this->consumCPU; }

int MasinaVirtuala::getContorVMs() { return contorVMs; }

float &MasinaVirtuala::operator[](int index) {
    if (index < 0 || index >= this->numarCores) {
        throw std::out_of_range(
            "Index invalid pentru consumCPU in MasinaVirtuala::operator[]");
    }
    return this->consumCPU[index];
}

const float &MasinaVirtuala::operator[](int index) const {
    if (index < 0 || index >= this->numarCores) {
        throw std::out_of_range(
            "Index invalid pentru consumCPU in MasinaVirtuala::operator[] const");
    }
    return this->consumCPU[index];
}

MasinaVirtuala &MasinaVirtuala::operator++() {
    if (this->numarCores < MAX_CORES) {
        float *buffer = new float[this->numarCores + 1];
        for (int i = 0; i < this->numarCores; i++) {
            buffer[i] = this->consumCPU[i];
        }
        buffer[this->numarCores] = 0.0f;

        delete[] this->consumCPU;
        this->consumCPU = buffer;
        this->numarCores++;
    }
    return *this;
}

MasinaVirtuala MasinaVirtuala::operator++(int) {
    MasinaVirtuala copie(*this);
    ++(*this);
    return copie;
}

bool MasinaVirtuala::operator<(const MasinaVirtuala &alt) const {
    if (this->memorieRam != alt.memorieRam) {
        return this->memorieRam < alt.memorieRam;
    }
    if (this->numarCores != alt.numarCores) {
        return this->numarCores < alt.numarCores;
    }
    for (int i = 0; i < this->numarCores; i++) {
        if (std::round(this->consumCPU[i] * 100.0f) !=
            std::round(alt.consumCPU[i] * 100.0f)) {
            return this->consumCPU[i] < alt.consumCPU[i];
        }
    }
    return this->estePornita < alt.estePornita;
}

bool MasinaVirtuala::operator>(const MasinaVirtuala &alt) const { return alt < *this; }

bool MasinaVirtuala::operator==(const MasinaVirtuala &alt) const {
    if (this->numeInstanta != alt.numeInstanta)
        return false;
    if (this->numarCores != alt.numarCores)
        return false;
    if (this->memorieRam != alt.memorieRam)
        return false;
    if (!(this->ipAlocat == alt.ipAlocat))
        return false;

    // Folosim compare() pt std::string
    const std::string &serverSt = (this->serverGazda != nullptr)
                               ? this->serverGazda->getNumeServer()
                               : "";
    const std::string &serverDr = (alt.serverGazda != nullptr)
                               ? alt.serverGazda->getNumeServer()
                               : "";
    // Variabila temporara cand serverGazda e nullptr
    static const std::string golStr;
    const std::string &refSt = (this->serverGazda != nullptr) ? this->serverGazda->getNumeServer() : golStr;
    const std::string &refDr = (alt.serverGazda != nullptr) ? alt.serverGazda->getNumeServer() : golStr;
    if (refSt != refDr)
        return false;

    if (this->estePornita != alt.estePornita)
        return false;

    for (int i = 0; i < this->numarCores; i++) {
        if (std::round(this->consumCPU[i] * 100.0f) !=
            std::round(alt.consumCPU[i] * 100.0f)) {
            return false;
        }
    }

    return true;
}

// operator+ simplu folosind operatorul + de la string-uri
MasinaVirtuala MasinaVirtuala::operator+(const MasinaVirtuala &alt) const {
    int coresNoi = this->numarCores + alt.numarCores;
    if (coresNoi > MAX_CORES) {
        coresNoi = MAX_CORES;
    }

    long long memorieNoua = this->memorieRam;
    if (alt.memorieRam > 0 &&
        memorieNoua > std::numeric_limits<long long>::max() - alt.memorieRam) {
        memorieNoua = std::numeric_limits<long long>::max();
    }
    else {
        memorieNoua += alt.memorieRam;
    }

    std::string numeCombinat = this->numeInstanta + " + " + alt.numeInstanta;

    MasinaVirtuala rezultat(numeCombinat, coresNoi, memorieNoua, this->ipAlocat,
                            nullptr, this->estePornita || alt.estePornita);
    for (int i = 0; i < coresNoi; i++) {
        float v1 = i < this->numarCores ? this->consumCPU[i] : 0.0f;
        float v2 = i < alt.numarCores ? alt.consumCPU[i] : 0.0f;
        rezultat.consumCPU[i] = (v1 + v2) / 2.0f;
    }

    return rezultat;
}

// operator- simplu pt string-uri
MasinaVirtuala MasinaVirtuala::operator-(const MasinaVirtuala &alt) const {
    int coresNoi = this->numarCores - alt.numarCores;
    if (coresNoi < 1) {
        coresNoi = 1;
    }

    long long memorieNoua = this->memorieRam - alt.memorieRam;
    if (memorieNoua < 1) {
        memorieNoua = 1;
    }

    std::string numeCombinat = this->numeInstanta + " - " + alt.numeInstanta;

    MasinaVirtuala rezultat(numeCombinat, coresNoi, memorieNoua, this->ipAlocat,
                            nullptr, this->estePornita && !alt.estePornita);

    for (int i = 0; i < coresNoi; i++) {
        float v1 = (i < this->numarCores) ? this->consumCPU[i] : 0.0f;
        float v2 = (i < alt.numarCores) ? alt.consumCPU[i] : 0.0f;
        float diferenta = v1 - v2;
        rezultat.consumCPU[i] = diferenta >= 0.0f ? diferenta : 0.0f;
    }

    return rezultat;
}

MasinaVirtuala MasinaVirtuala::operator+(int coresExtra) const {
    int coresNoi = this->numarCores + coresExtra;
    coresNoi = valideazaCores(coresNoi);

    MasinaVirtuala rezultat(*this);
    if (coresNoi != this->numarCores) {
        delete[] rezultat.consumCPU;
        rezultat.consumCPU = new float[coresNoi];
        for (int i = 0; i < coresNoi; i++) {
            rezultat.consumCPU[i] =
                (i < this->numarCores) ? this->consumCPU[i] : 0.0f;
        }
        rezultat.numarCores = coresNoi;
    }
    return rezultat;
}

MasinaVirtuala MasinaVirtuala::operator-(int coresScazute) const {
    return operator+(-coresScazute);
}

std::ostream &operator<<(std::ostream &os, const MasinaVirtuala &vm) {
    os << "Nume instanta VM: " << vm.numeInstanta << "\n";
    os << "Numar cores VM: " << vm.numarCores << "\n";
    os << "Memorie RAM VM (bytes): " << vm.memorieRam << "\n";
    os << "IP alocat VM: " << vm.ipAlocat.getAdresaIP() << "\n";
    os << "Server gazda VM: "
       << (vm.serverGazda != nullptr ? vm.serverGazda->getNumeServer() : "N/A")
       << "\n";
    os << std::boolalpha << "VM pornita: " << vm.estePornita << "\n";
    os << "Consum CPU per core: ";
    for (int i = 0; i < vm.numarCores; i++) {
        os << vm.consumCPU[i];
        if (i < vm.numarCores - 1)
            os << ", ";
    }
    os << "\n";
    return os;
}

std::istream &operator>>(std::istream &is, MasinaVirtuala &vm) {
    std::string bufferNume;
    int coresNoi = 1;
    long long memorieNoua = 1;
    bool pornitaNou = false;
    IP ipNou;

    while (true) {
        std::cout << "Introduceti nume instanta VM: ";
        is >> bufferNume;
        if (!is)
            return is;
        if (!bufferNume.empty()) {
            break;
        }
        std::cout << "Numele instantei nu poate fi gol.\n";
    }

    while (true) {
        std::cout << "Introduceti numar cores VM (1-" << MasinaVirtuala::MAX_CORES << "): ";
        if (is >> coresNoi) {
            coresNoi = MasinaVirtuala::valideazaCores(coresNoi);
            break;
        }
        if (is.eof())
            return is;
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Valoare invalida pentru cores.\n";
    }

    while (true) {
        std::cout << "Introduceti memorie RAM VM (bytes, > 0): ";
        if (is >> memorieNoua) {
            memorieNoua = MasinaVirtuala::valideazaMemorie(memorieNoua);
            break;
        }
        if (is.eof())
            return is;
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Valoare invalida pentru memorie.\n";
    }

    std::cout << "Introduceti IP pentru VM:\n";
    if (!(is >> ipNou)) {
        return is;
    }

    while (true) {
        std::cout << "VM este pornita? ('true' sau 'false'): ";
        if (is >> std::boolalpha >> pornitaNou) {
            break;
        }
        if (is.eof())
            return is;
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Valoare invalida pentru stare pornire.\n";
    }

    float *consumNou = new float[coresNoi];
    for (int i = 0; i < coresNoi; i++) {
        while (true) {
            std::cout << "Consum CPU pentru core " << i << " (>=0): ";
            if (is >> consumNou[i]) {
                if (consumNou[i] < 0.0f) {
                    consumNou[i] = 0.0f;
                }
                break;
            }
            if (is.eof()) {
                delete[] consumNou;
                return is;
            }
            is.clear();
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Valoare invalida pentru consum CPU.\n";
        }
    }

    if (vm.serverGazda != nullptr) {
        vm.serverGazda->elibereazaVM();
        vm.serverGazda = nullptr;
    }

    delete[] vm.consumCPU;

    vm.numeInstanta = bufferNume;
    vm.numarCores = coresNoi;
    vm.memorieRam = memorieNoua;
    vm.ipAlocat = ipNou;
    vm.estePornita = pornitaNou;
    vm.consumCPU = consumNou;

    return is;
}

MasinaVirtuala operator+(int val, const MasinaVirtuala &vm) { return vm + val; }
