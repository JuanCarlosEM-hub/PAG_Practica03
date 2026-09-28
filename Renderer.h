/**
* @file Renderer.h
 * @author Juan Carlos Enríquez Muñoz
 *
 * @date 22/09/2026
 *
 * @brief Declaración de la clase Renderer (Observador concreto).
 */
#ifndef PR02_RENDERER_H
#define PR02_RENDERER_H
#include "Listener.h"

namespace PAG
{
    /**
    * @brief Esta clase coordina el renderizado de las escenas OpenGL. Se implementa
    * aplicando el patrón de diseño Singleton. Está pensada para que las funciones callback hagan llamadas a sus métodos
     */
    class Renderer: public Listener //Hereda de la Interfaz Listener
    {
        private:
            static Renderer* instance; ///< Puntero al objeto

            GLuint idVS = 0; ///<  Identificador del vertex shader
            GLuint idFS = 0; ///<  Identificador del fragment shader
            GLuint idSP = 0; ///<  Identificador del shader program
            GLuint idVAO = 0; ///<  Identificador del vertex array object
            GLuint idVBO = 0; ///<  Identificador del vertex buffer object
            GLuint idIBO = 0; ///<  Identificador del index buffer object

            // El constructor es privado para evitar que se cree el objeto desde otros módulos
            Renderer();
        public:
            ~Renderer();
            static Renderer& getInstance();

            void inicializar();
            void refrescar();

            void establecerViewport(int x, int y, int width, int height);

            // Gestión del color de fondo (clear color)
            void establecerColor(float r, float g, float b, float a = 1.0f);
            void obtenerColor(float colorActual[4]);

            void mostrarInformacionGL();

            void notificarCambioColor(float r, float g, float b, float a) override;

            void creaShaderProgram();
            void creaModelo();
    };

};





#endif //PR02_RENDERER_H
