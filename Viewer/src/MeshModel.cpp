#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	model_name(model_name)
{

}

MeshModel::~MeshModel()
{
}

void MeshModel::scale(int scalar)
{
	for (glm::vec3 &vector : this->vertices)
	{
		vector.x *= scalar;
		vector.y *= scalar;
		vector.z *= scalar;
	}
	return;
}
void MeshModel::move(int x, int y)
{
	for (glm::vec3& vector : this->vertices)
	{
		vector.x += x;
		vector.y += y;
	}

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