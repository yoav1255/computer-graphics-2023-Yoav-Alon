#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer.h"
#include "InitShader.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenglRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer;
	delete[] z_buffer;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;
	
	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}
void Renderer::DrawLineReversedAxis(int x1, int y1, int x2, int y2, const glm::vec3& color, bool fillTriangle, glm::vec3& v1, glm::vec3& v2, glm::vec3& v3)
{
	int e, dx, dy, reflect = 1;

	dx = x2 - x1;
	dy = y2 - y1;
	if (x1 > x2) // if the slope is negative
		reflect = -1;
	e = -dy;
	while (y1 <= y2) // Bresenham algorithm
	{
		if (e > 0)
		{
			x1 += 1 * reflect;
			e -= 2 * dy;
		}
		if ((!fillTriangle) || (fillTriangle && testAndSetZBuffer(int(x1), int(y1), calculateZ(v1, v2, v3, x1, y1))))
			PutPixel(x1, y1, color);
		y1 += 1;
		e += 2 * dx * reflect;
	}
}
void Renderer::drawRectangle(glm::vec3& verticeModel0, glm::vec3& verticeModel1, glm::vec3& verticeModel2)
{
	float x_Min_Local = viewport_width;
	float y_Min_Local = viewport_height;
	float z_Min_Local = viewport_width;
	float x_Max_Local = 0;
	float y_Max_Local = 0;
	float z_Max_Local = 0;
	vector<glm::vec3> points_to_rectangle;

	x_Min_Local = std::min(verticeModel0.x, verticeModel1.x);
	x_Min_Local = std::min(x_Min_Local, verticeModel2.x);
	y_Min_Local = std::min(verticeModel0.y, verticeModel1.y);
	y_Min_Local = std::min(y_Min_Local, verticeModel2.y);
	z_Min_Local = std::min(verticeModel0.z, verticeModel1.z);
	z_Min_Local = std::min(z_Min_Local, verticeModel2.z);
	x_Max_Local = std::max(verticeModel0.x, verticeModel1.x);
	x_Max_Local = std::max(x_Max_Local, verticeModel2.x);
	y_Max_Local = std::max(verticeModel0.y, verticeModel1.y);
	y_Max_Local = std::max(y_Max_Local, verticeModel2.y);
	z_Max_Local = std::max(verticeModel0.z, verticeModel1.z);
	z_Max_Local = std::max(z_Max_Local, verticeModel2.z);

	points_to_rectangle.push_back(glm::vec3(x_Min_Local, y_Min_Local, 0));
	points_to_rectangle.push_back(glm::vec3(x_Min_Local, y_Max_Local, 0));
	points_to_rectangle.push_back(glm::vec3(x_Max_Local, y_Min_Local, 0));
	points_to_rectangle.push_back(glm::vec3(x_Max_Local, y_Max_Local, 0));

	DrawLine(points_to_rectangle[0], points_to_rectangle[1], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local ));
	DrawLine(points_to_rectangle[0], points_to_rectangle[2], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local ));
	DrawLine(points_to_rectangle[1], points_to_rectangle[3], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local ));
	DrawLine(points_to_rectangle[2], points_to_rectangle[3], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local ));

	points_to_rectangle.pop_back();
	points_to_rectangle.pop_back();
	points_to_rectangle.pop_back();
	points_to_rectangle.pop_back();
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color,bool fillTriangle, glm::vec3& v1, glm::vec3& v2, glm::vec3& v3)
{
	float x1 = 0, x2 = 0, y1 = 0, y2 = 0, e = 0, dx = 0, dy = 0, reflect = 1; // we init some flags
	if (p1.x < p2.x)  // init on base of is p1.x < p2.x
	{
		x1 = p1.x;
		y1 = p1.y;
		x2 = p2.x;
		y2 = p2.y;
	}
	else
	{
		x1 = p2.x;
		y1 = p2.y;
		x2 = p1.x;
		y2 = p1.y;
	}
	dx = x2 - x1; // init delta for x and y
	dy = y2 - y1;
	if (abs(dy) > abs(dx)) // deceiding if need to run the algo. with reversed axis for slopes that are bigger than abs(1)
	{
		if (y1 < y2) // same as we did for p1.x and p2.x above
			DrawLineReversedAxis(x1, y1, x2, y2, color,fillTriangle,v1,v2,v3);
		else
			DrawLineReversedAxis(x2, y2, x1, y1, color, fillTriangle,v1,v2,v3);
		return;
	}
	if (y1 > y2) // if the slope is negative
		reflect = -1;
	e = -dx;
	while (x1 <= x2) // Bresenham algorithm
	{
		if (e > 0)
		{
			y1 += 1 * reflect;
			e -= 2 * dx;
		}
		if ((!fillTriangle)||(fillTriangle && testAndSetZBuffer(int(x1), int(y1), calculateZ(v1, v2, v3, x1, y1)))) 
			PutPixel(x1, y1, color);
		x1 += 1;
		e += 2 * dy * reflect;
	}
}
void Renderer::DrawCircle(const glm::ivec2& center, const int& radius, const int& stepSize, const glm::vec3& color)
{
	const int x0 = center.x;
	const int y0 = center.y;
	glm::ivec2 centerP = glm::ivec2(x0, y0);
	for (int i = 0;i < 360;i++)
	{
		double x1 = x0 + radius * (sin((2 * M_PI * i) / stepSize));
		double y1 = y0 + radius * (cos((2 * M_PI * i) / stepSize));
		DrawLine(centerP, glm::ivec2(x1, y1), color);
	}
}
bool Renderer::testAndSetZBuffer(int x,int y,float z)
{
	if (!(x >= 0 && y >= 0 && x < viewport_width && y < viewport_height))
		return false;
	if (this->z_buffer[ Z_INDEX(this->viewport_width,x,y)]  <= z) 
		return false;
	this->z_buffer[Z_INDEX(this->viewport_width, x, y)] = z;
	return true;

}

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	this->z_buffer = new float[w * h];
	color_buffer = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
	clearZBuffer();
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenglRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::CreateOpenglBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}
void Renderer::clearZBuffer()
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			this->z_buffer[Z_INDEX(this->viewport_width, i, j)] = std::numeric_limits<float>::infinity();
		}
	}
}

