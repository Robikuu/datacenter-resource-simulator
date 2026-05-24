#include "IP.h"
#include "ExceptiiCloud.h"

#include <cstdio>
#include <iomanip>

bool IP::ipv4InUint32(const std::string &ip, uint32_t &rezultat) {
  if (!valideazaIP4(ip))
    return false;

  unsigned int o1 = 0, o2 = 0, o3 = 0, o4 = 0;
  if (std::sscanf(ip.c_str(), "%u.%u.%u.%u", &o1, &o2, &o3, &o4) != 4)
    return false;
  if (o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255)
    return false;

  rezultat = (static_cast<uint32_t>(o1) << 24) |
             (static_cast<uint32_t>(o2) << 16) |
             (static_cast<uint32_t>(o3) << 8) | static_cast<uint32_t>(o4);
  return true;
}

void IP::uint32InIpv4(uint32_t valoare, char *buffer, std::size_t dimBuffer) {
  std::snprintf(buffer, dimBuffer, "%u.%u.%u.%u", (valoare >> 24) & 0xFFu,
                (valoare >> 16) & 0xFFu, (valoare >> 8) & 0xFFu,
                valoare & 0xFFu);
}

IP::IP(const std::string &adresa, const std::string &protocol, bool alocat)
    : adresaIP("0.0.0.0"), protocolIP("ipv4"), esteAlocat(alocat) {

  if (esteValidAdresaIP(adresa)) {
    this->adresaIP = adresa;
  } else {
    throw ExceptieValidare("Adresa IP invalida: \"" + adresa + "\"", adresa,
                           "constructor IP");
  }

  if (esteValidProtocolIP(protocol)) {
    this->protocolIP = protocol;
    for (auto &c : this->protocolIP)
      c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
  } else {
    this->protocolIP = "ipv4";
  }
}

// Copy ctor, operator= si destructor eliminati! (clasa are doar string)

std::ostream &operator<<(std::ostream &os, const IP &ip) {
  os << "Adresa IP: " << ip.adresaIP << "\n";
  os << "Protocol IP(IPv4 sau IPv6): " << ip.protocolIP << "\n";
  os << std::boolalpha << "Este Alocat IP: " << ip.esteAlocat << std::endl;
  return os;
}

std::istream &operator>>(std::istream &is, IP &ip) {
  std::string nouIP;

  while (true) {
    std::cout << "Introduceti adresa IP: ";
    is >> nouIP;
    if (!is)
      return is;
    if (IP::esteValidAdresaIP(nouIP)) {
      ip.adresaIP = nouIP;
      break;
    } else {
      std::cout << "Adresa IP nu este valida. Introduceti o adresa IP de tip "
                   "IPv4 sau IPv6";
    }
  }

  while (true) {
    std::cout << "Protocol clasa (IPv4/IPv6): ";
    std::string protNou;
    is >> protNou;
    if (!is)
      return is;
    if (IP::esteValidProtocolIP(protNou)) {
      ip.protocolIP = protNou;
      for (auto &c : ip.protocolIP)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
      break;
    } else {
      std::cout << "Protocolul introdusa nu este valida. Introduceti un "
                   "protocol valida.";
    }
  }

  std::cout << "Este alocat? ('true' sau 'false'): ";
  if (!(is >> std::boolalpha >> ip.esteAlocat)) {
    return is;
  }
  return is;
}

void IP::setAdresaIP(const std::string &nouIP) {
  if (nouIP.empty())
    return;
  if (esteValidAdresaIP(nouIP)) {
    this->adresaIP = nouIP;
  }
}

void IP::setProtocolIP(const std::string &nouProtocol) {
  if (nouProtocol.empty())
    return;
  if (esteValidProtocolIP(nouProtocol)) {
    this->protocolIP = nouProtocol;
    for (auto &c : this->protocolIP)
      c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
  }
}

void IP::setEsteAlocat(bool status) { this->esteAlocat = status; }

bool IP::operator<(const IP &ipComparat) const {
  uint32_t adresaStNum = 0;
  uint32_t adresaDrNum = 0;
  const bool stIPv4 = ipv4InUint32(this->adresaIP, adresaStNum);
  const bool drIPv4 = ipv4InUint32(ipComparat.adresaIP, adresaDrNum);

  if (stIPv4 && drIPv4) {
    if (adresaStNum != adresaDrNum) {
      return adresaStNum < adresaDrNum;
    }
  } else {
    int cmpAdresa = this->adresaIP.compare(ipComparat.adresaIP);
    if (cmpAdresa != 0) {
      return cmpAdresa < 0;
    }
  }

  int cmpProtocol = this->protocolIP.compare(ipComparat.protocolIP);
  if (cmpProtocol != 0) {
    return cmpProtocol < 0;
  }

  return this->esteAlocat < ipComparat.esteAlocat;
}

bool IP::operator>(const IP &ipComparat) const { return ipComparat < *this; }

bool IP::operator==(const IP &ipComparat) const {
  return this->adresaIP == ipComparat.adresaIP &&
         this->protocolIP == ipComparat.protocolIP &&
         this->esteAlocat == ipComparat.esteAlocat;
}

IP IP::operator-() const {
  IP rezultat(*this);

  uint32_t adresa = 0;
  if (!ipv4InUint32(this->adresaIP, adresa))
    return rezultat;

  const uint32_t mascaRetea = adresa & 0xFFFF0000u;
  char buffer[16];
  uint32InIpv4(mascaRetea, buffer, sizeof(buffer));
  rezultat.setAdresaIP(std::string(buffer));
  rezultat.setProtocolIP("ipv4");

  return rezultat;
}

IP IP::operator+(int val) const {
  IP rezultat(*this);

  uint32_t adresa = 0;
  if (!ipv4InUint32(this->adresaIP, adresa))
    return rezultat;

  long long suma = static_cast<long long>(adresa) + static_cast<long long>(val);
  if (suma < 0) {
    suma = 0;
  } else if (suma > 0xFFFFFFFFLL) {
    suma = 0xFFFFFFFFLL;
  }

  char buffer[16];
  uint32InIpv4(static_cast<uint32_t>(suma), buffer, sizeof(buffer));
  rezultat.setAdresaIP(std::string(buffer));
  rezultat.setProtocolIP("ipv4");

  return rezultat;
}

IP operator+(int val, const IP &ip) { return ip + val; }
