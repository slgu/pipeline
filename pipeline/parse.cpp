//
//  parse.cpp
//  cg
//
//  Created by GuShenlong on 2/15/16.
//  Copyright © 2016 slgu. All rights reserved.
//
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
using namespace std;
#include "parse.h"
#include <stdlib.h>
//read float from input stream
float getTokenAsFloat (string inString, int whichToken)
{
    
    float thisFloatVal = 0.;    // the return value
    
    if (whichToken == 0) {
        cerr << "error: the first token on a line is a character!" << endl;
        exit (-1);
    }
    
    // c++ string class has no super-easy way to tokenize, let's use c's:
    char *cstr = new char [inString.size () + 1];
    
    strcpy (cstr, inString.c_str());
    
    char *p = strtok (cstr, " ");
    if (p == 0) {
        cerr << "error: the line has nothing on it!" << endl;
        exit (-1);
    }
    
    for (int i = 0; i < whichToken; i++) {
        p = strtok (0, " ");
        if (p == 0) {
            cerr << "error: the line is not long enough for your token request!" << endl;
            exit (-1);
        }
    }
    
    thisFloatVal = atof (p);
    
    delete[] cstr;
    
    return thisFloatVal;
}

void Parser::parse_obj_file (const char * fillnam, std::vector< int > &tris, std::vector< float > &verts) {
    // clear out the tris and verts vectors:
    tris.clear ();
    verts.clear ();
    ifstream in(fillnam);
    char buffer[1025];
    string cmd;
    for (int line=1; in.good(); line++) {
        in.getline(buffer,1024);
        buffer[in.gcount()]=0;
        cmd="";
        istringstream iss (buffer);
        
        iss >> cmd;
        
        if (cmd[0]=='#' or cmd.empty()) {
            // ignore comments or blank lines
            continue;
        }
        else if (cmd=="v") {
            // got a vertex:
            
            // read in the parameters:
            double pa, pb, pc;
            iss >> pa >> pb >> pc;
            
            verts.push_back (pa);
            verts.push_back (pb);
            verts.push_back (pc);
        }
        else if (cmd=="f") {
            // got a face (triangle)
            
            // read in the parameters:
            int i, j, k;
            iss >> i >> j >> k;
            
            // vertex numbers in OBJ files start with 1, but in C++ array
            // indices start with 0, so we're shifting everything down by
            // 1
            tris.push_back (i-1);
            tris.push_back (j-1);
            tris.push_back (k-1);
        }
        else {
            std::cerr << "Parser error: invalid command at line " << line << std::endl;
        }
        
    }
    in.close();
}

void Parser::parse_bazier_surface(const char * filename, std::vector < std::vector < std::vector <vec3> > > & control_point) {
    FILE * file_handle;
    file_handle = fopen (filename,"r");
    int nsurface;
    fscanf(file_handle, "%d", &nsurface);
    control_point.clear();
    for (int i = 0; i < nsurface; ++i) {
        int n, m;
        fscanf(file_handle, "%d %d", &n, &m);
        std::vector < std::vector <vec3> > surface_ctrl_p;
        for (int j = 0; j <= m; ++j) {
            std::vector <vec3> tmp;
            for (int k = 0; k <= n; ++k) {
                double x, y, z;
                fscanf(file_handle, "%lf %lf %lf", &x, &y, &z);
                //std::cout << x << " " << y << " " << z << std::endl;
                tmp.push_back(vec3(x, y, z));
            }
            surface_ctrl_p.push_back(tmp);
        }
        control_point.push_back(surface_ctrl_p);
    }
    fclose(file_handle);
}
