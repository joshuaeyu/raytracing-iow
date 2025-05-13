#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

class interval {
    public:
        float min, max;

        interval()
         : min(+infinity), max(-infinity) {}

        interval(float min, float max)
         : min(min), max(max) {}

        interval(const interval& a, const interval& b) {
            min = a.min <= b.min ? a.min : b.min;
            max = a.max >= b.max ? a.max : b.max;
        }

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

        interval expand(float delta) {
            float padding = delta/2;
            return interval(min - padding, max + padding);
        }

        static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

const interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

const interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif