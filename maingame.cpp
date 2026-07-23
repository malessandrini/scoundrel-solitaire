#include "maingame.h"
#include <thread>
#include <chrono>
#include <algorithm>
using namespace std::chrono_literals;


std::mutex guiMutexEvent, guiMutexDraw;
std::condition_variable guiCv;
std::optional<sf::Event> guiEvent;
std::vector<std::function<void()>> drawFunctions;


MainGame::MainGame(sf::RenderWindow &w, Assets &asst):
    window(w), view(window.getDefaultView()), assets(asst),
    txtDeck(assets.font, "", 30, sf::Color::White), txtAvoid(assets.font, "Avoid", 30, sf::Color::Black),
    txtHealth(assets.font, "", 50, sf::Color::Yellow), txtDialog(assets.font, "", 35, sf::Color::White), txtBtn1(assets.font, "", 25, sf::Color::Black),
    txtBtn2(assets.font, "", 25, sf::Color::Black), txtCancel(assets.font, "", 25, sf::Color::Black)
{
    assets.bg.setScale({view.getSize().x / assets.bg.getTexture().getSize().x, view.getSize().y / assets.bg.getTexture().getSize().y});  // background image can be smaller
    for (char s: {'d', 'h'})
        for (uint8_t v: {11, 12, 13, 14})
            deck.remove(s, v);
    deck.shuffle();
    drawFunctions.clear();
    drawFunctions.push_back([this](){ drawTable(); });
    drawFunctions.push_back([](){});  // to be replaced with extra drawings
    center(txtAvoid, rectAvoid);
    assets.skull.setOrigin(assets.skull.getLocalBounds().getCenter());
    animHealth.reset();
}


// exception used to catch window-close event in main loop
struct WindowClosed: public std::runtime_error {
    WindowClosed(): std::runtime_error("Window closed!") {}
};


void MainGame::syncGui(std::function<void ()> func) {
    std::lock_guard lk(guiMutexDraw);
    func();
}


