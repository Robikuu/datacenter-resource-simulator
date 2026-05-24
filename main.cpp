//@Tudorache Dumitru-Robert
//Grupa 141
//Hours spent: 25
//
//mai am de adaugat multe la proiect, nu am reusit sa adaug toate clasele si functionalitatiile pe care le voiam
//de asemenea m-am folosit de AI pentru a crea meniul crud (cred ca este destul de evident) deoarece intampinam
//mult prea multe probleme la el. mult AI am mai folosit si pentru debug, generare de teste si chestii de genul acesta.
//in rest codul este scris in proportie de 80% de mine iar majoritatea deciziilor de design au fost luate de catre mine.
//de asemenea codul poate pare facut de AI deoarece am rulat la final clang-format pentru a uniformiza codul
//cum imi place mie.
//in rest sper sa nu primesc nota 1 pt compilare 🙏🙏. am incercat sa acopar cat mai multe din buguri posibile inclusiv
//cele care au legatura cu platforma, sistemul de operare, diacritice, 32-bit vs 64-bit.
//codul este scris in linux deci daca apar probleme de compilare sper sa nu fie din cauza ca nu are terminatie crlf
//sau ceva de genul acesta.

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "utils.h"
// Algoritmi STL folositi: sort, find_if, count_if, for_each, any_of, transform, erase-remove_if

#include "IP.h"
#include "Server.h"
#include "ServerDedicat.h"
#include "ServerVirtual.h"
#include "MasinaVirtuala.h"
#include "Datacenter.h"
#include "ExceptiiCloud.h"
#include "IObject.h"
#include "Depozitar.h"
#include "UtilitatiTemplate.h"
#include "Logger.h"
#include "ClientCloud.h"
#include "FabricaServer.h"
#include <algorithm>
#include <typeinfo>
#include <utility>
#include <vector>
#include <functional>

// Demonstrare Stack Unwinding si catch ordonat (derivata inainte de baza!)
void demonstreazaStackUnwinding() {
    std::cout << "\n===== DEMONSTRARE STACK UNWINDING & TOSTRING =====\n";
    
    // --- PART E: toString() verification ---
    std::cout << "[PART E] Verificare apel polimorfic toString() prin IObject*:\n";
    Server* bSrv = new Server("BaseSrv", 2048, 2.0f, 2, true);
    ServerDedicat* dSrv = new ServerDedicat("DedSrv", 4096, 3.0f, 4, true, "ID_D1", 10.0, "Xeon", true, 12);
    ServerVirtual* vSrv = new ServerVirtual("VirtSrv", 1024, 2.0f, 1, true, "ID_V1", 5.0, "KVM", 50.0f, 1000);
    
    IObject* obj1 = bSrv;
    IObject* obj2 = dSrv;
    IObject* obj3 = vSrv;
    
    std::cout << "1) " << obj1->toString() << "\n";
    std::cout << "2) " << obj2->toString() << "\n";
    std::cout << "3) " << obj3->toString() << "\n";
    
    delete bSrv;
    delete dSrv;
    delete vSrv;
    std::cout << "--------------------------------------------------\n\n";

    std::cout << "Se creeaza un IP local valid in interiorul blocului try.\n";
    std::cout << "Apoi se incearca crearea unui Server cu 0 cores (invalid).\n";
    std::cout << "Exceptia va cauza stack unwinding — IP-ul local va fi distrus\n";
    std::cout << "INAINTE ca executia sa ajunga in blocul catch.\n\n";

    try {
        // Obiect local cu destructor care printeaza mesaj (IP::~IP).
        // La stack unwinding, destructorul lui ipLocal VA fi apelat
        // inainte de intrarea in catch.
        IP ipLocal("192.168.1.1", "ipv4", false);
        std::cout << "[try] IP local creat: " << ipLocal.getAdresaIP() << "\n";

        // Aceasta linie va arunca ExceptieValidare (cores = 0 este invalid).
        // Stack unwinding va distruge ipLocal inainte de catch.
        std::cout << "[try] Se incearca crearea unui Server cu 0 cores...\n";
        Server serverInvalid("TestUnwinding", 1024, 2.5f, 0);
        // Linia urmatoare NU se va executa niciodata
        std::cout << "[try] Aceasta linie nu ar trebui sa apara!\n";
    }
    // Ordinea catch: derivata inainte de baza
    catch (const ExceptieCapacitateDepasita &e) {
        std::cout << "[catch ExceptieCapacitateDepasita] " << e.what() << "\n";
        std::cout << "  Limita: " << e.getLimita()
                  << ", Cerut: " << e.getValoareCeruta() << "\n";
    }
    catch (const ExceptieValidare &e) {
        // VERIFICARE: destructorul IP-ului local a printat INAINTE de acest mesaj.
        // Aceasta confirma ca stack unwinding a functionat corect.
        std::cout << "[catch ExceptieValidare] " << e.what() << "\n";
        std::cout << "  Valoare invalida: \"" << e.getValoareInvalida() << "\"\n";
        std::cout << "  Operatie: " << e.getOperatie() << "\n";
    }
    catch (const ExceptieCloud &e) {
        std::cout << "[catch ExceptieCloud] " << e.what() << "\n";
    }
    catch (const std::exception &e) {
        std::cout << "[catch std::exception] " << e.what() << "\n";
    }
    catch (...) {
        std::cout << "[catch ...] Exceptie necunoscuta!\n";
    }

    std::cout << "\n===== SFARSIT DEMONSTRARE STACK UNWINDING =====\n\n";
}

