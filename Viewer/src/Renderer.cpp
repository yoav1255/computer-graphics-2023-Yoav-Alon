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

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	// TODO: Implement bresenham algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

		int	x0 = p1.x, x1 = p2.x, y0 = p1.y, y1 = p2.y;
		const int distanceY = -abs(y1 - y0), distanceX = abs(x1 - x0);
		const int setX = x0 < x1 ? 1 : -1;
		const int setY = y0 < y1 ? 1 : -1;
		int error = distanceX + distanceY;
		bool flagX ,flagY;
		flagX = x0 != x1 ? false : true;
		flagY = y0 != y1 ? false : true;
		while (!flagX||!flagY)
		{
			if (x0 == x1)flagX = true;
			if (y0 == y1)flagY = true;
			PutPixel(x0, y0, color);
			if (!flagX || !flagY)
			{
				int e2 = 2 * error;
				if (e2 >= distanceY) {
					if (!flagX)
					{
						error += distanceY;
						x0 += setX;
					}
				}
				else
				{
					if (e2 <= distanceX)
					{
						if (!flagY)
						{
							error += distanceX;
							y0 += setY;
						}
					}
				}
			}
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

void Renderer::DrawSunrise()
{
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

void Renderer::PutSymmetricPixelsHalfDown(const int& xc, const int& yc, const int& x, const int& y, const glm::vec3& color)
{
	PutPixel(xc + y, yc - x, color);
	PutPixel(xc - y, yc - x, color);
	PutPixel(xc + x, yc - y, color);
	PutPixel(xc - x, yc - y, color);
}


void Renderer::PutSymmetricPixels(const int& xc, const int& yc, const int& x, const int& y, const glm::vec3& color)
{
	PutPixel(xc + x, yc + y, color);
	PutPixel(xc - x, yc + y, color);
	PutPixel(xc + x, yc - y, color);
	PutPixel(xc - x, yc - y, color);
	PutPixel(xc + y, yc + x, color);
	PutPixel(xc - y, yc + x, color);
	PutPixel(xc + y, yc - x, color);
	PutPixel(xc - y, yc - x, color);
}

void Renderer::DrawParameterCircle(const glm::ivec2& center, const int& radius, const glm::vec3& color,bool half)
{
	int d = 3 - (2 * radius);
	int x = 0;
	int y = radius;
	const int xCenter = center.x;
	const int yCenter = center.y;
	if(half)	PutSymmetricPixelsHalfDown(xCenter, yCenter, x, y,color);
	else		PutSymmetricPixels(xCenter, yCenter, x, y, color);
	while (y >= x) {
		x++;
		if (d >= 0) { // X is incremented , Y is decremented
			y--;
			d += 4 * (x - y) + 10;
		}
		else { // Only X is incremented
			d += 4 * x + 6;
		}
		if (half)	PutSymmetricPixelsHalfDown(xCenter, yCenter, x, y, color);
		else		PutSymmetricPixels(xCenter, yCenter, x, y, color);
	}
}


void Renderer::Render(const Scene& scene)
{

	const glm::ivec2 earRight = glm::ivec2(viewport_width*0.6,viewport_height*0.75);
	const glm::ivec2 earLeft = glm::ivec2(viewport_width * 0.4, viewport_height * 0.75);

	const glm::ivec2 face = glm::ivec2(viewport_width * 0.5, viewport_height * 0.5);
	const glm::ivec2 nose = glm::ivec2(viewport_width * 0.5, viewport_height * 0.5);

	const glm::ivec2 eyeRightOutside = glm::ivec2(viewport_width * 0.55, viewport_height * 0.62);
	const glm::ivec2 eyeRightInside = glm::ivec2(viewport_width * 0.55, viewport_height * 0.60);

	const glm::ivec2 eyeLeftOutside = glm::ivec2(viewport_width * 0.45, viewport_height * 0.62);
	const glm::ivec2 eyeLeftInside = glm::ivec2(viewport_width * 0.45, viewport_height * 0.60);

	const glm::ivec2 mouth = glm::ivec2(viewport_width * 0.5, viewport_height * 0.43);


	const glm::vec3 color = glm::vec3(1, 0.5, 0.3);

	DrawParameterCircle(earRight, 60, color);
	DrawParameterCircle(earLeft, 60, color);
	DrawParameterCircle(face, 150, color);
	DrawParameterCircle(eyeRightOutside, 35, color);
	DrawParameterCircle(eyeLeftOutside, 35, color);
	DrawParameterCircle(mouth,50,color,true);

	DrawCircle(nose, 25, 360, color);
	DrawCircle(eyeRightInside, 22, 360, color);
	DrawCircle(eyeLeftInside, 22, 360, color);

	
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}