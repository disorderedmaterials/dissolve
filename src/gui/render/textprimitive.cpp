/*
    *** Text Primitive
    *** src/gui/render/textprimitive.cpp
    Copyright T. Youngs 2013-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/render/textprimitive.h"
#include "base/sysfunc.h"
#include "gui/render/fontinstance.h"
#include "gui/render/symbol.h"
#include "gui/render/textprimitive_grammar.hh"
#include <QtGui/qopengl.h>

// Static members
TextPrimitive *TextPrimitive::target_ = nullptr;
FontInstance *TextPrimitive::fontInstance_ = nullptr;
QString TextPrimitive::stringSource_;
int TextPrimitive::stringPos_, TextPrimitive::stringLength_;
List<TextFormat> TextPrimitive::formatStack_;
double TextPrimitive::horizontalPosition_;

TextPrimitive::TextPrimitive() : ListItem<TextPrimitive>() {}

TextPrimitive::~TextPrimitive() {}

// Return enum options for TextAnchor
EnumOptions<TextPrimitive::TextAnchor> &TextPrimitive::textAnchors()
{
    static EnumOptionsList TextAnchorOptions = EnumOptionsList()
                                               << EnumOption(TextPrimitive::TopLeftAnchor, "TopLeft")
                                               << EnumOption(TextPrimitive::TopMiddleAnchor, "TopMiddle")
                                               << EnumOption(TextPrimitive::TopRightAnchor, "TopRight")
                                               << EnumOption(TextPrimitive::MiddleLeftAnchor, "MiddleLeft")
                                               << EnumOption(TextPrimitive::CentralAnchor, "Central")
                                               << EnumOption(TextPrimitive::MiddleRightAnchor, "MiddleRight")
                                               << EnumOption(TextPrimitive::BottomLeftAnchor, "BottomLeft")
                                               << EnumOption(TextPrimitive::BottomMiddleAnchor, "BottomMiddle")
                                               << EnumOption(TextPrimitive::BottomRightAnchor, "BottomRight");

    static EnumOptions<TextPrimitive::TextAnchor> options("TextAnchor", TextAnchorOptions);

    return options;
}

// Return enum options for TextAnchor
EnumOptions<TextPrimitive::EscapeSequence> &TextPrimitive::escapeSequences()
{
    static EnumOptionsList EscapeSequenceOptions =
        EnumOptionsList() << EnumOption(TextPrimitive::BoldEscape, "b") << EnumOption(TextPrimitive::ItalicEscape, "it")
                          << EnumOption(TextPrimitive::NewLineEscape, "n") << EnumOption(TextPrimitive::SubScriptEscape, "sub")
                          << EnumOption(TextPrimitive::SuperScriptEscape, "sup")
                          << EnumOption(TextPrimitive::SymbolEscape, "sym");

    static EnumOptions<TextPrimitive::EscapeSequence> options("EscapeSequence", EscapeSequenceOptions);

    return options;
}

// Set data
void TextPrimitive::set(FontInstance &fontInstance, QString text, Vec3<double> anchorPoint,
                        TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix4 localRotation,
                        double textSize, bool flat)
{
    // Call the parser
    generateFragments(&fontInstance, this, text);

    anchorPoint_ = anchorPoint;
    anchorPosition_ = anchorPosition;
    adjustmentVector_ = adjustmentVector;
    localRotation_ = localRotation;
    textSize_ = textSize;
    flat_ = flat;
}

// Return transformation matrix to use when rendering the text
Matrix4 TextPrimitive::transformationMatrix(FontInstance &fontInstance, const Matrix4 &viewMatrixInverse, double baseFontSize,
                                            TextFragment *fragment)
{
    Matrix4 textMatrix, A;
    Vec3<double> lowerLeft, upperRight, anchorPos, anchorPosRotated, textCentre;

    // Calculate scaling factor for font
    double scale = fontInstance.fontBaseHeight() * textSize_ / baseFontSize;

    // Calculate bounding box and anchor position on it
    boundingBox(fontInstance, lowerLeft, upperRight);
    switch (anchorPosition_)
    {
        case (TextPrimitive::TopLeftAnchor):
            anchorPos.set(lowerLeft.x, upperRight.y, 0.0);
            break;
        case (TextPrimitive::TopMiddleAnchor):
            anchorPos.set((lowerLeft.x + upperRight.x) * 0.5, upperRight.y, 0.0);
            break;
        case (TextPrimitive::TopRightAnchor):
            anchorPos = upperRight;
            break;
        case (TextPrimitive::MiddleLeftAnchor):
            anchorPos.set(lowerLeft.x, (lowerLeft.y + upperRight.y) * 0.5, 0.0);
            break;
        case (TextPrimitive::CentralAnchor):
            anchorPos.set((lowerLeft.x + upperRight.x) * 0.5, (lowerLeft.y + upperRight.y) * 0.5, 0.0);
            break;
        case (TextPrimitive::MiddleRightAnchor):
            anchorPos.set(upperRight.x, (lowerLeft.y + upperRight.y) * 0.5, 0.0);
            break;
        case (TextPrimitive::BottomLeftAnchor):
            anchorPos = lowerLeft;
            break;
        case (TextPrimitive::BottomMiddleAnchor):
            anchorPos.set((lowerLeft.x + upperRight.x) * 0.5, lowerLeft.y, 0.0);
            break;
        case (TextPrimitive::BottomRightAnchor):
            anchorPos.set(upperRight.x, lowerLeft.y, 0.0);
            break;
        default:
            break;
    }

    // Rotate anchor position with local rotation matrix
    textCentre = (lowerLeft + upperRight) * 0.5;
    anchorPosRotated = localRotation_ * (anchorPos - textCentre) * scale;

    // Construct matrix

    // -- Translate to centre of text bounding box (not rotated) accounting for fragment translation if one was specified
    if (fragment)
        textCentre -= fragment->translation();
    textMatrix.createTranslation(-textCentre);

    // -- Apply scaled local rotation matrix (if not flat)
    if (flat_)
        A = viewMatrixInverse;
    else
        A = localRotation_;
    A.applyScaling(scale, scale, scale);
    textMatrix.preMultiply(A);

    // -- Apply translation to text anchor point
    textMatrix.applyPreTranslation(-anchorPosRotated + anchorPoint_ + adjustmentVector_ * scale);

    // -- Apply fragment specific operations
    if (fragment)
    {
        // -- Apply local scaling to text (if fragment was provided)
        textMatrix.applyScaling(fragment->scale());
        // -- Apply local shear to text (if fragment is italic)
        if (fragment->italic())
            textMatrix.applyShearX(0.2);
    }

    return textMatrix;
}

// Calculate bounding box of primitive
void TextPrimitive::boundingBox(FontInstance &fontInstance, Vec3<double> &lowerLeft, Vec3<double> &upperRight)
{
    // Set initial lowerLeft and upperRight from the first primitive in the list
    if (fragments_.first())
        fontInstance.boundingBox(qPrintable(fragments_.first()->text()), lowerLeft, upperRight);
    else
    {
        // No fragments in list!
        lowerLeft.zero();
        upperRight.zero();
        return;
    }

    // Loop over remaining fragments, keeping track of the total width of the primitive and the max/min y values
    Vec3<double> ll, ur;
    // 	double width = upperRight.x - lowerLeft.x;
    for (auto *fragment = fragments_.first()->next(); fragment != nullptr; fragment = fragment->next())
    {
        // Get bounding box for this fragment
        fontInstance.boundingBox(qPrintable(fragment->text()), ll, ur);

        // Scale the box by the current scaling factor...
        ur.x = ll.x + (ur.x - ll.x) * fragment->scale();
        ur.y = ll.y + (ur.y - ll.y) * fragment->scale();

        // Translate the box by the defined amount
        ll += fragment->translation();
        ur += fragment->translation();

        // Update lowerLeft and upperRight values
        if (ll.y < lowerLeft.y)
            lowerLeft.y = ll.y;
        if (ur.y > upperRight.y)
            upperRight.y = ur.y;
        if (ur.x > upperRight.x)
            upperRight.x = ur.x;
    }
}

// Render primitive
void TextPrimitive::render(FontInstance &fontInstance, const Matrix4 &viewMatrix, const Matrix4 &viewMatrixInverse,
                           double baseFontSize)
{
    Matrix4 textMatrix;

    // Loop over fragments
    for (auto *fragment = fragments_.first(); fragment != nullptr; fragment = fragment->next())
    {
        textMatrix = viewMatrix * transformationMatrix(fontInstance, viewMatrixInverse, baseFontSize, fragment);
        glLoadMatrixd(textMatrix.matrix());

        // Draw bounding boxes around each fragment
        if (false)
        {
            glDisable(GL_LINE_STIPPLE);
            glLineWidth(1.0);
            Vec3<double> ll, ur;
            fontInstance.boundingBox(qPrintable(fragment->text()), ll, ur);
            glBegin(GL_LINE_LOOP);
            glVertex3d(ll.x, ll.y, 0.0);
            glVertex3d(ur.x, ll.y, 0.0);
            glVertex3d(ur.x, ur.y, 0.0);
            glVertex3d(ll.x, ur.y, 0.0);
            glEnd();
        }

        // Apply any scaling stored in the FontInstance
        glScaled(fontInstance.scaleFactor(), fontInstance.scaleFactor(), fontInstance.scaleFactor());

        if (fragment->bold())
        {
            // Render the text twice - once with lines, and once with polygon fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            fontInstance.renderText(qPrintable(fragment->text().toUtf8()));
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            fontInstance.renderText(qPrintable(fragment->text().toUtf8()));
        }
        else
            fontInstance.renderText(qPrintable(fragment->text().toUtf8()));
    }
}

/*
 * Generation
 */

