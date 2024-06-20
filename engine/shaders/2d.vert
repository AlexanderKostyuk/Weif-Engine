#version 460 core

layout(location = 0) in vec2 in_vertex_position;
layout(location = 1) in mat4 in_transform;

layout(location = 0) smooth out vec2 out_uv;

layout(binding = 0) uniform projection_global_uniform {
  mat4 camera_clip_space_transform;
  mat4 world_camera_transform;
};


void main() {

  gl_Position = camera_clip_space_transform * world_camera_transform * in_transform * vec4(in_vertex_position, 0.0f, 1.0f);
  
  out_uv = in_vertex_position;

}
