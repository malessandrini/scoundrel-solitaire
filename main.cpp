#include <SFML/Graphics.hpp>
#include "maingame.h"


int main(int, char**) {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Scoundrel Solitaire");

    while (window.isOpen()) {
        MainGame mainGameLoop(window);  // create new game every time it's restarted
        int res = mainGameLoop.run();
        if (res < 0) window.close();  // terminate program
    }
}
