#include <raylib.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

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

bool operator==(const Color& lhs, const Color& rhs)
{
	return std::tie(lhs.r, lhs.g, lhs.b, lhs.a) == std::tie(rhs.r, rhs.g, rhs.b, rhs.a);
}

bool operator!=(const Color& lhs, const Color& rhs)
{
	return !(lhs == rhs);
}

struct grid
{
	static constexpr int width_in_squares = 10;
	static constexpr int height_in_squares = 20;

	std::array<std::array<std::optional<Color>, width_in_squares>, height_in_squares> grid;

	int screen_width = GetRenderWidth();
	int screen_height = GetRenderHeight();

	float square_width = GetRenderWidth() * (30 / 1366.0f);

	float grid_width = (square_width * width_in_squares);
	float grid_height = (square_width * height_in_squares);

	float x = ((screen_width / 2.0f) - (grid_width / 2.0f));
	float y = ((screen_height / 2.0f) - (grid_height / 2.0f));

	void draw(Texture2D texture)
	{
		for (int row = 1; row < height_in_squares; ++row)
			DrawLine(x, y + (square_width * row), x + grid_width, y + (square_width * row), DARKGRAY);
		for (int column = 1; column < width_in_squares; ++column)
			DrawLine(x + (square_width * column), y, x + (square_width * column), y + grid_height, DARKGRAY);

		Rectangle source(0, 0, 30, 30);

		for (int row = 0; row < grid.size(); ++row)
		{
			for (int column = 0; column < grid[row].size(); ++column)
			{
				Rectangle destination(x + (square_width * column), y + (square_width * row), square_width, square_width);
				NPatchInfo nPatchInfo(source, 30, 30, 5, 5, NPATCH_NINE_PATCH);
				if (grid[row][column] != std::nullopt)
					DrawTextureNPatch(texture, nPatchInfo, destination, Vector2(0, 0), 0, grid[row][column].value());
			}
		}

		DrawRectangleLines(x, y, grid_width, grid_height, WHITE);
	}

	bool line_full(point& pos)
	{
		for (std::optional<Color> color : grid[pos.y])
		{
			if (color == std::nullopt)
				return false;
		}
		return true;
	}

	void delete_line(point& pos, int& score)
	{
		for (std::optional<Color>& color : grid[pos.y])
			color = std::nullopt;
		score = score + 100;
	}

	void move_lines(std::vector<point>& deleted_lines_positions, int& deleted_lines)
	{
		std::array<std::array<std::optional<Color>, width_in_squares>, height_in_squares> temp_grid;
		for (int i = grid.size() - 1; i >= 0; --i)
		{
			if (i > deleted_lines_positions[0].y)
				temp_grid[i] = grid[i];
			else if (i == 0)
				grid = temp_grid;
			else
			{
				if(i - deleted_lines >= 0)
					temp_grid[i] = grid[i - deleted_lines];
			}
				
		}
	}
};

struct shape
{
	Color color;
	std::array<point, 4> blocks;

	static const shape I, J, L, O, S, T, Z;

	point lowest()
	{
		point lowest(0,0);

		for (point pos : blocks)
		{
			if (pos.y > lowest.y)
				lowest = pos;
		}
		return lowest;
	}

	point leftest()
	{
		point leftest(blocks[0].x, 0);

		for (point pos : blocks)
		{
			if (pos.x < leftest.x)
				leftest = pos;
		}
		return leftest;
	}

	point rightest()
	{
		point rightest(0, 0);

		for (point pos : blocks)
		{
			if (pos.x > rightest.x)
				rightest = pos;
		}
		return rightest;
	}

	bool collides(const std::array<std::array<std::optional<Color>, grid::width_in_squares>, grid::height_in_squares>& grid, std::optional<int> key, point& shape_pos)
	{
		for (point pos : blocks)
		{
			pos.x = pos.x + shape_pos.x;
			pos.y = pos.y + shape_pos.y;

			if (key == std::nullopt && grid[pos.y + 1][pos.x] != std::nullopt)
				return true;
			if (key == KEY_RIGHT && grid[pos.y][pos.x + 1] != std::nullopt)
				return true;
			if (key == KEY_LEFT && grid[pos.y][pos.x - 1] != std::nullopt)
				return true;
		}
		return false;
	}

	void draw(grid& grid, Texture2D& texture, point& shape_pos)
	{
		Rectangle source(0, 0, 30, 30);

		for (point pos : blocks)
		{
			Rectangle destination(grid.x + (grid.square_width * (pos.x+shape_pos.x)), grid.y + (grid.square_width * (pos.y+shape_pos.y)), grid.square_width, grid.square_width);
			NPatchInfo nPatchInfo(source, 30, 30, 5, 5, NPATCH_NINE_PATCH);
			DrawTextureNPatch(texture, nPatchInfo, destination, Vector2(0, 0), 0, color);
		}
	}

