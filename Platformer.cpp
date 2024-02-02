#include <iostream>
#include "Entity.h"
#include "map.h"
#include <fstream>
#pragma warning(disable : 4996)



void mapDisplay(RectangleShape &wall, RenderWindow &win, Level &lvl)
{
	Texture plate_texture;
	plate_texture.loadFromFile("grass.png");
	for (int i = 0; i < lvl.H; i++)
	{
		for (int j = 0; j < lvl.W; j++)
		{
			if (lvl.Map[i][j] == 'W')wall.setTexture(&plate_texture);
			if (lvl.Map[i][j] == '0')continue;
			if (lvl.Map[i][j] == ' ')continue;

			wall.setPosition(j * lvl.b_s - offset_x, i * lvl.b_s - offset_y);
			win.draw(wall);
		}
	}
}


int main()
{

	Level A_1;
	string path = "Level.bin";

	ifstream fin;
	fin.open(path);
	if (!fin.is_open())
	{
		cout << "error open" << endl;
		return -1;
	}
	else
	{
		cout << "opened!" << endl;
		fin.read((char*)&A_1, sizeof(Level));
	}
	fin.close();

	RenderWindow win(VideoMode(1920, 1080), "Platformer");
	Event event;

	
	//Фон
	RectangleShape font;
	Texture font_texture;


	if(!font_texture.loadFromFile("font2.png")) { cout << "Error!"; }
	font.setSize(Vector2f(1920, 1080));
	font.setTexture(&font_texture);


	//Спрайт игрока

	Texture player_texture;
	player_texture.loadFromFile("test.png");
	Player player(player_texture, A_1);

	Clock clock;


	RectangleShape wall(Vector2f(A_1.b_s, A_1.b_s));
	//Обработка
	while (win.isOpen()) 
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		time = time / 1000;
		while (win.pollEvent(event))
		{
			if (event.type == Event::Closed) win.close(); // Закрыть окно
		}

		player.update(time);
		win.clear();
		win.draw(font);
		win.draw(player.sprite);
		mapDisplay(wall, win, A_1);
		win.display();

	}

}