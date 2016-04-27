// Very simple display triangle program, that allows you to rotate the
// triangle around the Y axis.
//
// This program does NOT use a vertex shader to define the vertex colors.
// Instead, it computes the colors in the display callback (using Blinn/Phong)
// and passes those color values, one per vertex, to the vertex shader, which
// passes them directly to the fragment shader. This achieves what is called
// "gouraud shading".

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#endif


#include "amath.h"
#include "parse.h"
#include "camera.h"
#include "config.h"
#include "BazierSurf.hpp"
std::string shader_dir = "/Users/slgu1/Dropbox/graduate_courses@CU/cg/pipeline/pipeline";
typedef amath::vec4  point4;
typedef amath::vec4  color4;

float ytheta = 0.0;  // rotation around the Y (up) axis
float xtheta = 0.0; //rotation arround the X
float posx = 0.0;   // translation along X
float posy = 0.0;   // translation along Y

GLuint buffers[2];
GLint matrix_loc;


// viewer's position, for lighting calculations
vec4 viewer = vec4(0, 0, 3, 1);
//view trans
mat4 viewtrans;
//orth trans
mat4 orthtrans;
int detail_level = INIT_LEVEL;


//all vectices
std::vector <point4> vertices;

//all tris
std::vector <int> tris;
Camera camera;
//per vertex per norm
std::vector <vec4> norms;

//per tri-vertex per norm and point;
std::vector <point4> points;
std::vector <vec4> tri_norms;

three_vec4 pre_norms;
three_vec4 pre_points;

// a transformation matrix, for the rotation, which we will apply to every
// vertex:
mat4 ctm;

GLuint program; //shaders


// product of components, which we will use for shading calculations:
vec4 product(vec4 a, vec4 b)
{
    return vec4(a[0]*b[0], a[1]*b[1], a[2]*b[2], a[3]*b[3]);
}

GLuint loc, loc2, loc3, loc4, loc5, loc6, loc7, loc8, loc9, loc10;

// transform the triangle's vertex data and put it into the points array.
// also, compute the lighting at each vertex, and put that into the colors
// array.


void update_viewer() {
    vec4 viewdir = normalize(-viewer);
    vec4 tmp = normalize(cross(viewdir, vec4(0, 1, 0, 0)));
    vec4 up = normalize(cross(tmp, viewdir));
    viewtrans = LookAt(viewer, vec4(0, 0, 0, 1), vec4(0, 1, 0, 0));
    glUniformMatrix4fv(loc4, 1, GL_TRUE, viewtrans);
}

