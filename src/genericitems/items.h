// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

/*
 * This file should be included if the use of GenericItems is required.
 * It simply includes all other GenericItemContainer specialisations,
 * followed by the templated GenericItemContainer.
 */

#include "genericitems/bool.h"
#include "genericitems/double.h"
#include "genericitems/int.h"
#include "genericitems/stdstring.h"
#include "genericitems/streampos.h"

#include "genericitems/vec3double.h"
#include "genericitems/vec3int.h"

#include "genericitems/array2d_vector_double.h"
#include "genericitems/std_vector.h"

#include "genericitems/array.h"
#include "genericitems/array2d.h"
#include "genericitems/array2dchar.h"
#include "genericitems/array2ddouble.h"
#include "genericitems/array2ddummy.h"
#include "genericitems/arraydouble.h"
#include "genericitems/arraydummy.h"
#include "genericitems/arrayint.h"
#include "genericitems/arrayvec3double.h"
#include "genericitems/arrayvec3int.h"

#include "genericitems/container.h"
