#include "render/texture_manager.h"

#include "stb_image.h"
#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#include <array>
#include <cstdio>
#include <vector>

namespace WE::Render {

TextureId TextureManager::LoadTexture(const char *texture_path) {
  TextureId current_id = next_id;
  next_id++;
  GLuint sprite;
  glGenTextures(1, &sprite);
  glBindTexture(GL_TEXTURE_2D, sprite);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLint width, height, nr_channels;
  unsigned char *data =
      stbi_load(texture_path, &width, &height, &nr_channels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load image!\n");
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
  sprites.insert({current_id, sprite});
  return current_id;
}

TextureId TextureManager::LoadTextureFromVector(
    const std::vector<std::vector<std::array<unsigned char, 4>>> &texture) {

  TextureId current_id = next_id;
  next_id++;
  GLuint sprite;
  glGenTextures(1, &sprite);
  glBindTexture(GL_TEXTURE_2D, sprite);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texture[0].size(),
               texture.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               &texture[0][0].at(0));

  glBindTexture(GL_TEXTURE_2D, 0);
  sprites.insert({current_id, sprite});
  return current_id;
}

void TextureManager::UnloadTexture(TextureId sprite_id) {
  glDeleteTextures(1, &sprites[sprite_id]);
  sprites.erase(sprite_id);
}

void TextureManager::SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                                     GLint parameter_value) {
  glBindTexture(GL_TEXTURE_2D, sprites[sprite_id]);
  glTexParameteri(GL_TEXTURE_2D, parameter_name, parameter_value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                                     GLfloat parameter_value) {
  glBindTexture(GL_TEXTURE_2D, sprites[sprite_id]);
  glTexParameterf(GL_TEXTURE_2D, parameter_name, parameter_value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace WE::Render
