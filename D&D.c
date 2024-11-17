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
#define MAX_MAP_CELLS 6
#define MAX_PLAYER_HEALTH 150
#define MAX_MONSTER_HEALTH 200
#define MAX_MONSTER_LEVEL 5


struct Player
{
	char name[MAX_PLAYER_NAME];
	int energy;
    int health;
    int damage;
	int cell;
	int object;
	int treasure;
};

struct Monster
{
    int cell;
    int health;
    int mEnergy;
    int level;
    int damage;
};

struct Cell
{
	int north;
	int south;
	int west;
	int east;
	int up;
	int down;
	char description[MAX_CELL_DESCRIPTION];
	int object;
	int treasure;
};

struct ThreadData
{
    struct Player *pPlayer;
    struct Cell *map;
    struct Monster *pMonster;

    int *quitFlag; 
};

void StartGame(struct Player *pPlayer, struct Cell *map, struct Monster *pMonster);
void InitMonster(struct Monster *pMonster);
void Fight(struct Player *pPlayer, struct Monster *pMonster, struct Cell *map);

void *PlayerThread(void *data)
{
    struct ThreadData *threadData = (struct ThreadData *)data;

    while (!*(threadData->quitFlag))
    { 
        StartGame(threadData->pPlayer, threadData->map, threadData->pMonster);
    }

    return NULL;
}

void InitPlayer(struct Player *pPlayer)
{
	printf("Olá aventureiro! Qual o teu nome? ");
	scanf("%s", pPlayer->name);
	pPlayer->energy = MAX_PLAYER_ENERGY;
    pPlayer->health = 150;
	pPlayer->cell = INIT_PLAYER_CELL;
    pPlayer->damage = rand() % (25 + 10 + 1) + 10;
	pPlayer->object = -1;
	pPlayer->treasure = -1;
}

void InitMonster(struct Monster *pMonster)
{
    
    while (pMonster->cell == 0 )
    {
        pMonster->cell = rand() % MAX_MAP_CELLS;
    }
        pMonster->mEnergy = rand() % (80 - 10 + 1) + 10;
        pMonster->health = MAX_MONSTER_HEALTH;
        pMonster->level = rand() % (5+ 1 +1 ) + 1;
        pMonster->damage = rand() % (30+ 8 + 1) + 8;
}

int IsCriticalHit()
{
    int chance = rand() % 100 + 1;
    
    if (chance <= 5)
    {
        return 1;
    }
    
    return 0;
}


void MoveMonster(struct Monster *pMonster, struct Cell *map, int direction)
{
    int newCellIndex;

    switch (direction)
    {
    case 1: // North
        newCellIndex = pMonster->cell + map[pMonster->cell].north;
        break;
    case 2: // South
        newCellIndex = pMonster->cell + map[pMonster->cell].south;
        break;
    case 3: // West
        newCellIndex = pMonster->cell + map[pMonster->cell].west;
        break;
    case 4: // East
        newCellIndex = pMonster->cell + map[pMonster->cell].east;
        break;
    default:
        return; 
    }


    if (newCellIndex >= 0 && newCellIndex < MAX_MAP_CELLS)
    {
 
        pMonster->cell = newCellIndex;
        
    }
}

void *MonsterThread(void *data)
{
    struct ThreadData *threadData = (struct ThreadData *)data;

    while (!(*threadData->quitFlag))
    {
    
        int direction = rand() % 4 + 1;

 
        MoveMonster(threadData->pMonster, threadData->map, direction);

        usleep(rand() % 901 + 100); 
    }
    pthread_exit(NULL);
}

void ShowPlayer(struct Player player)
{
	printf("Boa sorte %s! Vamos jogar.\n", player.name);
	printf("Energia: %d\n", player.energy);
    printf("Health: %d\n", player.health);
	printf("Local: %d\n", player.cell);
	printf("Objecto: %d\n", player.object);
	printf("Tesouro: %d\n", player.treasure);
}

void ShowMonster(struct Monster monster)
{
	printf("Local: %d\n", monster.cell);
}

void MovePlayer(struct Player *pPlayer, struct Cell *map, int direction, struct Monster *pMonster);
void StartGame(struct Player *pPlayer, struct Cell *map, struct Monster *pMonster);

