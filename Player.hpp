#pragma once
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "RoleBase.hpp"

namespace coup_game {

    enum class Role {
        Governor, Spy, Baron, General, Judge, Merchant, None
    };

    enum class ActionResult {
        Pending, Succeeded, Blocked
    };

    std::string roleToString(Role role);
    std::string resultToString(ActionResult result);

    struct Action {
        std::string type;
        std::string target;
        int turnNumber;
        ActionResult result;
    };

    class Player {
    private:
        std::string name;
        Role role;
        int coins;
        bool alive;
        bool underSanction = false;
        bool sanctionJustApplied = false;
        int lastTurnPlayed;
        std::vector<Action> actionsHistory;
        std::shared_ptr<RoleBase> rolePtr;
        bool arrestDisabled = false;

    public:
        Player(const std::string& name, Role role);

        std::string getName() const;
        Role getRole() const;
        int getCoins() const;
        bool isAlive() const;
        int getLastTurnPlayed() const;
        const std::vector<Action>& getActionsHistory() const;
        std::shared_ptr<RoleBase> getRolePtr() const;
        std::string getRoleName() const;

        void addCoins(int amount);
        void removeCoins(int amount);
        void eliminate();
        void setLastTurnPlayed(int turn);
        void addAction(const std::string& type, const std::string& target, int turn, ActionResult result);
        void updateLastActionResult(ActionResult result);
        void setRolePtr(std::shared_ptr<RoleBase> r);
        void assignRolePtr();

        void printStatus() const;
        void printHistory() const;

        void recordBlockedAction(const std::string& actionType, const std::string& target, int turnNumber);

        void setArrestDisabled(bool value);
        bool isArrestDisabled() const;
        void revive();

        bool isUnderSanction() const;
        void setUnderSanction(bool value);
        bool isSanctionJustApplied() const;
        void clearSanctionJustApplied();
    };

}