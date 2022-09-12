// Used as a template for creating my singleton class
// https://refactoring.guru/design-patterns/singleton/cpp/example

#pragma once
#include <unordered_map>
#include <shader.h>
#include <glm/glm.hpp>

class ShaderCoordinator
{
public:
    static ShaderCoordinator& getInstance();

    Shader* fetchSkyboxShader()
    {
        return Skybox;
    }
    Shader* fetchShader(const std::string& name) const
    {
        auto iter = m_Shaders.begin();
        iter = m_Shaders.find(name);
        if(iter == m_Shaders.end())
        {
            return Fallback;
        }
        else
        {
            return iter->second;
        }
    }
    Shader* loadShader(const std::string& shaderName)
    {
        auto iter = m_Shaders.find(shaderName);
        if(iter != m_Shaders.end())
           return (*iter).second;
        // Her bare printer vi ut de viktigste shader uniformene for debugging ;)
        m_Shaders[shaderName] = new Shader(shaderName);
        return m_Shaders[shaderName];
    }
    bool updateShaderUniformVec3(const std::string& shaderName, const std::string& uniformName, const glm::vec3& value)
    {
        auto iter = m_Shaders.find(shaderName);
        if(iter == m_Shaders.end())
        {
            qDebug() << "Could not update Shader Uniform (" <<uniformName.c_str() <<")\n"<<shaderName.c_str() << " isn't loaded or doesn't exist\n";
            return false;
        }
        m_Shaders[shaderName]->use();
        m_Shaders[shaderName]->SetUniform3f(uniformName,value);
        return true;
    }
    void operator=(const ShaderCoordinator &) = delete;
    void CameraUpdate(glm::mat4 v, glm::mat4 p)
    {
        for( const auto &a : m_Shaders )
        {
            a.second->use();
            a.second->SetUniformMatrix4fv("view",v);
            a.second->SetUniformMatrix4fv("projection",p);
        }
    }
protected:
    Shader* Fallback {nullptr};
    Shader* Skybox {nullptr};
    std::unordered_map<std::string, Shader*> m_Shaders;
    ShaderCoordinator() { initializeShaders(); };

private:
    ShaderCoordinator ShaderCoordiantor(ShaderCoordinator const&) = delete;
    void initializeShaders()
    {
        Fallback = loadShader ( "plainshader" );
        Skybox = loadShader ( "skybox" );
        loadShader ( "blinn_phongshader" );
        loadShader ( "blinn_phongtextureshader" );
        loadShader ( "unlit_texture" );

        m_Shaders.erase("skybox");
    }

};

inline ShaderCoordinator& ShaderCoordinator::getInstance()
{
    static ShaderCoordinator INSTANCE;
    return INSTANCE;
}

