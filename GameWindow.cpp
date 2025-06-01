// 👇 כאן יופיע הקובץ במלואו (משתמשת ביקשה הכל בפנים)

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include "Game.hpp"
#include "Player.hpp"
#include "Actions.hpp"

using namespace std;
using namespace coup_game;

struct Confetti {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

int main() {
    Game game;

    auto spy = make_shared<Player>("Shira", Role::Spy);
    auto judge = make_shared<Player>("Hadar", Role::Judge);
    auto merchant = make_shared<Player>("Noam", Role::Merchant);
    auto baron = make_shared<Player>("Evyatar", Role::Baron);
    auto general = make_shared<Player>("Barak", Role::General);
    auto governor = make_shared<Player>("Shay", Role::Governor);

    game.addPlayer(spy);
    game.addPlayer(judge);
    game.addPlayer(merchant);
    game.addPlayer(baron);
    game.addPlayer(general);
    game.addPlayer(governor);

    spy->addCoins(2);
    judge->addCoins(4);
    merchant->addCoins(3);
    baron->addCoins(3);
    general->addCoins(5);
    governor->addCoins(7);

    sf::RenderWindow window(sf::VideoMode(1200, 720), "Coup Game");

    sf::Font font;
    if (!font.loadFromFile("Marcellus-Regular.ttf")) {
        cerr << "❌ Could not load font!" << endl;
        return -1;
    }

    vector<string> actions = {"gather", "tax", "bribe", "arrest", "sanction", "coup"};
    map<string, sf::Texture> actionTextures;
    map<string, sf::Sprite> actionSprites;

    float buttonWidth = 200, buttonHeight = 160, buttonGap = 10, rowGap = 10, startY = 140;
    float centerX = (window.getSize().x - (3 * buttonWidth + 2 * buttonGap)) / 2.f;

    for (size_t i = 0; i < actions.size(); ++i) {
        const string& name = actions[i];
        sf::Texture texture;
        if (!texture.loadFromFile("images/" + name + ".png")) {
            cerr << "❌ Failed to load image: " << name << endl;
            return -1;
        }
        actionTextures[name] = texture;

        sf::Sprite sprite;
        sprite.setTexture(actionTextures[name]);
        sprite.setScale(buttonWidth / sprite.getLocalBounds().width, buttonHeight / sprite.getLocalBounds().height);

        int row = i / 3, col = i % 3;
        float x = centerX + col * (buttonWidth + buttonGap);
        float y = startY + row * (buttonHeight + rowGap);
        sprite.setPosition(x, y);

        actionSprites[name] = sprite;
    }

    string errorMessage = "", infoMessage = "";
    sf::Clock errorClock, infoClock;
    bool showError = false, showInfo = false;
    bool isAwaitingTarget = false;
    string pendingAction = "";
    shared_ptr<Player> actionInitiator = nullptr;

    bool winnerAnnounced = false;
    string winnerName = "";
    sf::Clock winnerClock;
    vector<Confetti> confetti;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f click = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                auto current = game.getCurrentPlayer();

                if (isAwaitingTarget) {
                    const auto& players = game.getAllPlayers();
                    float cardW = 150, cardH = 110, gap = 10, y = 600;
                    int numPlayers = players.size();
                    float totalWidth = numPlayers * cardW + (numPlayers - 1) * gap;
                    float margin = 40;
                    float startX = max((window.getSize().x - totalWidth) / 2.0f, margin);

                    for (size_t i = 0; i < players.size(); ++i) {
                        auto& p = players[i];
                        float x = startX + i * (cardW + gap);
                        sf::FloatRect area(x, y, cardW, cardH);
                        if (area.contains(click)) {
                            try {
                                if (pendingAction == "arrest") Actions::arrest(game, actionInitiator, p);
                                else if (pendingAction == "sanction") Actions::sanction(game, actionInitiator, p);
                                else if (pendingAction == "coup") Actions::coup(game, actionInitiator, p);
                                else if (pendingAction == "preventArrest") Actions::spyBlockArrest(game, actionInitiator, p);
                                else if (pendingAction == "seeCoins") {
                                    int coins = Actions::spySeeCoins(game, actionInitiator, p);
                                    infoMessage = p->getName() + " has " + to_string(coins) + " coins.";
                                    infoClock.restart(); showInfo = true;
                                } else if (pendingAction == "cancelBribe") Actions::cancelBribe(game, actionInitiator, p);
                                else if (pendingAction == "cancelTax") Actions::cancelTax(game, actionInitiator, p);
                                else if (pendingAction == "blockCoup") Actions::blockCoup(game, actionInitiator, game.getLastCoupInitiator(), p);
                            } catch (exception& e) {
                                errorMessage = e.what();
                                errorClock.restart(); showError = true;
                            }
                            isAwaitingTarget = false;
                            pendingAction = "";
                            actionInitiator = nullptr;
                            break;
                        }
                    }
                } else {
                    for (const string& name : actions) {
                        if (actionSprites[name].getGlobalBounds().contains(click)) {
                            try {
                                if (name == "gather") Actions::gather(game, current);
                                else if (name == "tax") Actions::tax(game, current);
                                else if (name == "bribe") Actions::bribe(game, current);
                                else {
                                    isAwaitingTarget = true;
                                    pendingAction = name;
                                    actionInitiator = current;
                                    infoMessage = "👉 Choose a player to " + name + ".";
                                    infoClock.restart(); showInfo = true;
                                }
                            } catch (exception& e) {
                                errorMessage = e.what();
                                errorClock.restart(); showError = true;
                            }
                            break;
                        }
                    }

                    // ✔️ בדיקת כפתורי פעולות מיוחדות (Invest וכו)
                    const auto& players = game.getAllPlayers();
                    float cardW = 150, gap = 10, y = 600;
                    float totalWidth = players.size() * cardW + (players.size() - 1) * gap;
                    float startX = max((window.getSize().x - totalWidth) / 2.0f, 40.f);

                    for (size_t i = 0; i < players.size(); ++i) {
                        auto& p = players[i];
                        float x = startX + i * (cardW + gap);
                        vector<pair<string, string>> specials;

                        switch (p->getRole()) {
                            case Role::Spy: specials = {{"See Coins", "seeCoins"}, {"Prevent Arrest", "preventArrest"}}; break;
                            case Role::Judge: specials = {{"Cancel Bribe", "cancelBribe"}}; break;
                            case Role::Baron: specials = {{"Invest", "invest"}}; break;
                            case Role::General: specials = {{"Block Coup", "blockCoup"}}; break;
                            case Role::Governor: specials = {{"Cancel Tax", "cancelTax"}}; break;
                            default: break;
                        }

                        for (size_t j = 0; j < specials.size(); ++j) {
                            sf::FloatRect btnRect(x + 5, y - 35 - j * 30, cardW - 10, 25);
                            if (btnRect.contains(click)) {
                                if (specials[j].second == "invest") {
                                    try {
                                        Actions::invest(game, p);
                                    } catch (exception& e) {
                                        errorMessage = e.what(); errorClock.restart(); showError = true;
                                    }
                                } else {
                                    pendingAction = specials[j].second;
                                    isAwaitingTarget = true;
                                    actionInitiator = p;
                                    infoMessage = "👉 Choose a player for " + specials[j].first;
                                    infoClock.restart(); showInfo = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        // ציור:
        window.clear(sf::Color(238, 232, 170));
        auto current = game.getCurrentPlayer();
        sf::Text turnText("Current Turn: " + current->getName() + " (Role: " + roleToString(current->getRole()) + ")", font, 24);
        turnText.setFillColor(sf::Color::Black);
        turnText.setStyle(sf::Text::Bold);
        turnText.setPosition(20, 20);
        window.draw(turnText);

        for (const auto& name : actions) window.draw(actionSprites[name]);

        const auto& players = game.getAllPlayers();
        float cardW = 150, cardH = 110, gap = 10, y = 600;
        int numPlayers = players.size();
        float totalWidth = numPlayers * cardW + (numPlayers - 1) * gap;
        float margin = 40;
        float startX = max((window.getSize().x - totalWidth) / 2.0f, margin);

        for (size_t i = 0; i < players.size(); ++i) {
            auto& p = players[i];
            float x = startX + i * (cardW + gap);
            sf::RectangleShape card(sf::Vector2f(cardW, cardH));
            card.setFillColor(p->isAlive() ? sf::Color(47, 79, 79) : sf::Color(105, 105, 105));
            card.setPosition(x, y);
            window.draw(card);

            vector<string> info = {
                "Name: " + p->getName(),
                "Role: " + roleToString(p->getRole()),
                "Coins: " + to_string(p->getCoins()),
                "Alive: " + string(p->isAlive() ? "Yes" : "No"),
                "Under Sanction: " + string(p->isUnderSanction() ? "Yes" : "No")
            };

            for (size_t j = 0; j < info.size(); ++j) {
                sf::Text txt(info[j], font, 14);
                txt.setPosition(x + 10, y + 5 + j * 20);
                txt.setFillColor(sf::Color::White);
                window.draw(txt);
            }

            vector<pair<string, string>> specials;
            switch (p->getRole()) {
                case Role::Spy: specials = {{"See Coins", "seeCoins"}, {"Prevent Arrest", "preventArrest"}}; break;
                case Role::Judge: specials = {{"Cancel Bribe", "cancelBribe"}}; break;
                case Role::Baron: specials = {{"Invest", "invest"}}; break;
                case Role::General: specials = {{"Block Coup", "blockCoup"}}; break;
                case Role::Governor: specials = {{"Cancel Tax", "cancelTax"}}; break;
                default: break;
            }

            for (size_t j = 0; j < specials.size(); ++j) {
                sf::RectangleShape btn(sf::Vector2f(cardW - 10, 25));
                btn.setFillColor(sf::Color(0, 128, 128));
                btn.setPosition(x + 5, y - 35 - j * 30);
                window.draw(btn);

                sf::Text label(specials[j].first, font, 14);
                label.setFillColor(sf::Color::White);
                label.setPosition(btn.getPosition().x + 5, btn.getPosition().y + 4);
                window.draw(label);
            }
        }

        if (showError && errorClock.getElapsedTime().asSeconds() < 3.0f) {
            sf::Text errorText(errorMessage, font, 20);
            errorText.setFillColor(sf::Color::Red);
            errorText.setPosition(20, 500); 
            window.draw(errorText);
        }

        if (showInfo && infoClock.getElapsedTime().asSeconds() < 3.0f) {
            sf::Text infoText(infoMessage, font, 20);
            infoText.setFillColor(sf::Color::Black);
            infoText.setPosition(20, 480);  
            window.draw(infoText);
        }

        if (!winnerAnnounced) {
            try {
                winnerName = game.winner();
                winnerAnnounced = true;
                winnerClock.restart();
                for (int i = 0; i < 1000; ++i) {
                    Confetti c;
                    c.shape = sf::CircleShape(2.f);
                    c.shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
                    c.shape.setPosition(rand() % window.getSize().x, rand() % window.getSize().y);
                    c.velocity = sf::Vector2f((rand() % 200 - 100) / 100.f, (rand() % 100 + 50) / 50.f);
                    confetti.push_back(c);
                }
            } catch (...) {}
        }

        if (winnerAnnounced) {
            sf::RectangleShape bg(sf::Vector2f(window.getSize().x, 120));
            bg.setFillColor(sf::Color(0, 0, 0, 200));
            bg.setPosition(0, window.getSize().y / 2.f - 60);
            window.draw(bg);

            sf::Text winText("🏆 Winner: " + winnerName + " 🏆", font, 40);
            winText.setFillColor(sf::Color::White);
            winText.setStyle(sf::Text::Bold);
            winText.setPosition((window.getSize().x - winText.getGlobalBounds().width) / 2.f,
                                window.getSize().y / 2.f - 40);
            window.draw(winText);

            for (auto& c : confetti) {
                c.shape.move(c.velocity);
                if (c.shape.getPosition().y > window.getSize().y)
                    c.shape.setPosition(rand() % window.getSize().x, 0);
                window.draw(c.shape);
            }

            if (winnerClock.getElapsedTime().asSeconds() > 5.0f)
                window.close();
        }

        window.display();
    }

    return 0;
}
