// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

class ArrayIndex2D
{
    public:
    ArrayIndex2D(Array2D<double> inArray)
    {
        nX_ = inArray.nRows();
        nY_ = inArray.nColumns();
        ptr_ = 0;
    }

    ArrayIndex2D(Array2D<double> inArray, std::tuple<int, int> position)
    {
        nX_ = inArray.nRows();
        nY_ = inArray.nColumns();
        ptr_ = nX_ * nY_;
    }

    ArrayIndex2D(int nX, int nY)
    {
        nX_ = nX;
        nY_ = nY;
        ptr_ = 0;
    }

    ArrayIndex2D(int nX, int nY, std::tuple<int, int> position)
    {
        nX_ = nX;
        nY_ = nY;
        ptr_ = nX_ * nY_;
    }

    ArrayIndex2D &operator++()
    {
        ++ptr_;
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
        --ptr_;
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
        ptr_ += value;
        return *this;
    }
    ArrayIndex2D &operator-=(int value)
    {
        ptr_ -= value;
        return *this;
    }

    bool operator==(const ArrayIndex2D &rhs)
    {
        if (*this == rhs)
            return true;
        else
            return false;
    }

    bool operator!=(const ArrayIndex2D &rhs) { return !(*this == rhs); }

    std::tuple<int, int> operator*() { return {ptr_ / nY_, ptr_ % nY_}; }

    ArrayIndex2D begin() { return ArrayIndex2D(nX_, nY_); }

    ArrayIndex2D end() { return ArrayIndex2D(nX_, nY_, {nX_ - 1, nY_ - 1}); }

    private:
    int nX_;
    int nY_;
    int ptr_;
};