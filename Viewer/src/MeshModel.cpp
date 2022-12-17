#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	model_name(model_name),
	faces(faces),
	vertices(vertices),
	normals(normals),
	objectTransform(glm::mat4(1.0)),
	worldTransform(glm::mat4(1.0)),
	translationObject(glm::vec3(0, 0, 0)),
	scaleObject(glm::vec3(1, 1, 1)),
	rotationObject(glm::vec3(0, 0, 0)),
	translationWorld(glm::vec3(0, 0, 0)),
	scaleWorld(glm::vec3(1, 1, 1)),
	rotationWorld(glm::vec3(0, 0, 0)),
	vCenter(glm::vec3(0, 0, 0)),
	axisLocal(false),
	axisWorld(false),
	bounding_box_local(false),
	bounding_box_world(false),
	drawVerticeNormals(false),
	drawFaceNormals(false)
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

void MeshModel::setVertices(const glm::mat4 mat)
{
	for (int i = 0;i < vertices.size();i++)
	{
		vertices[i] = mat * glm::vec4(vertices[i],1);
	}
}
glm::vec3 MeshModel::GetVCenter(int half_width,int half_height)
{
	float x_Min = vertices[0].x;
	float y_Min = vertices[0].y;
	float z_Min = vertices[0].z;

	float x_Max = vertices[0].x;
	float y_Max = vertices[0].y;
	float z_Max = vertices[0].z;
	for (int i = 0; i < GetVertices().size(); i++)
	{
		x_Min = std::min(GetVertices()[i][0], x_Min);
		y_Min = std::min(GetVertices()[i][1], y_Min);
		z_Min = std::min(GetVertices()[i][2], z_Min);
		x_Max = std::max(GetVertices()[i][0], x_Max);
		y_Max = std::max(GetVertices()[i][1], y_Max);
		z_Max = std::max(GetVertices()[i][2], z_Max);
	}


	vCenter.x = ((x_Min + x_Max) / 2.0f) ;
	vCenter.y = ((y_Min + y_Max) / 2.0f) ;
	vCenter.z = (z_Min + z_Max) / 2.0f;
	return vCenter;

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

  glm::mat4x4 MeshModel::GetTransform()
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
	 glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.x), glm::vec3(1.0f, 0.0f, 0.0f));
	 glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.y), glm::vec3(0.0f, 1.0f, 0.0f));
	 glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.z), glm::vec3(0.0f, 0.0f, 1.0f));

	 glm::mat4 matRotation = rotateX * rotateY * rotateZ;
	 glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), translationObject);
	 glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scaleObject);

	 objectTransform = matTranslation * matRotation * matScale;
}

 void MeshModel::SetWorldTransform()
{
	 glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.x), glm::vec3(1.0f, 0.0f, 0.0f));
	 glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.y), glm::vec3(0.0f, 1.0f, 0.0f));
	 glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.z), glm::vec3(0.0f, 0.0f, 1.0f));

	 glm::mat4 matRotation = rotateX * rotateY * rotateZ;
	 glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), translationWorld);
	 glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scaleWorld);

	 worldTransform = matTranslation * matRotation * matScale;
 }