void Renderer::SetNewViewport(float frameBufferWidth, float frameBufferHeight)
{
	viewport_width = frameBufferWidth;
	viewport_height = frameBufferHeight;
}
/*
void Renderer::putFlower(int radius,int x ,const glm::vec3& color, int stemLen)
{
	int ground = 50;
	glm::ivec2 center(x, 50 + stemLen + radius );
	for (int i = 0, step = 180; i < step; i++)
	{
		glm::ivec2 b(center.x + radius * sin((2 * M_PI * i) / step), center.y + radius * cos((2 * M_PI * i) / step));
		DrawLine(center, b, color);
	}
	DrawLine({ center.x,center.y - radius }, { center.x,ground }, { 0,1,0 });
	DrawLine({ center.x,center.y - radius }, { center.x-1,ground }, { 0,1,0 });
	DrawLine({ center.x,center.y - radius }, { center.x+1,ground }, { 0,1,0 });
}
void Renderer::drawSomeFlowers()
{
	glm::ivec2 a(0, 700);
	glm::ivec3 color(200, 0, 0);
	for (int i = 90, r = 200, step = 360; i <=180; i++) // draws the sun
	{
		glm::ivec2 b(a.x+r * sin((2 * M_PI * i) / step), a.y+r * cos((2 * M_PI * i) / step));
		DrawLine( a, b, color);
	}
	// drawing the flowers
	putFlower(100, 400, { 70,70,70 }, 200);
	putFlower(80, 200, { 70,70,70 }, 200);
	putFlower(60, 600, { 70,70,70 }, 100);
	putFlower(70, 800, { 70,70,70 }, 80);
	putFlower(130, 1000, { 70,70,70 }, 250);

	return;
}
*/
void Renderer::drawBoundingBox(MeshModel& myModel, Scene& scene,const glm::vec3& color, bool isWorld, float x_Min, float y_Min, float z_Min, float x_Max, float y_Max, float z_Max)
{
	float half_width = viewport_width / 2;
	float half_height = viewport_height / 2;

	Camera& cam = scene.GetActiveCamera();
	glm::mat4 view = cam.GetViewTransformation();				//view
	glm::mat4 projection = cam.GetProjectionTransformation();   //projection
	glm::mat4 Transformation = myModel.GetTransform();
	glm::mat4 modelTransform = myModel.GetObjectTransform();
	glm::mat4 worldTransform = myModel.GetWorldTransform();
	glm::vec3 b_Box1;
	glm::vec3 b_Box2;
	glm::vec3 b_Box3;
	glm::vec3 b_Box4;
	glm::vec3 b_Box5;
	glm::vec3 b_Box6;
	glm::vec3 b_Box7;
	glm::vec3 b_Box8;
	if (isWorld)
	{
		b_Box1 = glm::vec4(x_Min, y_Min, z_Min, 1.0f);
		b_Box2 = glm::vec4(x_Min, y_Min, z_Max, 1.0f);
		b_Box3 = glm::vec4(x_Min, y_Max, z_Min, 1.0f);
		b_Box4 = glm::vec4(x_Min, y_Max, z_Max, 1.0f);
		b_Box5 = glm::vec4(x_Max, y_Min, z_Min, 1.0f);
		b_Box6 = glm::vec4(x_Max, y_Min, z_Max, 1.0f);
		b_Box7 = glm::vec4(x_Max, y_Max, z_Min, 1.0f);
		b_Box8 = glm::vec4(x_Max, y_Max, z_Max, 1.0f);
	}
	else
	{
		b_Box1 = glm::project(glm::vec3(x_Min, y_Min, z_Min), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height)); //projection * view * transform* glm::vec4(x_Min, y_Min, z_Min, 1.0f);
		b_Box2 = glm::project(glm::vec3(x_Min, y_Min, z_Max), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));
		b_Box3 = glm::project(glm::vec3(x_Min, y_Max, z_Min), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));
		b_Box4 = glm::project(glm::vec3(x_Min, y_Max, z_Max), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));
		b_Box5 = glm::project(glm::vec3(x_Max, y_Min, z_Min), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));
		b_Box6 = glm::project(glm::vec3(x_Max, y_Min, z_Max), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));
		b_Box7 = glm::project(glm::vec3(x_Max, y_Max, z_Min), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));
		b_Box8 = glm::project(glm::vec3(x_Max, y_Max, z_Max), view * Transformation,projection, glm::vec4(0, 0, viewport_width, viewport_height));

		b_Box1.x += half_width;
		b_Box1.y += half_height;
		b_Box2.x += half_width;
		b_Box2.y += half_height;
		b_Box3.x += half_width;
		b_Box3.y += half_height;
		b_Box4.x += half_width;
		b_Box4.y += half_height;
		b_Box5.x += half_width;
		b_Box5.y += half_height;
		b_Box6.x += half_width;
		b_Box6.y += half_height;
		b_Box7.x += half_width;
		b_Box7.y += half_height;
		b_Box8.x += half_width;
		b_Box8.y += half_height;
	}

	//draw Bounding Box
	DrawLine(b_Box1, b_Box2, color);
	DrawLine(b_Box1, b_Box3, color);
	DrawLine(b_Box1, b_Box5, color);
	DrawLine(b_Box2, b_Box4, color);
	DrawLine(b_Box2, b_Box6, color);
	DrawLine(b_Box3, b_Box4, color);
	DrawLine(b_Box3, b_Box7, color);
	DrawLine(b_Box4, b_Box8, color);
	DrawLine(b_Box5, b_Box6, color);
	DrawLine(b_Box5, b_Box7, color);
	DrawLine(b_Box6, b_Box8, color);
	DrawLine(b_Box7, b_Box8, color);
}
void Renderer::drawAxisLocal(MeshModel& myModel, Scene& scene, const glm::vec3& color, float x_Min, float y_Min, float z_Min, float x_Max, float y_Max, float z_Max)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	Camera& cam = scene.GetActiveCamera();
	glm::mat4 Transformation = myModel.GetTransform();
	glm::mat4 modelTransform = myModel.GetObjectTransform();
	glm::mat4 worldTransform = myModel.GetWorldTransform();
	glm::mat4 model_axis_transform = worldTransform * myModel.GetTranslationObjectMat() * myModel.GetScaleObjectMat();

	glm::mat4 view = cam.GetViewTransformation();				//view
	glm::mat4 projection = cam.GetProjectionTransformation();   //projection
	glm::mat4 trans;
	trans = model_axis_transform;
	float xCenter = (x_Min + x_Max) / 2.0f;
	float yCenter = (y_Min + y_Max) / 2.0f;
	float zCenter = (z_Min + z_Max) / 2.0f;

	glm::vec3 x_Vec(x_Max, yCenter, zCenter);
	glm::vec3 y_Vec(xCenter, y_Max,zCenter);
	glm::vec3 z_Vec(xCenter, yCenter, z_Max);
	glm::vec3 vCenter = glm::vec3(xCenter,yCenter,zCenter);

	glm::vec3 x_Vec_Transformed = glm::project(x_Vec, view * trans, projection, glm::vec4(0, 0, viewport_width, viewport_height));
	glm::vec3 y_Vec_Transformed = glm::project(y_Vec, view * trans, projection, glm::vec4(0, 0, viewport_width, viewport_height));
	glm::vec3 z_Vec_Transformed = glm::project(z_Vec, view * trans, projection, glm::vec4(0, 0, viewport_width, viewport_height));
	glm::vec3 center_Vec_Transformed = glm::project(vCenter, view * trans, projection, glm::vec4(0, 0, viewport_width, viewport_height));

	x_Vec_Transformed.x += half_width;
	x_Vec_Transformed.y += half_height;
	y_Vec_Transformed.x += half_width;
	y_Vec_Transformed.y += half_height;
	z_Vec_Transformed.x += half_width;
	z_Vec_Transformed.y += half_height;
	center_Vec_Transformed.x += half_width;
	center_Vec_Transformed.y += half_height;

	//draw Axis
	DrawLine(center_Vec_Transformed, x_Vec_Transformed, color);
	DrawLine(center_Vec_Transformed, y_Vec_Transformed, color);
	DrawLine(center_Vec_Transformed, z_Vec_Transformed, color);
}
void Renderer::drawAxisWorld(const glm::vec3& color)
{
	DrawLine(glm::vec3(viewport_width / 2, viewport_height, 0), glm::vec3(viewport_width / 2, 0, 0), color);
	DrawLine(glm::vec3(viewport_width, viewport_height/2, 0), glm::vec3(0, viewport_height/2, 0), color);
}
void Renderer::drawVerticeNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;

	Camera& cam = scene.GetActiveCamera();
	glm::mat4 Transformation = myModel.GetTransform();
	glm::mat4 modelTransform = myModel.GetObjectTransform();
	glm::mat4 worldTransform = myModel.GetWorldTransform();
	glm::mat4 view = cam.GetViewTransformation();				//view
	glm::mat4 projection = cam.GetProjectionTransformation();   //projection

	

	for (int i = 0;i <myModel.GetFacesCount();i++)
	{
		glm::vec3 v0 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(0) - 1];
		glm::vec3 v1 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(1) - 1];
		glm::vec3 v2 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(2) - 1];

		glm::vec3 v1_to_v0 = v1 - v0;
		glm::vec3 v2_to_v0 = v2 - v0;
		glm::vec3 v0_cross = glm::cross(v1_to_v0, v2_to_v0)+v0;

		glm::vec3 v0_cross_projected = glm::project(glm::vec3(v0_cross), view * Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 v0_projected = glm::project(glm::vec3(v0), view * Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));

		v0_projected.x += half_width;
		v0_projected.y += half_height;

		v0_cross_projected.x += half_width;
		v0_cross_projected.y += half_height;

		DrawLine(v0_cross_projected, v0_projected, color);

	}
}
void Renderer::drawFacesNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;

	Camera& cam = scene.GetActiveCamera();
	glm::mat4 Transformation = myModel.GetTransform();
	glm::mat4 modelTransform = myModel.GetObjectTransform();
	glm::mat4 worldTransform = myModel.GetWorldTransform();
	glm::mat4 view = cam.GetViewTransformation();				//view
	glm::mat4 projection = cam.GetProjectionTransformation();   //projection

	for (int i = 0;i < myModel.GetFacesCount();i++)
	{
		glm::vec3 v0 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(0) - 1];
		glm::vec3 v1 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(1) - 1];
		glm::vec3 v2 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(2) - 1];

		glm::vec3 c0 = glm::cross((v1-v0), (v2-v0));
		glm::vec3 c0_normalized = glm::normalize(c0);


		glm::vec3 vCenter = (v0 + v1 + v2) / 3.0f;
		glm::vec3 normCenter = c0_normalized +vCenter ;

		glm::vec3 face_norm_projected = glm::project(glm::vec3(normCenter),view* Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 vCenter_projected = glm::project(glm::vec3(vCenter), view * Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));

		vCenter_projected.x += half_width;
		vCenter_projected.y += half_height;

		face_norm_projected.x += half_width;
		face_norm_projected.y += half_height;
		
		DrawLine(face_norm_projected, vCenter_projected, color);
	}
}
void Renderer::colorBottomTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& color)
{
	float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
	float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);
	float currx1 = v1.x;
	float currx2 = v1.x;
	glm::ivec2 iv1, iv2;
	for (int y_line = round(v1.y); y_line < v2.y; y_line++)
	{
		iv1.x = currx1;
		iv1.y = y_line;
		iv2.x = currx2;
		iv2.y = y_line;
		DrawLine(iv1, iv2, color, true, v1, v2, v3);
		currx1 += invslope1;
		currx2 += invslope2;

	}

}
void Renderer::colorTopTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& color)
{
	float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
	float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);
	float currx1 = v3.x;
	float currx2 = v3.x;
	glm::ivec2 iv1, iv2;
	for (int y_line= round(v3.y); y_line > v1.y; y_line--)
	{

		iv1.x = currx1;
		iv1.y = y_line;
		iv2.x = currx2;
		iv2.y = y_line;
		DrawLine(iv1, iv2, color,true,v1,v2,v3);
		currx1 -= invslope1;
		currx2 -= invslope2;
	}

}
float Renderer::calculateZ(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, float x, float y) 
{
	float det = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);

	float l1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / det;
	float l2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / det;
	float l3 = 1.0f - l1 - l2;

	return (l1 * v1.z) + (l2 * v2.z) + (l3 * v3.z);
}
void Renderer::drawTriangles(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3& color)
{ 
	std::vector<glm::vec3> temp(3);
	
	glm::vec3 my_v1, my_v2, my_v3;
	temp[0] = v1;
	temp[1] = v2;
	temp[2] = v3;
	std::sort(temp.begin(), temp.end(), [](const glm::vec3& a, const glm::vec3& b) // sort vertices by y coordinate
		{
			return a.y < b.y;
		});
	my_v1 = temp[0];
	my_v2 = temp[1];
	my_v3 = temp[2];
	if (my_v2.y == my_v3.y) // flat bottom triangle
	{
		colorBottomTriangle(my_v1, my_v2, my_v3, color);
	}
	else if (my_v1.y == my_v2.y) // flat top triangle
	{
		colorTopTriangle(my_v1, my_v2, my_v3, color);
	}
	else // need to add another vertex as we break the triangle into 2 seprate triangles
	{
		glm::vec3 my_v4;

		my_v4.y = my_v2.y;
		if (my_v1.x == my_v3.x)
			my_v4.x = my_v1.x;
		else
		{
			float m = (my_v1.y - my_v3.y) / (my_v1.x - my_v3.x);
			my_v4.x = (my_v4.y - my_v1.y + m * my_v1.x) / m; // m can not be equal to 0 because of the sorting of the vertices
		}
		//use barycentric coordinates to find depth (z) of my_v4:
		my_v4.z = calculateZ(my_v1, my_v2, my_v3, my_v4.x, my_v4.y);
		colorBottomTriangle(my_v1, my_v2, my_v4, color);
		colorTopTriangle(my_v2, my_v4, my_v3, color);
		DrawLine(my_v2, my_v4, color);
	}

}
	

