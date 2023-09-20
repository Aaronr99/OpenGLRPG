#include "MainCharacter.h"

void DebugPosition(glm::vec3 position);
glm::vec3 NormalizeToOneComponent(const glm::vec3& inputVec3);
glm::vec3 ObtainLookRotation(glm::vec3 from, glm::vec3 to);

void MainCharacter::Update() {

	if (isMoving)
	{
		if (glm::distance(transform.position, targetPos) > 0.05f)
		{
			glm::vec3 direction = transform.position - targetPos;
			transform.Move(glm::mix(transform.position, targetPos, 12.5f * GlobalData::deltaTime));
		}
		else if ((glm::distance(transform.position, targetPos) <= 0.05f))
		{
			isMoving = false;
			transform.Move(targetPos);
		}
		return;
	}

	bool wMovement = GlobalData::inputManager.GetKeyDown(GLFW_KEY_W);
	bool sMovement = GlobalData::inputManager.GetKeyDown(GLFW_KEY_S);
	bool aMovement = GlobalData::inputManager.GetKeyDown(GLFW_KEY_A);
	bool dMovement = GlobalData::inputManager.GetKeyDown(GLFW_KEY_D);

	if (wMovement || sMovement || aMovement || dMovement)
	{
		glm::vec3 front = NormalizeToOneComponent(camera->Front);
		glm::vec3 right = NormalizeToOneComponent(camera->Right);
		isMoving = true;
		if (wMovement)
		{
			targetPos = transform.position + front;
		}
		else if (sMovement)
		{
			targetPos = transform.position - front;
		}
		else if (aMovement)
		{
			targetPos = transform.position - right;
		}
		else if (dMovement)
		{
			targetPos = transform.position + right;			
		}
		glm::vec3 eulerRotation = ObtainLookRotation(transform.position, targetPos);
		transform.Rotate(-eulerRotation);
	}
}

void DebugPosition(glm::vec3 position)
{
	std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

glm::vec3 ObtainLookRotation(glm::vec3 from, glm::vec3 to)
{
	// Calcula la dirección hacia la que quieres mirar
	glm::vec3 direction = glm::normalize(to - from);

	// Usa glm::lookAt para crear una matriz de vista
	glm::mat4 viewMatrix = glm::lookAt(from, to, glm::vec3(0.0f, 1.0f, 0.0f));

	// Extrae la rotación de la matriz de vista
	glm::quat rotation = glm::quat_cast(viewMatrix);

	// Convierte la rotación a grados si lo deseas
	glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(rotation));

	return eulerRotation;
}

glm::vec3 NormalizeToOneComponent(const glm::vec3& inputVec3)
{
	// Encuentra el índice de la componente más grande
	int maxIndex = 0;

	float xValue = inputVec3.x;
	if (xValue < 0) xValue *= -1;
	float yValue = inputVec3.y;
	if (yValue < 0) yValue *= -1;
	float zValue = inputVec3.z;
	if (zValue < 0) zValue *= -1;

	float maxValue = xValue;

	if (yValue > maxValue)
	{
		maxIndex = 1;
		maxValue = yValue;
	}

	if (zValue > maxValue)
	{
		maxIndex = 2;
	}

	glm::vec3 result(0.0f);

	if (inputVec3[maxIndex] < 0)
	{
		result[maxIndex] = -1.0f;
	}
	else
	{
		result[maxIndex] = 1.0f;
	}

	return result;
}

