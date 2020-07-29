//  Version 1.0
//  Terminal Tancala
//  Author: Ryder Daniel <ryderdaniel83@gmail.com>
//  Date: 12/7/2020


#include <ncurses.h>
#include <string>
#include <vector>

using namespace std;

typedef struct _win_border_struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {

	int startx, starty;
	int height, width;
	WIN_BORDER border;
}WIN;

void init_mancala(WIN *p_win, int sx, int sy){
    p_win->height = 10;
    p_win->width  = 4;
    p_win->startx = sx;
    p_win->starty = sy;

    p_win->border.ls = '|';
	p_win->border.rs = '|';
	p_win->border.ts = '-';
	p_win->border.bs = '-';
	p_win->border.tl = '+';
	p_win->border.tr = '+';
	p_win->border.bl = '+';
	p_win->border.br = '+';
}

void init_pot(WIN *p_win, int sx, int sy){
    p_win->height = 4;
    p_win->width  = 4;
    p_win->startx = sx;
    p_win->starty = sy;

    p_win->border.ls = '|';
	p_win->border.rs = '|';
	p_win->border.ts = '-';
	p_win->border.bs = '-';
	p_win->border.tl = '+';
	p_win->border.tr = '+';
	p_win->border.bl = '+';
	p_win->border.br = '+';
}

void init_pot_selected(WIN *p_win){
    p_win->border.ls = '\\';
    p_win->border.rs = '\\';
    p_win->border.ts = '=';
    p_win->border.bs = '=';
    p_win->border.tl = '@';
    p_win->border.tr = '@';
    p_win->border.bl = '@';
    p_win->border.br = '@';
}

void create_box(WIN *p_win, int amt, bool flag){
    int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;

    mvaddch(y, x, p_win->border.tl);
	mvaddch(y, x + w, p_win->border.tr);
	mvaddch(y + h, x, p_win->border.bl);
	mvaddch(y + h, x + w, p_win->border.br);
	mvhline(y, x + 1, p_win->border.ts, w - 1);
	mvhline(y + h, x + 1, p_win->border.bs, w - 1);
	mvvline(y + 1, x, p_win->border.ls, h - 1);
	mvvline(y + 1, x + w, p_win->border.rs, h - 1);
    if(amt < 10) mvprintw(y+1, x+1, "0%d", amt);
    else mvprintw(y+1, x+1, "%d",amt);
    if(flag) mvprintw(y+3, x+1, "VV");
    else mvprintw(y+3, x+1, "  ");
}

void draw(vector<int> st, bool tn, int ins){
    WIN boxes[14];
    int posx[14];
    int posy[15];
    int cen_x = COLS/2;
    int cen_y = LINES/2;
    for(int i = 0; i < 6; ++i)
        posx[i] = -23 + 7*i;
    for(int i = 7; i < 13; ++i)
        posx[i] = 12 -7*(i-7);
    for(int i = 0; i < 6; ++i) posy[i] = 1;
    for(int i = 7; i < 13; ++i)posy[i] = -5;

    init_mancala(&boxes[6], cen_x + 19, cen_y -5);
    init_mancala(&boxes[13], cen_x - 30, cen_y -5);
    for(int i = 0; i < 6; ++i){
        init_pot(&boxes[i], cen_x + posx[i], cen_y + posy[i]);
        init_pot(&boxes[i+7], cen_x + posx[i+7], cen_y + posy[i+7]);
    }
    if(ins >= 0) init_pot_selected(&boxes[ins]);
    int state_copy = st[ins];
    for(int i = 0; i < 14; ++i){
        create_box(&boxes[i], st[i], ((tn && i != 6) || (!tn && i != 13))&&((st[ins] >= 14) || ((i <= ins + st[ins]) && (i > ins)) || ((i <= (ins + st[ins])%14) && (ins + st[ins] >= 14))));
    }
    mvprintw(cen_y - 7, cen_x - 30, "Player %d's turn", (tn + 1));
}


