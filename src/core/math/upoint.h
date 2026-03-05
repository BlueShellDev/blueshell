#pragma once
#include <vector>
#include "core/types.h"

namespace Core
{
    struct UPoint
    {
        u32 x{0}, y{0};

        UPoint() = default;
        UPoint(u32 x, u32 y) : x(x), y(y) {};

        UPoint operator+(const UPoint &other) const
        {
            return UPoint(this->x + other.x, this->y + other.y);
        };

        UPoint operator-(const UPoint &other) const
        {
            return UPoint(this->x - other.x, this->y - other.y);
        };

        UPoint operator*(const u32 &scalar) const
        {
            return UPoint(this->x * scalar, this->y * scalar);
        };

        UPoint operator/(const u32 &scalar) const
        {
            return UPoint(this->x / scalar, this->y / scalar);
        };

        UPoint &operator+=(const UPoint &other)
        {
            this->x += other.x;
            this->y += other.y;
            return *this;
        };

        UPoint &operator-=(const UPoint &other)
        {
            this->x -= other.x;
            this->y -= other.y;
            return *this;
        };

        UPoint &operator*=(const u32 &scalar)
        {
            this->x *= scalar;
            this->y *= scalar;
            return *this;
        };

        UPoint &operator/=(const u32 &scalar)
        {
            this->x /= scalar;
            this->y /= scalar;
            return *this;
        };
    };
}