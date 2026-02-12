// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "gui/render/textFragment.h"
#include "math/matrix4.h"
#include "math/vector3.h"
#include "templates/optionalRef.h"
#include <QString>

// Forward Declarations
class FontInstance;

// Text Primitive
class TextPrimitive
{
    public:
    TextPrimitive() = default;
    ~TextPrimitive() = default;
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
    static EnumOptions<TextPrimitive::TextAnchor> textAnchors();

    /*
     * Definition
     */
    private:
    // Coordinates of anchorpoint of text
    Vector3 anchorPoint_;
    // Location of anchorpoint on text bounding box
    TextAnchor anchorPosition_;
    // Vector by which to adjust position of text
    Vector3 adjustmentVector_;
    // Local transform matrix for the text
    Matrix4 localRotation_;
    // Whether text should be rendered flat (w.r.t viewer's screen plane)
    bool flat_;
    // Text size
    double textSize_;
    // Text fragments to render
    std::vector<TextFragment> fragments_;

    public:
    // Set data
    bool set(FontInstance *fontInstance, QString text, Vector3 anchorPoint, TextAnchor anchorPosition, Vector3 adjustmentVector,
             Matrix4 localRotation, double textSize, bool flat);
    // Return transformation matrix to use when rendering (including fragment scale/translation if one is specified)
    Matrix4 transformationMatrix(FontInstance &fontInstance, const Matrix4 &viewMatrixInverse, double baseFontSize,
                                 OptionalReferenceWrapper<const TextFragment> optFragment = std::nullopt) const;
    // Calculate bounding box of primitive
    void boundingBox(FontInstance &fontInstance, Vector3 &lowerLeft, Vector3 &upperRight) const;
    // Render primitive
    void render(FontInstance &fontInstance, const Matrix4 &viewMatrix, const Matrix4 &viewMatrixInverse,
                double baseFontSize) const;
};
