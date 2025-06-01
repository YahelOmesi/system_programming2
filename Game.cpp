//yahelomessi@gmail.com

#include "Game.hpp"

using namespace std;

namespace coup_game{

    //constructor
    Game::Game():currentTurnIndex(0), turnCounter(1), gameStarted(false){ //constructor
        cout << "[Game] New game initialized.\n";
    }

    void Game::addPlayer(const shared_ptr<Player>& player){

        //prevent adding new players after game start
        if(gameStarted){
            throw runtime_error("cannot add players after the game has started");
        }

        //prevent duplicate player name
        for (const auto& existing : playersList){
            if (existing->getName() == player->getName()){
                throw runtime_error("duplicate player name: " + player->getName());
            }
        }    

        //enforce player limit
        if(playersList.size() >= 6){
            throw runtime_error("maximum number of players is 6");
        }
        playersList.push_back(player); //add player to list
        cout << "[Game] Added player: " << player->getName() << "\n";
    }

    //return list of all active players
    vector<string> Game::players() const{

        //to store only active players
        vector<string> activePlayers;

        for(const shared_ptr<Player>& p : playersList){ //loop over all players
            if(p->isAlive()){
                activePlayers.push_back(p->getName());
            }
        }
        return activePlayers;
    }

    //returns the name of the player whose turn it is currently
    string Game::turn() const{

        if(playersList.size() < 2){
            throw runtime_error("not enough players to start the game.");
        }

        if(playersList.empty()){
            throw runtime_error("no players in the game.");
        }

        int index = currentTurnIndex % playersList.size();
        if(!playersList[index]->isAlive()){
            throw runtime_error("current player is eliminated, use advanceTurn() first.");
        }

        return playersList[index]->getName();

    }

    //passes the turn to the next active player in the list
    void Game::advanceTurn(){

        //if the game has not started yet, perform initialization
        if(!gameStarted){

            //the game cannot start unless there are at least 2 players
            if(playersList.size() < 2){
                throw runtime_error("cannot start game with less than 2 players.");
            }
    
            gameStarted = true; //mark the game as started
            turnCounter++; 
            std::cout << "[Game] Game has started!\n";
    
            auto& currentPlayer = playersList[currentTurnIndex];
    
            ////
            startTurn();

            //apply any start of turn role effects
            if(currentPlayer->getRolePtr()){
                currentPlayer->getRolePtr()->onTurnStart(*currentPlayer, *this);
            }
            
            std::cout << "[Game] Next turn: " << currentPlayer->getName() << "\n";
            return;
        }

        //the game is over
        if(players().size() == 1) {
            return;
        }
    
        //advance the turn counter
        turnCounter++;
    
        //find the next player who is still alive
        int totalPlayers = playersList.size();
        for(int i = 1; i < totalPlayers; ++i){ //start from 1 to skip current player
            int nextIndex = (currentTurnIndex + i) % totalPlayers;
    
            //if the player is still alive, its their turn now
            if(playersList[nextIndex]->isAlive()){
                currentTurnIndex = nextIndex;

                startTurn();
        
                //playersList[nextIndex]->decreaseSanction();
                std::cout << "[Game] Next turn: " << playersList[nextIndex]->getName() << "\n";
                return;
            }
        }
        throw runtime_error("no active players remaining.");
    }
    
    //returns winner's name
    string Game::winner() const{ 

        //check if the game has startd and has at least 2 players
        if(!gameStarted || playersList.size() < 2){
            throw runtime_error("Game has not started or too few players to declare a winner.");
        }
        
        //get a list of all currently alive players
        vector<string> active = this->players();

        if(active.size() == 1){
            cout << "[Game] Winner is: " << active[0] << "\n"; //print winner's name
            return active[0];
        }else{
            //if more than 1 player is still active the game is still ongoing
            throw runtime_error("The game is still ongoing. no winner yet.");
        }
    }

    //print all active players
    void Game::printPlayers() const{ 
        cout << "[Game] active players:\n";
        for(const shared_ptr<Player>& p : playersList){
            if(p->isAlive()){
                cout << "  - " << p->getName() << "\n";
            }
        }
    }

    //returns a list of all players added to the game, whether they are alive or dead
    const std::vector<std::shared_ptr<Player>>& Game::getAllPlayers() const{
        return playersList;
    }

    //helper function for Actions classs
    bool Game::isGameStarted() const{ return gameStarted;}
    void Game::startGame(){gameStarted = true;}
    int Game::getPlayerCount() const{return playersList.size();}
    int Game::getTurnCounter() const{return turnCounter;}
    void Game::incrementTurnCounter(){turnCounter++;}
 
    void Game::startTurn() {
    auto& player = playersList[currentTurnIndex];

    if (player->isUnderSanction() && player->isSanctionJustApplied()) {
        player->clearSanctionJustApplied(); 
    }
    else if (player->isUnderSanction()) {
        player->setUnderSanction(false); 
        std::cout << "[Effect] " << player->getName() << "'s sanction has ended.\n";
    }

    if (player->getRolePtr()) {
        player->getRolePtr()->onTurnStart(*player, *this);
    }
}


    //returns current player
    std::shared_ptr<Player> Game::getCurrentPlayer() const{
    if(playersList.empty()){
        throw std::runtime_error("No players in game");
    }

    int totalPlayers = playersList.size();
    int index = currentTurnIndex;

    for(int i = 0; i < totalPlayers; ++i){
        auto& player = playersList.at(index);
        if(player->isAlive()){
            return player;
        }
        index = (index + 1) % totalPlayers; //move to next
    }

    throw std::runtime_error("No alive players found"); 
}

    ////
    void Game::setLastCoupInitiator(const std::shared_ptr<Player>& player) {
        lastCoupInitiator = player;
    }

    std::shared_ptr<Player> Game::getLastCoupInitiator() const {
        return lastCoupInitiator;
    }

}

