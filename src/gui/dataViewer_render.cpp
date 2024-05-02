// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataViewer.h"

// Perform post-initialisation operations
void DataViewer::postInitialiseGL() {}

// Perform post-resize operations
void DataViewer::postResizeGL() {}

// Render 2D overlay content
void DataViewer::render2DOverlay()
{
    // Set up some metrics
    const auto overlaySpacing = 2.0 * pixelScaling_;
    const auto overlayTextSize = 12.0 * pixelScaling_;
    const auto legendLineLength = 20.0 * pixelScaling_;

    LineStyle(1.0).sendToGL(pixelScaling_);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_LINE_SMOOTH);

    /*
     * Draw indicators in top-left corner
     */

    QString indicatorText;
    if (view_.autoFollowType() == View::AllAutoFollow)
        indicatorText += "|A| ";
    else if (view_.autoFollowType() == View::XAutoFollow)
        indicatorText += "A\\sub{X} ";
    if (groupManager_.verticalShiftAmount() > 0)
        indicatorText += QString("S\\sub{%1}").arg(groupManager_.verticalShiftAmount());
    TextPrimitive indicatorPrimitive;
    indicatorPrimitive.set(&fontInstance_, indicatorText,
                           Vec3<double>(overlaySpacing, view_.viewportMatrix()[3] - overlaySpacing, 0.0),
                           TextPrimitive::TopLeftAnchor, Vec3<double>(), Matrix4(), overlayTextSize, false);
    glColor3d(0.0, 0.0, 0.0);
    Matrix4 identity;
    if (fontInstance_.fontOK())
        indicatorPrimitive.render(fontInstance_, identity, identity, 1.0);

    /*
     * Draw legend in top-right corner
     */

    auto maxTextWidth = -1.0;
    for (auto &rend : renderables_)
    {
        if (!rend->isVisible())
            continue;

        auto textWidth = fontInstance_.boundingBoxWidth(rend->name()) * overlayTextSize;
        if (textWidth > maxTextWidth)
            maxTextWidth = textWidth;
    }

    // Simple column layout - set the render position to be the left-hand edge of the longest text item
    glColor3d(0.0, 0.0, 0.0);
    glLoadIdentity();
    glTranslated(view_.viewportMatrix()[2] - maxTextWidth - overlaySpacing,
                 view_.viewportMatrix()[3] - overlayTextSize - overlaySpacing, 0);

    // Loop over legend entries
    GLfloat colour[4];
    for (auto &rend : renderables_)
    {
        if (!rend->isVisible())
            continue;

        // Grab copy of the relevant colour definition for this Renderable
        const auto &colourDefinition = rend->colour();

        // Draw line indicator
        glPushMatrix();
        glTranslated(-overlaySpacing, (overlayTextSize / 2.0) - (rend->lineStyle().width() / 2.0), 0.0);
        // -- What are we drawing for the line indicator?
        if (colourDefinition.style() == ColourDefinition::SingleColourStyle)
        {
            rend->lineStyle().sendToGL(pixelScaling_);
            GLfloat lineWidth;
            glGetFloatv(GL_LINE_WIDTH, &lineWidth);
            glLineWidth(lineWidth * 2.0);
            colourDefinition.colour(0.0, colour);
            glColor4f(colour[0], colour[1], colour[2], colour[3]);
            glBegin(GL_LINES);
            glVertex2i(0.0, 0.0);
            glVertex2i(-legendLineLength, 0.0);
            glEnd();
        }
        glPopMatrix();

        // Draw text
        glPushMatrix();
        glColor3d(0.0, 0.0, 0.0);
        glScaled(overlayTextSize, overlayTextSize, overlayTextSize);
        fontInstance_.renderText(std::string(rend->name()));
        glPopMatrix();

        // Shift to next position
        glTranslated(0.0, -(overlayTextSize + overlaySpacing), 0.0);
    }

    /*
     * Draw interaction mode embellishments
     */
    glLoadIdentity();

    static LineStyle selectionBoxStyle(1.0, LineStipple::HalfDashStipple);

    if (interacting())
        switch (interactionMode())
        {
            case (DataViewer::InteractionMode::ZoomToArea):
                // Draw dashed box indicating selection area, form clicked to current mouse coordinates
                selectionBoxStyle.sendToGL();
                glBegin(GL_LINE_LOOP);
                glVertex2d(rMouseDown_.x, rMouseDown_.y);
                glVertex2d(rMouseLast_.x, rMouseDown_.y);
                glVertex2d(rMouseLast_.x, rMouseLast_.y);
                glVertex2d(rMouseDown_.x, rMouseLast_.y);
                glEnd();
                break;
            default:
                break;
        }
}
