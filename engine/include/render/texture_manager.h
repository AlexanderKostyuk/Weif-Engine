#ifndef WE_RENDER_TEXTURE_MANAGER_H
#define WE_RENDER_TEXTURE_MANAGER_H

#include <GL/glcorearb.h>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace WE::Render {

using TextureId = std::uint16_t;

class TextureManager {
public:
  TextureManager() {}
  TextureId LoadTexture(const char *texture_path);
  TextureId LoadTextureFromVector(
      const std::vector<std::vector<std::array<unsigned char, 4>>> &texture);
  void UnloadTexture(TextureId sprite_id);

  void SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                       GLint parameter_value);
  void SetTexParameter(TextureId sprite_id, GLenum parameter_name,
                       GLfloat parameter_value);

  inline GLuint GetTexture(TextureId sprite_id) {
    return sprites.at(sprite_id);
  }

  TextureManager(TextureManager &&other) = default;
  TextureManager &operator=(TextureManager &&other) = default;

  TextureManager(const TextureManager &) = delete;
  TextureManager &operator=(const TextureManager &) = delete;

private:
  std::unordered_map<TextureId, GLuint> sprites{};
  TextureId next_id = 1;
};

} // namespace WE::Render

#endif // !WE_RENDER_TEXTURE_MANAGER_H
