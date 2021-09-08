#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

#include <map>
#include <cstring>

#include "Tema2Camera.h"
#include "Utils.h"

class Tema2 : public SimpleScene
{
	public:
		Tema2();
		~Tema2();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void UpdateGameStatus(float deltaTimeSeconds);
		void UpdateTimes(float deltaTimeSeconds);

		void UpdateBall(float deltaTimeSeconds);
		void RenderBall(Shader *shader, glm::vec3 color);

		void UpdatePlatforms(float deltaTimeSeconds);
		void GeneratePlatform(int i, int j);
		void GameOver();

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color = glm::vec3(1));

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		// Camera information:
		Tema::Camera *camera;
		bool firstPersonMode = false;
		glm::mat4 projectionMatrix;

		// Information about platforms:
		platform platforms[4][20];
		int platforms_count[4] = { 0, 0, 0, 0 };

		float platforms_aux = 0;

		float platforms_max_z = 10;
		float platforms_min_z = -100;

		float platforms_velocity = 0;
		float platforms_max_velocity = 20;
		float platforms_min_velocity = 3;

		glm::vec3 leftmostPoint = glm::vec3(-2.1f, 0.0f, -4.0f);

		// Information about interval for new special platforms spawning:
		float next_special_in = 5;
		float last_special_in = 0;

		// Information about the ball:
		glm::vec3 ball_position = glm::vec3(0, 0.55, 0.6);
		glm::vec3 ball_velocity_ox = glm::vec3(15, 0, 0);
		
		bool ball_is_changing_columns = false;
		int ball_column = 2;
		int ball_next_column;

		bool ball_is_jumping = false;
		glm::vec3 gravity = glm::vec3(0, -30, 0);
		glm::vec3 ball_velocity_oy = glm::vec3(0, 0, 0);
		glm::vec3 ball_initial_velocity_oy = glm::vec3(0, 10, 0);

		// Columns position:
		float columns_ox[5] = { -4.2f, -2.1f, 0, 2.1f, 4.2f };

		// Special events:

		// Ball stepped outside of platforms:
		bool ball_is_falling = false;

		// Ball stepped on red platform:
		bool ball_is_dying = false;
		float ball_dying_time = 0;

		// Ball is out of fuel:
		bool ball_is_out_of_gas = false;
		float time_without_fuel = 0;

		// Ball stepped on orange platform:
		bool speed_blocked = false;
		float speed_blocked_time;
		float high_speed = 20;
		float old_speed;

		// Information about game status:
		bool game_over = false;
		float score = 0;

		float fuel_max = 10;
		float fuel = 10;
		glm::vec3 fuel_bar_position = glm::vec3(-0.48, 0.9, 0);

		// Colors used:
		glm::vec3 background = glm::vec3(1);

		std::map<std::string, glm::vec3> colors{
			{ "pink", glm::vec3(255.0f / 255, 183.0f / 255, 197.0f / 255) },
			{ "blue", glm::vec3(118.0f / 255, 175.0f / 255, 236.0f / 255) },
			{ "red", glm::vec3(1, 104.0f / 255, 98.0f / 255) },
			{ "yellow", glm::vec3(253.0f / 255, 253.0f / 255, 102.0f / 255) },
			{ "orange", glm::vec3(255.0f / 255, 199.0f / 255, 132.0f / 255) },
			{ "green", glm::vec3(169.0f / 255, 231.0f / 255, 146.0f / 255) },
			{ "purple", glm::vec3(156.0f / 255, 129.0f / 255, 198.0f / 255) },
			{ "grey", glm::vec3(243.0f / 255, 244.0f / 255, 247.0f / 255) },
			{ "dark_red", glm::vec3(124.0f / 255, 10.0f / 255, 1.0f / 255) },
			{ "black", glm::vec3(0) }
		};

		std::string platform_colors[4] = { "red", "yellow", "orange", "green" };
};
