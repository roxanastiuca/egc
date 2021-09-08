#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	// Initialize meshes:
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Initialize shader programs:
	{
		Shader *shader = new Shader("ShaderTema2");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader *shader = new Shader("ShaderTema2_2D");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader2D.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader *shader = new Shader("ShaderTema2_Noise");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShaderNoise.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShaderNoise.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Initialize camera:
	{
		camera = new Tema::Camera();
		// Tilt camera a bit to see the platforms better:
		camera->RotateThirdPerson_OX(-20 * (float)M_PI / 180);
		projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	}

	// Initialize platforms:
	{
		// Initialize first long row:
		for (int i = 1; i <= 3; i++) {
			platforms[i][0] = platform(leftmostPoint + glm::vec3((i - 1) * 2.1f, 0.0f, 0.0f), 10);
			platforms_count[i] = 1;
		}

		// Randomly generate next rows:
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j < 10; j++) {
				GeneratePlatform(i, j);
			}
		}
	}
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(background.x, background.y, background.z, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	if (!game_over) {
		UpdateGameStatus(deltaTimeSeconds);
		UpdateTimes(deltaTimeSeconds);
		UpdateBall(deltaTimeSeconds);
		UpdatePlatforms(deltaTimeSeconds);
	}
}

// Update all elements related to game status (fuel bar and score):
void Tema2::UpdateGameStatus(float deltaTimeSeconds) {
	// Update fuel:
	fuel -= deltaTimeSeconds * platforms_velocity * 0.005;

	// Check if out of fuel:
	if (fuel <= 0) {
		ball_is_out_of_gas = true;
		platforms_velocity = 0;
		background = colors["black"];

		time_without_fuel += deltaTimeSeconds;

		if (time_without_fuel > 3) {
			GameOver();
		}
	}

	// Update score (= distance crossed):
	score += platforms_velocity * deltaTimeSeconds;

	// Render actual fuel bar:
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, fuel_bar_position + glm::vec3(-(fuel_max - fuel) / 20, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel / 10, 0.1, 0.01));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, colors["green"]);
	}

	// Render fuel bar - target:
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, fuel_bar_position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel_max / 10, 0.1, 0.01));
		if (ball_is_out_of_gas) {
			// Ball is out of fuel, show fuel bar as dark red:
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, colors["dark_red"]);
		}
		else {
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, colors["grey"]);
		}
	}
}

// Update time variabiles:
void Tema2::UpdateTimes(float deltaTimeSeconds) {
	// Update last special spawn time:
	last_special_in += deltaTimeSeconds;

	if (speed_blocked) {
		// Update time since stepping on orange platform.
		speed_blocked_time += deltaTimeSeconds;

		if (speed_blocked_time >= 5) {
			// After 5 sec, return speed back to normal:
			speed_blocked = false;
			speed_blocked_time = 0;
			platforms_velocity = old_speed;
		}
	}
}

