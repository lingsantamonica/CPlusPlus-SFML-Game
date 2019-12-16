
// YunPeng Ling  November 2019
//This is a 2D game developed with sfml library
				     //The player use   W S A D control the spaceship and 
				    //click left mouse button to shoot the enemy spaceships
//The png pictures used in this game are found online 
//I don't own any of those pictures
// Have fun !!!

#include <iostream>
#include <math.h>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Audio.hpp"
#include <vector>
#include<cstdlib>

using namespace sf;


//Classes for player and enemies


class Bullet
{
public:
	Sprite shape;

	Bullet(Texture *texture, Vector2f pos)
	{
		this->shape.setTexture(*texture);

		this->shape.setScale(0.05f, 0.05f);

		this->shape.setPosition(pos);
	}

	~Bullet() {}
};


class Player
{
public:
	Sprite shape;
	Texture *texture;

	int HP;
	int HPMax;

	std::vector<Bullet> bullets;

	Player(Texture *texture)
	{
		this->HPMax = 6;
		this->HP = this->HPMax;

		this->texture = texture;
		this->shape.setTexture(*texture);

		this->shape.setScale(0.4f, 0.4f);
	}

	~Player() {}

};

class Enemy
{
public:
	Sprite shape;

	int HP;
	int HPMax;

	Enemy(Texture *texture, Vector2u windowSize)
	{
		this->HPMax = 1;
		this->HP = this->HPMax;

		this->shape.setTexture(*texture);

		this->shape.setScale(0.2f, 0.2f);

		this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width, rand() % (int)(windowSize.y - this->shape.getGlobalBounds().height));
	}

	~Enemy() {}
};






//Main function and game loop
int main()
{

	//Game window setup  and game frame rate setting
	RenderWindow window(VideoMode(1024, 768), "GO LING!", Style::Close);
	window.setFramerateLimit(60);

	//Use random generator for enemies
	srand(time(NULL));

	//Setup font style text
	Font font;
	font.loadFromFile("Fonts/GarishingWorse.ttf");

	//Setup player textures-----------------------------------------------------
	Texture playerTexture;
	playerTexture.loadFromFile("f22.png");

	//Texture for player's anti air missile
	Texture bulletTexture;
	bulletTexture.loadFromFile("missile.png");



	//Setup enemy textures------------------------------------------------------
	Texture enemyTexture;
	enemyTexture.loadFromFile("alienShip.png");

	




	// Score system of the game
	Text score;
	score.setPosition(15.f, 15.f);
	score.setFont(font);
	score.setCharacterSize(20);
	score.setFillColor(Color::White);

	//Reminder the player the game is over if the blood is empty
	Text GameOver;
	GameOver.setPosition(400.f, window.getSize().y / 2);
	GameOver.setFont(font);
	GameOver.setCharacterSize(60);
	GameOver.setFillColor(Color::Yellow);
	GameOver.setString("GAME OVER");



	//Setup Player------------------------------------------------- 
	int playerScore = 0;
	Player player(&playerTexture);
	int shootTimer = 15;
	Text hpText;
	hpText.setFont(font);
	hpText.setCharacterSize(15);
	hpText.setFillColor(Color::White);

	//Setup Enemies--------------------------------------------------
	int enemySpawnTimer = 0;
	std::vector<Enemy> enemies;


	//Health bar------------------------------------------------------
	Text eHpText;
	eHpText.setFont(font);
	eHpText.setCharacterSize(15);
	eHpText.setFillColor(Color::Magenta);


	//Sound setting  some freedom music
	/*SoundBuffer backgroundMusic;
	if (!backgroundMusic.loadFromFile("countryroads.wav"))
	{
		return -1;
	}
	Sound music;
	music.setBuffer(backgroundMusic);
	music.play();
	*/

	//Main game loop stars here----------------------------------------------------------

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}


		//Update starts here--------------------------------------------------------------

		//Using if loop to check player status
		if (player.HP > 0)
		{

			//Keyboard control  W S A D
			if (Keyboard::isKeyPressed(Keyboard::W))
				player.shape.move(0.f, -15.f);

			if (Keyboard::isKeyPressed(Keyboard::S))
				player.shape.move(0.f, 15.f);

			if (Keyboard::isKeyPressed(Keyboard::A))
				player.shape.move(-15.f, 0.f);

			if (Keyboard::isKeyPressed(Keyboard::D))
				player.shape.move(15.f, 0.f);

			hpText.setPosition(player.shape.getPosition().x, player.shape.getPosition().y - hpText.getGlobalBounds().height);
			hpText.setString(std::to_string(player.HP));



			// Check window collision 
			//x coordinate decide left or right   y coordinate decide up or down
			if (player.shape.getPosition().x <= 0)
				player.shape.setPosition(0.f, player.shape.getPosition().y);

			if (player.shape.getPosition().x >= window.getSize().x - player.shape.getGlobalBounds().width)
				player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);

			if (player.shape.getPosition().y <= 0)
				player.shape.setPosition(player.shape.getPosition().x, 0.f);

			if (player.shape.getPosition().y >= window.getSize().y - player.shape.getGlobalBounds().height)
				player.shape.setPosition(player.shape.getPosition().x, window.getSize().y - player.shape.getGlobalBounds().height);

			// Update Controls
			if (shootTimer < 20)
				shootTimer++;

			//Left mouse to shoot
			if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 20)
			{
				player.bullets.push_back(Bullet(&bulletTexture, player.shape.getPosition()));
				shootTimer = 0;
			}


			//Bullets movement on the screen
			for (size_t i = 0; i < player.bullets.size(); i++)
			{

				player.bullets[i].shape.move(25.f, 0.f);

				if (player.bullets[i].shape.getPosition().x > window.getSize().x)
				{
					player.bullets.erase(player.bullets.begin() + i);
					break;
				}

				// Enemy collision
				for (size_t k = 0; k < enemies.size(); k++)
				{
					if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
					{
						if (enemies[k].HP <= 1)
						{
							playerScore += enemies[k].HPMax;
							enemies.erase(enemies.begin() + k);
						}
						else
							enemies[k].HP--;
						player.bullets.erase(player.bullets.begin() + i);
						break;
					}
				}
			}

			// Enemy Update
			if (enemySpawnTimer < 25)
				enemySpawnTimer++;


			if (enemySpawnTimer >= 25)
			{
				enemies.push_back(Enemy(&enemyTexture, window.getSize()));
				enemySpawnTimer = 0;
			}

			for (size_t i = 0; i < enemies.size(); i++)
			{
				enemies[i].shape.move(-3.f, 0.f);

				if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
				{
					enemies.erase(enemies.begin() + i);
					break;
				}

				if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
				{
					enemies.erase(enemies.begin() + i);

					player.HP--;
					break;
				}
			}

			//Score Update
			score.setString("Score: " + std::to_string(playerScore));
		}

		// Draw here-------------------------------------------------------------------

		window.clear();

		//draw player
		window.draw(player.shape);


		//draw bullets
		for (size_t i = 0; i < player.bullets.size(); i++)
		{
			window.draw(player.bullets[i].shape);
		}

		//draw enemy
		for (size_t i = 0; i < enemies.size(); i++)
		{

			window.draw(enemies[i].shape);
		}

	

		//draw score and health bar
		window.draw(score);
		window.draw(hpText);

		//If loop draw Game Over text if player hp less or equal to zero
		if (player.HP <= 0)
			window.draw(GameOver);

		//Display here----------------------------------------------------------------
		window.display();
	}






	return 0;
}
