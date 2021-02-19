# Terminal Mancala

This is the famous board game mancala implemented in the terminal. It has the capability to play local multiplayer and play against the CPU. The CPU uses the minmax algorithm to play.

There is also a variant of the game with no stealing. This is a work in progress and should be implemented soon.

## Usage

* `./mancala <side(1/2)> <search depth>` This is to play against the AI. Side 1 always plays first. The search depth is the depth in which minmax will search. Anything greater than 10 would be considered hard. Still beatable but hard.
* `./mancala` This is to play local multiplayer.

You use the left and right arrow keys to change the selected pot, and spacebar to choose the pot of interest.

---

### More Information:
* [Rules of mancala](https://www.thesprucecrafts.com/how-to-play-mancala-409424#:~:text=Place%20four%20stones%20into%20each%20of%20the%20pits.&text=2.,a%20stone%20in%20there%2C%20too.)
* [Minmax](https://en.wikipedia.org/wiki/Minimax)

