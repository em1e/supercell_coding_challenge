#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "Game.h"

class Player;

struct InputData
{
    bool m_movingUp = false;
    bool m_movingDown = false;
    bool m_movingLeft = false;
    bool m_movingRight = false;
    bool m_space = false; // ray shooting
    bool m_enter = false; // start game
    bool m_esc = false; // pause game
    bool m_spaceReleased = true;
    bool m_enterReleased = true;
    bool m_escReleased = true;

    bool hasInputs() { return m_movingUp || m_movingDown || m_movingLeft || m_movingRight || m_space;}
};

class GameInput
{
public:

    GameInput(Game* pGame, Player* pPlayer);
    ~GameInput();
    
    void update(float deltaTime);
    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);
    
    Game::State getGameState() const { return m_pGame->getState(); }
    void setGameState(Game::State newState);

    bool getEsc() {return m_inputData.m_esc; }

private:
    InputData m_inputData;
    Game* m_pGame;
    Player* m_pPlayer;
};
