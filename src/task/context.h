// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"

class Dissolve;
class ProcedurePool;

class TaskContext
{

    public:
    explicit TaskContext(const ProcedurePool& procPool);
    TaskContext(const ProcedurePool& procPool, Dissolve& dissolve);
    ~TaskContext() = default;

    private:
    const ProcedurePool& procedurePool_;
    OptionalReferenceWrapper<Dissolve> dissolve_;

    public:
    const ProcedurePool& procedurePool();
    Dissolve& dissolve();

};