#include "Tema1.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"

using namespace std;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 axes_origin(0, 0, 0);

	// Creating necessary meshes:
	Mesh* bow = Object2D::CreateBow("bow", axes_origin, bow_radius, bow_thickness, bow_brown_color);
	AddMeshToList(bow);

	Mesh* arrow = Object2D::CreateArrow("arrow", axes_origin + glm::vec3(0, - arrow_thickness / 2, 0), arrow_length, arrow_thickness, arrow_brown_color);
	AddMeshToList(arrow);

	Mesh* shuriken = Object2D::CreateShuriken("shuriken", axes_origin, shuriken_side, grey_color);
	AddMeshToList(shuriken);

	Mesh* red_balloon = Object2D::CreateBalloon("red_balloon", axes_origin, balloon_width, balloon_height, balloon_neckLength, red_color);
	AddMeshToList(red_balloon);

	Mesh* yellow_balloon = Object2D::CreateBalloon("yellow_balloon", axes_origin, balloon_width, balloon_height, balloon_neckLength, yellow_color);
	AddMeshToList(yellow_balloon);

	Mesh* balloon_string = Object2D::CreateBalloonString("balloon_string", axes_origin, balloon_string_length, white_color);
	AddMeshToList(balloon_string);

	Mesh* power_bar = Object2D::CreateRectangle("power_bar", axes_origin, 5, 10, orange_color);
	AddMeshToList(power_bar);

	Mesh* score_bar = Object2D::CreateRectangle("score_bar", axes_origin + glm::vec3(0.5, 0, 0), 1, 10, green_color);
	AddMeshToList(score_bar);

	Mesh* target_score_bar = Object2D::CreateRectangle("target_score_bar", axes_origin + glm::vec3(100, 0, 0), 200, 10, dark_grey_color);
	AddMeshToList(target_score_bar);

	Mesh *red_heart = Object2D::CreateHeart("red_heart", axes_origin, 28, 30, red_color);
	AddMeshToList(red_heart);

	Mesh* figure = Object2D::CreateStickFigure("figure", axes_origin, 150, black_color);
	AddMeshToList(figure);

	game_score = 0; // start game with score 0
}

