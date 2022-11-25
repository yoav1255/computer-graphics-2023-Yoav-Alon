#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

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
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;
	
	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}
void Renderer::drawCircle(const int& x,const int& y, const int& xCenter, const int& yCenter)
{
	glm::vec3 color = { 10,0,0 };
	PutPixel(x + xCenter, y + yCenter, color);
	PutPixel(-x + xCenter, y + yCenter, color);
	PutPixel(x + xCenter, -y + yCenter, color);
	PutPixel(-x + xCenter, -y + yCenter, color);
	PutPixel(y + xCenter, x + yCenter, color);
	PutPixel(y + xCenter, -x + yCenter, color);
	PutPixel(-y + xCenter, x + yCenter, color);
	PutPixel(-y + xCenter, -x + yCenter, color);
	return;

}

void Renderer::drawCircleMain()
{
	int e = 0, radius = 100, x = 0, y = 0, xCenter = 400, yCenter = 500;
	e = 3 - 2 * radius;
	y = radius;
	drawCircle(x, y, xCenter, yCenter);
	while (x < y)
	{
		if (e <= 0)
		{
			e = e + (4 * x) + 6;
			x++;
			drawCircle(x, y, xCenter, yCenter);
		}
		else
		{
			e = e + (4 * (x - y)) + 10;
			x++;
			y--;
			drawCircle(x, y, xCenter, yCenter);
		}
	}
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

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
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

void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
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
void Renderer::drawModel(MeshModel& model)
{
	glm::vec3 vecA, vecB, vecC;
	model.setLocal();
	model.setWorld();
	glm::mat4 transformationMat = (model.getWorld() * model.getLocal());
	int triangles = model.GetFacesCount();
	//std::cout << "Face count is: " << triangles << std::endl;
	for (int i = 0; i < triangles; i++)
	{
		int a = model.GetFace(i).GetVertexIndex(0) - 1;
		int b = model.GetFace(i).GetVertexIndex(1) - 1;
		int c = model.GetFace(i).GetVertexIndex(2) - 1;
		vecA = model.GetVertexAt(a), vecB = model.GetVertexAt(b), vecC = model.GetVertexAt(c);


		DrawLine(transformationMat * glm::vec4(vecA, 1.0f), transformationMat * glm::vec4(vecB, 1.0f), { 255,0,0 });
		DrawLine(transformationMat * glm::vec4(vecA, 1.0f), transformationMat * glm::vec4(vecC, 1.0f), { 255,0,0 });
		DrawLine(transformationMat * glm::vec4(vecC, 1.0f), transformationMat * glm::vec4(vecB, 1.0f), { 255,0,0 });
		/*DrawLine({model.GetVertexAt(a).x , model.GetVertexAt(a).z}, {model.GetVertexAt(b).x, model.GetVertexAt(b).z}, {255,0,0});
		DrawLine({ model.GetVertexAt(a).x , model.GetVertexAt(a).z }, { model.GetVertexAt(c).x, model.GetVertexAt(c).z }, { 255,0,0 });
		DrawLine({ model.GetVertexAt(b).x , model.GetVertexAt(b).z }, { model.GetVertexAt(c).x, model.GetVertexAt(c).z }, { 255,0,0 });
		DrawLine({ model.GetVertexAt(a).z , model.GetVertexAt(a).y }, { model.GetVertexAt(b).z, model.GetVertexAt(b).y }, { 255,0,0 });
		DrawLine({ model.GetVertexAt(a).z , model.GetVertexAt(a).y }, { model.GetVertexAt(c).z, model.GetVertexAt(c).y }, { 255,0,0 });
		DrawLine({ model.GetVertexAt(b).z , model.GetVertexAt(b).y }, { model.GetVertexAt(c).z, model.GetVertexAt(c).y }, { 255,0,0 });*/
		//std::cout << "Vertices are: " << a << " , " << b << " , " << c << std::endl;
	}
	return;
}
void Renderer::Render(const Scene& scene)
{
	
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	//drawSomeFlowers();
	MeshModel& model = scene.GetModel(0);
	drawModel(model);
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}