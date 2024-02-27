#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include <vector>
#include <fstream>
#include <map>
using namespace std;
using namespace sf;

class Level
{
public:
	int H;
	int W;
	int b_s;
	RectangleShape font;
	Texture font_texture;
	vector<string> Map;
	string next_lvl;
	bool to_next_lvl = false;


	Level() : H{ 0 }, W{ 0 }, b_s{ 0 } {}
	
	void Load_map(string lvl_name)
	{
		string map_path = "Assets/Map/settings/" +lvl_name + ".map";
		string map_settings = "Assets/Map/settings/" + lvl_name + ".data";

		font_texture.loadFromFile("Assets/Map/fonts/" + lvl_name + "_font" + ".png");
		font.setSize(Vector2f(1920, 1080));
		font.setTexture(&font_texture);

		ofstream pl_sv("Assets/Player/player.data");
		if (!pl_sv.eof())pl_sv << lvl_name;

		ifstream f_map(map_path);
		string buff;
		Map.clear();

		while (!f_map.eof())
		{
			getline(f_map, buff);
			Map.push_back(buff);
		}

		ifstream f_sett(map_settings);

		for (int i = 1; i <= 4; i++)
		{
			if (!f_sett.eof())
			{
				getline(f_sett, buff);

				switch (i)
				{
				case 1:
					H = stoi(buff);
					break;
				case 2:
					W = stoi(buff);
					break;
				case 3:
					b_s = stoi(buff);
					break;
				case 4:
					next_lvl = buff;
					break;
				}
			}
		}
	}


	void mapDisplay(RectangleShape& wall, RenderWindow& win, int offset_x, int offset_y)
	{
		Texture grass_texture;
		Texture portal_texture;
		Texture old_block;
		old_block.loadFromFile("Assets/Map/textures/block.png");
		grass_texture.loadFromFile("Assets/Map/textures/grass.png");
		portal_texture.loadFromFile("Assets/Map/textures/portal.png");
		win.draw(font);

		for (int i = 0; i < Map.size(); i++)
		{
			for (int j = 0; j < Map[i].size(); j++)
			{
				if (Map[i][j] == 'L' || Map[i][j] == 'I')
				{
					wall.setTexture(&portal_texture);
				}
				if (Map[i][j] == 'W')
				{
					wall.setTexture(&grass_texture);
				}
				if (Map[i][j] == 'S')
				{
					wall.setTexture(&old_block);
				}
				if (Map[i][j] == 'E')continue;
				if (Map[i][j] == 'P')continue;
				if (Map[i][j] == '0')continue;
				if (Map[i][j] == ' ')continue;
				wall.setPosition(j * b_s - offset_x, i * b_s - offset_y);
				win.draw(wall);
			}
		}
	}
};