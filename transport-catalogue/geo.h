#pragma once

#include <cmath>
struct geo
{
    struct Coordinates
    {
        double lat;
        double lng;
        bool operator==(const Coordinates &other) const
        {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates &other) const
        {
            return !(*this == other);
        }
    };

    static double ComputeDistance(Coordinates from, Coordinates to);
};
