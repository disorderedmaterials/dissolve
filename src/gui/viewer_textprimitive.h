/*
	*** Text Primitive
	*** src/gui/viewer_textprimitive.h
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

#ifndef DUQ_TEXTPRIMITIVE_H
#define DUQ_TEXTPRIMITIVE_H

#include <QtGui/QtGui>
#include "templates/vector3.h"
#include "templates/list.h"

#define TEXTCHUNKSIZE 100

// Forward Declarations
class Viewer;

// Text Primitive
class TextPrimitive
{
	private:
	// Position of text (3D coordinates)
	Vec3<double> position_;
	// Whether to right-align text
	bool rightAlign_;
	// Text to render
	QString text_;
	
	public:
	// Set data
	void set(Vec3<double> pos, QString text, bool rightAlign);
	// Return position
	Vec3<double> &position();
	// Return text to render
	QString &text();
	// Return whether to right-align text
	bool rightAlign();
};

// Text Primitive Chunk
class TextPrimitiveChunk
{
	public:
	// Constructor
	TextPrimitiveChunk();
	// List pointers
	TextPrimitiveChunk* prev, *next;

	private:
	// Array of TextPrimitive
	TextPrimitive textPrimitives_[TEXTCHUNKSIZE];
	// Number of text primitives currently in the array
	int nTextPrimitives_;

	public:
	// Forget all text primitives in list
	void forgetAll();
	// Return whether array is full
	bool full();
	// Add primitive to list
	void add(Vec3<double> pos, QString text, bool rightAlign = false);
	// Render all primitives in chunk
	void renderAll(QPainter &painter, Viewer* viewer, bool isIsotope, bool superScripts);
};

// Text Primitive List
class TextPrimitiveList
{
	public:
	// Constructor
	TextPrimitiveList();

	private:
	// List of text primitive chunks
	List<TextPrimitiveChunk> textPrimitives_;
	// Current TextPrimitiveChunk
	TextPrimitiveChunk* currentChunk_;

	public:
	// Forget all text primitives, but keeping lists intact
	void forgetAll();
	// Add primitive to list
	void add(Vec3<double> pos, QString text, bool rightAlign = false);
	// Render all primitives in list
	void renderAll(QPainter& painter, Viewer* viewer, bool isIsotope = false, bool superScripts = false);
};

#endif
