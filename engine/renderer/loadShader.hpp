#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "glad/glad.h"
#include "core/common.hpp"

class Shader {
public:
    unsigned int ID = 0;
    Shader() {}

    void init(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            
            vShaderFile.close();
            fShaderFile.close();
            
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        
        unsigned int vertex, fragment;
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    
    void use() { 
        glUseProgram(ID); 
    }
    
    void setBool(const std::string& name, bool value) const {
        int location;
        auto it = uniformCache.find(name);
        if (it != uniformCache.end()) {
            location = it->second;
        } else {
            location = glGetUniformLocation(ID, name.c_str());
            if (location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' doesn't exist or is not used in the shader." << std::endl;
            }
            uniformCache[name] = location;
        }
        glUniform1i(location, (int)value);
    }

    void setInt(const std::string& name, int value) const {
        int location;
        auto it = uniformCache.find(name);
        if (it != uniformCache.end()) {
            location = it->second;
        } else {
            location = glGetUniformLocation(ID, name.c_str());
            if (location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' doesn't exist or is not used in the shader." << std::endl;
            }
            uniformCache[name] = location;
        }
        glUniform1i(location, value);
    }

    void setFloat(const std::string& name, float& value) const {
        int location;
        auto it = uniformCache.find(name);
        if (it != uniformCache.end()) {
            location = it->second;
        } else {
            location = glGetUniformLocation(ID, name.c_str());
            if (location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' doesn't exist or is not used in the shader." << std::endl;
            }
            uniformCache[name] = location;
        }
        glUniform1f(location, value);
    }

    void setVec2(const std::string& name, glm::vec2& value) const {
        int location;
        auto it = uniformCache.find(name);
        if (it != uniformCache.end()) {
            location = it->second;
        } else {
            location = glGetUniformLocation(ID, name.c_str());
            if (location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' doesn't exist or is not used in the shader." << std::endl;
            }
            uniformCache[name] = location;
        }
        glUniform2f(location, value.x, value.y);
    }

    void setMat4(const std::string& name, glm::mat4& value) const {
        int location;
        auto it = uniformCache.find(name);
        if(it != uniformCache.end()) {
            location = it->second;
        } else {
            location = glGetUniformLocation(ID, name.c_str());
            if(location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' doesnt exist or is not used in the shader." << std::endl;
            }
            uniformCache[name] = location;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void setVec4(const std::string& name, glm::vec4 value) const {
        int location;
        auto it = uniformCache.find(name);
        if(it != uniformCache.end()) {
            location = it->second;
        } else {
            location = glGetUniformLocation(ID, name.c_str());
            if(location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' doesnt exist or is not used in the shader." << std::endl;
            }
            uniformCache[name] = location;
        }
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    mutable std::unordered_map<std::string, int> uniformCache;
};