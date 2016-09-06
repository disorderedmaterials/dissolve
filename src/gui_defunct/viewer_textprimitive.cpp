/*
	*** Text Primitive
	*** src/gui/viewer_textprimitive.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer_textprimitive.h"
#include "gui/viewer.uih"

/*
// Text Primitive
*/

// Set data
void TextPrimitive::set(Vec3<double> pos, QString text, bool rightAlign)
{
	position_ = pos;
	rightAlign_ = rightAlign;
	text_ = text;
}

// Return position
Vec3<double> &TextPrimitive::position()
{
	return position_;
}

// Return text to render
QString &TextPrimitive::text()
{
	return text_;
}

// Return whether to right-align text
bool TextPrimitive::rightAlign()
{
	return rightAlign_;
}

/*
// Text Primitive Chunk
*/

// Constructor
TextPrimitiveChunk::TextPrimitiveChunk()
{
	// Public variables
	prev = NULL;
	next = NULL;

	// Private variable
	nTextPrimitives_ = 0;
}

// Forget all text primitives in list
void TextPrimitiveChunk::forgetAll()
{
	nTextPrimitives_ = 0;
}

// Return whether array is full
bool TextPrimitiveChunk::full()
{
	return (nTextPrimitives_ == TEXTCHUNKSIZE);
}

// Add primitive to chunk
void TextPrimitiveChunk::add(Vec3<double> pos, QString text, bool rightAlign)
{
	textPrimitives_[nTextPrimitives_].set(pos, text, rightAlign);
	++nTextPrimitives_;
}

// Render all primitives in chunk
void TextPrimitiveChunk::renderAll(QPainter &painter, Viewer* viewer, bool isIsotope, bool superScripts)
{
	// Grab contextHeight
	QRect rect;
	int height = viewer->contextHeight();
	int pointSize = viewer->fontSize();
	int superScriptSize = pointSize*viewer->superScriptFraction();
	Vec4<double> v;
	if (Viewer::useNiceText())
	{
		if (isIsotope)
		{
			for (int n=0; n<nTextPrimitives_; ++n)
			{
				v = viewer->modelToScreen(textPrimitives_[n].position());
// 				rect = painter.boundingRect(v.x, height-v.y-pointsize, 1, -1, textPrimitives_[n].rightAlign() ? Qt::AlignRight : Qt::AlignLeft, textPrimitives_[n].text());
				if (superScripts)
				{
					// Assume that first two characters in the text are the isotope number
					rect = painter.boundingRect(v.x, height-v.y-(pointSize-superScriptSize*0.5), 1, -1, Qt::AlignLeft, textPrimitives_[n].text().mid(0,2));
					painter.drawText(rect, Qt::AlignLeft, textPrimitives_[n].text().mid(0,2));
				}
				else
				{
					// Guess where edge of superscript text is...
					rect = painter.boundingRect(v.x, height-v.y-pointSize, 1, -1, Qt::AlignLeft, textPrimitives_[n].text().mid(0,2));
					painter.drawText(rect.left()+rect.width()*viewer->superScriptFraction(), rect.bottom(), textPrimitives_[n].text().mid(2));
				}
			}
		}
		else
		{
			for (int n=0; n<nTextPrimitives_; ++n)
			{
				v = viewer->modelToScreen(textPrimitives_[n].position());
				rect = painter.boundingRect(v.x, height-v.y-pointSize, 1, -1, textPrimitives_[n].rightAlign() ? Qt::AlignRight : Qt::AlignLeft, textPrimitives_[n].text());
				painter.drawText(rect, textPrimitives_[n].rightAlign() ? Qt::AlignRight : Qt::AlignLeft, textPrimitives_[n].text());
			}
		}
	}
	else for (int n=0; n<nTextPrimitives_; ++n)
	{
		v = viewer->modelToScreen(textPrimitives_[n].position());
		viewer->renderText(v.x, height-v.y-pointSize, textPrimitives_[n].text());
	}
}

/*
// Text Primitive List
*/

// Constructor
TextPrimitiveList::TextPrimitiveList()
{
	currentChunk_ = NULL;
}

// Forget all text primitives, but keeping lists intact
void TextPrimitiveList::forgetAll()
{
	for (TextPrimitiveChunk* chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->forgetAll();
	currentChunk_ = textPrimitives_.first();
}

// Set data from literal coordinates and text
void TextPrimitiveList::add(Vec3<double> pos, QString text, bool rightAlign)
{
	if (currentChunk_ == NULL) currentChunk_ = textPrimitives_.add();
	else if (currentChunk_->full()) currentChunk_ = textPrimitives_.add();
	// Add primitive and set data
	currentChunk_->add(pos, text, rightAlign);
}

// Render all primitives in list
void TextPrimitiveList::renderAll(QPainter &painter, Viewer* viewer, bool isSpecial, bool superScripts)
{
	for (TextPrimitiveChunk* chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->renderAll(painter, viewer, isSpecial, superScripts);
}

