#pragma once
#include <SFML/Graphics.hpp>
#include "map.h"
using namespace std;
using namespace sf;


float offset_x, offset_y;

class Entity
{
public:

	float mx, my;
	FloatRect hitbox;
	bool on_ground;
	float c_frame = 0;
	Sprite sprite;

public:
	virtual void update(float time) = 0;



	virtual void Collision(int dir, Level& lvl)
	{
		for (int i = hitbox.top / lvl.b_s; i < (hitbox.top + hitbox.height) / lvl.b_s; i++)
			for (int j = hitbox.left / lvl.b_s; j < (hitbox.left + hitbox.width) / lvl.b_s; j++)
			{
				if (lvl.Map[i][j] == 'W' || lvl.Map[i][j] == 'B' || lvl.Map[i][j] == '0')
				{
					if ((mx > 0) && (dir == 0)) hitbox.left = j * lvl.b_s - hitbox.width;
					if ((mx < 0) && (dir == 0)) hitbox.left = j * lvl.b_s + lvl.b_s;
					if ((my > 0) && (dir == 1)) 
					{ 
						hitbox.top = i * lvl.b_s - hitbox.height; 
						my = 0; 
						on_ground = true; 
					}

					if ((my < 0) && (dir == 1)) 
					{ 
						hitbox.top = i * lvl.b_s + lvl.b_s; 
						my = 0; 
					}
				}

			}

	}
};


class Player : public Entity
{
private:
	bool side = false;
	Level* lvl;
public:

	Player(Texture& s, Level& c_lvl)
	{
		lvl = &c_lvl;
		sprite.setTexture(s);
		sprite.scale(3, 3);
		sprite.setTextureRect(IntRect(9, 8, 41, 49));
		hitbox = FloatRect(lvl->b_s, lvl->b_s, 120, 140);

		mx = 0;
		my = 0;
		c_frame = 0;
	}

	virtual void update(float time)
	{
		move();

		hitbox.left += mx * time;
		Collision(0, *lvl);

		if (!on_ground)my = my + 0.005 * time;
		hitbox.top += my * time;
		on_ground = false;
		Collision(1, *lvl);

		
		cam_follow();
		animation(time);

		sprite.setPosition(hitbox.left - offset_x, hitbox.top - offset_y);

		mx = 0;
	}


	void move()
	{
		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			mx = -1;
			side = true;
		}

		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			mx = 1;
			side = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			if (on_ground)
			{
				my = -2;
				on_ground = false;
			}
		}

		if (side == false && on_ground == true)sprite.setTextureRect(IntRect(50, 8, -41, 49));
		if (side == true && on_ground == true)sprite.setTextureRect(IntRect(9, 8, 41, 49));
	}


	void animation(float time)
	{
		c_frame += 0.01 * time;
		if (c_frame > 6) c_frame -= 6;

		if (mx > 0)
		{
			if (my < 0 && !on_ground)
			{
				sprite.setTextureRect(IntRect(129, 75, 44, 41));
			}
			else sprite.setTextureRect(IntRect(43 * int(c_frame), 70, 44, 47));
		}
		if (mx < 0)
		{
			if (my < 0 && !on_ground)
			{
				sprite.setTextureRect(IntRect(173, 75, -44, 41));
			}
			else sprite.setTextureRect(IntRect(43 * int(c_frame) + 44, 70, -44, 47));
		}
		if (my < 0 && !on_ground && side == false)sprite.setTextureRect(IntRect(129, 75, 44, 41));
		if (my < 0 && !on_ground && side == true)sprite.setTextureRect(IntRect(173, 75, -44, 41));
	}



	void cam_follow()
	{
		if (hitbox.left > 900) offset_x = hitbox.left - 900;
		if (hitbox.left > 5780) offset_x = 4870;
	}
};