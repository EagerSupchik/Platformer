#pragma once
#include "map.h"

float offset_x, offset_y; // переменные смещения 

class Entity
{
public:

	bool win = false; //проверка на победу
	float mx, my; // переменные передвижения
	FloatRect hitbox; // хитбокс объекта
	bool on_ground; // проверка "на земле"
	float c_frame = 0; // текущий кадр (для анимации)
	Sprite sprite; // спрайт объекта

	void Collision(int dir, Level& lvl)
	{
		for (int i = hitbox.top / lvl.b_s; i < (hitbox.top + hitbox.height) / lvl.b_s; i++)
			for (int j = hitbox.left / lvl.b_s; j < (hitbox.left + hitbox.width) / lvl.b_s; j++)
			{
				if (lvl.Map[i][j] == 'W' ||  lvl.Map[i][j] == 'S' || lvl.Map[i][j] == '0')
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
				if (lvl.Map[i][j] == 'L')
				{
					if (Keyboard::isKeyPressed(Keyboard::E))
					{
						lvl.to_next_lvl = true;
					}
				}

				if (lvl.Map[i][j] == 'I')
				{
					if (Keyboard::isKeyPressed(Keyboard::E))
					{
						win = true;
					}
				}

			}
	}
};



class Enemy : public Entity
{
private:
	Level* lvl;
public:
	int hp; // хп врага
	bool shield = false; // проверка на щит
	float sh_kd = 0;  //кд щита
	int damage = 24; //дамаг от врага 
	float respawn = 0; //кд на респавн (один раз за 1 уровень)

	Enemy(Texture& s, Level& c_lvl, int x, int y)
	{
		lvl = &c_lvl;
		sprite.setTexture(s);
		sprite.scale(3, 3);
		sprite.setTextureRect(IntRect(6, 0, 45, 80));
		hitbox = FloatRect(x, y, 130, 220);

		hp = 100;
		mx = 0;
		my = 0;
		c_frame = 0;
	}

	void update(float time, int x, int y)
	{
		hitbox.left += mx * time;
		Collision(0, *lvl);
		move(x, y, time);

		if (!on_ground)my = my + 0.005 * time;
		hitbox.top += my * time;
		on_ground = false;
		Collision(1, *lvl);

		sprite.setPosition(hitbox.left - offset_x, hitbox.top - offset_y);

		if (sh_kd < 1.5 && shield)
		{
			sh_kd += time * 0.002;
		}
		else if (sh_kd > 1.5)
		{
			sh_kd = 0;
		 	shield = false;
		}

		if (respawn < 5)
		{
			respawn += time * 0.002;
		}
	}

	void move(int x, int y, int time)
	{
		if (respawn >= 2)
		{
			c_frame += 0.005 * time;
			if (c_frame > 9) c_frame -= 9;

			if (hitbox.left < x - 1100 || hitbox.left > x + 1100)
			{
				mx = 0;
			}
			else
			{
				if (!shield)
				{
					if (hitbox.left < x)
					{
						mx = 0.1;
						sprite.setTextureRect(IntRect(68 * int(c_frame), 82, 50, 80));
					}
					if (hitbox.left > x)
					{
						mx = -0.1;
						sprite.setTextureRect(IntRect(68 * int(c_frame) + 50, 82, -50, 80));
					}
				}
			}
		}
	}
};




class Weapon
{
public:
	int damage = 10; // урон от меча
	FloatRect wep_hit; // хитбокс меча
	Sprite wep_t; // спрайт меча
};


class Player : public Entity
{
public:
	Level* lvl; // уровень
	Weapon sword; // объект оружия

	bool side = false; // поворот игрока
	int hp = 100; // хп игрока
	bool shield = false; //проверка щита
	float sh_kd = 0; //кулдаун щита
	float damage_kd = 0; //кулдаун на атаку

	RectangleShape hp_bar; // отображение хп

