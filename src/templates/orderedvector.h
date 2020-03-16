/*
	*** OrderedVector Definition
	*** src/templates/orderedvector.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_ORDEREDVECTOR_H
#define DISSOLVE_ORDEREDVECTOR_H

#include <algorithm>
#include <functional>
#include <vector>

template <class T> class ordered_vector : std::vector<T> {
  private:
  std::function<bool(T,T)> compare_ = std::less<T>();
  using std::vector<T>::back;
  using std::vector<T>::emplace_back;
  using std::vector<T>::erase;
  using std::vector<T>::insert;
  using std::vector<T>::pop_back;
  using std::vector<T>::push_back;

  public:
  using std::vector<T>::begin;
  using std::vector<T>::clear;
  using std::vector<T>::end;
  using std::vector<T>::size;
  ordered_vector() : std::vector<T>() {};
  ordered_vector(std::function<bool(T,T)> comp) : compare_(comp) {};
  void insert(T& value) {
      auto location = std::lower_bound(begin(), end(), value, compare_);
      if(location != end() && *location == value) return;
      insert(location, value);
  }

  template <class... Args> void emplace(Args&&... args) {
    emplace_back(args...);
    T& item = back();
    pop_back();
    insert(item);
  }

  bool erase(T& value) {
    auto location = std::lower_bound(begin(), end(), value, compare_);
    if (location == end() || *location != value)
      return false;
    erase(location);
    return true;
  }
};

#endif
