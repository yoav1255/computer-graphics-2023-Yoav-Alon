#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render( Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawLineReversedAxis(int x1, int y1, int x2, int y2, const glm::vec3& color);
	void drawSomeFlowers();
	void putFlower(int radius, int x, const glm::vec3& color , int stemLen);
	void drawModel( MeshModel& myModel,Scene& scene);
	void drawBoundingBox(MeshModel& myModel, Scene& scene, const glm::vec3& color,bool isWorld);
	void drawAxis(MeshModel& myModel, Scene& scene, const glm::vec3& color, bool isWorld);
	void drawVerticeNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color);


	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();

	float* color_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};
