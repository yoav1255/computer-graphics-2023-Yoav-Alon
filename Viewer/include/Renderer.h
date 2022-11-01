#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "imgui/imgui.h"

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;

	
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawSunrise();
	void DrawCircle(const glm::ivec2& center, const int& radius,const int &stepSize, const glm::vec3& color);
	void PutSymmetricPixelsHalfDown(const int& xc, const int& yc, const int& x, const int& y, const glm::vec3& color);
	void PutSymmetricPixels(const int& xc, const int& yc, const int& x, const int& y, const glm::vec3& color);
	void DrawParameterCircle(const glm::ivec2& center, const int& radius, const glm::vec3& color,bool half=false);
	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();

	float* color_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};
