#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Map.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode{{800, 600}}, "The Darkest Laberynth");
    const int mapSize = 20;
    const int cellSize = 400 / mapSize;
    Player player(cellSize, mapSize);
    Map map(cellSize, mapSize);
    sf::Clock clock;

    // Main loop
    while (window.isOpen())
    {
        while (auto maybeEvent = window.pollEvent())
        {
            auto& event = *maybeEvent;
            if (event.is<sf::Event::Closed>())
                window.close();

            player.handleEvent(event);
        }        
        window.clear(sf::Color::Black);
        map.draw(window);
        player.draw(window);
        window.display();
    }

    return 0;
}
