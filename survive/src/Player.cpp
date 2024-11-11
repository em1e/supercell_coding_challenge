#include "include/Player.h"
#include "include/Weapon.h"
#include "include/InputHandler.h"
#include "include/Constants.h"
#include <vector>
#include <cmath>
#include <iostream>
#include "include/Game.h"

Player::Player(Game* pGame) :
    Rectangle(sf::Vector2f(PlayerWidth, PlayerHeight)),
    m_pGame(pGame),
    m_pWeapon(std::make_unique<Weapon>())
{
    setOrigin(sf::Vector2f(0.0f, 0.0f));
}

bool Player::initialise()
{
    m_sprite.setTexture(*m_pGame->getPlayerTexture());
    m_sprite.setScale(3.5f, 3.5f);
    setIsDead(false);
    setPosition(ScreenWidth / 2, ScreenHeight / 2);
    m_sprite.setPosition(getPosition());
    rotation = M_PI * 0.5;
    dir_x = cos(rotation);
    dir_y = sin(rotation);
    return true;
}

void Player::move(InputData inputData, float deltaTime)
{
    float xSpeed = 0.0f;
    float ySpeed = 0.0f;
    float step_x =  inputData.m_movingLeft - inputData.m_movingRight;
    float step_y =  inputData.m_movingDown - inputData.m_movingUp;
    float new_x;
    float new_y;

    // this is code from OUR cub3d, so from https://github.com/em1e/42_cub3d
    xSpeed = xSpeed
        + (step_y * deltaTime * PlayerSpeed * dir_x)
        + (step_x * deltaTime * PlayerSpeed * -dir_y);
    ySpeed = ySpeed
        + (step_y * deltaTime * PlayerSpeed * dir_y)
        + (step_x * deltaTime * PlayerSpeed * dir_x);

    // std::cout << "stepy " << step_y << ", step_x "<< step_x << std::endl;
    // std::cout << "PlayerSpeed " << PlayerSpeed << ", deltaTime "<< deltaTime << std::endl;
    // std::cout << "dir_y " << dir_y << ", dir_x " << dir_x << std::endl;
    // std::cout << "moving: x=" << xSpeed << ", y="<< ySpeed << std::endl;
    sf::Transformable::move(sf::Vector2f(xSpeed, ySpeed));
    setPosition(std::clamp(getPosition().x, 0.0f, (float)ScreenWidth), getPosition().y);

    if (m_pWeapon->isActive() == false)
    {
        if (inputData.m_movingLeft == true && inputData.m_movingRight == false)
            m_direction = LEFT;
        else if (inputData.m_movingLeft == false && inputData.m_movingRight == true)
            m_direction = RIGHT;
    }
}

// this function is from OUR cub3d, so from https://github.com/em1e/42_cub3d
void Player::rotate(float rot_direction, float deltaTime)
{
	if (rot_direction > 0)
		rotation += 2 * deltaTime;
	else
		rotation -= 2 * deltaTime;
	if (rotation > 2 * M_PI)
		rotation -= 2 * M_PI;
	if (rotation < 0)
		rotation += 2 * M_PI;
	dir_x = cos(rotation);
	dir_y = sin(rotation);
    // std::cout << "ROTATING PLAYER" << std::endl;
}

void Player::attack()
{
    m_pWeapon->setActive(true);
}

void Player::update(float deltaTime)
{
    sf::Vector2f weaponSize = m_pWeapon->getSize();

    m_sprite.setPosition(getPosition());
    m_pWeapon->setPosition(sf::Vector2f(
        getCenter().x - dir_x,
        getCenter().y - dir_y));
    m_pWeapon->update(deltaTime);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    Rectangle::draw(target, states);
    m_pWeapon->draw(target, states);
}

/*
    changes player weapon to given bool
*/
void Player::setWeapon(bool setTo)
{
    if (m_pWeapon->isActive() == setTo)
        return;
    m_pWeapon->setActive(setTo);
}