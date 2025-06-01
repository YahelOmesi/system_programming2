//yahelomessi@gmail.com

#pragma once
#include "RoleBase.hpp"

namespace coup_game{

    class General : public RoleBase{
    public:

        //returns the role name
        std::string name() const override;

        //when arrested, general recovers the stolen coin
        bool onArrested(Player& target, Player& attacker, Game& game) override;

    };

}

