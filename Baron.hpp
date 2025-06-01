//yahelomessi@gmail.com

#pragma once
#include "RoleBase.hpp"

namespace coup_game{

    class Baron:public RoleBase{
    public:

        //returns the role name
        std::string name() const override;

        //baron gets automatic compensation in the event of a sanction
        void onSanctioned(Player& player, Player& source, Game& game) override;
    };

}

