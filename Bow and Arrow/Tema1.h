#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

#include "FloatingObjects.h"

// Colors used for objects:
const glm::vec3 arrow_brown_color(61.0 / 255, 43.0 / 255, 31.0 / 255);
const glm::vec3 bow_brown_color(101.0 / 255, 67.0 / 255, 33.0 / 255);
const glm::vec3 yellow_color(1, 1, 0);
const glm::vec3 red_color(1, 0, 0);
const glm::vec3 white_color(1, 1, 1);
const glm::vec3 green_color(0, 1, 0);
const glm::vec3 black_color(0, 0, 0);
const glm::vec3 grey_color(211.0 / 255, 211.0 / 255, 211.0 / 255);
const glm::vec3 dark_grey_color(105.0 / 255, 105.0 / 255, 105.0 / 255);
const glm::vec3 orange_color(1.0, 165.0 / 255, 0);

class Tema1 : public SimpleScene
{
	public:
		Tema1();
		~Tema1();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void UpdateGameStatus(float deltaTimeSeconds);
		void UpdateBow(float deltaTimeSeconds);
		void UpdateArrow(float deltaTimeSeconds);
		void UpdateShurikens(float deltaTimeSeconds);
		void UpdateBalloons(float deltaTimeSeconds);

		bool CheckCollisionPointEllipse(float point_x, float point_y, float center_x, float center_y, float radius_OX, float radius_OY);
		bool CheckCollisionCircleCircle(float c1_x, float c1_y, float c1_radius, float c2_x, float c2_y, float c2_radius);

	protected:
		glm::mat3 modelMatrix;

		// Objects' initial dimensions:
		const float bow_radius = 40;
		const float bow_thickness = 6;

		const float arrow_length = bow_radius + 10;
		const float arrow_thickness = 6;

		const float shuriken_side = 25;

		const float balloon_width = 50;
		const float balloon_height = 80;
		const float balloon_neckLength = 10;

		const float balloon_string_length = 30;

		// Objects' speed:
		const float bow_speed = 200;
		const float shuriken_speed[5] = { 100, 150, 200, 300, 400 }; // changes based on level
		const float shuriken_falling_speed = 500;
		const float balloon_speed[5] = { 100, 150, 200, 300, 400 }; // changes based on level
		const float balloon_falling_speed = 500;

		// Floating objects:
		int nb_shurikens = 10, shurikens_in_use = 0;
		shuriken_t shurikens[20];
		int max_shurikens_in_use[5] = { 2, 4, 6, 8, 10 }; // maximum number of shurikens on screen (changes based on levels)
		float last_shuriken_spawn = 0, next_shuriken_spawn = 5; // used for giving a pause between shurikens
		float max_seconds_between_shurikens[5] = { 10, 8, 5, 3, 3 }; // changes based on level

		int nb_balloons = 20, red_balloons_in_use = 0, yellow_balloons_in_use = 0;
		balloon_t balloons[30];
		int max_red_balloons_in_use = 15; // maximum number of red balloons on screen at a time
		int max_yellow_balloons_in_use[5] = {3, 5, 7, 10, 10}; // max number of yellow balloons on screen (changes based on level)
		float last_red_balloon_spawn = 0, next_red_balloon_spawn = 2, max_seconds_between_red_balloons = 3; // used for giving a pause -
		float last_yellow_balloon_spawn = 0, next_yellow_balloon_spawn = 10, max_seconds_between_yellow_balloons = 4; // - between balloons

		// Positions of bow and arrow:
		float bow_x = 75, bow_y = 150, bow_angle = 0; // the position and rotation of the bow
		float bow_y_min = bow_radius / 2 + bow_thickness + 40; // min y coordinate for bow (so it doesn't go out of screen)
		float bow_y_max = window->GetResolution().y - bow_radius / 2 - bow_thickness - 20; // max y coordinate for bow
		
		float arrow_x = 75, arrow_y = 150, arrow_angle = 0; // the position and rotation of the bow
		bool arrow_moving = false; // is set when arrow was launched and is moving towards a target
		float arrow_speed = 0, arrow_speed_max = 30; // the speed based on how long the left mouse button was pressed
		
		// Variables for game progression:
		int game_score = 0;
		int game_lives = 3;
		int level = 0; // increases based on score (the game has 5 levels)
		const int collision_points_rb = 10; // 10p for each red balloon
		const int collision_points_yb = -20; // -20p for each yellow balloon hit
		const int collision_points_sh = 5; // 5p for each shuriken
		float next_show_score = 5, last_show_score = 0; // periodically display score
		bool restart = false; // used for game over, determining if player wants to restart game
};
