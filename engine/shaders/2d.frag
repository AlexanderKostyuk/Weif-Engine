#version 460 core

layout(location = 0) smooth in vec2 in_uv;

out vec4 out_color;

layout(binding = 1) uniform sampler2D diffuse_texture;

void main() {

  out_color = texture(diffuse_texture, in_uv);

}
