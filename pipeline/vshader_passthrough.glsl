// a contrived vertex shader, that colors each vertex in a triangle
// according to its position

// we are going to be getting an attribute from the main program, named
// "vPosition", one for each vertex.
//
// on mac you may have to say: "attribute vec4 vPosition;" instead of this:
//the position
attribute vec4 vPosition;

// light & material definitions, again for lighting calculations:
vec4 light_position = vec4(100, 100, 100, 1);
vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);

vec4 material_ambient = vec4(1.0, 0.0, 1.0, 1.0);
vec4 material_diffuse = vec4(1.0, 0.8, 0.0, 1.0);
vec4 material_specular = vec4(1.0, 0.8, 0.0, 1.0);

//the vnorm
attribute vec4 vNorm;

//the trans matrix
// viewer's position, for lighting calculations
uniform vec4 viewer;
uniform mat4 vctm;
uniform mat4 viewtrans;
uniform mat4 orthtrans;
varying vec4 color;
vec4 product(vec4 a, vec4 b)
{
    return vec4(a[0]*b[0], a[1]*b[1], a[2]*b[2], a[3]*b[3]);
}
void main()
{
    gl_Position = orthtrans * viewtrans * vPosition;
    vec4 light_dir = normalize(light_position - vPosition);
    vec4 view_dir = normalize(viewer - vPosition);
    vec4 hlf = normalize(light_dir+view_dir);
    vec4 ambient_color = product(material_ambient, light_ambient);
    float dd = dot(light_dir, vNorm);
    vec4 diffuse_color, specular_color;
    if(dd>0.0) diffuse_color = dd*product(light_diffuse, material_diffuse);
    else diffuse_color =  vec4(0.0, 0.0, 0.0, 1.0);
    dd = dot(hlf, vNorm);
    if(dd > 0.0) specular_color = exp(100.0*log(dd))*product(light_specular, material_specular);
    else specular_color = vec4(0.0, 0.0, 0.0, 1.0);
    color = ambient_color + diffuse_color + specular_color;
}
