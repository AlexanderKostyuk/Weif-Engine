#version 430

layout(location = 0) smooth in vec3 in_camera_space_position;
layout(location = 1) smooth in vec3 in_vertex_normal;
layout(location = 2) smooth in vec2 in_uv;

out vec4 out_color;

layout(location = 128) uniform vec3 camera_space_light_position_uniform;

layout(binding = 0) uniform sampler2D albedo_texture;
layout(binding = 1) uniform sampler2D normal_texture;

void main() {

  vec3 light_difference = camera_space_light_position_uniform - in_camera_space_position;
  float light_distance_square = dot(light_difference,light_difference);
  vec3 light_direction = light_difference * inversesqrt(light_distance_square);

  float distance_factor = sqrt(light_distance_square);

  float cos_angle_incidence = dot(in_vertex_normal, light_direction);
  cos_angle_incidence = clamp(cos_angle_incidence, 0, 1);
  vec3 view_direction = normalize(-in_camera_space_position);
  
  vec3 half_angle = normalize(light_direction + view_direction);
  float angle_normal_half = acos(dot(half_angle,in_vertex_normal));
  float exponent = angle_normal_half / 1.0f;
  exponent = -(exponent * exponent);
  float gaussian_term = exp(exponent);
  gaussian_term = cos_angle_incidence != 0 ? gaussian_term : 0;
  
  float attentation_intesity = (1/(1 + distance_factor/4.0f)) * 0.4f;
  vec4 ambient = 0.2f * texture(albedo_texture, in_uv);
  vec4 diffuse = 0.4f * texture(albedo_texture, in_uv) * cos_angle_incidence;
  vec4 specular = attentation_intesity * vec4(0.1f, 0.1f, 0.1f, 1.0f) * gaussian_term;
 
  out_color = ambient + diffuse + specular;

}