void Tema1::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(46.0 / 255, 139.0 / 255, 87.0 / 255, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
	if (game_lives > 0)  {
		// Player still has lives left. Update all game components:
		UpdateGameStatus(deltaTimeSeconds);
		UpdateBow(deltaTimeSeconds);
		UpdateArrow(deltaTimeSeconds);
		UpdateShurikens(deltaTimeSeconds);
		UpdateBalloons(deltaTimeSeconds);
	} else {
		if (restart == true) {
			// Restart game.
			restart = false;
			game_lives = 3;
			game_score = 0;
			shurikens_in_use = red_balloons_in_use = yellow_balloons_in_use = 0;
		} else {
			// Player has lost. Show red screen with 'dead' stick figure, symbolizing game over.
			glClearColor(0.5, 0.0, 0.0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(window->GetResolution().x / 2, window->GetResolution().y / 2);
			modelMatrix *= Transform2D::Rotate(M_PI / 2);
			modelMatrix *= Transform2D::Scale(3, 3);
			RenderMesh2D(meshes["figure"], shaders["VertexColor"], modelMatrix);
		}
	}
}

/**
	* Update the components for keeping track of game status:
	- score disaplyed periodically;
	- level determined based on score;
	- score bar and target score bar (greyed out score bar that used must fill in order to complete the game);
	- hearts for lives left.
**/
void Tema1::UpdateGameStatus(float deltaTimeSeconds) {
	// Periodically display score:
	last_show_score += deltaTimeSeconds;
	if (last_show_score > next_show_score) {
		last_show_score = 0;
		cout << "Level: " << level+1 << "; Score: " << game_score << "." << endl;
	}

	// Determine level, based on score:
	level = (game_score < 25) ? 0 :
		(game_score < 50) ? 1 :
		(game_score < 100) ? 2 :
		(game_score < 150) ? 3 :
		4;

	// Display score bar:
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(10, 690);
	float scale = (game_score < 200) ? game_score : 200;
	scale = (scale < 0) ? 0 : scale;
	modelMatrix *= Transform2D::Scale(scale, 1);
	RenderMesh2D(meshes["score_bar"], shaders["VertexColor"], modelMatrix);

	// Display score target:
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(10, 690);
	RenderMesh2D(meshes["target_score_bar"], shaders["VertexColor"], modelMatrix);

	// Display hearts for number of lives left:
	float heart_x = 240, heart_y = 680;
	for (int i = 0; i < game_lives; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix = Transform2D::Translate(heart_x, heart_y);
		RenderMesh2D(meshes["red_heart"], shaders["VertexColor"], modelMatrix);
		heart_x += 40;
	}
}

/**
	* Update the bow and archer positions.
**/
void Tema1::UpdateBow(float deltaTimeSeconds) {
	// Display bow, by translating and rotating it:
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(bow_x, bow_y);
	modelMatrix *= Transform2D::Rotate(bow_angle);
	RenderMesh2D(meshes["bow"], shaders["VertexColor"], modelMatrix);

	// Display archer, translating it to be next to bow:
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(bow_x, bow_y);
	RenderMesh2D(meshes["figure"], shaders["VertexColor"], modelMatrix);
}

/**
	* Update the arrow's position. Also, check for collision with shurikens and balloons.
**/
void Tema1::UpdateArrow(float deltaTimeSeconds) {
	// Get new position of arrow:
	if (arrow_moving) {
		// Arrow is moving with arrow_speed.
		arrow_x += arrow_speed * cos(arrow_angle);
		arrow_y += arrow_speed * sin(arrow_angle);
		if (arrow_x > window->GetResolution().x || arrow_y > window->GetResolution().y || arrow_y < 0) {
			// Arrow left screen. Stop its movement and place it back on bow.
			arrow_moving = false;
			arrow_x = bow_x;
			arrow_y = bow_y;
			arrow_angle = bow_angle;
			arrow_speed = 0;
		}
	} else {
		// Arrow is placed on bow and follows its movements.
		arrow_x = bow_x;
		arrow_y = bow_y;
		arrow_angle = bow_angle;
	}

	// Display arrow:
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(arrow_x, arrow_y);
	modelMatrix *= Transform2D::Rotate(arrow_angle);
	RenderMesh2D(meshes["arrow"], shaders["VertexColor"], modelMatrix);

	// Display power bar if user is holding left mouse button:
	if (arrow_speed != 0 && !arrow_moving) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(bow_x, bow_y - bow_radius - bow_thickness - 60);
		modelMatrix *= Transform2D::Scale(arrow_speed, 1);
		RenderMesh2D(meshes["power_bar"], shaders["VertexColor"], modelMatrix);
	}

	// Get arrowhead coordinates:
	float arrowhead_x = arrow_x + arrow_length * cos(arrow_angle);
	float arrowhead_y = arrow_y + arrow_length * sin(arrow_angle);

	// Check collisions if arrow is moving:
	if (arrow_moving) {

		// Check collisions arrow-balloon:
		for (int i = 0; i < red_balloons_in_use + yellow_balloons_in_use; i++) {
			if (balloons[i].is_shrinking_and_falling) {
				continue; // skip for balloons already hit
			}

			float center_x = balloons[i].x;
			float center_y = balloons[i].y + balloon_height / 2;

			if (CheckCollisionPointEllipse(arrowhead_x, arrowhead_y, center_x, center_y, balloon_width / 2, balloon_height / 2)) {
				if (balloons[i].is_yellow) {
					game_score += collision_points_yb; // collision with yellow balloon (negative value => reduce score)
				} else {
					game_score += collision_points_rb; // collision with red balloon
				}
				balloons[i].is_shrinking_and_falling = true;
			}
		}

		// Check collisions arrow-shuriken:
		for (int i = 0; i < shurikens_in_use; i++) {
			if (shurikens[i].is_falling) {
				continue; // skip for shurikens already hit
			}

			if (CheckCollisionPointEllipse(arrowhead_x, arrowhead_y, shurikens[i].x, shurikens[i].y, shuriken_side, shuriken_side)) {
				game_score += collision_points_sh;
				shurikens[i].is_falling = true;
			}
		}
	}
}

