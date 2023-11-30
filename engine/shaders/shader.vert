#version 430

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_vertex_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) smooth out vec3 out_camera_space_position;
layout(location = 1) smooth out vec3 out_vertex_normal;
layout(location = 2) smooth out vec2 out_uv;

layout(binding = 0) uniform projection_global_uniform {
  mat4 perspective_matrix_uniform;
};
layout(location = 1) uniform mat4 model_camera_matrix_uniform;
layout(location = 129) uniform mat3 model_camera_normal_matrix_uniform;

void main() {
  gl_Position = perspective_matrix_uniform * model_camera_matrix_uniform * vec4(in_position,1.0f);
  out_camera_space_position = (model_camera_matrix_uniform * vec4(in_position,1.0f)).xyz;
  out_vertex_normal = normalize(model_camera_normal_matrix_uniform * in_vertex_normal);
  out_uv = in_uv;
}
