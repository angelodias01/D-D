#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_PLAYER_NAME 20
#define MAX_PLAYER_ENERGY 100
#define MAX_MONSTER_ENERGY 80
#define INIT_PLAYER_CELL 0
#define INIT_MONSTER_CELL 5
#define MAX_CELL_DESCRIPTION 200
#define MAX_MAP_CELLS 12 
#define MAX_PLAYER_HEALTH 150
#define MAX_MONSTER_HEALTH 200
#define MAX_MONSTER_LEVEL 5
#define MAX_OBJECT_NAME 50

struct Player {
    char name[MAX_PLAYER_NAME];
    int energy;
    int health;
    int damage;
    int cell;
    int object;
    int treasure;
};

struct Monster {
    int cell;
    int health;
    int mEnergy;
    int level;
    int damage;
};

struct Cell {
    int north;
    int south;
    int west;
    int east;
    int up;
    int down;
    char description[MAX_CELL_DESCRIPTION];
    int object;
    int treasure;
    char treasureName[MAX_OBJECT_NAME];
};

struct ThreadData {
    struct Player *pPlayer;
    struct Cell *map;
    struct Monster *pMonster;
    pthread_mutex_t *mutex;
    int *quitFlag;
    int *gameOver;
};

void StartGame(struct Player *pPlayer, struct Cell *map, struct Monster *pMonster, int *gameOver, pthread_mutex_t *mutex);
void InitMonster(struct Monster *pMonster);
void Fight(struct Player *pPlayer, struct Monster *pMonster, struct Cell *map, int *gameOver, pthread_mutex_t *mutex);
void MovePlayer(struct Player *pPlayer, struct Cell *map, int direction, struct Monster *pMonster);
void InitPlayer(struct Player *pPlayer);
void ShowPlayer(struct Player player);
void ShowMonster(struct Monster monster);
void *MonsterThread(void *data);
void *PlayerThread(void *data);
int IsCriticalHit();
void MoveMonster(struct Monster *pMonster, struct Cell *map, int direction);

void *PlayerThread(void *data) {
    struct ThreadData *threadData = (struct ThreadData *)data;

    while (1) {
        pthread_mutex_lock(threadData->mutex);
        if (*(threadData->quitFlag) || *(threadData->gameOver)) {
            pthread_mutex_unlock(threadData->mutex);
            break;
        }
        pthread_mutex_unlock(threadData->mutex);

        StartGame(threadData->pPlayer, threadData->map, threadData->pMonster, threadData->gameOver, threadData->mutex);
    }

    return NULL;
}

void InitPlayer(struct Player *pPlayer) {
    printf("Hello Player! What is your name? ");
    scanf("%s", pPlayer->name);
    pPlayer->energy = MAX_PLAYER_ENERGY;
    pPlayer->health = MAX_PLAYER_HEALTH;
    pPlayer->cell = INIT_PLAYER_CELL;
    pPlayer->damage = rand() % (25 + 10 + 1) + 10;
    pPlayer->object = -1;
    pPlayer->treasure = -1;
}

void InitMonster(struct Monster *pMonster) {
    do {
        pMonster->cell = rand() % MAX_MAP_CELLS;
    } while (pMonster->cell == 0);
    
    pMonster->mEnergy = rand() % (80 - 10 + 1) + 10;
    pMonster->health = MAX_MONSTER_HEALTH;
    pMonster->level = rand() % (MAX_MONSTER_LEVEL + 1) + 1;
    pMonster->damage = rand() % (30 + 8 + 1) + 8;
}

int IsCriticalHit() {
    int chance = rand() % 100 + 1;
    return (chance <= 5);
}

void MoveMonster(struct Monster *pMonster, struct Cell *map, int direction)
{
    int newCellIndex;

    switch (direction)
    {
    case 1:
        newCellIndex = pMonster->cell + map[pMonster->cell].north;
        break;
    case 2:
        newCellIndex = pMonster->cell + map[pMonster->cell].south;
        break;
    case 3:
        newCellIndex = pMonster->cell + map[pMonster->cell].west;
        break;
    case 4:
        newCellIndex = pMonster->cell + map[pMonster->cell].east;
        break;
    case 5:
        newCellIndex = pMonster->cell + map[pMonster->cell].up;
        break;
    case 6:
        newCellIndex = pMonster->cell + map[pMonster->cell].down;
        break;
    default:
        return; 
    }

    if (newCellIndex >= 0 && newCellIndex < MAX_MAP_CELLS)
    {
        pMonster->cell = newCellIndex;
    }
}

