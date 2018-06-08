#include "TerrainFPS.h"

TerrainFPS::TerrainFPS(std::string filename)
{
	data = new GridFloatData(filename);
	ncols = data->ncols;
	nrows = data->nrows;

	BgColor[0] = 0.2f;
	BgColor[1] = 0.3f;
	BgColor[2] = 0.3f;

	camera.MoveForce = 25.0f;
	terrainHeightScale = 50.0f;

	//shooter.GeneratePolygonSurface();
	std::cout << "Begin terrain creation" << std::endl;
	GenSubDividedTerrainSurface();
	std::cout << "End terrain creation" << std::endl;
}

TerrainFPS::~TerrainFPS()
{
	//delete data;
}

void do_movement(TerrainFPS terrain)
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_Q])
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (keys[GLFW_KEY_E])
		camera.ProcessKeyboard(UP, deltaTime);
	if (keys[GLFW_KEY_SPACE])
		camera.ProcessKeyboard(SPACE, deltaTime);
	if (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) 
		shifted = true;
	else
		shifted = false;
	camera.UpdatePosition(terrain.TerrainHeight(camera.Position.x + terrain.nrows / 2, camera.Position.z + terrain.ncols / 2) * terrain.terrainHeightScale, deltaTime);
	//std::cout << camera.Position.y << std::endl;

	if (keys[GLFW_KEY_TAB]) {

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
		
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	WIDTH = width;
	HEIGHT = height;

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetWindowSize(window, WIDTH, HEIGHT);
}

// Is called whenever a key is pressed/released via GLFW
void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		Wireframe = !Wireframe;
		std::cout << "Toggled Wireframe Mode" << std::endl;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		ResetCamera();
		std::cout << "Reset the camera to horizontal viewing angle" << std::endl;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		Disco = !Disco;
		std::cout << "Toggled Disco Mode" << std::endl;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		flashLight = !flashLight;
		std::cout << "Toggled Flash Light" << std::endl;
	}
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void OnMouseMoved(GLFWwindow* window, double xpos, double ypos)
{	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void OnMouseButton(GLFWwindow* window, int glfwButton, int glfwAction) {
	//mouseHeldDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		std::cout << "Fired Bullet" << std::endl;
		Bullet bullet(camera.Position, camera.Front * 100.0f, camera.Yaw, camera.Pitch);
		if (shifted)
			bullet.Position.y += 0.0f;
		else
			bullet.Position.y += 0.0f;
		bullets.push_back(bullet);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}
}

void OnScrollMoved(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset*25);
}

void ResetCamera() {
	camera.Pitch = 0.0f;
	camera.Zoom = ZOOM;
	camera.ProcessMouseMovement(0, 0);
}

void setup_window() {
	// Init GLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);


	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "Terrain FPS", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, OnKeyPressed);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)OnMouseButton);
	glfwSetCursorPosCallback(window, OnMouseMoved);
	glfwSetScrollCallback(window, OnScrollMoved);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	glfwSwapInterval(1);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
}

void setup_bullet_vao(){
	glGenVertexArrays(1, &bulletVAO);
	glGenBuffers(1, &bulletVBO);

	glBindBuffer(GL_ARRAY_BUFFER, bulletVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

	glBindVertexArray(bulletVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void update_draw_bullets(Shader myShader, TerrainFPS terrain, float deltaTime) {
	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4();

	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i].ToBeDestroyed) { 
			bullets[i].updatePosition(terrain.TerrainHeight(bullets[i].Position.x, bullets[i].Position.z), deltaTime);
			
			myShader.Use();
			modelLoc = glGetUniformLocation(myShader.Program, "model");
			viewLoc = glGetUniformLocation(myShader.Program, "view");
			projLoc = glGetUniformLocation(myShader.Program, "projection");

			model = glm::mat4();
			model = glm::translate(model, bullets[i].Position);
			if (shifted)
				model = glm::translate(model, glm::vec3(0, -1.0f, 0));
			else
				model = glm::translate(model, glm::vec3(0, -2.0f, 0));
			model = glm::rotate(model, -glm::radians(bullets[i].Yaw + 90.0f), glm::vec3(0,1,0));
			model = glm::rotate(model, glm::radians(bullets[i].Pitch), glm::vec3(1, 0, 0));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.8f));

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(bulletVAO);

			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	
}

