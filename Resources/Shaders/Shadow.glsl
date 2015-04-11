#version 330 core

#ifdef VERTEX
layout(location = VERTEX_POSITION) in vec3 position;
uniform mat4 modelViewProjection;
#endif //VERTEX


#ifdef VERTEX
void main()
{
    gl_Position = modelViewProjection * vec4(position, 1.0);
}
#endif //VERTEX


#ifdef FRAGMENT
void main()
{
}
#endif //FRAGMENT
