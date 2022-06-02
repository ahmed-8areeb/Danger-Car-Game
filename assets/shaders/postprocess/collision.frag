#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;

out vec4 frag_color;

void main(){ 

  ///vec4 Faded =(2,1,1,1);
  vec4 originalTex=texture(tex,tex_coord);
  frag_color = vec4(originalTex.x*2,originalTex.y,originalTex.z,originalTex.w);
}