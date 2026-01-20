
👤 Author
Yahel Omesi
📧 yahelomessi@gmail.com

# 🎮 Coup Game Engine (C++)

System Programming II course project implementing a turn-based strategy game inspired by *Coup*.

## 🚀 Overview
A modular C++ game engine supporting 2–6 players, each assigned a role with unique abilities.  
The project focuses on game state management, rule enforcement, and turn-based logic until a single winner remains.

A simple graphical interface was implemented to visualize gameplay and player states.

## 🧠 Core Features
- Turn-based game flow with strict rule validation  
- Multiple roles with unique behaviors and constraints  
- Dynamic player elimination and win-condition detection  
- Modular, object-oriented architecture  
- Basic GUI for game visualization  

## 🎮 Game Mechanics
Players can perform actions such as gathering resources, taxing, bribing, sanctioning, arresting, and executing coups.  
Special roles (Governor, Spy, Baron, General, Judge, Merchant) introduce additional mechanics that affect gameplay dynamics.

## 🖥 GUI
- Displays current player turn and role  
- Shows all players and their status (eliminated players are marked)  
- Announces the winner at the end of the game  

## 🛠 Technologies
- C++
- Object-Oriented Programming (OOP)
- GUI integration (SFML)
- Makefile-based build system
- Unit testing with doctest

## ▶️ Build & Run
```bash
make main


🧪 Run Tests
make test

📁 File Structure
main.cpp – Program entry point
Game.* – Game state and turn management
Player.* – Player representation
RoleBase.* – Abstract base class for roles
Spy / Judge / General / Governor / Merchant / Baron – Role implementations
Actions.* – Game action logic
GameWindow.cpp – GUI implementation
test.cpp – Unit tests (doctest)
Makefile – Build automation
