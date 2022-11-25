#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	model_name(model_name),
	local(glm::mat4(1)),
	world(glm::mat4(1)),
	scaleLocal((1, 1, 1)),
	translateLocal((0, 0, 0)),
	rotateLocal((0, 0, 0)),
	scaleWorld((1, 1, 1)),
	translateWorld((0, 0, 0)),
	rotateWorld((0, 0, 0))
{
	//scaleMat = glm::scale(glm::vec3(scaleLocal.x, scaleLocal.y, scaleLocal.z));
}

MeshModel::~MeshModel()
{
}


void MeshModel::setLocal()
{
	glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotateLocal.x), glm::vec3(1, 0, 0));
	glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotateLocal.y), glm::vec3(0, 1, 0));
	glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotateLocal.z), glm::vec3(0, 0, 1));
	glm::mat4 rotationMatrix = xRotation * yRotation * zRotation;
	glm::mat4 transalationMatrix = glm::translate(glm::mat4(1.0f), translateLocal);
	glm::mat4 scaleMatrix = glm::scale(scaleLocal);
	
	this->local = transalationMatrix * rotationMatrix * scaleMatrix;

}
glm::mat4 MeshModel::getLocal()
{
	return this->local;
}
void MeshModel::setWorld()
{
	glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotateWorld.x), glm::vec3(1, 0, 0));
	glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotateWorld.y), glm::vec3(0, 1, 0));
	glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotateWorld.z), glm::vec3(0, 0, 1));
	glm::mat4 rotationMatrix = xRotation * yRotation * zRotation;
	glm::mat4 transalationMatrix = glm::translate(glm::mat4(1.0f), translateWorld);
	glm::mat4 scaleMatrix = glm::scale(scaleWorld);

	this->world = transalationMatrix * rotationMatrix * scaleMatrix;

}
glm::mat4 MeshModel::getWorld()
{
	return this->world;
}


const Face& MeshModel::GetFace(int index) const
{
	return faces[index];
}
glm::vec3 MeshModel::GetVertexAt(int index)
{
	if (index >= vertices.size())
	{
		std::cout << "Vertex index can only be less than " << vertices.size() << std::endl;
		return glm::vec3();
	}
	return vertices.at(index);
}
int MeshModel::GetFacesCount() const
{
	return faces.size();
}

const std::string& MeshModel::GetModelName() const
{
	return model_name;
}
int MeshModel::getVerticesCount() const
{
	return vertices.size();

}

std::ostream& operator<< (std::ostream& out, MeshModel& mesh)
{
	int verSize = mesh.vertices.size();
	int faceSize = mesh.faces.size();
	std::size_t index = mesh.model_name.find_last_of("/");
	std::string name = mesh.model_name.substr(index+1);
	//std::cout << "There are " << verSize << " vertices" << std::endl;
	std::cout << "The " << name << " vertices are : " << std::endl;
	for (int i = 0; i < verSize; i++)
	{
		std::cout << mesh.vertices.at(i).x << " , " << mesh.vertices.at(i).y << " , " <<
			mesh.vertices.at(i).z << std::endl;
	}
	std::cout << "The " << name << " faces are : " << std::endl;
	for (int i = 0; i < faceSize; i++)
	{
		std::cout << mesh.faces.at(i).GetVertexIndex(0) << " " << mesh.faces.at(i).GetVertexIndex(1) <<
			" " << mesh.faces.at(i).GetVertexIndex(2) << std::endl;
	}

	return out;
}