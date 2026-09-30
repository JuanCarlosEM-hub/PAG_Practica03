# Prácticas: Programación de Aplicaciones Gráficas (PAG)  
**Profesor:** Ángel Luis García Fernández  
**Alumno:** Juan Carlos Enríquez Muñoz  
**Año:** 2026

---
## Práctica 2: Renderer

###  Resumen de Cambios Realizados

En esta práctica se ha extendido el motor gráfico desacoplando la interfaz de usuario de la lógica de renderizado mediante la integración de la biblioteca **Dear ImGui** y la implementación del patrón obsevador

#### 1. Integración de Dear ImGui
* **Selector de Color de Fondo (`COLOR DE FONDO`):** Se ha añadido una ventana interactiva para modificar el color de fondo a través de un selector de color triangular.
* **Consola de Eventos Interna (`CONSOLA`):** Se ha diseñado una ventan con área de desplazamiento, botón de limpiado y checkbox de *auto-scroll* para mostrar los mensajes por consola de la aplicación directamente en la interfaz.

#### 2. Arquitectura y Patrones de Diseño
* **Patrón Singleton:** Implementado en las clases `PAG::Renderer` y `PAG::GUI` para centralizar la gestión del motor gráfico y de la interfaz respectivamente.
* **Patrón Observador (Observer):**
    * **`PAG::Listener` (Interfaz/Abstracta):** Define el contrato `notificarCambioColor(r, g, b, a)` para los objetos interesados en reaccionar a cambios en la GUI.
    * **`PAG::GUI` (Sujeto/Publicador):** Gestiona la lista de observadores (`std::vector<Listener*>`). Al interactuar con la rueda de color, emite una notificación a los observadores sin tener dependencia directa de OpenGL o `PAG::Renderer`.
    * **`PAG::Renderer` (Observador Concreto):** Implementa `PAG::Listener` y se suscribe a `PAG::GUI` durante su método `inicializar()`. Al recibir la notificación, actualiza el estado de `glClearColor`.

#### 3. Refactorización del Bucle Principal (`main.cpp`)
* Se han eliminado todas las llamadas directas a funciones OpenGL de `main.cpp`.
* El ciclo de renderizado llama ordenadamente a `PAG::Renderer::getInstance().refrescar()` seguido de `PAG::GUI::getInstance().render()`.

---

## Práctica 3: Primer Triángulo

En esta práctica se ha avanzado en el pipeline de renderizado desacoplando los archivos GLSL de C++ y añadiendo soporte para múltiples atributos por vértice (posición y color interpolado).

### 1. Carga Dinámica de Shaders GLSL desde Disco
* **Extracción a Archivos Externos:** El código GLSL se ha extraído a los archivos independientes `pag03-vs.glsl` (Vertex Shader) y `pag03-fs.glsl` (Fragment Shader).
* **Lectura Dinámica (`cargarArchivoTexto`):** Se implementó el método auxiliar privado en `PAG::Renderer` para leer el código fuente de los shaders en tiempo de ejecución utilizando `std::ifstream` y `std::stringstream`.
* **Verificación de Errores (`comprobarComprobacionShader` y `comprobarEnlazadoProgram`):** Se agregaron métodos de control que consultan la bitácora de OpenGL (`glGetShaderInfoLog` y `glGetProgramInfoLog`) y lanzan excepciones `std::runtime_error` en caso de fallos de compilación o enlazado.

### 2. Paso de Atributos e Interpolación de Colores
* **Atributo 0 (Posición):** Declarado en el Vertex Shader mediante `layout (location = 0) in vec3 posicion`.
* **Atributo 1 (Color):** Declarado en el Vertex Shader mediante `layout (location = 1) in vec3 color`.
* **Interpolación en Fragment Shader:** El Vertex Shader envía el color al Fragment Shader mediante una variable de salida (`out vec3 vColor`), permitiendo que el rasterizador interpole los colores a lo largo de la superficie del triángulo creando un gradiente.

### 3. Organización de VBOs (Entrelazados vs No Entrelazados)
Se implementaron dos estrategias para la gestión de buffers de vértices en `PAG::Renderer`:
* **VBOs No Entrelazados (`crearModeloNoEntrelazado`):** Utiliza dos buffers independientes (`idVBO` para posiciones e `idVBO_color` para colores).
* **VBO Entrelazado (`crearModeloEntrelazado`):** Utiliza una estructura C++ (`struct Vertice`) contigua en memoria y un único buffer (`idVBO`), haciendo uso de `sizeof(Vertice)` como *stride* y `offsetof` para calcular el desplazamiento (*offset*) de cada atributo.

---

## Diagrama de Clases UML Actualizado

El siguiente diagrama refleja la estructura de clases del proyecto, incluyendo el patrón observador y los nuevos métodos/atributos añadidos a `PAG::Renderer`:

```mermaid
classDiagram
    class Listener {
        <<interface>>
        +~Listener()
        +notificarCambioColor(r: float, g: float, b: float, a: float)* void
    }

    class Renderer {
        -static instance: Renderer*
        -idVS: GLuint
        -idFS: GLuint
        -idSP: GLuint
        -idVAO: GLuint
        -idVBO: GLuint
        -idIBO: GLuint
        -idVBO_color: GLuint
        -Renderer()
        -cargarArchivoTexto(rutaArchivo: const string&) string
        -comprobarComprobacionShader(shader: GLuint, tipoShader: const string&) void
        -comprobarEnlazadoProgram(program: GLuint) void
        +~Renderer()
        +static getInstance() Renderer&
        +inicializar() void
        +refrescar() void
        +establecerViewport(x: int, y: int, width: int, height: int) void
        +establecerColor(r: float, g: float, b: float, a: float) void
        +obtenerColor(colorActual: float[4]) void
        +mostrarInformacionGL() void
        +notificarCambioColor(r: float, g: float, b: float, a: float) void
        +creaShaderProgram(nombreBase: const string&) void
        +creaModelo() void
        +crearModeloEntrelazado() void
        +crearModeloNoEntrelazado() void
    }

    class GUI {
        -static instance: GUI*
        -mensajes: vector~string~
        -autoScroll: bool
        -colorActual: float[4]
        -listeners: vector~Listener*~
        -GUI()
        -selectorColorTriangular() void
        -consola() void
        -notificarObservadoresColor() void
        +~GUI()
        +static getInstance() GUI&
        +inicializar(window: GLFWwindow*) void
        +render() void
        +finalizar() void
        +procesarBotonRaton(button: int, action: int) void
        +addMensaje(mensaje: const string&) void
        +addListener(listener: Listener*) void
        +removeListener(listener: Listener*) void
    }

    class Main ["main.cpp"] {
        <<file>>
        +main() int
    }

    Listener <|.. Renderer : Implementa
    GUI "1" o-- "*" Listener : listeners
    Main ..> GUI : Utiliza / Inicializa
    Main ..> Renderer : Utiliza / Inicializa