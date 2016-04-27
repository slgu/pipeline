//
//  BazierSurf.hpp
//  pipeline
//
//  Created by GuShenlong on 4/26/16.
//  Copyright © 2016 slgu. All rights reserved.
//

#ifndef BazierSurf_hpp
#define BazierSurf_hpp

#include <stdio.h>
#include "amath.h"
#include <vector>
#include <iostream>
#define MAX_FACTOR 10
class BazierSurf {
private:
    //calculate bernstein coe
    int factor[MAX_FACTOR];
    float poww(float t, int times) {
        if (n == 0)
            return 1;
        float res = 1;
        for (int i = 0; i < times; ++i)
            res *= t;
        return res;
    }
    float bernstein(int n, int i, float t) {
        int a = factor[n];
        int b = factor[i];
        int c = factor[n - i];
        float e = poww(t, i);
        float f = poww(1 - t, n - i);
        return (a / b / c) * e * f;
    }
    float bernstein_der(int n, int i, float t) {
        int a = factor[n];
        int b = factor[i];
        int c = factor[n - i];
        int combine_coe = a / b / c;
        if (i == 0)
            return - combine_coe * n * poww(1 - t, n - 1);
        if (i == n)
            return combine_coe * n * poww(t, n - 1);
        return combine_coe * (i * poww(t, i - 1) * pow(1 - t, n - 1) - (n - i) * pow(1 - t, n - i - 1) * pow(t, i));
    }
public:
    //m + 1 n + 1 point
    int n, m;
    //conrol points
    std::vector < std::vector <vec3> > ctrl_p;
    //generate point given u v
    vec4 gen_point(float u, float v) {
        vec3 res(0, 0, 0);
        for (int i = 0; i <= m; ++i)
            for (int j = 0; j <= n; ++j) {
                res += bernstein(m, i, u) * bernstein(n, j, v) * ctrl_p[j][i];
            }
        return vec4(res, 1);
    }
    //generate norm give u v;
    vec4 gen_norm(float u, float v) {
        vec3 der_u(0, 0, 0);
        for (int i = 0; i <= m; ++i)
            for (int j = 0; j <= n; ++j) {
                der_u += bernstein_der(m, i, u) * bernstein(n, j, v) * ctrl_p[j][i];
            }
        vec3 der_v(0, 0, 0);
        for (int i = 0; i <= m; ++i)
            for (int j = 0; j <= n; ++j) {
                der_v += bernstein(m, i, u) * bernstein_der(n, j, v) * ctrl_p[j][i];
            }
        return vec4(normalize(cross(der_u, der_v)), 0);
    }
    void init(int _n, int _m, std::vector < std::vector <vec3> > & _ctrl_p) {
        //init factor
        factor[0] = 1;
        for (int i = 1; i < MAX_FACTOR; ++i)
            factor[i] = i * factor[i - 1];
        //copy
        n = _n;
        m = _m;
        ctrl_p = _ctrl_p;
    }
};
#endif /* BazierSurf_hpp */
