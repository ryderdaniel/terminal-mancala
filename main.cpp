#include <ncurses.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <iostream>
#include "mancala.h"
#include "minmax.h"

using namespace std;


typedef struct _win_border_struct {
	chtype  ls, rs, ts, bs,
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

	p_win->border.ls = ACS_VLINE;
	p_win->border.rs = ACS_VLINE;
	p_win->border.ts = ACS_HLINE;
	p_win->border.bs = ACS_HLINE;
	p_win->border.tl = ACS_ULCORNER;
	p_win->border.tr = ACS_URCORNER;
	p_win->border.bl = ACS_LLCORNER;
	p_win->border.br = ACS_LRCORNER;
}

void init_pot(WIN *p_win, int sx, int sy){
	p_win->height = 4;
	p_win->width  = 4;
	p_win->startx = sx;
	p_win->starty = sy;

	p_win->border.ls = ACS_VLINE;
	p_win->border.rs = ACS_VLINE;
	p_win->border.ts = ACS_HLINE;
	p_win->border.bs = ACS_HLINE;
	p_win->border.tl = ACS_ULCORNER;
	p_win->border.tr = ACS_URCORNER;
	p_win->border.bl = ACS_LLCORNER;
	p_win->border.br = ACS_LRCORNER;
}

void init_pot_selected(WIN *p_win){
	p_win->border.ls = ACS_VLINE;
	p_win->border.rs = ACS_VLINE;
	p_win->border.ts = ACS_HLINE;
	p_win->border.bs = ACS_HLINE;
	p_win->border.tl = ACS_ULCORNER;
	p_win->border.tr = ACS_URCORNER;
	p_win->border.bl = ACS_LLCORNER;
	p_win->border.br = ACS_LRCORNER;
}

void create_box(WIN *p_win, int amt, bool flag, int player){
	int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;

	init_pair(1, COLOR_BLUE , COLOR_BLACK);
	init_pair(2, COLOR_RED  , COLOR_BLACK);
	init_pair(3, COLOR_YELLOW , COLOR_BLACK);

	// 1 - player 1 = blue      2 - player 2 = red      3 - inspected = yellow
	if(player == 2) attron(COLOR_PAIR(2));
	else if(player == 1) attron(COLOR_PAIR(1));
	else attron(COLOR_PAIR(3));
	
	mvaddch(y, x, p_win->border.tl);
	mvaddch(y, x + w, p_win->border.tr);
	mvaddch(y + h, x, p_win->border.bl);
	mvaddch(y + h, x + w, p_win->border.br);
	mvhline(y, x + 1, p_win->border.ts, w - 1);
	mvhline(y + h, x + 1, p_win->border.bs, w - 1);
	mvvline(y + 1, x, p_win->border.ls, h - 1);
	mvvline(y + 1, x + w, p_win->border.rs, h - 1);
	
	if(player == 2 ) attroff(COLOR_PAIR(2));
	else if(player == 1) attroff(COLOR_PAIR(1));
	else attroff(COLOR_PAIR(3));
	
	if(amt < 10) mvprintw(y+1, x+1, " %d", amt);
	else mvprintw(y+1, x+1, "%d",amt);
	
	attron(COLOR_PAIR(3));
	
	if(flag) mvaddch(y+3, x+2, ACS_DIAMOND);
	else mvprintw(y+3, x+1, "  ");
	
	attroff(COLOR_PAIR(3));
}

void draw(Mancala_game &game, int inspected_pot){

	// Array of integers [ player1's pots player1's mancala player2's pots player2's mancala]
	int state[14];
	game.get_state(state);

	int turn = game.getTurn();			// 0 -> Player 1		1 -> Player 2

	WIN boxes[14];
	int posx[14];
	int posy[15];
	int cen_x = COLS/2;
	int cen_y = LINES/2;

	for(int i = 0; i < 6; ++i)  posx[i] = -23 + 7*i;
	for(int i = 7; i < 13; ++i) posx[i] = 12 -7*(i-7);
	for(int i = 0; i < 6; ++i)  posy[i] = 1;
	for(int i = 7; i < 13; ++i) posy[i] = -5;

	init_mancala(&boxes[6], cen_x + 19, cen_y -5);
	init_mancala(&boxes[13], cen_x - 30, cen_y -5);
	
	int i;
	for(i = 6; i--;){
		init_pot(&boxes[i], cen_x + posx[i], cen_y + posy[i]);
		init_pot(&boxes[i+7], cen_x + posx[i+7], cen_y + posy[i+7]);
	}
	
	init_pot_selected(&boxes[inspected_pot]);

	for(int i = 0; i < 14; ++i){
		int c;
		
		if(i == inspected_pot) c = 3;
		else if(i < 7) c = 1;
		else c = 2;
		
		bool exp = (inspected_pot >=0) && ((((turn && i != 6)||(!turn && i !=13))&&(state[inspected_pot] >=13))
				|| ((!turn)&&(i != 13)&&((i > inspected_pot && i <= inspected_pot + state[inspected_pot] 
				|| (i <=(inspected_pot + state[inspected_pot])%13 && (inspected_pot + state[inspected_pot] > 12)))))
				||  ((turn)&&(i != 6)&&((i > inspected_pot && i <= inspected_pot + state[inspected_pot]) 
				|| ((i <=(inspected_pot + state[inspected_pot])%14 && (inspected_pot + state[inspected_pot])%14 < 6)&&(inspected_pot+state[inspected_pot]>13)) 
				|| ((i<=(inspected_pot+state[inspected_pot])%13 && (inspected_pot+state[inspected_pot])%14 >= 6) 
				&& (inspected_pot + state[inspected_pot]>13)))));

		create_box(&boxes[i], state[i], exp, c);
	}
	mvprintw(cen_y - 7, cen_x - 30, "Player %d's turn", (turn + 1));
}

