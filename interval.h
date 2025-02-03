#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

class interval {
    public:
        float min, max;

        interval() : min(+infinity), max(-infinity) {}

        interval(float min, float max) : min(min), max(max) {}

        float size() const {
            return max - min;
        }

        bool contains(float x) {
            return min <= x && x <= max;
        }

        bool surrounds(float x) {
            return min < x && x < max;
        }

        float clamp(float x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif