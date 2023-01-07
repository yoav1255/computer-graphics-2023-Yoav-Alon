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
void Renderer::DrawLineReversedAxis(int x1, int y1, int x2, int y2, const glm::vec3& color)
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
		PutPixel(x1, y1, color);
		y1 += 1;
		e += 2 * dx * reflect;
	}
}

void Renderer::DrawLine(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color)
{
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0, e = 0, dx = 0, dy = 0, reflect = 1; // we init some flags
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
			DrawLineReversedAxis(x1, y1, x2, y2, color);
		else
			DrawLineReversedAxis(x2, y2, x1, y1, color);
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
		PutPixel(x1, y1, color);
		x1 += 1;
		e += 2 * dy * reflect;
	}
}

void Renderer::DrawLineReversedAxis_CalcZ(int x1, int y1, int x2, int y2, const glm::vec3& color, glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
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
		if ((x1 > 0 && x1 < viewport_width) && (y1 > 0 && y1 < viewport_height))
		{
			float curr_z = At(x1, y1);
			float point_z = Find_Current_Point(glm::vec2(x1, y1), t1, t2, t3);
			if (point_z < curr_z)
			{
				PutPixel(x1, y1, color);
				z_buffer[x1 + viewport_width * y1] = point_z;
			}
		}
		y1 += 1;
		e += 2 * dx * reflect;
	}
}

void Renderer::DrawLine_CalcZ(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color, glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
{
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0, e = 0, dx = 0, dy = 0, reflect = 1; // we init some flags
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
			DrawLineReversedAxis(x1, y1, x2, y2, color);
		else
			DrawLineReversedAxis(x2, y2, x1, y1, color);
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
		if ((x1 > 0 && x1 < viewport_width) && (y1 > 0 && y1 < viewport_height))
		{
			float curr_z = At(x1, y1);
			float point_z = Find_Current_Point(glm::vec2(x1, y1), t1, t2, t3);
			if (point_z < curr_z)
			{
				PutPixel(x1, y1, color);
				z_buffer[x1 + viewport_width * y1] = point_z;
			}
		}
		x1 += 1;
		e += 2 * dy * reflect;
	}
}

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	z_buffer = new float[w*h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}
float Renderer::At(int x, int y)
{
	return z_buffer[x + viewport_width * y];
}

float Renderer::Find_Current_Point(glm::vec2 &P, glm::vec3 &A, glm::vec3 &B, glm::vec3 &C)
{

	glm::vec3 p = glm::vec3(P.x, P.y, 0);
	//float area = (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2.0f;
	//area = area > 0.0 ? area : -area;

	//float areaA = (p.x * (B.y - C.y) + B.x * (C.y - p.y) + C.x * (p.y - B.y)) / 2.0f;
	//areaA = areaA > 0.0 ? areaA : -areaA;

	//float areaB = (A.x * (p.y - C.y) + p.x * (C.y - A.y) + C.x * (A.y - p.y)) / 2.0f;
	//areaB = areaB > 0.0 ? areaB : -areaB;

	//float areaC = (A.x * (B.y - p.y) + B.x * (p.y - A.y) + p.x * (A.y - B.y)) / 2.0f;
	//areaC = areaC > 0.0 ? areaC : -areaC;

	float area = 0.5f * glm::length(glm::cross(B - A, C - A));
	area = area > 0.0 ? area : -area;

	float areaA = 0.5f * glm::length(glm::cross(B - p, C - p));
	areaA = areaA > 0.0 ? areaA : -areaA;

	float areaB = 0.5f * glm::length(glm::cross(p - A, C - A));
	areaB = areaB > 0.0 ? areaB : -areaB;

	float areaC = 0.5f * glm::length(glm::cross(B - A, p - A));
	areaC = areaC > 0.0 ? areaC : -areaC;


	float alpha = areaA / area;
	float beta = areaB / area;
	float gama = 1-alpha-beta;

	float z = alpha * A.z + beta * B.z + gama * C.z;
	return z;
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
void Renderer::ClearZBuffer()
{
	for (int i = 0; i < viewport_height; i++)
	{
		for (int j = 0; j < viewport_width; j++)
		{
			z_buffer[i * viewport_width + j] = std::numeric_limits<float>::infinity();
		}
	}
}

void Renderer::SetNewViewport(float frameBufferWidth, float frameBufferHeight)
{
	viewport_width = frameBufferWidth;
	viewport_height = frameBufferHeight;
}

//void Renderer::putFlower(int radius,int x ,const glm::vec3& color, int stemLen)
//{
//	int ground = 50;
//	glm::ivec2 center(x, 50 + stemLen + radius );
//	for (int i = 0, step = 180; i < step; i++)
//	{
//		glm::ivec2 b(center.x + radius * sin((2 * M_PI * i) / step), center.y + radius * cos((2 * M_PI * i) / step));
//		DrawLine(center, b, color);
//	}
//	DrawLine({ center.x,center.y - radius }, { center.x,ground }, { 0,1,0 });
//	DrawLine({ center.x,center.y - radius }, { center.x-1,ground }, { 0,1,0 });
//	DrawLine({ center.x,center.y - radius }, { center.x+1,ground }, { 0,1,0 });
//}
//void Renderer::drawSomeFlowers()
//{
//	glm::ivec2 a(0, 700);
//	glm::ivec3 color(200, 0, 0);
//	for (int i = 90, r = 200, step = 360; i <=180; i++) // draws the sun
//	{
//		glm::ivec2 b(a.x+r * sin((2 * M_PI * i) / step), a.y+r * cos((2 * M_PI * i) / step));
//		DrawLine( a, b, color);
//	}
//	// drawing the flowers
//	putFlower(100, 400, { 70,70,70 }, 200);
//	putFlower(80, 200, { 70,70,70 }, 200);
//	putFlower(60, 600, { 70,70,70 }, 100);
//	putFlower(70, 800, { 70,70,70 }, 80);
//	putFlower(130, 1000, { 70,70,70 }, 250);
//
//	return;
//}

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
void Renderer::drawRectangle(glm::vec3 &verticeModel0, glm::vec3 &verticeModel1, glm::vec3 &verticeModel2)
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

	points_to_rectangle.push_back(glm::vec3(x_Min_Local, y_Min_Local,0));
	points_to_rectangle.push_back(glm::vec3(x_Min_Local, y_Max_Local,0));
	points_to_rectangle.push_back(glm::vec3(x_Max_Local, y_Min_Local,0));
	points_to_rectangle.push_back(glm::vec3(x_Max_Local, y_Max_Local,0));

	DrawLine(points_to_rectangle[0], points_to_rectangle[1], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local / 1));
	DrawLine(points_to_rectangle[0], points_to_rectangle[2], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local / 1));
	DrawLine(points_to_rectangle[1], points_to_rectangle[3], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local / 1));
	DrawLine(points_to_rectangle[2], points_to_rectangle[3], glm::vec3(1.0f, 1.0f, 1.0f) - float(z_Max_Local / 1));

	points_to_rectangle.pop_back();
	points_to_rectangle.pop_back();
	points_to_rectangle.pop_back();
	points_to_rectangle.pop_back();
}

bool comparePoints(glm::vec3 v1, glm::vec3 v2)
{
	return v1.x < v2.x;
}
//glm::vec3 min_points(glm::vec3 v1, glm::vec3 v2)
//{
//	return v1.x < v2.x?v1:v2;
//}
//glm::vec3 max_points(glm::vec3 v1, glm::vec3 v2)
//{
//	return v1.x < v2.x ? v2 : v1;
//}
//glm::vec3 mid_point(glm::vec3 v1, glm::vec3 v2,glm::vec3 v3)
//{
//	if ((v1.x <= v2.x && v2.x <= v3.x)||(v1.x >= v2.x && v2.x >= v3.x)) return v2;
//	if ((v1.x >= v2.x && v1.x <= v3.x)||(v1.x <= v2.x && v1.x >= v3.x)) return v1;
//	if ((v3.x <= v2.x && v1.x <= v3.x)||(v3.x >= v2.x && v1.x >= v3.x)) return v3;
//}
void Renderer::fill_left_Triangle(glm::vec3 v_min, glm::vec3 v_mid, glm::vec3 v_max,glm::vec3 color)
{
	float slope1 = (v_max.y - v_min.y) / float(v_max.x - v_min.x);
	float slope2 = (v_mid.y - v_min.y) / float(v_mid.x - v_min.x);
	glm::vec3 p1 = v_min;
	glm::vec3 p2 = v_min;
	for (float x = v_min.x;x <= v_mid.x;x++)
	{
		p1.x = x;
		p2.x = x;
		p1.y = v_min.y + (x-v_min.x) * slope1;
		p2.y = v_min.y + (x-v_min.x) * slope2;
		DrawLine_CalcZ(p1, p2, color,v_min,v_mid,v_max);
	}
}
void Renderer::fill_right_Triangle(glm::vec3 v_min, glm::vec3 v_mid, glm::vec3 v_max, glm::vec3 color)
{
	float slope1 = (v_min.y -v_max.y ) / float(v_min.x - v_max.x);
	float slope2 = (v_mid.y - v_max.y) / float(v_mid.x - v_max.x);
	glm::vec3 p1 = v_max;
	glm::vec3 p2 = v_max;
	for (float x = v_max.x;x >= v_mid.x;x--)
	{
		p1.x = x;
		p2.x = x;
		p1.y = v_max.y + (x-v_max.x) * slope1;
		p2.y = v_max.y + (x-v_max.x) * slope2;
		DrawLine_CalcZ(p1, p2, color, v_min, v_mid, v_max);
	}
}
void Renderer::edgeWalking(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, const glm::vec3& color)
{
	glm::vec3 color_test = glm::vec3(0.7f, 0.2f, 0.2f);
	glm::vec3 color_white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 color_black = glm::vec3(0.0f, 0.0f, 0.0f);

	vector<glm::vec3> points;
	points.push_back(v1);
	points.push_back(v2);
	points.push_back(v0);
	std::sort(points.begin(), points.end(), comparePoints);

	//glm::vec3 v_min = min_points(min_points(v0, v1), v2);
	//glm::vec3 v_max = max_points(max_points(v0, v1), v2);
	//glm::vec3 v_mid = mid_point(v0,v1,v2);

	glm::vec3 v_min = points[0];
	glm::vec3 v_mid = points[1];
	glm::vec3 v_max = points[2];

	if (v_mid.x == v_max.x) fill_left_Triangle(v_min, v_mid, v_max, color);
	else if (v_mid.x == v_min.x) fill_right_Triangle(v_min, v_mid, v_max, color);
	else
	{
		fill_left_Triangle(v_min, v_mid, v_max, color);
		fill_right_Triangle(v_min, v_mid, v_max, color);
	}
}
void Renderer::drawZBuffer(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3)
{

}

