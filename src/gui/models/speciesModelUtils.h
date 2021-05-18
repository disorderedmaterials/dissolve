#include <QAbstractTableModel>
#include <algorithm>

template <class T> QString joinParameters(const T &source)
{
    if (source.parameters().empty())
	return "";
    std::string result = std::to_string(source.parameters().front());
    std::for_each(std::next(source.parameters().begin()), source.parameters().end(),
		  [&result](const auto value) { result += "," + std::to_string(value); });
    return QString::fromStdString(result);
}

template <class T> bool splitParameters(const QString &params, T &destination)
{
    if (destination.masterParameters())
	return false;
    auto terms = params.split(",");
    if (terms.size() != destination.parameters().size())
	return false;
    for (int i = 0; i < terms.size(); ++i)
	destination.setParameter(i, terms[i].toDouble());
    return true;
}
