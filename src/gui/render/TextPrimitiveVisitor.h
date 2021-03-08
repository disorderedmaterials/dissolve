// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "TextPrimitiveParserBaseVisitor.h"
#include "base/enumoptions.h"
#include "gui/render/textformat.h"
#include "gui/render/textfragment.h"
#include "templates/optionalref.h"
#include <antlr4-runtime.h>

// Forward Declarations
class FontInstance;

// TextPrimitive Visitor for ANTLR
class TextPrimitiveVisitor : TextPrimitiveParserBaseVisitor
{
    public:
    // Escaped Formatters Enum
    enum EscapedFormatter
    {
        BoldFormatter,
        ItalicFormatter,
        SubScriptFormatter,
        SuperScriptFormatter,
        nEscapedFormatters
    };
    // Return enum options for EscapedFormatter
    static EnumOptions<TextPrimitiveVisitor::EscapedFormatter> escapedFormatters();

    /*
     * Construction
     */
    private:
    // Generated fragments
    std::vector<TextFragment> fragments_;
    // Current horizontal position
    double horizontalPosition_;
    // Current font instance
    FontInstance *fontInstance_;
    // Format stack
    std::vector<TextFormat> formatStack_;

    private:
    // Return the topmost format in the stack
    const TextFormat &currentFormat() const;
    // Add new text fragment
    void addText(std::string_view text, const TextFormat format);

    public:
    // Construct description within supplied object, from given tree
    std::vector<TextFragment> create(TextPrimitiveParser::PrimitiveContext *tree, FontInstance *fontInstance);

    /*
     * Visitor Overrides
     */
    private:
    antlrcpp::Any visitPlainText(TextPrimitiveParser::PlainTextContext *ctx) override;
    antlrcpp::Any visitFormatter(TextPrimitiveParser::FormatterContext *ctx) override;
    antlrcpp::Any visitSymbol(TextPrimitiveParser::SymbolContext *ctx) override;
};
