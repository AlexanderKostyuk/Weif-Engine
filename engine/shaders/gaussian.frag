#version 430

layout(location = 0) smooth in vec3 in_camera_space_position;
layout(location = 1) smooth in vec3 in_vertex_normal;
layout(location = 2) smooth in vec2 in_uv;

out vec4 out_color;
struct LightInfo {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};
struct PointLight {
  vec3 position;
  LightInfo light_info;
  vec3 intensity;
};
struct DirectionalLight{
  vec3 direction;
  LightInfo light_info;
};

layout(std140, binding = 1) uniform DirectionLight {
  DirectionalLight directional_light;
};
layout(std140, binding = 2) uniform PointLights {
  PointLight point_lights[8];
};
layout(location = 132) uniform bool direction_light_exists;
layout(location = 133) uniform float shininess;
layout(location = 134) uniform int amount_of_point_lights;


layout(binding = 0) uniform sampler2D gaussian_texture;
layout(binding = 1) uniform sampler2D diffuse_texture;
layout(binding = 2) uniform sampler2D specular_texture;

vec4 CalculateLightParameters(vec3 light_direction, LightInfo light_info){
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
  
  vec4 ambient = light_info.ambient * texture(diffuse_texture, in_uv);
  vec4 diffuse = light_info.diffuse * texture(diffuse_texture, in_uv) * cos_angle_incidence;
  vec4 specular = light_info.specular * texture(specular_texture, in_uv) * gaussian_term;

  return ambient + diffuse + specular;

}

vec4 CalculateDirectionalLight(DirectionalLight directional_light){
  
  vec3 light_direction = normalize(-directional_light.direction);
  vec4 color = CalculateLightParameters(light_direction, directional_light.light_info);

  return color;
}

vec4 CalculatePointLight(PointLight point_light){

  vec3 light_direction = point_light.position - in_camera_space_position;
  float distance_square = dot(light_direction, light_direction);
  light_direction = normalize(light_direction);
  float attentuation = 1.0 / (point_light.intensity.x + point_light.intensity.y * sqrt(distance_square) + point_light.intensity.z * distance_square);

  vec4 color = CalculateLightParameters(light_direction, point_light.light_info);
  return color * attentuation;
}

void main() {

  out_color = vec4(0.0f,0.0f,0.0f,1.0f);

  if(direction_light_exists) out_color += CalculateDirectionalLight(directional_light);
  for(int i = 0; i < amount_of_point_lights; i++) {
    out_color += CalculatePointLight(point_lights[i]);
  }

}
