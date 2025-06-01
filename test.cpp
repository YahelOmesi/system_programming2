//yahelomessi@gmail.com

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Actions.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Governor.hpp"
#include "Merchant.hpp"
#include "Judge.hpp"
#include "Spy.hpp"

#include <memory>
#include <vector>

using namespace std;
using namespace coup_game;

////////////////////////////////////////////////////////////
// General Game Setup and Rules (1–8)
////////////////////////////////////////////////////////////

TEST_CASE("1. Game cannot start with fewer than 2 players"){
    Game game;
    auto p1 = make_shared<Player>("Alice", Role::Baron);
    game.addPlayer(p1);
    CHECK_THROWS(game.winner());
}

TEST_CASE("2. Cannot add a seventh player"){
    Game game;
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("P1", Role::Spy)));
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("P2", Role::Spy)));
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("P3", Role::Spy)));
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("P4", Role::Spy)));
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("P5", Role::Spy)));
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("P6", Role::Spy)));
    CHECK_THROWS(game.addPlayer(make_shared<Player>("P7", Role::Spy)));
}

TEST_CASE("3. Cannot add a player with duplicate name"){
    Game game;
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("Alice", Role::Spy)));
    CHECK_THROWS(game.addPlayer(make_shared<Player>("Alice", Role::Spy)));
}

TEST_CASE("4. Different players can have the same role"){
    Game game;
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("Player1", Role::Spy)));
    CHECK_NOTHROW(game.addPlayer(make_shared<Player>("Player2", Role::Spy)));
}

TEST_CASE("5. Player cannot perform paid actions with 0 coins"){
    Game game;
    auto p1 = make_shared<Player>("Alice", Role::General);
    auto p2 = make_shared<Player>("Bob", Role::Spy);
    game.addPlayer(p1);
    game.addPlayer(p2);
    CHECK_THROWS(Actions::bribe(game, p1));
    CHECK_THROWS(Actions::arrest(game, p1, p2));
    CHECK_THROWS(Actions::coup(game, p1, p2));
    CHECK_THROWS(Actions::sanction(game, p1, p2));
}

TEST_CASE("6. Player with 10+ coins must coup"){
    Game game;
    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);
    game.addPlayer(p1);
    game.addPlayer(p2);
    
    p1->addCoins(10);
    
    CHECK_THROWS(Actions::gather(game, p1));
    CHECK_THROWS(Actions::tax(game, p1));
    CHECK_THROWS(Actions::arrest(game, p1, p2));
    CHECK_THROWS(Actions::sanction(game, p1, p2));
    CHECK_NOTHROW(Actions::coup(game, p1, p2));
}

TEST_CASE("7. Turns go in player joining order"){
    Game game;
    auto a = make_shared<Player>("Alice", Role::Spy);
    auto b = make_shared<Player>("Bob", Role::Spy);
    auto c = make_shared<Player>("Carol", Role::Spy);
    game.addPlayer(a);
    game.addPlayer(b);
    game.addPlayer(c);
    CHECK(game.turn() == "Alice");
    Actions::gather(game, a);
    CHECK(game.turn() == "Bob");
    Actions::gather(game, b);
    CHECK(game.turn() == "Carol");
    Actions::gather(game, c);
    CHECK(game.turn() == "Alice");
}

TEST_CASE("8. Eliminated player cannot perform actions"){
    Game game;
    auto killer = make_shared<Player>("Killer", Role::Spy);
    auto victim = make_shared<Player>("Victim", Role::Spy);
    game.addPlayer(killer);
    game.addPlayer(victim);

    killer->addCoins(10);
    Actions::coup(game, killer, victim);

    //make sure he is not alive
    CHECK(victim->isAlive() == false);

    //aliminated player cannot perform any actions
    CHECK_THROWS(Actions::gather(game, victim));
    CHECK_THROWS(Actions::tax(game, victim));
    CHECK_THROWS(Actions::bribe(game, victim));
    CHECK_THROWS(Actions::arrest(game, victim, killer));
    CHECK_THROWS(Actions::coup(game, victim, killer));
}

////////////////////////////////////////////////////////////
// Gather Action Tests (9)
////////////////////////////////////////////////////////////

TEST_CASE("9. Gather adds 1 coin to current player"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    
    Actions::gather(game, p1);
    CHECK(p1->getCoins() == 1);
}

////////////////////////////////////////////////////////////
// Tax Action Tests (10)
////////////////////////////////////////////////////////////

TEST_CASE("10. Tax gives 2 coins to current player"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);

    Actions::tax(game, p1);
    CHECK(p1->getCoins() == 2);
}

