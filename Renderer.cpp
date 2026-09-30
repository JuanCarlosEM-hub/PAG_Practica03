/**
 * @file Renderer.cpp
 * @author Juan Carlos Enríquez Muñoz
 *
 * @date 22/09/2026
 *
 * @brief Implementación de la clase Renderer
 */

#include <glad/glad.h>

#include "Renderer.h"
#include "GUI.h"

#include <iostream>


namespace PAG {

    Renderer* Renderer::instance = nullptr;

    /**
     * @brief Constructor por defecto
     */
    Renderer::Renderer()= default;

    /**
     * @brief Destructor por defecto
     */
    Renderer::~Renderer()
    {
        if ( idVS != 0 )
        { glDeleteShader ( idVS );
        }
        if ( idFS != 0 )
        { glDeleteShader ( idFS );
        }
        if ( idSP != 0 )
        { glDeleteProgram ( idSP );
        }
        if ( idVBO != 0 )
        { glDeleteBuffers ( 1, &idVBO );
        }
        if ( idIBO != 0 )
        { glDeleteBuffers ( 1, &idIBO );
        }
        if ( idVAO != 0 )
        { glDeleteVertexArrays ( 1, &idVAO );
        }
    }

    /**
     * @brief Consulta del objeto único de la clase
     * @return Referencia a la instancia única de Renderer
     */
    Renderer& Renderer::getInstance() {
        if (!Renderer::instance) {//Inicialización perezosa
            instance = new Renderer();
        }
        return *instance;
    }

    /**
     * @brief Configura el estado inicial de OpenGL (color de fondo por defecto y test de profundidad) y oregistramos el observador.
     */
    void Renderer::inicializar() {
        //Añadimos el escuchador a la lista de GUI
        PAG::GUI::getInstance().addListener(this);
        // Configuración inicial del estado de OpenGL
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glEnable ( GL_DEPTH_TEST );
        glEnable ( GL_MULTISAMPLE );

    }

    /**
     * @brief Realiza el borrado de los buffers de color y profundidad en cada fotograma.
     */
    void Renderer::refrescar() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // refrescamos el buffer

        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
        glUseProgram ( idSP );
        glBindVertexArray ( idVAO );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO );
        glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );

    }

    /**
     * @brief Establece el área de dibujo en la ventana de OpenGL.
     * @param x Coordenada X de la esquina inferior izquierda.
     * @param y Coordenada Y de la esquina inferior izquierda.
     * @param width Ancho del viewport en píxeles.
     * @param height Alto del viewport en píxeles.
     */
    void Renderer::establecerViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    /**
     * @brief Modifica el color (glClearColor) de la escena.
     * @param r Componente Roja [0.0f, 1.0f].
     * @param g Componente Verde [0.0f, 1.0f].
     * @param b Componente Azul [0.0f, 1.0f].
     * @param a Componente Alfa (Transparencia) [0.0f, 1.0f]. Por defecto es 1.0f.
     */
    void Renderer::establecerColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    /**
     * @brief Consulta el color configurado actualmente en OpenGL.
     * @param[out] colorActual Array flotante de 4 elementos donde se almacenarán las componentes (R, G, B, A).
     */
    void Renderer::obtenerColor(float colorActual[4]) {
        glGetFloatv(GL_COLOR_CLEAR_VALUE, colorActual);
    }

    /**
     * @brief Muestra en la consola estándar la información técnica del controlador gráfico y versión de OpenGL.
     */
    void Renderer::mostrarInformacionGL() {
        std::cout << "Renderizador: " << glGetString(GL_RENDERER) << std::endl
                  << "Proveedor:    " << glGetString(GL_VENDOR) << std::endl
                  << "Versión GL:   " << glGetString(GL_VERSION) << std::endl
                  << "GLSL:         " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    }

    /**
     * @brief Implementación del callback del patrón Observador.
     * Recibe la notificación cuando se cambia el color desde la GUI.
     */
    void Renderer::notificarCambioColor(float r, float g, float b, float a) {
        // Aplicamos el cambio de color al recibir la notificación
        establecerColor(r, g, b, a);
    }

    /**
    * Método para crear, compilar y enlazar el shader program
    * @note No se incluye ninguna comprobación de errores
    */
    void PAG::Renderer::creaShaderProgram( )
    {
        std::string miVertexShader =
        "#version 410\n"
        "layout (location = 0) in vec3 posicion;\n"
        "void main ()\n"
        "{ gl_Position = vec4 ( posicion, 1 );\n"
        "}\n";
        std::string miFragmentShader =
            "#version 410\n"
            "out vec4 colorFragmento;\n"
            "void main ()\n"
            "{ colorFragmento = vec4 ( 1.0, .4, .2, 1.0 );\n"
            "}\n";

        idVS = glCreateShader ( GL_VERTEX_SHADER );
        const GLchar* fuenteVS = miVertexShader.c_str ();
        glShaderSource ( idVS, 1, &fuenteVS, nullptr );
        glCompileShader ( idVS );

        idFS = glCreateShader ( GL_FRAGMENT_SHADER );
        const GLchar* fuenteFS = miFragmentShader.c_str ();
        glShaderSource ( idFS, 1, &fuenteFS, nullptr );
        glCompileShader ( idFS );

        idSP = glCreateProgram ();
        glAttachShader ( idSP, idVS );
        glAttachShader ( idSP, idFS );
        glLinkProgram ( idSP );
    }

    /**
    * Método para crear el VAO para el modelo a renderizar
    * @note No se incluye ninguna comprobación de errores
    */
    void PAG::Renderer::creaModelo ( )
    {
        GLfloat vertices[] = { -.5, -.5, 0,
                                .5, -.5, 0,
                                .0, .5, 0 };
        GLuint indices[] = { 0, 1, 2 };
        glGenVertexArrays ( 1, &idVAO );
        glBindVertexArray ( idVAO );

        glGenBuffers ( 1, &idVBO );
        glBindBuffer ( GL_ARRAY_BUFFER, idVBO );
        glBufferData ( GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW );
        glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr );
        glEnableVertexAttribArray ( 0 );
        glGenBuffers ( 1, &idIBO );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO );
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLuint), indices, GL_STATIC_DRAW );
    }

    /**
     * @brief Comprueba si hubo errores durante la compilación de un Shader.
     * @param tipoShader std::string&
     * @param shader GLuit
     * @throws std::runtime_error si falla la compilación.
     */
    void Renderer::comprobarComprobacionShader(GLuint shader, const std::string& tipoShader) {
        GLint compilado = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compilado);

        if (compilado == GL_FALSE) {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::string logMsg(logLength, '\0');
            glGetShaderInfoLog(shader, logLength, &logLength, &logMsg[0]);

            throw std::runtime_error("Error al compilar " + tipoShader + ":\n" + logMsg);
        }
    }

    /**
     * @brief Comprueba si hubo errores durante el enlazado del Shader Program.
     * @param program GLuit
     * @throws std::runtime_error si falla el enlazado.
     */
    void Renderer::comprobarEnlazadoProgram(GLuint program) {
        GLint enlazado = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &enlazado);

        if (enlazado == GL_FALSE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            std::string logMsg(logLength, '\0');
            glGetProgramInfoLog(program, logLength, &logLength, &logMsg[0]);

            throw std::runtime_error("Error al enlazar el Shader Program:\n" + logMsg);
        }
    }

}