#include "IObject.h"

int IObject::contorGlobal = 0;

IObject::IObject() : id(++contorGlobal) {}

IObject::IObject(const IObject &copie) : id(copie.id) {}

int IObject::getId() const noexcept { return this->id; }

bool IObject::operator==(const IObject &alt) const {
  return this->id == alt.id;
}

std::ostream &operator<<(std::ostream &os, const IObject &obj) {
  os << obj.toString();
  return os;
}
