// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/TextPrimitiveVisitor.h"
#include "gui/render/TextPrimitiveErrorListeners.h"
#include "gui/render/fontInstance.h"
#include "gui/render/symbol.h"

// Return enum options for EscapedFormatter
EnumOptions<TextPrimitiveVisitor::EscapedFormatter> TextPrimitiveVisitor::escapedFormatters()
{
    return EnumOptions<TextPrimitiveVisitor::EscapedFormatter>("EscapedFormatter",
                                                               {{TextPrimitiveVisitor::BoldFormatter, "b"},
                                                                {TextPrimitiveVisitor::ItalicFormatter, "it"},
                                                                {TextPrimitiveVisitor::SubScriptFormatter, "sub"},
                                                                {TextPrimitiveVisitor::SuperScriptFormatter, "sup"}});
}

/*
 * Construction
 */

// Return the topmost format in the stack
const TextFormat &TextPrimitiveVisitor::currentFormat() const
{
    assert(formatStack_.size() != 0);

    return formatStack_.back();
}

// Construct description within supplied object, from given tree
std::vector<TextFragment> TextPrimitiveVisitor::create(TextPrimitiveParser::PrimitiveContext *tree, FontInstance *fontInstance)
{
    fontInstance_ = fontInstance;
    horizontalPosition_ = 0.0;
    fragments_.clear();
    formatStack_.clear();
    formatStack_.emplace_back();

    // Traverse the AST
    visitChildren(tree);

    return fragments_;
}

/*
 * Private Functions
 */

// Add new text fragment
void TextPrimitiveVisitor::addText(std::string_view text, const TextFormat format)
{
    // Add new fragment using supplied format settings
    fragments_.emplace_back(text, format.scale(), Vec3<double>(horizontalPosition_, format.y(), 0.0), format.isItalic(),
                            format.isBold());

    // Update horizontal position
    horizontalPosition_ += fontInstance_->boundingBoxWidth(text) * format.scale();
}

/*
 * Visitor Overrides
 */

antlrcpp::Any TextPrimitiveVisitor::visitPlainText(TextPrimitiveParser::PlainTextContext *ctx)
{
    addText(ctx->getText(), currentFormat());

    return visitChildren(ctx);
}

antlrcpp::Any TextPrimitiveVisitor::visitFormatter(TextPrimitiveParser::FormatterContext *ctx)
{
    // Determine supplied formatter
    auto formatter = escapedFormatters().enumeration(ctx->FormatterEscape()->getText().substr(1));

    // Copy existing format
    auto format = formatStack_.back();

    switch (formatter)
    {
        case (TextPrimitiveVisitor::BoldFormatter):
            format.setBold(true);
            break;
        case (TextPrimitiveVisitor::ItalicFormatter):
            format.setItalic(true);
            break;
        case (TextPrimitiveVisitor::SubScriptFormatter):
            format.adjustY(-fontInstance_->fontBaseHeight() * format.scale() * (1.0 / 3.0));
            format.setScale(0.583 * format.scale());
            break;
        case (TextPrimitiveVisitor::SuperScriptFormatter):
            format.adjustY(fontInstance_->fontBaseHeight() * format.scale() * (2.0 / 3.0));
            format.setScale(0.583 * format.scale());
            break;
        default:
            throw(std::runtime_error("Unhandled formatting escape sequence found in text.\n"));
            break;
    }

    // Push new format
    formatStack_.emplace_back(format);

    auto result = visitChildren(ctx);

    formatStack_.pop_back();

    return result;
}

antlrcpp::Any TextPrimitiveVisitor::visitSymbol(TextPrimitiveParser::SymbolContext *ctx)
{
    // Get symbol data
    auto symbol = SymbolData::symbol(ctx->Symbol()->getText());

    addText(SymbolData::symbol(symbol), formatStack_.back());

    return visitChildren(ctx);
}

/*
    // Set fragment info
    Vec3<double> translation(horizontalPosition_, format->y(), 0.0);
    QString textToAdd;
    if (!format->symbol())
        textToAdd = text;
    else
    {
        // Must convert the supplied text into a symbol - use '??' if we don't recognise it
        SymbolData *symbol = SymbolData::symbol(text);
        if (symbol)
            textToAdd = symbol->character;
        else
            textToAdd = "<?>";
    }
    fragments_.emplace_back(ctx->getText(), format->scale(), Vec3<double>(horizontalPosition_, format->y(), 0.0),
   format->italic(), format->bold());

    // We have just added some text, so update the horizontal position
    horizontalPosition_ += fontInstance_->boundingBoxWidth(qPrintable(textToAdd)) * format->scale();

    return visitChildren(ctx);

  */

/*
// Add text fragment
bool TextPrimitiveVisitor::addFragment(QString text)
{
    TextFragment *fragment = fragments_.add();
    if (formatStack_.nItems() == 0)
    {
        Messenger::print("Internal Error: No TextFormat on stack in TextPrimitiveVisitor::addFragment().\n");
        fragment->set(text);
        return false;
    }

    // Get topmost TextFormat
    TextFormat *format = formatStack_.last();

    // Set fragment info
    Vec3<double> translation(horizontalPosition_, format->y(), 0.0);
    QString textToAdd;
    if (!format->symbol())
        textToAdd = text;
    else
    {
        // Must convert the supplied text into a symbol - use '??' if we don't recognise it
        SymbolData *symbol = SymbolData::symbol(text);
        if (symbol)
            textToAdd = symbol->character;
        else
            textToAdd = "<?>";
    }
    fragment->set(textToAdd, format->scale(), translation, format->italic(), format->bold());

    // We have just added some text, so update the horizontal position
    horizontalPosition_ += fontInstance_->boundingBoxWidth(qPrintable(textToAdd)) * format->scale();

    return true;
}

// Add escape code
bool TextPrimitiveVisitor::addEscape(TextPrimitiveVisitor::EscapeSequence escSeq)
{
    // Copy topmost formatting node first, since we retain any previously-set (i.e. nested) formats
    TextFormat *topMostFormat = formatStack_.last();
    TextFormat *newFormat = formatStack_.add();
    if (topMostFormat)
        (*newFormat) = (*topMostFormat);
    else
        Messenger::print("Internal Error: No topmost TextFormat to copy from in TextPrimitiveVisitor::addEscape().\n");

    // Deal with the escape sequence
    switch (escSeq)
    {
        // Add a symbol (flags that any text added to it should be converted to a named symbol)
        case (TextPrimitiveVisitor::SymbolFormatter):
            newFormat->setSymbol(true);
            break;
        default:
            Messenger::print("Escape {} not handled in TextPrimitiveVisitor::addEscape().\n", escSeq);
            return false;
            break;
    }

    return true;
}

// Remove escape code
void TextPrimitiveVisitor::removeEscape() { formatStack_.removeLast(); }
*/