void TerrainFPS::setup_vao() {
	// Set up vertex data (and buffer(s)) and attribute pointers
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);
	glGenBuffers(1, &terrainEBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(terrainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, 11 * (ncols * nrows + (ncols - 1) * (nrows - 1)) * sizeof(GLfloat), terVerts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * (ncols - 1) * (nrows - 1) * sizeof(GLuint), terIndices.data(), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// Texture coordinate attribute
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glBindVertexArray(0); // Unbind VAO
}

void TerrainFPS::GeneratePolygonSurface() {
	terVerts.resize(9 * ncols * nrows);
	terIndices.resize(6 * (ncols - 1) * (nrows - 1));
	int curPlace = 0;
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			terVerts[curPlace++] = i - data->nrows / 2;
			terVerts[curPlace++] = ((data->data[i][j]) - data->lowestValue) / data->range;
			terVerts[curPlace++] = j - data->ncols / 2;
			terVerts[curPlace++] = CalcColorAtElevation((data->data[i][j] - data->lowestValue) / data->range, 0);
			terVerts[curPlace++] = CalcColorAtElevation((data->data[i][j] - data->lowestValue) / data->range, 1);
			terVerts[curPlace++] = CalcColorAtElevation((data->data[i][j] - data->lowestValue) / data->range, 2);
			float hL = Height(i + 1, j);
			float hR = Height(i - 1, j);
			float hD = Height(i, j + 1);
			float hU = Height(i, j - 1);
			glm::vec3 normal(hR - hL, 1, hU - hD);
			//normal = glm::normalize(normal);
			terVerts[curPlace++] = normal.x; //x normal
			terVerts[curPlace++] = normal.y; //y normal
			terVerts[curPlace++] = normal.z; //z normal
		}
	}
	curPlace = 0;
	for (int i = 0; i < nrows - 1; i++) {
		for (int j = 0; j < ncols - 1; j++) {
			terIndices[curPlace++] = ncols*(i)+j;
			terIndices[curPlace++] = ncols*(i + 1) + (j);
			terIndices[curPlace++] = ncols*(i)+(j + 1);
			terIndices[curPlace++] = ncols*(i + 1) + (j);
			terIndices[curPlace++] = ncols*(i)+(j + 1);
			terIndices[curPlace++] = ncols*(i + 1) + (j + 1);
		}
	}
}

void TerrainFPS::GenSubDividedTerrainSurface() {
	terVerts.resize(11 * (ncols * nrows + (ncols - 1) * (nrows - 1)));
	terIndices.resize(12 * (ncols - 1) * (nrows - 1));
	int curPlace = 0;
	//on grid verts
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			terVerts[curPlace++] = i - data->nrows / 2;
			terVerts[curPlace++] = ((data->data[i][j]) - data->lowestValue) / data->range;
			terVerts[curPlace++] = j - data->ncols / 2;
			
			terVerts[curPlace++] = CalcColorAtElevation((data->data[i][j] - data->lowestValue) / data->range, 0);
			terVerts[curPlace++] = CalcColorAtElevation((data->data[i][j] - data->lowestValue) / data->range, 1);
			terVerts[curPlace++] = CalcColorAtElevation((data->data[i][j] - data->lowestValue) / data->range, 2);
			
			float hL = Height(i + 1, j);
			float hR = Height(i - 1, j);
			float hD = Height(i, j + 1);
			float hU = Height(i, j - 1);
			glm::vec3 normal(hR - hL, 1, hU - hD);
			//normal = glm::normalize(normal);
			terVerts[curPlace++] = normal.x; //x normal
			terVerts[curPlace++] = normal.y; //y normal
			terVerts[curPlace++] = normal.z; //z normal

			terVerts[curPlace++] = 0.25*i;
			terVerts[curPlace++] = 0.25*j;
		}
	}
	//subdiv verts
	for (int i = 0; i < nrows - 1; i++) {
		for (int j = 0; j < ncols - 1; j++) {
			terVerts[curPlace++] = 0.5 + i - data->nrows / 2;
			terVerts[curPlace++] = ((data->data[i][j] + data->data[i][j + 1] + data->data[i + 1][j] + data->data[i + 1][j + 1]) / 4 - data->lowestValue) / data->range;
			terVerts[curPlace++] = 0.5 + j - data->ncols / 2;
			terVerts[curPlace++] = CalcColorAtElevation(TerrainHeight(i + 0.5, j + 0.5), 0);
			terVerts[curPlace++] = CalcColorAtElevation(TerrainHeight(i + 0.5, j + 0.5), 1);
			terVerts[curPlace++] = CalcColorAtElevation(TerrainHeight(i + 0.5, j + 0.5), 2);
			glm::vec3 normalTL(Height(i - 1, j) - Height(i + 1, j), 1, Height(i, j - 1) - Height(i, j + 1));
			glm::vec3 normalTR(Height(i - 1, j + 1) - Height(i + 1, j + 1), 1, Height(i, j) - Height(i, j + 2));
			glm::vec3 normalBL(Height(i, j + 1) - Height(i + 2, j + 1), 1, Height(i + 1, j) - Height(i + 1, j + 2));
			glm::vec3 normalBR(Height(i, j) - Height(i + 2, j), 1, Height(i + 1, j - 1) - Height(i + 1, j + 1));
			glm::vec3 normal = glm::normalize((normalTL + normalTR + normalBL + normalBR));

			terVerts[curPlace++] = normal.x; //x normal
			terVerts[curPlace++] = normal.y; //y normal
			terVerts[curPlace++] = normal.z; //z normal

			terVerts[curPlace++] = 0.25*(i + 0.5f) ; //uv coord
			terVerts[curPlace++] = 0.25*(j + 0.5f) ; //uv coord
		}
	}
	curPlace = 0;
	for (int i = 0; i < nrows - 1; i++) {
		for (int j = 0; j < ncols - 1; j++) {
			terIndices[curPlace++] = ncols*(i)+(j);
			terIndices[curPlace++] = nrows*ncols + (ncols - 1)*(i)+j;
			terIndices[curPlace++] = ncols*(i)+(j + 1);
			terIndices[curPlace++] = ncols*(i)+(j + 1);
			terIndices[curPlace++] = nrows*ncols + (ncols - 1)*(i)+j;
			terIndices[curPlace++] = ncols*(i + 1)+(j + 1);
			terIndices[curPlace++] = ncols*(i + 1) + (j + 1);
			terIndices[curPlace++] = nrows*ncols + (ncols - 1)*(i)+j;
			terIndices[curPlace++] = ncols*(i + 1) + (j);
			terIndices[curPlace++] = ncols*(i + 1) + (j);
			terIndices[curPlace++] = nrows*ncols + (ncols - 1)*(i)+j;
			terIndices[curPlace++] = ncols*(i) + (j);
		}
	}
}

