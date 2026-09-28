#include <iostream>
// IMPORTANTE: El include de GLAD debe estar siempre ANTES de el de GLFW
#include <random>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "GUI.h"

// - Esta función callback será llamada cuando GLFW produzca algún error
void error_callback ( int errno, const char* desc )
{
    std::string aux (desc);
    std::cout << "Error de GLFW número " << errno << ": " << aux << std::endl;
    // Voy a dejar esta llamada por la consola tradicional por si el error produce un bloqueo
}

// - Esta función callback será llamada cada vez que el área de dibujo
// OpenGL deba ser redibujada.
void window_refresh_callback ( GLFWwindow *window )
{
    //Llamamos a la instancia y refrescamos
    PAG::Renderer::getInstance().refrescar();

    //Dibujamos la interfaz de usuario
    PAG::GUI::getInstance().render();

    glfwSwapBuffers ( window );
    PAG::GUI::getInstance().addMensaje("Refresh callback called");
}

// - Esta función callback será llamada cada vez que se cambie el tamaño
// del área de dibujo OpenGL.
void framebuffer_size_callback ( GLFWwindow *window, int width, int height )
{
    //glViewport ( 0, 0, width, height );
    PAG::Renderer::getInstance().establecerViewport(0,0,width,height);
    PAG::GUI::getInstance().addMensaje("Resize callback called");
}

// - Esta función callback será llamada cada vez que se pulse una tecla
// dirigida al área de dibujo OpenGL.
void key_callback ( GLFWwindow *window, int key, int scancode, int action, int mods )
{
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    PAG::GUI::getInstance().addMensaje("Key callback called");
}

// - Esta función callback será llamada cada vez que se pulse algún botón
// del ratón sobre el área de dibujo OpenGL.
void mouse_button_callback ( GLFWwindow *window, int button, int action, int mods )
{
    if ( action == GLFW_PRESS )
    {
        PAG::GUI::getInstance().addMensaje("Pulsado el boton:" + std::to_string(button));
    }
    else if ( action == GLFW_RELEASE )
    {
        PAG::GUI::getInstance().addMensaje("Soltando el boton:" + std::to_string(button));
    }

    // Notificamo del evento a GUI
    PAG::GUI::getInstance().procesarBotonRaton(button, action);
}

// - Esta función callback será llamada cada vez que se mueva la rueda
// del ratón sobre el área de dibujo OpenGL.
void scroll_callback ( GLFWwindow *window, double xoffset, double yoffset )
{
    PAG::GUI::getInstance().addMensaje("Moviemiento rueda del raton: X = "+std::to_string(xoffset)+" Y = "+std::to_string(yoffset)+"");
    // Arreglo para guardar los componentes (R, G, B, A)
    float colorActual[4];

    PAG::Renderer::getInstance().obtenerColor(colorActual);

    std::random_device rd;
    std::mt19937 gen(rd());

    // Buscamos el valor flotante mínimo justo por encima de 0.0
    double minimo_exclusivo = std::nextafter(0.0, 1.0);

    // std::uniform_real_distribution incluye el límite inferior pero excluye el superior: [min, 1.0)
    std::uniform_real_distribution<double> distrib(minimo_exclusivo, 1.0);

    // Generar el número decimal estrictamente entre 0 y 1
    double valor_sum_nuevo_color = distrib(gen);

    // Aplicamos el nuevo color a cada elemento
    for ( int i = 0; i < 3; i++ )
    {
        // Sin comprobar, modificamos el elemento del array
        // Para que el color se vea distinto, multiplicamos la posicion del elemento por el sentido de rotación. De este modo algunos elementos suman y otros restan
        // Aclarar, que aunque algunos sumen y otros resten, siempre suman o restan los mismo respectivamente el eje de la rueda
        colorActual[i] += valor_sum_nuevo_color * i *yoffset;

        //Comprobamos si se sale de rango por el límite superior
        if ( colorActual[i] > 1.0 ) {
            colorActual[i] = 0; //Lo establecemos a 0 si es inválido
        }else if ( colorActual[i] < 0.0 )
        {
            colorActual[i] = 1.0; // Lo establecemos a 1 si es inválido
        }
    }
    // Aplicamos en el fondo
    PAG::Renderer::getInstance().establecerColor(
        colorActual[0], colorActual[1], colorActual[2], colorActual[3]
    );

}

