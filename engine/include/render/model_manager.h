#ifndef WE_RENDER_MODEL_MANAGER_H
#define WE_RENDER_MODEL_MANAGER_H

#include <array>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

#include "render/model.h"

namespace WE::Render {

using MeshId = std::uint16_t;

struct Mesh {
  std::array<GLuint, 3> VBO{};
  GLuint EBO;
  GLuint VAO;
  GLuint indices_amount;
};

class ModelManager {

public:
  ModelManager() {}
  MeshId LoadModel(Model model);
  void UnloadModel(MeshId mesh_id);
  inline GLuint GetVAO(MeshId mesh_id) const {
    try {
      return meshes.at(mesh_id).VAO;
    } catch (std::out_of_range err) {
      printf("Mesh ID: %i is out of range\n", mesh_id);
      return 0;
    }
  }
  inline GLuint GetIndicesAmount(MeshId mesh_id) const {
    try {
      return meshes.at(mesh_id).indices_amount;
    } catch (std::out_of_range err) {
      printf("Mesh ID: %i is out of range\n", mesh_id);
      return 0;
    }
  }

  std::vector<MeshId> LoadModelsFromFile(const char *file_path);

  Model GenerateModelFlatShading(std::vector<glm::vec3> &vertices,
                                 std::vector<glm::vec2> &uv,
                                 std::vector<glm::uvec3> &indices);

  Model GenerateModelSmoothShading(std::vector<glm::vec3> &vertices,
                                   std::vector<glm::vec2> &uv,
                                   std::vector<glm::uvec3> &indices);

  ModelManager(ModelManager &&other) = default;
  ModelManager &operator=(ModelManager &&other) = default;

  ModelManager(const ModelManager &) = delete;
  ModelManager &operator=(const ModelManager &) = delete;

private:
  std::unordered_map<MeshId, Mesh> meshes{};
  MeshId next_id = 1;
};

} // namespace WE::Render

#endif // !WE_RENDER_MODEL_MANAGER_H
