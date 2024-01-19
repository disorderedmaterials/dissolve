// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atom.h"
#include <vector>

using AtomVector = std::vector<Atom>;

class AtomRef {
 private:
  Atom *ref_{nullptr}, *origin_{nullptr};
 public:
  AtomRef();
  AtomRef(Atom *ref, Atom * origin);
  operator Atom*() const;
  Atom &operator*();
  const Atom &operator*() const;
  Atom *operator->();
  const Atom *operator->() const;
  bool operator==(const AtomRef& other) const;
  bool operator==(Atom *other) const;
  AtomRef &operator++();
  int globalAtomIndex() const;
};
