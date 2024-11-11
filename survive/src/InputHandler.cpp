#include "include/InputHandler.h"
#include "include/Weapon.h"
#include "include/Player.h"
#include "include/Game.h"
#include <iostream>

GameInput::GameInput(Game* pGame, Player* pPlayer) :
    m_pGame(pGame), m_pPlayer(pPlayer)
{

    
}

GameInput::~GameInput()
{
    
}

void GameInput::update(float deltaTime)
{

    // starts the game only if enter is pressed
    if (m_inputData.m_enter && (getGameState() == Game::State::START || getGameState() == Game::State::NEW || getGameState() == Game::State::HELP))
    {
        std::cout << "game started" << std::endl;
        setGameState(Game::State::ACTIVE);
        m_pGame->resetClock();
    }

    // returns if game isn't active
    if (getGameState() == Game::State::START || getGameState() == Game::State::PAUSED || getGameState() == Game::State::NEW)
        return;

    if (m_inputData.hasInputs())
    {
        m_pPlayer->move(m_inputData, deltaTime);
    }

    if (m_inputData.m_space)
    {
        m_pPlayer->attack();
    }
}

void GameInput::onKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Up)
    {
        m_inputData.m_movingUp = true;
    }
    else if (key == sf::Keyboard::Down)
    {
        m_inputData.m_movingDown = true;
    }
    else if (key == sf::Keyboard::Left)
    {
        m_inputData.m_movingLeft = true;
    }
    else if (key == sf::Keyboard::Right)
    {
        m_inputData.m_movingRight = true;
    }
    else if (key == sf::Keyboard::Space)
    {
        if (m_inputData.m_spaceReleased)
        {
            m_inputData.m_space = true;
        }
        m_inputData.m_spaceReleased = false;
    }
    else if (key == sf::Keyboard::Enter)
    {
        if (m_inputData.m_enterReleased)
        {
            m_inputData.m_enter = true;
        }
        m_inputData.m_enterReleased = false;
    }
    else if (key == sf::Keyboard::Escape)
    {
        if (m_inputData.m_escReleased)
        {
            m_inputData.m_esc = true;
        }
        m_inputData.m_escReleased = false;
    }
}

void GameInput::onKeyReleased(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Up)
    {
        m_inputData.m_movingUp = false;
    }
    else if (key == sf::Keyboard::Down)
    {
        m_inputData.m_movingDown = false;
    }
    else if (key == sf::Keyboard::Left)
    {
        m_inputData.m_movingLeft = false;
    }
    else if (key == sf::Keyboard::Right)
    {
        m_inputData.m_movingRight = false;
    }
    else if (key == sf::Keyboard::Space)
    {
        m_inputData.m_space = false;
        m_inputData.m_spaceReleased = true;
    }
    else if (key == sf::Keyboard::Enter)
    {
        m_inputData.m_enter = false;
        m_inputData.m_enterReleased = true;
    }
    else if (key == sf::Keyboard::Escape)
    {
        m_inputData.m_esc = false;
        m_inputData.m_escReleased = true;
    }
}

/*
    changes game status
*/
void GameInput::setGameState(Game::State newState)
{
    if (m_pGame->getState() == newState)
        return;
    m_pGame->setState(newState);
}
