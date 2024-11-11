#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Constants.h"

class Player;
class Game;
class GameInput;
class Vampire;

namespace sf { class Clock; }

class Game : public sf::Drawable
{
public:

    enum class State
    {
        START,
        HELP,
        HELP2,
        PAUSED,
        ACTIVE,
        NEW,
    };

    Game();
    ~Game();

    bool initialise();
    void update(float deltaTime);
    void resetLevel();
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    
    State getState() const { return m_state; }
    void setState(State newState);
    
    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);

    Player* getPlayer() const;
    sf::Texture* getPlayerTexture() { return &m_playerTexture; }
    sf::Texture* getVampireTexture() { return &m_vampTexture; }

    void vampireSpawner(float deltaTime);

    void resetClock();

    void setMouseMoved(sf::Vector2f mousePosF, float deltaTime);
    void setMousePressed(sf::Vector2f mousePosF);
    int wantToQuitGame(sf::Vector2f mousePosF);

    void rotatePlayer(sf::Vector2f mousePosF, float deltaTime);

private:
    std::unique_ptr<Player> m_pPlayer;

    std::vector<std::unique_ptr<Vampire>> m_pVampires;

    State m_state;
    std::unique_ptr<sf::Clock> m_pClock;
    int m_paused_time_scs;
    std::unique_ptr<GameInput> m_pGameInput;

    float m_vampireCooldown = 0.0f;
    float m_nextVampireCooldown = 2.0f;
    int m_spawnCount = 0;
    
    sf::Font m_font;
    sf::Texture m_vampTexture;
    sf::Texture m_playerTexture;

    sf::Texture m_start_help_window;

    sf::Texture m_pause_screen;
    sf::Texture m_pause_movement_help;

    float m_mouse_x;
    float m_mouse_y;

    mutable sf::Text t_headerText;
    mutable sf::Text t_startText;
    mutable sf::Text t_continueText;
    mutable sf::Text t_quitText;
    mutable sf::Text t_helpText;
    mutable sf::Text t_pauseHelpText;

// this implementation does not work, don't use more time on it

    // sf::Texture m_start_button;
    // sf::Texture m_start_quit_button;
    // sf::Texture m_start_help_button;

    // sf::Texture m_pause_resume_button;
    // sf::Texture m_pause_restart_button;
    // sf::Texture m_pause_help_button;

    // sf::Sprite st_startButton;
    // sf::Sprite st_quitButton;
    // sf::Sprite st_helpButton;

    // sf::Sprite ps_startButton;
    // sf::Sprite ps_quitButton;
    // sf::Sprite ps_helpButton;


};
