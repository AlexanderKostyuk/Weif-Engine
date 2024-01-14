#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

layout(location = 2)uniform mat4 shadow_matrices[6];

out vec4 frag_position;

void main() {

  for(int face = 0; face < 6; face++){
    gl_Layer = face;
    for(int i = 0; i < 3; i++){
      frag_position = gl_in[i].gl_Position;
      gl_Position = shadow_matrices[face] * frag_position;
      EmitVertex();
    }
    EndPrimitive();
  }

}