// Meniu server - foloseste std::vector<Server*>
void menuServer(std::vector<Server*>& servere, std::vector<MasinaVirtuala>& vms) {
    int opt = -1;
    while (true) {
        std::cout << "\n===== MENIU SERVER =====\n";
        std::cout << "1. Creare\n2. Afisare toate\n3. Actualizare (dupa index)\n4. "
                     "Stergere (dupa index)\n"
                  << "5. Transform: lista nume servere (std::transform)\n"
                  << "6. Filtru: doar online (erase-remove_if pe copie)\n"
                  << "0. Inapoi\n";
        opt = citesteOptiune("Optiune: ");

        if (opt == 0)
            break;

        if (opt == 1) {
            try {
                Server *s = new Server();
                std::cin >> *s;
                if (!std::cin) {
                    curataCin();
                    delete s;
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }
                servere.push_back(s);
                std::cout << "Server creat cu succes. [index=" << servere.size() - 1
                          << "]\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 2) {
            if (servere.empty()) {
                std::cout << "Nu exista servere.\n";
                continue;
            }
            for (std::size_t i = 0; i < servere.size(); i++) {
                std::cout << "--- Server [" << i << "] ---\n"
                          << *servere[i] << "\n";
            }
        }
        else if (opt == 3) {
            if (servere.empty()) {
                std::cout << "Nu exista servere.\n";
                continue;
            }
            int idx = citesteOptiune("Index server de actualizat: ");
            if (idx < 0 || idx >= static_cast<int>(servere.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            try {
                std::cin >> *servere[static_cast<std::size_t>(idx)];
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire.\n";
                    continue;
                }
                std::cout << "Server actualizat cu succes.\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 4) {
            if (servere.empty()) {
                std::cout << "Nu exista servere.\n";
                continue;
            }
            int idx = citesteOptiune("Index server de sters: ");
            if (idx < 0 || idx >= static_cast<int>(servere.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }

            Server* deSters = servere[static_cast<std::size_t>(idx)];
            for (auto& vm : vms) {
                if (vm.getServerGazda() == deSters) {
                    vm.setServerGazda(nullptr);
                }
            }

            delete deSters;
            servere.erase(servere.begin() + idx);
            std::cout << "Server sters cu succes.\n";
        }
        else if (opt == 5) {
            // std::transform — construieste un vector de nume din vectorul de pointeri
            if (servere.empty()) {
                std::cout << "Nu exista servere.\n";
                continue;
            }
            std::vector<std::string> nume(servere.size());
            std::transform(servere.begin(), servere.end(), nume.begin(),
                           [](const Server* s) -> std::string { return s->getNumeServer(); });
            std::cout << "Nume servere (via std::transform):\n";
            for (std::size_t i = 0; i < nume.size(); i++) {
                std::cout << "  [" << i << "] " << nume[i] << "\n";
            }
        }
        else if (opt == 6) {
            // erase-remove_if idiom pe o COPIE a listei (nu mutam lista reala)
            if (servere.empty()) {
                std::cout << "Nu exista servere.\n";
                continue;
            }
            std::vector<Server*> copie(servere);
            copie.erase(
                std::remove_if(copie.begin(), copie.end(),
                               [](const Server* s) { return !s->getEsteOnline(); }),
                copie.end());
            std::cout << "Servere online (" << copie.size() << " din "
                      << servere.size() << "):\n";
            for (std::size_t i = 0; i < copie.size(); i++) {
                std::cout << "  " << copie[i]->getNumeServer() << "\n";
            }
            if (copie.empty()) {
                std::cout << "  (niciun server online)\n";
            }
        }
        else {
            std::cout << "Optiune invalida.\n";
        }
    }
}

// Meniu IP - foloseste std::vector<IP>
void menuIP(std::vector<IP>& ipuri) {
    int opt = -1;
    while (true) {
        std::cout << "\n===== MENIU IP =====\n";
        std::cout << "1. Creare\n2. Afisare toate\n3. Actualizare (dupa index)\n4. "
                     "Stergere (dupa index)\n0. Inapoi\n";
        opt = citesteOptiune("Optiune: ");

        if (opt == 0)
            break;

        if (opt == 1) {
            try {
                IP ip;
                std::cin >> ip;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }
                ipuri.push_back(ip);
                std::cout << "IP creat cu succes. [index=" << ipuri.size() - 1 << "]\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 2) {
            if (ipuri.empty()) {
                std::cout << "Nu exista IP-uri.\n";
                continue;
            }
            for (std::size_t i = 0; i < ipuri.size(); i++) {
                std::cout << "--- IP [" << i << "] ---\n"
                          << ipuri[i] << "\n";
            }
        }
        else if (opt == 3) {
            if (ipuri.empty()) {
                std::cout << "Nu exista IP-uri.\n";
                continue;
            }
            int idx = citesteOptiune("Index IP de actualizat: ");
            if (idx < 0 || idx >= static_cast<int>(ipuri.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            try {
                std::cin >> ipuri[static_cast<std::size_t>(idx)];
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire.\n";
                    continue;
                }
                std::cout << "IP actualizat cu succes.\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 4) {
            if (ipuri.empty()) {
                std::cout << "Nu exista IP-uri.\n";
                continue;
            }
            int idx = citesteOptiune("Index IP de sters: ");
            if (idx < 0 || idx >= static_cast<int>(ipuri.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            ipuri.erase(ipuri.begin() + idx);
            std::cout << "IP sters cu succes.\n";
        }
        else {
            std::cout << "Optiune invalida.\n";
        }
    }
}

// Meniu masina virtuala - foloseste std::vector<MasinaVirtuala>
void menuMasinaVirtuala(std::vector<MasinaVirtuala>& vms,
                        std::vector<Server*>& servere) {
    int opt = -1;
    while (true) {
        std::cout << "\n===== MENIU MASINA VIRTUALA =====\n";
        std::cout << "1. Creare\n2. Afisare toate\n3. Actualizare (dupa index)\n4. "
                     "Stergere (dupa index)\n"
                  << "5. Verifica VM-uri pornite (std::any_of)\n"
                  << "0. Inapoi\n";
        opt = citesteOptiune("Optiune: ");

        if (opt == 0)
            break;

        if (opt == 1) {
            try {
                MasinaVirtuala vm;
                std::cin >> vm;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }

                // optiune linking la server existent
                Server *serverLink = nullptr;
                if (!servere.empty()) {
                    int alegere = citesteOptiune(
                        "Legare server (0=Nu, 1=Da, sau index direct >=0): ");

                    int idxS = -1;
                    if (alegere == 1) {
                        std::cout << "Servere disponibile:\n";
                        for (std::size_t i = 0; i < servere.size(); i++) {
                            std::cout << "  [" << i << "] " << servere[i]->getNumeServer()
                                      << "\n";
                        }
                        idxS = citesteOptiune("Index server: ");
                    } else if (alegere > 1) {
                        idxS = alegere;
                    }

                    if (idxS >= 0 && idxS < static_cast<int>(servere.size())) {
                        serverLink = servere[static_cast<std::size_t>(idxS)];
                    } else if (idxS >= 0) {
                        std::cout << "Index invalid. VM creat fara server.\n";
                    }
                }
                vm.setServerGazda(serverLink);

                if (serverLink != nullptr && vm.getServerGazda() == serverLink) {
                    std::cout << "VM legat de serverul \""
                              << serverLink->getNumeServer() << "\".\n";
                } else if (serverLink != nullptr && vm.getServerGazda() == nullptr) {
                    std::cout << "Serverul selectat nu permite alocare (offline). VM creat fara server.\n";
                }

                vms.push_back(vm);
                std::cout << "VM creat cu succes. [index=" << vms.size() - 1 << "]\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 2) {
            if (vms.empty()) {
                std::cout << "Nu exista VM-uri.\n";
                continue;
            }
            for (std::size_t i = 0; i < vms.size(); i++) {
                std::cout << "--- VM [" << i << "] ---\n"
                          << vms[i] << "\n";
            }
        }
        else if (opt == 3) {
            if (vms.empty()) {
                std::cout << "Nu exista VM-uri.\n";
                continue;
            }
            int idx = citesteOptiune("Index VM de actualizat: ");
            if (idx < 0 || idx >= static_cast<int>(vms.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            try {
                std::cin >> vms[static_cast<std::size_t>(idx)];
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire.\n";
                    continue;
                }
                std::cout << "VM actualizat cu succes.\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 4) {
            if (vms.empty()) {
                std::cout << "Nu exista VM-uri.\n";
                continue;
            }
            int idx = citesteOptiune("Index VM de sters: ");
            if (idx < 0 || idx >= static_cast<int>(vms.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            vms.erase(vms.begin() + idx);
            std::cout << "VM sters cu succes.\n";
        }
        else if (opt == 5) {
            // std::any_of — verifica daca exista cel putin un VM pornit
            if (vms.empty()) {
                std::cout << "Nu exista VM-uri.\n";
                continue;
            }
            bool existaPornite = std::any_of(vms.begin(), vms.end(),
                [](const MasinaVirtuala& vm) { return vm.getEstePornita(); });
            if (existaPornite) {
                std::cout << "Exista cel putin un VM pornit.\n";
            } else {
                std::cout << "ATENTIE: Niciun VM nu este pornit!\n";
            }
        }
        else {
            std::cout << "Optiune invalida.\n";
        }
    }
}

// Meniu datacenter - foloseste std::vector<Datacenter>
void menuDatacenter(std::vector<Datacenter>& datacentere,
                    std::vector<Server*>& servere) {
    int opt = -1;
    while (true) {
        std::cout << "\n===== MENIU DATACENTER =====\n";
        std::cout << "1. Creare\n2. Afisare toate\n3. Actualizare (dupa index)\n4. "
                     "Stergere (dupa index)\n0. Inapoi\n";
        opt = citesteOptiune("Optiune: ");

        if (opt == 0)
            break;

        if (opt == 1) {
            try {
                std::string nume;
                std::string loc;

                std::cout << "Introduceti nume datacenter: ";
                std::cin >> nume;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }

                std::cout << "Introduceti locatie datacenter: ";
                std::cin >> loc;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }

                Datacenter dc(nume, loc, 0, nullptr);

                int cate = citesteOptiune("Cate servere existente doriti sa adaugati? ");
                if (cate < 0)
                    cate = 0;

                if (!servere.empty() && cate > 0) {
                    std::cout << "Servere disponibile:\n";
                    for (std::size_t i = 0; i < servere.size(); i++) {
                        std::cout << "  [" << i << "] " << servere[i]->getNumeServer() << "\n";
                    }
                }

                for (int j = 0; j < cate; j++) {
                    int idxS = citesteOptiune("Index server de adaugat: ");
                    if (idxS >= 0 && idxS < static_cast<int>(servere.size())) {
                        dc.adaugaServer(*servere[static_cast<std::size_t>(idxS)]);
                        std::cout << "Server \"" << servere[static_cast<std::size_t>(idxS)]->getNumeServer()
                                  << "\" adaugat.\n";
                    } else {
                        std::cout << "Index invalid. Server ignorat.\n";
                    }
                }

                datacentere.push_back(dc);
                std::cout << "Datacenter creat cu succes. [index=" << datacentere.size() - 1 << "]\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 2) {
            if (datacentere.empty()) {
                std::cout << "Nu exista datacentere.\n";
                continue;
            }
            for (std::size_t i = 0; i < datacentere.size(); i++) {
                std::cout << "--- Datacenter [" << i << "] ---\n"
                          << datacentere[i] << "\n";
            }
        }
        else if (opt == 3) {
            if (datacentere.empty()) {
                std::cout << "Nu exista datacentere.\n";
                continue;
            }
            int idx = citesteOptiune("Index datacenter de actualizat: ");
            if (idx < 0 || idx >= static_cast<int>(datacentere.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            try {
                std::string nume;
                std::string loc;

                std::cout << "Introduceti nume datacenter: ";
                std::cin >> nume;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire.\n";
                    continue;
                }

                std::cout << "Introduceti locatie datacenter: ";
                std::cin >> loc;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Eroare la citire.\n";
                    continue;
                }

                Datacenter actualizat(nume, loc, 0, nullptr);

                int cate = citesteOptiune("Cate servere existente doriti sa ramana in datacenter? ");
                if (cate < 0)
                    cate = 0;

                if (!servere.empty() && cate > 0) {
                    std::cout << "Servere disponibile:\n";
                    for (std::size_t i = 0; i < servere.size(); i++) {
                        std::cout << "  [" << i << "] " << servere[i]->getNumeServer() << "\n";
                    }
                }

                for (int j = 0; j < cate; j++) {
                    int idxS = citesteOptiune("Index server de adaugat: ");
                    if (idxS >= 0 && idxS < static_cast<int>(servere.size())) {
                        actualizat.adaugaServer(*servere[static_cast<std::size_t>(idxS)]);
                    } else {
                        std::cout << "Index invalid. Server ignorat.\n";
                    }
                }

                datacentere[static_cast<std::size_t>(idx)] = actualizat;
                std::cout << "Datacenter actualizat cu succes.\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 4) {
            if (datacentere.empty()) {
                std::cout << "Nu exista datacentere.\n";
                continue;
            }
            int idx = citesteOptiune("Index datacenter de sters: ");
            if (idx < 0 || idx >= static_cast<int>(datacentere.size())) {
                std::cout << "Index invalid.\n";
                continue;
            }
            datacentere.erase(datacentere.begin() + idx);
            std::cout << "Datacenter sters cu succes.\n";
        }
        else {
            std::cout << "Optiune invalida.\n";
        }
    }
}

// Meniu pentru resurse cloud polimorfice, foloseste Depozitar ca backing store
void menuResurseCloud(Depozitar<ResursaCloud, 50>& depozit, ClientCloud& client) {
    int opt = -1;
    while (true) {
        std::cout << "\n===== MENIU RESURSE CLOUD (Polimorfism) =====\n";
        std::cout << "1. Creare ServerDedicat\n"
                  << "2. Creare ServerVirtual\n"
                  << "3. Afisare toate (polimorfic)\n"
                  << "4. Detalii specifice tip (dynamic_cast)\n"
                  << "5. Calculeaza cost total\n"
                  << "6. Afiseaza status (demonstrare extindere metoda baza)\n"
                  << "7. Stergere (dupa index)\n"
                  << "8. Cauta dupa ID (functie template + specializare)\n"
                  << "9. Sorteaza dupa cost (functie template sorteaza)\n"
                  << "10. Numara servere online (functie template numara)\n"
                  << "11. Copiaza in vector (metoda template copiazaIn)\n"
                  << "12. Filtrare si sortare (STL algorithms)\n"
                  << "13. Afisare jurnal sistem (Singleton Logger)\n"
                  << "14. Creare server prin factory (Abstract Factory)\n"
                  << "0. Inapoi\n";
        opt = citesteOptiune("Optiune: ");

        if (opt == 0)
            break;

        if (opt == 1) {
            if (depozit.estePlin()) {
                std::cout << "Depozitul este plin (capacitate maxima 50).\n";
                continue;
            }
            try {
                ServerDedicat *sd = new ServerDedicat();
                std::cin >> *sd;
                if (!std::cin) {
                    curataCin();
                    delete sd;
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }
                depozit.adauga(sd);
                Logger::getInstance().log("CRUD: ServerDedicat creat, index=" +
                    std::to_string(depozit.getNr() - 1));
                std::cout << "ServerDedicat creat cu succes. [index=" << depozit.getNr() - 1 << "]\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 2) {
            if (depozit.estePlin()) {
                std::cout << "Depozitul este plin (capacitate maxima 50).\n";
                continue;
            }
            try {
                ServerVirtual *sv = new ServerVirtual();
                std::cin >> *sv;
                if (!std::cin) {
                    curataCin();
                    delete sv;
                    std::cout << "Eroare la citire. Operatiune anulata.\n";
                    continue;
                }
                depozit.adauga(sv);
                Logger::getInstance().log("CRUD: ServerVirtual creat, index=" +
                    std::to_string(depozit.getNr() - 1));
                std::cout << "ServerVirtual creat cu succes. [index=" << depozit.getNr() - 1 << "]\n";
            } catch (const ExceptieCapacitateDepasita &e) {
                std::cout << "Eroare capacitate: " << e.what() << "\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare validare: " << e.what() << "\n";
            } catch (const ExceptieCloud &e) {
                std::cout << "Eroare cloud: " << e.what() << "\n";
            } catch (const std::exception &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 3) {
            // Afisare polimorfica: apelam afiseazaDetalii() prin pointer la baza.
            // Late binding — fiecare obiect executa propria implementare.
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            for (int i = 0; i < depozit.getNr(); i++) {
                try {
                    ResursaCloud *r = depozit.get(i);
                    std::cout << "\n--- Resursa [" << i << "] (tip real: "
                              << typeid(*r).name() << ") ---\n";
                    // toString() din IObject — afisare polimorfica prin operator<<
                    std::cout << "toString(): " << r->toString() << "\n";
                    r->afiseazaDetalii();
                } catch (const ExceptieValidare &e) {
                    std::cout << "Eroare la accesare index " << i << ": " << e.what() << "\n";
                }
            }
        }
        else if (opt == 4) {
            // Operatii specifice tipului concret — accesibile dupa dynamic_cast.
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            int idx = citesteOptiune("Index resursa: ");
            try {
                ResursaCloud *r = depozit.get(idx);

                // Folosim dynamic_cast in loc de static_cast deoarece:
                // - Nu stim la compile-time tipul real al obiectului din spatele pointerului.
                // - dynamic_cast verifica tipul la runtime si returneaza nullptr daca
                //   conversia nu este valida, evitand Undefined Behaviour.
                // - static_cast nu face aceasta verificare si ar putea duce la
                //   accesarea de memorie invalida daca tipul real nu se potriveste.
                ServerDedicat *sd = dynamic_cast<ServerDedicat *>(r);
                if (sd != nullptr) {
                    std::cout << "\nAceasta resursa este un ServerDedicat!\n";
                    std::cout << "Procesor: " << sd->getTipProcesor() << "\n";
                    std::cout << "RAID: " << std::boolalpha << sd->getAreRAID() << "\n";
                    std::cout << "Garantie: " << sd->getGarantieLuni() << " luni\n";
                    std::cout << "Cost orar dedicat: " << sd->calculeazaCost() << " RON\n";
                } else {
                    ServerVirtual *sv = dynamic_cast<ServerVirtual *>(r);
                    if (sv != nullptr) {
                        std::cout << "\nAceasta resursa este un ServerVirtual!\n";
                        std::cout << "Virtualizare: " << sv->getTipVirtualizare() << "\n";
                        std::cout << "Procent resurse: " << sv->getProcentResurse() << "%\n";
                        std::cout << "Limite IOPS: " << sv->getLimiteIOPS() << "\n";
                        std::cout << "Cost orar virtual: " << sv->calculeazaCost() << " RON\n";
                    } else {
                        std::cout << "Tipul resursei nu a putut fi determinat (dynamic_cast esuat).\n";
                    }
                }
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 5) {
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            double costTotal = 0.0;
            for (int i = 0; i < depozit.getNr(); i++) {
                ResursaCloud *r = depozit.get(i);
                double cost = r->calculeazaCost();
                std::cout << "Resursa [" << i << "]: " << cost << " RON/ora\n";
                costTotal += cost;
            }
            std::cout << "Cost total: " << costTotal << " RON/ora\n";
        }
        else if (opt == 6) {
            // Demonstrare extindere metoda bazei:
            // afiseazaStatus() este definita in ResursaCloud (concreta),
            // suprascrisa in Server (apeleaza ResursaCloud::afiseazaStatus()),
            // si suprascrisa din nou in ServerDedicat/ServerVirtual
            // (apeleaza Server::afiseazaStatus()).
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            int idx = citesteOptiune("Index resursa: ");
            try {
                depozit.get(idx)->afiseazaStatus();
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 7) {
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            int idx = citesteOptiune("Index resursa de sters: ");
            try {
                // Demonstrare destructor virtual: delete prin pointer la baza
                // apeleaza destructorul derivatei apoi al bazei, in ordine corecta.
                ResursaCloud *r = depozit.get(idx);
                std::cout << "Se sterge resursa [" << idx << "]...\n";
                Logger::getInstance().log("CRUD: Resursa stearsa, index=" +
                    std::to_string(idx) + ", id=" + r->getIdResursa());
                delete r;
                depozit.elimina(idx);
                std::cout << "Resursa stearsa cu succes.\n";
            } catch (const ExceptieValidare &e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
        }
        else if (opt == 8) {
            // Cautare dupa ID — foloseste cautaDupaId() care intern apeleaza
            // specializarea completa cautaLiniar<ResursaCloud>.
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            int idCautat = citesteOptiune("Introduceti ID-ul cautat: ");

            int idx = cautaDupaId(depozit.getElemente(), depozit.getNr(), idCautat);
            if (idx >= 0) {
                std::cout << "Resursa gasita la index " << idx << ": "
                          << depozit.get(idx)->toString() << "\n";
            } else {
                std::cout << "Nicio resursa cu ID=" << idCautat << " nu a fost gasita.\n";
            }

            // Demonstrare si cu versiunea generica (predicat lambda)
            std::string idStr = depozit.get(0)->getIdResursa();
            int idxGen = cautaLiniar<ResursaCloud>(
                depozit.getElemente(), depozit.getNr(),
                [&idStr](const ResursaCloud* r) { return r->getIdResursa() == idStr; });
            if (idxGen >= 0) {
                std::cout << "(Generic) Prima resursa cu idResursa=\"" << idStr
                          << "\" gasita la index " << idxGen << "\n";
            }
        }
        else if (opt == 9) {
            // Sortare dupa cost — functia template sorteaza<ResursaCloud, double>
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            std::function<double(const ResursaCloud*)> extractorCost =
                [](const ResursaCloud* r) -> double { return r->calculeazaCost(); };

            sorteaza<ResursaCloud, double>(
                depozit.getElemente(), depozit.getNr(), extractorCost);

            std::cout << "Resursele au fost sortate dupa cost (crescator):\n";
            for (int i = 0; i < depozit.getNr(); i++) {
                ResursaCloud *r = depozit.get(i);
                std::cout << "  [" << i << "] " << r->toString()
                          << " — cost: " << r->calculeazaCost() << " RON/ora\n";
            }
        }
        else if (opt == 10) {
            // Numara servere online — functia template numara<ResursaCloud>
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }

            // Folosim dynamic_cast in predicatul lambda pentru a verifica
            // daca resursa este un Server si daca este online.
            std::function<bool(const ResursaCloud*)> esteOnline =
                [](const ResursaCloud* r) -> bool {
                    const Server* s = dynamic_cast<const Server*>(r);
                    return s != nullptr && s->getEsteOnline();
                };

            int nrOnline = numara<ResursaCloud>(
                depozit.getElemente(), depozit.getNr(), esteOnline);

            std::cout << "Servere online: " << nrOnline
                      << " din " << depozit.getNr() << " resurse totale.\n";
        }
        else if (opt == 11) {
            // Demonstrare copiazaIn<std::vector<ResursaCloud*>> — metoda template
            // din Depozitar care copiaza pointerii intr-un container extern.
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }
            std::vector<ResursaCloud*> copie;
            depozit.copiazaIn(copie);
            std::cout << "Copiate " << copie.size()
                      << " resurse in std::vector. Continut:\n";
            for (std::size_t i = 0; i < copie.size(); i++) {
                std::cout << "  [" << i << "] " << copie[i]->toString() << "\n";
            }
            // Vectorul copie NU detine obiectele — nu facem delete pe ele.
        }
        else if (opt == 12) {
            // Submeniu Filtrare si sortare — demonstreaza STL algorithms cu lambda
            if (depozit.esteGol()) {
                std::cout << "Nu exista resurse cloud.\n";
                continue;
            }

            // Copiem pointerii in std::vector pentru a folosi STL algorithms
            // (Depozitar nu suporta iteratori STL, dar vectorul da).
            std::vector<ResursaCloud*> vec;
            depozit.copiazaIn(vec);

            std::cout << "\n--- Filtrare si sortare ---\n";
            std::cout << "1. Sorteaza dupa cost descrescator (std::sort)\n";
            std::cout << "2. Cauta primul cu cost > prag (std::find_if)\n";
            std::cout << "3. Numara online (std::count_if)\n";
            std::cout << "4. Afisare sumar (std::for_each)\n";
            std::cout << "5. Cel mai scump (std::pair + structured bindings)\n";
            int subopt = citesteOptiune("Sub-optiune: ");

            if (subopt == 1) {
                // std::sort cu lambda — sorteaza descrescator dupa cost
                std::sort(vec.begin(), vec.end(),
                    [](const ResursaCloud* a, const ResursaCloud* b) {
                        return a->calculeazaCost() > b->calculeazaCost();
                    });
                std::cout << "Resurse sortate descrescator dupa cost:\n";
                for (std::size_t i = 0; i < vec.size(); i++) {
                    std::cout << "  [" << i << "] " << vec[i]->toString()
                              << " — cost: " << vec[i]->calculeazaCost() << " RON/ora\n";
                }
            }
            else if (subopt == 2) {
                // std::find_if cu lambda — gaseste prima resursa cu cost > prag
                std::cout << "Introduceti pragul de cost: ";
                double prag = 0.0;
                std::cin >> prag;
                if (!std::cin) {
                    curataCin();
                    std::cout << "Valoare invalida.\n";
                    continue;
                }
                auto it = std::find_if(vec.begin(), vec.end(),
                    [prag](const ResursaCloud* r) { return r->calculeazaCost() > prag; });
                if (it != vec.end()) {
                    auto idx = std::distance(vec.begin(), it);
                    std::cout << "Prima resursa cu cost > " << prag << ": [" << idx << "] "
                              << (*it)->toString() << " — cost: " << (*it)->calculeazaCost() << "\n";
                } else {
                    std::cout << "Nicio resursa cu cost > " << prag << ".\n";
                }
            }
            else if (subopt == 3) {
                // std::count_if cu lambda — numara cate resurse sunt online
                auto nrOnline = std::count_if(vec.begin(), vec.end(),
                    [](const ResursaCloud* r) {
                        const Server* s = dynamic_cast<const Server*>(r);
                        return s != nullptr && s->getEsteOnline();
                    });
                std::cout << "Resurse online: " << nrOnline << " din " << vec.size() << "\n";
            }
            else if (subopt == 4) {
                // std::for_each cu lambda — afiseaza sumar one-liner pt fiecare resursa
                std::cout << "Sumar resurse (via std::for_each):\n";
                int idx = 0;
                std::for_each(vec.begin(), vec.end(),
                    [&idx](const ResursaCloud* r) {
                        std::cout << "  [" << idx++ << "] " << r->toString() << "\n";
                    });
            }
            else if (subopt == 5) {
                // std::pair<int, double> + structured bindings (C++17)
                // Gaseste resursa cu costul maxim si returneaza (index, cost).
                auto cautaCelMaiScump = [&vec]() -> std::pair<int, double> {
                    int idxMax = 0;
                    double costMax = vec[0]->calculeazaCost();
                    for (std::size_t i = 1; i < vec.size(); i++) {
                        double c = vec[i]->calculeazaCost();
                        if (c > costMax) {
                            costMax = c;
                            idxMax = static_cast<int>(i);
                        }
                    }
                    return {idxMax, costMax};
                };
                // Structured bindings (C++17) — despachetare std::pair
                auto [idx, cost] = cautaCelMaiScump();
                std::cout << "Cea mai scumpa resursa: [" << idx << "] "
                          << vec[static_cast<std::size_t>(idx)]->toString()
                          << " — cost: " << cost << " RON/ora\n";
            }
            else {
                std::cout << "Sub-optiune invalida.\n";
            }
        }
        else if (opt == 13) {
            Logger::getInstance().afiseazaJurnal();
        }
        else if (opt == 14) {
            std::cout << "\n--- Creare server prin factory ---\n";
            std::cout << "Provider curent: " << client.getFabrica()->getNumeProvider() << "\n";
            std::cout << "1. Creare ServerDedicat\n";
            std::cout << "2. Creare ServerVirtual\n";
            int subopt = citesteOptiune("Sub-optiune: ");

            ResursaCloud* resursaNoua = nullptr;
            if (subopt == 1) {
                resursaNoua = client.creeazaServerDedicat();
            } else if (subopt == 2) {
                resursaNoua = client.creeazaServerVirtual();
            } else {
                std::cout << "Optiune invalida.\n";
                continue;
            }

            if (resursaNoua != nullptr) {
                try {
                    depozit.adauga(resursaNoua);
                    Logger::getInstance().log("CRUD: Server creat prin Factory, index=" +
                        std::to_string(depozit.getNr() - 1));
                    std::cout << "Server creat cu succes prin factory si adaugat in depozit. [index="
                              << depozit.getNr() - 1 << "]\n";
                } catch (const ExceptieCapacitateDepasita &e) {
                    std::cout << "Eroare capacitate: " << e.what() << "\n";
                    delete resursaNoua;
                }
            } else {
                std::cout << "Eroare la crearea serverului din factory.\n";
            }
        }
        else {
            std::cout << "Optiune invalida.\n";
        }
    }
}
int main() {
    // Colectii STL
    std::vector<Server*> servere;
    std::vector<IP> ipuri;
    std::vector<MasinaVirtuala> vms;
    std::vector<Datacenter> datacentere;

    // Colectie polimorfica gestionata prin Depozitar
    Depozitar<ResursaCloud, 50> depozitResurse;

    // Instantiere fabrici concrete si injectare in client
    FabricaProviderA fabricaA;
    FabricaProviderB fabricaB;
    ClientCloud client(&fabricaA); // Default: Provider A

    // Stack unwinding la pornire
    demonstreazaStackUnwinding();

    int optiune = -1;
    while (true) {
        std::cout << "\n========== MENIU PRINCIPAL ==========\n";
        std::cout
            << "1. Server\n2. IP\n3. Masina Virtuala\n4. Datacenter\n"
            << "5. Resurse Cloud (Polimorfism)\n"
            << "6. Jurnal sistem (Singleton Logger)\n"
            << "7. Selectare provider (Abstract Factory)\n"
            << "0. Iesire\n";
        optiune = citesteOptiune("Optiune: ");

        if (optiune == 0) {
            std::cout << "La revedere!\n";
            break;
        }

        switch (optiune) {
        case 1:
            menuServer(servere, vms);
            break;
        case 2:
            menuIP(ipuri);
            break;
        case 3:
            menuMasinaVirtuala(vms, servere);
            break;
        case 4:
            menuDatacenter(datacentere, servere);
            break;
        case 5:
            menuResurseCloud(depozitResurse, client);
            break;
        case 6:
            Logger::getInstance().afiseazaJurnal();
            break;
        case 7: {
            std::cout << "Provider curent: " << client.getFabrica()->getNumeProvider() << "\n";
            std::cout << "Selectati noul provider:\n";
            std::cout << "1. Provider A (AWS-style)\n";
            std::cout << "2. Provider B (Azure-style)\n";
            int choice = citesteOptiune("Optiune: ");
            
            // Schimba fabrica = schimba furnizorul. Clientul nu stie nimic despre A sau B.
            if (choice == 1) {
                client.setFabrica(&fabricaA);
                std::cout << "Provider schimbat cu succes pe A.\n";
                Logger::getInstance().log("Provider schimbat pe A");
            } else if (choice == 2) {
                client.setFabrica(&fabricaB);
                std::cout << "Provider schimbat cu succes pe B.\n";
                Logger::getInstance().log("Provider schimbat pe B");
            } else {
                std::cout << "Optiune invalida.\n";
            }
            break;
        }
        default:
            std::cout << "Optiune invalida.\n";
            break;
        }
    }

    // Cleanup: dezleaga VM-urile de servere
    for (auto& vm : vms) {
        vm.setServerGazda(nullptr);
    }

    // Eliberare servere alocate cu new
    for (auto* s : servere) {
        delete s;
    }
    servere.clear();

    // Eliberare resurse cloud din Depozitar — delete prin pointer la baza
    // cu destructor virtual asigura apelul corect al destructorilor.
    // Depozitar nu le sterge singur (nu detine obiectele).
    for (int i = 0; i < depozitResurse.getNr(); i++) {
        delete depozitResurse.get(i);
    }

    return 0;
}
