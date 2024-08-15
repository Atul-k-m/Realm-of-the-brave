#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <exception>
#include <fstream>


class Character {
protected:
    std::string name;
    int health;
    int attackPower;
    std::vector<std::string> inventory;

public:
    Character(const std::string& n, int h, int a) : name(n), health(h), attackPower(a) {}

    virtual ~Character() {}

    virtual void attack(Character& target) = 0; 

    void showInventory() const {
        std::cout << "\n============================\n";
        std::cout << name << "'s Inventory:\n";
        std::cout << "============================\n";
        if (inventory.empty()) {
            std::cout << "|  Nothing.\n";
        } else {
            for (const auto& item : inventory) {
                std::cout << "|  - " << item << "\n";
            }
        }
        std::cout << "============================\n";
    }

    void addItem(const std::string& item) {
        inventory.push_back(item);
        std::cout << "\n*** " << item << " added to your inventory. ***\n";
    }

    void removeItem(const std::string& item) {
        auto it = std::find(inventory.begin(), inventory.end(), item);
        if (it != inventory.end()) {
            inventory.erase(it);
            std::cout << "*** " << item << " removed from your inventory. ***\n";
        } else {
            std::cout << "*** " << item << " is not in your inventory. ***\n";
        }
    }

    void useHealthPotion() {
        if (std::find(inventory.begin(), inventory.end(), "Health Potion") != inventory.end()) {
            health += 20;
            std::cout << "*** You use a health potion. Your health is now " << health << ". ***\n";
            removeItem("Health Potion");
        } else {
            std::cout << "*** You don't have a health potion in your inventory. ***\n";
        }
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttackPower() const { return attackPower; }

    void takeDamage(int damage) { health -= damage; }

    bool isAlive() const { return health > 0; }
};

// Player 
class Player : public Character {
public:
    Player(const std::string& n, int h, int a) : Character(n, h, a) {}

    void attack(Character& target) override {
        int damage = rand() % attackPower + 1;
        target.takeDamage(damage);
        std::cout << "*** " << name << " attacks " << target.getName() << " for " << damage << " damage! ***\n";
    }

    void useSpecialAbility(Character& target) {
        int furyDamage = rand() % attackPower + 10;
        target.takeDamage(furyDamage);
        std::cout << "*** You unleash your special ability: Fury Strike! ***\n";
        std::cout << "*** You deal " << furyDamage << " damage to " << target.getName() << "! ***\n";
    }
};

// Enemy
class Enemy : public Character {
public:
    Enemy(const std::string& n, int h, int a) : Character(n, h, a) {}

    void attack(Character& target) override {
        int damage = rand() % attackPower + 1;
        target.takeDamage(damage);
        std::cout << "*** " << name << " attacks " << target.getName() << " for " << damage << " damage! ***\n";
    }
};


class GameEvent {
public:
    virtual ~GameEvent() {}
    virtual void triggerEvent(Player& player) = 0;
};

// explore
class ExplorationEvent : public GameEvent {
public:
    void triggerEvent(Player& player) override {
        std::cout << "\n*** You explore the surroundings... ***\n";
        std::cout << "*** You find a mysterious cave. Do you want to enter? ***\n";
        std::cout << "1. Yes\n2. No\n";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "\n*** You enter the cave and find a treasure chest! ***\n";
            player.addItem("Treasure Chest");
        } else {
            std::cout << "\n*** You decide to stay outside. A storm begins, and you take shelter in a nearby hut. ***\n";
        }

        std::cout << "\n*** You find a healing potion while exploring. Do you want to take it? ***\n";
        std::cout << "1. Yes\n2. No\n";
        std::cin >> choice;

        if (choice == 1) {
            player.addItem("Health Potion");
        } else {
            std::cout << "\n*** You leave the potion behind. ***\n";
        }
    }
};

// battle 
class BattleEvent : public GameEvent {
private:
    std::unique_ptr<Enemy> enemy;

public:
    BattleEvent() {
        enemy = std::make_unique<Enemy>("Wild Beast", rand() % 50 + 25, rand() % 5 + 3);
    }