// Bison-generated ExpressionParser_lex()
int TextPrimitiveParser_lex()
{
    if (!TextPrimitive::target())
        return 0;
    return TextPrimitive::target()->lex();
}

// Get next character from current input stream
QChar TextPrimitive::getChar()
{
    // Are we at the end of the current string?
    if (stringPos_ == stringLength_)
        return 0;

    // Return current char and increment index
    return stringSource_.at(stringPos_++);
}

// Peek next character from current input stream
QChar TextPrimitive::peekChar() { return (stringPos_ == stringLength_ ? 0 : stringSource_.at(stringPos_)); }

// 'Replace' last character read from current input stream
void TextPrimitive::unGetChar() { --stringPos_; }

// Parser lexer, called by yylex()
int TextPrimitive::lex()
{
    bool done, isEscape;
    static QString token;
    QChar c;

    // Reset some variables
    token.clear();
    isEscape = false;
    done = false;

    do
    {
        c = getChar();
        if (c == QChar(0))
        {
            if (token.length() == 0)
                return 0;
            done = true;
            break;
        }
        else if (c == QChar('\\'))
        {
            if (token.length() == 0)
                isEscape = true;
            else
            {
                unGetChar();
                done = true;
            }
        }
        else if (c == QChar('{'))
        {
            if (token.length() == 0)
                return '{';
            unGetChar();
            done = true;
        }
        else if (c == QChar('}'))
        {
            if (token.length() == 0)
                return '}';
            unGetChar();
            done = true;
        }
        else
            token += c;

        // Break out if we are finished
        if (done)
            break;
    } while (c != 0);

    // Did we find an escape sequence, or just normal text?
    if (isEscape)
    {
        // Is the text a recognised escape?
        if (TextPrimitive::escapeSequences().isValid(qPrintable(token)))
        {
            TextPrimitiveParser_lval.escSeq = TextPrimitive::escapeSequences().enumeration(qPrintable(token));
            return DISSOLVE_TEXT_ESCAPE;
        }
        else
        {
            Messenger::printVerbose("Error: String '{}' is not a valid escape sequence.\n", qPrintable(token));
            return DISSOLVE_TEXT_FAIL;
        }
    }
    else
    {
        TextPrimitiveParser_lval.text = &token;
        return DISSOLVE_TEXT_TEXT;
    }

    return 0;
}

