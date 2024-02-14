#ifndef WE_RENDER_TEXTURE_MANAGER_H
#define WE_RENDER_TEXTURE_MANAGER_H

#include <GL/glcorearb.h>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace WE::Render {

using TextureId = std::uint16_t;
using RGBAVec = std::array<unsigned char, 4>;
using RGBVec = std::array<unsigned char, 3>;
using TextureRGBA = std::vector<std::vector<RGBAVec>>;
using TextureRGB = std::vector<std::vector<RGBVec>>;

class TextureManager {

private:
  TextureId AddTexture(GLuint texture);

public:
  TextureManager(){};
  void Init();
  TextureId LoadTexture(const char *texture_path);
  TextureId LoadTexture(const TextureRGBA &texture);
  TextureId Create2DShadowMap(const uint16_t size);
  TextureId CreateCubeShadowMap(const uint16_t size);
  void UnloadTexture(TextureId sprite_id);

  void SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                       GLint parameter_value);
  void SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                       GLfloat parameter_value);

  inline GLuint GetTexture(TextureId texture_id) const {
    return textures.at(texture_id);
  }

  inline GLuint GetGaussianTermTextureId() const {
    return gaussian_term_texture_id;
  }

  inline GLuint GetDefaultDiffuseTextureId() const {
    return default_diffuse_texture_id;
  }

  inline GLuint GetDefaultSpecularTextureId() const {
    return default_specular_texture_id;
  }

  TextureManager(TextureManager &&other) = default;
  TextureManager &operator=(TextureManager &&other) = default;

  TextureManager(const TextureManager &) = delete;
  TextureManager &operator=(const TextureManager &) = delete;

private:
  std::unordered_map<TextureId, GLuint> textures{};
  TextureId gaussian_term_texture_id;
  TextureId default_diffuse_texture_id;
  TextureId default_specular_texture_id;
  TextureId next_id = 1;
};

} // namespace WE::Render

#endif // !WE_RENDER_TEXTURE_MANAGER_H
