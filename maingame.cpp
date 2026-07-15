#include "maingame.h"


MainGame::MainGame(sf::RenderWindow &w, Assets &asst):
    GameLoop(w), view(window.getDefaultView()), assets(asst),
    spriteBg(assets.bg)
{
    spriteBg.setScale({view.getSize().x / assets.bg.getSize().x, view.getSize().y / assets.bg.getSize().y});  // background image can be smaller
    for (char s: {'d', 'h'})
        for (uint8_t v: {11, 12, 13, 14})
            deck.remove(s, v);
    deck.shuffle();
}


int MainGame::run() {
    matchAspectRatio(view, window.getSize());

    // clear pending inputs
    while (pollEvent());

    int ccc = 0;

    while (window.isOpen()) {
        // process events
        while (const auto event = pollEvent()) {
            if (const auto *e = event->getIf<sf::Event::KeyPressed>()) {
                if (e->scancode == sf::Keyboard::Scancode::Escape)
                    return -1;  // terminate program
                ccc = (ccc + 1) % 52;
            }
        }

        // draw
        window.clear();
        window.setView(view);
        window.draw(spriteBg);
        assets.cards[ccc].setPosition({62, 60});
        window.draw(assets.cards[ccc]);
        window.display();
    }
    return -1;  // if here, window has been closed
}


void MainGame::onResize(sf::Vector2u sz) {
    matchAspectRatio(view, sz);
}
