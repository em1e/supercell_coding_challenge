#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include "include/Game.h"
#include <memory>
#include <iostream>

#include "include/ResourceManager.h"

int main(int argc, char* argv[])
{
    // ResourceManager Must be Instantiated here -- DO NOT CHANGE
    ResourceManager::init(argv[0]);

    sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "Survive");
    window.setKeyRepeatEnabled(false);
    
    std::unique_ptr<Game> pGame = std::make_unique<Game>();
    if (!pGame->initialise())
    {
        std::cerr << "Game Failed to initialise" << std::endl;
        return 1;
    }
    
    sf::Clock clock;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            sf::Time elapsedTime = clock.getElapsedTime();
            clock.restart();
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            switch(event.type)
            {
                case sf::Event::Closed:
                    // "close requested" event: we close the window
                    window.close();
                    break;
                case sf::Event::MouseMoved:
                    // How to make mouse movement was referenced from https://gamedev.stackexchange.com/questions/142308/how-to-create-a-button-in-sfml
                    // since I had no idea how to do it initially
                    pGame->setMouseMoved(mousePosF, elapsedTime.asSeconds());
                    break;
                case sf::Event::MouseButtonPressed:
                    pGame->setMousePressed(mousePosF);
                    if (pGame->wantToQuitGame(mousePosF))
                    {
                        std::cout << "It's sad to see you go :(" << std::endl;
                        window.close();
                    }
                    break;
                case sf::Event::KeyPressed:
                    pGame->onKeyPressed(event.key.code);
                    break;
                case sf::Event::KeyReleased:
                    pGame->onKeyReleased(event.key.code);
                    break;
                default:
                    break;
            }
        }

        // Pressing Q quits the game, I'm not sure how to send close request, so did it this way C:
        if (event.key.code == sf::Keyboard::Q) 
        {
            window.close();
            break ;
        }

        sf::Time elapsedTime = clock.getElapsedTime();
        clock.restart();
        pGame->update(elapsedTime.asSeconds());
        
        // clear the window with black color
        window.clear(sf::Color::Black);
        
        window.draw(*pGame.get());
        // end the current frame
        window.display();
    }
    return 0;
}