int main()
{
    struct Player player;
    struct Cell map[MAX_MAP_CELLS];
    struct Monster monster;
    int nCells;
    int quitFlag = 0; 

    InitPlayer(&player);
    InitMonster(&monster);
	nCells = 6;
 
    // Init map
	// Cell 0
	map[0].north = -1;
	map[0].south = -1;
	map[0].west = -1;
	map[0].east = 2;
	map[0].up = -1;
	map[0].down = -1;
	strcpy(map[0].description, "Encontra-se na entrada do castelo. Está escuro e ouvem-se ruídos estranhos!");
	map[0].object = -1;
	map[0].treasure = -1;

	// Cell 1

	map[1].north = -1;
	map[1].south = 0;
	map[1].west = -1;
	map[1].east = 3;
	map[1].up = -1;
	map[1].down = -1;
	strcpy(map[1].description, "You are in a dimly lit corridor. Faint sounds of dripping water can be heard.");
	map[1].object = 2; // 
	map[1].treasure = -1;

	// Cell 2
	map[2].north = -1;
	map[2].south = -1;
	map[2].west = 0;
	map[2].east = 4;
	map[2].up = -1;
	map[2].down = -1;
	strcpy(map[2].description, "You enter a large hall with high ceilings. The air is stale, and there's an eerie silence.");
	map[2].object = -1;
	map[2].treasure = 3; 

	// Cell 3
	map[3].north = -1;
	map[3].south = -1;
	map[3].west = 1;
	map[3].east = 5;
	map[3].up = -1;
	map[3].down = -1;
	strcpy(map[3].description, "You find yourself in a small room with faded murals on the walls. Dust covers the floor.");
	map[3].object = -1;
	map[3].treasure = -1;

	// Cell 4
	map[4].north = -1;
	map[4].south = -1;
	map[4].west = 2;
	map[4].east = -1;
	map[4].up = -1;
	map[4].down = -1;
	strcpy(map[4].description, "A narrow passage opens up, and you see a flickering light in the distance.");
	map[4].object = 1; 
	map[4].treasure = -1;

	// Cell 5
	map[5].north = -1;
	map[5].south = -1;
	map[5].west = 3;
	map[5].east = -1;
	map[5].up = -1;
	map[5].down = -1;
	strcpy(map[5].description, "You enter a cavernous chamber with a high ceiling. Strange markings cover the walls.");
	map[5].object = -1;
	map[5].treasure = 4; 

    struct ThreadData playerThreadData = {&player, map, &monster, &quitFlag};
    struct ThreadData monsterThreadData = {&player, map, &monster, &quitFlag};

    pthread_t playerThreadId, monsterThreadId;


    pthread_create(&playerThreadId, NULL, PlayerThread, (void *)&playerThreadData);
    pthread_create(&monsterThreadId, NULL, MonsterThread, (void *)&monsterThreadData);


    sleep(10);

  
    quitFlag = 1;


    pthread_join(playerThreadId, NULL);
    pthread_join(monsterThreadId, NULL);

  
    ShowPlayer(player);


    StartGame(&player, map, &monster);

    return 0;
}


void Fight(struct Player *pPlayer,struct Monster *pMonster, struct Cell *map)
{
    printf("You encountered a level %d monster!\n", pMonster->level);


    while (pPlayer->health > 0 && pMonster->health > 0)
    {
    
        int playerDamage = pPlayer->damage; 
        if (IsCriticalHit())
        {
        
            playerDamage * 2;
            printf("Player's critical hit! ");
        }
        pMonster->health -= playerDamage;
        printf("You dealt %d damage to the monster. Monster's health: %d\n", playerDamage, pMonster->health);
        usleep(500000); 
       
        if (pMonster->health <= 0)
        {
            printf("You defeated the monster! You are victorious!\n");
            
            InitMonster(pMonster);
            break;
        }

     
        int monsterDamage = pMonster->damage;
        if (IsCriticalHit())
        {
            
            monsterDamage * 2;
            printf("Monster's critical hit! ");
        }
        pPlayer->health -= monsterDamage;
        printf("The monster dealt %d damage to you. Your health: %d\n", monsterDamage, pPlayer->health);
        usleep(500000); 
    }

   
    if (pPlayer->health <= 0)
    {
        printf("You were defeated by the monster! Game over.\n");
        StartGame(pPlayer, map, pMonster);
    }
}
void StartGame(struct Player *pPlayer, struct Cell *map, struct Monster *pMonster)
{
    int choice;

    do
    {
      
        printf("Choose a direction to move (1: North, 2: South, 3: West, 4: East, 0: Quit): ");
        scanf("%d", &choice);

       
        switch (choice)
        {
        case 1:
        case 2:
        case 3:
        case 4:
            MovePlayer(pPlayer, map, choice, pMonster);
            if (pPlayer->cell == pMonster->cell)
            {
               
                Fight(pPlayer, pMonster, map);
            }
            break;
        case 0:
            printf("Exiting the game.\n");
            break;
        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 0);
}

void MovePlayer(struct Player *pPlayer, struct Cell *map, int direction, struct Monster *pMonster)
{
    int newCellIndex;

    switch (direction)
    {
    case 1: // North
        newCellIndex = pPlayer->cell + map[pPlayer->cell].north;
        break;
    case 2: // South
        newCellIndex = pPlayer->cell + map[pPlayer->cell].south;
        break;
    case 3: // West
        newCellIndex = pPlayer->cell + map[pPlayer->cell].west;
        break;
    case 4: // East
        newCellIndex = pPlayer->cell + map[pPlayer->cell].east;
        break;
    default:
        printf("Invalid direction.\n");
        return;
    }

  
    if (newCellIndex >= 0 && newCellIndex < MAX_MAP_CELLS)
    {
       
        pPlayer->cell = newCellIndex;

        
        if (map[pPlayer->cell].treasure > 0)
        {
            printf("You found a treasure!\n");
         
            pPlayer->treasure = map[pPlayer->cell].treasure;
            
        }

       
        ShowPlayer(*pPlayer);
        ShowMonster(*pMonster);  

        printf("Description: %s\n", map[pPlayer->cell].description);
    }
    else
    {
        printf("You cannot move in that direction.\n");
    }
}