int logic(vector<int>& st, int ins, bool& tn){
    int in_hand = st[ins];
    st[ins] = 0;
    if(tn){   // player 2's turn - check empty range from 7-12 inclusively and dont place into 6;
        int p = 1;
        for(int i = 0; i < in_hand; ++i){
            if((ins + p)%14 == 6){
                ++p; --i;
                continue;
            }
            ++st[(ins + p++)%14];
            if(i == in_hand-1 && (ins + p -1 )%14 == 13) tn = !tn;
            if(i == in_hand-1 && st[(ins + p-1)%14] == 1 && (ins+p-1)%14 <=12 && (ins+p-1)%14 >=7 ){
                st[13] += 1 + st[12-(ins+p-1)%14];
                st[(ins+p-1)%14] = 0; st[12-(ins+p-1)%14] = 0;
            }
        }
    } else {    // player 1's turn - check empty range from 0-5 inclusively and dont place into 13.
        int p = 1;
        for(int i = 0; i < in_hand; ++i){
            if((ins + p)%14 == 13){
                ++p; --i;
                continue;
            }
            ++st[(ins+p++)%14];
            if(i == in_hand-1 && (ins+p-1)%14 == 6) tn = !tn;
            if(i == in_hand-1 && st[(ins+p-1)%14] == 1 && (ins+p-1)%14 <=5 && (ins+p-1)%14 >= 0){
                st[6] += 1 + st[12-(ins+p-1)%14];
                st[(ins+p-1)%14]=0; st[12-(ins+p-1)%14]=0;
            }
        }
    }
    if(in_hand == 0) tn = !tn;
    bool flag = true;
    for(int i = 0; i < 6; ++i) if(st[i]) flag = false;
    if(flag){
        for(int i = 0; i < 6; ++i){
            st[6] += st[i]; st[i] = 0;
            st[13] += st[i+7]; st[i+7] = 0;
        }
        if(st[6] == st[13]) return 4;
        return st[6] < st[13];
    }
    for(int i = 7; i < 13; ++i) if(st[i]) flag = true;
    if(!flag){
        for(int i = 0; i < 6; ++i){
            st[6] += st[i]; st[i] = 0;
            st[13] += st[i+7]; st[i+7] = 0;
        }
        if(st[6] == st[13]) return 4;
        return st[6] < st[13];
    }
    return 3;
}


int main(){
    initscr();
    // game variables
    bool turn = false; // false = player 1, true = player 2
    
    vector<int> state (14,0);
    for(int i = 0; i < 6; ++i){
        state[i] = 4;
        state[i + 7] = 4;
    }

    int inspect = 0; // tells the game which pot the player is currently looking at.
    
    //loop
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    init_pair(1,COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    printw("Press F1 to quit.");
    refresh();
    attroff(COLOR_PAIR(1));


    draw(state, turn, inspect);
    int ch;
    int def_p1 = 0, def_p2 = 7;
    bool cont = true;
    while(cont && ((ch = getch()) != KEY_F(1))){
        switch(ch){
            case KEY_LEFT:
                if(turn) ++inspect;
                else --inspect;
                if(turn && inspect >= 13) inspect = 12;
                else if(!turn && inspect <= -1) inspect = 0;
                break;
            case KEY_RIGHT:
                if(turn) --inspect;
                else ++inspect;
                if(turn && inspect <= 6) inspect = 7;
                else if(!turn && inspect >= 6) inspect = 5;
                break;
            case ' ':
                int res;
                if((res = logic(state, inspect, turn)) != 3){
                    cont = false;
                    draw(state, turn, -1);
                    if(res == 4) mvprintw(LINES/2, (COLS-6)/2, "Tie!!!");
                    mvprintw(LINES/2, (COLS-39)/2, "Player %d Wins!!! Score: <~%d : %d~>", res+1, state[13], state[6]);
                    getch();
                }
                inspect = (turn)?def_p1:def_p2;
                turn = !turn;
                break;
            default:
                break;
        }
        draw(state, turn, inspect);
    }
    

    endwin();
    return 0;
}
