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

	{
		Mesh* mesh = new Mesh("oildrum_center");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "oildrum_center.obj");
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
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader_2D.glsl", GL_FRAGMENT_SHADER);
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

	// Initialize textures:
	
	// Texture for background:
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/background2.png", GL_CLAMP_TO_BORDER);
		mapTextures["background"] = texture;
	}

	// Texture for fence:
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/fence2.jpg", GL_REPEAT);
		mapTextures["fence"] = texture;
	}

	// Texture for coin:
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/gold.jpg", GL_REPEAT);
		mapTextures["gold"] = texture;
	}

	// Texture for ball:
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/ball.jpg", GL_REPEAT);
		mapTextures["ball"] = texture;
	}

	// Texture for blue platform: (normal) - stone
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/grass.jpg", GL_REPEAT);
		mapTextures["blue"] = texture;
	}
	
	// Texture for red platform: (death) - flame
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/red_flowers.jpg", GL_REPEAT);
		mapTextures["red"] = texture;
	}

	// Texture for purple platform: (walking on it) - dirt
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/grass2.png", GL_CLAMP_TO_BORDER);
		mapTextures["purple"] = texture;
	}

	// Texture for yellow platform: (less fuel) - sane
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/sand.png", GL_CLAMP_TO_BORDER);
		mapTextures["yellow"] = texture;
	}

	// Texture for orange platform: (speed) - pavement
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/stone.jpg", GL_CLAMP_TO_BORDER);
		mapTextures["orange"] = texture;
	}

	// Texture for green plaform: (more fuel) - grass
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/flowers.jpg", GL_CLAMP_TO_BORDER);
		mapTextures["green"] = texture;
	}

	// Texture for wall obstacle: (warning sign)
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema2/Textures/cows.png", GL_CLAMP_TO_BORDER);
		mapTextures["warning"] = texture;
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
			for (int j = 1; j < 2; j++) {
				GeneratePlatform(i, j);
			}
		}
	}

	// Initialize fences:
	{
		fences_oz[0][0] = -1;
		fences_oz[1][0] = -1.3;
		lamp_type[0][0] = lamp_type[1][0] = 0;

		for (int i = 1; i < 20; i++) {
			fences_oz[0][i] = fences_oz[0][i-1] - 4;
			lamp_type[0][i] = i % 5 == 0 ? (rand() % 2 + 1) : 0;
			if (lamp_type[0][i] != 0) {
				GenerateLight(0, i, fences_oz[0][i] - 1.95);
			}

			fences_oz[1][i] = fences_oz[1][i - 1] - 4;
			lamp_type[1][i] = i % 5 == 2 ? (rand() % 2 + 1) : 0;
			if (lamp_type[1][i] != 0) {
				GenerateLight(1, i, fences_oz[1][i] - 1.95);
			}
		}
		fences_count[0] = fences_count[1] = 20;
	}

	// Light & material properties:
	// Light & material properties:
	{
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 0.3;
		materialKd = 1.2;
		materialKs = 0.2;
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
	UpdateBackground();

	if (!game_over) {
		UpdateGameStatus(deltaTimeSeconds);
		UpdateTimes(deltaTimeSeconds);
		UpdateBall(deltaTimeSeconds);
		UpdatePlatforms(deltaTimeSeconds);
		UpdateCollectibles(deltaTimeSeconds);
		UpdateLights(deltaTimeSeconds);
		UpdateFences(deltaTimeSeconds);
		UpdateObstacles(deltaTimeSeconds);
	}
}

// /////////////// TEMA 3 ///////////////////////
void Tema2::UpdateBackground() {
	// Render box for background:
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -40,30));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(200, 100, 200));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["background"], false);
}

