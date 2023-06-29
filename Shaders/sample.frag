// point light implementation

#version 330 core

// diffuse
uniform vec3 lightPos;
uniform vec3 lightColor;

// ambient
uniform float ambientStr;
uniform vec3 ambientColor;

// specular
uniform vec3 camPos;
uniform float specStr;
uniform float specPhong;

// point light
uniform float pointLightRadius;

// texture
uniform vec3 newColor;
uniform sampler2D tex0;

// other variables
in vec3 normCoord;
in vec3 fragPos;
in vec2 texCoord;
out vec4 FragColor;


void main()
{
	// Get lightDir
	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPos - fragPos);

	// Compute diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Compute ambience
	vec3 ambientCol = ambientColor * ambientStr;

	// Compute specularity
	vec3 viewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	vec3 specColor = spec * specStr * lightColor;

	// Compute point light
	float distanceToLight = length(lightPos - fragPos);
	float distanceOverRadius = distanceToLight / pointLightRadius;					
	float pointLightAttenuation = 1 / (1 + pow(7 * distanceOverRadius, 2));			// uses the same formula from Unity's rendering pipeline
																					// inverse-square law has been modified since point lights should also consider how light is affected by its radius
																					// since distance can also be 0, we add 1 to denominator to avoid division by 0

	// Apply all
	vec3 overallLightValue = pointLightAttenuation * (specColor + diffuse + ambientCol);
	FragColor = vec4(overallLightValue, 1.0) * texture(tex0, texCoord);
}