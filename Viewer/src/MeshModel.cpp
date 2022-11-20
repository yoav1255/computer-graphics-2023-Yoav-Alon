#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	objectTransform(glm::mat4(1.0)),
	worldTransform(glm::mat4(1.0)),
	translation(glm::vec3(0,0,0)),
	scale(glm::vec3(1,1,1)),
	rotation(glm::vec3(0,0,0))
{

}

MeshModel::~MeshModel()
{
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
	return model_name;
}

std::vector<glm::vec3> MeshModel::GetVertices() const
{
	return vertices;
}

void MeshModel::setVertices()
{
	glm::mat4 transMat = GetTransform();
	for (int i = 0;i < vertices.size();i++)
	{
		vertices[i] = transMat * glm::vec4(vertices[i],1);
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

  glm::mat4x4 MeshModel::GetTransform()const
{
	return worldTransform * objectTransform;
}

 glm::mat4x4 MeshModel::GetObjectTransform()
{
	return objectTransform;
}

 glm::mat4x4 MeshModel::GetWorldTransform()
{
	return worldTransform;
}

 void MeshModel::SetObjectTransform()
{
	 glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	 glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	 glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	 glm::mat4 matRotation = rotateX * rotateY * rotateZ;
	 glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), translation);
	 glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);

	 objectTransform = matTranslation * matRotation * matScale;

}

 void MeshModel::SetWorldTransform(const glm::mat4& mat)
{
	worldTransform = mat;
}
