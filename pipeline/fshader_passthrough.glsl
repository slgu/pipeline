// the simplest fragment shader: get the color (from the vertex shader - 
// actually interpolated from values specified in the vertex shader - and
// just pass them through to the render:
// 
// on the mac, you may need to say "varying vec4 color;" instead of this:
varying vec4 sur_norm;
varying vec4 light_dir;
varying vec4 view_dir;
vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);

vec4 material_ambient = vec4(1.0, 0.0, 1.0, 1.0);
vec4 material_diffuse = vec4(1.0, 0.8, 0.0, 1.0);
vec4 material_specular = vec4(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;
vec4 product(vec4 a, vec4 b)
{
    return vec4(a[0]*b[0], a[1]*b[1], a[2]*b[2], a[3]*b[3]);
}
void main() 
{ 
  // "gl_FragColor" is already defined for us - it's the one thing you have
  // to set in the fragment shader:
    vec4 hlf = normalize(light_dir+view_dir);
    vec4 ambient_color = product(material_ambient, light_ambient);
    float dd = dot(light_dir, sur_norm);
    vec4 diffuse_color, specular_color;
    if(dd>0.0) diffuse_color = dd*product(light_diffuse, material_diffuse);
    else diffuse_color =  vec4(0.0, 0.0, 0.0, 1.0);
    dd = dot(hlf, sur_norm);
    if(dd > 0.0) specular_color = exp(material_shininess*log(dd))*product(light_specular, material_specular);
    else specular_color = vec4(0.0, 0.0, 0.0, 1.0);
    gl_FragColor = ambient_color + diffuse_color + specular_color;
}