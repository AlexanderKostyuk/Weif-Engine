#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

layout(std140, binding = 3) uniform ShadowMatrices {
  mat4 ortho_projection;
  mat4 cube_direction[6];
};
layout(location = 8)uniform vec3 light_position;

out vec4 frag_position;

void main() {

  for(int face = 0; face < 6; face++){
    gl_Layer = face;
    for(int i = 0; i < 3; i++){
      frag_position = gl_in[i].gl_Position;
      gl_Position = cube_direction[face] * (frag_position - vec4(light_position,0.0f));
      EmitVertex();
    }
    EndPrimitive();
  }

}