void *MonsterThread(void *data) {
    struct ThreadData *threadData = (struct ThreadData *)data;

    while (1) {
        pthread_mutex_lock(threadData->mutex);
        if (*(threadData->quitFlag) || *(threadData->gameOver)) {
            pthread_mutex_unlock(threadData->mutex);
            break;
        }
        pthread_mutex_unlock(threadData->mutex);

        int direction = rand() % 6 + 1;
        MoveMonster(threadData->pMonster, threadData->map, direction);
        usleep(rand() % 901 + 100);
    }

    pthread_exit(NULL);
}

void ShowPlayer(struct Player player) {
    printf("========================================\n");
    printf("🔥 Good luck %s! Let's play.\n", player.name);
    printf("🔋 Energy: %d\n", player.energy);
    printf("🩷 Health: %d\n", player.health);
    printf("🏆 Location: %d\n", player.cell);
    printf("📦 Object: %d\n", player.object);
    printf("💎 Treasure: %d\n", player.treasure);
    printf("========================================\n");
}

void ShowMonster(struct Monster monster) {
    printf("========================================\n");
    printf("👹 Monster Location: %d\n", monster.cell);
    printf("========================================\n");
}

void Fight(struct Player *pPlayer, struct Monster *pMonster, struct Cell *map, int *gameOver, pthread_mutex_t *mutex) {
    printf("You encountered a level %d monster!\n", pMonster->level);

    while (pPlayer->health > 0 && pMonster->health > 0) {
        int playerDamage = pPlayer->damage;
        if (IsCriticalHit()) {
            playerDamage *= 2;
            printf("Player's critical hit! ");
        }
        pMonster->health -= playerDamage;
        printf("You dealt %d damage to the monster. Monster's health: %d\n", playerDamage, pMonster->health);
        usleep(500000);

        if (pMonster->health <= 0) {
            printf("You defeated the monster! You are victorious!\n");
            InitMonster(pMonster);
            return;
        }

        int monsterDamage = pMonster->damage;
        if (IsCriticalHit()) {
            monsterDamage *= 2;
            printf("Monster's critical hit! ");
        }
        pPlayer->health -= monsterDamage;
        printf("The monster dealt %d damage to you. Your health: %d\n", monsterDamage, pPlayer->health);
        usleep(500000);
    }

    if (pPlayer->health <= 0) {
        printf("You were defeated by the monster! Game over.\n");
        *gameOver = 1;
    }
}

void StartGame(struct Player *pPlayer, struct Cell *map, struct Monster *pMonster, int *gameOver, pthread_mutex_t *mutex) {
    int choice;

    do {
        pthread_mutex_lock(mutex);
        if (*gameOver) {
            printf("========================================\n");
            printf("💔 The game has ended. Thank you for playing!\n");
            pthread_mutex_unlock(mutex);
            return;
        }
        pthread_mutex_unlock(mutex);

        printf("\n========================================\n");
        printf("You are at cell %d. Choose a direction to move:\n", pPlayer->cell);
        printf("Available directions: ");

        int hasValidDirection = 0;

        if (map[pPlayer->cell].north >= 0) {
            printf("1: North ");
            hasValidDirection = 1;
        }
        if (map[pPlayer->cell].south >= 0) {
            printf("2: South ");
            hasValidDirection = 1;
        }
        if (map[pPlayer->cell].west >= 0) {
            printf("3: West ");
            hasValidDirection = 1;
        }
        if (map[pPlayer->cell].east >= 0) {
            printf("4: East ");
            hasValidDirection = 1;
        }
        if (map[pPlayer->cell].up >= 0) {
            printf("5: Up ");
            hasValidDirection = 1;
        }
        if (map[pPlayer->cell].down >= 0) {
            printf("6: Down ");
            hasValidDirection = 1;
        }

        if (!hasValidDirection) {
            printf("No valid directions available.\n");
            printf("You cannot move from here.\n");
            return;
        }

        printf("\nChoose a direction to move (1: North, 2: South, 3: West, 4: East, 5: Up, 6: Down, 0: Exit): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                if ((choice == 1 && map[pPlayer->cell].north >= 0) ||
                    (choice == 2 && map[pPlayer->cell].south >= 0) ||
                    (choice == 3 && map[pPlayer->cell].west >= 0) ||
                    (choice == 4 && map[pPlayer->cell].east >= 0) ||
                    (choice == 5 && map[pPlayer->cell].up >= 0) ||
                    (choice == 6 && map[pPlayer->cell].down >= 0)) {
                    MovePlayer(pPlayer, map, choice, pMonster);
                    if (pPlayer->cell == pMonster->cell) {
                        Fight(pPlayer, pMonster, map, gameOver, mutex);
                    }
                } else {
                    printf("Invalid direction for this cell. Try again.\n");
                }
                break;
            case 0:
                printf("Exiting the game.\n");
                pthread_mutex_lock(mutex);
                *gameOver = 1;
                pthread_mutex_unlock(mutex);
                return;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 0 && !(*gameOver));
}