// initialization: set up a Vertex Array Object (VAO) and then
void init_shader()
{
    
    program = InitShader((shader_dir + "/vshader_passthrough.glsl").c_str(), (shader_dir + "/fshader_passthrough.glsl").c_str());
    // ...and set them to be active
    glUseProgram(program);
}
void set_buffer() {
    // create a vertex array object - this defines mameory that is stored
    // directly on the GPU
    GLuint vao;
    
    // deending on which version of the mac OS you have, you may have to do this:
#ifdef __APPLE__
    glGenVertexArraysAPPLE( 1, &vao );  // give us 1 VAO:
    glBindVertexArrayAPPLE( vao );      // make it active
#else
    glGenVertexArrays( 1, &vao );   // give us 1 VAO:
    glBindVertexArray( vao );       // make it active
#endif
    
    // set up vertex buffer object - this will be memory on the GPU where
    // we are going to store our vertex data (that is currently in the "points"
    // array)
    glGenBuffers(1, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);  // make it active
    // specify that its part of a VAO, what its size is, and where the
    // data is located, and finally a "hint" about how we are going to use
    // the data (the driver will put it in a good memory location, hopefully)
    
    //send points and tri_norms and ctm first to test
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(point4) +
                 tri_norms.size() * sizeof(tri_norms),
                 NULL, GL_STATIC_DRAW);
    
}
void set_mesh_vao() {
    // this time, we are sending TWO attributes through: the position of each
    // transformed vertex, and the color we have calculated in tri().
    loc = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc);
    // the vPosition attribute is a series of 4-vecs of floats, starting at the
    // beginning of the buffer
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    loc2 = glGetAttribLocation(program, "vNorm");
    glEnableVertexAttribArray(loc2);
    // the vColor attribute is a series of 4-vecs of floats, starting just after
    // the points in the buffer
    glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(point4)));
}
void update_buffer() {
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(point4) +
                 tri_norms.size() * sizeof(tri_norms),
                 NULL, GL_STATIC_DRAW);
    set_mesh_vao();
}
void set_pos_change_vao() {
    //the trans matrix
    loc3 = glGetUniformLocation(program, "vctm");
    glUniformMatrix4fv(loc3, 1, GL_TRUE, ctm);
    //set viewr
    viewer = camera.toVec();
    vec4 viewdir = normalize(-viewer);
    vec4 tmp = normalize(cross(viewdir, vec4(0, 1, 0, 0)));
    vec4 up = normalize(cross(tmp, viewdir));
    viewtrans = LookAt(viewer, vec4(0, 0, 0, 0), up);
    loc4 = glGetUniformLocation(program, "viewtrans");
    glUniformMatrix4fv(loc4, 1, GL_TRUE, viewtrans);
    orthtrans = Perspective(40, 1, 1, 50);
    loc5 = glGetUniformLocation(program, "orthtrans");
    glUniformMatrix4fv(loc5, 1, GL_TRUE, orthtrans);
    loc6 = glGetUniformLocation(program, "viewer");
    glUniform4fv(loc6, 1, viewer);
    glClearColor(1.0, 1.0, 1.0, 1.0); 
}
void init() {
    set_buffer();
    init_shader();
    set_mesh_vao();
    set_pos_change_vao();
}


void set_pos_change() {
}


void display( void )
{
    // clear the window (with white) and clear the z-buffer (which isn't used
    // for this example).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    
    // based on where the mouse has moved to, construct a transformation matrix:
    ctm = Translate(posx*.01,posy*.01,0.)* RotateY(ytheta) * RotateX(xtheta);

    // now build transform all the vertices and put them in the points array,
    // and their colors in the colors array:
    
    // tell the VBO to get the data from the points arrat and the colors array:
    glBufferSubData( GL_ARRAY_BUFFER, 0, points.size() * sizeof(point4), &points[0]);
    glBufferSubData( GL_ARRAY_BUFFER, points.size() * sizeof(point4), tri_norms.size() * sizeof(tri_norms), &tri_norms[0]);
    //send trans matrix to the buffer
    
    // draw the VAO:
    glDrawArrays(GL_TRIANGLES, 0, points.size());
    
    
    // move the buffer we drew into to the screen, and give us access to the one
    // that was there before:
    glutSwapBuffers();
}


// use this motionfunc to demonstrate rotation - it adjusts "theta" based
// on how the mouse has moved. Theta is then used the the display callback
// to generate the transformation, ctm, that is applied
// to all the vertices before they are displayed:
void mouse_move_rotate (int x, int y)
{
    static int lastx = 0;// keep track of where the mouse was last:
    static int lasty = 0;//keep track of y
    int amntX = x - lastx; 
    if (amntX != 0) {
        ytheta +=  amntX;
        if (ytheta > 360.0 ) ytheta -= 360.0;
        if (ytheta < 0.0 ) ytheta += 360.0;
        
        lastx = x;
    }
    int amntY = y - lasty;
    if (amntY != 0) {
        xtheta +=  amntY;
        if (xtheta > 360.0 ) xtheta -= 360.0;
        if (xtheta < 0.0 ) xtheta += 360.0;
        
        lasty = y;
    }
    glUniformMatrix4fv(loc3, 1, GL_TRUE, ctm);
    // force the display routine to be called as soon as possible:
    glutPostRedisplay();
}


