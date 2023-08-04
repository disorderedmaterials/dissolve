// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"

class Dissolve;
class ProcessPool;

class TaskContext
{

    public:
    explicit TaskContext(const ProcessPool& procPool);
    TaskContext(const ProcessPool& procPool, Dissolve& dissolve);
    ~TaskContext() = default;

    private:
    const ProcessPool& processPool_;
    OptionalReferenceWrapper<Dissolve> dissolve_;

    public:
    const ProcessPool& processPool();
    Dissolve& dissolve();

};