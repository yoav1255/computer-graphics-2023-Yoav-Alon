#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render( Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void ClearZBuffer();
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void SetNewViewport(float frameBufferWidth, float frameBufferHeight);
	
private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color);
	void DrawLineReversedAxis(int x1, int y1, int x2, int y2, const glm::vec3& color);
	void DrawLine_CalcZ(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec3& color,glm::vec3 t1, glm::vec3 t2, glm::vec3 t3);
	void DrawLineReversedAxis_CalcZ(int x1, int y1, int x2, int y2, const glm::vec3& color,glm::vec3 t1, glm::vec3 t2, glm::vec3 t3);
	void drawSomeFlowers();
	void putFlower(int radius, int x, const glm::vec3& color , int stemLen);
	void drawModel( MeshModel& myModel,Scene& scene);
	void drawBoundingBox(MeshModel& myModel, Scene& scene, const glm::vec3& color,bool isWorld,float x_Min,float y_Min,float z_Min,float x_Max, float y_Max,float z_Max);
	void drawAxisLocal(MeshModel& myModel, Scene& scene, const glm::vec3& color, float x_Min, float y_Min, float z_Min, float x_Max, float y_Max, float z_Max);
	void drawAxisWorld(const glm::vec3& color);
	void drawVerticeNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color);
	void drawFacesNormals(MeshModel& myModel, Scene& scene, const glm::vec3& color);
	void drawRectangle(glm::vec3& verticeModel0, glm::vec3 &verticeModel1, glm::vec3 &verticeModel2);
	void fill_left_Triangle(glm::vec3 v_min, glm::vec3 v_mid, glm::vec3 v_max, glm::vec3 color);
	void fill_right_Triangle(glm::vec3 v_min, glm::vec3 v_mid, glm::vec3 v_max, glm::vec3 color);
	void edgeWalking(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, const glm::vec3& color);
	float Find_Current_Point(glm::vec2 &p,glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3);
	void drawZBuffer(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3);

	float At(int x, int y);
	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();

	float* color_buffer;
	float* z_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};
