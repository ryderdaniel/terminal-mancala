using namespace std;

class Mancala_game{
private:
	unsigned char board[2*6];
	unsigned char mancala[2];
	unsigned char turn; // 0 -> Player 1	1 -> Player 2
	unsigned char gameOver;

	unsigned char& b(int i, int j){
		return board[6*i + j];
	}

	int sumSide(int side){
		int s = 0;
		int i;
		for(i = 6; i--;) s += b(side, i);
		return s;
	}

	void flip(unsigned char &x){
		x = (x + 1)%2;
	}

public:
	Mancala_game(){
		int i;
		for(i = 12; i--;) board[i] = 4;
		this->mancala[0] = 0;
		this->mancala[1] = 0;
		this->turn = 0;
		this->gameOver = 0;
	}

	Mancala_game(const Mancala_game &target){
		int i;
		for(i = 12; i--;) board[i] = target.board[i];
		this->mancala[0] = target.mancala[0];
		this->mancala[1] = target.mancala[1];
		this->turn = target.turn;
		this->gameOver = target.gameOver;
	}

	bool isOver(){
		return this->gameOver;
	}

	int getTurn(){
		return this->turn;
	}
	
	int mancala_pot(int side){
		return this->mancala[side];
	}

	bool check_win(){
		if(sumSide(0) == 0 || sumSide(1) == 0){
			this->gameOver = 1;
			this->mancala[0] += sumSide(0);
			this->mancala[1] += sumSide(1);
			return true;
		}
		return false;
	}
	
	int get_winner(){
		// 0  -> player 1 won
		// 1  -> player 2 won
		// -1 -> the game is not over
		// 2  -> tie
		if(!this->gameOver) return -1;
		// Clean board here so minmax doesnt do this.
		int i;
		for(i = 6; i--;) b(0,i) = b(1,i) = 0;
		if(this->mancala[0] > 24) return 0;
		else if(this->mancala[1] > 24) return 1;
		else return 2;
	}

	bool pot_available(int pot){
		return b(this->turn, pot) != 0;
	}

	int heuristic(int depth){
		if(this->mancala[0] > 24){
			return 49 + depth;
		} else if(this->mancala[1] > 24){
			return -49 - depth;
		}
		//cout << this->mancala[0] - this->mancala[1] << endl;
		return this->mancala[0] - this->mancala[1];
	}

	void get_state(int state[14]){
		state[6] = (int)this->mancala[0];
		state[13]= (int)this->mancala[1];
		int i;
		for(i = 6; i--;){
			state[i] = (int)b(0,i);
			state[i+7] = (int)b(1,i);
		}
	}

	void move(int pot){
		if(this->gameOver || !pot_available(pot)) return;

		unsigned char side = this->turn;
		unsigned char drop_pot = pot;
		unsigned char stones = b(side, pot);
		b(side, pot) = 0;

		while(stones-1){
			++drop_pot;

			if(drop_pot == 6){
				if(side == this->turn){
					++this->mancala[side];
				} else {
					++stones;
				}
				drop_pot = -1;
				flip(side);
			} else {
				++b(side, drop_pot);
			}
			--stones;
		}
		++drop_pot;
		if(side == this->turn && drop_pot == 6){
			++this->mancala[side];
			flip(this->turn);
		} else if(side == this->turn && b(side, drop_pot) == 0){
			if(b((side + 1)%2, 5 - drop_pot) == 0){
				++b(side, drop_pot);
			} else {
				this->mancala[side] += 1 + b((side + 1)%2, 5 - drop_pot);
				b(side, drop_pot) = 0;
				b((side + 1)%2, 5 - drop_pot) = 0;
			}
		} else if(drop_pot == 6){
			flip(side);
			drop_pot = 0;
			if(b(side, drop_pot) != 0){
				++b(side, drop_pot);
			} else {
				if(b((side + 1)%2, 5) != 0){
					this->mancala[this->turn] += 1 + b((side + 1)%2, 5);
					b(side, 0) = 0;
					b((side + 1)%2, 5) = 0;
				} else {
					++b(side, drop_pot);
				}
			}
		} else {
			++b(side, drop_pot);
		}

		check_win();
		flip(this->turn);
	}
};
