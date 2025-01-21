// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

/*

  The Phantom type is a compile time marker of zero size that allows
  the passing of type info to a function.  For example, you could have
  the `getBits` function with overloads.

  >>>
    int getBits(Phantom<char> phantom) { return 8; }
    int getBits(Phantom<double> phantom) { return 64; }
  <<<

  Now, there's two other ways that this could have been written

  >>>
    int getBits(char phantom) { return 8; }
    int getBits(double phantom) { return 64; }
  <<<

  While this works well for our trivial example, imagine that we had a
  type with an expensive constructor.  If the information is purely
  related to the type, there's no need to instantiate a copy of it to
  determine what we already know.

  The other solution would be to use a function template

  >>>
    template <typename T> int getBits();
    template <> int getBits<char> { return 8; }
    template <> int getBits<double> { return 64; }
  <<<

  This works up until the moment that we have a type for which
  `getBits` is undefined.  We would not implement the template
  overload for that type, but attempts to call that function would
  return an odd linker error.  After all, we *have* provided a
  function header that declares that `getBits` is defined for *all*
  types T.

  Using Phantom types, we can still get the overloads at compile time,
  but we're only producing headers for explicitly the types that we
  want to support.

 */
template <typename T> struct Phantom
{
};
