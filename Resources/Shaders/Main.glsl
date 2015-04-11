#ifdef VERTEX
#define EXCHANGE out

layout(location = VERTEX_POSITION)  in vec3 position;
layout(location = VERTEX_TEXCOORD)  in vec2 texcoord;
layout(location = VERTEX_NORMAL)    in vec3 normal;
layout(location = VERTEX_TANGENT)   in vec3 tangent;
layout(location = VERTEX_BITANGENT) in vec3 bitangent;
#endif //VERTEX

#ifdef FRAGMENT
#define EXCHANGE in

out vec4 color;
#endif //FRAGMENT


EXCHANGE Interpolation {
    vec2 texcoord;
    vec3 position_worldSpace;

#if defined(DIRECTION_LIGHT) || defined(SPOT_LIGHT)
    vec4 depth_lightSpace;
#endif //DIRECTION_LIGHT || SPOT_LIGHT

    mat3 TBN;
} inter;


uniform struct Transform
{
    mat4 viewProjection;
    mat4 model;
    mat3 normal;

#if defined(DIRECTION_LIGHT) || defined(SPOT_LIGHT)
    mat4 light;
#endif //DIRECTION_LIGHT || SPOT_LIGHT
} transform;


uniform struct Light
{   
    vec3 viewPosition_worldSpace;

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
    vec3 position_worldPosition;
#endif //POINT_LIGHT || SPOT_LIGHT

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shadowDepthBiasAngleCoef;
    float shadowDepthBias;

#ifdef POINT_LIGHT
    samplerCubeShadow depthTexture;
    float far;
    float near;
#endif //POINT_LIGHT

#if defined(DIRECTION_LIGHT) || defined(SPOT_LIGHT)
    sampler2DShadow depthTexture;
#endif //DIRECTION_LIGHT || SPOT_LIGHT

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
    vec3 attenuation;
#endif //POINT_LIGHT || SPOT_LIGHT

#ifdef DIRECTION_LIGHT
    vec3 lightDirection;
#endif //DIRECTION_LIGHT

#ifdef SPOT_LIGHT
    vec3 spotDirection;
    float spotCosCutoff;
#endif //SPOT_LIGHT
} light;


#ifdef FRAGMENT
uniform struct Material
{
    sampler2D diffuseTexture;
    sampler2D normalTexture;
    sampler2D specularTexture;

    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    vec4  emission;
    float shininess;
} material;
#endif //FRAGMENT


#ifdef POINT_LIGHT
float vectorToDepth(vec3 vec)
{
    vec3 absVec = abs(vec);
    float localZ = max(absVec.x, max(absVec.y, absVec.z));

    float depth = (light.far + light.near)
        / (light.far - light.near) - (2 * light.far * light.near)
        / (light.far - light.near)
        / localZ;

    return (depth + 1.0) * 0.5;
}
#endif //POINT_LIGHT


#ifdef VERTEX
void main(void) 
{
    vec4 position_worldSpace = transform.model * vec4(position, 1.0);

    inter.texcoord = texcoord;
    inter.position_worldSpace = vec3(position_worldSpace);

    inter.TBN = transpose(mat3(
        transform.normal * tangent,
        transform.normal * bitangent,
        transform.normal * normal
    ));

#if defined(DIRECTION_LIGHT) || defined(SPOT_LIGHT)
    inter.depth_lightSpace = transform.light * position_worldSpace;
#endif //DIRECTION_LIGHT || SPOT_LIGHT

    gl_Position = transform.viewProjection * position_worldSpace;
}
#endif //VERTEX


#ifdef FRAGMENT
void main(void)
{
    vec3 normal_tangentSpace = normalize(texture2D(material.normalTexture, inter.texcoord).rgb * 2.0 - 1.0);
    vec3 viewDirection_tangentSpace = inter.TBN * normalize(light.viewPosition_worldSpace - inter.position_worldSpace);

#ifdef DIRECTION_LIGHT
    vec3 lightDirection_tangentSpace = inter.TBN * normalize(light.lightDirection);
#endif //DIRECTION_LIGHT

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
    vec3 lightDirection = inter.position_worldSpace - light.position_worldPosition;
    vec3 lightDirection_tangentSpace = inter.TBN * normalize(lightDirection);
#endif //POINT_LIGHT || SPOT_LIGHT

    float normalDotLight = max(dot(normal_tangentSpace, -lightDirection_tangentSpace), 0.0);

    float shadowDepthBias = light.shadowDepthBiasAngleCoef * tan(acos(normalDotLight));
    shadowDepthBias = clamp(shadowDepthBias, 0.0, light.shadowDepthBias);

#ifdef POINT_LIGHT
    float depth = vectorToDepth(lightDirection) - shadowDepthBias;
    float shadow = texture(light.depthTexture, vec4(normalize(lightDirection), depth));
#endif //POINT_LIGHT

#if defined(DIRECTION_LIGHT) || defined(SPOT_LIGHT)
    float shadow = textureProj(light.depthTexture, inter.depth_lightSpace - vec4(0.0, 0.0, shadowDepthBias, 0.0));
#endif //DIRECTION_LIGHT || SPOT_LIGHT

    vec4 diffuseColor = material.diffuse * light.diffuse * normalDotLight;

    vec4 specular = texture(material.specularTexture, inter.texcoord);
    float reflectedLightDotViewPow = pow(dot(reflect(lightDirection_tangentSpace, normal_tangentSpace), viewDirection_tangentSpace), material.shininess);
    reflectedLightDotViewPow = max(reflectedLightDotViewPow, 0.0);

    vec4 specularColor = specular * material.specular * light.specular * reflectedLightDotViewPow;

    color = diffuseColor;
    color += specularColor;

#ifdef SPOT_LIGHT
    float coneCosine = dot(inter.TBN * light.spotDirection, lightDirection_tangentSpace);
    float dif = 1.0 - light.spotCosCutoff;
    float factor = clamp((coneCosine - light.spotCosCutoff) / dif, 0.0, 1.0);

    color *= factor;
#endif //SPOT_LIGHT

    color *= shadow;
    color += material.ambient * light.ambient;

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
    float distance = length(lightDirection);
    float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * distance + light.attenuation[2] * distance * distance);
    color *= attenuation;
#endif //POINT_LIGHT || SPOT_LIGHT

    color += material.emission;

    color *= texture(material.diffuseTexture, inter.texcoord);
}
#endif //FRAGMENT