/**
	* Update the shurikens. Check for collision with bow. Spawn new shurikens if maximum (for current level) wasn't reached.
**/
void Tema1::UpdateShurikens(float deltaTimeSeconds) {
	// Update already spawned shurikens:
	for (int i = 0; i < shurikens_in_use; i++) {
		// Determine new coordinates:
		if (shurikens[i].is_falling) {
			shurikens[i].y -= deltaTimeSeconds * shuriken_falling_speed;
		} else {
			shurikens[i].x -= deltaTimeSeconds * shuriken_speed[level];
			shurikens[i].angle += deltaTimeSeconds;

			// If shuriken is not falling (it wasn't hit by an arrow), check for collision with bow:
			if (CheckCollisionCircleCircle(bow_x, bow_y, bow_radius, shurikens[i].x, shurikens[i].y, shuriken_side)) {
				game_lives--; // lost one life
				shurikens[i].is_falling = true;

				cout << "Shuriken hit bow! Lives left: " << game_lives << "." << endl;

				if (game_lives <= 0) {
					// Game over.
					cout << "Game over! Final score: " << game_score << "." << endl;
					cout << "Press SPACE to restart game!" << endl;
				}
			}
		}

		if (shurikens[i].x <= -shuriken_side || shurikens[i].y <= -shuriken_side) {
			// Shuriken is outside of window. Disable it:
			for (int j = i; j < shurikens_in_use - 1; j++) {
				shurikens[j] = shurikens[j + 1];
			}
			shurikens_in_use--;
			i--;
		} else {
			// Shuriken is still in use. Display it:
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(shurikens[i].x, shurikens[i].y);
			modelMatrix *= Transform2D::Rotate(shurikens[i].angle);
			RenderMesh2D(meshes["shuriken"], shaders["VertexColor"], modelMatrix);
		}
	}

	// Spawn a new shuriken after next_shuriken_spawn units of time, if maximum of shurikens in use for
	// current level hasn't been reached yet:
	last_shuriken_spawn += deltaTimeSeconds;
	if (last_shuriken_spawn > next_shuriken_spawn && shurikens_in_use < max_shurikens_in_use[level]) {
		int idx = shurikens_in_use++;
		shurikens[idx].x = window->GetResolution().x + shuriken_side * 2;
		shurikens[idx].y = rand() % (int)(window->GetResolution().y - 2 * shuriken_side) + shuriken_side;
		shurikens[idx].angle = 0;
		shurikens[idx].is_falling = false;
		last_shuriken_spawn = 0;

		// In order to have shurikens spawn more randomly, randomize the number of units of time
		// until next spawn:
		next_shuriken_spawn = (float)(rand() % (int)max_seconds_between_shurikens[level]) + 1;
	}
}

