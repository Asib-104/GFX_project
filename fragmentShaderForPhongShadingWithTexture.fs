#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
    vec3 emissive;
};

struct DirectionalLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float k_c;
    float k_l;
    float k_q;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float k_c;
    float k_l;
    float k_q;
};

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 2

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;

// function prototypes
vec3 CalcDirectionalLight(Material material, DirectionalLight light, vec3 N, vec3 V);
vec3 CalcPointLight(Material material, PointLight light, vec3 N, vec3 fragPos, vec3 V);
vec3 CalcSpotLight(Material material, SpotLight light, vec3 N, vec3 fragPos, vec3 V);

void main()
{
    // properties
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // directional lighting
    result += CalcDirectionalLight(material, directionalLight, N, V);
    
    // point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(material, pointLights[i], N, FragPos, V);
    
    // spot lights
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(material, spotLights[i], N, FragPos, V);
    
    // emissive
    result += material.emissive;
      
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirectionalLight(Material material, DirectionalLight light, vec3 N, vec3 V)
{
    vec3 L = normalize(-light.direction);
    vec3 R = reflect(-L, N);
    
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
    vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = vec3(texture(material.specular, TexCoords)) * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(Material material, PointLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);
    
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));
    
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
    vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = vec3(texture(material.specular, TexCoords)) * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(Material material, SpotLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);
    
    // spotlight intensity
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // attenuation
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));
    
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
    vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = vec3(texture(material.specular, TexCoords)) * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;
    
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}