    void triggerEvent(Player& player) override {
        std::cout << "\n====================================\n";
        std::cout << "*** A wild enemy appears! ***\n";
        std::cout << "Enemy: " << enemy->getName() << " | Health: " << enemy->getHealth() << "\n";
        std::cout << "====================================\n";

        while (player.isAlive() && enemy->isAlive()) {
            std::cout << "\nChoose your action:\n1. Attack\n2. Use Health Potion\n3. Special Ability\n4. Flee\n";
            int action;
            std::cin >> action;

            if (action == 1) {
                player.attack(*enemy);
            } else if (action == 2) {
                player.useHealthPotion();
            } else if (action == 3) {
                player.useSpecialAbility(*enemy);
            } else if (action == 4) {
                std::cout << "\n*** You flee from the battle! ***\n";
                return;
            } else {
                std::cout << "\n*** Invalid action. Try again. ***\n";
                continue;
            }

            if (enemy->isAlive()) {
                enemy->attack(player);
            }
        }

        if (!player.isAlive()) {
            std::cout << "\n*** You have been defeated! ***\n";
        } else if (!enemy->isAlive()) {
            std::cout << "\n*** You defeated the enemy! ***\n";
            player.addItem("Enemy Loot");
        }
    }
};

// Utility func save 
template <typename T>
void saveGame(const T& player) {
    std::ofstream file("savegame.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open save file!");
    }

    file << player.getName() << "\n";
    file << player.getHealth() << "\n";
    file << player.getAttackPower() << "\n";
    file.close();
}

// Utility func load
template <typename T>
T loadGame() {
    std::ifstream file("savegame.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open save file!");
    }

    std::string name;
    int health, attackPower;
    std::getline(file, name);
    file >> health >> attackPower;
    file.close();

    return T(name, health, attackPower);
}

void showAsciiArt(const std::string &title);

void startGame(Player &player);
void displayMenu(Player &player);
void handleEvent(GameEvent& event, Player& player);

int main() {
    srand(static_cast<unsigned int>(time(0))); 

    Player player("Hero", 100, 10);

    try {
        startGame(player);
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << '\n';
    }

    return 0;
}

void startGame(Player &player) {
    showAsciiArt("ADVENTURE GAME");
    std::cout << "Welcome to the Adventure Game, " << player.getName() << "!\n";
    displayMenu(player);
}

void displayMenu(Player &player) {
    int choice;
    do {
        std::cout << "\n====================================\n";
        std::cout << "1. Explore\n2. Fight\n3. Inventory\n4. Save Game\n5. Load Game\n6. Exit\n";
        std::cout << "====================================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        try {
            switch(choice) {
                case 1: {
                    ExplorationEvent exploration;
                    handleEvent(exploration, player);
                    break;
                }
                case 2: {
                    BattleEvent battle;
                    handleEvent(battle, player);
                    break;
                }
                case 3:
                    player.showInventory();
                    break;
                case 4:
                    saveGame(player);
                    std::cout << "*** Game saved successfully! ***\n";
                    break;
                case 5:
                    player = loadGame<Player>();
                    std::cout << "*** Game loaded successfully! ***\n";
                    break;
                case 6:
                    std::cout << "Exiting the game...\n";
                    break;
                default:
                    std::cout << "*** Invalid choice. Try again. ***\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << '\n';
        }
    } while(choice != 6);
}

void handleEvent(GameEvent& event, Player& player) {
    event.triggerEvent(player);
}

void showAsciiArt(const std::string &title) {
    std::cout << "\n===============================\n";
    std::cout << "|                             |\n";
    std::cout << "|   " << std::setw(23) << title << "   |\n";
    std::cout << "|                             |\n";
    std::cout << "===============================\n";
    std::cout << "          __      __           \n";
    std::cout << "         /  \\    /  \\          \n";
    std::cout << "        /    \\  /    \\         \n";
    std::cout << "       /      \\/      \\        \n";
    std::cout << "      /               \\       \n";
    std::cout << "     /    ________     \\      \n";
    std::cout << "    /   /          \\    \\     \n";
    std::cout << "   /   /            \\    \\    \n";
    std::cout << "  /   /              \\    \\   \n";
    std::cout << " /   /                \\    \\  \n";
    std::cout << "===============================\n";
}