// Generate TextFragment data for specified TextPrimitive from supplied string
bool TextPrimitive::generateFragments(FontInstance *fontInstance, TextPrimitive *target, QString inputString)
{
    // Set / reset variables
    target_ = target;
    fontInstance_ = fontInstance;
    stringPos_ = 0;
    stringSource_ = inputString;
    stringLength_ = stringSource_.length();

    // Clear the format stack and create a basic format
    formatStack_.clear();
    formatStack_.add();
    horizontalPosition_ = 0.0;

    return (TextPrimitiveParser_parse() == 0);
}

// Return current target
TextPrimitive *TextPrimitive::target() { return target_; }

// Add text fragment
bool TextPrimitive::addFragment(QString text)
{
    TextFragment *fragment = fragments_.add();
    if (formatStack_.nItems() == 0)
    {
        Messenger::print("Internal Error: No TextFormat on stack in TextPrimitive::addFragment().\n");
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
bool TextPrimitive::addEscape(TextPrimitive::EscapeSequence escSeq)
{
    // Copy topmost formatting node first, since we retain any previously-set (i.e. nested) formats
    TextFormat *topMostFormat = formatStack_.last();
    TextFormat *newFormat = formatStack_.add();
    if (topMostFormat)
        (*newFormat) = (*topMostFormat);
    else
        Messenger::print("Internal Error: No topmost TextFormat to copy from in TextPrimitive::addEscape().\n");

    // Deal with the escape sequence
    switch (escSeq)
    {
        // Add bold level
        case (TextPrimitive::BoldEscape):
            newFormat->setBold(true);
            break;
        // Add italic level
        case (TextPrimitive::ItalicEscape):
            newFormat->setItalic(true);
            break;
        // Newline
        case (TextPrimitive::NewLineEscape):
            // 			newFormat->		TODO
            break;
        // Add subscript level - adjust baseline position and scale of current format
        case (TextPrimitive::SubScriptEscape):
            newFormat->adjustY(-fontInstance_->fontBaseHeight() * newFormat->scale() * (1.0 / 3.0));
            newFormat->setScale(0.583 * newFormat->scale());
            break;
        // Add superscript level - adjust baseline position and scale of current format
        case (TextPrimitive::SuperScriptEscape):
            newFormat->adjustY(fontInstance_->fontBaseHeight() * newFormat->scale() * (2.0 / 3.0));
            newFormat->setScale(0.583 * newFormat->scale());
            break;
        // Add a symbol (flags that any text added to it should be converted to a named symbol)
        case (TextPrimitive::SymbolEscape):
            newFormat->setSymbol(true);
            break;
        default:
            Messenger::print("Escape {} not handled in TextPrimitive::addEscape().\n", escSeq);
            return false;
            break;
    }

    return true;
}

// Remove escape code
void TextPrimitive::removeEscape() { formatStack_.removeLast(); }
