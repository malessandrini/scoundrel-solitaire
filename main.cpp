#include <SFML/Graphics.hpp>
#include "maingame.h"
#include "assets.h"


int main(int, char**) {
    sf::RenderWindow window(sf::VideoMode({1024, 768}), "Scoundrel Solitaire");

    Assets assets;

    while (window.isOpen()) {
        MainGame mainGameLoop(window, assets);  // create new game every time it's restarted
        int res = mainGameLoop.run();
        if (res < 0) window.close();  // terminate program
    }
}