////////////////////////////////////////////////////////////
// Bribe Action Tests (11)
////////////////////////////////////////////////////////////

TEST_CASE("11. Bribe costs 4 coins and does not end turn"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p1->addCoins(4);

    Actions::bribe(game, p1);

    CHECK(p1->getCoins() == 0);
    CHECK(game.turn() == "Alice");
}

////////////////////////////////////////////////////////////
// Arrest Action Tests (12–13)
////////////////////////////////////////////////////////////

TEST_CASE("12. Arrest transfers 1 coin from target to player"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p2->addCoins(1);

    Actions::arrest(game, p1, p2);

    CHECK(p1->getCoins() == 1);
    CHECK(p2->getCoins() == 0);
}

TEST_CASE("13. Cannot arrest same player in consecutive turns"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);
    auto p3 = make_shared<Player>("Carol", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    game.addPlayer(p3);
    p2->addCoins(2);
    p3->addCoins(2);

    Actions::arrest(game, p1, p2); //first turn
    Actions::gather(game, p2); //skip turn
    Actions::gather(game, p3); //skip turn

    CHECK_THROWS(Actions::arrest(game, p1, p2)); //consecutive arrest on same target
}

////////////////////////////////////////////////////////////
// Sanction Action Tests (14–16)
////////////////////////////////////////////////////////////

TEST_CASE("14. Sanction prevents tax"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p1->addCoins(3);

    Actions::sanction(game, p1, p2);
    game.advanceTurn();
    CHECK_THROWS(Actions::tax(game, p2));
}

TEST_CASE("15. Sanction prevents gather"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p1->addCoins(3);
    Actions::sanction(game, p1, p2);
    game.advanceTurn();
    CHECK_THROWS(Actions::gather(game, p2));
}

TEST_CASE("16. Sanction costs 3 coins"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p1->addCoins(3);
    Actions::sanction(game, p1, p2);
    
    CHECK(p1->getCoins() == 0);
}

////////////////////////////////////////////////////////////
// Coup Action Tests (17–18)
////////////////////////////////////////////////////////////

TEST_CASE("17. Coup eliminates another player"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p1->addCoins(7);
    Actions::coup(game, p1, p2);

    CHECK(p2->isAlive() == false);
}

TEST_CASE("18. Coup costs 7 coins"){

    Game game;

    auto p1 = make_shared<Player>("Alice", Role::Spy);
    auto p2 = make_shared<Player>("Bob", Role::Spy);

    game.addPlayer(p1);
    game.addPlayer(p2);
    p1->addCoins(7);
    Actions::coup(game, p1, p2);

    CHECK(p1->getCoins() == 0);
}

////////////////////////////////////////////////////////////
// Role: Governor (19–20)
////////////////////////////////////////////////////////////

TEST_CASE("19. Governor gets 3 coins from tax instead of 2"){

    Game game;

    auto gov = make_shared<Player>("Gov", Role::Governor);
    auto dummy = make_shared<Player>("D", Role::Spy);

    game.addPlayer(gov);
    game.addPlayer(dummy);
    Actions::tax(game, gov);
    
    CHECK(gov->getCoins() == 3);
}


TEST_CASE("20. Governor can block tax of others"){

    Game game;

    auto gov = make_shared<Player>("Gov", Role::Governor);
    auto baron = make_shared<Player>("Baron", Role::Baron);
    game.addPlayer(baron);
    game.addPlayer(gov);

    Actions::tax(game, baron);
    CHECK(baron->getCoins() == 2);

    //governor cancels the tax
    CHECK_NOTHROW(Actions::cancelTax(game, gov, baron));
    CHECK(baron->getCoins() == 0);
}

// ////////////////////////////////////////////////////////////
// // Role: Spy (21–23)
// ////////////////////////////////////////////////////////////

TEST_CASE("21. Spy can see coins of another player"){

    Game game;

    auto target = make_shared<Player>("T", Role::Spy);
    auto spy = make_shared<Player>("SpyGuy", Role::Spy);

    game.addPlayer(target);
    game.addPlayer(spy);
    game.startGame();

    Actions::gather(game, target);

    CHECK(Actions::spySeeCoins(game, spy, target) == 1);
}

TEST_CASE("22. Spy can block arrest of others on next turn"){

    Game game;

    auto spy = make_shared<Player>("SpyGuy", Role::Spy);
    auto judge = make_shared<Player>("Judgey", Role::Judge);
    auto target = make_shared<Player>("Victim", Role::Spy);

    game.addPlayer(spy);
    game.addPlayer(judge);
    game.addPlayer(target);

   judge->addCoins(3);

    Actions::spyBlockArrest(game, spy, judge);
    CHECK_THROWS(Actions::arrest(game, judge, target));
}

