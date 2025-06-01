//yahelomessi@gmail.com

#include "Spy.hpp"
#include "Player.hpp"
#include <iostream>

namespace coup_game {

    //returns the role name
    std::string Spy::name() const{
        return "Spy";
    }
    
    //sees the amount of coins of another player
    void Spy::onSpy(Player& source, Player& target) const{
        std::cout << "[Role Effect] " << source.getName()
                  << " (Spy) sees that " << target.getName()
                  << " has " << target.getCoins() << " coins.\n";
    }
    

}
