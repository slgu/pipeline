// the simplest fragment shader: get the color (from the vertex shader - 
// actually interpolated from values specified in the vertex shader - and
// just pass them through to the render:
// 
// on the mac, you may need to say "varying vec4 color;" instead of this:
#ifdef linux
in vec4 color;
#else
varying vec4 color;
#endif

void main() 
{ 
  // "gl_FragColor" is already defined for us - it's the one thing you have
  // to set in the fragment shader:
  gl_FragColor = color;
} 

