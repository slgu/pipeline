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
vec4 viewer = vec4(0, 0, -5,0);
mat4 viewtrans;
mat4 orthtrans;
// light & material definitions, again for lighting calculations:
vec4 light_position = vec4(0, 0, -1 ,0);
color4 light_ambient = color4(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse = color4(1.0, 1.0, 1.0, 1.0);
color4 light_specular = color4(1.0, 1.0, 1.0, 1.0);

color4 material_ambient = color4(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse = color4(1.0, 0.8, 0.0, 1.0);
color4 material_specular = color4(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

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
void tri()
{
    for (int i = 0; i < vertices.size(); i += 3) {
    }
}

void update_viewer() {
    vec4 viewdir = normalize(-viewer);
    vec4 tmp = normalize(cross(viewdir, vec4(0, 1, 0, 0)));
    vec4 up = normalize(cross(tmp, viewdir));
    viewtrans = LookAt(viewer, vec4(0, 0, 0, 0), up);
    std::cout << viewtrans << std::endl;
    glUniformMatrix4fv(loc4, 1, GL_TRUE, viewtrans);
}
// initialization: set up a Vertex Array Object (VAO) and then
void init()
{
    
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
    
    // load in these two shaders...  (note: InitShader is defined in the
    // accompanying initshader.c code).
    // the shaders themselves must be text glsl files in the same directory
    // as we are running this program:
    program = InitShader((shader_dir + "/vshader_passthrough.glsl").c_str(), (shader_dir + "/fshader_passthrough.glsl").c_str());
    // ...and set them to be active
    glUseProgram(program);
    
    
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
    // set the background color (white)
    glClearColor(1.0, 1.0, 1.0, 1.0); 
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
    tri();
    
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

    if (x - lastx < 0) --posx;
    else if (x - lastx > 0) ++posx;
    lastx = x;

    if (y - lasty > 0) --posy;
    else if (y - lasty < 0) ++posy;
    lasty = y;
    // force the display routine to be called as soon as possible:
    glutPostRedisplay();
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
        camera.r += 1;
        viewer = camera.toVec();
        glUniform4fv(loc6, 1, viewer);
        update_viewer();
        glutPostRedisplay();
    }
    else if (key == 'z') {
        camera.r -= 1;
        viewer = camera.toVec();
        glUniform4fv(loc6, 1, viewer);
        update_viewer();
        glutPostRedisplay();
    }
}


int main(int argc, char** argv)
{
    
    //read obj file
    Parser parser;
    std::vector <int> tris;
    std::vector <float> verts;
    /*
    if (argc != 2) {
        std::cout << "usage: ./render filename.obj" << std::endl;
        return 1;
    }
    parser.parse_obj_file(argv[1], tris, verts);
     */
    
    parser.parse_obj_file("/Users/slgu1/Desktop/kitten.obj", tris, verts);
    //push verts
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
    glutMotionFunc(mouse_move_rotate);
 
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
