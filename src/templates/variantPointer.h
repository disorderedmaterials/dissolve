// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QVariant>

// Simple class to convert between QVariant pointer (void*) and a custom class pointer
template <class A> class VariantPointer
{
    private:
    // Pointer to target class
    A *pointer_;

    public:
    VariantPointer(A *ptr) { pointer_ = ptr; }
    VariantPointer(QVariant variant) { pointer_ = (A *)variant.value<void *>(); }

    operator QVariant() { return QVariant::fromValue((void *)pointer_); }

    operator A *() { return dynamic_cast<A *>(pointer_); }
};
