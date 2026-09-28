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
    Renderer::~Renderer() = default;

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

}