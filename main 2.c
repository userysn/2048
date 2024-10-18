#include <ncurses.h>
#include <time.h>
#include <stdlib.h>

int board[4][4];
int score = 0;

typedef enum 
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    END_GAME
} key;


void init_board() {
    for (int i = 0; i < 4; i++) 
    {
        for (int c = 0; c < 4; c++) 
        {
            board[i][c] = 0;
        }
    }
}

void display_board() {
    clear();
    printw("========= 2048 ==================================\n");
    printw("==================================== Score : %4i\n", score);
    printw("\n");
    for (int l = 0; l < 4; l++) {
        printw("+-------+-------+-------+-------+\n");
        for (int c = 0; c < 4; c++) 
        {
            if (board[l][c] == 0)
                printw("|       ");
            else
                printw("|%7d", board[l][c]);
        }
        printw("|\n");
    }
    printw("+-------+-------+-------+-------+\n");
    refresh();
}

void INIT_2048() 
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    srand(time(NULL));
}

void DONE_2048() 
{
    endwin();
    exit(0);
}

int count_empty() 
{
    int empty_count = 0;
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            if (board[i][j] == 0) 
            {
                empty_count++;
            }
        }
    }
    return empty_count;
}

void add_two(int empty)
 {
    if (empty == 0)
        return;

    int n = rand() % empty;
    int count = 0;
    for (int i = 0; i < 4; i++)
     {
        for (int j = 0; j < 4; j++) 
        {
            if (board[i][j] == 0) 
            {
                if (count == n) 
                {
                    board[i][j] = 2;
                    return;
                }
                count++;
            }
        }
    }
}

int game_over(int add) {
    int empty = count_empty();

    if (empty == 0) 
    {
        printw("\n\nFin du jeu! Aucune case vide.\n");
        printw("Appuyez sur une touche pour continuer...");
        refresh();
        getch();
        return 1;
    } 
    else if (add) 
    {
        add_two(empty);
        display_board();
        return 0;
    } 
    else
        return 0;
    
}

int shift_board() {
    int moved = 0;
    for (int i = 0; i < 4; i++) { // Parcours des lignes
        int write_pos = 0; // Position où écrire la prochaine valeur non-nulle
        for (int j = 0; j < 4; j++) { // Parcours des colonnes
            if (board[i][j] != 0) { // Si la case n'est pas vide
                if (j != write_pos) { // Si la tuile peut être déplacée
                    board[i][write_pos] = board[i][j]; // Déplace la tuile
                    board[i][j] = 0; // Vide l'ancienne position
                    moved = 1; // Marque qu'un déplacement a eu lieu
                }
                write_pos++; // Incrémente la position d'écriture pour la prochaine valeur non-nulle
            }
        }
    }
    return moved;
}

int update_board() {
    int moved = shift_board(); // Premier décalage pour compresser les tuiles
    for (int i = 0; i < 4; i++) 
    { // Parcours des lignes
        for (int j = 0; j < 3; j++) 
        { // Parcours des colonnes, jusqu'à l'avant-dernière
            if (board[i][j] != 0 && board[i][j] == board[i][j + 1])
             { // Si deux tuiles adjacentes sont égales
                board[i][j] *= 2; // Double la valeur de la tuile de gauche
                score += board[i][j]; // Met à jour le score
                board[i][j + 1] = 0; // Vide la tuile de droite
                moved = 1; // Marque qu'une addition a eu lieu
                j++; // Saute la tuile suivante puisqu'elle a déjà été fusionnée
            }
        }
    }
    moved |= shift_board(); // Deuxième décalage pour compresser après les additions
    return moved;
}

key get_key() {
    int ch;
    while (1) 
    {
        ch = getch();
        switch (ch)
         {
            case KEY_UP:
                return UP;
            case KEY_DOWN:
                return DOWN;
            case KEY_LEFT:
                return LEFT;
            case KEY_RIGHT:
                return RIGHT;
            case KEY_BACKSPACE: 
                return END_GAME;
            default:
                // Ignore les autres touches
                break;
        }
    }
}

void swap(int* a, int* b) 
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void mirror_board()
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 2; j++) 
        { // Seulement jusqu'à la moitié
            swap(&board[i][j], &board[i][4 - j - 1]);
        }
    }
}

void pivot_board() 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = i; j < 4; j++)
        {
            swap(&board[i][j], &board[j][i]);
        }
    }
}


int play(key dir) {
    int moved = 0;
    switch (dir) 
    {
        case LEFT: // Déjà dans le bon sens, pas de transformation nécessaire
            moved = update_board();
            break;
        case RIGHT:
            mirror_board(); // Miroir pour jouer comme si on allait à gauche
            moved = update_board();
            mirror_board(); // Miroir pour revenir à l'orientation originale
            break;
        case UP:
            pivot_board(); // Pivot pour jouer comme si on allait à gauche
            moved = update_board();
            pivot_board(); // Pivot pour revenir à l'orientation originale
            break;
        case DOWN:
            pivot_board(); // Pivot pour jouer comme si on allait à gauche
            mirror_board(); // Miroir pour ajuster l'orientation
            moved = update_board();
            mirror_board(); // Miroir pour revenir à l'étape pivot
            pivot_board(); // Pivot pour revenir à l'orientation originale
            break;
        default:
            break;
    }
    return moved;
}


int main() 
{
    int gameStatus = 1;
    INIT_2048();
    init_board();
    add_two(count_empty()); // Ajoute un "2" initial pour commencer le jeu
    display_board();

    while (gameStatus)
     {
        key dir = get_key();
        if (dir == END_GAME) 
        {
            break; // Sort du jeu
        }
        if (play(dir)) 
        { // Si au moins un entier a bougé ou a été additionné
            add_two(count_empty());
        }
        display_board();
        gameStatus = !game_over(0); // Met à jour le statut du jeu sans ajouter de nouveau "2"
    }

    DONE_2048();
    return 0;
}

