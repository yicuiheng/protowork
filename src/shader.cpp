#include <fstream>
#include <sstream>
#include <protowork.hpp>

const char* vertex_shader_code = R"(
#version 430 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexCoord;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 LightDirection_cameraspace;
out vec2 Coord;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

void main(){
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

    Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
    vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

    vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;

    Coord = vertexCoord;
})";

const char* fragment_shader_code = R"(
#version 430 core

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec2 Coord;

out vec3 color;

uniform vec3 LightPosition_worldspace;

vec3 calcColor(vec2 coord,float n) {
    vec3 color1 = vec3(1.0, 0.2, 0.2);
    vec3 color2 = vec3(0.9, 0.8, 0.8);

    coord = coord * n;
    float a = mod(floor(coord.x) + floor(coord.y), 2.0);
    return mix(color1, color2, a);
}

void main()
{
    vec3 LightColor = vec3(1,1,1);
    float LightPower = 50.0f;

    // Material properties
    vec3 MaterialDiffuseColor = calcColor(Coord, 5.0);
    vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;

    float distance = length(LightPosition_worldspace - Position_worldspace);

    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);
    float cosTheta = clamp(dot(n, l), 0, 1);

    color =
        MaterialAmbientColor +
        MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance);
}
)";

id_t protowork::detail::load_shader_program() {
    id_t vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    id_t fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    
    GLint result = GL_FALSE;
    int info_log_length;
    
    // Compile Vertex Shader
    glShaderSource(vertex_shader_id, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex_shader_id);
    
    // Check Vertex Shader
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0){
        std::vector<char> msg(info_log_length);
        glGetShaderInfoLog(vertex_shader_id, info_log_length, nullptr, msg.data());
        throw std::runtime_error{std::string{msg.data()}};
    }
    
    // Compile Fragment Shader
    glShaderSource(fragment_shader_id, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment_shader_id);
    
    // Check Fragment Shader
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if ( info_log_length > 0 ){
        std::vector<char> msg(info_log_length);
        glGetShaderInfoLog(fragment_shader_id, info_log_length, nullptr, msg.data());
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
    if (info_log_length > 0){
        std::vector<char> msg(info_log_length);
        glGetProgramInfoLog(fragment_shader_id, info_log_length, nullptr, msg.data());
        throw std::runtime_error{std::string{msg.data()}};
    }
    
    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);
    
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    
    return program_id;
}

