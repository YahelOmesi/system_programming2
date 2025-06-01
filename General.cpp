//yahelomessi@gmail.com

#include "General.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <iostream>

namespace coup_game{

    //returns the role name
    std::string General::name() const{
        return "General";
    }

    //if arrested, the general gets back the coin taken from him
    bool General::onArrested(Player& target, Player& attacker, Game& game){
        std::cout << "[Role Effect] " << target.getName()
                  << " (General) got back the coin taken during arrest.\n";
        return true; 
    }

}