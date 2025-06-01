#include "Player.hpp"
#include "Baron.hpp"
#include "Spy.hpp"
#include "Judge.hpp"
#include "Governor.hpp"
#include "Merchant.hpp"
#include "General.hpp"

using namespace std;

namespace coup_game {

    string roleToString(Role role) {
        switch (role) {
            case Role::Governor: return "Governor";
            case Role::Spy: return "Spy";
            case Role::Baron: return "Baron";
            case Role::General: return "General";
            case Role::Judge: return "Judge";
            case Role::Merchant: return "Merchant";
            case Role::None: return "None";
        }
        return "Unknown";
    }

    string resultToString(ActionResult result) {
        switch (result) {
            case ActionResult::Pending: return "Pending";
            case ActionResult::Succeeded: return "Succeeded";
            case ActionResult::Blocked: return "Blocked";
        }
        return "Unknown";
    }

    Player::Player(const string& name, Role role)
        : name(name), role(role), coins(0), alive(true), lastTurnPlayed(0) {
        cout << "[Player] Created player '" << name << "' with role '" << roleToString(role) << "'\n";
        assignRolePtr();
    }

    string Player::getName() const { return name; }
    Role Player::getRole() const { return role; }
    int Player::getCoins() const { return coins; }
    bool Player::isAlive() const { return alive; }
    int Player::getLastTurnPlayed() const { return lastTurnPlayed; }
    const vector<Action>& Player::getActionsHistory() const { return actionsHistory; }
    shared_ptr<RoleBase> Player::getRolePtr() const { return rolePtr; }
    string Player::getRoleName() const { return rolePtr ? rolePtr->name() : "None"; }

    void Player::addCoins(int amount) {
        if (amount < 0) throw invalid_argument("cannot add negative coins.");
        coins += amount;
        cout << "[Player] " << name << " received " << amount << " coins. Total: " << coins << "\n";
    }

    void Player::removeCoins(int amount) {
        if (amount < 0) throw invalid_argument("cannot remove negative coins.");
        if (coins < amount) throw runtime_error("not enough coins to remove.");
        coins -= amount;
        cout << "[Player] " << name << " lost " << amount << " coins. Total: " << coins << "\n";
    }

    void Player::eliminate() {
        alive = false;
        cout << "[Player] " << name << " has been eliminated from the game.\n";
    }

    void Player::setLastTurnPlayed(int turn) {
        lastTurnPlayed = turn;
        cout << "[Player] " << name << " played on turn " << turn << "\n";
    }

    void Player::addAction(const string& type, const string& target, int turn, ActionResult result) {
        actionsHistory.push_back({type, target, turn, result});
        cout << "[Player] " << name << " performed action '" << type
             << "' on '" << target << "' (turn " << turn
             << ", result: " << resultToString(result) << ")\n";
    }

    void Player::updateLastActionResult(ActionResult result) {
        if (actionsHistory.empty()) throw runtime_error("no action to update.");
        actionsHistory.back().result = result;
        cout << "[Player] " << name << "'s last action updated to result: " << resultToString(result) << "\n";
    }

    void Player::setRolePtr(shared_ptr<RoleBase> r) {
        rolePtr = move(r);
        cout << "[Role Assigned] " << name << " is now a " << getRoleName() << ".\n";
    }

    void Player::assignRolePtr() {
        switch (role) {
            case Role::Baron: rolePtr = make_shared<Baron>(); break;
            case Role::Spy: rolePtr = make_shared<Spy>(); break;
            case Role::Judge: rolePtr = make_shared<Judge>(); break;
            case Role::Governor: rolePtr = make_shared<Governor>(); break;
            case Role::Merchant: rolePtr = make_shared<Merchant>(); break;
            case Role::General: rolePtr = make_shared<General>(); break;
            default: rolePtr = nullptr; break;
        }
    }

    void Player::printStatus() const {
        cout << "[Status] Player: " << name
             << " | Role: " << getRoleName()
             << " | Coins: " << coins
             << " | Alive: " << (alive ? "Yes" : "No")
             << " | Under Sanction: " << (isUnderSanction() ? "Yes" : "No")
             << " | Last Turn: " << lastTurnPlayed
             << "\n";
    }

    void Player::printHistory() const {
        cout << "[History] Action log for " << name << ":\n";
        for (const Action& action : actionsHistory) {
            cout << "  - Turn " << action.turnNumber
                 << ": " << action.type
                 << " → " << action.target
                 << " [" << resultToString(action.result) << "]\n";
        }
    }

    void Player::recordBlockedAction(const string& actionType, const string& target, int turnNumber) {
        setLastTurnPlayed(turnNumber);
        addAction(actionType, target, turnNumber, ActionResult::Blocked);
    }

    void Player::setArrestDisabled(bool value) { arrestDisabled = value; }
    bool Player::isArrestDisabled() const { return arrestDisabled; }
    void Player::revive() { alive = true; }

    bool Player::isUnderSanction() const { return underSanction; }
    void Player::setUnderSanction(bool value) {
        underSanction = value;
        sanctionJustApplied = value;
    }

    bool Player::isSanctionJustApplied() const { return sanctionJustApplied; }
    void Player::clearSanctionJustApplied() { sanctionJustApplied = false; }
}