void MainGame::run() {
    try {  // to catch WindowClosed
        while (!isDone && window.isOpen()) { // main game loop (choose card, ...)

            // fill the missing cards
            for (int i = 0; i < 4 && deck.num_cards(); ++i) {
                if (room[i]) continue;
                std::this_thread::sleep_for(200ms);
                syncGui([this, i](){ room[i] = deck.pick(); });
            }

            // check if victory
            if (!deck.num_cards() && currentCards() == 1) {
                // victory!
                break;  // will be tested below
            }

            // wait: pick a card or avoid room (if enabled)
            bool usedHeart = false;  // in this turn
            while (currentCards() > 1) {
                bool canAvoid = !avoidedLast && currentCards() == 4;
                syncGui([this, canAvoid](){ drawFunctions[1] = canAvoid ? std::function<void()>([this](){drawAvoid();}) : [](){}; });
                auto [input, index] = getInput();
                if (input == UserInput::Card && room[index]) {
                    // play a card
                    Card card = room[index].value();
                    if (card.suit == 'd') {
                        // weapon
                        if (showDialog("Equip weapon?", "Equip", "", true) == UserInput::Btn1) {
                            syncGui([this, id=index, &card](){
                                room[id].reset();
                                lastMonster.reset();
                                weapon = card;
                                avoidedLast = false;
                                assets.sndMelee.play();
                            });
                        }
                    }
                    else if (card.suit == 'h') {
                        // potion
                        if (showDialog("Use potion?", (usedHeart ? "Discard" : "Heal"), "", true) == UserInput::Btn1) {
                            syncGui([this, id=index, &card, &usedHeart](){
                                room[id].reset();
                                if (!usedHeart) {
                                    health = std::min(20, health + card.value);
                                    assets.sndHeal.play();
                                    animHealth.restart();
                                }
                                usedHeart = true;
                                avoidedLast = false;
                            });
                        }
                    }
                    else {
                        // monster
                        const bool canWeapon = weapon && (!lastMonster || lastMonster->value > card.value);
                        int damageBare = card.value, damageWeapon = canWeapon ? (card.value > weapon->value ? card.value - weapon->value : 0) : 0;
                        auto res = showDialog("Fight monster (" + std::to_string(card.value) + ")?", "Bare hands", (canWeapon ? "Weapon" : ""), true,
                            damageBare >= health ? &assets.skull : nullptr, damageWeapon >= health ? &assets.skull : nullptr);
                        if (res == UserInput::Btn1) {  // barehand
                            syncGui([this, id=index, damageBare](){
                                room[id].reset();
                                health = std::max(0, health - damageBare);
                                if (damageBare) {
                                    assets.sndHit.play();
                                    animHealth.restart();
                                }
                                else assets.sndKnife.play();
                                avoidedLast = false;
                            });
                        }
                        else if (res == UserInput::Btn2 && canWeapon) {  // weapon
                            syncGui([this, id=index, &card, damageWeapon](){
                                room[id].reset();
                                health = std::max(0, health - damageWeapon);
                                if (damageWeapon) {
                                    assets.sndHit.play();
                                    animHealth.restart();
                                }
                                else assets.sndKnife.play();
                                lastMonster = card;
                                avoidedLast = false;
                            });
                        }
                        if (health <= 0) {
                            break;  // will be tested below
                        }
                    }
                }
                else if (input == UserInput::Avoid && canAvoid) {
                    // avoid room
                    if (showDialog("Avoid room?", "Avoid", "", true) == UserInput::Btn1) {
                        syncGui([this](){ drawFunctions[1] = [this](){ draw4Backs(); }; });
                        std::this_thread::sleep_for(500ms);
                        avoidedLast = true;
                        syncGui([this](){
                            drawFunctions[1] = [](){};
                            // put cards back in random order
                            std::vector<int> v{0, 1, 2, 3};
                            std::shuffle(v.begin(), v.end(), Deck::gen);
                            for (int i: v) {
                                Card c = room[i].value();
                                room[i].reset();
                                deck.add(c);
                            }
                        });
                    }
                }
                else if (input == UserInput::Esc) {
                    if (showDialog("Quit game?", "Quit", "", true) == UserInput::Btn1) {
                        mustQuit = true;
                        throw WindowClosed();
                    }
                }
            }  // while currentCards() > 1
            // test if died
            if (health <= 0) break;
        }  // main game loop
        // Here: we won or we died
        while (animHealth.isRunning());
        const int score = finalScore();
        if (score <= 0) assets.sndDie.play();
        else assets.sndVictory.play();
        if (showDialog(std::string(score > 0 ? "You win!" : "You die!") +  "   Score: " + std::to_string(score), "Restart", "Quit", false) == UserInput::Btn2) mustQuit = true;
        isDone = true;
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
            if (e->code == sf::Keyboard::Key::Escape) return {UserInput::Esc, 0};
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
    syncGui([this, sz](){  matchAspectRatio(view, sz); });
}


void MainGame::drawTable() {
    // fixed parts
    window.clear();
    window.setView(view);
    window.draw(assets.bg);
    // cardd in the room
    for (int i = 0; i < 4; ++i)
        if (room[i]) {
            assets.cards[room[i]->sprite_index].setPosition(posRoom[i]);
            window.draw(assets.cards[room[i]->sprite_index]);
        }
    // back of deck and number of cards
    if (deck.num_cards()) {
        assets.back.setPosition(posDeck);
        window.draw(assets.back);
    }
    txtDeck.setPosition(posDeck + szCard + sf::Vector2f{-80, 20});
    txtDeck.setString(std::to_string(deck.num_cards()));
    window.draw(txtDeck);
    // health
    txtHealth.setString(std::to_string(health));
    if (animHealth.isRunning() && animHealth.getElapsedTime() < 400ms) {
        txtHealth.setCharacterSize(100);
        center(txtHealth, rectHealt, {0, -50 + animHealth.getElapsedTime().asMilliseconds() / 8.f});
    }
    else {
        animHealth.reset();
        txtHealth.setCharacterSize(50);
        center(txtHealth, rectHealt);
    }
    window.draw(txtHealth);
    // weapon and last killed monster
    if (weapon) {
        assets.cards[weapon->sprite_index].setPosition(posWeapon);
        window.draw(assets.cards[weapon->sprite_index]);
    }
    if (lastMonster) {
        assets.cards[lastMonster->sprite_index].setPosition(posWeapon + offKilledMonster);
        window.draw(assets.cards[lastMonster->sprite_index]);
    }
}


void MainGame::drawAvoid() {
    window.draw(rectAvoid);
    window.draw(txtAvoid);
}


void MainGame::drawDialog() {
    static const sf::Vector2f offSpr{0, -50};
    window.draw(rectDlg);
    window.draw(txtDialog);
    if (dlgSpr1) {
        dlgSpr1->setPosition(rectBtn1.getGlobalBounds().getCenter() + offSpr);
        window.draw(*dlgSpr1);
    }
    window.draw(rectBtn1);
    window.draw(txtBtn1);
    if (dlgBtn2.size()) {
        if (dlgSpr2) {
            dlgSpr2->setPosition(rectBtn2.getGlobalBounds().getCenter() + offSpr);
            window.draw(*dlgSpr2);
        }
        window.draw(rectBtn2);
        window.draw(txtBtn2);
    }
    if (dlgCancel) {
        window.draw(rectCancel);
        window.draw(txtCancel);
    }
}


void MainGame::draw4Backs() {
    for (int i = 0; i < 4; ++i) {
        assets.back.setPosition(posRoom[i]);
        window.draw(assets.back);
    }
}


void MainGame::matchAspectRatio(sf::View &view, sf::Vector2u winSize) {
    const float aspect_ratio = (float)view.getSize().x / view.getSize().y, new_asp = (float)winSize.x / winSize.y;
    if (new_asp > aspect_ratio) view.setViewport(sf::FloatRect({(1 - aspect_ratio / new_asp) / 2, 0}, {aspect_ratio / new_asp, 1}));
    else view.setViewport(sf::FloatRect({0, (1 - new_asp /aspect_ratio) / 2}, {1, new_asp / aspect_ratio}));
}


void MainGame::center(sf::Text &t, const sf::FloatRect &rect, sf::Vector2f off) const {
    const float off_x = off.x, off_y = -0.1 * t.getCharacterSize() + off.y;  // strings look some pixels too low
    auto r = t.getLocalBounds();
    sf::Vector2f size = r.size + r.position;
    t.setPosition({rect.position.x + (rect.size.x - size.x) / 2 + off_x, rect.position.y + (rect.size.y - size.y) / 2 + off_y});
}


void MainGame::center(sf::Text &t, const sf::Shape &sh, sf::Vector2f off) const {
    center(t, sh.getGlobalBounds(), off);
}


int MainGame::currentCards() const {
    return std::count_if(std::begin(room), std::end(room), [](auto &r){ return r; });
}


int MainGame::finalScore() const {
    if (health <= 0) {
        int score = 0;
        for (const Card &c: deck.deck())
            if (c.suit == 's' || c.suit == 'c')  score -= c.value;
        return score;
    }
    else {
        int score = health;
        if (score == 20)
            for (int i = 0; i < 4; ++i)
                if (room[i] && room[i]->suit == 'h') {  // there should be 1 only card remaining
                    score += room[i]->value;
                    break;
                }
        return score;
    }
}


MainGame::UserInput MainGame::showDialog(const std::string &text, const std::string &btn1, const std::string &btn2, bool cancel, sf::Sprite *spr1, sf::Sprite *spr2) {
    dlgText = text;
    txtDialog.setString(dlgText);
    center(txtDialog, rectDlg, cancel ? sf::Vector2f(0, -30): sf::Vector2f(0, -30));
    dlgBtn1 = btn1;
    txtBtn1.setString(dlgBtn1);
    center(txtBtn1, rectBtn1);
    dlgBtn2 = btn2;
    if (dlgBtn2.size()) {
        txtBtn2.setString(dlgBtn2);
        center(txtBtn2, rectBtn2);
    }
    dlgCancel = cancel;
    if (dlgCancel) {
        txtCancel.setString("X");
        center(txtCancel, rectCancel);
    }
    dlgSpr1 = spr1;
    dlgSpr2 = spr2;
    syncGui([this](){ drawFunctions[1] = [this](){ drawDialog(); }; });

    // object to remove drawDialog() function at function exit
    struct Exit {
        Exit() {}
        ~Exit() { syncGui([](){ drawFunctions[1] = [](){}; }); }
    };
    Exit ex;

    // wait user action
    while(1) {
        auto event = waitEvent();
        if (const auto *e = event.getIf<sf::Event::KeyPressed>()) {
            if (e->code == sf::Keyboard::Key::Num1) return UserInput::Btn1;
            if (e->code == sf::Keyboard::Key::Num2 && dlgBtn2.size()) return UserInput::Btn2;
            if (e->code == sf::Keyboard::Key::Escape && dlgCancel) return UserInput::Cancel;
        }
        if (const auto *e = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (e->button != sf::Mouse::Button::Left) continue;
            auto pos = window.mapPixelToCoords(e->position);
            if (sf::FloatRect(posBtn1, szBtn12).contains(pos)) return UserInput::Btn1;
            if (sf::FloatRect(posBtn2, szBtn12).contains(pos) && dlgBtn2.size()) return UserInput::Btn2;
            if (sf::FloatRect(posCancel, szBtnCancel).contains(pos) && dlgCancel) return UserInput::Cancel;
        }
    }
}
