/*
    *** Line Stipple
    *** src/gui/render/linestipple.cpp
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

#include "gui/render/linestipple.h"
#include "base/sysfunc.h"
#include <QComboBox>
#include <QPainter>
#include <QVector>
#include <bitset>
#include <stdio.h>

// Static list of LineStipples
LineStipple LineStipple::stipple[] = {{1, 0xffff, "Solid"},       {1, 0xaaaa, "Dots"},         {1, 0xcccc, "Fine Dash"},
                                      {3, 0xaaaa, "Eighth Dash"}, {1, 0xf0f0, "Quarter Dash"}, {1, 0xff00, "Half Dash"},
                                      {1, 0x6f6f, "Dot Dash 1"}};

// Convert text string to StippleType
LineStipple::StippleType LineStipple::stippleType(std::string_view s)
{
    for (int n = 0; n < LineStipple::nStippleTypes; ++n)
        if (DissolveSys::sameString(s, LineStipple::stipple[n].name))
            return (LineStipple::StippleType)n;
    return LineStipple::nStippleTypes;
}

// Convert StippleType to text string
std::string_view LineStipple::stippleType(LineStipple::StippleType st) { return LineStipple::stipple[st].name; }

/*
 * Stipple
 */

// Add stipple pattern to specified QComboBox
void LineStipple::addStippleItem(QComboBox *combo, int lineHeight)
{
    auto lineWidth = combo->width() - 8;
    QLine line(0, lineHeight / 2, lineWidth, lineHeight / 2);
    QPalette palette = combo->palette();
    QPen pen;
    pen.setWidth(lineHeight);
    pen.setCapStyle(Qt::FlatCap);
    combo->setIconSize(QSize(lineWidth, lineHeight));

    // Create an icon with the stippled line on it
    QPixmap lineImage(lineWidth, lineHeight);
    QPainter painter(&lineImage);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, false);
    painter.setBackground(QBrush(Qt::white));
    painter.fillRect(0, 0, lineWidth, lineHeight, QBrush(palette.window()));
    pen.setDashPattern(dashPattern());
    painter.setPen(pen);
    painter.drawLine(line);
    painter.end();
    combo->addItem(QIcon(lineImage), QString::fromStdString(name));
}

// Return stipple pattern as a Qt-compatible dash pattern
QVector<qreal> &LineStipple::dashPattern()
{
    static QVector<qreal> pattern;
    pattern.clear();

    // Look at each of the first 16 bits of the stipple in turn...
    // 	char test[17];
    // 	test[16] = '\0';
    auto consecutive = 0, last = -1, nEntries = 0;
    int bit;
    for (int n = 15; n >= 0; --n)
    {
        bit = (stipplePattern & (1 << n) ? 1 : 0);
        // 		test[15-n] = (bit ? '1' : '0');

        // If this bit is the same as the last, then increase the 'run'
        if (bit == last)
            ++consecutive;
        else if (last == -1)
        {
            last = bit;
            consecutive = 1;
        }
        else
        {
            // Special case if nEntries = 0, since if the first run is a space (0) we must skip the first dash...
            if ((nEntries == 0) && (last == 0))
                pattern << 0;

            // Add next run integer
            pattern << consecutive * stippleFactor;

            // Reset counter and flag
            last = bit;
            consecutive = 1;
            ++nEntries;
        }
    }
    // Add on the last bit that we have....
    pattern << consecutive * stippleFactor;

    // Ensure that we have an even number of entries in the vector...
    if (pattern.size() % 2 == 1)
        pattern << 0;

    return pattern;
}

/*
 * GL
 */

// Apply stipple pattern
void LineStipple::apply() { glLineStipple(stippleFactor, stipplePattern); }
