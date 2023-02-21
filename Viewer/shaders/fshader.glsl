#version 330 core

struct Material
{
	sampler2D textureMap;
	// You can add more fields here...
	// Such as:
	//		1. diffuse/specular relections constants
	//		2. specular alpha constant
	//		3. Anything else you find relevant
};

// We set this field's properties from the C++ code
uniform Material material;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 color;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform bool plane;
uniform bool sphere;
uniform bool cylindar;
uniform bool toon_shading;

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 orig_fragPos;
// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	vec3 I = normalize(fragPos - viewPos);
    vec3 R = reflect(I, normalize(fragNormal));
	vec3 textureColor = vec3(texture(material.textureMap, vec2(R)));
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'
	//vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));
	//vec3 normal = vec3(texture(normalMap, fragTexCoords));
    // transform normal vector to range [-1,1]
    //normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
	
	// ambient
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
   // vec3 norm = normalize(normal);
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * diffuseStrength;
    
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    //  frag_color = vec4(result, 1.0);
    // textureColor=textureColor*result;
	frag_color = vec4(textureColor, 1.0);
     if(plane)
		textureColor = vec3(texture(material.textureMap, vec2(orig_fragPos.x, orig_fragPos.y))); // Plane projection 
     if(cylindar)
		textureColor = vec3(texture(material.textureMap, vec2(atan(orig_fragPos.z / orig_fragPos.x), orig_fragPos.y))); // Cylinder projection
	if(sphere)
	{
		  // Sphere projection:
		 float r = sqrt(pow(orig_fragPos.x, 2) + pow(orig_fragPos.y, 2) + pow(orig_fragPos.z, 2));
		 float cos = orig_fragPos.z / r;
		 float t = acos(cos);
		 float fi = atan(orig_fragPos.y / orig_fragPos.x);
		 textureColor = vec3(texture(material.textureMap, vec2(fi, t)));
		
		 frag_color = vec4(textureColor, 1.0);
		 //frag_color = vec4(objectColor,1);
	 }
	
	
	if(toon_shading)
	{
		 for(float i = 0.f ; i <= 1 ; i = i + 0.25f)
				{
					if(diff >= i && diff  <= i+0.25f )
					{
						float newBrightness = i;
						diffuse = lightColor * newBrightness ;
					
					}
					if(spec >= i && spec  <= i+0.25f )
					{
						float newBrightness = i;
						specular = lightColor * newBrightness ;
					}
				
				}
		frag_color = vec4(ambient+diffuse+specular,1);
		}
			
		

  
}
