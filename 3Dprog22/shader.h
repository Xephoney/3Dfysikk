#pragma once
#include <QOpenGLFunctions_4_1_Core>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "GL/glew.h" //We use QOpenGLFunctions instead, so no need for Glew (or GLAD)!

// HER KAN VI LEGGE DIV Matrise Uniformer
// * Lagre div. tall og slikt til hver Shader.
// * Lage en Parent-shader som tar av seg det meste og det som er felles
// * Mulig å legge inn projMatrix* og viewMatrix* fra kamera inn i shaderen som blir brukt?
//   Da kan kamera oppdatere sine matriser en gang hver frame, og shaderen tar seg av hvilken
//   MatrixUnifrom som trengs til hvilken shader.

//This class is pretty much a copy of the shader class at
//https://github.com/SonarSystems/Modern-OpenGL-Tutorials/blob/master/%5BLIGHTING%5D/%5B8%5D%20Basic%20Lighting/Shader.h
//which is based on stuff from http://learnopengl.com/ and http://open.gl/.

//must inherit from QOpenGLFunctions_4_1_Core, since we use that instead of glfw/glew/glad
class Shader : protected QOpenGLFunctions_4_1_Core
{
public:
    // Constructor generates the shader on the fly
    Shader(const std::string& shaderPath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    ///Use the current shader
    void use();

    void SetUniform1i(const std::string& name, const int& data);
    void SetUniform1f(const std::string& name, const float& data);
    void SetUniform3f(const std::string& name, const glm::vec3& data);
    void SetUniform4f(const std::string& name, const glm::vec4& data);
    void SetUniformMatrix4fv(const std::string& name, const glm::mat4& matrix);
    ///Returns the program number for this shader
    GLuint getProgram() const;

    int GetUniformLocation (const std::string& name);

private:
    ///The int OpenGL gives as a reference to this shader
    GLuint mProgram;
    std::string m_filePath;
    std::string shaderNavn;
    unsigned int m_RenderID;
    std::unordered_map<std::string,int> m_UniformLocationCache;

    void CompileShader(const std::string& vertexPath, const std::string& fragmentPath);
    ///The logger - Output Log in the application
    class Logger *mLogger{nullptr};
};