void human(Mancala_game &g){
	int oldTurn = g.getTurn();
	while(g.getTurn() == oldTurn && !g.isOver()){
		int move;
		cout << "Your Move: ";
		cin >> move;
		g.move(move);
	}
}


int main(int argc, char * argv[]){
	if(argc != 1 && argc != 3){
		cout << "\nUsage:\t./mancala\t\t\tIf you would like to play with two players.\n\t./mancala <side(1/2)> <depth>\tIf you would like to play against an AI.\n" << endl;
		return 0;
	}
	
	int inspect = 0;
	Mancala_game game = Mancala_game();

	initscr();

	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	start_color();

	curs_set(0);

	init_pair(1,COLOR_CYAN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	printw("Press F1 to quit.\n");
	printw("Use left and right arrow keys to change the selected pot, and spacebar to select the pot under inspection.");
	refresh();
	attroff(COLOR_PAIR(1));

	if(argc == 1){
		// Local Multiplayer
		
		draw(game, inspect);
		int ch;
		int old_turn;

		while(!game.isOver() && (((ch = getch()) != KEY_F(1)))){
			old_turn = game.getTurn();
			switch(ch){
				case KEY_LEFT:
					if(game.getTurn() == 1) ++inspect; else --inspect;
					if(game.getTurn() == 1 && inspect >= 13) inspect = 12;
					else if(game.getTurn() == 0 && inspect <= -1) inspect = 0;
					break;
				case KEY_RIGHT:
					if(game.getTurn() == 1) --inspect; else ++inspect;
					if(game.getTurn() == 1 && inspect <= 6) inspect = 7;
					else if(game.getTurn() == 0 && inspect >= 6) inspect = 5;
					break;
				case ' ':
					{
						int pot_selected = (game.getTurn())?inspect-7:inspect;
						if(!game.pot_available(pot_selected)) continue;
						game.move(pot_selected);
						if(game.getTurn() != old_turn) inspect = (old_turn)?0:7;
					}
					break;
				case 'f':
					clear();
					draw(game, inspect);
					break;
				default:
					break;
			}
			draw(game, inspect);
		}
		
		if(game.isOver()){
			int res = game.get_winner();
			clear();
			draw(game,-10);

			if(res == 2) mvprintw(LINES/2, (COLS-6)/2, "Tie!!!");
			else mvprintw(LINES/2, (COLS-39)/2, "Player %d Wins!!! Score: <~%d : %d~>", res+1, game.mancala_pot(1), game.mancala_pot(0));
			refresh();
			getch();
		}
		curs_set(1);
		endwin();
		return 0;

	} else {
		// Single Player vs AI

		int human_turn = stoi(argv[1])-1;
		int search_depth = stoi(argv[2]);
		int ch;
		int human_side = 7*human_turn;
		if(human_side) inspect = -1;

		draw(game,inspect);
		while(!game.isOver() && ((game.getTurn() != human_turn) || (ch = getch()) != KEY_F(1))){
			if(game.getTurn() != human_turn){
				// Robot is playing
				mvprintw(LINES/2,(COLS-15)/2, " Thinking... ");
				refresh();
				int robot = minmax_move(game, search_depth,(human_turn)?1:-1);
				mvprintw(LINES/2, (COLS-13)/2, "I chose pot %d", robot+1);
				inspect = robot + 7*(!human_turn);
				draw(game,inspect);
				refresh();
				sleep(1);
				game.move(robot);
				inspect = human_side;
				mvprintw(LINES/2,(COLS-15)/2, "                    ");
			} else {
				// Human is playing
				
				switch(ch){
					case KEY_LEFT:
						if(game.getTurn() == 1) ++inspect; else --inspect;
						if(game.getTurn() == 1 && inspect >= 13) inspect = 12;
						else if(game.getTurn() == 0 && inspect <= -1) inspect = 0;
						break;
					case KEY_RIGHT:
						if(game.getTurn() == 1) --inspect; else ++inspect;
						if(game.getTurn() == 1 && inspect <= 6) inspect = 7;
						else if(game.getTurn() == 0 && inspect >= 6) inspect = 5;
						break;
					case ' ':
						if(!game.pot_available(inspect-7*(game.getTurn()))) continue;
						game.move(inspect-7*(game.getTurn()));
						break;
					case 'f':
						clear();
						draw(game, inspect);
						break;
					default:
						break;
				}	

			}
			int robot_render = (game.getTurn() == human_turn)?inspect:-1;
			draw(game,robot_render);
		}

		if(game.isOver()){
			int res = game.get_winner();
			clear();
			draw(game,-10);

			if(res == 2) mvprintw(LINES/2, (COLS-6)/2, "Tie!!!");
			else mvprintw(LINES/2, (COLS-39)/2, "Player %d Wins!!! Score: <~%d : %d~>", res+1, game.mancala_pot(1), game.mancala_pot(0));
			refresh();
			getch();
		}
		curs_set(1);
		endwin();
		return 0;
	}
}

