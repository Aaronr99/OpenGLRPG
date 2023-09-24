#include "LightObject.h"

void LightObject::Awake()
{
	glGenVertexArrays(1, lightVao);
	glBindVertexArray(*lightVao);

	glGenBuffers(1, lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LightObject::Update()
{
	std::string pointIdentifier = "pointLights[" + std::to_string(lightID) + "]";
	lightShader->use();
	lightShader->setVec3(pointIdentifier + ".position", transform.position);
	lightShader->setVec3(pointIdentifier + ".ambient", 0.05f, 0.05f, 0.05f);
	lightShader->setVec3(pointIdentifier + ".diffuse", 0.8f, 0.8f, 0.8f);
	lightShader->setFloat(pointIdentifier + ".constant", 1.0f);
	lightShader->setFloat(pointIdentifier + ".linear", 0.09f);
	lightShader->setFloat(pointIdentifier + ".quadratic", 0.032f);
}

void LightObject::Render()
{
	objectShader.use();
	glm::mat4 projection = glm::perspective(glm::radians(GlobalData::camera.Zoom), (float)GlobalData::SCR_WIDTH / (float)GlobalData::SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = GlobalData::camera.GetViewMatrix();
	objectShader.setMat4("projection", projection);
	objectShader.setMat4("view", view);
	// we now draw as many light bulbs as we have point lights.
	glBindVertexArray(*lightVao);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::scale(model, glm::vec3(0.5f)); // Make it a smaller cube
	objectShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
