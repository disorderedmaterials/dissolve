#pragma once
#include <algorithm>
#include <iterator>
#include <vector>

template <typename T> class PackedIterator : public std::iterator<std::random_access_iterator_tag, T>
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
    T &operator[](int offset) { return values_[indices_[index_ + offset]]; }
    PackedIterator<T> operator++(int)
    {
	auto copy = PackedIterator(*this);
	index_++;
	return copy;
    }
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
    PackedIterator<T> operator+=(int offset)
    {
	index_ += offset;
	return *this;
    }
    PackedIterator<T> operator+(const long unsigned int offset) const
    {
	return PackedIterator<T>(values_, indices_, index_ + offset);
    }
    int operator-(const PackedIterator<T> &other) const { return index_ - other.index_; }
    bool operator==(const PackedIterator<T> &other) const { return index_ == other.index_; }
    bool operator<(const PackedIterator<T> &other) const { return index_ < other.index_; }
    bool operator!=(const PackedIterator<T> &other) const { return index_ != other.index_; }
};

template <typename T> class ConstPackedIterator
{
    private:
    const std::vector<T> &values_;
    const std::vector<unsigned int> &indices_;
    int index_;

    public:
    using iterator_category = std::random_access_iterator_tag;
    ConstPackedIterator(const std::vector<T> &values, const std::vector<unsigned int> &indices, unsigned int index = 0)
	: values_(values), indices_(indices), index_(index)
    {
    }

    const T &operator*() { return values_[indices_[index_]]; }
    const T *operator->() { return &values_[indices_[index_]]; }
    ConstPackedIterator<T> operator++(int)
    {
	auto copy = ConstPackedIterator(*this);
	index_++;
	return copy;
    }
    ConstPackedIterator<T> operator++()
    {
	index_++;
	return *this;
    }
    ConstPackedIterator<T> operator--()
    {
	index_--;
	return *this;
    }
    ConstPackedIterator<T> operator+(const long unsigned int offset) const
    {
	return ConstPackedIterator<T>(values_, indices_, index_ + offset);
    }
    int operator-(const ConstPackedIterator<T> &other) const { return index_ - other.index_; }
    bool operator==(const ConstPackedIterator<T> &other) const { return index_ == other.index_; }
    bool operator<(const ConstPackedIterator<T> &other) const { return index_ < other.index_; }
    bool operator!=(const ConstPackedIterator<T> &other) const { return index_ != other.index_; }
};

template <typename T> class PackedRange
{
    private:
    std::vector<T> &values_;
    std::vector<unsigned int> &indices_;

    public:
    PackedRange(std::vector<T> &values, std::vector<unsigned int> &indices) : values_(values), indices_(indices) {}
    PackedIterator<T> begin() const { return PackedIterator<T>(values_, indices_, 0); }
    PackedIterator<T> end() const { return PackedIterator<T>(values_, indices_, indices_.size()); }
    unsigned int size() const { return indices_.size(); }
};

template <typename T> class ConstPackedRange
{
    private:
    const std::vector<T> &values_;
    const std::vector<unsigned int> &indices_;

    public:
    ConstPackedRange(const std::vector<T> &values, const std::vector<unsigned int> &indices)
	: values_(values), indices_(indices)
    {
    }
    ConstPackedIterator<T> begin() const { return ConstPackedIterator<T>(values_, indices_, 0); }
    ConstPackedIterator<T> end() const { return ConstPackedIterator<T>(values_, indices_, indices_.size()); }
    unsigned int size() const { return indices_.size(); }
};
