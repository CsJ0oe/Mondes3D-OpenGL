#version 330 core

in vec3 vtx_position;
in vec4 vtx_color;
out vec4 var_color;
uniform float zoom = 1;
uniform vec2 trans = vec2(0., 0.);
uniform int mode = 0;
uniform int view = 0;

void main()
{
  if (view == 0)
      gl_Position = vec4(vtx_position.xy, -vtx_position.z, 1.);
  else
      gl_Position = vec4(-vtx_position.z, vtx_position.yx, 1.);

  gl_Position = gl_Position * vec4(zoom,zoom,zoom,1.) + vec4(trans,0.,0.);

  if (mode == 0)
      var_color = vtx_color;
  else
      var_color = vec4(1.,1.,1.,1.);
}
