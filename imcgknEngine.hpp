/*
    This header depends on SDL2, GLAD, GLM and STB_IMAGE. You must install/link these libraries.

    SingleFileOpenGLClass.h
    Copyright (c) 2025 IMCG KN

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <stb_image.h>

#include <SDL/SDL.h>
#undef main
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#define Log(x)\
std::clog << x << '\n';

#define Error(x)\
{\
    std::stringstream sStr;\
    sStr << "Error! << " << x << std::endl;\
    throw std::runtime_error(sStr.str());\
}

namespace imcgkn
{
    struct Vertex
    {
        glm::vec3 aPos;
        glm::vec3 aColor;
        glm::vec3 aNormal;
        glm::vec2 aUV;
    };

    struct Transform
    {
        glm::vec3 Position;
        glm::vec3 Scale;
        glm::vec3 Rotation;
    };

    enum class KeyState
    {
        JustPressed,
        JustReleased,
        Down,
        Released
    };

    enum class ShaderType
    {
        Vert = GL_VERTEX_SHADER,
        Frag = GL_FRAGMENT_SHADER
    };

    enum class RenderMode
    {
        Points = GL_POINTS,
        Lines = GL_LINES,
        Line_Loop = GL_LINE_LOOP,
        Line_Strip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
        Triangle_Fan = GL_TRIANGLE_FAN,
        Triangle_Strip = GL_TRIANGLE_STRIP
    };

    enum class WrapMode
    {
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        Repeat = GL_REPEAT,
        MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    };

    enum class MinFilter
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    enum class MagFilter
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR
    };

    enum class WindowFlags
    {
        Shown = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL,
        Hidden = SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL,
        Borderless = SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL,
        Resizable = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL,
        Minimized = SDL_WINDOW_MINIMIZED | SDL_WINDOW_OPENGL,
        Maximized = SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL,
    };

    enum class BufferUsage
    {
        Empty = 0,
        StaticDraw = GL_STATIC_DRAW,
        StaticCopy = GL_STATIC_COPY,
        StaticRead = GL_STATIC_READ,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicCopy = GL_DYNAMIC_COPY,
        DynamicRead = GL_DYNAMIC_READ
    };

    enum class TextureType
    {
        Texture1D = GL_TEXTURE_1D,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Texture2D = GL_TEXTURE_2D,
        Texture2DArray = GL_TEXTURE_2D_ARRAY,
        Texture3D = GL_TEXTURE_3D,
        TextureCubemap = GL_TEXTURE_CUBE_MAP
    };

    inline WindowFlags operator|(WindowFlags a, WindowFlags b)
    {
        return static_cast<WindowFlags>(
            static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
            );
    }

    class Window
    {
    private:
        SDL_Window* window = nullptr;
        SDL_GLContext glContext;
        int m_Width = 0;
        int m_Height = 0;

        bool m_IsOpen = true;
        bool m_IsResized = false;

        double m_DeltaTime = 0.0;
        double m_LastFrame = 0.0;

        float m_MousePosX = 0.0f;
        float m_MousePosY = 0.0f;
        float m_MouseRelX = 0.0f;
        float m_MouseRelY = 0.0f;
        float m_ScrollDistance = 0.0f;
        float m_ZoomSpeed = 550.0f;

        std::unordered_map<SDL_Scancode, KeyState> m_Keys;
        std::unordered_map<uint8_t, KeyState> m_MouseButtons;

    public:
        Window(int w, int h, const std::string& title, int openglMajorVersion = 4, int openglMinorVersion = 5, WindowFlags flags = WindowFlags::Shown | WindowFlags::Resizable)
        {
            m_Width = w;
            m_Height = h;

            if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
                Error("Failed to initialize SDL2.");

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openglMajorVersion);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openglMinorVersion);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, (uint32_t)flags);
            if (!window)
                Error("Failed to create SDL_window.");

            glContext = SDL_GL_CreateContext(window);

            SDL_GL_MakeCurrent(window, glContext);

            if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
            {
                Error("Failed to load GLAD!");
            }

            SDL_GL_GetDrawableSize(window, &m_Width, &m_Height);
            glViewport(0, 0, m_Width, m_Height);

            m_LastFrame = (double)SDL_GetPerformanceCounter();

            for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
                m_Keys[static_cast<SDL_Scancode>(i)] = KeyState::Released;

            for (int i = 0; i < 5; ++i)
                m_MouseButtons[i] = KeyState::Released;
        }

        ~Window()
        {
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        /// <summary>
        /// Updates delta time variable.
        /// </summary>
        void UpdateDeltaTime()
        {
            double newFrame = (double)SDL_GetPerformanceCounter();
            m_DeltaTime = ((newFrame - m_LastFrame) / (double)SDL_GetPerformanceFrequency());
            m_LastFrame = newFrame;
        }

        /// <summary>
        /// <para>Updates deltaTime and polls events. </para>
        /// <para>ALL FUNCTIONS FOR THIS CLASS MUST BE INBETWEEN [ FirstUpdate(), SecondUpdate() ] FUNCTIONS!</para>
        /// </summary>
        void FirstUpdate()
        {
            SDL_Event ev;

            while (SDL_PollEvent(&ev))
            {
                switch (ev.type)
                {
                case SDL_QUIT:
                    m_IsOpen = false;
                    break;
                case SDL_KEYDOWN:
                    m_Keys[ev.key.keysym.scancode] = KeyState::JustPressed;
                    break;
                case SDL_KEYUP:
                    m_Keys[ev.key.keysym.scancode] = KeyState::JustReleased;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    m_MouseButtons[ev.button.button] = KeyState::JustPressed;
                    break;
                case SDL_MOUSEBUTTONUP:
                    m_MouseButtons[ev.button.button] = KeyState::JustReleased;
                    break;
                case SDL_WINDOWEVENT:
                    if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        m_Width = ev.window.data1;
                        m_Height = ev.window.data2;
                        glViewport(0, 0, m_Width, m_Height);
                        m_IsResized = true;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    m_MouseRelX = static_cast<float>(ev.motion.xrel);
                    m_MouseRelY = static_cast<float>(ev.motion.yrel);
                    m_MousePosX = static_cast<float>(ev.motion.x);
                    m_MousePosY = static_cast<float>(ev.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    if (ev.wheel.preciseY > 0)
                        m_ScrollDistance -= m_ZoomSpeed * static_cast<float>(m_DeltaTime);
                    else if (ev.wheel.preciseY < 0)
                        m_ScrollDistance += m_ZoomSpeed * static_cast<float>(m_DeltaTime);
                    break;
                }
            }
        }

        /// <summary>
        /// <para>Resets keychecks, isResize and relMotion parameters.</para>
        /// <para>ALL FUNCTIONS FOR THIS CLASS MUST BE INBETWEEN [ FirstUpdate(), SecondUpdate() ] FUNCTIONS!</para>
        /// </summary>
        void SecondUpdate()
        {
            for (auto& key : m_Keys)
            {
                if (key.second == KeyState::JustPressed)
                    key.second = KeyState::Down;
                if (key.second == KeyState::JustReleased)
                    key.second = KeyState::Released;
            }

            for (auto& mouseButton : m_MouseButtons)
            {
                if (mouseButton.second == KeyState::JustPressed)
                    mouseButton.second = KeyState::Down;
                if (mouseButton.second == KeyState::JustReleased)
                    mouseButton.second = KeyState::Released;
            }

            m_IsResized = false;
            m_MouseRelX = 0.0f;
            m_MouseRelY = 0.0f;
        }

        /// <summary>
        /// Swaps OpenGL buffer. Call it after all rendering is done!
        /// </summary>
        void SwapBuffer()
        {
            SDL_GL_SwapWindow(window);
        }

        void ClearScreen(unsigned int bitField, float r = 0.05f, float g = 0.05f, float b = 0.05f, float a = 1.0f)
        {
            glClearColor(r, g, b, a);
            glClear(bitField);
        }

        /// <summary>
        /// Closes window.
        /// </summary>
        void CloseWindow()
        {
            m_IsOpen = false;
        }

        /// <summary>
        /// Returns mouse positions!
        /// </summary>
        /// <param name="x">position of cursor on window</param>
        /// <param name="y">position of cursor on window</param>
        void GetMousePos(float* x, float* y) const
        {
            *x = m_MousePosX;
            *y = m_MousePosY;
        }

        /// <summary>
        /// <para>Returns direction which mouse is moving to.</para>
        /// <para>Only works if RelativeMouseMode is set to true! </para>
        /// </summary>
        /// <param name="x">position of cursor on window</param>
        /// <param name="y">position of cursor on window</param>
        void GetRelMousePos(float* relx, float* rely) const
        {
            *relx = m_MouseRelX;
            *rely = m_MouseRelY;
        }

        /// <summary>
        /// Sets relative mouse mose for this window!
        /// </summary>
        /// <param name="v">true or false</param>
        void SetRelativeMouseMode(bool v) const
        {
            SDL_SetRelativeMouseMode((SDL_bool)v);
        }

        /// <summary>
        /// Checks if m_IsOpen is true.
        /// </summary>
        /// <returns>True or false</returns>
        bool IsOpen() const
        {
            return m_IsOpen == true;
        }

        /// <summary>
        /// Checks if window was resized.
        /// </summary>
        /// <returns>True or false</returns>
        bool GetResized() const
        {
            return m_IsResized;
        }

        /// <summary>
        /// Checks if key was just released!
        /// </summary>
        /// <param name="key">Scancode for key</param>
        /// <returns>True or false</returns>
        bool CheckKeyUp(SDL_Scancode key)
        {
            return m_Keys[key] == KeyState::JustReleased;
        }

        /// <summary>
        /// Checks if key was held!
        /// </summary>
        /// <param name="key">Scancode for key</param>
        /// <returns>True or false</returns>
        bool CheckKeyDown(SDL_Scancode key)
        {
            return m_Keys[key] == KeyState::Down;
        }

        /// <summary>
        /// Checks if mouse button was just released!
        /// </summary>
        /// <param name="key">Scancode for key</param>
        /// <returns>True or false</returns>
        bool CheckMouseButtonUp(uint8_t key)
        {
            return m_MouseButtons[key] == KeyState::JustReleased;
        }

        /// <summary>
        /// Checks if mouse button was held!
        /// </summary>
        /// <param name="key">Scancode for key</param>
        /// <returns>True or false</returns>
        bool CheckMouseButtonDown(uint8_t key)
        {
            return m_MouseButtons[key] == KeyState::Down;
        }

        float GetDeltaTime() const
        {
            return static_cast<float>(m_DeltaTime);
        }

        float GetScrollDistance() const
        {
            return static_cast<float>(m_ScrollDistance);
        }

        /// <returns>viewport width</returns>
        int GetWidth() const
        {
            return m_Width;
        }

        /// <returns>viewport height</returns>
        int GetHeight() const
        {
            return m_Height;
        }
    };

    class Shader
    {
    private:
        unsigned int m_ID = 0;

        std::map<std::string, int> m_UniformLocations;

        enum class ShaderType
        {
            Vert,
            Frag,
            Geom
        };

    public:
        /// <summary>
        /// Creates OpenGL Shader.
        /// </summary>
        /// <param name="vertexShaderPath">Path to vertex shader</param>
        /// <param name="fragmentShaderPath">Path to fragment shader</param>
        /// <param name="geometryShaderPath">Path to geometry shader(Can be empty)</param>
        Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath = "")
        {
            unsigned int vertex = 0, fragment = 0, geometry = 0;
            std::string vertexStringSource = GetShaderSource(vertexShaderPath);
            std::string fragmentStringSource = GetShaderSource(fragmentShaderPath);
            std::string geometryStringSource = GetShaderSource(geometryShaderPath);

            const char* vShaderSource = vertexStringSource.c_str();
            const char* fShaderSource = fragmentStringSource.c_str();
            const char* gShaderSource = geometryStringSource.c_str();

            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderSource, nullptr);
            CompileShader(vertex, ShaderType::Vert);

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderSource, nullptr);
            CompileShader(fragment, ShaderType::Frag);

            m_ID = glCreateProgram();

            if (!geometryStringSource.empty())
            {
                std::cout << "NOT EMPTY!\n";
                geometry = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(geometry, 1, &gShaderSource, nullptr);
                CompileShader(geometry, ShaderType::Geom);
                glAttachShader(m_ID, geometry);
            }

            glAttachShader(m_ID, vertex);
            glAttachShader(m_ID, fragment);

            glLinkProgram(m_ID);

            if (!geometryStringSource.empty())
            {
                glDeleteShader(geometry);
            }

            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }

        ~Shader()
        {
            glDeleteProgram(m_ID);
        }

        void Use()
        {
            glUseProgram(m_ID);
        }

        void Unuse()
        {
            glUseProgram(0);
        }

        /// <summary>
        /// Sets Matrix 4x4 to this shader.
        /// </summary>
        /// <param name="name">Name of matrix in shader</param>
        /// <param name="count">Count</param>
        /// <param name="transpose">is transpose?</param>
        /// <param name="v">Value</param>
        void SetMat4(const std::string& name, int count, bool transpose, const glm::mat4& v)
        {
            Use();
            glUniformMatrix4fv(GetUniformLocation(name), count, transpose, glm::value_ptr(v));
        }

        /// <summary>
        /// Sets Vector4 to this shader.
        /// </summary>
        /// <param name="name">Name of vector in shader</param>
        /// <param name="count">Count</param>
        /// <param name="v">Value</param>
        void SetVec4(const std::string& name, int count, const glm::vec4& v)
        {
            Use();
            glUniform4fv(GetUniformLocation(name), count, glm::value_ptr(v));
        }

        /// <summary>
        /// Sets Vector3 to this shader.
        /// </summary>
        /// <param name="name">Name of vector in shader</param>
        /// <param name="count">Count</param>
        /// <param name="v">Value</param>
        void SetVec3(const std::string& name, int count, const glm::vec3& v)
        {
            Use();
            glUniform3fv(GetUniformLocation(name), count, glm::value_ptr(v));
        }

        /// <summary>
        /// Sets Vector2 to this shader.
        /// </summary>
        /// <param name="name">Name of vector in shader</param>
        /// <param name="count">Count</param>
        /// <param name="v">Value</param>
        void SetVec2(const std::string& name, int count, const glm::vec2& v)
        {
            Use();
            glUniform2fv(GetUniformLocation(name), count, glm::value_ptr(v));
        }

        /// <summary>
        /// Sets Float to this shader.
        /// </summary>
        /// <param name="name">Name of float in shader</param>
        /// <param name="v">Value</param>
        void SetFloat(const std::string& name, float v)
        {
            Use();
            glUniform1f(GetUniformLocation(name), v);
        }

        /// <summary>
        /// Sets Int to this shader.
        /// </summary>
        /// <param name="name">Name of int in shader</param>
        /// <param name="v">Value</param>
        void SetInt(const std::string& name, int v)
        {
            Use();
            glUniform1i(GetUniformLocation(name), v);
        }

        /// <summary>
        /// Sets Bool to this shader.
        /// </summary>
        /// <param name="name">Name of bool in shader</param>
        /// <param name="v">Value</param>
        void SetBool(const std::string& name, bool v)
        {
            Use();
            glUniform1i(GetUniformLocation(name), v);
        }

    private:
        int GetUniformLocation(const std::string& name)
        {
            auto uniformIter = m_UniformLocations.find(name);

            int location = 0;

            if (uniformIter != m_UniformLocations.end())
            {
                location = uniformIter->second;
            }
            else
            {
                location = glGetUniformLocation(m_ID, name.c_str());
                m_UniformLocations.insert({ name, location });
            }

            return location;
        }

        static std::string GetShaderSource(const std::string& path)
        {
            std::string source;

            std::ifstream file(path);

            if (file.is_open())
            {
                std::stringstream sStr;
                sStr << file.rdbuf();

                source = sStr.str();

                return source;
            }
            else
            {
                Log("Warning! << Failed to open file from path: " << path);
                return "";
            }
        }

        void CompileShader(unsigned int shader, ShaderType typeShader)
        {
            glCompileShader(shader);

            std::string shaderStringType =
                (typeShader == ShaderType::Vert) ? "Vertex" :
                (typeShader == ShaderType::Frag) ? "Fragment" :
                (typeShader == ShaderType::Geom) ? "Geometry" :
                "";

            int success;

            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                char infoLog[1024];
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                Log("Warning! << Failed to compile " << shaderStringType << " shader.\nInfoLog: \n" << infoLog << '\n');
            }
            else
            {
                Log("Info! << Succesfully compiled " << shaderStringType << " shader.");
            }
        }
    };

    class Texture
    {
    private:
        unsigned int m_ID = 0;
        int m_Width = 0;
        int m_Height = 0;
        int m_Channels = 0;
        TextureType m_Type{};

    public:
        /// <param name="path">path to texture file.</param>
        /// <param name="type">See TextureType struct.</param>
        /// <param name="flipY">should flip texture vertically?</param>
        Texture(const std::string& path, TextureType type, bool flipY, WrapMode wrapS, WrapMode wrapT, WrapMode wrapR, MinFilter minFilter, MagFilter magFilter)
            : m_Type(type)
        {
            glGenTextures(1, &m_ID);
            glBindTexture((unsigned int)m_Type, m_ID);

            glTexParameteri((unsigned int)m_Type, GL_TEXTURE_WRAP_S, (int)wrapS);
            glTexParameteri((unsigned int)m_Type, GL_TEXTURE_WRAP_T, (int)wrapT);
            glTexParameteri((unsigned int)m_Type, GL_TEXTURE_WRAP_R, (int)wrapR);
            glTexParameteri((unsigned int)m_Type, GL_TEXTURE_MIN_FILTER, (int)minFilter);
            glTexParameteri((unsigned int)m_Type, GL_TEXTURE_MAG_FILTER, (int)magFilter);

            stbi_set_flip_vertically_on_load(flipY);

            unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
            if (data)
            {
                unsigned int format = GL_RGB;
                if (m_Channels == 1) format = GL_RED;
                else if (m_Channels == 2) format = GL_RG;
                else if (m_Channels == 3) format = GL_RGB;
                else if (m_Channels == 4) format = GL_RGBA;
                glTexImage2D((unsigned int)m_Type, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap((unsigned int)m_Type);
            }
            else
            {
                Log("Warning! << Failed to load texture from path: " << path);
            }
            stbi_image_free(data);
        }

        ~Texture()
        {
            glDeleteTextures(1, &m_ID);
        }

        Texture(Texture&& other) noexcept
            : m_ID(other.m_ID)
        {
            other.m_ID = 0;
        }

        Texture& operator=(Texture&& other) noexcept
        {
            if (this != &other)
            {
                glDeleteTextures(1, &m_ID);
                m_ID = other.m_ID;
                other.m_ID = 0;
            }
            return *this;
        }

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Bind(unsigned int slot = 0) const
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture((unsigned int)m_Type, m_ID);
        }

        void Unbind() const
        {
            glBindTexture((unsigned int)m_Type, 0);
        }
    };

    class VertexBufferObject
    {
    private:
        unsigned int m_ID;

        BufferUsage m_Usage;

        size_t m_Size = 0;
        size_t m_VertexCount = 0;

    public:
        VertexBufferObject(const std::vector<Vertex>& vertices, BufferUsage _usage) : m_ID(0)
        {
            glGenBuffers(1, &m_ID);

            m_Usage = _usage;
            m_Size = vertices.size() * sizeof(Vertex);
            m_VertexCount = vertices.size();

            glBindBuffer(GL_ARRAY_BUFFER, m_ID);
            glBufferData(GL_ARRAY_BUFFER, m_Size, vertices.data(), (unsigned int)m_Usage);
        }

        VertexBufferObject() : m_ID(0)
        {
            glGenBuffers(1, &m_ID);
            m_Usage = BufferUsage::Empty;
            m_Size = 0;
        }

        ~VertexBufferObject()
        {
            glDeleteBuffers(1, &m_ID);
        }

        VertexBufferObject(VertexBufferObject&& other) noexcept
            : m_ID(other.m_ID), m_Usage(other.m_Usage)
        {
            other.m_ID = 0;
            other.m_Usage = BufferUsage::Empty;
        }

        VertexBufferObject& operator=(VertexBufferObject&& other) noexcept
        {
            if (this != &other)
            {
                glDeleteBuffers(1, &m_ID);
                m_VertexCount = other.m_VertexCount;
                m_Usage = other.m_Usage;
                m_ID = other.m_ID;
                other.m_ID = 0;
            }
            return *this;
        }

        VertexBufferObject(const VertexBufferObject&) = delete;
        VertexBufferObject& operator=(const VertexBufferObject&) = delete;
       
        void UpdateVBO(const std::vector<Vertex>& vertices)
        {
            Use();
            size_t newSize = vertices.size() * sizeof(Vertex);
            m_VertexCount = vertices.size();

            if (newSize == m_Size)
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, newSize, vertices.data());
            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, newSize, vertices.data(), (unsigned int)m_Usage);
                m_Size = newSize;
            }
        }

        void Use() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        }

        void Unuse() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        size_t GetVertexCount() const
        {
            return m_VertexCount;
        }
    };

    class ElementBufferObject
    {
    private:
        unsigned int m_ID;

        BufferUsage m_Usage;

        size_t m_Size = 0;
        size_t m_IndexCount = 0;

    public:
        ElementBufferObject(const std::vector<unsigned int>& indices, BufferUsage _usage) : m_ID(0)
        {
            glGenBuffers(1, &m_ID);

            m_Usage = _usage;
            m_Size = indices.size() * sizeof(unsigned int);
            m_IndexCount = indices.size();

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, indices.data(), (unsigned int)m_Usage);
        }

        ElementBufferObject() : m_ID(0)
        {
            glGenBuffers(1, &m_ID);
            m_Usage = BufferUsage::Empty;
            m_Size = 0;
        }

        ~ElementBufferObject()
        {
            glDeleteBuffers(1, &m_ID);
        }

        ElementBufferObject(ElementBufferObject&& other) noexcept
            : m_ID(other.m_ID), m_Usage(other.m_Usage)
        {
            other.m_ID = 0;
            other.m_Usage = BufferUsage::Empty;
        }

        ElementBufferObject& operator=(ElementBufferObject&& other) noexcept
        {
            if (this != &other)
            {
                glDeleteBuffers(1, &m_ID);
                m_IndexCount = other.m_IndexCount;
                m_Usage = other.m_Usage;
                m_ID = other.m_ID;
                other.m_ID = 0;
            }
            return *this;
        }

        ElementBufferObject(const ElementBufferObject&) = delete;
        ElementBufferObject& operator=(const ElementBufferObject&) = delete;

        void UpdateVBO(const std::vector<unsigned int>& indices)
        {
            Use();
            size_t newSize = indices.size() * sizeof(unsigned int);
            m_IndexCount = indices.size();

            if (newSize == m_Size)
            {
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, newSize, indices.data());
            }
            else
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, indices.data(), (unsigned int)m_Usage);
                m_Size = newSize;
            }
        }

        void Use() const
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
        }

        void Unuse() const
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        size_t GetIndexCount() const
        {
            return m_IndexCount;
        }
    };

    class VertexArrayObject
    {
    private:
        unsigned int m_ID;

    public:
        VertexArrayObject() : m_ID(0)
        {
            glGenVertexArrays(1, &m_ID);
        }

        ~VertexArrayObject()
        {
            glDeleteVertexArrays(1, &m_ID);
        }

        VertexArrayObject(const VertexArrayObject&) = delete;
        VertexArrayObject& operator=(const VertexArrayObject&) = delete;

        VertexArrayObject(VertexArrayObject&& other) noexcept
            : m_ID(other.m_ID)
        {
            other.m_ID = 0;
        }

        VertexArrayObject& operator=(VertexArrayObject&& other) noexcept
        {
            if (this != &other)
            {
                glDeleteVertexArrays(1, &m_ID);
                m_ID = other.m_ID;
                other.m_ID = 0;
            }
            return *this;
        }

        void LinkAttrib(VertexBufferObject* vbo, int index, int size, unsigned int type, bool normalized, int stride, const void* pointer)
        {
            Use();
            vbo->Use();
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        }

        void Use() const
        {
            glBindVertexArray(m_ID);
        }

        void Unuse() const
        {
            glBindVertexArray(0);
        }
    };

    class Renderable
    {
    private:
        std::unique_ptr<VertexArrayObject> m_VAO;
        std::unique_ptr<VertexBufferObject> m_VBO;
        std::unique_ptr<ElementBufferObject> m_EBO;

        std::vector<Vertex> m_Vertices;
        std::vector<unsigned int> m_Indices;

    public:
        Renderable()
        {
            m_VAO = std::make_unique<VertexArrayObject>();
        }

        Renderable(const std::vector<Vertex>& vertices, BufferUsage vboUsage)
            : m_Vertices(vertices)
        {
            m_VAO = std::make_unique<VertexArrayObject>();
            m_VBO = std::make_unique<VertexBufferObject>(m_Vertices, vboUsage);

            m_VAO->Use();
            m_VBO->Use();

            m_VAO->LinkAttrib(m_VBO.get(), 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aPos));
            m_VAO->LinkAttrib(m_VBO.get(), 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aColor));
            m_VAO->LinkAttrib(m_VBO.get(), 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aNormal));
            m_VAO->LinkAttrib(m_VBO.get(), 3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aUV));

            m_VAO->Unuse();
        }

        Renderable(const std::vector<Vertex>& vertices, BufferUsage vboUsage, const std::vector<unsigned int>& indices, BufferUsage eboUsage)
            : m_Vertices(vertices), m_Indices(indices)
        {
            m_VAO = std::make_unique<VertexArrayObject>();
            m_VBO = std::make_unique<VertexBufferObject>(m_Vertices, vboUsage);

            m_VAO->Use();

            if (indices.size() > 0)
            {
                m_EBO = std::make_unique<ElementBufferObject>(m_Indices, eboUsage);
                m_EBO->Use();
            }

            m_VBO->Use();

            m_VAO->LinkAttrib(m_VBO.get(), 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aPos));
            m_VAO->LinkAttrib(m_VBO.get(), 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aColor));
            m_VAO->LinkAttrib(m_VBO.get(), 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aNormal));
            m_VAO->LinkAttrib(m_VBO.get(), 3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, aUV));

            m_VAO->Unuse();
        }

        Renderable(Renderable&& other) noexcept
            : m_VAO(std::move(other.m_VAO)),
              m_VBO(std::move(other.m_VBO)),
              m_EBO(std::move(other.m_EBO)),
              m_Vertices(other.m_Vertices),
              m_Indices(other.m_Indices)
        {
        }

        Renderable& operator=(Renderable&& other) noexcept
        {
            if (this != &other)
            {
                m_VAO = std::move(other.m_VAO);
                m_VBO = std::move(other.m_VBO);
                m_EBO = std::move(other.m_EBO);
                m_Vertices = std::move(other.m_Vertices);
                m_Indices = std::move(other.m_Indices);
            }
            return *this;
        }

        Renderable(const Renderable&) = delete;
        Renderable& operator=(const Renderable&) = delete;

        VertexArrayObject& GetVAO()
        {
            return *m_VAO.get();
        }

        VertexBufferObject& GetVBO()
        {
            return *m_VBO.get();
        }

        ElementBufferObject* GetEBO()
        {
            return m_EBO.get();
        }
    };

    class GameObject
    {
    private:
        Transform m_Transform;
        std::shared_ptr<Renderable> m_Renderable;
        std::shared_ptr<Texture> m_Texture;

    public:
        GameObject(const Transform& transform)
            : m_Transform(transform)
        {
        }

        GameObject(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation)
            : m_Transform({ position, scale, rotation })
        {

        }

        GameObject(float xpos, float ypos, float zpos, float xscale, float yscale, float zscale, float xrot, float yrot, float zrot)
            : m_Transform({ { xpos, ypos, zpos }, { xscale, yscale, zscale }, { xrot, yrot, zrot } })
        {

        }

        /// <summary>
        /// See Renderable class arguments!
        /// </summary>
        template<typename... Args>
        void CreateRenderable(Args&& ...args)
        {
            m_Renderable = std::make_shared<Renderable>(std::forward<Args>(args)...);
        }

        /// <param name="renderable">Is of type shared_ptr!</param>
        void SetRenderable(std::shared_ptr<Renderable> renderable)
        {
            m_Renderable = std::move(renderable);
        }

        /// <summary>
        /// See Texture class arguments!
        /// </summary>
        template<typename... Args>
        void CreateTexture(Args&& ...args)
        {
            m_Texture = std::make_shared<Texture>(std::forward<Args>(args)...);
        }

        /// <param name="texture">Is of type shared_ptr!</param>
        void SetTexture(std::shared_ptr<Texture> texture)
        {
            m_Texture = std::move(texture);
        }

        /// <param name="shader">pointer to existing shader!</param>
        /// <param name="modelName">Name of mat4 model parameter in your shader.</param>
        /// <param name="sampler2DName">Name of sampler2D in your shader. (Texture)</param>
        /// <param name="renderMode">See the struct RenderMode in this header file.</param>
        void Render(Shader* shader, const std::string& modelName, const std::string& sampler2DName, RenderMode renderMode)
        {
            if (m_Renderable == nullptr || shader == nullptr)
                return;

            shader->Use();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, m_Transform.Position);
            model = glm::scale(model, m_Transform.Scale);

            model = glm::rotate(model, glm::radians(m_Transform.Rotation.z), glm::vec3(0, 0, 1));
            model = glm::rotate(model, glm::radians(m_Transform.Rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(m_Transform.Rotation.x), glm::vec3(1, 0, 0));

            shader->SetMat4(modelName, 1, GL_FALSE, model);

            if (m_Texture != nullptr)
            {
                m_Texture->Bind(0);
                shader->SetInt(sampler2DName, 0);
            }

            m_Renderable->GetVAO().Use();

            if (m_Renderable->GetEBO()->GetIndexCount() > 0 && m_Renderable->GetEBO() != nullptr)
            {
                glDrawElements((int)renderMode, m_Renderable->GetEBO()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            }
            else
            {
                glDrawArrays((int)renderMode, 0, m_Renderable->GetVBO().GetVertexCount());
            }

            m_Renderable->GetVAO().Unuse();

            if (m_Texture != nullptr)
            {
                m_Texture->Unbind();
            }

            shader->Unuse();
        }
    };
}