	void move_left(point& shape_pos)
	{
		--shape_pos.x;
	}

	void move_right(point& shape_pos)
	{
		++shape_pos.x;
	}

	void rotate_clockwise()
	{
		if (color == SKYBLUE)
		{
			if (blocks[0] == point(-1, 0))
				blocks = { point(1, -1), point(1, 0), point(1, 1), point(1, 2) };
			else if (blocks[0] == point(1, -1))
				blocks = { point(2, 1), point(1, 1), point(0, 1), point(-1, 1) };
			else if (blocks[0] == point(2, 1))
				blocks = { point(0, 2), point(0, 1), point(0, 0), point(0, -1) };
			else if (blocks[0] == point(0, 2))
				blocks = { point(-1, 0), point(0, 0), point(1, 0), point(2, 0) };
		}
		else if (color != YELLOW)
		{
			for (point& block : blocks)
				block = point(block.y * -1, block.x);
		}
	}

	void rotate_counterclockwise()
	{
		if (color == SKYBLUE)
		{
			if (blocks[0] == point(-1, 0))
				blocks = { point(0, 2), point(0, 1), point(0, 0), point(0, -1) };
			else if (blocks[0] == point(0, 2))
				blocks = { point(2, 1), point(1, 1), point(0, 1), point(-1, 1) };
			else if (blocks[0] == point(2, 1))
				blocks = { point(1, -1), point(1, 0), point(1, 1), point(1, 2) };
			else if (blocks[0] == point(1, -1))
				blocks = { point(-1, 0), point(0, 0), point(1, 0), point(2, 0) };
		}
		else if (color != YELLOW)
		{
			for (point& block : blocks)
				block = point(block.y, block.x * -1);
		}
	}
};

const shape shape::I = { .color = SKYBLUE,	.blocks = {point(-1, 0), point(0, 0), point(1, 0), point(2, 0)} };
const shape shape::J = { .color = BLUE,		.blocks = {point(-1, -1), point(-1, 0), point(0, 0), point(1, 0)} };
const shape shape::L = { .color = ORANGE,	.blocks = {point(1, -1), point(-1, 0), point(0, 0), point(1, 0)} };
const shape shape::O = { .color = YELLOW,	.blocks = {point(0, -1), point(1, -1), point(0, 0), point(1, 0)} };
const shape shape::S = { .color = GREEN,	.blocks = {point(0, -1), point(1, -1), point(-1, 0), point(0, 0)} };
const shape shape::T = { .color = PURPLE,	.blocks = {point(0, -1), point(-1, 0), point(0, 0), point(1, 0)} };
const shape shape::Z = { .color = RED,		.blocks = {point(-1,-1), point(0,-1), point(0,0), point(1,0)} };

struct preview
{
	static constexpr int width_in_squares = 5;
	static constexpr int height_in_squares = 3;

	void draw(grid grid, std::array<shape, 7> falling_shapes, int current_shape, Texture2D texture, point shape_pos)
	{
		float preview_width = grid.square_width * width_in_squares;
		float preview_height = grid.square_width * height_in_squares;

		float x = grid.x + grid.grid_width;
		float y = grid.y;

		DrawRectangleLines(x, y, preview_width, preview_height, WHITE);

		Rectangle source(0, 0, 30, 30);
		for (point pos : falling_shapes[current_shape+1].blocks)
		{
			pos.x = pos.x + 4;
			pos.y = pos.y + 1;

			Rectangle destination(x + (grid.square_width * (pos.x - 2.5f)), y + (grid.square_width * (pos.y + 0.5f)), grid.square_width, grid.square_width);
			NPatchInfo nPatchInfo(source, 30, 30, 5, 5, NPATCH_NINE_PATCH);
			DrawTextureNPatch(texture, nPatchInfo, destination, Vector2(0, 0), 0, falling_shapes[current_shape + 1].color);
		}
	}
};

struct score
{
	int score;

	void draw(grid grid)
	{
		std::string s = std::to_string(score);
		DrawText("Score:", grid.x, grid.y - grid.square_width, grid.square_width, WHITE);
		DrawText(s.c_str(), (grid.x + grid.grid_width) - MeasureText(s.c_str(), grid.square_width), grid.y - grid.square_width, grid.square_width, WHITE);
	}
};