void MovePlayer(struct Player *pPlayer, struct Cell *map, int direction, struct Monster *pMonster) {
    int newCellIndex = pPlayer->cell;

    switch (direction) {
        case 1:
            newCellIndex = map[pPlayer->cell].north;
            break;
        case 2:
            newCellIndex = map[pPlayer->cell].south;
            break;
        case 3:
            newCellIndex = map[pPlayer->cell].west;
            break;
        case 4:
            newCellIndex = map[pPlayer->cell].east;
            break;
        case 5:
            newCellIndex = map[pPlayer->cell].up;
            break;
        case 6:
            newCellIndex = map[pPlayer->cell].down;
            break;
        default:
            printf("Invalid direction.\n");
            return;
    }

    if (newCellIndex >= 0 && newCellIndex < MAX_MAP_CELLS) {
        pPlayer->cell = newCellIndex;

        if (pPlayer->cell == pMonster->cell) {
            Fight(pPlayer, pMonster, map, NULL, NULL);
        }

        if (map[pPlayer->cell].treasure > 0) {
            printf("💎 Treasure found: %s!!\n", map[pPlayer->cell].treasureName);
            pPlayer->treasure = map[pPlayer->cell].treasure;
        
            int choice;
            printf("Do you want to continue exploring? (1 to continue, 0 to leave): ");
            scanf("%d", &choice);
        
            if (choice == 0) {
                printf("You decide to leave with the treasure.\n");
                exit(0);
            } else if (choice == 1) {
                printf("You continue exploring...\n");
            } else {
                printf("Invalid input. Continuing exploration by default.\n");
            }
        }

        if (map[pPlayer->cell].object > 0) {
            printf("📦 You found an item! ");
            switch (map[pPlayer->cell].object) {
                case 1:
                    pPlayer->health = MAX_PLAYER_HEALTH;
                    printf("You fully restored your health!\n");
                    break;
                case 2:
                    pPlayer->damage += 10;
                    printf("Your damage increased by 10!\n");
                    break;
                case 3:
                    printf("You now have a higher chance of critical hits!\n");
                    break;
                case 4:
                    pPlayer->energy = MAX_PLAYER_ENERGY;
                    printf("You restored all your energy!\n");
                    break;
                default:
                    printf("Unknown item!\n");
                    break;
            }
        }

        ShowPlayer(*pPlayer);
        ShowMonster(*pMonster);
        printf("Description: %s\n", map[pPlayer->cell].description);
    } else {
        printf("You cannot move in that direction.\n");
    }
}

int main() {
    struct Player player;
    struct Cell map[MAX_MAP_CELLS];
    struct Monster monster;

    int quitFlag = 0;
    int gameOver = 0;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    InitPlayer(&player);
    InitMonster(&monster);

    map[0] = (struct Cell){-1, 1, 4, -1, -1, -1, "You are at the castle entrance.", -1, -1, ""};
    map[1] = (struct Cell){0, -1, 3, 4, -1, -1, "You are in a corridor.", 2, -1, "Gold"};
    map[2] = (struct Cell){1, -1, 5, -1, -1, -1, "You enter a large hall.", -1, 3, "Diamonds"};
    map[3] = (struct Cell){-1, 1, 2, 5, -1, -1, "A small room with murals.", -1, -1, "Copper"};
    map[4] = (struct Cell){0, 5, 1, 3, -1, -1, "Narrow passage.", 1, -1, ""};
    map[5] = (struct Cell){4, -1, 6, 3, -1, -1, "Large cave with high ceiling.", -1, 4, ""};
    map[6] = (struct Cell){5, -1, 7, -1, 8, -1, "A hidden chamber.", -1, -1, "Ruby"};
    map[7] = (struct Cell){6, -1, -1, 8, -1, -1, "Secret entrance to another area.", -1, 2, "Emerald"};
    map[8] = (struct Cell){7, 9, 6, -1, -1, -1, "Stairs leading down.", -1, -1, "Obsidian"};
    map[9] = (struct Cell){8, 10, 5, -1, -1, -1, "Underground tunnel.", 1, -1, ""};
    map[10] = (struct Cell){9, -1, 11, -1, 9, -1, "Dark, damp passage.", -1, -1, "Diamond"};
    map[11] = (struct Cell){-1, -1, 10, 5, -1, -1, "End of the tunnel.", 3, -1, ""};

    pthread_t playerThread, monsterThread;
    struct ThreadData threadData = {&player, map, &monster, &mutex, &quitFlag, &gameOver};

    pthread_create(&playerThread, NULL, PlayerThread, (void *)&threadData);
    pthread_create(&monsterThread, NULL, MonsterThread, (void *)&threadData);

    pthread_join(playerThread, NULL);
    pthread_join(monsterThread, NULL);

    return 0;
}
