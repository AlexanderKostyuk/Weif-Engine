#include "render/model_manager.h"

#include "stdio.h"
#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace {
void GenBuffers(WE::Render::Mesh &mesh) {
  glGenBuffers(mesh.VBO.size(), mesh.VBO.data());
  glGenBuffers(1, &mesh.EBO);
  glGenVertexArrays(1, &mesh.VAO);
}
void DeleteBuffers(WE::Render::Mesh &mesh) {
  glDeleteBuffers(mesh.VBO.size(), mesh.VBO.data());
  glDeleteBuffers(1, &mesh.EBO);
  glDeleteVertexArrays(1, &mesh.VAO);
}

} // namespace

namespace WE::Render {

MeshId ModelManager::LoadModel(Model model) {
  MeshId current_id = next_id;
  next_id++;
  Mesh mesh;
  GenBuffers(mesh);
  mesh.indices_amount = model.indices.size() * 3;
  glBindVertexArray(mesh.VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.vertices.size(),
               &model.vertices[0].x, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.normals.size(),
               &model.normals[0].x, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.uv.size(),
               &model.uv[0].x, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(glm::uvec3) * model.indices.size(), &model.indices[0].x,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  meshes.insert({current_id, mesh});
  printf("Generated model mesh for ID: %d, VAO ID: %d\n", current_id, mesh.VAO);
  printf("Meshes amount: %zu\n", meshes.size());
  return current_id;
}

void ModelManager::UnloadModel(MeshId mesh_id) {
  DeleteBuffers(meshes[mesh_id]);
  meshes.erase(mesh_id);
}

Model ModelManager::GenerateModelFlatShading(std::vector<glm::vec3> &vertices,
                                             std::vector<glm::vec2> &uv,
                                             std::vector<glm::uvec3> &indices) {

  std::vector<glm::vec3> normals(indices.size() * 3, glm::vec3(0.0f));

  // for (int i = 0; i < indices.size(); i++) {
  //   auto face_indices = indices[i];
  //   glm::vec3 normal =
  //       glm::cross(vertices[face_indices.z] - vertices[face_indices.y],
  //                  vertices[face_indices.y] - vertices[face_indices.x]);
  //   normals[face_indices.x] += normal;
  //   normals[face_indices.y] += normal;
  //   normals[face_indices.z] += normal;
  // }
  //
  // for (int i = 0; i < normals.size(); i++) {
  //   normals[i] = glm::normalize(normals[i]);
  // }
  //
  // return Model{
  //     .vertices = vertices, .normals = normals, .uv = uv, .indices =
  //     indices};

  std::vector<glm::vec3> new_vertices(indices.size() * 3, glm::vec3(0.0f));
  std::vector<glm::vec2> new_uv(indices.size() * 3, glm::vec2(0.0f));
  std::vector<glm::uvec3> new_indices(indices.size(), glm::uvec3(0));

  for (int i = 0; i < indices.size(); i++) {
    auto face_indices = indices[i];
    glm::vec3 normal =
        glm::cross(vertices[face_indices.z] - vertices[face_indices.y],
                   vertices[face_indices.y] - vertices[face_indices.x]);
    normals[i * 3] = normal;
    normals[i * 3 + 1] = normal;
    normals[i * 3 + 2] = normal;
    new_vertices[i * 3] = vertices[face_indices.x];
    new_vertices[i * 3 + 1] = vertices[face_indices.y];
    new_vertices[i * 3 + 2] = vertices[face_indices.z];
    new_uv[i * 3] = uv[face_indices.x];
    new_uv[i * 3 + 1] = uv[face_indices.y];
    new_uv[i * 3 + 2] = uv[face_indices.z];
    new_indices[i].x = i * 3;
    new_indices[i].y = i * 3 + 1;
    new_indices[i].z = i * 3 + 2;
  }

  return Model{.vertices = new_vertices,
               .normals = normals,
               .uv = new_uv,
               .indices = new_indices};
}
Model ModelManager::GenerateModelSmoothShading(
    std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uv,
    std::vector<glm::uvec3> &indices) {

  std::vector<glm::vec3> normals(indices.size() * 3, glm::vec3(0.0f));

  for (int i = 0; i < indices.size(); i++) {
    auto face_indices = indices[i];
    glm::vec3 normal =
        glm::cross(vertices[face_indices.z] - vertices[face_indices.y],
                   vertices[face_indices.y] - vertices[face_indices.x]);
    normals[face_indices.x] += normal;
    normals[face_indices.y] += normal;
    normals[face_indices.z] += normal;
  }

  for (int i = 0; i < normals.size(); i++) {
    normals[i] = glm::normalize(normals[i]);
  }

  return Model{
      .vertices = vertices, .normals = normals, .uv = uv, .indices = indices};
}

} // namespace WE::Render
