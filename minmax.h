// Requires utility.h
// Requires mancala.h
//
// -1 -> Minimizing		1 -> Maximizing
// returns Value, then move.
pair<int,int> minmax(Mancala_game &game, int depth_left, int mode, int alpha=-10000000, int beta=10000000){
	if(!depth_left || game.isOver()) return make_pair(game.heuristic(depth_left), -1);

	int best_value = -100000 * mode;
	int best_move = -1;
	int game_turn = game.getTurn();
	int move;

	for(move = 6; move--;){
		if(!game.pot_available(move)) continue;
		Mancala_game temp_game = Mancala_game(game);
		temp_game.move(move);
		int temp_mode = -mode;
		if(temp_game.getTurn() == game_turn) temp_mode = mode;

		pair<int,int> result = minmax(temp_game, depth_left-1, temp_mode, alpha, beta);

		if(mode < 0){
			if(result.first < best_value){
				best_value = result.first;
				best_move = move;
			}
			beta = min(beta, result.first);
			if(beta <= alpha) break;
		} else {
			if(result.first > best_value){
				best_value = result.first;
				best_move = move;
			}
			alpha = max(alpha, result.first);
			if(beta <= alpha) break;
		}
	}
	return make_pair(best_value, best_move);
}
