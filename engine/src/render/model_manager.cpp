#include "render/model_manager.h"

#include "assimp/vector3.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "render/model.h"
#include "stdio.h"
#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <queue>

namespace {

glm::vec3 AiVector3DToGLMVec3(const aiVector3D &vector) {
  return glm::vec3(vector.x, vector.y, vector.z);
}
glm::vec2 AiVector3DToGLMvec2(const aiVector3D &vector) {
  return glm::vec2(vector.x, vector.y);
}
glm::uvec3 AiFaceToGLMuvec3(const aiFace &face) {
  return glm::uvec3(face.mIndices[2], face.mIndices[1], face.mIndices[0]);
}

WE::Render::Model GenerateModelFromAiMesh(const aiMesh *mesh) {
  WE::Render::Model model{};
  model.vertices.reserve(mesh->mNumVertices);
  model.normals.reserve(mesh->mNumVertices);
  model.uv.reserve(mesh->mNumVertices);
  for (std::uint32_t index = 0; index < mesh->mNumVertices; index++) {
    model.vertices.push_back(AiVector3DToGLMVec3(mesh->mVertices[index]));
    model.normals.push_back(AiVector3DToGLMVec3(mesh->mNormals[index]));
    if (mesh->mTextureCoords[0] != nullptr)
      model.uv.push_back(AiVector3DToGLMvec2(mesh->mTextureCoords[0][index]));
    else
      model.uv.push_back(glm::uvec2(0.0f));
  }
  model.indices.reserve(mesh->mNumFaces);
  for (std::uint32_t face_index = 0; face_index < mesh->mNumFaces;
       face_index++) {
    model.indices.push_back(AiFaceToGLMuvec3(mesh->mFaces[face_index]));
  }
  return model;
}

} // namespace

namespace WE::Render {

Mesh::Mesh() {
  glGenBuffers(VBO.size(), VBO.data());
  glGenBuffers(1, &transform_buffer);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);
}

Mesh::~Mesh() {
  glDeleteBuffers(VBO.size(), VBO.data());
  glDeleteBuffers(1, &transform_buffer);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
}

MeshId ModelManager::LoadModel(Model model) {
  MeshId current_id = next_id;
  next_id++;

  Mesh &mesh = meshes[current_id];

  mesh.indices_amount = model.indices.size() * 3;
  glBindVertexArray(mesh.VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glEnableVertexAttribArray(6);

  // Vertex positions
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.vertices.size(),
               &model.vertices[0].x, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

  // Normals
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.normals.size(),
               &model.normals[0].x, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

  // UV
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.uv.size(),
               &model.uv[0].x, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

  // Transform matrices
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO[3]);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(0 * sizeof(glm::vec4)));
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(1 * sizeof(glm::vec4)));
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(2 * sizeof(glm::vec4)));
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(3 * sizeof(glm::vec4)));
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);

  // Elements
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(glm::uvec3) * model.indices.size(), &model.indices[0].x,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  printf("Generated model mesh for ID: %d, VAO ID: %d\n", current_id, mesh.VAO);
  printf("Meshes amount: %zu\n", meshes.size());
  return current_id;
}

std::vector<MeshId> ModelManager::LoadModelsFromFile(const char *file_path) {

  std::vector<MeshId> mesh_ids{};

  Assimp::Importer assimp_importer;
  const aiScene *scene = assimp_importer.ReadFile(
      file_path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    printf("Assimp error! %s\n", assimp_importer.GetErrorString());
    return mesh_ids;
  }

  std::queue<aiNode *> nodes{};
  nodes.push(scene->mRootNode);
  while (!nodes.empty()) {

    aiNode *node = nodes.front();
    nodes.pop();

    for (std::uint32_t index = 0; index < node->mNumMeshes; index++) {
      mesh_ids.push_back(LoadModel(
          GenerateModelFromAiMesh(scene->mMeshes[node->mMeshes[index]])));
    }
    for (std::uint32_t index = 0; index < node->mNumChildren; index++) {
      nodes.push(node->mChildren[index]);
    }
  }
  return mesh_ids;
}

Model ModelManager::GenerateModelFlatShading(std::vector<glm::vec3> &vertices,
                                             std::vector<glm::vec2> &uv,
                                             std::vector<glm::uvec3> &indices) {

  std::vector<glm::vec3> normals(indices.size() * 3, glm::vec3(0.0f));

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
