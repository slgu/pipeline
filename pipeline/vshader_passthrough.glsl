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
uniform int shader_type;//use checkboard shader or bing-phone shader
uniform int detail; //checkboard shader detail
//for fragment shader
varying vec4 color;
varying vec4 sur_norm;
varying vec4 light_dir;
varying vec4 view_dir;
varying float fshader_type;
varying vec4 pos;
varying float fdetail;
void main()
{
    gl_Position = orthtrans * viewtrans * vPosition;
    pos = vPosition;
    if (shader_type == 1) {
        fshader_type = 1.0;
        light_dir = normalize(light_position - vPosition);
        view_dir = normalize(viewer - vPosition);
        sur_norm = vNorm;
    }
    else {
        fshader_type = 0.0;
        fdetail = float(detail);
    }
}