/**
	* Update balloons. Spawn new red and yellow balloons if maximum (for current level) wasn't reached.
**/
void Tema1::UpdateBalloons(float deltaTimeSeconds) {
	// Update already spawned balloons:
	for (int i = 0; i < red_balloons_in_use + yellow_balloons_in_use; i++) {
		// Determine new coordinates:
		if (balloons[i].is_shrinking_and_falling) {
			balloons[i].scale -= deltaTimeSeconds * 2.5;
			balloons[i].y -= deltaTimeSeconds * balloon_falling_speed;
		} else {
			balloons[i].y += deltaTimeSeconds * balloon_speed[level];
		}

		if (balloons[i].y > window->GetResolution().y + balloon_height || balloons[i].scale <= 0) {
			// Balloon is outside of window or it completely shrinked. Disable it:
			bool is_yellow = balloons[i].is_yellow;

			for (int j = i; j < red_balloons_in_use + yellow_balloons_in_use - 1; j++) {
				balloons[j] = balloons[j + 1];
			}

			if (is_yellow)	yellow_balloons_in_use--;
			else			red_balloons_in_use--;
			i--;
		} else {
			// Balloon is still in use. Display it:
			string mesh_name = balloons[i].is_yellow ? "yellow_balloon" : "red_balloon";
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(balloons[i].x, balloons[i].y);
			modelMatrix *= Transform2D::Scale(balloons[i].scale, balloons[i].scale);
			RenderMesh2D(meshes[mesh_name], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_string"], shaders["VertexColor"], modelMatrix);
		}
	}

	// Spawn a new red balloons after next_shuriken_spawn units of time, if maximum of red balloons in use
	// hasn't been reached yet:
	last_red_balloon_spawn += deltaTimeSeconds;
	if (last_red_balloon_spawn > next_red_balloon_spawn && red_balloons_in_use < max_red_balloons_in_use) {
		// Spawn a red balloon:
		int idx = yellow_balloons_in_use + red_balloons_in_use;
		red_balloons_in_use++;
		int bow_area_x = bow_x + bow_radius + 2 * bow_thickness; // don't spawn any balloons in this area (reserved for bow)
		balloons[idx].x = rand() % (int)(window->GetResolution().y - balloon_width - bow_area_x) + balloon_width + bow_area_x + 200;
		balloons[idx].y = -balloon_height;
		balloons[idx].is_yellow = false;
		balloons[idx].is_shrinking_and_falling = false;
		balloons[idx].scale = 1;
		last_red_balloon_spawn = 0;

		// In order to have red balloons spawn more randomly, randomize the number of units of time
		// until next spawn:
		next_red_balloon_spawn = (float)(rand() % (int)max_seconds_between_red_balloons) + 0.5;
	}

	// Spawn a new yellow balloon after next_shuriken_spawn units of time, if maximum of yellow balloons in use for
	// current level hasn't been reached yet:
	last_yellow_balloon_spawn += deltaTimeSeconds;
	if (last_yellow_balloon_spawn > next_yellow_balloon_spawn && yellow_balloons_in_use < max_yellow_balloons_in_use[level]) {
		// Spawn a yellow balloon:
		int idx = yellow_balloons_in_use + red_balloons_in_use;
		yellow_balloons_in_use++;
		int bow_area_x = bow_x + bow_radius + 2 * bow_thickness; // don't spawn any balloons in this area (reserved for bow)
		balloons[idx].x = rand() % (int)(window->GetResolution().y - balloon_width - bow_area_x) + balloon_width + bow_area_x + 200;
		balloons[idx].y = -balloon_height;
		balloons[idx].is_yellow = true;
		balloons[idx].is_shrinking_and_falling = false;
		balloons[idx].scale = 1;
		last_yellow_balloon_spawn = 0;

		// In order to have yellow balloons spawn more randomly, randomize the number of units of time
		// until next spawn:
		next_yellow_balloon_spawn = (float)(rand() % (int)max_seconds_between_yellow_balloons) + 0.5;
	}
}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W) == true) {
		// Move bow up.
		bow_y += deltaTime * bow_speed;
		if (bow_y > bow_y_max) {
			bow_y = bow_y_max;
		}
	}

	if (window->KeyHold(GLFW_KEY_S) == true) {
		// Move bow down.
		bow_y -= deltaTime * bow_speed;
		if (bow_y < bow_y_min) {
			bow_y = bow_y_min;
		}
	}

	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) == true && !arrow_moving) {
		// Increase speed with time button is pressed.
		if (arrow_speed < arrow_speed_max) {
			arrow_speed += 50 * deltaTime;
		}
	}

	if (window->KeyHold(GLFW_KEY_SPACE) == true && game_lives <= 0) {
		restart = true;
	}
}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Restirct bow to only face east.
	if (mouseX < bow_x) {
		// Mouse is to the left of the bow.
		bow_angle = 0;
	} else {
		// Bow follows mouse movement.
		bow_angle = atan(((window->GetResolution().y - mouseY) - bow_y) / (mouseX - bow_x));
	}

}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) == false && arrow_speed > 0) {
		arrow_moving = true; // arrow starts moving
		arrow_speed += 5; // +5 so the arrow is not agonizingly slow
	}
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}

/**
	* Checks if a point is inside an ellipse.
**/
bool Tema1::CheckCollisionPointEllipse(float point_x, float point_y, float center_x, float center_y, float radius_OX, float radius_OY) {
	// check if ellipse equation is true: (point_x-center_x)^2/radius_OX^2 + (point_y-center_y)^2/radius_OY^2 <= 1
	// <=> (point_x-center_x)^2 * radius_OY^2 + (point_y-center_y)^2 * radius_OX^2 <= radius_OX^2 * radius_OY^2
	float left_eq = (point_x - center_x) * (point_x - center_x) * radius_OY * radius_OY;
	left_eq += (point_y - center_y) * (point_y - center_y) * radius_OX * radius_OX;

	float right_eq = radius_OX * radius_OX * radius_OY * radius_OY;

	return (left_eq <= right_eq);
}

/**
	* Checks if 2 circles intersect.
**/
bool Tema1::CheckCollisionCircleCircle(float c1_x, float c1_y, float c1_radius, float c2_x, float c2_y, float c2_radius) {
	// check if equation is true: dist(c1, c2) < c1_radius + c2_radius
	float dx = c1_x - c2_x;
	float dy = c1_y - c2_y;
	float left_eq = sqrt(dx * dx + dy * dy);

	float right_eq = c1_radius + c2_radius;

	return (left_eq < right_eq);
}