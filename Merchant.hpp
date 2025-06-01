//yahelomessi@gmail.com

#pragma once
#include "RoleBase.hpp"

namespace coup_game{

    class Merchant:public RoleBase{
    public:

        //returns the role name
        std::string name() const override;

        //checks at the beginning of the turn if it deserves a bonus coin
        void onTurnStart(Player& player, Game& game) override;

        // When arrested – pays 2 to the pot instead of losing a coin to the attacking player
        bool onArrested(Player& target, Player& attacker, Game& game) override;    
    };

}
