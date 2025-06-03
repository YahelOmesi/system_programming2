//yahelomessi@gmail.com

#include "Game.hpp"
#include "Player.hpp"
#include "Actions.hpp"
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>

using namespace std;
using namespace coup_game;

void printStatus(const Game& game){
    cout << "\n[--- Player Status ---]\n";
    for(const auto& p : game.getAllPlayers()){
        cout << "[Status] Player: " << p->getName()
             << " | Role: " << roleToString(p->getRole())
             << " | Coins: " << p->getCoins()
             << " | Alive: " << (p->isAlive() ? "Yes" : "No")
             << " | Under Sanction: " << (p->isUnderSanction() ? "Yes" : "No")
             << " | Last Turn: " << p->getLastTurnPlayed()
             << "\n";
    }
}

int main() {

    Game game; //initialize a new game instance

    //creates 6 players, each with a distinct role 
    auto spy = make_shared<Player>("Spy", Role::Spy);
    auto judge = make_shared<Player>("Judge", Role::Judge);
    auto merchant = make_shared<Player>("Merchant", Role::Merchant);
    auto baron = make_shared<Player>("Baron", Role::Baron);
    auto general = make_shared<Player>("General", Role::General);
    auto governor = make_shared<Player>("Governor", Role::Governor);

    //ddd players to the game. The order matters since it determines turn order.
    game.addPlayer(spy);
    game.addPlayer(judge);
    game.addPlayer(merchant);
    game.addPlayer(baron);
    game.addPlayer(general);
    game.addPlayer(governor);

    //assigns initial coin states
    spy->addCoins(2);
    judge->addCoins(4);
    merchant->addCoins(0);
    baron->addCoins(3);
    general->addCoins(5);
    governor->addCoins(7);

    cout << "\n--- Turn 1: Spy ---\n";
    Actions::spyBlockArrest(game, spy, judge); //spy disables judge's next arrest
    Actions::gather(game, spy);  //spy collects 1 coin
    printStatus(game);

    cout << "\n--- Turn 2: Judge ---\n";
    try{
        Actions::arrest(game, judge, general); //attempt to arrest General
    }catch(...){} //catches any exceptions
    Actions::tax(game, judge);//collect 2 coins
    printStatus(game);

    cout << "\n--- Turn 3: Merchant ---\n";
    Actions::gather(game, merchant); //merchant gathers 1 coin
    printStatus(game);

    cout << "\n--- Turn 4: Baron ---\n";
    Actions::invest(game, baron); //baron invests 3 coins to get 6
    printStatus(game);

    cout << "\n--- Turn 5: General ---\n"; 
    Actions::tax(game, general); //general collects 2 coins
    printStatus(game);

    cout << "\n--- Turn 6: Governor ---\n";
    Actions::tax(game, governor); //governor collects 3 coins
    printStatus(game);

    cout << "\n--- Turn 7: Spy ---\n"; 
    Actions::tax(game, spy); //spy collects 2 coins
    Actions::cancelTax(game, governor, spy); //governor cancels it
    printStatus(game);

    //manual coin adjustments
    judge->addCoins(7);
    general->addCoins(5);

    cout << "\n--- Turn 8: Judge ---\n";
    Actions::coup(game, judge, general); //judge tries to coup
    Actions::blockCoup(game, general, judge, general); //general blocks it
    printStatus(game);

    //manual coin adjustments
    merchant->addCoins(4);

    cout << "\n--- Turn 9: Merchant ---\n";
    Actions::bribe(game, merchant); //merhant bribe and get an extra turn
    Actions::cancelBribe(game, judge, merchant); //judge cancled his bribe
    printStatus(game);

    //manual coin adjustments
    baron->addCoins(4);

    cout << "\n--- Turn 10: Baron ---\n";
    Actions::coup(game, baron, general); //baron coup general
    printStatus(game);

    //manual coin adjustments
    governor->addCoins(7);

    cout << "\n--- Turn 11: Governor ---\n";
    Actions::coup(game, governor, spy); //governor coup spy
    printStatus(game);

    //manual coin adjustments
    judge->addCoins(7);

    cout << "\n--- Turn 12: Judge ---\n";
    Actions::coup(game, judge, merchant); //judge coup nerchant
    printStatus(game);

    //manual coin adjustments
    baron->addCoins(7);

    cout << "\n--- Turn 13: Baron ---\n";
    Actions::coup(game, baron, governor); //baron coup governor
    printStatus(game);

    //manual coin adjustments
    judge->addCoins(7);

    cout << "\n--- Turn 14: Judge ---\n";
    Actions::coup(game, judge, baron); //judge coup baron
    printStatus(game);

 
    //the winner should be judge
    cout << "\n====== GAME OVER ======\n";
    cout << "Winner: " << game.winner() << " \n";
    printStatus(game);

    return 0;
}

