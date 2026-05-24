#pragma once

#include <iostream>
#include <string>

class IObject {
private:
  static int contorGlobal;
  int id;

public:
  IObject();
  IObject(const IObject &copie);
  IObject &operator=(const IObject &) = delete;
  virtual ~IObject() = default;

  int getId() const noexcept;
  bool operator==(const IObject &alt) const;

  virtual std::string toString() const = 0;

  friend std::ostream &operator<<(std::ostream &os, const IObject &obj);
};
