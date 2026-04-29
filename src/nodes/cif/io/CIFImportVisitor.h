// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "CIFImportParserBaseVisitor.h"
#include "nodes/cif/io/cifContext.h"
#include "templates/optionalRef.h"
#include <antlr4-runtime.h>

// CIFImport Visitor for ANTLR
class CIFImportVisitor : CIFImportParserBaseVisitor
{
    public:
    CIFImportVisitor(CIFContext::CIFTags &tags);
    ~CIFImportVisitor() override = default;

    /*
     * Data
     */
    private:
    // Dictionary data storage
    CIFContext::CIFTags &tags_;

    public:
    // Extract information from tree
    void extract(CIFImportParser::CifContext *tree);

    /*
     * Visitor Overrides
     */
    private:
    antlrcpp::Any visitDataBlockHeading(CIFImportParser::DataBlockHeadingContext *ctx) override;
    antlrcpp::Any visitTaggedData(CIFImportParser::TaggedDataContext *ctx) override;
    antlrcpp::Any visitLoopedData(CIFImportParser::LoopedDataContext *ctx) override;
};