	Player(Texture& s, Level& c_lvl)
	{
		lvl = &c_lvl;
		sprite.setTexture(s);
		sprite.scale(3, 3);
		sprite.setTextureRect(IntRect(9, 8, 41, 49));
		hitbox = FloatRect(lvl->b_s, lvl->b_s, 120, 140);
		sword.wep_hit = FloatRect(lvl->b_s, lvl->b_s, 130, 20);
		sword.wep_t.scale(0.5,0.5);
		hp_bar.setSize(Vector2f(100 * 5, 60));
		hp_bar.setFillColor(Color::Red);
		hp_bar.setPosition(30, 30);
		

		mx = 0;
		my = 0;
		c_frame = 0;
	}

	void update(float time, vector<Enemy>& Enemy_list)
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
		sword.wep_hit.top = hitbox.top + 65;

		if (!side)
		{
			sword.wep_hit.left = hitbox.left + 50;
			sword.wep_t.setPosition(hitbox.left - offset_x + 70, hitbox.top - offset_y - 30);
		}
		else
		{
			sword.wep_hit.left = hitbox.left - 50;
			sword.wep_t.setPosition(hitbox.left - offset_x - 170, hitbox.top - offset_y - 30);
		}

		attack_checker(Enemy_list);

		mx = 0;

		if (sh_kd < 3 && shield)
		{
			sh_kd += time * 0.002;
		}
		if (sh_kd > 3)
		{
			sh_kd = 0; 
			shield = false;
		}

		if (damage_kd > 0)
		{
			damage_kd -= time * 0.002;
		}

		if (damage_kd <= 1.5)
		{
			sword.wep_t.setTextureRect(IntRect(0, 0, 0, 0));
		}
		else
		{
			if (side)
			{
				sword.wep_t.setTextureRect(IntRect(420, 0, -420, 420));
			}
			else
			{
				sword.wep_t.setTextureRect(IntRect(0, 0, 420, 420));
			}
		}

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

		if (!side && on_ground)sprite.setTextureRect(IntRect(50, 8, -41, 49));
		if (side && on_ground)sprite.setTextureRect(IntRect(9, 8, 41, 49));
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
		if (my < 0 && !on_ground && !side)sprite.setTextureRect(IntRect(129, 75, 44, 41));
		if (my < 0 && !on_ground && side)sprite.setTextureRect(IntRect(173, 75, -44, 41));

	}

	void cam_follow()
	{
		if (hitbox.left > (960 - lvl->b_s)) offset_x = hitbox.left - (960 - lvl->b_s);
		if (hitbox.left > (lvl->W * lvl->b_s - 960 - lvl->b_s)) offset_x = lvl->W * lvl->b_s - 1920;
		if (hitbox.top < lvl->b_s * 4)offset_y = hitbox.top - 360 - (1080 - lvl->H * lvl->b_s);
		else offset_y = -(1080 - lvl->H * lvl->b_s);
	}


	void attack_checker(vector<Enemy>& Enemy_list)
	{
		for (int i = 0; i < Enemy_list.size(); i++)
		{
			if (hitbox.intersects(Enemy_list[i].hitbox))
			{

				if (!shield)
				{
					hp -= Enemy_list[i].damage;
					hp_bar.setSize(Vector2f(hp_bar.getSize().x - Enemy_list[i].damage * 5, hp_bar.getSize().y));
					shield = true;
					my = -1.5;

				}
			}



			if (sword.wep_hit.intersects(Enemy_list[i].hitbox))
			{
				if (Mouse::isButtonPressed(Mouse::Left))
				{
					if (Enemy_list[i].hp != 0)
					{
						if (damage_kd <= 0)
						{
							if (!Enemy_list[i].shield)
							{
								Enemy_list[i].hp -= sword.damage;
								Enemy_list[i].my += -1.5;
								if (!side)Enemy_list[i].mx += 0.3;
								else Enemy_list[i].mx += -0.3;
								Enemy_list[i].shield = true;
								damage_kd = 3;
							}
						}
					}

				}
			}
		}
	}




	void level_start(int x, int y)
	{
		hitbox.left = x;
		hitbox.top = y;
	}
};