// Update all events regarding the ball, as well as render the ball:
void Tema2::UpdateBall(float deltaTimeSeconds) {
	// Check if ball is in a special event:
	if (ball_is_falling) {
		// Ball has fallen from the platforms and is in the process of
		// falling:
		ball_velocity_oy += deltaTimeSeconds * gravity;
		ball_position += deltaTimeSeconds * 5 * ball_velocity_oy;

		if (firstPersonMode) {
			// If first person mode, make player feel like they're falling:
			camera->TranslateUpword(deltaTimeSeconds * ball_velocity_oy.y);
		}

		if (ball_position.y < -1.5) {
			// Ball is out of the screen, we can stop the game:
			GameOver();
		}
	}
	else if (ball_is_dying) {
		// Ball has stepped on a red platform and is dying:
		ball_dying_time += deltaTimeSeconds;

		if (ball_dying_time > 3) {
			// Animation for dying is complete. We can stop the game.
			GameOver();
		}

		// Render ball using special shader and special "scary" color:
		RenderBall(shaders["ShaderTema2_Noise"], colors["dark_red"]);
		return;
	}
	else if (ball_is_out_of_gas) {
		// Ball is out of fuel. Render ball using special "scary" color:
		RenderBall(shaders["ShaderTema2"], colors["dark_red"]);
		return;
	}

	if (ball_is_changing_columns) {
		if (ball_next_column > ball_column) {
			// Move it towards the column on the right:
			ball_position += ball_velocity_ox * deltaTimeSeconds;

			if (firstPersonMode) {
				// Move camera as well, for first person mode.
				camera->TranslateRight(deltaTimeSeconds * ball_velocity_ox.x);
			}

			if (ball_position.x >= columns_ox[ball_next_column]) {
				// Ball has reached the new column.
				ball_is_changing_columns = false;
				ball_column = ball_next_column;
				ball_position.x = columns_ox[ball_column];
			}
		}
		else {
			// Move it towards the column on the left:
			ball_position -= ball_velocity_ox * deltaTimeSeconds;

			if (firstPersonMode) {
				// Move camera as well, for first person mode.
				camera->TranslateRight(-deltaTimeSeconds * ball_velocity_ox.x);
			}

			if (ball_position.x <= columns_ox[ball_next_column]) {
				// Ball has reached the new column.
				ball_is_changing_columns = false;
				ball_column = ball_next_column;
				ball_position.x = columns_ox[ball_column];
			}
		}
	}

	if (ball_is_jumping) {
		// Jumping movement, using gravity, for a realistic jump.
		ball_position += deltaTimeSeconds * ball_velocity_oy;

		if (firstPersonMode) {
			camera->TranslateUpword(deltaTimeSeconds * ball_velocity_oy.y);
		}

		ball_velocity_oy += deltaTimeSeconds * gravity;

		if (ball_velocity_oy.y < 0 && ball_position.y <= 0.55) {
			// Ball is the process of going down and it reached its initial place. Stop
			// the movement:
			ball_is_jumping = false;
			ball_position.y = 0.55;
		}
	}

	if (!ball_is_jumping) {
		// Ball is not jumping so it must be on a platform. Check for collisions:
		string platform_color = "void";

		// Only check for collisions with platforms on the same column as ball:
		int i = ball_is_changing_columns ? ball_next_column : ball_column;

		for (int j = 0; j < platforms_count[i]; j++) {
			float start = platforms[i][j].pos.z - platforms[i][j].len / 2 - 0.2;
			float end = platforms[i][j].pos.z + platforms[i][j].len / 2 + 0.2;

			if (start <= ball_position.z && ball_position.z <= end) {
				// Collision found:
				platform_color = platforms[i][j].color;
				platforms[i][j].color = "purple";
			}
		}

		if (platform_color == "void") {
			// No collision was found. Ball has fallen off the platforms:
			platforms_velocity = 0;
			ball_is_falling = true;
			ball_velocity_oy = glm::vec3(0, 0, 0);
			background = colors["black"];
		}
		else if (platform_color == "red") {
			// Ball has stepped on red platform => death.
			platforms_velocity = 0;
			ball_is_dying = true;
			ball_dying_time = 0;
			background = colors["black"];
		}
		else if (platform_color == "orange") {
			// Ball has stepped on orange platform => high speed for a while.

			if (speed_blocked) {
				// Ball was allready locked at high speed, just reset timer.
				speed_blocked_time = 0;
			}
			else {
				old_speed = platforms_velocity;
				platforms_velocity = high_speed;
				speed_blocked = true;
				speed_blocked_time = 0;
			}
		}
		else if (platform_color == "green") {
			// Green platform => more fuel.
			fuel = fuel + 3;
			if (fuel > fuel_max)	fuel = fuel_max;
		}
		else if (platform_color == "yellow") {
			// Yellow platform => less fuel.
			fuel = fuel - 2;
			if (fuel < 0)	fuel = 0;
		}
	}

	RenderBall(shaders["ShaderTema2"], colors["pink"]);
}

// Render ball using specified shader program and color.
void Tema2::RenderBall(Shader *shader, glm::vec3 color) {
	if (!firstPersonMode) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, ball_position);
		RenderSimpleMesh(meshes["sphere"], shader, modelMatrix, color);
	}
}

// Update all platforms:
void Tema2::UpdatePlatforms(float deltaTimeSeconds) {
	// Keep speed between limits:
	if (platforms_velocity > platforms_max_velocity) {
		platforms_velocity = platforms_max_velocity;
	}
	if (platforms_velocity < platforms_min_velocity && !ball_is_dying && !ball_is_falling && !ball_is_out_of_gas) {
		platforms_velocity = platforms_min_velocity;
	}

	for (int i = 1; i <= 3; i++) {
		for (int j = 0; j < platforms_count[i]; j++) {
			// Update position:
			platforms[i][j].pos.z += deltaTimeSeconds * platforms_velocity;

			// Check if platform is behind character and no longer can be seen:
			if (platforms[i][j].pos.z > platforms_max_z) {
				// Delete platform:
				for (int k = j; k < platforms_count[i] - 1; k++) {
					platforms[i][k] = platforms[i][k + 1];
				}
				j--;
				platforms_count[i]--;
				continue;
			}

			// Don't render yet platforms which are more than a set distance away:
			if (platforms[i][j].pos.z < platforms_min_z) {
				continue;
			}

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, platforms[i][j].pos);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(platforms[i][j].width, platforms[i][j].height, platforms[i][j].len));
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, colors[platforms[i][j].color]);
		}

		if (platforms_count[i] < 10) {
			// If platforms on this column are fewer than 10, generate a new one:
			GeneratePlatform(i, platforms_count[i]);
		}
	}
}

