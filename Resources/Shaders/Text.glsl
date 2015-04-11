#version 330 core

#ifdef VERTEX
layout(location = VERTEX_POSITION) in vec2 position;
layout(location = VERTEX_COORD) in vec2 coordIn;

#define EXCHANGE out
#endif //VERTEX


#ifdef FRAGMENT
out vec4 color;

#define EXCHANGE in
#endif //FRAGMENT


EXCHANGE vec2 coord;


#ifdef FRAGMENT
uniform sampler2D atlas;
uniform vec4 textColor;
#endif //FRAGMENT


#ifdef VERTEX
void main()
{
    coord = coordIn;
    gl_Position = vec4(position, 0.0, 1.0);
}
#endif //VERTEX


#ifdef FRAGMENT
void main()
{
    float intensity = texture(atlas, coord).r / 65.0 * 255;
    color = textColor * intensity;
}
#endif //FRAGMENT
