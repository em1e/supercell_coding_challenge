#include "include/Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>

#include "include/ResourceManager.h"
#include "include/InputHandler.h"
#include "include/Weapon.h"
#include "include/Player.h"
#include "include/Rectangle.h"
#include "include/Vampire.h"

Game::Game() :
    m_state(State::START),
    m_pClock(std::make_unique<sf::Clock>()),
    m_pPlayer(std::make_unique<Player>(this)),
    m_vampireCooldown(2.0f),
    m_nextVampireCooldown(2.0f),
    m_paused_time_scs(0)
{
    m_pGameInput = std::make_unique<GameInput>(this, m_pPlayer.get());
}

Game::~Game()
{
}

bool Game::initialise() {
    /* PAY ATTENTION HIVER! If you want to load any assets (fonts, textures) please use the pattern shown below */
    
    if (!m_font.loadFromFile(ResourceManager::getFilePath("Lavigne.ttf"))) {
        std::cerr << "Unable to load font" << std::endl;
        return false;
    }
    
    if (!m_vampTexture.loadFromFile(ResourceManager::getFilePath("vampire.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_playerTexture.loadFromFile(ResourceManager::getFilePath("player.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_pause_screen.loadFromFile(ResourceManager::getFilePath("pause_menu.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_pause_movement_help.loadFromFile(ResourceManager::getFilePath("pause_movement_help.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_start_help_window.loadFromFile(ResourceManager::getFilePath("start_help_window.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }


// this implementation does not work, don't use more time on it
// time wasted: 2.5 hours

/*
    if (!m_start_button.loadFromFile(ResourceManager::getFilePath("start_button.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_start_quit_button.loadFromFile(ResourceManager::getFilePath("start_button.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_start_help_button.loadFromFile(ResourceManager::getFilePath("start_button.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_pause_resume_button.loadFromFile(ResourceManager::getFilePath("pause_button.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_pause_restart_button.loadFromFile(ResourceManager::getFilePath("pause_button.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    if (!m_pause_help_button.loadFromFile(ResourceManager::getFilePath("pause_button.png"))) {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    init buttons
    sf::Sprite st_startButton(m_start_button);
    sf::Sprite st_quitButton(m_start_quit_button);
    sf::Sprite st_helpButton(m_start_help_button);

    Initialize start screen button textures
    st_startButton.setTexture(m_start_button);
    st_quitButton.setTexture(m_start_quit_button);
    st_helpButton.setTexture(m_start_help_button);

    Initialize their position
    st_startButton.setPosition(ButtonStartx, ButtonStarty);
    st_quitButton.setPosition(ButtonStartx, ButtonStarty + 150);
    st_helpButton.setPosition(ButtonStartx, ButtonStarty + 300);
*/

    resetLevel();
    return true;
}

void Game::resetLevel()
{
    m_pVampires.clear();
    m_pPlayer->setWeapon(false); // this hides weapon if it is active
    m_pPlayer->initialise();
    m_pClock->restart();
}

void Game::update(float deltaTime)
{
    switch (m_state)
    {
        case State::ACTIVE:
            m_pGameInput->update(deltaTime);
            m_pPlayer->update(deltaTime);
            vampireSpawner(deltaTime);
            for (auto& temp : m_pVampires)
                temp->update(deltaTime);
            if (m_pPlayer->isDead())
            {
                m_state = State::NEW;
                resetLevel();
            }
            break;
        default:
            m_pGameInput->update(deltaTime);
            break;
    }

    int i = 0;
    while (i < m_pVampires.size())
    {
        if (m_pVampires[i]->isKilled())
        {
            std::swap(m_pVampires[i], m_pVampires.back());
            m_pVampires.pop_back();
            continue;
        }
        i++;
    }
}

void Game::setMouseMoved(sf::Vector2f mousePosF, float deltaTime)
{
    if (m_state != State::ACTIVE)
    {
        t_startText.setFillColor(sf::Color::White);
        t_continueText.setFillColor(sf::Color::White);
        t_quitText.setFillColor(sf::Color::White);
        t_helpText.setFillColor(sf::Color::White);
        t_pauseHelpText.setFillColor(sf::Color::White);

        if (t_startText.getGlobalBounds().contains(mousePosF))
            t_startText.setFillColor(sf::Color::Red);
        else if (t_quitText.getGlobalBounds().contains(mousePosF))
            t_quitText.setFillColor(sf::Color::Red);
        else if (t_helpText.getGlobalBounds().contains(mousePosF))
            t_helpText.setFillColor(sf::Color::Red);
        else if (t_pauseHelpText.getGlobalBounds().contains(mousePosF))
            t_pauseHelpText.setFillColor(sf::Color::Red);
        else if (t_continueText.getGlobalBounds().contains(mousePosF))
            t_continueText.setFillColor(sf::Color::Red);
    }
    else if (m_state == State::ACTIVE)
    {
        rotatePlayer(mousePosF, deltaTime);
        m_mouse_x = mousePosF.x;
        m_mouse_y = mousePosF.y;
    }
    if (t_pauseHelpText.getGlobalBounds().contains(mousePosF))
        m_state = State::HELP2;
    else if (m_state == State::HELP2)
        m_state = State::PAUSED;
    
}

void Game::rotatePlayer(sf::Vector2f mousePosF, float deltaTime)
{
    if (mousePosF.x < ScreenWidth / 2)
        m_pPlayer->rotate(-1, deltaTime);
    else
        m_pPlayer->rotate(1, deltaTime);
}

void Game::setMousePressed(sf::Vector2f mousePosF)
{
    if (t_startText.getGlobalBounds().contains(mousePosF) && m_state == State::START)
    {
        std::cout << "game started, yay!" << std::endl;
        m_pClock->restart();
        m_state = State::ACTIVE;
    }
    if (t_startText.getGlobalBounds().contains(mousePosF) && (m_state == State::PAUSED || m_state == State::NEW))
    {
        m_pClock->restart();
        m_paused_time_scs = 0;
        resetLevel();
        m_state = State::ACTIVE;
    }
    else if (t_helpText.getGlobalBounds().contains(mousePosF))
        m_state = State::HELP;
    else if (t_continueText.getGlobalBounds().contains(mousePosF))
    {
        m_pClock->restart();
        m_state = State::ACTIVE;
    }

}

int Game::wantToQuitGame(sf::Vector2f mousePosF)
{
    if (t_quitText.getGlobalBounds().contains(mousePosF))
        return 1;
    return 0;
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{

    if (m_state == State::HELP)
    {
        sf::Sprite helpwindow(m_start_help_window);
        helpwindow.setPosition(0.0f, 0.0f);
        target.draw(helpwindow, states);
        return ;
    }
    else if (m_state == State::START)
    {
        // start screen header
        t_headerText.setFont(m_font);
        t_headerText.setCharacterSize(120);
        t_headerText.setString("Welcome to Spookie");
        t_headerText.setFillColor(sf::Color::White);
        t_headerText.setPosition(HeaderStartPos, HeaderStartPos);
        t_headerText.setStyle(sf::Text::Bold);
        target.draw(t_headerText, states);

        // start text button
        // target.draw(st_startButton, states);
        t_startText.setFont(m_font);
        t_startText.setCharacterSize(50);
        t_startText.setStyle(sf::Text::Bold);
        t_startText.setString("Start Game");
        t_startText.setPosition(ButtonStartx + 220, ButtonStarty + 420);
        target.draw(t_startText, states);

        // Quit game button
        // target.draw(st_quitButton, states);
        t_quitText.setFont(m_font);
        t_quitText.setCharacterSize(50);
        t_quitText.setString("Quit Game");
        t_quitText.setPosition(ButtonStartx + 220, ButtonStarty + 570);
        target.draw(t_quitText, states);

        // Help button
        // target.draw(st_helpButton, states);
        t_helpText.setFont(m_font);
        t_helpText.setCharacterSize(50);
        t_helpText.setString("Help");
        t_helpText.setPosition(ButtonStartx + 280, ButtonStarty + 720);
        target.draw(t_helpText, states);
    }
    else if (m_state == State::NEW)
    {
        // game restart screen text
        sf::Text newGameText;
        newGameText.setFont(m_font);
        newGameText.setString("GAME OVER");
        newGameText.setCharacterSize(120);
        newGameText.setFillColor(sf::Color::White);
        newGameText.setPosition(HeaderStartPos, HeaderStartPos);
        newGameText.setStyle(sf::Text::Bold);
        target.draw(newGameText);

        // restart text button
        // target.draw(st_startButton, states);
        t_startText.setFont(m_font);
        t_startText.setCharacterSize(50);
        t_startText.setStyle(sf::Text::Bold);
        t_startText.setString("Try again");
        t_startText.setPosition(ButtonStartx + 220, ButtonStarty + 420);
        target.draw(t_startText, states);

        // Quit game button
        // target.draw(st_quitButton, states);
        t_quitText.setFont(m_font);
        t_quitText.setCharacterSize(50);
        t_quitText.setString("Quit Game");
        t_quitText.setPosition(ButtonStartx + 220, ButtonStarty + 570);
        target.draw(t_quitText, states);

        // Help button
        // target.draw(st_helpButton, states);
        t_helpText.setFont(m_font);
        t_helpText.setCharacterSize(40);
        t_helpText.setString("Are you still lost on how this works?");
        t_helpText.setPosition(ButtonStartx + 220, ButtonStarty + 720);
        target.draw(t_helpText, states);
        
        // add buttons if time?
    }
    else
    {
        sf::Text timerText;
        timerText.setFont(m_font);
        timerText.setFillColor(sf::Color::White);
        timerText.setStyle(sf::Text::Bold);
        if (m_state == State::ACTIVE)
            timerText.setString(std::to_string((int)m_pClock->getElapsedTime().asSeconds() + m_paused_time_scs));
        else
            timerText.setString(std::to_string(m_paused_time_scs));
        timerText.setPosition(sf::Vector2f((ScreenWidth - timerText.getLocalBounds().getSize().x) * 0.5, 20));
        target.draw(timerText);
    }

    // Draw player.
    m_pPlayer->draw(target, states);

    //  Draw world.
    for (auto& temp : m_pVampires)
    {
        temp->draw(target, states);
    }

    if (m_state == State::PAUSED || m_state == State::HELP2)
    {
        sf::Sprite pausewindow(m_pause_screen);
        pausewindow.setPosition(0.0f, 0.0f);
        target.draw(pausewindow, states);

        // continue text button
        t_continueText.setFont(m_font);
        t_continueText.setCharacterSize(50);
        t_continueText.setStyle(sf::Text::Bold);
        t_continueText.setString("Continue");
        t_continueText.setPosition(ButtonStartx + 150, ButtonStarty + 270);
        target.draw(t_continueText, states);

        // restart game button
        t_startText.setFont(m_font);
        t_startText.setCharacterSize(50);
        t_startText.setString("Restart Game");
        t_startText.setPosition(ButtonStartx + 150, ButtonStarty + 400);
        target.draw(t_startText, states);

        // quit game button
        t_quitText.setFont(m_font);
        t_quitText.setCharacterSize(50);
        t_quitText.setString("Quit Game");
        t_quitText.setPosition(ButtonStartx + 150, ButtonStarty + 530);
        target.draw(t_quitText, states);

        // Help button
        t_pauseHelpText.setFont(m_font);
        t_pauseHelpText.setCharacterSize(50);
        t_pauseHelpText.setString("Help");
        t_pauseHelpText.setPosition(ButtonStartx + 200, ButtonStarty + 660);
        target.draw(t_pauseHelpText, states);
    }
    if (m_state == State::HELP2)
    {
        sf::Sprite pausewindow(m_pause_movement_help);
        pausewindow.setPosition(0.0f, 150.0f);
        target.draw(pausewindow, states);
    }
}


void Game::onKeyPressed(sf::Keyboard::Key key)
{
    m_pGameInput->onKeyPressed(key);

    // pauses or unpauses the game
    if (m_pGameInput->getEsc() && getState() == State::ACTIVE)
    {
        std::cout << "pausing game" << std::endl;
        m_paused_time_scs = m_paused_time_scs + (int)m_pClock->getElapsedTime().asSeconds();
        setState(State::PAUSED);
    }
    else if (m_pGameInput->getEsc() && getState() == State::PAUSED)
    {
        std::cout << "game unpaused" << std::endl;
        m_pClock->restart();
        setState(State::ACTIVE);
    }
}

void Game::onKeyReleased(sf::Keyboard::Key key)
{
    m_pGameInput->onKeyReleased(key);
}

Player* Game::getPlayer() const 
{
    return m_pPlayer.get();
}

void Game::vampireSpawner(float deltaTime)
{
    if (m_vampireCooldown > 0.0f)
    {
        m_vampireCooldown -= deltaTime;
        return;
    }

    float randomXPos = rand() % ScreenWidth;
    float randomYPos = rand() % ScreenHeight;

    float distToRight = (float) ScreenWidth - randomXPos;
    float distToBottom = (float) ScreenHeight - randomYPos;

    float xMinDist = randomXPos < distToRight ? -randomXPos : distToRight;
    float yMinDist = randomYPos < distToBottom ? -randomYPos : distToBottom;

    if (abs(xMinDist) < abs(yMinDist))
        randomXPos += xMinDist;
    else
        randomYPos += yMinDist;

    sf::Vector2f spawnPosition = sf::Vector2f(randomXPos, randomYPos);
    m_pVampires.push_back(std::make_unique<Vampire>(this, spawnPosition));

    m_spawnCount++;
    if (m_spawnCount % 2 == 0)
        m_nextVampireCooldown -= 0.5f;

    m_vampireCooldown = m_nextVampireCooldown;
    if (m_nextVampireCooldown >= 0.0f)
        m_nextVampireCooldown = rand() % 5;
    // great enemy bug, it made me laugh!
}


// changes game status
void Game::setState(State newState)
{
    if (m_state == newState)
        return;
    m_state = newState;
}

void Game::resetClock()
{
    m_pClock->restart();
}