#ifndef PROTOWORK_UTIL_HPP
#define PROTOWORK_UTIL_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace protowork {

namespace detail {
id_t load_shader_program(const char *, const char *);
} // namespace detail

using matrix_t = glm::mat4;
using pos_t = glm::vec3;
using index_t = GLushort;
using id_t = GLuint;

} // namespace protowork

#endif
