#version 460 core

layout(location = 0) in vec3 in_position;
layout(location = 3) in mat4 in_model_transform;

layout(std140, binding = 0) uniform projection_global_uniform {
  mat4 camera_clip_space_transform;
  mat4 world_camera_transform;
  mat4 world_directional_light_transform;
};

layout(std140, binding = 3) uniform ShadowMatrices {
  mat4 ortho_projection;
};

void main() {
  gl_Position = ortho_projection * world_directional_light_transform * in_model_transform * vec4(in_position, 1.0);
}


