#pragma once
#include <iostream>
#include "vecmat.h"

class InterpolatedColor {
public:
    static int width;
    static int height;

    static void PrepareInterpolation(int w, int h) {
        width = w;
        height = h;
    }

    static unsigned char GetRed(int x, int y, Vector4* vectors) {
        return (unsigned char)InterpolateColorComponent(x, y, vectors, 0);
    }

    static unsigned char GetGreen(int x, int y, Vector4* vectors) {
        return (unsigned char)InterpolateColorComponent(x, y, vectors, 1);
    }

    static unsigned char GetBlue(int x, int y, Vector4* vectors) {
        return (unsigned char)InterpolateColorComponent(x, y, vectors, 2);
    }

private:
    static double InterpolateColorComponent(int x, int y, Vector4* vectors, int component) {
        int x1 = std::max(0, std::min(width - 1, static_cast<int>(floor(x))));
        int x2 = std::max(0, std::min(width - 1, static_cast<int>(ceil(x))));
        int y1 = std::max(0, std::min(height - 1, static_cast<int>(floor(y))));
        int y2 = std::max(0, std::min(height - 1, static_cast<int>(ceil(y))));

        double q11 = GetColorComponent(x1, y1, vectors, component);
        double q21 = GetColorComponent(x2, y1, vectors, component);
        double q12 = GetColorComponent(x1, y2, vectors, component);
        double q22 = GetColorComponent(x2, y2, vectors, component);

        double r1 = (x1 != x2) ? ((x2 - x) / (x2 - x1)) * q11 + ((x - x1) / (x2 - x1)) * q21 : q11;
        double r2 = (x1 != x2) ? ((x2 - x) / (x2 - x1)) * q12 + ((x - x1) / (x2 - x1)) * q22 : q12;
        double p = (y1 != y2) ? ((y2 - y) / (y2 - y1)) * r1 + ((y - y1) / (y2 - y1)) * r2 : r1;

        return p;
    }

    static double GetColorComponent(int x, int y, Vector4* vectors, int component) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return 0;
        }
        int index = y * width + x;
        return vectors[index].color[component];
    }
};