void Renderer::drawModel( MeshModel& myModel,Scene &scene)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	Camera& cam = scene.GetActiveCamera();
	glm::mat4 Transformation = myModel.GetTransform();
	glm::mat4 modelTransform = myModel.GetObjectTransform();	
	glm::mat4 worldTransform = myModel.GetWorldTransform();
	glm::mat4 view = cam.GetViewTransformation();				//view
	glm::mat4 projection = cam.GetProjectionTransformation();   //projection

	glm::mat4 translate_Local = glm::translate(glm::mat4(1.0f), myModel.GetTranslationObject());

	const glm::vec3 colorAxisLocal = glm::vec3(0.5, 0.9, 0.2);
	const glm::vec3 colorAxisWorld = glm::vec3(0.7, 0.2, 0.3);
	const glm::vec3 colorBBoxLocal = glm::vec3(0.5, 0.3, 0);
	const glm::vec3 colorBBoxWorld = glm::vec3(0.3, 0, 1);

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

		glm::vec3 verticeModel0 = glm::project(v0, view * Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 verticeModel1 = glm::project(v1, view * Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 verticeModel2 = glm::project(v2, view * Transformation, projection, glm::vec4(0, 0, viewport_width, viewport_height));

		glm::vec3 verticeLocal0 = glm::project(v0, view * myModel.GetTranslationWorldMat() * myModel.GetScaleWorldMat() * modelTransform, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 verticeLocal1 = glm::project(v1, view * myModel.GetTranslationWorldMat() * myModel.GetScaleWorldMat() * modelTransform, projection, glm::vec4(0, 0, viewport_width, viewport_height));
		glm::vec3 verticeLocal2 = glm::project(v2, view * myModel.GetTranslationWorldMat() * myModel.GetScaleWorldMat() * modelTransform, projection, glm::vec4(0, 0, viewport_width, viewport_height));

		verticeModel0.x += half_width;
		verticeModel0.y += half_height;
		verticeModel1.x += half_width;
		verticeModel1.y += half_height;
		verticeModel2.x += half_width;
		verticeModel2.y += half_height;

		verticeLocal0.x += half_width;
		verticeLocal0.y += half_height;
		verticeLocal1.x += half_width;
		verticeLocal1.y += half_height;
		verticeLocal2.x += half_width;
		verticeLocal2.y += half_height;

		x_Min_World = std::min(verticeModel0.x, x_Min_World);
		y_Min_World = std::min(verticeModel0.y, y_Min_World);
		z_Min_World = std::min(verticeModel0.z, z_Min_World);
		x_Max_World = std::max(verticeModel0.x, x_Max_World);
		y_Max_World = std::max(verticeModel0.y, y_Max_World);
		z_Max_World = std::max(verticeModel0.z, z_Max_World);

		//x_Min_Local = std::min(verticeLocal0.x, x_Min_Local);
		//y_Min_Local = std::min(verticeLocal0.y, y_Min_Local);
		//z_Min_Local = std::min(verticeLocal0.z, z_Min_Local);
		//x_Max_Local = std::max(verticeLocal0.x, x_Max_Local);
		//y_Max_Local = std::max(verticeLocal0.y, y_Max_Local);
		//z_Max_Local = std::max(verticeLocal0.z, z_Max_Local);

		x_Min = std::min(v0.x, x_Min);
		y_Min = std::min(v0.y, y_Min);
		z_Min = std::min(v0.z, z_Min);
		x_Max = std::max(v0.x, x_Max);
		y_Max = std::max(v0.y, y_Max);
		z_Max = std::max(v0.z, z_Max);

		//DrawLine(verticeModel0, verticeModel1, colorBBoxWorld);
		//DrawLine(verticeModel0, verticeModel2, colorBBoxWorld);
		//DrawLine(verticeModel2, verticeModel1, colorBBoxWorld);

		edgeWalking(verticeModel0, verticeModel1, verticeModel2, myModel.colors[i]);

		myModel.drawRectangle = false; // change in GUI!
		if (myModel.drawRectangle) { drawRectangle(verticeModel0, verticeModel1, verticeModel2); }

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
		ClearZBuffer();
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