// Returns the correct color for the specified elevation and color channel (rgb)
float TerrainFPS::CalcColorAtElevation(float elevation, int rgb) {
	if (elevation < 0.35) {
		return colorHeightMap[0][rgb] + (elevation - 0)*((colorHeightMap[1][rgb] - colorHeightMap[0][rgb]) / (0.35 - 0));
	} else if (elevation < 0.6)
		return colorHeightMap[1][rgb] + (elevation - 0.35)*((colorHeightMap[2][rgb] - colorHeightMap[1][rgb]) / (0.6 - 0.35));
	else if (elevation < 0.8)
		return colorHeightMap[2][rgb] + (elevation - 0.6)*((colorHeightMap[3][rgb] - colorHeightMap[2][rgb]) / (0.8 - 0.6));
	return 1.2;
	
}

static float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

//Bounds check height of the grid (only works on discrete point.)
float TerrainFPS::Height(float x, float y) {
	//Check if x,y are in bounds 0,0 to ncols,nrows
	if (x < 0 && y <= 0) return ((data->data[0][0]) - data->lowestValue) / data->range;
	else if (x < 0 && y >= ncols -1) return ((data->data[0][ncols - 1]) - data->lowestValue) / data->range;
	else if (x < 0)  return ((data->data[0][y]) - data->lowestValue) / data->range;

	if (x >= nrows - 1 && y < 0) return ((data->data[nrows - 1][0]) - data->lowestValue) / data->range;
	else if (x >= nrows - 1&& y >= ncols - 1) return ((data->data[nrows - 1][ncols - 1]) - data->lowestValue) / data->range;
	else if (x >= nrows - 1) return ((data->data[nrows - 1][y]) - data->lowestValue) / data->range;

	if (y < 0) return ((data->data[x][0]) - data->lowestValue) / data->range;
	else if (y >= ncols - 1) return ((data->data[x][ncols - 1]) - data->lowestValue) / data->range;

	return ((data->data[x][y]) - data->lowestValue) / data->range;
}

//Arbitrary height algorithm, for continuous movement along the terrain (ie player position's height)
float TerrainFPS::TerrainHeight(float x, float y) {
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x > nrows - 2)
		x = nrows - 2;
	if (y > ncols - 2)
		y = ncols - 2;

	int gridx = floor(x);
	int gridy = floor(y);

	float h1 = ((data->data[gridx][gridy]) - data->lowestValue) / data->range;
	float h2 = ((data->data[gridx][gridy + 1]) - data->lowestValue) / data->range;
	float h3 = ((data->data[gridx + 1][gridy + 1]) - data->lowestValue) / data->range;
	float h4 = ((data->data[gridx + 1][gridy]) - data->lowestValue) / data->range;

	if (x - gridx >= (1 - (y - gridy))) {
		return barryCentric(glm::vec3(0, h2, 1), glm::vec3(1, h4, 0), glm::vec3(1, h3, 1), glm::vec2(x - gridx, y - gridy));
	}
	else {
		return barryCentric(glm::vec3(0, h1, 0), glm::vec3(0, h2, 1), glm::vec3(1, h4, 0), glm::vec2(x - gridx, y - gridy));
	}
}