void Tema2::UpdateFences(float deltaTimeSeconds) {
	for (int col = 0; col < 2; col++) {
		for (int i = 0; i < fences_count[col]; i++) {
			fences_oz[col][i] += deltaTimeSeconds * platforms_velocity;

			// Check if fence is behind character and no longer can be seen:
			if (fences_oz[col][i] > platforms_max_z) {
				// Delete fence:
				for (int k = i; k < fences_count[col] - 1; k++) {
					fences_oz[col][k] = fences_oz[col][k + 1];
					lamp_type[col][k] = lamp_type[col][k + 1];
				}
				i--;
				fences_count[col]--;
				continue;
			}

			// Don't render yet platforms which are more than a set distance away:
			if (fences_oz[col][i] < platforms_min_z) {
				continue;
			}

			float x = col == 0 ? 4.2f : -4.2f;

			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(x, fence_height1 - 0.1, fences_oz[col][i]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(fence_width2, fence_height2, fence_length2));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
			}
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(x, fence_height1 / 2, fences_oz[col][i]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(fence_width2, fence_height2, fence_length2));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
			}
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(x, fence_height1 / 2, fences_oz[col][i] + fence_length2 / 2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(fence_width1, fence_height1, fence_width1));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
			}
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(x, fence_height1 / 2, fences_oz[col][i] - fence_length2 / 2));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(fence_width1, fence_height1, fence_width1));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
			}

			// Render a grass platform underneath:
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(x, leftmostPoint.y, fences_oz[col][i]));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1, 0.1, fence_length2 + 1));
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["green"], true);

			// Render a lamp next to it:
			if (lamp_type[col][i] != 0) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(x, lamp_height / 2, fences_oz[col][i] - 1.95));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3, lamp_height, 0.3));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);

				if (lamp_type[col][i] == 2) {
					// This will be a spotlight.
					glm::mat4 modelMatrix = glm::mat4(1);
					if (col == 0) {
						modelMatrix = glm::translate(modelMatrix, glm::vec3(x - lamp_length / 2 + 0.15, lamp_height, fences_oz[col][i] - 1.95));
					} else {
						modelMatrix = glm::translate(modelMatrix, glm::vec3(x + lamp_length / 2 - 0.15, lamp_height, fences_oz[col][i] - 1.95));
					}
					modelMatrix = glm::scale(modelMatrix, glm::vec3(lamp_length, 0.3, 0.3));
					RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
				}
			}
		}

		if (fences_count[col] < 20) {
			fences_oz[col][fences_count[col]] = fences_oz[col][fences_count[col] - 1] - 4;
			lamp_type[col][fences_count[col]] = lamp_type[col][fences_count[col] - 5] == 0 ? 0 : (rand() % 2 + 1);
			if (lamp_type[col][fences_count[col]] != 0) {
				GenerateLight(col, fences_count[col], fences_oz[col][fences_count[col]] - 1.95);
			}
			fences_count[col]++;
		}

	}
}

void Tema2::UpdateCollectibles(float deltaTimeSeconds) {
	for (int col = 1; col <= 3; col++) {
		for (int i = 0; i < collectible_count[col]; i++) {
			collectible_oz[col][i] += platforms_velocity * deltaTimeSeconds;
			collectible_angle[col][i] += deltaTimeSeconds * 100;

			// Check if collectible is behind character and no longer can be seen:
			if (collectible_oz[col][i] > platforms_max_z) {
				// Delete collectible:
				for (int k = i; k < collectible_count[col] - 1; k++) {
					collectible_oz[col][k] = collectible_oz[col][k + 1];
					collectible_angle[col][k] = collectible_angle[col][k + 1];
				}
				i--;
				collectible_count[col]--;
				continue;
			}

			// Don't render yet collectibles which are more than a set distance away:
			if (collectible_oz[col][i] < platforms_min_z) {
				continue;
			}

			// Check collision with ball:
			if (ball_position.x == columns_ox[col] && abs(collectible_oz[col][i] - ball_position.z) < 0.5) {
				score += 1;
				// Delete collectible:
				for (int k = i; k < collectible_count[col] - 1; k++) {
					collectible_oz[col][k] = collectible_oz[col][k + 1];
					collectible_angle[col][k] = collectible_angle[col][k + 1];
				}
				collectible_count[col]--;
				i--;
				continue;
			}

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(columns_ox[col], 0.75, collectible_oz[col][i]));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(collectible_angle[col][i]), glm::vec3(1, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(150, 10, 150));
			RenderSimpleMesh(meshes["oildrum_center"], shaders["ShaderTema2"], modelMatrix, mapTextures["gold"], true);

		}
	}
}

