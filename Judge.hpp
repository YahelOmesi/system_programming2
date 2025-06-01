//yahelomessi@gmail.com

#pragma once
#include "RoleBase.hpp"

namespace coup_game{

    class Judge:public RoleBase{
    public:

        //returns the role name
        std::string name() const override;

    };
}
