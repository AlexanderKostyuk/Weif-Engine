#include "render/texture_manager.h"

#include "stb_image.h"
#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#include <array>
#include <cmath>
#include <cstdio>
#include <gl/gl.h>
#include <vector>

namespace {

std::vector<GLubyte> GenerateGaussianTextureData(int cos_angle_resolution,
                                                 int shininess_resolution) {
  std::vector<GLubyte> gaussian_texture{};
  gaussian_texture.resize(cos_angle_resolution * shininess_resolution);
  auto current_pixel = gaussian_texture.begin();
  for (int shininess_counter = 1; shininess_counter <= shininess_resolution;
       shininess_counter++) {

    float shininess = shininess_counter / (float)shininess_resolution;

    for (int cos_angle_counter = 0; cos_angle_counter < cos_angle_resolution;
         cos_angle_counter++) {
      float cos_angle = cos_angle_counter / (float)(cos_angle_resolution - 1);
      float angle = acosf(cos_angle);
      float exponent = angle / shininess;
      exponent = -(exponent * exponent);
      float gaussian_term = std::exp(exponent);

      *current_pixel = static_cast<GLubyte>(gaussian_term * 255.0f);
      current_pixel++;
    }
  }

  return gaussian_texture;
}

GLuint CreateGaussianTexture(int cos_angle_resolution,
                             int shininess_resolution) {

  auto gaussian_texture_data =
      GenerateGaussianTextureData(cos_angle_resolution, shininess_resolution);

  GLuint gaussian_texture;
  glGenTextures(1, &gaussian_texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gaussian_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, cos_angle_resolution,
               shininess_resolution, 0, GL_RED, GL_UNSIGNED_BYTE,
               &gaussian_texture_data[0]);
  glBindTexture(GL_TEXTURE_2D, 0);
  return gaussian_texture;
}

GLuint CreateTextureFromFile(const char *texture_path) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLint width, height, nr_channels;
  unsigned char *texture_data =
      stbi_load(texture_path, &width, &height, &nr_channels, 0);

  if (texture_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load image!\n");
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(texture_data);
  return texture;
}

GLuint CreateTextureFromVector(const WE::Render::TextureRGBA &texture_data) {

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texture_data[0].size(),
               texture_data.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               &texture_data[0][0].at(0));

  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
}

} // namespace

namespace WE::Render {

void TextureManager::Init() {

  GLuint gaussian_term_texture = CreateGaussianTexture(4096, 128);
  gaussian_term_texture_id = AddTexture(gaussian_term_texture);

  TextureRGBA default_diffuse_texture{
      std::vector<RGBAVec>{RGBAVec{255, 255, 255, 255}}};
  default_diffuse_texture_id = LoadTexture(default_diffuse_texture);

  TextureRGBA default_specular_texture{
      std::vector<RGBAVec>{RGBAVec{255, 255, 255, 255}}};
  default_specular_texture_id = LoadTexture(default_specular_texture);
}

TextureId TextureManager::AddTexture(GLuint texture) {
  TextureId current_id = next_id;
  textures.insert({current_id, texture});
  printf("Created new texture with ID: %d\n", current_id);
  next_id++;
  return current_id;
}

TextureId TextureManager::LoadTexture(const char *texture_path) {
  GLuint texture = CreateTextureFromFile(texture_path);
  TextureId texture_id = AddTexture(texture);
  return texture_id;
}

TextureId TextureManager::LoadTexture(const TextureRGBA &texture_data) {
  GLuint texture = CreateTextureFromVector(texture_data);
  TextureId texture_id = AddTexture(texture);
  return texture_id;
}

TextureId TextureManager::Create2DShadowMap(uint16_t size) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);
  TextureId texture_id = AddTexture(texture);
  return texture_id;
}

TextureId TextureManager::CreateCubeShadowMap(uint16_t size) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  for (unsigned int i = 0; i < 6; ++i)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  TextureId texture_id = AddTexture(texture);
  return texture_id;
}

void TextureManager::UnloadTexture(TextureId sprite_id) {
  glDeleteTextures(1, &textures[sprite_id]);
  textures.erase(sprite_id);
}

void TextureManager::SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                                     GLint parameter_value) {
  glBindTexture(GL_TEXTURE_2D, textures[sprite_id]);
  glTexParameteri(GL_TEXTURE_2D, parameter_name, parameter_value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                                     GLfloat parameter_value) {
  glBindTexture(GL_TEXTURE_2D, textures[sprite_id]);
  glTexParameterf(GL_TEXTURE_2D, parameter_name, parameter_value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace WE::Render
