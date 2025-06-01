 //yahelomessi@gmail.com
 
 #pragma once
 #include "Game.hpp"
 #include "Player.hpp"
 #include <memory>

 namespace coup_game{
    class Actions{
        public:

        //1. ------------------------- gather -------------------------
        
        //add 1 coin to the player 
        static void gather(Game& game, const std::shared_ptr<Player>& player);

        //2. ------------------------- tax -------------------------
        
        
        //adds 2 coins (or more depending role) to the player
        static void tax(Game& game, const std::shared_ptr<Player>& player);
        
        //cancels the effect of tax action performed by another player
        static void cancelTax(Game& game, const std::shared_ptr<Player>& governor, const std::shared_ptr<Player>& target);

        //3. ------------------------- bribe -------------------------
        
        //costs 4 coins, give the player an extra turn
        static void bribe(Game& game, const std::shared_ptr<Player>& player);

        //cancels the bribe action performed by another player
        static void cancelBribe(Game& game, const std::shared_ptr<Player>& judge, const std::shared_ptr<Player>& target);
        
        //4. ------------------------- arrest -------------------------
        
        //steals 1 coin from the target(if possible)
        static void arrest(Game& game, const std::shared_ptr<Player>& player, const std::shared_ptr<Player>& target);
        
        //spy actions:
        static void spyReveal(Game& game, const std::shared_ptr<Player>& source, const std::shared_ptr<Player>& target);

        //prevents a specific player from performing an arres on their next turn
        static void spyBlockArrest(Game& game, const std::shared_ptr<Player>& source, const std::shared_ptr<Player>& target);


        //5. ------------------------- sanction -------------------------
        
        //disables the target player from tax&gather on their next turn
        static void sanction(Game& game, const std::shared_ptr<Player>& player, const std::shared_ptr<Player>& target);         

        //6. ------------------------- coup -------------------------
        
        //costs 7 coins. eliminate another player from the game
        static void coup(Game& game, const std::shared_ptr<Player>& player, const std::shared_ptr<Player>& target);


        //---------------------------other---------------------------

        //helper function - the Baron can exchange 3 coins for 6
        static void invest(Game& game, const std::shared_ptr<Player>& player);
        
        //helper function - the General can block a coup by paing 5 coins
        static void blockCoup(Game& game,
            const std::shared_ptr<Player>& general,
            const std::shared_ptr<Player>& coupInitiator,
            const std::shared_ptr<Player>& coupTarget); 
        
        
        //helper function - spy can see hoe many coins another player has.
        static int spySeeCoins(const Game& game, const std::shared_ptr<Player>& spy, const std::shared_ptr<Player>& target);
        
        };

        //if there are 10+ coins, a coup is forced
        bool mustCoup(const std::shared_ptr<Player>& player);

     
}