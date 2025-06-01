//yahelomessi@gmail.com

#pragma once
#include "RoleBase.hpp"

namespace coup_game{

class Governor:public RoleBase{

public:

    //returns the role name
    std::string name() const override;

};

}
