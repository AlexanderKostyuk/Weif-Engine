#version 460 core

struct LightInfo {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

struct PointLight {
  vec3 camera_space_position;
  LightInfo light_info;
  vec3 intensity;
};


layout(location = 0) smooth in vec3 in_camera_space_position;
layout(location = 1) smooth in vec3 in_world_space_position;
layout(location = 2) smooth in vec3 in_vertex_normal;
layout(location = 3) smooth in vec2 in_uv;
layout(location = 4) smooth in vec4 in_directional_light_space_position;
layout(location = 5) smooth in vec3 in_point_light_space_position[8];

out vec4 out_color;

//Transform matrix uniforms
layout(binding = 0) uniform projection_global_uniform {
  mat4 camera_clip_space_transform;
  mat4 world_camera_transform;
  mat4 world_directional_light_transform;
};

//Directional light info
layout(location = 8) uniform bool directional_light_exists;
layout(location = 9) uniform vec3 directional_light_camera_space_direction;
layout(std140, binding = 1) uniform DirectionLight {
  LightInfo directional_light;
};
layout(binding = 0) uniform sampler2D directional_light_shadow_map;

//Point Lights info
layout(location = 14) uniform int point_lights_amount;
layout(location = 15) uniform vec3 point_light_position[8];
layout(location = 23) uniform float far_plane;
layout(std140, binding = 2) uniform PointLights {
  PointLight point_lights[8];
};
layout(binding = 1) uniform samplerCube shadow_maps[8];


//Textures
layout(binding = 9) uniform sampler2D gaussian_texture;
layout(binding = 10) uniform sampler2D diffuse_texture;
layout(binding = 11) uniform sampler2D specular_texture;

vec4 CalculateColors(vec3 light_direction, LightInfo light_info){

  float cos_angle_incidence = dot(in_vertex_normal, light_direction);
  cos_angle_incidence = clamp(cos_angle_incidence, 0, 1);
  vec3 view_dir = normalize(-in_camera_space_position);
  vec3 half_angle = normalize(light_direction + view_dir);
  float gaussian_term = 0;
  if(cos_angle_incidence != 0){
    vec2 texture_coord;
    texture_coord.s = dot(in_vertex_normal, half_angle);
    texture_coord.t = 0.5f;

    gaussian_term = texture(gaussian_texture, texture_coord).r;
  }
  
  vec4 color = light_info.ambient * texture(diffuse_texture, in_uv);
  color += light_info.diffuse * texture(diffuse_texture, in_uv) * cos_angle_incidence;
  color += light_info.specular * texture(specular_texture, in_uv) * gaussian_term;

  return color;
}

vec4 CalculateDirectionalLight(LightInfo directional_light, float shadow){
  
  if(shadow >= 1.0f) return directional_light.ambient * texture(diffuse_texture, in_uv);

  vec3 light_direction = normalize(-directional_light_camera_space_direction);
  vec4 color = CalculateColors(light_direction, directional_light);

  return color;
}

vec4 CalculatePointLight(PointLight point_light, float shadow){

  vec3 light_direction = point_light.camera_space_position - in_camera_space_position;
  float distance_square = dot(light_direction, light_direction);
  light_direction = normalize(light_direction);
  float attentuation = 1.0 / (point_light.intensity.x + point_light.intensity.y * sqrt(distance_square) + point_light.intensity.z * distance_square);

  if(shadow >= 1.0f) return point_light.light_info.ambient * texture(diffuse_texture, in_uv) * attentuation;

  vec4 color = CalculateColors(light_direction, point_light.light_info);
  return color * attentuation;
}

float CalculatePointShadow(samplerCube shadow_map, vec3 light_space_position) {

  float shadow = 0.0f;

  float closest_depth = texture(shadow_map,light_space_position).r;
  closest_depth *= far_plane;
  float current_depth = length(light_space_position);
  if(current_depth - 0.05f > closest_depth) shadow = 1.0f;

  return shadow;
}

float CalculateDirectionalShadow() {
  float shadow = 0.0f;

  vec3 texture_coords = in_directional_light_space_position.xyz * 0.5f + 0.5f;
  float closest_depth = texture(directional_light_shadow_map, texture_coords.xy).r;
  float current_depth = texture_coords.z;
  if(current_depth - 0.05f > closest_depth) shadow = 1.0f;

  return shadow;
}

void main() {

  out_color = vec4(0.0f,0.0f,0.0f,1.0f);


  float shadow = 0.0f;
  if(directional_light_exists) {
    shadow = CalculateDirectionalShadow();
    out_color += CalculateDirectionalLight(directional_light,shadow);
  }
  for(int i = 0; i < point_lights_amount; i++) {
    shadow = CalculatePointShadow(shadow_maps[i], in_point_light_space_position[i]);
    out_color += CalculatePointLight(point_lights[i],shadow);
  }

}
