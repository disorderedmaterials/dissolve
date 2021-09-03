#include <algorithm>
#include <vector>

template <typename T> class PackedIterator
{
    private:
    std::vector<T> &values_;
    std::vector<unsigned int> &indices_;
    int index_;

    public:
    PackedIterator(std::vector<T> &values, std::vector<unsigned int> &indices, unsigned int index = 0)
	: values_(values), indices_(indices), index_(index)
    {
    }

    T &operator*() { return values_[indices_[index_]]; }
    PackedIterator<T> operator++()
    {
	index_++;
	return *this;
    }
    PackedIterator<T> operator--()
    {
	index_--;
	return *this;
    }
    bool operator==(const PackedIterator<T> &other) { return index_ == other.index_; }
};

template <typename T> class PackedRange
{
    private:
    std::vector<T> &values_;
    std::vector<unsigned int> &indices_;

    public:
    PackedRange(std::vector<T> &values, std::vector<unsigned int> &indices) : values_(values), indices_(indices) {}
    PackedIterator<T> begin() { return PackedIterator<T>(values_, indices_, 0); }
    PackedIterator<T> end() { return PackedIterator<T>(values_, indices_, indices_.size()); }
};