// Generate new platform for column i at index j:
void Tema2::GeneratePlatform(int i, int j) {
	// Coordinate x is column-specific:
	float x = columns_ox[i];

	// Coordinate y is same for all platforms:
	float y = leftmostPoint.y;

	// Get a random length between [7, 16]:
	float len = (float)(rand() % 10) + 7;

	// Get a random gap between the new platform and the one before it on the column:
	float gap = (float)(rand() % 3) + 1;

	// Compute coordinate z:
	float z = platforms[i][j - 1].pos.z - platforms[i][j - 1].len / 2 - len / 2 - gap;

	platforms[i][j] = platform(x, y, z, len);
	platforms_count[i]++;

	// From time to time, generate special platform:
	if (last_special_in >= next_special_in) {
		if (fuel < fuel_max / 2) {
			// A green platform is needed:
			platforms[i][j].color = "green";
		}
		else {
			// Special platform color is random between green/yellow/red/orange:
			platforms[i][j].color = platform_colors[rand() % 4];
		}

		last_special_in = 0;

		// Set timer for next spawn of special platform as a random number between [2, 5]:
		next_special_in = (float)(rand() % 3) + 2;
	}
}

// All losing events are over and we can stop the game:
void Tema2::GameOver() {
	game_over = true;

	// Print score:
	cout << "Game over! Score: " << (int)score << '\n';
}

void Tema2::FrameEnd()
{
}

void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind object color
	int loc_object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(loc_object_color, 1, glm::value_ptr(color));

	// Bind model matrix
	int loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind noise time (which will be the time since ball started dying):
	int loc_noise_time = glGetUniformLocation(shader->program, "noise_time");
	glUniform1f(loc_noise_time, ball_dying_time);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W) && !speed_blocked && !ball_is_falling && !ball_is_dying && !ball_is_out_of_gas) {
		// Increase speed. W works only if there is not a special event happening.
		platforms_velocity += 5 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_S) && !speed_blocked && !ball_is_falling && !ball_is_dying && !ball_is_out_of_gas) {
		// Decrease speed. S works only if there is not a special event happening.
		platforms_velocity -= 5 * deltaTime;
	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_C) {
		// Change between third person mode and first person mode:
		if (!firstPersonMode) {
			camera->Set(ball_position + glm::vec3(0, 1, 0), ball_position + glm::vec3(0, 1, -1), glm::vec3(0, 1, 0));
			// Tilt camera for a better view:
			camera->RotateFirstPerson_OX(-20 * (float)M_PI / 180);
			firstPersonMode = true;
		}
		else {
			camera->Set(glm::vec3(0, 4, 5.25), glm::vec3(0, 4, 4.25), glm::vec3(0, 1, 0));
			// Tilt camera for a better view:
			camera->RotateThirdPerson_OX(-20 * (float)M_PI / 180);
			firstPersonMode = false;
		}
	}

	if (key == GLFW_KEY_D && !ball_is_falling && !ball_is_dying && !ball_is_out_of_gas) {
		// Change column. D works only if no losing events are happening.
		ball_is_changing_columns = true;
		ball_next_column = ball_column + 1;
	}

	if (key == GLFW_KEY_A && !ball_is_falling && !ball_is_dying && !ball_is_out_of_gas) {
		// Change column. D works only if no losing events are happening.
		ball_is_changing_columns = true;
		ball_next_column = ball_column - 1;
	}

	if (key == GLFW_KEY_SPACE && !ball_is_jumping && !ball_is_falling && !ball_is_dying && !ball_is_out_of_gas) {
		// Start jump movement. SPACE works only if no losing events are happening or the ball is not already jumping.
		ball_is_jumping = true;
		ball_velocity_oy = ball_initial_velocity_oy;
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
