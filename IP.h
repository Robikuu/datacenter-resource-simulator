#pragma once

#include <cctype>
#include <cstdint>
#include <iostream>
#include <string>

// adresaIP si protocolIP sunt std::string. Rule of 3 nu mai e necesar!

class IP {
private:
  std::string adresaIP;
  std::string protocolIP;
  bool esteAlocat;

  static bool ipv4InUint32(const std::string &ip, uint32_t &rezultat);
  static void uint32InIpv4(uint32_t valoare, char *buffer,
                           std::size_t dimBuffer);

  static bool valideazaIP4(const std::string &ip) {
    int puncte = 0;
    int numarCurent = 0;
    int cifreConsecutive = 0;

    for (char c : ip) {
      if (c == '.') {
        if (cifreConsecutive == 0)
          return false;
        puncte++;
        numarCurent = 0;
        cifreConsecutive = 0;
      } else if (c >= '0' && c <= '9') {
        numarCurent = numarCurent * 10 + (c - '0');
        if (numarCurent > 255)
          return false;
        cifreConsecutive++;
        if (cifreConsecutive > 3)
          return false;
      } else {
        return false;
      }
    }
    return (cifreConsecutive > 0 && puncte == 3);
  }

  static bool valideazaIP6(const std::string &ip) {
    int douaPuncte = 0;
    int caractereConsecutive = 0;
    for (char c : ip) {
      if (c == ':') {
        caractereConsecutive = 0;
        douaPuncte++;
      } else {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
              (c >= 'A' && c <= 'F')) ||
            caractereConsecutive >= 4)
          return false;
        caractereConsecutive++;
      }
    }
    return (douaPuncte >= 2 && douaPuncte <= 7);
  }

  static bool esteValidAdresaIP(const std::string &adresa) {
    if (adresa.empty())
      return false;
    return valideazaIP6(adresa) || valideazaIP4(adresa);
  }

  static bool esteValidProtocolIP(const std::string &prot) {
    if (prot.size() != 4)
      return false;
    return tolower(static_cast<unsigned char>(prot[0])) == 'i' &&
           tolower(static_cast<unsigned char>(prot[1])) == 'p' &&
           tolower(static_cast<unsigned char>(prot[2])) == 'v' &&
           (prot[3] == '4' || prot[3] == '6');
  }

public:
  explicit IP(const std::string &adresa = "0.0.0.0",
              const std::string &protocol = "ipv4", bool alocat = false);

  // Rule of Three nu mai e necesar manual, std::string se copiaza automat.

  friend std::ostream &operator<<(std::ostream &os, const IP &ip);
  friend std::istream &operator>>(std::istream &is, IP &ip);

  void setAdresaIP(const std::string &nouIP);
  void setProtocolIP(const std::string &nouProtocol);
  void setEsteAlocat(bool status);

  bool operator<(const IP &ipComparat) const;
  bool operator>(const IP &ipComparat) const;
  bool operator==(const IP &ipComparat) const;

  IP operator-() const;
  IP operator+(int val) const;

  const std::string &getAdresaIP() const { return this->adresaIP; }
  const std::string &getClasaIP() const { return this->protocolIP; }
  bool getEsteAlocat() const { return this->esteAlocat; }
};

IP operator+(int val, const IP &ip);
