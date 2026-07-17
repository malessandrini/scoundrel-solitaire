#include "maingame.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
using namespace std::chrono_literals;


std::mutex guiMutexEvent, guiMutexDraw;
std::condition_variable guiCv;
std::optional<sf::Event> guiEvent;
std::vector<std::function<void()>> drawFunctions;


MainGame::MainGame(sf::RenderWindow &w, Assets &asst):
    window(w), view(window.getDefaultView()), assets(asst),
    spriteBg(assets.bg), spriteBack(assets.back),
    txtDeck(assets.font, "", 30), txtAvoid(assets.font, "Avoid", 30)
{
    spriteBg.setScale({view.getSize().x / assets.bg.getSize().x, view.getSize().y / assets.bg.getSize().y});  // background image can be smaller
    for (char s: {'d', 'h'})
        for (uint8_t v: {11, 12, 13, 14})
            deck.remove(s, v);
    deck.shuffle();
    drawFunctions.clear();
    drawFunctions.push_back([this](){ drawTable(); });
    drawFunctions.push_back([](){});  // to be replaced with extra drawings
    txtDeck.setFillColor(sf::Color::White);
    rectAvoid.setFillColor(sf::Color::White);
    rectAvoid.setPosition(posAvoid);
    txtAvoid.setFillColor(sf::Color::Black);
    center(txtAvoid, rectAvoid);
}


// exception used to catch window-close event in main thread
struct WindowClosed: public std::runtime_error {
    WindowClosed(): std::runtime_error("Window closed!") {}
};


void MainGame::run() {
    try {  // to catch WindowClosed
        while (!isDone) { // main game loop (choose card, ...)

            // fill the missing cards
            for (int i = 0; i < 4 && deck.num_cards(); ++i) {
                if (room[i]) continue;
                std::this_thread::sleep_for(200ms);
                { std::lock_guard lk(guiMutexDraw); room[i] = deck.pick(); }
            }

            // check if victory
            if (!deck.num_cards() && currentCards() == 1) {
                // victory! TODO
            }

            // wait: pick a card or avoid room (if enabled)
            while (currentCards() > 1) {
                bool canAvoid = !avoidedLast && currentCards() == 4;
                { std::lock_guard lk(guiMutexDraw);
                    if (canAvoid) drawFunctions[1] = [this](){ drawAvoid(); };
                    else drawFunctions[1] = [](){};
                }
                auto [input, index] = getInput();
                std::cout << (int)input << " " << index << std::endl;
                if (input == UserInput::Card && room[index]) {
                    // play a card
                }
                else if (input == UserInput::Avoid && canAvoid) {
                    // avoid room
                }
            }

/*
            while (true) {
                auto event = waitEvent();
                if (const auto *e = event.getIf<sf::Event::KeyPressed>()) {
                    if (e->scancode == sf::Keyboard::Scancode::Escape) {
                        isDone = true;
                        mustQuit = true;
                        break;
                    }
                }
            }
*/

        }
    }
    catch (WindowClosed&) {
        isDone = true;  // signal main thread
    }
}


sf::Event MainGame::waitEvent() {
    // Block on wait condition, letting the main GUI thread continue.
    // When notified, return the event to the game function. Gui is
    // immediately unlocked at the end of the function, so if the caller
    // must do GUI-affecting stuff in response to this event, like changing
    // the data used by the draw functions or those functions themselves, it
    // must acquire the draw mutex.
    if (!window.isOpen()) throw WindowClosed();
    std::unique_lock lk(guiMutexEvent);  // lock the mutex
    while(1) {
        if (!guiEvent) guiCv.wait(lk, [this](){ return guiEvent || !window.isOpen(); });  // mutex is unlocked while waiting
        // here we own the mutex again
        if (!window.isOpen() || guiEvent->is<sf::Event::Closed>()) throw WindowClosed();
        if (const auto *e = guiEvent->getIf<sf::Event::Resized>()) {
            onResize(e->size);
            guiEvent.reset();
        }
        else break;
    }
    // an event has been sent from GUI thread
    auto event = guiEvent.value();
    guiEvent.reset();
    return event;
}


std::pair<MainGame::UserInput, int> MainGame::getInput() {
    while(1) {
        auto event = waitEvent();
        if (const auto *e = event.getIf<sf::Event::KeyPressed>()) {
            if (e->code == sf::Keyboard::Key::Num1) return {UserInput::Card, 0};
            if (e->code == sf::Keyboard::Key::Num2) return {UserInput::Card, 1};
            if (e->code == sf::Keyboard::Key::Num3) return {UserInput::Card, 2};
            if (e->code == sf::Keyboard::Key::Num4) return {UserInput::Card, 3};
            if (e->code == sf::Keyboard::Key::A) return {UserInput::Avoid, 0};
        }
        if (const auto *e = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (e->button != sf::Mouse::Button::Left) continue;
            auto pos = window.mapPixelToCoords(e->position);
            for (int i = 0; i < 4; ++i)
                if (sf::FloatRect(posRoom[i], szCard).contains(pos)) return {UserInput::Card, i};
            if (sf::FloatRect(posAvoid, szAvoid).contains(pos)) return {UserInput::Avoid, 0};
        }
    }
}


void MainGame::onResize(sf::Vector2u sz) {
    std::lock_guard lk(guiMutexDraw);
    matchAspectRatio(view, sz);
}


void MainGame::drawTable() {
    // fixed parts
    window.clear();
    window.setView(view);
    window.draw(spriteBg);
    for (int i = 0; i < 4; ++i)
        if (room[i]) {
            assets.cards[room[i]->sprite_index].setPosition(posRoom[i]);
            window.draw(assets.cards[room[i]->sprite_index]);
        }
    if (deck.num_cards()) {
        spriteBack.setPosition(posDeck);
        window.draw(spriteBack);
    }
    txtDeck.setPosition(posDeck + szCard + sf::Vector2f{-80, 6});
    txtDeck.setString(std::to_string(deck.num_cards()));
    window.draw(txtDeck);
}


void MainGame::drawAvoid() {
    window.draw(rectAvoid);
    window.draw(txtAvoid);
}


void MainGame::matchAspectRatio(sf::View &view, sf::Vector2u winSize) {
    const float aspect_ratio = (float)view.getSize().x / view.getSize().y, new_asp = (float)winSize.x / winSize.y;
    if (new_asp > aspect_ratio) view.setViewport(sf::FloatRect({(1 - aspect_ratio / new_asp) / 2, 0}, {aspect_ratio / new_asp, 1}));
    else view.setViewport(sf::FloatRect({0, (1 - new_asp /aspect_ratio) / 2}, {1, new_asp / aspect_ratio}));
}


void MainGame::center(sf::Text &t, const sf::RectangleShape &rect) const {
    auto r = t.getLocalBounds();
    sf::Vector2f size = r.size + r.position;
    t.setPosition({rect.getPosition().x + (rect.getSize().x - size.x) / 2, rect.getPosition().y + (rect.getSize().y - size.y) / 2 });
}


int MainGame::currentCards() const {
    return std::count_if(std::begin(room), std::end(room), [](auto &r){ return r; });
}
