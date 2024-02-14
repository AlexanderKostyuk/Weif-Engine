#ifndef WE_RENDER_OPENGL_GLOBAL_VARIABLES_H
#define WE_RENDER_OPENGL_GLOBAL_VARIABLES_H

#include "GL/glcorearb.h"
#include "glm/fwd.hpp"
#include <cstddef>
#include <cstdint>
namespace WE::Render::Opengl {
inline constexpr GLuint kProjectionUniformBlockIndex = 0;
inline constexpr GLuint kDirectionalLightUniformBlockIndex = 1;
inline constexpr GLuint kPointLightsUniformBlockIndex = 2;

// General pass locations
inline constexpr GLuint kModelWorldTransformLocation = 1;
inline constexpr GLuint kModelCameraNormalTransformLocation = 2;

inline constexpr GLuint kDirectionalLightExistsLocation = 8;
inline constexpr GLuint kDirectionalLightCameraSpaceDirectionLocation = 9;

inline constexpr GLuint kAmountOfPointLightsLocation = 14;
inline constexpr GLuint kPointLightPositionLocation = 15;
inline constexpr GLuint kFarPlaneLocation = 23;

inline constexpr GLenum kDirectionalShadowMapBinding = GL_TEXTURE0;
inline constexpr GLenum kPointShadowMapBinding = GL_TEXTURE0 + 1;
inline constexpr GLenum kGaussianTextureBinding = GL_TEXTURE0 + 9;
inline constexpr GLenum kDiffuseTextureBinding = GL_TEXTURE0 + 10;
inline constexpr GLenum kSpecularTextureBinding = GL_TEXTURE0 + 11;

// Shadow pass locations
inline constexpr GLuint kShadowMatricesLocation = 2;
inline constexpr GLuint kLightPositionLocation = 8;
inline constexpr GLuint kShadowPassFarPlaneLocation = 9;

inline constexpr std::size_t MAT4_SIZE = sizeof(glm::mat4);
inline constexpr std::size_t VEC4_SIZE = sizeof(glm::vec4);

} // namespace WE::Render::Opengl

#endif // !WE_RENDER_OPENGL_GLOBAL_VARIABLES_H
