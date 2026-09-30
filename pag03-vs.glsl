#version 410

// Atributo 0: Posición (vec3)
layout (location = 0) in vec3 posicion;
// Atributo 1: Color por vértice (vec3)
layout (location = 1) in vec3 color;

// Variable de salida para el Fragment Shader
out vec3 vColor;

void main () {
    vColor = color;
    gl_Position = vec4 (posicion, 1.0);
}