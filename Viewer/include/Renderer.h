#pragma once
#include "Scene.h"
#include <random>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void clearZBuffer();
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void SetNewViewport(float frameBufferWidth, float frameBufferHeight);

private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color
		, bool fillTriangle=false ,glm::vec3 &v1=glm::vec3(0,0,0), glm::vec3& v2= glm::vec3(0, 0, 0), glm::vec3& v3= glm::vec3(0, 0, 0));
	void DrawLineReversedAxis(int x1, int y1, int x2, int y2, const glm::vec3& color, 
		bool fillTriangle = false, glm::vec3& v1 = glm::vec3(0, 0, 0), glm::vec3& v2 = glm::vec3(0, 0, 0), glm::vec3& v3 = glm::vec3(0, 0, 0));
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& aI, const glm::vec3& bI, glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3 colorv1, glm::vec3 colorv2, glm::vec3 colorv3);
	void DrawLineReversedAxis(int x1, int y1, int x2, int y2, const glm::vec3& aI, const glm::vec3& bI, glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3 colorv1, glm::vec3 colorv2, glm::vec3 colorv3);
	//void putFlower(int radius, int x, const glm::vec3& color, int stemLen);
	void Renderer::DrawCircle(const glm::ivec2& center, const int& radius, const int& stepSize, const glm::vec3& color);
	void drawModel(MeshModel& myModel, Scene& scene);
	void drawBoundingBox(MeshModel& myModel, Scene& scene, const glm::vec3& color, bool isWorld, float x_Min, float y_Min, float z_Min, float x_Max, float y_Max, float z_Max);
	void drawAxisLocal(MeshModel& myModel, Scene& scene, const glm::vec3& color, float x_Min, float y_Min, float z_Min, float x_Max, float y_Max, float z_Max);
	void drawAxisWorld(const glm::vec3& color);
	void drawVerticeNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color);
	glm::vec3 drawFacesNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color,bool draw=true);
	void drawTriangles(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& color);
	void colorBottomTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& color);
	void colorTopTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& color);
	float calculateZ(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, float x, float y);
	void drawRectangle(glm::vec3& verticeModel0, glm::vec3& verticeModel1, glm::vec3& verticeModel2);
	void shadeTrianglesGouraud(std::vector<glm::vec3> intensities, std::vector<glm::vec3> vertices);
	void colorTopTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& v1I, glm::vec3& v2I, glm::vec3& v3I);
	void colorBottomTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& v1I, glm::vec3& v2I, glm::vec3& v3I);



	//glm::vec3 diffuseColor(MeshModel& myModel, light &light_test,)
	
	bool testAndSetZBuffer(int x, int y,float z);
	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();
	float* z_buffer;
	float* color_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};
