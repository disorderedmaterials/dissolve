// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/CIFImportVisitor.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "io/import/CIFImportErrorListeners.h"

CIFImportVisitor::CIFImportVisitor(CIFHandler::CIFTags &tags) : tags_(tags) {}

/*
 * Data
 */

// Extract information from tree into supplied objects
void CIFImportVisitor::extract(CIFImportParser::CifContext *tree)
{
    // Traverse the AST
    visitChildren(tree);

    // Summarise
    Messenger::print("Read in {} unique CIF data tags.\n", tags_.size());
}

/*
 * Visitor Overrides
 */

antlrcpp::Any CIFImportVisitor::visitDataBlockHeading(CIFImportParser::DataBlockHeadingContext *ctx)
{
    tags_["DATA_"].push_back(std::string(DissolveSys::afterChar(ctx->getText(), "DATA_")));

    return visitChildren(ctx);
}

antlrcpp::Any CIFImportVisitor::visitTaggedData(CIFImportParser::TaggedDataContext *ctx)
{
    tags_[ctx->name->getText()].push_back(ctx->value->getText());

    return visitChildren(ctx);
}

antlrcpp::Any CIFImportVisitor::visitLoopedData(CIFImportParser::LoopedDataContext *ctx)
{
    // Sanity check table data size
    if (ctx->loop()->values.size() % ctx->loop()->columns.size())
        throw(std::runtime_error(fmt::format("CIF table has {} columns (first is '{}') but {} values (and C%V != 0).\n",
                                             ctx->loop()->columns.size(), ctx->loop()->columns.front()->getText(),
                                             ctx->loop()->values.size())));

    // Construct / retrieve dictionary elements for columns
    std::vector<std::reference_wrapper<std::vector<std::string>>> columns;
    for (auto *column : ctx->loop()->columns)
        columns.emplace_back(tags_[column->getText()]);

    // Add values to columns - data will be in row-major order
    auto colId = 0u;
    for (auto *value : ctx->loop()->values)
    {
        columns[colId].get().emplace_back(value->getText());
        ++colId;
        colId %= ctx->loop()->columns.size();
    }

    return visitChildren(ctx);
}
