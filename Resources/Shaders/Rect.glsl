#ifdef VERTEX
layout(location = VERTEX_POSITION) in vec2 position;
#endif //VERTEX

#ifdef FRAGMENT
out vec4 color;

uniform vec4 rectColor;
#endif //FRAGMENT

#ifdef VERTEX
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
}
#endif //VERTEX

#ifdef FRAGMENT
void main()
{      
    color = rectColor;
}
#endif //FRAGMENT
