//yahelomessi@gmail.com

#pragma once
#include <string>
#include <memory>

namespace coup_game{

class Player;
class Game;

class RoleBase{ //abstract clss
public:

    //retuns the name of a role, nust be implemented by all subclasses
    virtual std::string name() const = 0;

    //called at the beginning of a turn
    virtual void onTurnStart(Player& player, Game& game);

    //called when the player is arrested by another player
    virtual bool onArrested(Player& target, Player& attacker, Game& game);   

    //called when the player is sanctioned by another player
    virtual void onSanctioned(Player& player, Player& source, Game& game);

    //destructor
    virtual ~RoleBase() = default;
};

}
