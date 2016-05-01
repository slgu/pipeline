// a contrived vertex shader, that colors each vertex in a triangle
// according to its position

// we are going to be getting an attribute from the main program, named
// "vPosition", one for each vertex.
//
// on mac you may have to say: "attribute vec4 vPosition;" instead of this:
//the position
attribute vec4 vPosition;



vec4 light_position = vec4(100, 100, 100, 1);
//the vnorm
attribute vec4 vNorm;

//the trans matrix
// viewer's position, for lighting calculations
uniform vec4 viewer;
uniform mat4 vctm;
uniform mat4 viewtrans;
uniform mat4 orthtrans;

//for fragment shader
varying vec4 color;
varying vec4 sur_norm;
varying vec4 light_dir;
varying vec4 view_dir;

void main()
{
    gl_Position = orthtrans * viewtrans * vPosition;
    light_dir = normalize(light_position - vPosition);
    view_dir = normalize(viewer - vPosition);
    sur_norm = vNorm;
}