void TerrainFPS::DrawTerrain(Shader ourShader) {

	ourShader.Use();

	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stone_texture);
	glUniform1i(glGetUniformLocation(ourShader.Program, "myTexture"), 0);
	
	GLfloat time = (GLfloat)(glfwGetTime());
	time *= 5;
	glUniform1f(glGetUniformLocation(ourShader.Program, "time"), time);

	if (Disco) {
		glUniform1f(glGetUniformLocation(ourShader.Program, "crazyStrength"), glm::sin(time));
		glClearColor(glm::sin(time), glm::cos(time), glm::tan(time), 1.0f);
	}
	else {
		glClearColor(BgColor[0], BgColor[1], BgColor[2], 1.0f);
		glUniform1f(glGetUniformLocation(ourShader.Program, "crazyStrength"), 0.0f);
	}
	GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
	glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

	// Set material properties
	glUniform3f(glGetUniformLocation(ourShader.Program, "material.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(ourShader.Program, "material.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(ourShader.Program, "material.specular"), 0.1f, 0.1f, 0.1f); // Specular doesn't have full effect on this object's material
	glUniform1f(glGetUniformLocation(ourShader.Program, "material.shininess"), 0.5f);

	// Directional light
	glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.direction"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.diffuse"), 0.7f, 0.7f, 0.7f);
	glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.specular"), 0.1f, 0.1f, 0.1f);

	// SpotLight
	glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
	glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
	glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
	if (flashLight) {
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.specular"), 0.2f, 0.2f, 0.2f);
	}	else {
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
	}
	glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.linear"), 0.09);
	glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(15.0f)));
	glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(35.0f)));

	// Set lights properties
	glm::vec3 lightColor;
	if (Disco) {
		lightColor.x = sin(glfwGetTime() * 2.25f);
		lightColor.y = sin(glfwGetTime() * 2.7f + 2);
		lightColor.z = sin(glfwGetTime() * 1.6f + 1);
	}
	else {
		lightColor.x = 1.0f;
		lightColor.y = 1.0f;
		lightColor.z = 1.0f;
	}
	//glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f); // Decrease the influence
	//glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // Low influence
	//glUniform3f(glGetUniformLocation(ourShader.Program, "light.ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
	//glUniform3f(glGetUniformLocation(ourShader.Program, "light.diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
	//glUniform3f(glGetUniformLocation(ourShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);

	// Projection 
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.01f, 100000.0f);
	// Get the uniform locations
	GLfloat angle = 20.0f;
	//model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
	//model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.05f, glm::vec3(0.0f, 0.5f, 0.0f));
	if (shifted)
		model = glm::translate(model, glm::vec3(0, -1.0f, 0));
	else 
		model = glm::translate(model, glm::vec3(0, -2.0f, 0));
	model = glm::scale(model, glm::vec3(1, terrainHeightScale, 1));
	GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
	GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
	GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(terrainVAO);

	glDrawElements(GL_TRIANGLES, 12 * (ncols - 1)*(nrows - 1), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

int main() {
	TerrainFPS shooter(inputFileName);
	setup_window();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// Set up shaders, vertex data & buffers, and attribute pointers
	Shader basicShader("BasicVS.glsl", "BasicFS.glsl"); 
	Shader bulletShader("LightVS.glsl", "LightFS.glsl");
	shooter.setup_vao();
	setup_bullet_vao();

	glGenTextures(1, &stone_texture);
	glBindTexture(GL_TEXTURE_2D, stone_texture);

	int stone_width, stone_height;
	unsigned char* image_stone = SOIL_load_image("182.png", &stone_width, &stone_height, 0, SOIL_LOAD_RGB);

	//stone_texture = SOIL_load_OGL_texture("182.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

	if (0 == image_stone)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stone_width, stone_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_stone);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(image_stone);

	// Wireframe mode
	if (Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwSetWindowTitle(window,
			std::string(std::string("Terrain FPS ") + std::to_string(1 / deltaTime).c_str()).c_str()
			);

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement(shooter);

		if (Wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		// Render
		// Clear the colorbuffer
		if (Disco) {
			glClearColor(glm::sin(glfwGetTime()*10), glm::cos(glfwGetTime()*10), glm::tan(glfwGetTime()*10), 1.0f);
		}
		else 
			glClearColor(shooter.BgColor[0], shooter.BgColor[1], shooter.BgColor[2], 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the bullets
		update_draw_bullets(bulletShader, shooter, deltaTime);

		// Draw the terrain
		shooter.DrawTerrain(basicShader);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate, clearing any resources allocated.
	glfwDestroyWindow(window);
	glfwTerminate();
	glDeleteBuffers(1, &shooter.terrainVBO);
	glDeleteBuffers(1, &shooter.terrainEBO);
	exit(EXIT_SUCCESS);
}