//
//  camera.h
//  pipeline
//
//  Created by GuShenlong on 4/20/16.
//  Copyright © 2016 slgu. All rights reserved.
//

#ifndef _camera_h
#define _camera_h
#include "amath.h"
#include <iostream>
#define PI 3.14159265
class Camera {
public:
    float r;
    float lat;
    float lon;
    void init() {
        r = 2;
        lat = -90;
        lon = 0;
    }
    Camera() {
        init();
    }
    vec4 toVec() {
        double x = r * cos(lon*PI/180) * sin(lon*PI/180);
        double y = r * sin(lon*PI/180) * cos(lat*PI/180);
        double z = r * sin(lat*PI/180);
        std::cout << vec4(x, y, z, 0) << std::endl;
        return vec4(x, y, z, 0);
    }
};
#endif /* camera_h */
