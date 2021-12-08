// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <typeindex>

namespace dissolve {
  class any_ptr {
  private:
    void* ptr_;
    std::type_index type_;
  public:
    template <typename T> any_ptr(T &data) : ptr_(&data), type_(typeid(data)) {}
  };
}