// use this motionfunc to demonstrate translation - it adjusts posx and
// posy based on the mouse movement. posx and posy are then used in the
// display callback to generate the transformation, ctm, that is applied
// to all the vertices before they are displayed:
void mouse_move_translate (int x, int y)
{
    static int lastx = 0;
    static int lasty = 0;  // keep track of where the mouse was last:
    camera.lon -= (x - lastx);
    camera.lat += (y - lasty);
    if (camera.lat >= 85)
        camera.lat = 85;
    if (camera.lat <= -85)
        camera.lat = -85;
    viewer = camera.toVec();
    glUniform4fv(loc6, 1, viewer);
    update_viewer();
    lastx = x;
    lasty = y;
    // force the display routine to be called as soon as possible:
    glutPostRedisplay();
}

void bazier_to_mesh(int detail) {
    //return if detail is too low or too high
    if (detail > MAX_LEVEL)
        return;
    if (detail < INIT_LEVEL)
        return;
    int coe = MAX_LEVEL / detail;
    points.clear();
    tri_norms.clear();
    int num_of_surs = pre_points.size();
    for (int i = 0; i < num_of_surs; ++i) {
        //fill the triangle
        for (int k = 0; k < detail; ++k)
            for (int j = 0; j < detail; ++j) {
                //add left triangle
                int x[3], y[3];
                x[0] = k;
                y[0] = j;
                x[1] = x[0] + 1;
                y[1] = y[0];
                x[2] = x[1];
                y[2] = y[1] + 1;
                for(int o = 0; o < 3; ++o)
                    points.push_back(pre_points[i][x[o] * coe][y[o] * coe]);
                for(int o = 0; o < 3; ++o)
                    //push tri norms
                    tri_norms.push_back(pre_norms[i][x[o] * coe][y[o] * coe]);
                //add right triangle
                x[1] = x[0] + 1;
                y[1] = y[0] + 1;
                x[2] = x[0];
                y[2] = y[0] + 1;
                for(int o = 0; o < 3; ++o)
                    points.push_back(pre_points[i][x[o] * coe][y[o] * coe]);
                for(int o = 0; o < 3; ++o)
                    //push tri norms
                    tri_norms.push_back(pre_norms[i][x[o] * coe][y[o] * coe]);
            }
    }
}