void reset_game(grid& grid, preview& preview, score& score , std::array<shape, 7>& falling_shapes, int& current_shape, point& shape_pos)
{
	for (auto& row : grid.grid)
	{
		for (auto& column : row)
			column = std::nullopt;
	}

	score.score = 0;

	//std::array<shape, 7> next_falling_shapes = { shape::I, shape::J, shape::L, shape::O, shape::S, shape::T, shape::Z };
	//falling_shapes = next_falling_shapes;

	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(falling_shapes.begin(), falling_shapes.end(), g);

	current_shape = 0;

	shape_pos = point(4, 1);
}

int main()
{
	InitWindow(800, 500, "Tetris");
	SetTargetFPS(60);
	
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	MaximizeWindow();

	Texture2D texture = LoadTexture("./assets/block.png");

	grid grid;
	preview preview;
	score score = { .score = 0 };

	std::array<shape, 7> falling_shapes = { shape::I, shape::J, shape::L, shape::O, shape::S, shape::T, shape::Z };
	std::array<shape, 7> next_falling_shapes = { shape::I, shape::J, shape::L, shape::O, shape::S, shape::T, shape::Z };

	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(falling_shapes.begin(), falling_shapes.end(), g);
	std::shuffle(next_falling_shapes.begin(), next_falling_shapes.end(), g);

	int current_shape = 0;

	auto time_of_last_frame = std::chrono::steady_clock::now();

	point shape_pos(4, 1);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

			grid.draw(texture);

			if (current_shape < 6)
				preview.draw(grid, falling_shapes, current_shape, texture, shape_pos);
			else
				preview.draw(grid, next_falling_shapes, -1, texture, shape_pos);

			score.draw(grid);

			falling_shapes[current_shape].draw(grid, texture, shape_pos);

			auto falling_time = std::chrono::milliseconds(250);

			const auto now = std::chrono::steady_clock::now();
			const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - time_of_last_frame);
			
			if (IsKeyPressed(KEY_LEFT_CONTROL))
				reset_game(grid, preview, score, falling_shapes, current_shape, shape_pos);

			if (IsKeyPressed(KEY_LEFT) && falling_shapes[current_shape].leftest().x + shape_pos.x > 0 && !falling_shapes[current_shape].collides(grid.grid, KEY_LEFT, shape_pos))
				falling_shapes[current_shape].move_left(shape_pos);

			if (IsKeyPressed(KEY_RIGHT) && falling_shapes[current_shape].rightest().x+shape_pos.x < grid.grid[0].size() - 1 &&!falling_shapes[current_shape].collides(grid.grid, KEY_RIGHT, shape_pos))
				falling_shapes[current_shape].move_right(shape_pos);

			if (IsKeyDown(KEY_S))
				falling_time = std::chrono::milliseconds(100);
			
			if (IsKeyReleased(KEY_S))
				falling_time = std::chrono::milliseconds(250);


			if (IsKeyPressed(KEY_SPACE))
			{
				while (falling_shapes[current_shape].lowest().y+shape_pos.y < 19 && !falling_shapes[current_shape].collides(grid.grid, std::nullopt, shape_pos))
					++shape_pos.y;
			}

			if (IsKeyPressed(KEY_UP))
				falling_shapes[current_shape].rotate_clockwise();

			if (IsKeyPressed(KEY_DOWN))
				falling_shapes[current_shape].rotate_counterclockwise();


			if (elapsed_time > falling_time)
			{
				if (falling_shapes[current_shape].lowest().y+shape_pos.y < 19 && !falling_shapes[current_shape].collides(grid.grid, std::nullopt, shape_pos))
				{
					++shape_pos.y;
				}
				else
				{
					int deleted_lines = 0;
					std::vector<point> deleted_lines_positions;
					for (point pos : falling_shapes[current_shape].blocks)
					{
						pos.x = pos.x + shape_pos.x;
						pos.y = pos.y + shape_pos.y;

						grid.grid[pos.y][pos.x] = falling_shapes[current_shape].color;
						if (grid.line_full(pos))
						{
							grid.delete_line(pos, score.score);
							++deleted_lines;
							deleted_lines_positions.push_back(pos);
						}
					}

					shape_pos = point(4, 1);

					if (deleted_lines > 0)
					{
						grid.move_lines(deleted_lines_positions, deleted_lines);
						deleted_lines = 0;
					}
					
					if (current_shape < 6)
					{
						++current_shape;
					}
					else
					{
						current_shape = 0;
						falling_shapes = next_falling_shapes;
						std::shuffle(next_falling_shapes.begin(), next_falling_shapes.end(), g);
					}
				}

				time_of_last_frame = now;
			}
		
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
