//yahelomessi@gmail.com

#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "Player.hpp"

namespace coup_game{

    class Game{

        private:

        std::vector<std::shared_ptr<Player>> playersList; //list of all players in the game
        int currentTurnIndex; //index of current player's turn
        int turnCounter; //counter of turns
        bool gameStarted;

        ////
        std::shared_ptr<Player> lastCoupInitiator = nullptr;

        public:

        Game(); 

        //adds a player to the game
        void addPlayer(const std::shared_ptr<Player>& player);
       
        //returns a list of names of all active players
        std::vector<std::string> players() const; 

        //returns the name of the player whose turn it is currently
        std::string turn() const; 
        
        //passes the turn to the next active player in the list
        void advanceTurn(); 

        //returns winner's name
        std::string winner() const; 

        //print all active players
        void printPlayers() const; 

        //returns a list of all players added to the game, whether they are alive or dead
        const std::vector<std::shared_ptr<Player>>& getAllPlayers() const; 

        //-------------helper functions-------------

        //check if the game has alreadt started
        bool isGameStarted() const;

        //marks the game as started
        void startGame();

        //return total number of players
        int getPlayerCount() const;

        //returns current turn number
        int getTurnCounter() const;

        //increments the global turn counter
        void incrementTurnCounter();

        //helper function to merchant
        void startTurn();

        //returns current player
        std::shared_ptr<Player> getCurrentPlayer() const;


        ////
        void setLastCoupInitiator(const std::shared_ptr<Player>& player);
        std::shared_ptr<Player> getLastCoupInitiator() const;
    };
}