int main()
{
    std::cout << "Starting Application PAG - Prueba 02" << std::endl;

    // - Este callback hay que registrarlo ANTES de llamar a glfwInit
    glfwSetErrorCallback ( (GLFWerrorfun) error_callback );

    // - Inicializa GLFW. Es un proceso que sólo debe realizarse una vez en la aplicación
    if ( glfwInit () != GLFW_TRUE )
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // - Definimos las características que queremos que tenga el contexto gráfico
    // OpenGL de la ventana que vamos a crear. Por ejemplo, el número de muestras o el
    // modo Core Profile.
    glfwWindowHint ( GLFW_SAMPLES, 4 ); // - Activa antialiasing con 4 muestras.
    glfwWindowHint ( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE ); // - Esta y las 2
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 4 ); // siguientes activan un contexto
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 3 ); // OpenGL Core Profile 4.3.

    // - Definimos el puntero para guardar la dirección de la ventana de la aplicación y
    // la creamos
    GLFWwindow *window;
    // - Tamaño, título de la ventana, en ventana y no en pantalla completa,
    // sin compartir recursos con otras ventanas.
    window = glfwCreateWindow ( 1024, 576, "PAG Introduction", nullptr, nullptr );
    // - Comprobamos si la creación de la ventana ha tenido éxito.
    if ( window == nullptr )
    {
        std::cout << "Failed to open GLFW window" << std::endl;
        glfwTerminate (); // - Liberamos los recursos que ocupaba GLFW.
        return -2;
    }

    // 1. Establecer el contexto OpenGL actual antes de invocar cualquier función OpenGL o cargar GLAD
    glfwMakeContextCurrent ( window );

    // 2. Inicializar GLAD (debe hacerse inmediatamente después de establecer el contexto)
    if ( !gladLoadGLLoader ( (GLADloadproc) glfwGetProcAddress ) )
    {
        std::cout << "GLAD initialization failed" << std::endl;
        glfwDestroyWindow ( window ); // - Liberamos los recursos que ocupaba GLFW.
        window = nullptr;
        glfwTerminate ();
        return -3;
    }

    // Inicializamos la clase GUI pasando el puntero a la ventana GLFW
    PAG::GUI::getInstance().inicializar(window);

    PAG::Renderer::getInstance().inicializar();

    // - Registramos los callbacks que responderán a los eventos principales
    glfwSetWindowRefreshCallback ( window, window_refresh_callback );
    glfwSetFramebufferSizeCallback ( window, framebuffer_size_callback );
    glfwSetKeyCallback ( window, key_callback );
    glfwSetMouseButtonCallback ( window, mouse_button_callback );
    glfwSetScrollCallback ( window, scroll_callback );

    PAG::Renderer::getInstance().mostrarInformacionGL();

    PAG::Renderer::getInstance().creaShaderProgram ();
    PAG::Renderer::getInstance().creaModelo ();

    // - Ciclo de eventos principal de la aplicación (se unifica en un único bucle)
    while ( !glfwWindowShouldClose ( window ) )
    {
        // Limpiamos el buffer con el color actual
        PAG::Renderer::getInstance().refrescar();

        // Dibujamos la interfaz de ImGui encima
        PAG::GUI::getInstance().render();

        // - Intercambia los buffers back y front
        glfwSwapBuffers ( window );

        // - Procesa los eventos pendientes (teclado, ratón, etc.)
        glfwPollEvents ();
    }

    // - Una vez terminado el ciclo de eventos, liberar recursos
    std::cout << "Finishing application pag prueba" << std::endl;
    glfwDestroyWindow ( window ); // - Cerramos y destruimos la ventana de la aplicación.
    window = nullptr;
    glfwTerminate (); // - Liberamos los recursos que ocupaba GLFW.

    return 0;
}