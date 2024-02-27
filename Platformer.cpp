#include <iostream>
#include "Entity.h"
#include "map.h"
using namespace std;
using namespace sf;
#pragma warning(disable : 4996)


vector<Enemy> Enemy_list;

void Death(RenderWindow& win, Event& event)
{
	RectangleShape death_screen;
	death_screen.setSize(Vector2f(1920, 1080));
	Texture death_screen_texture;
	death_screen_texture.loadFromFile("Assets/Menu/Death_screen.png");
	death_screen.setTexture(&death_screen_texture);
	ofstream pl_sv("Assets/Player/player.data");
	if (!pl_sv.eof())pl_sv << "1_Level";

	while (win.isOpen())
	{
		while (win.pollEvent(event))
		{
			if (Keyboard::isKeyPressed(Keyboard::Escape))win.close();
			if (event.type == Event::Closed) win.close();
		}

		win.draw(death_screen);
		win.display();
	}
}


void Win(RenderWindow& win, Event& event)
{
	RectangleShape win_screen;
	win_screen.setSize(Vector2f(1920, 1080));
	Texture win_screen_texture;
	win_screen_texture.loadFromFile("Assets/Menu/win_screen.png");
	win_screen.setTexture(&win_screen_texture);
	ofstream pl_sv("Assets/Player/player.data");
	if (!pl_sv.eof())pl_sv << "1_Level";

	while (win.isOpen())
	{
		while (win.pollEvent(event))
		{
			if (Keyboard::isKeyPressed(Keyboard::Escape))win.close();
			if (event.type == Event::Closed) win.close();
		}

		win.draw(win_screen);
		win.display();
	}
}

bool Start_game(RenderWindow& win, Level& temp, Event& event)
{
	RectangleShape font;
	Texture font_texture;


	Texture buttons;

	buttons.loadFromFile("Assets/Menu/buttons.png");

	Sprite new_game;
	Sprite continue_game;
	Sprite exit_game;

	new_game.setTexture(buttons);
	continue_game.setTexture(buttons);
	exit_game.setTexture(buttons);


	new_game.setTextureRect(IntRect(0, 0, 377, 48));
	continue_game.setTextureRect(IntRect(0, 193, 418, 48));
	exit_game.setTextureRect(IntRect(0, 387, 196, 48));
	new_game.setColor(Color::Black);
	continue_game.setColor(Color::Black);
	exit_game.setColor(Color::Black);

	new_game.setPosition(770, 400);
	continue_game.setPosition(750, 480);
	exit_game.setPosition(850, 555);


	ifstream player_data("Assets/Player/player.data");
	string save;

	if(!player_data.eof())getline(player_data, save);


	font_texture.loadFromFile("Assets/Map/fonts/" + save + "_font.png");
	font.setSize(Vector2f(1920, 1080));
	font.setTexture(&font_texture);

	while (win.isOpen())
	{
		Vector2i mousePoz = Mouse::getPosition(win);

		while (win.pollEvent(event))
		{
			if (event.type == Event::Closed) win.close();
			if (Keyboard::isKeyPressed(Keyboard::Escape))win.close();
			if (new_game.getGlobalBounds().contains(Mouse::getPosition(win).x, Mouse::getPosition(win).y))
			{
				new_game.setColor(Color::White);
				if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left)
				{
					temp.Load_map("1_Level");
					return true;
				}
			}
			else new_game.setColor(Color::Black);

			if (continue_game.getGlobalBounds().contains(Mouse::getPosition(win).x, Mouse::getPosition(win).y))
			{
				continue_game.setColor(Color::White);
				if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left)
				{
					temp.Load_map(save);
					return true;
				}
			}
			else continue_game.setColor(Color::Black);

			if (exit_game.getGlobalBounds().contains(Mouse::getPosition(win).x, Mouse::getPosition(win).y))
			{
				exit_game.setColor(Color::White);
				if (event.type == Event::MouseButtonPressed && event.key.code == Mouse::Left)
				{
					win.close();
				}
			}
			else exit_game.setColor(Color::Black);
		}

		win.draw(font);
		win.draw(new_game);
		win.draw(continue_game);
		win.draw(exit_game);
		win.display();
	}
}

void Ent_spawn(Texture& enemy_texture, Level& lvl, Player& player)
{
	for (int i  = 0; i < lvl.Map.size(); i++)
	{
		for (int j = 0; j < lvl.Map[i].size(); j++)
		{
			if (lvl.Map[i][j] == 'P')player.level_start(i * lvl.b_s, j * lvl.b_s);
			if (lvl.Map[i][j] == 'E')Enemy_list.push_back(Enemy(enemy_texture, lvl, j * lvl.b_s, i * lvl.b_s - 60));
		}
	}
}

int main()
{
	Level temp;

	RenderWindow win(VideoMode(1920, 1080), "Platformer", Style::Fullscreen);

	Event event;

	if (Start_game(win, temp, event))
	{
		//Спрайт игрока
		Texture player_texture;
		player_texture.loadFromFile("Assets/Player/rimuru_template.png");
		Player player(player_texture, temp);

		//Спрайт оружия игрока
		Texture wep_texture;
		wep_texture.loadFromFile("Assets/Player/weapon_template.png");
		player.sword.wep_t.setTexture(wep_texture);

		//Спрайт врага
		Texture enemy_texture;
		enemy_texture.loadFromFile("Assets/Enemy/enemy_template1.png");

		//объект стены
		RectangleShape wall(Vector2f(temp.b_s, temp.b_s));


		Clock clock;

		//Спавн существ
		Ent_spawn(enemy_texture, temp, player);


		while (win.isOpen())
		{
			if (temp.to_next_lvl)
			{
				Enemy_list.clear();
				win.clear();
				temp.Load_map(temp.next_lvl);
				Ent_spawn(enemy_texture, temp, player);
				temp.to_next_lvl = false;
			}

			float time = clock.getElapsedTime().asMicroseconds();
			clock.restart();

			time = time / 1000;
			while (win.pollEvent(event))
			{
				if (event.type == Event::Closed) win.close();
				if (Keyboard::isKeyPressed(Keyboard::Escape))win.close();
			}

			win.clear();

			if (player.hp > 0)
			{
				if (player.win) Win(win, event);
				temp.mapDisplay(wall, win, offset_x, offset_y);
				player.update(time, Enemy_list);
				win.draw(player.sprite);
				win.draw(player.sword.wep_t);
				win.draw(player.hp_bar);

				for (int i = 0; i < Enemy_list.size(); i++)
				{
					if (Enemy_list[i].hp != 0)
					{
						Enemy_list[i].update(time, player.hitbox.left, player.hitbox.top);
						win.draw(Enemy_list[i].sprite);
					}
					else
					{
						Enemy_list.erase(Enemy_list.begin() + i);
					}
				}
			}
			else
			{
				Death(win, event);
			}

			win.display();
		}
	}
}