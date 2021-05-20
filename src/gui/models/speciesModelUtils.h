#include "classes/speciesintra.h"
#include <QString>
#include <iostream>
#include <numeric>
#include <vector>

// Join the parameters into a comma separated string
template <typename Iterator> std::string joinStrings(Iterator begin, Iterator end, std::string delim = ", ")
{
    if (begin == end)
	return std::string();
    return std::accumulate(std::next(begin), end, fmt::format("{}", *begin),
			   [&delim](const auto acc, const auto value) { return fmt::format("{}{}{}", acc, delim, value); });
}

// Join the parameters into a comma separated string
template <class Class> std::string joinStrings(Class range, std::string delim = ", ")
{
    return joinStrings(range.begin(), range.end(), delim);
}

// Split a string over a delimiter and store the results in an iterator.  Returns the number of elements split, or -1 if the
// number of elements exceeds the givens size of the container.
template <class T> int splitString(std::string str, T iterator, int size, std::string delim = " ")
{
    int found = 0, index = 0, count = 0;
    while (true)
    {
	found = str.find(delim, index);
	count += 1;
	if (count > size)
	    return -1;
	if (found == std::string::npos)
	{
	    *iterator++ = str.substr(index);
	    return count;
	}
	*iterator++ = str.substr(index, found - index);
	index = found + delim.size();
    }
}

// Populate the parameters of a SpeciesIntra with a comma separated
// string of values
bool splitParameters(const QString &params, SpeciesIntra &destination);
