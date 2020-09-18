/*
    *** Text Primitive
    *** src/gui/render/textprimitive.h
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

#pragma once

#include "base/enumoptions.h"
#include "gui/render/textformat.h"
#include "gui/render/textfragment.h"
#include "math/cuboid.h"
#include "math/matrix4.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include <QString>

// External Declarations
extern int TextPrimitiveParser_parse();

// Forward Declarations
class FontInstance;

// Text Primitive
class TextPrimitive : public ListItem<TextPrimitive>
{
    public:
    TextPrimitive();
    ~TextPrimitive();
    // Text Anchors enum
    enum TextAnchor
    {
        TopLeftAnchor,
        TopMiddleAnchor,
        TopRightAnchor,
        MiddleLeftAnchor,
        CentralAnchor,
        MiddleRightAnchor,
        BottomLeftAnchor,
        BottomMiddleAnchor,
        BottomRightAnchor,
        nTextAnchors
    };
    // Return enum options for TextAnchor
    static EnumOptions<TextPrimitive::TextAnchor> &textAnchors();
    // Escape Sequence enum
    enum EscapeSequence
    {
        BoldEscape,
        ItalicEscape,
        NewLineEscape,
        SubScriptEscape,
        SuperScriptEscape,
        SymbolEscape,
        nEscapeSequences
    };
    // Return enum options for TextAnchor
    static EnumOptions<TextPrimitive::EscapeSequence> &escapeSequences();

    /*
     * Definition
     */
    private:
    // Coordinates of anchorpoint of text
    Vec3<double> anchorPoint_;
    // Location of anchorpoint on text bounding box
    TextAnchor anchorPosition_;
    // Vector by which to adjust position of text
    Vec3<double> adjustmentVector_;
    // Local transform matrix for the text
    Matrix4 localRotation_;
    // Whether text should be rendered flat (w.r.t viewer's screen plane)
    bool flat_;
    // Text size
    double textSize_;
    // Text fragments to render
    List<TextFragment> fragments_;

    public:
    // Set data
    void set(FontInstance &fontInstance, QString text, Vec3<double> anchorPoint, TextAnchor anchorPosition,
             Vec3<double> adjustmentVector, Matrix4 localRotation, double textSize, bool flat);
    // Return transformation matrix to use when rendering (including fragment scale/translation if one is specified)
    Matrix4 transformationMatrix(FontInstance &fontInstance, const Matrix4 &viewMatrixInverse, double baseFontSize,
                                 TextFragment *fragment = nullptr);
    // Calculate bounding box of primitive
    void boundingBox(FontInstance &fontInstance, Vec3<double> &lowerLeft, Vec3<double> &upperRight);
    // Render primitive
    void render(FontInstance &fontInstance, const Matrix4 &viewMatrix, const Matrix4 &viewMatrixInverse, double baseFontSize);

    /*
     * Generation
     */
    private:
    // Character string source
    static QString stringSource_;
    // Integer position in stringSource, total length of string, and starting position of current token/function
    static int stringPos_, stringLength_;
    // Get next character from current input stream
    static QChar getChar();
    // Peek next character from current input stream
    static QChar peekChar();
    // 'Replace' last character read from current input stream
    static void unGetChar();
    // Current target for generation
    static TextPrimitive *target_;
    // Current FontInstance
    static FontInstance *fontInstance_;
    // Format stack, used when generating primitive
    static List<TextFormat> formatStack_;
    // Current horizontal position, used when generating primitive
    static double horizontalPosition_;

    public:
    // Parser lexer, called by yylex()
    static int lex();
    // Generate TextFragment data for specified TextPrimitive from supplied string
    static bool generateFragments(FontInstance *fontInstance, TextPrimitive *target, QString inputString);
    // Return current target TextPrimitive
    static TextPrimitive *target();
    // Add text fragment
    bool addFragment(QString text);
    // Add escape marker
    static bool addEscape(TextPrimitive::EscapeSequence escSeq);
    // Remove escape marker
    static void removeEscape();
};
