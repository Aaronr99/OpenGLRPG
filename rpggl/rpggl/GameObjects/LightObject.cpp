#include "LightObject.h"

void LightObject::Awake()
{
	glGenVertexArrays(1, lightVao);
	glBindVertexArray(*lightVao);

	glGenBuffers(1, lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LightObject::Render()
{
	/*shader.use();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	// we now draw as many light bulbs as we have point lights.
	glBindVertexArray(*lightVao);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::scale(model, glm::vec3(0.5f)); // Make it a smaller cube
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);*/
}
