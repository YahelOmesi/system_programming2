//yahelomessi@gmail.com

#include "Actions.hpp"
#include "Spy.hpp"
#include "Judge.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include <iostream>
 #include <memory>

using namespace std;

namespace coup_game {

 //game actions

 //1. ------------------------- gather -------------------------
 
 void Actions::gather(Game& game, const shared_ptr<Player>& player){

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 cout << "[Game] game has started!\n";
 }else{
 throw runtime_error("cannot gather: not enough players to start the game.");
 }
 }

 //make sure it's the player's turn
 if(game.turn() != player->getName()){
 throw runtime_error("it's not " + player->getName() + "'s turn.");
 }

 //if there are 10+ coins, a coup is forced
 if(mustCoup(player)){
 throw runtime_error(player->getName() + " cannot perform 'gather' with 10+ coins.");
 }


 //if under sanction, the action is blocked
 if(player->isUnderSanction()){
 player->recordBlockedAction("gather", "bank", game.getTurnCounter());
 throw runtime_error(player->getName() + " is under sanction and cannot gather.");
 }

 //adds a coin to the player
 player->addCoins(1);

 //track player state
 player->setLastTurnPlayed(game.getTurnCounter());

 //records the action in the player's history
 player->addAction("gather", "bank", game.getTurnCounter(), ActionResult::Succeeded); //no one can block it afterwards
 cout << "[Game] " << player->getName() << " gathered 1 coin.\n";

 //advance to the next player only if the action was successful
 game.advanceTurn();
 }

 //2. ------------------------- tax -------------------------
 
 void Actions::tax(Game& game, const shared_ptr<Player>& player){

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 cout << "[Game] game has started!\n";
 }else{
 throw runtime_error("cannot tax: not enough players to start the game.");
 }
 }

 //make sure it's the player's turn
 if(game.turn() != player->getName()){
 throw runtime_error("it's not " + player->getName() + "'s turn.");
 }

 //if there are 10+ coins, a coup is forced
 if (mustCoup(player)) {
 throw runtime_error(player->getName() + " cannot perform 'tax' with 10+ coins.");
 }
 

 //if under sanction, the action is blocked
 if(player->isUnderSanction()){
 player->recordBlockedAction("tax", "bank", game.getTurnCounter());
 throw runtime_error(player->getName() + " is under sanction and cannot perform tax.");
 }

 //adds 2 coins to the player
 player->addCoins(2);

 if(player->getRole() == Role::Governor){
 player->addCoins(1);
 }

 //track player state
 player->setLastTurnPlayed(game.getTurnCounter());

 //records the action in the player's history
 player->addAction("tax", "bank", game.getTurnCounter(), ActionResult::Succeeded);
 cout << "[Game] " << player->getName() << " collected tax (2 coins).\n";

 //advance to the next player only if the action was successful
 game.advanceTurn();
 }
 
 void Actions::cancelTax(Game& game, const std::shared_ptr<Player>& governor, const std::shared_ptr<Player>& target){

 //check valid references
 if(!governor || !target){
 throw runtime_error("invalid player reference.");
 }
 
 //checks whether the canceler role even exists.
 if(governor->getRole() != Role::Governor){
 throw runtime_error(governor->getName() + " is not Governor and scannot cancel tax actions.");
 }

 //preventing self destruction
 if(governor == target){
 throw runtime_error("Governor cannnot cancel their own tax action");
 }
 
 //retrieves the history of target player actions
 const std::vector<Action>& history = target->getActionsHistory();
 if(history.empty()){
 throw runtime_error("no actions to cancel for " + target->getName() + ".");
 }
 
 //retrive the last action the target performed
 const Action& last = history.back();

 //only allow cancellation if the last action was a successful tax
 if(last.type != "tax" || last.result != ActionResult::Succeeded){
 throw runtime_error("last action was not a successful tax by " + target->getName() + ".");
 }
 
 target->removeCoins(2); //return the 2 he received
 target->updateLastActionResult(ActionResult::Blocked); //update action result
 
 std::cout << "[Role Effect] " << governor->getName()
 << " (Governor) canceled tax of " << target->getName() << ". 2 coins removed.\n";
 }
 
 //3. ------------------------- bribe -------------------------
 
 void Actions::bribe(Game& game, const shared_ptr<Player>& player){

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 cout << "[Game] game has started!\n";
 }else{
 throw runtime_error("cannot bribe: not enough players to start the game.");
 }
 }

 //make sure it's the player's turn
 if(game.turn() != player->getName()){
 throw runtime_error("it's not " + player->getName() + "'s turn.");
 }

 //if there are 10+ coins, a coup is forced
 if(mustCoup(player)){
 throw runtime_error(player->getName() + " cannot perform 'bribe' with 10+ coins.");
 }
 

 const int bribeCost = 4;

 //makes sure player has enough money to bribe
 if(player->getCoins() < bribeCost){
 player->recordBlockedAction("bribe", "bank", game.getTurnCounter());
 throw runtime_error(player->getName() + " does not have enough coins to bribe.");
 }

 //update amount of coins
 player->removeCoins(bribeCost);

 //track player state
 player->setLastTurnPlayed(game.getTurnCounter());

 //records the action in the player's history
 player->addAction("bribe", "bank", game.getTurnCounter(), ActionResult::Succeeded);
 cout << "[Game] " << player->getName() << " paid 4 coins to bribe and gets an extra turn.\n";
 }

 void Actions::cancelBribe(Game& game, const shared_ptr<Player>& judge, const shared_ptr<Player>& target){
 
 //check that the player's role is Judge
 auto judgePtr = dynamic_pointer_cast<Judge>(judge->getRolePtr());
 if (!judgePtr) {
 throw runtime_error(judge->getName() + " is not a Judge and cannot cancel bribe.");
 }
 
 //check if there was a previous action
 if (target->getActionsHistory().empty()) {
 throw runtime_error("No actions to cancel.");
 }
 
 //get the last action
 const Action& last = target->getActionsHistory().back();
 
 //is this a successfully executed bribe operation
 if (last.type != "bribe" || last.result != ActionResult::Succeeded) {
 throw runtime_error("Last action is not a successful bribe, cannot cancel.");
 }
 
 //update status to Blocked
 target->updateLastActionResult(ActionResult::Blocked);
 
 cout << "[Role Effect] " << judge->getName() << " (Judge) canceled "
 << target->getName() << "'s bribe. 4 coins lost.\n";

 //advance the turn to the next player
 game.advanceTurn();
 }

 
 //4. ------------------------- arrest -------------------------
 void Actions::arrest(Game& game, const shared_ptr<Player>& player, const shared_ptr<Player>& target){

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 cout << "[Game] game has started!\n";
 } else {
 throw runtime_error("cannot perform arrest: not enough players.");
 }
 }
 
 //make sure it's the player's turn
 if(game.turn() != player->getName()){
 throw runtime_error("it's not " + player->getName() + "'s turn.");
 }

 //if there are 10+ coins, a coup is forced
 if(mustCoup(player)){
 throw runtime_error(player->getName() + " cannot perform 'arrest' with 10+ coins.");
 }
 
 
 //check if player is blocked from arrest (by Spy)
 if(player->isArrestDisabled()){
 player->recordBlockedAction("arrest", target->getName(), game.getTurnCounter());
 player->setArrestDisabled(false); // clear the block after attempted use
 throw runtime_error(player->getName() + " cannot use arrest this turn.");
 }
 
 //make sure target is alive
 if(!target->isAlive()){
 throw runtime_error("target player is not active.");
 }
 
 //make sure target has coins
 if(target->getCoins() == 0){
 player->recordBlockedAction("arrest", target->getName(), game.getTurnCounter());
 throw runtime_error(target->getName() + " has no coins to be arrested for.");
 }
 
 //prevent arresting the same target on consecutive turn
 const std::vector<Action>& history = player->getActionsHistory(); 
 
 if(!history.empty()){
 const Action& last = history.back(); //the last action the player performed
 if (last.type == "arrest" &&
 last.target == target->getName() &&
 last.turnNumber == player->getLastTurnPlayed()){
 player->recordBlockedAction("arrest", target->getName(), game.getTurnCounter());
 throw runtime_error("cannot arrest the same player in consecutive turns.");
 }
 }
 
 bool handled = false;
 if(target->getRolePtr()){
 handled = target->getRolePtr()->onArrested(*target, *player, game);
 }
 
 if(!handled){

 //take one coin from the target player
 target->removeCoins(1);
 
 //add the coin to the player who makes the arrest
 player->addCoins(1);
 
 cout << "[Game] " << player->getName() << " arrested " << target->getName() << " and took 1 coin.\n";
 }else{
 // action handled by role (e.g. General) – no coin gained
 cout << "[Game] " << player->getName() << " arrested " << target->getName() << " but received no coins.\n";
 }
 
 //update players last tirn
 player->setLastTurnPlayed(game.getTurnCounter());
 
 //record the action in the player's history
 player->addAction("arrest", target->getName(), game.getTurnCounter(), ActionResult::Succeeded);
 
 //advance to the next player
 game.advanceTurn();
 }
 
 void Actions::spyBlockArrest(Game& game, const std::shared_ptr<Player>& source, const std::shared_ptr<Player>& target){
 
 if(source->getRole() != Role::Spy){
 throw std::runtime_error(source->getName() + " is not a Spy.");
 }

 //update target state
 target->setArrestDisabled(true);

 source->addAction("spy-block-arrest", target->getName(), game.getTurnCounter(), ActionResult::Succeeded);
}
 
 //5. ------------------------- sanction -------------------------
 void Actions::sanction(Game& game, const shared_ptr<Player>& player, const shared_ptr<Player>& target){

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 cout << "[Game] game has started!\n";
 }else{
 throw runtime_error("cannot perform sanction: not enough players.");
 }
 }

 //make sure it's the player's turn
 if(game.turn() != player->getName()){
 throw runtime_error("it's not " + player->getName() + "'s turn.");
 }

 //if there are 10+ coins, a coup is forced
 if (mustCoup(player)) {
 throw runtime_error(player->getName() + " cannot perform 'sanction' with 10+ coins.");
 }
 
 int cost = 3;

 if(target->getRole() == Role::Judge){
 cost += 1;
 }

 //make sure the player has enough coins
 if(player->getCoins() < cost){
 player->recordBlockedAction("sanction", target->getName(), game.getTurnCounter());
 throw runtime_error(player->getName() + " does not have enough coins to sanction another player.");
 }

 //make sure the target is alive
 if(!target->isAlive()){
 player->recordBlockedAction("sanction", target->getName(), game.getTurnCounter());
 throw runtime_error("cannot sanction a dead player.");
 }

 //perform the sanction
 player->removeCoins(cost);
 target->setUnderSanction(true);

 if(target->getRolePtr()){
 target->getRolePtr()->onSanctioned(*target,*player,game);
 }


 //track player state
 player->setLastTurnPlayed(game.getTurnCounter());

 //record the action
 player->addAction("sanction", target->getName(), game.getTurnCounter(), ActionResult::Succeeded);
 cout << "[Game] " << player->getName() << " sanctioned " << target->getName() << " for one turn.\n";

 //advance to the next player only if the action was successful
 game.advanceTurn();
 }
 
 //6. ------------------------- coup -------------------------
 void Actions::coup(Game& game, const shared_ptr<Player>& player, const shared_ptr<Player>& target){

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 cout << "[Game] game has started!\n";
 }else{
 throw runtime_error("cannot perform coup: not enough players.");
 }
 }

 //make sure it's the player's turn
 if(game.turn() != player->getName()){
 throw runtime_error("it's not " + player->getName() + "'s turn.");
 }

 const int cost = 7;

 //check if player has enough coins
 if(player->getCoins() < cost){
 player->recordBlockedAction("coup", target->getName(), game.getTurnCounter());
 throw runtime_error(player->getName() + " does not have enough coins to perform a coup.");
 }

 //check if target is alive
 if(!target->isAlive()){
 player->recordBlockedAction("coup", target->getName(), game.getTurnCounter());
 throw runtime_error("target player is already eliminated.");
 }

 //perform the coup
 player->removeCoins(cost);
 target->eliminate();

 //track player state
 player->setLastTurnPlayed(game.getTurnCounter());

 //record the action
 player->addAction("coup", target->getName(), game.getTurnCounter(), ActionResult::Succeeded);
 
 ////
 game.setLastCoupInitiator(player);
 
 cout << "[Game] " << player->getName() << " has eliminated " << target->getName() << " with a coup!\n";

 //advance to the next player only if the action was successful
 game.advanceTurn();
 }

 //---------------------------other---------------------------

 //helper function - for Baron
 void Actions::invest(Game& game, const std::shared_ptr<Player>& player){

 const int cost = 3;
 const int gain = 6;

 //make sure the game has started
 if(!game.isGameStarted()){
 if(game.getPlayerCount() >= 2){
 game.startGame();
 std::cout << "[Game] game has started!\n";
 }else{
 throw runtime_error("cannot invest: not enough players to start the game.");
 }
 }

//  //make sure it's the player's turn
//  if(game.turn() != player->getName()){
//  throw runtime_error("it's not " + player->getName() + "'s turn.");
//  }

//make sure he is a baron
 if(!player->isAlive()){
 throw runtime_error(player->getName() + " is not alive.");
 }

 //make sure he is a baron
 if(player->getRole() != Role::Baron){
 throw runtime_error(player->getName() + " is not a Baron and cannot invest.");
 }

 //make sure he has enough coins
 if(player->getCoins() < cost){
 throw runtime_error(player->getName() + " does not have enough coins to invest.");
 }

 player->removeCoins(cost);
 player->addCoins(gain);

 std::cout << "[Role Effect] " << player->getName()
 << " (Baron) invested " << cost
 << " coins and gained " << gain << ".\n";

 //update history
 player->setLastTurnPlayed(game.getTurnCounter());
 player->addAction("invest", "bank", game.getTurnCounter(), ActionResult::Succeeded);

 //move to next active player
 game.advanceTurn();

}

 //helper function - for General
 void Actions::blockCoup(Game& game, const shared_ptr<Player>& general, const shared_ptr<Player>& coupInitiator, const shared_ptr<Player>& coupTarget){

 //make sure he is a general
 auto generalRole = dynamic_pointer_cast<General>(general->getRolePtr());
 if(!generalRole){
 throw runtime_error(general->getName() + " is not a General and cannot block coup.");
 }
 
 //make sure he has enough coins
 const int blockCost = 5;
 if(general->getCoins() < blockCost){
 throw runtime_error(general->getName() + " does not have enough coins to block the coup.");
 }
 
 //make sure that his last action was a successful coup on this target.
 const auto& history = coupInitiator->getActionsHistory();
 if(history.empty()){
 throw runtime_error(coupInitiator->getName() + " has no recent action to block.");
 
 }
 
 const Action& lastAction = history.back();
 if(lastAction.type != "coup" ||
 lastAction.result != ActionResult::Succeeded ||
 lastAction.target != coupTarget->getName()){
 throw runtime_error("No valid coup to block by " + coupInitiator->getName() + " on " + coupTarget->getName() + "."); 
 }
 
 //update ganeral coins
 general->removeCoins(blockCost);
 
 //update coup state
 coupInitiator->updateLastActionResult(ActionResult::Blocked);
 
 //make target alive
 if(!coupTarget->isAlive()){
 coupTarget->revive();
 std::cout << "[Effect] " << coupTarget->getName() << " has been revived after coup was blocked.\n";
 }
 
 std::cout << "[Role Effect] " << general->getName()
 << " (General) blocked the coup of " << coupInitiator->getName()
 << " on " << coupTarget->getName()
 << " by paying 5 coins. Coup was cancelled.\n";
 }

 //if a player has 10+ coins, a coup is forced
 bool mustCoup(const std::shared_ptr<Player>& player){

 if(player->getCoins() >= 10){
 std::cout << "[Rule] " << player->getName()
 << " has 10 or more coins and must perform a coup.\n";
 return true;
 }
 return false;
 }
 
 //helper function - for Spy
 int Actions::spySeeCoins(const Game& game, const std::shared_ptr<Player>& spy, const std::shared_ptr<Player>& target){
 if(spy->getRole() != Role::Spy){
 throw runtime_error(spy->getName() + " is not a Spy.");
 }
 return target->getCoins();
 }
 
}


