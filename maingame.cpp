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
    drawFunctions.clear();
    drawFunctions.push_back([this](){ drawTable(); });
    drawFunctions.push_back([](){});  // to be replaced with extra drawings
}


void MainGame::run() {
    //matchAspectRatio(view, window.getSize());
    while (window.isOpen()) {
        auto event = waitEvent();
        if (const auto *e = event.getIf<sf::Event::KeyPressed>()) {
            if (e->scancode == sf::Keyboard::Scancode::Escape) {
                mustQuit = true;
                break;
            }
            else {
                std::lock_guard lk(guiMutexDraw);
                ccc = (ccc + 1) % 52;
            }
        }
        else if (const auto *e = event.getIf<sf::Event::Resized>())
            matchAspectRatio(view, e->size);
    }
    isDone = true;  // signal main thread
}


void MainGame::drawTable() {
    // fixed parts
    window.clear();
    window.setView(view);
    window.draw(spriteBg);
    assets.cards[ccc].setPosition(posRoom[3]);
    window.draw(assets.cards[ccc]);
    window.display();
}
