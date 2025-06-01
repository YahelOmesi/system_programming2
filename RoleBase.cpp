//yahelomessi@gmail.com

#include "RoleBase.hpp"
#include "Player.hpp"
#include "Game.hpp"

namespace coup_game{

void RoleBase::onTurnStart(Player& player, Game& game){
    player.setUnderSanction(false);
}

bool RoleBase::onArrested(Player& target, Player& attacker, Game& game){
    return false;
}

void RoleBase::onSanctioned(Player& player, Player& source, Game& game){
}

}
