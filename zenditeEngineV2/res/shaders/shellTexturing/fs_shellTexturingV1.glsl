#version 420 core

struct Material
{
    //sampler2D colorTexture;

    //vec3 ambient;
    //vec3 diffuse;
    //vec3 specular;
    float shininess;
};

struct Light
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform vec3 viewPos;
uniform Material material;
uniform Light light;

in vec2 texCoord;
in vec3 localPosition;

in vec3 FragPos;
in vec3 Normal;

in vec4 FragPosLightSpace;

out vec4 FragColor;

//uniform vec4 ourColor;

uniform sampler2D colorTexture;

//uniform sampler2D highTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler2D sandTexture;

uniform sampler2D shadowMap;

uniform float baseNoGrassValue;
uniform float layerHeight;

uniform float shadowIntensity;
//uniform float time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

//This functions code was taken from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 FragPosLightSpace)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow * shadowIntensity;
}


void main()
{
    float gridSize = 400.0; //was 50

    float minRockSlope = 0.7;
    float maxSnowSlope = 0.9;

    float shadow = ShadowCalculation(FragPosLightSpace);

    // Calculate the grid cell coordinates
    vec2 gridPos = floor(texCoord * gridSize);

    float result = random(gridPos);

    vec4 texColor;

    if(layerHeight != 0)
    {
        if (localPosition.y > 14.0 || localPosition.y < 12.0)
        {
            discard;
        }
    }

    //Add Dir Light info here:
    // ambient
    vec3 ambient = light.ambient;// *texture(rockTexture, texCoord).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = light.direction; //is normalized on the CPU side.
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;// *texture(rockTexture, texCoord).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;// * material.specular;

    vec3 lightResult = ambient + (diffuse + specular) * (1.0 - shadow);
    //FragColor = vec4(result, 1.0);


    if (result >= baseNoGrassValue * layerHeight) //baseNoGrassValue = 0.025 (is a good starting value)
    {
        texColor = texture(colorTexture, texCoord);
        texColor.x = texColor.x * lightResult.x;
        texColor.y = texColor.y * lightResult.y;
        texColor.z = texColor.z * lightResult.z;
    }
    else
    {
        //texColor = vec4(0.0, 0.0, 0.0, 1.0);
        discard;
    }
    

    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }

    bool check = false;

    if (layerHeight == 0)
    {
        if (localPosition.y > 14.0 || localPosition.y < 12.0)
        {
            float rock_snow_weight = norm.y;

            rock_snow_weight = max(minRockSlope, rock_snow_weight);
            rock_snow_weight = min(maxSnowSlope, rock_snow_weight);

            rock_snow_weight = rock_snow_weight - minRockSlope;

            rock_snow_weight = rock_snow_weight / (maxSnowSlope - minRockSlope);

            FragColor = mix(texture(rockTexture, texCoord) * vec4(lightResult, 1.0), texture(snowTexture, texCoord) * vec4(lightResult, 1.0), rock_snow_weight);
                
            
            if (localPosition.y < 12.0)
            {

                FragColor = texture(sandTexture, texCoord) * vec4(lightResult, 1.0);
            }
            
            check = true;
        }
    }

    if (check != true)
    {
        
        float intensity = layerHeight * 0.05;
        FragColor = texColor * (min(intensity, 1.0));
    }


    //FragColor = vec4(vec3(result), 1.0);
    
}