void Tema2::UpdateObstacles(float deltaTimeSeconds) {
	for (int col = 1; col <= 3; col++) {
		for (int i = 0; i < obstacle_count[col]; i++) {
			obstacle_oz[col][i] += deltaTimeSeconds * platforms_velocity;

			// Check if obstacle is behind character and no longer can be seen:
			if (obstacle_oz[col][i] > platforms_max_z) {
				// Delete obstacle:
				for (int k = i; k < obstacle_count[col] - 1; k++) {
					obstacle_oz[col][k] = obstacle_oz[col][k + 1];
				}
				obstacle_count[col]--;
				i--;
				continue;
			}

			// Don't render yet obstacles which are more than a set distance away:
			if (obstacle_oz[col][i] < platforms_min_z) {
				continue;
			}

			// Check collision with ball:
			if (abs(ball_position.x - columns_ox[col]) < 1.5 &&
				abs(ball_position.y - leftmostPoint.y) < (0.5 + obstacle_height) &&
				abs(ball_position.z - obstacle_oz[col][i]) < 0.6) {
				// Ball is dying:
				platforms_velocity = 0;
				ball_is_dying = true;
				ball_dying_time = 0;
				background = colors["black"];
			}

			// Render legs:
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(columns_ox[col] + 0.9, 0.2, obstacle_oz[col][i]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.4, 0.1));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
			}
			{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(columns_ox[col] - 0.9, 0.2, obstacle_oz[col][i]));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.4, 0.1));
				RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["fence"], true);
			}

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(columns_ox[col], obstacle_height / 2 + 0.4, obstacle_oz[col][i]));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(2, obstacle_height, 0.1));
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures["warning"], true);
		}
	}
}

void Tema2::GenerateLight(int col, int i, int z) {
	if (light_count < 10 && rand() % 3 == 0) {
		if (lamp_type[col][i] == 1) {
			float x = col == 0 ? 4.2f : -4.2f;

			// Generate a punctiform light source.
			light_positions[light_count] = glm::vec3(x, lamp_height + 0.3, fences_oz[col][i] - 1.95);
			is_spotlight[light_count] = 0;
			light_color[light_count] = light_colors_arr[rand() % 6];
			light_count++;
		}
		else if (lamp_type[col][i] == 2) {
			float x = col == 0 ? 4.2f - lamp_length + 0.3 : -4.2f + lamp_length - 0.3;

			// Generate a spotlight source.
			light_positions[light_count] = glm::vec3(x, lamp_height - 0.4, fences_oz[col][i] - 1.95);
			is_spotlight[light_count] = 1;
			light_color[light_count] = light_colors_arr[rand() % 6];
			light_count++;
		}
	}
}

void Tema2::UpdateLights(float deltaTimeSeconds) {
	for (int i = 0; i < light_count; i++) {
		light_positions[i].z += deltaTimeSeconds * platforms_velocity;

		if (light_positions[i].z > platforms_max_z) {
			for (int k = i; k < light_count - 1; k++) {
				light_positions[k] = light_positions[k + 1];
				is_spotlight[k] = is_spotlight[k + 1];
				light_color[k] = light_color[k + 1];	
			}
			light_count--;
			i--;
			continue;
		}

		if (light_positions[i].z < platforms_min_z) {
			continue;
		}

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, light_positions[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.6, 0.6, 0.6));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, mapTextures["gold"], false);
	}
}
// //////////////////////////////////////////////

