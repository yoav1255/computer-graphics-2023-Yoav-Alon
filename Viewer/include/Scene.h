#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Camera.h"

class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<std::shared_ptr<PointLight>> lights;
	AmbientLight ambientLight;
	std::vector<Camera> cameras;
	int active_light_index;
	int activeCameraIndex;
	int activeModelIndex;

public:
	Scene();

	void AddModel(const std::shared_ptr<MeshModel>& model);
	int GetModelCount() const;
	std::shared_ptr<MeshModel> GetModel(int index) const;
	Light& GetActiveLight();
	MeshModel& GetModel2(int index) const;
	void AddCamera(const Camera& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	const Camera& GetCamera(int index) const;
	MeshModel& GetActiveModel2() const;
	void AddLight(const std::shared_ptr<PointLight>& light);
	int GetLightCount() const;
	std::shared_ptr<PointLight> GetLight(int index) const;
	const std::vector<std::shared_ptr<PointLight>>& GetActiveLights() const;
	const int GetActiveLightIndex() const;
	const AmbientLight& GetAmbientLight();

	const Camera& GetActiveCamera() const;
	Camera& GetActiveCamera();

	void SetActiveCameraIndex(int index);
	void SetActiveLightIndex(int index);
	const int GetActiveCameraIndex() const;

	const std::shared_ptr<MeshModel>& GetActiveModel() const;

	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;
};