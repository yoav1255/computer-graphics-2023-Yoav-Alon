### 1):
The texture is displayed using 2 triangles that strech over the screen in the Renderer::InitOpenglRendering() function.
The data is now loaded to the gpu.

### 2): Completed.

### 3):
Code from the Vertex Shader :
```
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

void main()
{
	// Apply the model transformation to the 'position' and 'normal' properties of the vertex,
	// so the interpolated values of these properties will be available for usi n the fragment shader
	orig_fragPos = vec3(vec4(pos, 1.0f));
	fragPos = vec3(model * vec4(pos, 1.0f));
	fragNormal = mat3(model) * normal;

	// Pass the vertex texture coordinates property as it is. Its interpolated value
	// will be avilable for us in the fragment shader
	fragTexCoords = texCoords;

	// This is an internal OpenGL variable, we must set a value to this variable
	gl_Position = projection * view *  model * vec4(pos, 1.0f);
}
```

Code from Renderer.cpp :
```
colorShader.setUniform("model", currentModel->GetWorldTransform() * currentModel->GetObjectTransform());
colorShader.setUniform("view", camera.GetViewTransformation());
colorShader.setUniform("projection", camera.GetProjectionTransformation());
```
### 4):
Code:
```
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
// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 orig_fragPos;
// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	  frag_color = vec4(color, 1.0);
}
```
Result:

![const_color4](https://user-images.githubusercontent.com/108798956/219660711-99dcb005-b13e-486b-8f78-163c97e69386.PNG)

### 5):
```
colorShader.setUniform("model", currentModel->GetWorldTransform() * currentModel->GetObjectTransform());
colorShader.setUniform("view", camera.GetViewTransformation());
colorShader.setUniform("projection", camera.GetProjectionTransformation());
colorShader.setUniform("material.textureMap", 0);
colorShader.setUniform("objectColor", currentModel->GetColor());
colorShader.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
colorShader.setUniform("lightPos", currLight->translation);
colorShader.setUniform("viewPos", camera.pos);

// Set 'texture1' as the active texture at slot #0
texture1.bind(0);

// Drag our model's faces (triangles) in fill mode
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glBindVertexArray(currentModel->GetVAO());
glDrawArrays(GL_TRIANGLES, 0, currentModel->GetModelVertices().size());
glBindVertexArray(0);

```

https://user-images.githubusercontent.com/108798956/219663644-bcf603a2-92cf-4612-8449-5c9bb3cb6460.mp4

### 6):
Video of appying diffrent lights and view points to the Monster.obj :

https://user-images.githubusercontent.com/108798956/219666047-42577eaf-8908-41b1-881b-bcc1cc6b9ae7.mp4

### 7):

Plane texture projection:
![plane](https://user-images.githubusercontent.com/108798956/219666343-fa9819cf-26aa-4b37-b190-683b40d1347f.PNG)

Sphere texture projection:
![sphere](https://user-images.githubusercontent.com/108798956/219666367-63bbb7d0-b28b-4f48-8bd4-d83dc531cca8.PNG)

Cylinder texture projection:
![cylinder](https://user-images.githubusercontent.com/108798956/219666389-9bd7ce3f-dc53-49d3-9456-b2959e3a2b59.PNG)
