//yahelomessi@gmail.com

#include "Merchant.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <iostream>

namespace coup_game{

    std::string Merchant::name() const{
        return "Merchant";
    }

    //1 coin bonus if starting a turn with 3 coins at least
    void Merchant::onTurnStart(Player& player, Game& game){
        if(player.getCoins() >= 3){
            player.addCoins(1);
            std::cout << "[Role Effect] " << player.getName()
                      << " (Merchant) received 1 bonus coin for starting the turn with 3+ coins.\n";
        }
    }

    //if he was arrested, pays 2 to the pot instead of losing 1 to the attacker
    bool Merchant::onArrested(Player& target, Player& attacker, Game& game){
        int toPay = std::min(2, target.getCoins());
        target.removeCoins(toPay);
    
        std::cout << "[Role Effect] " << target.getName()
                  << " (Merchant) paid " << toPay
                  << " coin(s) to the treasury instead of 1 coin to the attecker.\n";
    
        return true; //indicate custom arrest handling was applied
    }

}
