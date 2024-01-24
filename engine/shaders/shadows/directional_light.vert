#version 460 core

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform projection_global_uniform {
  mat4 camera_clip_space_transform;
  mat4 world_camera_transform;
  mat4 world_directional_light_transform;
};

layout(location = 1) uniform mat4 model_world_transform;

void main() {
  gl_Position = world_directional_light_transform * model_world_transform * vec4(in_position, 1.0);
}


