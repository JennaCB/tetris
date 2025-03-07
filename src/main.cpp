#include <raylib.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <string>

struct point
{
	int x, y;

	friend bool operator==(const point& lhs, const point& rhs)
	{
		return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
	}

	friend bool operator!=(const point& lhs, const point& rhs)
	{
		return !(lhs == rhs);
	}

};

struct shape
{
	Color color;
	std::array<point, 4> shape;

	int lowest()
	{
		int lowest = 0;

		for (point pos : shape)
		{
			if (pos.y > lowest)
				lowest = pos.y;
		}
		return lowest;
	}

	bool collides(std::array<std::array<std::optional<Color>, 10>, 20> grid)
	{
		for (int i = 0; i < grid.size(); ++i)
		{
			for (std::optional<Color> block : grid[i])
			{
				if (block != std::nullopt)
				{
					if (i - lowest() == 1)
						return true;
				}
			}
		}
		return false;
	}

	void draw()
	{
		int screen_width = GetRenderWidth();
		int screen_height = GetRenderHeight();

		float square_width = GetRenderWidth() * (20 / 1366.0f);
		float square_height = GetRenderHeight() * (20 / 705.0f);

		float grid_width = (square_width * 10);
		float grid_height = (square_height * 20);

		float x = ((screen_width / 2.0f) - (grid_width / 2.0f));
		float y = ((screen_height / 2.0f) - (grid_height / 2.0f));

		for (point pos : shape)
			DrawRectangle(x + (square_width * pos.x), y + (square_height * pos.y), square_width, square_height, color);
	}

	/*void move_left()
	{

	}

	void move_right()
	{

	}*/
};

struct Ishape : public shape
{
	Ishape()
	{
		color = SKYBLUE;
		shape = { point(3, 0), point(4, 0), point(5, 0), point(6, 0) };
	}

};

struct Jshape : public shape
{
	Jshape()
	{
		color = BLUE;
		shape = { point(3, 0), point(3, 1), point(4, 1), point(5, 1) };
	}

};

struct Lshape : public shape
{
	Lshape()
	{
		color = ORANGE;
		shape = { point(5, 0), point(3, 1), point(4, 1), point(5, 1) };
	}

};

struct Oshape : public shape
{
	Oshape()
	{
		color = YELLOW;
		shape = { point(4, 0), point(5, 0), point(4, 1), point(5, 1) };
	}

};

struct Sshape : public shape
{
	Sshape()
	{
		color = GREEN;
		shape = { point(4, 0), point(5, 0), point(3, 1), point(4, 1) };
	}

};

struct Tshape : public shape
{
	Tshape()
	{
		color = PURPLE;
		shape = { point(4, 0), point(3, 1), point(4, 1), point(5, 1) };
	}

};

struct Zshape : public shape
{
	Zshape()
	{
		color = RED;
		shape = { point(3, 0), point(4, 0), point(4, 1), point(5, 1) };
	}

};

struct grid
{
	int width_in_squares = 10;
	int height_in_squares = 20;

	std::array<std::array<std::optional<Color>, 10>, 20> grid;

	void draw()
	{
		int screen_width = GetRenderWidth();
		int screen_height = GetRenderHeight();

		float square_width = GetRenderWidth() * (20/1366.0f);
		float square_height = GetRenderHeight() * (20/705.0f);

		float grid_width = (square_width * 10);
		float grid_height = (square_height * 20);

		float x = ((screen_width / 2.0f) - (grid_width / 2.0f));
		float y = ((screen_height / 2.0f) - (grid_height / 2.0f));

		for (int row = 1; row < height_in_squares; ++row)
			DrawLine(x, y + (square_height * row), x + grid_width, y + (square_height * row), DARKGRAY);
		for (int column = 1; column < width_in_squares; ++column)
			DrawLine(x + (square_width * column), y, x + (square_width * column), y + grid_height, DARKGRAY);

		for (int row = 0; row < grid.size(); ++row)
		{
			for (int column = 0; column < grid[row].size(); ++column)
			{
				if(grid[row][column] != std::nullopt)
					DrawRectangle(x + (square_width * column), y + (square_height * row), square_width, square_height, grid[row][column].value());
			}
		}

		DrawRectangleLines(x, y, grid_width, grid_height, WHITE);
	}
};

int main()
{
	grid grid;

	InitWindow(800, 500, "Tetris");
	SetTargetFPS(60);
	
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	MaximizeWindow();

	std::array<shape, 7> falling_shapes = {Ishape(), Jshape(), Lshape(), Oshape(), Sshape(), Tshape(), Zshape()};

	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(falling_shapes.begin(), falling_shapes.end(), g);

	int current_shape = 0;

	auto time_of_last_frame = std::chrono::steady_clock::now();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

			grid.draw();

			falling_shapes[current_shape].draw();

			const auto now = std::chrono::steady_clock::now();
			const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - time_of_last_frame);
			
			if (elapsed_time > std::chrono::milliseconds(100))
			{
				if (falling_shapes[current_shape].lowest() < 19 && !falling_shapes[current_shape].collides(grid.grid))
				{
					for (point& pos : falling_shapes[current_shape].shape)
						++pos.y;
				}
				else //if (falling_shapes[current_shape].lowest() == 19)
				{
					for (point pos : falling_shapes[current_shape].shape)
						grid.grid[pos.y][pos.x] = falling_shapes[current_shape].color;

					if (current_shape < 6)
					{
						++current_shape;
					}
					else
					{
						current_shape = 0;
						falling_shapes = { Ishape(), Jshape(), Lshape(), Oshape(), Sshape(), Tshape(), Zshape() };
						std::shuffle(falling_shapes.begin(), falling_shapes.end(), g);
					}
				}

				time_of_last_frame = now;
			}
		
		EndDrawing();
	}

	CloseWindow();
}