TEST_CASE("23. Spy block does not consume his turn"){

    Game game;

    auto spy = make_shared<Player>("SpyGuy", Role::Spy);
    auto judge = make_shared<Player>("Judgey", Role::Judge);

    game.addPlayer(spy);
    game.addPlayer(judge);

    Actions::spyBlockArrest(game, spy, judge);
    CHECK(game.turn() == "SpyGuy");
}

// ////////////////////////////////////////////////////////////
// // Role: Baron (24–25)
// ////////////////////////////////////////////////////////////

TEST_CASE("24. Baron can invest 3 coins to gain 6"){

    Game game;

    auto baron = make_shared<Player>("Baron", Role::Baron);
    auto dummy = make_shared<Player>("X", Role::Spy);

    game.addPlayer(baron);
    game.addPlayer(dummy);
    baron->addCoins(3);
    Actions::invest(game, baron);

    CHECK(baron->getCoins() == 6);
}

TEST_CASE("25. Baron gets 1 compensation when sanctioned") {
    Game game;
    auto baron = make_shared<Player>("Baron", Role::Baron);
    auto spy = make_shared<Player>("Spy", Role::Spy);
    game.addPlayer(spy);
    game.addPlayer(baron);
    spy->addCoins(3);

    CHECK(game.turn() == "Spy");

    Actions::sanction(game, spy, baron);
    CHECK(baron->getCoins() == 1);
}

// ////////////////////////////////////////////////////////////
// // Role: General (26–28)
// ////////////////////////////////////////////////////////////

TEST_CASE("26. General can block coup with 5 coins"){
    
    Game game;

    auto general = make_shared<Player>("General", Role::General);
    auto attacker = make_shared<Player>("Attacker", Role::Spy);
    auto target = make_shared<Player>("Target", Role::Spy);

    game.addPlayer(attacker);
    game.addPlayer(general);
    game.addPlayer(target);

    attacker->addCoins(7);
    general->addCoins(5);
    
    Actions::coup(game, attacker, target);
    CHECK(!target->isAlive());
    Actions::blockCoup(game, general, attacker, target);
    CHECK(target->isAlive());
    CHECK(general->getCoins() == 0);
}

TEST_CASE("27. Blocked coup still costs the attacker"){

    Game game;

    auto general = make_shared<Player>("General", Role::General);
    auto attacker = make_shared<Player>("Attacker", Role::Spy);
    auto target = make_shared<Player>("Target", Role::Spy);
    
    game.addPlayer(attacker);
    game.addPlayer(general);
    game.addPlayer(target);

    attacker->addCoins(7);
    general->addCoins(5);

    Actions::coup(game, attacker, target);
    Actions::blockCoup(game, general, attacker, target);
    CHECK(attacker->getCoins() == 0);
}

TEST_CASE("28. General gets back coin if arrested, judge gains nothing"){

    Game game;

    auto general = make_shared<Player>("General", Role::General);
    auto judge = make_shared<Player>("Judgey", Role::Judge);

    game.addPlayer(general);
    game.addPlayer(judge);

    judge->addCoins(3);

    Actions::gather(game, general);
    Actions::arrest(game, judge, general);

    CHECK(general->getCoins() == 1);
    CHECK(judge->getCoins() == 3);
}

// ////////////////////////////////////////////////////////////
// // Role: Judge (29–31)
// ////////////////////////////////////////////////////////////

TEST_CASE("29. Judge can block bribe"){

    Game game;

    auto judge = make_shared<Player>("Judge", Role::Judge);
    auto briber = make_shared<Player>("Briber", Role::General);

    game.addPlayer(briber);
    game.addPlayer(judge);

    briber->addCoins(4);

    Actions::bribe(game, briber);
    Actions::cancelBribe(game, judge, briber);

    CHECK(briber->getCoins() == 0);
}

TEST_CASE("30. Briber loses 4 coins even if blocked"){

    Game game;

    auto judge = make_shared<Player>("Judge", Role::Judge);
    auto briber = make_shared<Player>("Briber", Role::General);

    game.addPlayer(briber);
    game.addPlayer(judge);
    
    briber->addCoins(4);

    Actions::bribe(game, briber);
    Actions::cancelBribe(game, judge, briber);
    CHECK(briber->getCoins() == 0);
}

