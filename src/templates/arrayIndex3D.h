// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

class ArrayIndex3D
{

    public:
    ArrayIndex3D(Array3D<double> inArray)
    {
        nX_ = inArray.nX();
        nY_ = inArray.nY();
        nZ_ = inArray.nZ();
        index_ = 0;
    }

    ArrayIndex3D(Array3D<double> inArray, std::tuple<int, int, int> position)
    {
        nX_ = inArray.nX();
        nY_ = inArray.nY();
        nZ_ = inArray.nZ();
        index_ = std::get<0>(position) * nY_ * nZ_ + std::get<1>(position) * nZ_ + std::get<2>(position);
    }

    ArrayIndex3D(int nX, int nY, int nZ)
    {
        nX_ = nX;
        nY_ = nY;
        nZ_ = nZ;
        index_ = 0;
    }

    ArrayIndex3D(int nX, int nY, int nZ, std::tuple<int, int, int> position)
    {
        nX_ = nX;
        nY_ = nY;
        nZ_ = nZ;
        index_ = std::get<0>(position) * nY_ * nZ_ + std::get<1>(position) * nZ_ + std::get<2>(position);
    }

    ArrayIndex3D &operator++()
    {
        ++index_;
        return *this;
    }
    ArrayIndex3D operator++(int)
    {
        ArrayIndex3D temp = *this;
        ++*this;
        return temp;
    }
    ArrayIndex3D &operator--()
    {
        --index_;
        return *this;
    }
    ArrayIndex3D operator--(int)
    {
        ArrayIndex3D temp = *this;
        --*this;
        return temp;
    }
    ArrayIndex3D &operator+=(int value)
    {
        index_ += value;
        return *this;
    }
    ArrayIndex3D &operator-=(int value)
    {
        index_ -= value;
        return *this;
    }

    bool operator==(const ArrayIndex3D &rhs)
    {
        if (this->nX_ == rhs.nX_ && this->nY_ == rhs.nY_ && this->nZ_ == rhs.nZ_ && this->index_ == rhs.index_)
            return true;
        else
            return false;
    }

    bool operator!=(const ArrayIndex3D &rhs) { return !(*this == rhs); }

    std::tuple<int, int, int> operator*() { return {index_ / (nY_ * nZ_), (index_ / nZ_) % nY_, index_ % nZ_}; }

    ArrayIndex3D begin() { return ArrayIndex3D(nX_, nY_, nZ_); }

    ArrayIndex3D end() { return ArrayIndex3D(nX_, nY_, nZ_, {nX_ - 1, nY_, 0}); }

    private:
    int nX_;
    int nY_;
    int nZ_;
    int index_;
};
