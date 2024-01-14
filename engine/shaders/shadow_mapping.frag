#version 460 core

layout(location = 0) in vec4 frag_position;

layout(location = 8) uniform vec3 light_position;
layout(location = 9) uniform float far_plane;

void main() {

  float light_distance = length(frag_position.xyz - light_position);

  light_distance = light_distance / far_plane;

  gl_FragDepth = light_distance;

}
