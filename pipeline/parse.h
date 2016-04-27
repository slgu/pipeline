//
//  parse.h
//  cg
//
//  Created by GuShenlong on 2/15/16.
//  Copyright © 2016 slgu. All rights reserved.
//

#ifndef parse_h
#define parse_h
#include <vector>
#include "amath.h"
class Parser {
public:
    void parse_obj_file (const char * fillnam, std::vector< int > &tris, std::vector< float > &verts);
    void parse_bazier_surface(const char * filename, std::vector < std::vector < std::vector <vec3> > > & control_point);
};

#endif /* parse_h */