void Renderer::drawModel( MeshModel& myModel,Scene &scene)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	Camera& cam = scene.GetActiveCamera();
	light& light_test = scene.GetActiveLight();
	glm::mat4 Transformation = myModel.GetTransform();
	glm::mat4 modelTransform = myModel.GetObjectTransform();	
	glm::mat4 worldTransform = myModel.GetWorldTransform();
	glm::mat4 view = cam.GetViewTransformation();				//view
	glm::mat4 projection = cam.GetProjectionTransformation();   //projection

	glm::mat4 translate_Local = glm::translate(glm::mat4(1.0f), myModel.GetTranslationObject());
	glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
	const glm::vec3 colorAxisLocal = glm::vec3(0.5, 0.9, 0.2);
	const glm::vec3 colorAxisWorld = glm::vec3(0.7, 0.2, 0.3);
	const glm::vec3 colorBBoxLocal = glm::vec3(0.5, 0.3, 0);
	const glm::vec3 colorBBoxWorld = glm::vec3(0.3, 0, 1);

	//

	//

	float x_Min = viewport_width;
	float y_Min = viewport_height;
	float z_Min = viewport_width;
	float x_Max = 0;
	float y_Max = 0;
	float z_Max = 0;

	float x_Min_World = viewport_width;
	float y_Min_World = viewport_height;
	float z_Min_World = viewport_width;
	float x_Max_World = 0;
	float y_Max_World = 0;
	float z_Max_World = 0;



	for (int i = 0;i < myModel.GetFacesCount();i++)
	{
		glm::vec3 v0 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(0) - 1];
		glm::vec3 v1 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(1) - 1];
		glm::vec3 v2 = myModel.GetVertices()[myModel.GetFace(i).GetVertexIndex(2) - 1];

		glm::mat4 view_transform = view * Transformation;

		glm::vec3 verticeModel0 = glm::project(v0, view_transform, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 verticeModel1 = glm::project(v1, view_transform, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 verticeModel2 = glm::project(v2, view_transform, projection, glm::vec4(0, 0, viewport_width, viewport_height));

		glm::vec3 v0_no_camera = glm::vec3(glm::vec4(v0,1) * Transformation);
		glm::vec3 v1_no_camera = glm::vec3(glm::vec4(v1,1) * Transformation);
		glm::vec3 v2_no_camera = glm::vec3(glm::vec4(v2,1) * Transformation);

		verticeModel0.x += half_width;
		verticeModel0.y += half_height;
		verticeModel1.x += half_width;
		verticeModel1.y += half_height;
		verticeModel2.x += half_width;
		verticeModel2.y += half_height;

		v0_no_camera.x += half_width;
		v0_no_camera.y += half_height;
		v1_no_camera.x += half_width;
		v1_no_camera.y += half_height;
		v2_no_camera.x += half_width;
		v2_no_camera.y += half_height;

		x_Min_World = std::min(verticeModel0.x, x_Min_World);
		y_Min_World = std::min(verticeModel0.y, y_Min_World);
		z_Min_World = std::min(verticeModel0.z, z_Min_World);
		x_Max_World = std::max(verticeModel0.x, x_Max_World);
		y_Max_World = std::max(verticeModel0.y, y_Max_World);
		z_Max_World = std::max(verticeModel0.z, z_Max_World);
		
		x_Min = std::min(v0.x, x_Min);
		y_Min = std::min(v0.y, y_Min);
		z_Min = std::min(v0.z, z_Min);
		x_Max = std::max(v0.x, x_Max);
		y_Max = std::max(v0.y, y_Max);
		z_Max = std::max(v0.z, z_Max);

		glm::vec3 center = (verticeModel0 + verticeModel1 + verticeModel2) / 3.0f;
		glm::vec3 center_no_camera = (v0_no_camera + v1_no_camera + v2_no_camera) / 3.0f;

		glm::vec3 normal = glm::cross(verticeModel1 - verticeModel0, verticeModel2 - verticeModel0);
		//DrawLine(normal+center, center, colorAxisLocal);

		normal = glm::normalize(normal);

		
		//
		float ambientStrength = light_test.ambient_strength;
		glm::vec3 ambient_color = ambientStrength * light_test.ambient * myModel.ambient;
		glm::vec3 result_ambient = ambient_color * color;
		//

		glm::vec3 light_position = light_test.position;
		glm::mat4 light_transform = light_test.get_transform();
		glm::vec3 light_projected = glm::project(light_position, view*light_transform, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 light_direction = center - light_projected;
		float diffuse = glm::dot(normal, light_direction);
		diffuse = glm::max(diffuse, 0.0f);
		diffuse = glm::clamp(diffuse, 0.0f, 1.0f);
		glm::vec3 diffuse_color = diffuse * light_test.diffuse * myModel.diffuse;
		glm::vec3 result_diffuse = diffuse_color * color;

		//DrawLine(light_projected, center, colorBBoxLocal);

		glm::vec3 result_defuse_and_ambient = diffuse_color * color + ambient_color * color; //* (1.0f - ambientStrength - diffuse);

		float specularStrength = light_test.specular_strength;
		glm::vec3 view_direction = glm::normalize(light_position - center_no_camera);
		glm::vec3 reflect_direction = glm::reflect(-light_direction, normal);
		float exponent = 20;
		float specular = glm::pow(glm::dot(view_direction, reflect_direction),exponent);
		specular = glm::clamp(specular, 0.0f, 1.0f);
		glm::vec3 specular_color = specularStrength * specular * light_test.specular;
		glm::vec3 result_specular = specular_color + color * (1 - specular);

		glm::vec3 result = result_defuse_and_ambient + specular_color * color;
		
		DrawCircle(light_projected, 100, 10, colorAxisLocal);
		drawTriangles(verticeModel0, verticeModel1, verticeModel2, result_defuse_and_ambient);



		//DrawLine(verticeModel0, verticeModel1, color);
		//DrawLine(verticeModel0, verticeModel2, color);
		//DrawLine(verticeModel2, verticeModel1, color);
		if (myModel.drawRectangle) drawRectangle(verticeModel0, verticeModel1, verticeModel2);

	}
		if (myModel.getAxisLocal()) { drawAxisLocal(myModel, scene, colorAxisLocal, x_Min, y_Min, z_Min, x_Max, y_Max, z_Max); }
		if (myModel.getAxisWorld()) { drawAxisWorld(colorAxisWorld); }
		if (myModel.getbBoxLocal()) { drawBoundingBox(myModel, scene, colorBBoxLocal, false,x_Min,y_Min,z_Min,x_Max,y_Max,z_Max); }
		if (myModel.getbBoxWorld()) { drawBoundingBox(myModel, scene, colorBBoxWorld, true, x_Min_World, y_Min_World, z_Min_World, x_Max_World, y_Max_World, z_Max_World); }
		if (myModel.drawVerticeNormals) { drawVerticeNormals(myModel, scene, colorAxisLocal); }
		if (myModel.drawFaceNormals) { drawFacesNormals(myModel, scene, colorAxisLocal); };
}


void Renderer::Render( Scene& scene)
{
	if (scene.GetModelCount() > 0)
	{
		for (int i = 0;i < scene.GetModelCount();i++)
		{
			MeshModel myModel = scene.GetModel(i);
			drawModel(myModel,scene);
		}
	}
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}