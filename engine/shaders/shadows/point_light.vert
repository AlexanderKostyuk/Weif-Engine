#version 460 core

layout(location = 0) in vec3 in_position;

layout(location = 1) uniform mat4 model_world_transform; 

void main() {

  gl_Position = model_world_transform * vec4(in_position,1.0f);

}
