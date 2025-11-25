#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct Student {
    int id;
    char name[50];
    int level;
};

void createaccount() {
    FILE *ptr;
    ptr = fopen("accounts.txt", "a+");
    struct Student s_new, s, last;
    printf("Enter your name:\n");
    scanf("%s", s_new.name);
    int last_id = 53875;
    int has_last = 0;
    while (fscanf(ptr, "%d %s %d", &s.id, s.name, &s.level) != EOF) {
        last = s;
        has_last = 1;
    }
    if (has_last) {
        last_id = last.id;
    }
    printf("Your id is %d\n", last_id + 1);
    s_new.id = last_id + 1;
    s_new.level = 1;
    fprintf(ptr, "%d %s %d\n", s_new.id, s_new.name, s_new.level);
    fclose(ptr);
    printf("Account created successfully\n");
}

// Function to update level in file
void updateLevel(int id, int newLevel) {
    FILE *ptr = fopen("accounts.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    struct Student s;
    while (fscanf(ptr, "%d %s %d", &s.id, s.name, &s.level) != EOF) {
        if (s.id == id) {
            s.level = newLevel;
        }
        fprintf(temp, "%d %s %d\n", s.id, s.name, s.level);
    }
    fclose(ptr);
    fclose(temp);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");
}

// Level function with timer, monsters, obstacles, exit option, and dynamic board allocation using pointers
int playLevel(int level, struct Student *player) {
    int n = level + 7;  // Board size: 8x8 for level 1, up to 21x21 for level 14
    int timeLimit = (level <= 4) ? 60 : (level <= 9) ? 90 : 120;  // Time limits
    // Dynamically allocate board using pointers
    char ***board = (char ***)malloc(n * sizeof(char **));
    for (int i = 0; i < n; i++) {
        board[i] = (char **)malloc(n * sizeof(char *));
        for (int j = 0; j < n; j++) {
            board[i][j] = (char *)malloc(4 * sizeof(char));
            strcpy(board[i][j], "☐");
        }
    }
    int kingX = n - 1, kingY = 0;  // King starts at bottom-left
    int numMonsters = level;  // 1 monster for level 1, up to 14 for level 14
    int numObstacles = (level >= 5) ? (level - 4) * 10 : 0;  // Obstacles from level 5, increasing
    int monstersLeft = numMonsters;  // Track remaining monsters

    strcpy(board[kingX][kingY], "♕");  // King

    // Place monsters randomly (avoid king start)
    srand(time(NULL));
    for (int i = 0; i < numMonsters; i++) {
        int mx, my;
        do {
            mx = rand() % n;
            my = rand() % n;
        } while ((mx == kingX && my == kingY) || strcmp(board[mx][my], "☐") != 0);
        strcpy(board[mx][my], "☠");
    }

    // Place obstacles randomly (avoid king and monsters)
    for (int i = 0; i < numObstacles; i++) {
        int ox, oy;
        do {
            ox = rand() % n;
            oy = rand() % n;
        } while ((ox == kingX && oy == kingY) || strcmp(board[ox][oy], "☐") != 0);
        strcpy(board[ox][oy], "█");
    }

    printf("Level %d: Board %dx%d, Monsters: %d, Obstacles: %d, Time Limit: %d seconds\n", level, n, n, numMonsters, numObstacles, timeLimit);
    printf("Kill all monsters (☠) by moving onto them! Avoid obstacles (█).\n");
    printf("Controls: W.Up, S.Down, A.Left, D.Right, Q.Quit\n");

    time_t startTime = time(NULL);
    while (1) {
        // Display board
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf(" %s ", board[i][j]);
            }
            printf("\n");
        }

        // Check time
        time_t currentTime = time(NULL);
        int elapsed = (int)difftime(currentTime, startTime);
        int remaining = timeLimit - elapsed;
        if (remaining <= 0) {
            printf("Time's up! Level %d failed. Retrying level %d.\n", level, level);
            // Free memory
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    free(board[i][j]);
                }
                free(board[i]);
            }
            free(board);
            return 0;  // Loss, retry same level
        }
        printf("Time remaining: %d seconds, Monsters left: %d\n", remaining, monstersLeft);

        // Get move
        char choice;
        printf("Enter move: ");
        
        scanf(" %c", &choice);
	while(choice != 'A' || choice != 'S' || choice != 'W' || choice != 'D' || choice != 'a' || choice != 's' || choice != 'd' || choice != 'w' ){
	printf("Invalid Input.\n");
	break;
	}
	        

        if (choice == 'Q' || choice == 'q') {
            printf("Exiting game.\n");
            // Free memory
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    free(board[i][j]);
                }
                free(board[i]);
            }
            free(board);
            return -1;  // Quit
        }

        int newX = kingX, newY = kingY;
        if (choice == 'W' || choice == 'w') newX--;
        else if (choice == 'S' || choice == 's') newX++;
        else if (choice == 'A' || choice == 'a') newY--;
        else if (choice == 'D' || choice == 'd') newY++;
        else continue;  // Invalid input, retry

        // Check bounds
        if (newX < 0 || newX >= n || newY < 0 || newY >= n) continue;

        // Check for obstacle
        if (strcmp(board[newX][newY], "█") == 0) {
            printf("Hit an obstacle! Level %d failed. Retrying level %d.\n", level, level);
            // Free memory
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    free(board[i][j]);
                }
                free(board[i]);
            }
            free(board);
            return 0;  // Loss, retry same level
        }

        // Check for monster (kill it)
        if (strcmp(board[newX][newY], "☠") == 0) {
            monstersLeft--;
            printf("Monster killed! Monsters left: %d\n", monstersLeft);
        }

        // Move king
        strcpy(board[kingX][kingY], "☐");
        kingX = newX;
        kingY = newY;
        strcpy(board[kingX][kingY], "♕");

        // Check win (all monsters killed)
        if (monstersLeft == 0) {
            time_t endTime = time(NULL);
            int timeTaken = (int)difftime(endTime, startTime);
            printf("Level %d completed in %d seconds! All monsters killed.\n", level, timeTaken);
            player->level = level + 1;
            updateLevel(player->id, player->level);
            // Free memory
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    free(board[i][j]);
                }
                free(board[i]);
            }
            free(board);
            return 1;  // Win
        }
    }
}

