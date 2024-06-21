#version 460 core

layout(location = 0) in vec3 in_position;
layout(location = 3) in mat4 in_model_transform;


void main() {

  gl_Position = in_model_transform * vec4(in_position,1.0f);

}
