//yahelomessi@gmail.com

#pragma once
#include "RoleBase.hpp"

namespace coup_game{

    class Spy:public RoleBase{
    public:

        //returns the role name
        std::string name() const override;

        //sees the amount of coins of another player
        void onSpy(Player& source, Player& target) const;

    };

}
