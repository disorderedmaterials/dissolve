// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "main/dissolve.h"
#include "main/keywords.h"

EnumOptions<TaskBlock::TaskKeyword> TaskBlock::keywords()
{
    return EnumOptions<TaskBlock::TaskKeyword>("TaskKeyword",
                                                {{TaskBlock::EndTaskKeyword, "EndTask"},
                                                 {TaskBlock::ProcedureKeyword, "Procedure"},
                                                 {TaskBlock::TargetsKeyword, "Targets"}});
}

bool TaskBlock::parse(LineParser &parser, Dissolve *dissolve, Task* task)
{
    return true;
}