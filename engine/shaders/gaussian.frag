#version 430

struct DirectionLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

layout(location = 0) smooth in vec3 in_camera_space_position;
layout(location = 1) smooth in vec3 in_vertex_normal;
layout(location = 2) smooth in vec2 in_uv;

out vec4 out_color;

layout(location = 128) uniform DirectionLight direction_light;
layout(location = 132) uniform bool direction_light_exists;
layout(location = 133) uniform float shininess;


layout(binding = 0) uniform sampler2D gaussian_texture;
layout(binding = 1) uniform sampler2D diffuse_texture;
layout(binding = 2) uniform sampler2D specular_texture;

vec4 CalculateDirectionalLight(){
  
  vec3 light_direction = normalize(-direction_light.direction);
  float cos_angle_incidence = dot(in_vertex_normal, light_direction);
  cos_angle_incidence = clamp(cos_angle_incidence, 0, 1);


  vec3 view_dir = normalize(-in_camera_space_position);
  vec3 half_angle = normalize(light_direction + view_dir);
  vec2 gaussian_term_coord;
  gaussian_term_coord.s = dot(in_vertex_normal, half_angle);
  gaussian_term_coord.t = 0.5f;
  float gaussian_term = texture(gaussian_texture, gaussian_term_coord).r;
  if(cos_angle_incidence == 0)
    gaussian_term = 0;
  
  vec4 ambient = vec4(direction_light.ambient,1.0f) * texture(diffuse_texture, in_uv);
  vec4 diffuse = vec4(direction_light.diffuse,1.0f) * texture(diffuse_texture, in_uv) * cos_angle_incidence;
  vec4 specular = vec4(direction_light.specular,1.0f) * texture(specular_texture, in_uv) * gaussian_term;

  return ambient + diffuse + specular;

}

void main() {

  out_color = vec4(0.0f,0.0f,0.0f,1.0f);

  if(direction_light_exists) out_color += CalculateDirectionalLight();

}
