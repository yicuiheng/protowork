#include <fstream>
#include <protowork.hpp>
#include <sstream>

id_t protowork::detail::load_shader_program(const char *vertex_shader,
                                            const char *fragment_shader) {
    id_t vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    id_t fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    glShaderSource(vertex_shader_id, 1, &vertex_shader, nullptr);
    glCompileShader(vertex_shader_id);

    // Check Vertex Shader
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> msg(info_log_length);
        glGetShaderInfoLog(vertex_shader_id, info_log_length, nullptr,
                           msg.data());
        throw std::runtime_error{std::string{msg.data()}};
    }

    // Compile Fragment Shader
    glShaderSource(fragment_shader_id, 1, &fragment_shader, nullptr);
    glCompileShader(fragment_shader_id);

    // Check Fragment Shader
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> msg(info_log_length);
        glGetShaderInfoLog(fragment_shader_id, info_log_length, nullptr,
                           msg.data());
        throw std::runtime_error{std::string{msg.data()}};
    }
    // Link the program
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> msg(info_log_length);
        glGetProgramInfoLog(fragment_shader_id, info_log_length, nullptr,
                            msg.data());
        throw std::runtime_error{std::string{msg.data()}};
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}