// Update all elements related to game status (fuel bar and score bar):
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

	// Render actual fuel bar:
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, fuel_bar_position + glm::vec3(-(fuel_max - fuel) / 20, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel / 10, 0.1, 0.01));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, NULL, false, colors["green"]);
	}

	// Render fuel bar - target:
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, fuel_bar_position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel_max / 10, 0.1, 0.01));
		if (ball_is_out_of_gas) {
			// Ball is out of fuel, show fuel bar as dark red:
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, NULL, false, colors["dark_red"]);
		} else {
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, NULL, false, colors["grey"]);
		}
	}

	float score_aux = score > 90 ? 90 : score;
	// Render score bar:
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, score_bar_position + glm::vec3(-(score_max - score_aux) / 200, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(score_aux / 100, 0.1, 0.01));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, NULL, false, colors["pink"]);
	}

	// Render score bar - target:
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, score_bar_position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(score_max / 100, 0.1, 0.01));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2_2D"], modelMatrix, NULL, false, colors["grey"]);
	}
}

// Update time variabiles:
void Tema2::UpdateTimes(float deltaTimeSeconds) {
	// Update last special spawn time:
	last_special_in += deltaTimeSeconds;

	// Update last collectible spawn time:
	last_collectible_in += deltaTimeSeconds;

	// Update last obstacle spawn time:
	last_obstacle_in += deltaTimeSeconds;
	
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
	} else if (ball_is_dying) {
		// Ball has stepped on a red platform and is dying:
		ball_dying_time += deltaTimeSeconds;

		if (ball_dying_time > 3) {
			// Animation for dying is complete. We can stop the game.
			GameOver();
		}

		// Render ball using special shader and special "scary" color:
		RenderBall(shaders["ShaderTema2_Noise"], colors["dark_red"]);
		return;
	} else if (ball_is_out_of_gas) {
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
		} else {
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
		} else if (platform_color == "red") {
			// Ball has stepped on red platform => death.
			platforms_velocity = 0;
			ball_is_dying = true;
			ball_dying_time = 0;
			background = colors["black"];
		} else if (platform_color == "orange") {
			// Ball has stepped on orange platform => high speed for a while.

			if (speed_blocked) {
				// Ball was allready locked at high speed, just reset timer.
				speed_blocked_time = 0;
			} else {
				old_speed = platforms_velocity;
				platforms_velocity = high_speed;
				speed_blocked = true;
				speed_blocked_time = 0;
			}
		} else if (platform_color == "green") {
			// Green platform => more fuel.
			fuel = fuel + 3;
			if (fuel > fuel_max)	fuel = fuel_max;
		} else if (platform_color == "yellow") {
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
		RenderSimpleMesh(meshes["sphere"], shader, modelMatrix, mapTextures["ball"], true, color);
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
			RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, mapTextures[platforms[i][j].color], true);
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
		} else {
			// Special platform color is random between green/yellow/red/orange:
			platforms[i][j].color = platform_colors[rand() % 4];
		}

		last_special_in = 0;

		// Set timer for next spawn of special platform as a random number between [2, 5]:
		next_special_in = (float)(rand() % 3) + 2;
	}

	// Add collectible on platform
	if (collectible_count[i] < 20 && last_collectible_in >= next_collectible_in) {
		collectible_oz[i][collectible_count[i]] = platforms[i][j].pos.z;
		collectible_angle[i][collectible_count[i]] = 0;
		collectible_count[i]++;

		last_collectible_in = 0;

		// Set timer for next spawn of collectible as a random number between [0.5, 2.5]:
		next_collectible_in = (float)(rand() % 3) + 0.5;
	} else if (obstacle_count[i] < 20 && last_obstacle_in >= next_obstacle_in) {
		obstacle_oz[i][obstacle_count[i]] = platforms[i][j].pos.z;
		obstacle_count[i]++;

		last_obstacle_in = 0;

		// Set timer for next spawn of obstacle as a random number between [0.5, 3.5]:
		next_obstacle_in = (float)(rand() % 4) + 0.5;
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

void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix,
						Texture2D* texture, bool use_light, glm::vec3 color)
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

	// Bind texture
	if (texture != NULL) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
	}

	// Lighting
	if (use_light) {
		// Set shader uniforms for light & material properties
		// TODO: Set light position uniforms
		int aux = min(light_count, 5);
		int lights_count_location = glGetUniformLocation(shader->program, "lights_count");
		glUniform1i(lights_count_location, aux);

		// LIGHT 1
		int light_position1 = glGetUniformLocation(shader->program, "light_position1");
		glUniform3f(light_position1, light_positions[0].x, light_positions[0].y, light_positions[0].z);

		int is_spot_location1 = glGetUniformLocation(shader->program, "is_spot1");
		glUniform1i(is_spot_location1, is_spotlight[0]);

		int light_color1_location = glGetUniformLocation(shader->program, "light_color1");
		glUniform3f(light_color1_location, light_color[0].x, light_color[0].y, light_color[0].z);

		// LIGHT 2
		int light_position2 = glGetUniformLocation(shader->program, "light_position2");
		glUniform3f(light_position2, light_positions[1].x, light_positions[1].y, light_positions[1].z);

		int is_spot_location2 = glGetUniformLocation(shader->program, "is_spot2");
		glUniform1i(is_spot_location2, is_spotlight[1]);

		int light_color2_location = glGetUniformLocation(shader->program, "light_color2");
		glUniform3f(light_color2_location, light_color[1].x, light_color[1].y, light_color[1].z);

		// LIGHT 3
		int light_position3 = glGetUniformLocation(shader->program, "light_position3");
		glUniform3f(light_position3, light_positions[2].x, light_positions[2].y, light_positions[2].z);

		int is_spot_location3 = glGetUniformLocation(shader->program, "is_spot3");
		glUniform1i(is_spot_location3, is_spotlight[2]);

		int light_color3_location = glGetUniformLocation(shader->program, "light_color3");
		glUniform3f(light_color3_location, light_color[2].x, light_color[2].y, light_color[2].z);

		// LIGHT 4
		int light_position4 = glGetUniformLocation(shader->program, "light_position4");
		glUniform3f(light_position4, light_positions[3].x, light_positions[3].y, light_positions[3].z);

		int is_spot_location4 = glGetUniformLocation(shader->program, "is_spot4");
		glUniform1i(is_spot_location4, is_spotlight[3]);

		int light_color4_location = glGetUniformLocation(shader->program, "light_color4");
		glUniform3f(light_color4_location, light_color[3].x, light_color[3].y, light_color[3].z);

		// LIGHT 5
		int light_position5 = glGetUniformLocation(shader->program, "light_position5");
		glUniform3f(light_position5, light_positions[4].x, light_positions[4].y, light_positions[4].z);

		int is_spot_location5 = glGetUniformLocation(shader->program, "is_spot5");
		glUniform1i(is_spot_location5, is_spotlight[4]);

		int light_color5_location = glGetUniformLocation(shader->program, "light_color5");
		glUniform3f(light_color5_location, light_color[4].x, light_color[4].y, light_color[4].z);


		// LIGHT DIRECTION
		int light_direction = glGetUniformLocation(shader->program, "light_direction");
		glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

		// TODO: Set eye position (camera position) uniform
		//glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
		glm::vec3 eyePosition = camera->position;
		int eye_position = glGetUniformLocation(shader->program, "eye_position");
		glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

		// TODO: Set material property uniforms (shininess, kd, ks, object color) 
		int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
		glUniform1i(material_shininess, materialShininess);

		int material_kd = glGetUniformLocation(shader->program, "material_kd");
		glUniform1f(material_kd, materialKd);

		int material_ks = glGetUniformLocation(shader->program, "material_ks");
		glUniform1f(material_ks, materialKs);

		int object_color = glGetUniformLocation(shader->program, "object_color");
		glUniform3f(object_color, color.r, color.g, color.b);
	}
	else {
		// Only use ambient light:
		int aux = 0;
		int lights_count_location = glGetUniformLocation(shader->program, "lights_count");
		glUniform1i(lights_count_location, aux);
	}

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
		} else {
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
