// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once


class Procedure;
class TaskContext;

class Task : public Serialisable<const CoreData &>
{
    private:
    TaskContext context_;
    Procedure procedure_;
};