TEST_CASE("31. Player who sanctioned judge pays extra coin"){

    Game game;

    auto judge = make_shared<Player>("Judge", Role::Judge);
    auto spy = make_shared<Player>("Spy", Role::Spy);

    game.addPlayer(spy);
    game.addPlayer(judge);
    
    spy->addCoins(4);
    Actions::sanction(game, spy, judge);

    CHECK(spy->getCoins() == 0);
}

// ////////////////////////////////////////////////////////////
// // Role: Merchant (32–33)
// ////////////////////////////////////////////////////////////

TEST_CASE("32. Merchant gets 1 bonus coin if starts turn with 3+"){

    Game game;
    
    auto merchant = make_shared<Player>("Merchant", Role::Merchant);
    auto dummy = make_shared<Player>("X", Role::Spy);

    game.addPlayer(dummy);
    game.addPlayer(merchant);

    merchant->addCoins(3);

    Actions::gather(game, dummy); //advance turn to merchant again
    CHECK(merchant->getCoins() == 4);
}

TEST_CASE("33. Merchant pays 2 coins to bank when arrested"){

    Game game;

    auto merchant = make_shared<Player>("Merchant", Role::Merchant);
    auto judge = make_shared<Player>("Judge", Role::Judge);

    game.addPlayer(judge);
    game.addPlayer(merchant);

    judge->addCoins(3);
    merchant->addCoins(2);

    Actions::arrest(game, judge, merchant);
    CHECK(merchant->getCoins() == 0);
}

////////////////////////////////////////////////////////////
// Turn Transition and Turn Validity (34–35)
////////////////////////////////////////////////////////////

TEST_CASE("34. Turn advances only after valid action"){

    Game game;

    auto a = make_shared<Player>("A", Role::Spy);
    auto b = make_shared<Player>("B", Role::Spy);

    game.addPlayer(a);
    game.addPlayer(b);

    CHECK(game.turn() == "A");

    CHECK_THROWS(Actions::gather(game, b));
    CHECK(game.turn() == "A"); 

    CHECK_NOTHROW(Actions::gather(game, a)); 
    CHECK(game.turn() == "B");
}

TEST_CASE("35. Only current turn player can act"){
    Game game;
    auto a = make_shared<Player>("A", Role::Spy);
    auto b = make_shared<Player>("B", Role::Spy);

    game.addPlayer(a);
    game.addPlayer(b);

    CHECK_THROWS(Actions::tax(game, b));       
    CHECK_NOTHROW(Actions::gather(game, a));  
    CHECK_NOTHROW(Actions::tax(game, b));    
}


// ////////////////////////////////////////////////////////////
// // Required Public Methods (36–38)
// ////////////////////////////////////////////////////////////

TEST_CASE("36. turn() returns the current player's name"){

    Game game;

    auto a = make_shared<Player>("Alpha", Role::Spy);
    auto b = make_shared<Player>("Beta", Role::Spy);

    game.addPlayer(a);
    game.addPlayer(b);

    CHECK(game.turn() == "Alpha");
    Actions::gather(game, a);
    CHECK(game.turn() == "Beta");
}

TEST_CASE("37. players() returns only alive players"){

    Game game;

    auto a = make_shared<Player>("A", Role::Spy);
    auto b = make_shared<Player>("B", Role::Spy);
    auto c = make_shared<Player>("C", Role::Spy);

    game.addPlayer(a);
    game.addPlayer(b);
    game.addPlayer(c);

    a->addCoins(6);
    
    Actions::gather(game, a); //6+1=7
    Actions::gather(game, b);
    Actions::gather(game, c);

    Actions::coup(game, a, b);

    vector<string> alive = game.players();
    CHECK(alive.size() == 2);
    CHECK(find(alive.begin(), alive.end(), "B") == alive.end()); 
}

TEST_CASE("38. winner() returns the last player, throws if game not over"){

    Game game;

    auto a = make_shared<Player>("Alpha", Role::Spy);
    auto b = make_shared<Player>("Beta", Role::Spy);

    game.addPlayer(a);
    game.addPlayer(b);

    a->addCoins(7);

    Actions::gather(game, a);
    Actions::gather(game, b);

    Actions::coup(game, a, b);

    CHECK(game.players().size() == 1);
    CHECK(game.winner() == "Alpha");

    Game g2;

    auto p1 = make_shared<Player>("X", Role::Spy);
    auto p2 = make_shared<Player>("Y", Role::Spy);

    g2.addPlayer(p1);
    g2.addPlayer(p2);

    CHECK_THROWS(g2.winner());
}

	
