// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

class ArrayIndex2D
{
    public:
    ArrayIndex2D(Array2D<double> inArray)
    {
        nX_ = inArray.nRows();
        nY_ = inArray.nColumns();
        index_ = 0;
    }

    ArrayIndex2D(Array2D<double> inArray, std::tuple<int, int> position)
    {
        nX_ = inArray.nRows();
        nY_ = inArray.nColumns();
        index_ = std::get<0>(position) * nY_ + std::get<1>(position);
    }

    ArrayIndex2D(int nX, int nY)
    {
        nX_ = nX;
        nY_ = nY;
        index_ = 0;
    }

    ArrayIndex2D(int nX, int nY, std::tuple<int, int> position)
    {
        nX_ = nX;
        nY_ = nY;
        index_ = std::get<0>(position) * nY_ + std::get<1>(position);
    }

    ArrayIndex2D &operator++()
    {
        ++index_;
        return *this;
    }
    ArrayIndex2D operator++(int)
    {
        ArrayIndex2D temp = *this;
        ++*this;
        return temp;
    }
    ArrayIndex2D &operator--()
    {
        --index_;
        return *this;
    }
    ArrayIndex2D operator--(int)
    {
        ArrayIndex2D temp = *this;
        --*this;
        return temp;
    }
    ArrayIndex2D &operator+=(int value)
    {
        index_ += value;
        return *this;
    }
    ArrayIndex2D &operator-=(int value)
    {
        index_ -= value;
        return *this;
    }

    bool operator==(const ArrayIndex2D &rhs)
    {
        if (this->index_ == rhs.index_ && this->nX_ == rhs.nX_ && this->nY_ == rhs.nY_)
            return true;
        else
            return false;
    }

    bool operator!=(const ArrayIndex2D &rhs) { return !(*this == rhs); }

    std::tuple<int, int> operator*() { return {index_ / nY_, index_ % nY_}; }

    ArrayIndex2D begin() { return ArrayIndex2D(nX_, nY_); }

    ArrayIndex2D end() { return ArrayIndex2D(nX_, nY_, {nX_, 0}); }

    private:
    int nX_;
    int nY_;
    int index_;
};
