#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <glm/gtc/matrix_transform.hpp>

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName) :
	modelName(modelName),
	objectTransform(glm::mat4(1.0)),
	worldTransform(glm::mat4(1.0))
{
	worldTransform = glm::mat4x4(1);
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0, 1);
	color = glm::vec3(dist(mt), dist(mt), dist(mt));

	modelVertices.reserve(3 * faces.size());
	for (int i = 0; i < faces.size(); i++)
	{
		Face currentFace = faces.at(i);
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = currentFace.GetVertexIndex(j) - 1;

			Vertex vertex;
			vertex.position = vertices[vertexIndex];
			vertex.normal = normals[vertexIndex];

			if (textureCoords.size() > 0)
			{
				int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
				vertex.textureCoords = textureCoords[textureCoordsIndex];
			}

			modelVertices.push_back(vertex);
		}
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
	

}
const std::vector<Vertex>& MeshModel::GetModelVertices()
{
	return modelVertices;
}
void MeshModel::initNormals()
{
	glm::vec3 normal, v;
	int counter = 0, i = 0;
	for (i = 0, v = this->vertices[i], counter = 0; i < this->vertices.size(); i++)
	{
		normal = glm::vec3(0.0f, 0.0f, 0.0f);
		for (int j = 0; j < this->GetFacesCount(); j++)
		{
			for (int t = 0; t < 3; t++)
				if (i + 1 == this->GetFace(j).GetVertexIndex(t))
				{
					glm::vec3 v1 = vertices[this->GetFace(j).GetVertexIndex(0)-1], v2 = vertices[this->GetFace(j).GetVertexIndex(1)-1],
						v3 = vertices[this->GetFace(j).GetVertexIndex(2)-1];
					normal += glm::normalize(glm::cross(v2 - v1, v3 - v1));
					counter++;

				}
		}
		normal = glm::normalize(normal / float(counter));


	}
}


MeshModel::~MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}
const glm::vec3& MeshModel::GetColor() const
{
	return color;
}

void MeshModel::SetColor(const glm::vec3& color)
{
	this->color = color;
}

const Face& MeshModel::GetFace(int index) const
{
	return faces[index];
}

int MeshModel::GetFacesCount() const
{
	return faces.size();
}

const std::string& MeshModel::GetModelName() const
{
	return modelName;
}

std::vector<glm::vec3> MeshModel::GetVertices() const
{
	return vertices;
}

void MeshModel::setVertices(const glm::mat4 mat)
{
	for (int i = 0;i < vertices.size();i++)
	{
		vertices[i] = mat * glm::vec4(vertices[i],1);
	}
}

std::ostream& operator<<(std::ostream &os, const std::shared_ptr<MeshModel>& myModel)
{
	os << "Vertices : " << std::endl;
	os << std::endl;
	
	for (int i = 0;i < myModel->GetVertices().size();i++)
	{
		os << "Vertice " << i << " : ";
		for (int j = 0;j < 3;j++)
			os << myModel->GetVertices()[i][j] << " ";
		os << std::endl;

	}
	os << std::endl;

	os << "Faces : " << std::endl;
	os << std::endl;


	for (int i = 0;i < myModel->GetFacesCount();i++)
	{
		os << "Face " << i << " : ";
		for (int j = 0;j < 3;j++)
			os << myModel->GetFace(i).GetVertexIndex(j) << " ";
		os << std::endl;

	}
	return os;
}

const glm::mat4 MeshModel::GetRotationObjectMat() const
{
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.z), glm::vec3(0.0f, 0.0f, 1.0f));

	return rotateX * rotateY * rotateZ;
}
const glm::mat4 MeshModel::GetTranslationObjectMat() const
{
	return glm::translate(glm::mat4(1.0f), translationObject);

}
const glm::mat4 MeshModel::GetScaleObjectMat() const
{
	return glm::scale(glm::mat4(1.0f), scaleObject);
}
const glm::mat4 MeshModel::GetRotationWorldMat() const
{
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.z), glm::vec3(0.0f, 0.0f, 1.0f));

	return rotateX * rotateY * rotateZ;
}
const glm::mat4 MeshModel::GetTranslationWorldMat() const
{
	return glm::translate(glm::mat4(1.0f), translationWorld);
}
const glm::mat4 MeshModel::GetScaleWorldMat() const
{
	return glm::scale(glm::mat4(1.0f), scaleWorld);
}
GLuint MeshModel::GetVAO() const
{
	return vao;
}

  glm::mat4 MeshModel::GetTransform()
{
	return worldTransform * objectTransform;
}

 glm::mat4 MeshModel::GetObjectTransform()
{
	return objectTransform;
}

 glm::mat4 MeshModel::GetWorldTransform()
{
	return worldTransform;
}

 void MeshModel::SetObjectTransform()
{
	 objectTransform = GetTranslationObjectMat() * GetRotationObjectMat() * GetScaleObjectMat();
}

 void MeshModel::SetWorldTransform()
{
	 worldTransform = GetTranslationWorldMat() * GetRotationWorldMat() * GetScaleWorldMat();

 }
 void MeshModel::TranslateModel(const glm::vec3& translationVector)
 {
	objectTransform = Utils::TranslationMatrix(translationVector) * objectTransform;
 }

 void MeshModel::TranslateWorld(const glm::vec3& translationVector)
 {
	 worldTransform = Utils::TranslationMatrix(translationVector) * worldTransform;
 }

 void MeshModel::RotateXModel(double angle)
 {
	 objectTransform = Utils::XRotationMatrix(angle) * objectTransform;
 }

 void MeshModel::RotateYModel(double angle)
 {
	 objectTransform = Utils::YRotationMatrix(angle) * objectTransform;
 }

 void MeshModel::RotateZModel(double angle)
 {
	 objectTransform = Utils::ZRotationMatrix(angle) * objectTransform;
 }

 void MeshModel::ScaleXModel(double factor)
 {
	 objectTransform = Utils::XScalingMatrix(factor) * objectTransform;
 }

 void MeshModel::ScaleYModel(double factor)
 {
	 objectTransform = Utils::YScalingMatrix(factor) * objectTransform;
 }

 void MeshModel::ScaleZModel(double factor)
 {
	 objectTransform = Utils::ZScalingMatrix(factor) * objectTransform;
 }

 void MeshModel::ScaleModel(double factor)
 {
	 objectTransform = glm::scale(glm::mat4(1), glm::vec3(factor, factor, factor)) * objectTransform;
 }

 void MeshModel::RotateXWorld(double angle)
 {
	 worldTransform = Utils::XRotationMatrix(angle) * worldTransform;
 }

 void MeshModel::RotateYWorld(double angle)
 {
	 worldTransform = Utils::YRotationMatrix(angle) * worldTransform;
 }

 void MeshModel::RotateZWorld(double angle)
 {
	 worldTransform = Utils::ZRotationMatrix(angle) * worldTransform;
 }

 void MeshModel::ScaleXWorld(double factor)
 {
	 worldTransform = Utils::XScalingMatrix(factor) * worldTransform;
 }

 void MeshModel::ScaleYWorld(double factor)
 {
	 worldTransform = Utils::YScalingMatrix(factor) * worldTransform;
 }

 void MeshModel::ScaleZWorld(double factor)
 {
	 worldTransform = Utils::ZScalingMatrix(factor) * worldTransform;
 }

 void MeshModel::ScaleWorld(double factor)
 {
	 worldTransform = glm::scale(glm::mat4(1), glm::vec3(factor, factor, factor)) * worldTransform;
 }