// the keyboard callback, called whenever the user types something with the
// regular keys.
void mykey(unsigned char key, int mousex, int mousey)
{
    if(key=='q'||key=='Q') exit(0);
    
    // and r resets the view:
    if (key =='r') {
        posx = 0;
        posy = 0;
        xtheta = 0;
        ytheta = 0;
        glutPostRedisplay();
    }
    else if (key == 'x') {
        if (camera.r >= 50)
            return;
        camera.r += 1;
        viewer = camera.toVec();
        glUniform4fv(loc6, 1, viewer);
        update_viewer();
        glutPostRedisplay();
    }
    else if (key == 'z') {
        if (camera.r <= 2)
            return;
        camera.r -= 1;
        viewer = camera.toVec();
        glUniform4fv(loc6, 1, viewer);
        update_viewer();
        glutPostRedisplay();
    }
    else if(key == '>') {
        if (detail_level == INIT_LEVEL)
            return;
        detail_level /= 2;
        bazier_to_mesh(detail_level);
        update_buffer();
        glutPostRedisplay();
    }
    else if(key == '<') {
        if (detail_level == MAX_LEVEL)
            return;
        detail_level *= 2;
        bazier_to_mesh(detail_level);
        update_buffer();
        glutPostRedisplay();
    }
}
//use verts and tris to change to shader object
void obj_to_mesh(std::vector <int> & tris, std::vector <float> & verts) {
    //parser.parse_obj_file("/Users/slgu1/Desktop/kitten.obj", tris, verts);
    //push verts
    vertices.clear();
    norms.clear();
    tri_norms.clear();
    points.clear();
    for (int i = 0; i < verts.size(); i += 3) {
        vertices.push_back(point4(verts[i], verts[i + 1], verts[i + 2], 1));
    }
    
    norms.resize(vertices.size());
    for (int i = 0; i < vertices.size(); ++i) {
        norms[i] = vec4(0, 0, 0, 0);
    }
    //set tris and verts to vertices
    for (int i = 0;  i < tris.size(); i += 3) {
        int idxa = tris[i];
        int idxb = tris[i + 1];
        int idxc = tris[i + 2];
        //cal norm
        vec3 n = normalize(cross(vertices[idxb] - vertices[idxa],
                                 vertices[idxc] - vertices[idxb]));
        norms[idxa] += vec4(n,0);
        norms[idxb] += vec4(n,0);
        norms[idxc] += vec4(n,0);
    }
    //normalize
    for (int i = 0; i < vertices.size(); ++i) {
        norms[i] = normalize(norms[i]);
    }
    //set tri-points
    for (int i = 0;  i < tris.size(); i += 3) {
        int idxa = tris[i];
        int idxb = tris[i + 1];
        int idxc = tris[i + 2];
        points.push_back(vertices[idxa]);
        points.push_back(vertices[idxb]);
        points.push_back(vertices[idxc]);
        tri_norms.push_back(norms[idxa]);
        tri_norms.push_back(norms[idxb]);
        tri_norms.push_back(norms[idxc]);
    }
}
void pre_bazier_to_mesh(std::vector<std::vector<std::vector<vec3> > > & control_point) {
    pre_points.clear();
    pre_norms.clear();
    int num_of_surs = int(control_point.size());
    for (int i = 0; i < num_of_surs; ++i) {
        int m = int(control_point[i][0].size());
        int n = int(control_point[i].size());
        --n;
        --m;
        BazierSurf surf;
        surf.init(n, m, control_point[i]);
        two_vec4 pre_point;
        two_vec4 pre_norm;
        for (int j = 0; j <= MAX_LEVEL; ++j) {
            one_vec4 tmp_point;
            one_vec4 tmp_norm;
            for (int k = 0; k <= MAX_LEVEL; ++k) {
                tmp_norm.push_back(surf.gen_norm(j * 1.0 / MAX_LEVEL, k * 1.0 / MAX_LEVEL));
                tmp_point.push_back(surf.gen_point(j * 1.0 / MAX_LEVEL, k * 1.0 / MAX_LEVEL));
            }
            pre_point.push_back(tmp_point);
            pre_norm.push_back(tmp_norm);
        }
        pre_norms.push_back(pre_norm);
        pre_points.push_back(pre_point);
    }
}

int main(int argc, char** argv)
{
    
    //read obj file
    Parser parser;
    /*
    std::vector <int> tris;
    std::vector <float> verts;
    if (argc != 2) {
        std::cout << "usage: ./glrender filename.obj" << std::endl;
        return 1;
    }
     */
    if (argc != 2) {
        std::cout << "usage: ./glrender bazier_filename" << std::endl;
        return 1;
    }
    std::vector<std::vector<std::vector<vec3> > > control_point;
    parser.parse_bazier_surface(argv[1], control_point);

    //parse bezier
    pre_bazier_to_mesh(control_point);
    bazier_to_mesh(detail_level);
    /*
    parser.parse_obj_file(argv[1], tris, verts);
    obj_to_mesh(tris, verts);
    */
    
    // initialize glut, and set the display modes
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    
    // give us a window in which to display, and set its title:
    glutInitWindowSize(512, 512);
    glutCreateWindow("Rotate / Translate Triangle");
    
    // for displaying things, here is the callback specification:
    glutDisplayFunc(display);
    
    // when the mouse is moved, call this function!
    // you can change this to mouse_move_translate to see how it works
    //glutMotionFunc(mouse_move_rotate);
    glutMotionFunc(mouse_move_translate);
    
    // for any keyboard activity, here is the callback:
    glutKeyboardFunc(mykey);
    
#ifndef __APPLE__
    // initialize the extension manager: sometimes needed, sometimes not!
    glewInit();
#endif

    // call the init() function, defined above:
    init();
    
    // enable the z-buffer for hidden surface removel:
    glEnable(GL_DEPTH_TEST);

    // once we call this, we no longer have control except through the callbacks:
    glutMainLoop();
    return 0;
}
