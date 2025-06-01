//yahelomessi@gmail.com

#include "Baron.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <iostream>
#include <stdexcept>

namespace coup_game{

    //returns the role name
    std::string Baron::name() const{
        return "Baron";
    }

    //baron gets automatic compensation in the event of a sanction
    void Baron::onSanctioned(Player& player, Player& source, Game& game){
        player.addCoins(1); //compensation of 1 coin
        std::cout << "[Role Effect] " << player.getName()
                  << " (Baron) received 1 coin as compensation for being sanctioned.\n";
    }

}

