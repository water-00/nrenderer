#pragma once
#ifndef __NR_GL_SHADER_HPP__
#define __NR_GL_SHADER_HPP__

#include <iostream>
#include "geometry/vec.hpp"
#include "glad/glad.h"

namespace NRenderer
{
    using namespace std;
    class GlShader
    {
    public:
        using GlShaderId = unsigned int;
    private:
        GlShaderId shaderId;
        inline
        void checkShaderCompileError(unsigned int shader, string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
    public:
        GlShader(const char* vShaderSource, const char* fShaderSource) {
            unsigned int vShader, fShader;
            vShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vShader, 1, &vShaderSource, NULL);
            glCompileShader(vShader);
            checkShaderCompileError(vShader, "VERTEX");
            fShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fShader, 1, &fShaderSource, NULL);
            glCompileShader(fShader);
            checkShaderCompileError(fShader, "FRAGMENT");
            shaderId = glCreateProgram();
            glAttachShader(shaderId, vShader);
            glAttachShader(shaderId, fShader);
            glLinkProgram(shaderId);
            glDeleteShader(vShader);
            glDeleteShader(fShader);
        }

        void use() {
            glUseProgram(shaderId);
        }
    
        void setMat4x4(const char* name, const Mat4x4& m) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
        }

        void setVec3(const char* name, const Vec3& v) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform3f(loc, v.x, v.y, v.z);
        }

        void setVec4(const char* name, const Vec4& v) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform4f(loc, v.x, v.y, v.z, v.w);
        }
    
        void setInt(const char* name, int i) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform1i(loc, i);
        }

        void setFloat(const char* name, float f) {
            auto loc = glGetUniformLocation(shaderId, name);
            glUniform1f(loc,  f);
        }
    };
}

#endif