int main() {
    struct Student *player = (struct Student *)malloc(sizeof(struct Student));  // Use pointer for player
    FILE *ptr;
    printf("||***Welcome to Monster Hunter Game***||\n");
    printf("Enter your choice:\n1. Login\n2. Create Account\n");
    int choice = 0;
    int found = 0;
    int search_id;
    char search_name[50];
    scanf("%d", &choice);
    if (choice == 1) {
        printf("Enter Search mode:\n1. ID\n2. Name\n");
        int mode;
        scanf("%d", &mode);
        if (mode == 1) {
            printf("Welcome\nEnter your ID:\n");
            scanf("%d", &search_id);
        } else if (mode == 2) {
            printf("Enter your name:\n");
            scanf("%s", search_name);
        } else {
            printf("Enter 1 or 2\n");
            while (mode != 1 && mode != 2) {
                scanf("%d", &mode);
                if (mode == 1) {
                    printf("Welcome\nEnter your ID:\n");
                    scanf("%d", &search_id);
                } else if (mode == 2) {
                    printf("Enter your name:\n");
                    scanf("%s", search_name);
                }
            }
        }
        ptr = fopen("accounts.txt", "r");
        if (ptr == NULL) {
            printf("Account not found.\nWant to create account\n");
            int c6;
            printf("Enter \n1.To create account\n2.To exit\n");
            scanf("%d", &c6);
            if (c6 == 1) {
                createaccount();
            } else {
                free(player);
                return 0;
            }
        }
        while (fscanf(ptr, "%d %s %d", &player->id, player->name, &player->level) != EOF) {
            if ((mode == 1 && player->id == search_id) || (mode == 2 && strcmp(search_name, player->name) == 0)) {
                printf("Account found\n");
                printf("Welcome Mr.%s\n", player->name);
                printf("Your Id:%d\n", player->id);
                printf("Your Level:%d\n", player->level);
                found = 1;
                break;
            }
        }
        fclose(ptr);
        if (!found) {
            printf("Account not found.\nWant to create account\n");
            int c5;
            printf("Enter \n1.To create account\n2.To exit\n");
            scanf("%d", &c5);
            if (c5 == 1) {
                createaccount();
            } else {
                free(player);
                return 0;
            }
        }

        // Play levels from current to 14, retry on failure, allow quit
        while (player->level <= 14) {
            int result = playLevel(player->level, player);
            if (result == -1) {
                // Quit
                break;
            } else if (result == 0) {
                // Loss: Retry same level
                continue;
            }
            // Win: Proceed to next level
        }
        if (player->level > 14) {
            printf("Congratulations! You completed all levels!\n");
        }
        // End of game options
        printf("Game over. Enter 1 to play again (from your saved level), 2 to exit: ");
        int endChoice;
        scanf("%d", &endChoice);
        if (endChoice == 1) {
            // Reload player level and restart
            int saved_id = player->id;
            struct Student temp;
            ptr = fopen("accounts.txt", "r");
            while (fscanf(ptr, "%d %s %d", &temp.id, temp.name, &temp.level) != EOF) {
                if (temp.id == saved_id) {
                    player->id = temp.id;
                    strcpy(player->name, temp.name);
                    player->level = temp.level;
                    break;
                }
            }
            fclose(ptr);
            // Restart the game loop (using a loop instead of recursion)
            while (player->level <= 14) {
                int result = playLevel(player->level, player);
                if (result == -1) {
                    break;
                } else if (result == 0) {
                    continue;
                }
            }
            if (player->level > 14) {
                printf("Congratulations! You completed all levels!\n");
            }
        } else {
            printf("Exiting.\n");
        }
    }
    if (choice == 2) {
        createaccount();
    }

    free(player);
    return 0;
}
