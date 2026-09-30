#version 410

// Variable de entrada recibida e interpolada desde el Vertex Shader
in vec3 vColor;

out vec4 colorFragmento;

void main () {
    // Se asigna el color interpolado para generar el gradiente
    colorFragmento = vec4 (vColor, 1.0);
}