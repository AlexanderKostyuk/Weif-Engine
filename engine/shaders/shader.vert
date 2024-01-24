#version 460 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_vertex_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) smooth out vec3 out_camera_space_position;
layout(location = 1) smooth out vec3 out_world_space_position;
layout(location = 2) smooth out vec3 out_vertex_normal;
layout(location = 3) smooth out vec2 out_uv;
layout(location = 4) smooth out vec4 out_directional_light_space_position;
layout(location = 5) smooth out vec3 out_point_light_space_position[8];

//Transform matrix uniforms
layout(binding = 0) uniform projection_global_uniform {
  mat4 camera_clip_space_transform;
  mat4 world_camera_transform;
  mat4 world_directional_light_transform;
};
layout(location = 1) uniform mat4 model_world_transform;
layout(location = 2) uniform mat3 model_camera_normal_transform;

//Directional light info
layout(location = 8) uniform bool directional_light_exists;

//Point Lights info
layout(location = 14) uniform int point_lights_amount;
layout(location = 15) uniform vec3 point_light_position[8];



void main() {

  out_world_space_position = vec3(model_world_transform * vec4(in_position, 1.0f));
  out_camera_space_position = vec3(world_camera_transform * vec4(out_world_space_position, 1.0f));
  gl_Position = camera_clip_space_transform * vec4(out_camera_space_position,1.0f);

  out_vertex_normal = normalize(model_camera_normal_transform * in_vertex_normal);
  out_uv = in_uv;

  if(directional_light_exists)
    out_directional_light_space_position = world_directional_light_transform * vec4(out_world_space_position,1.0f);

  for(int i = 0 ; i < point_lights_amount; i++)
    out_point_light_space_position[i] = out_world_space_position - point_light_position[i];

}
