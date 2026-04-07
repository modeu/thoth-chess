# Thoth Chess Engine

> *Named after the Egyptian god of wisdom and knowledge — Thoth guides every move.*

Thoth is a chess engine written in C++, currently under active development. It communicates via the **UCI protocol** and can be used with any UCI-compatible chess GUI.

---

## Features

> ⚠️ Thoth is currently in development. Not all features may be fully implemented yet.

### Board Representation
- Bitboard-based board representation for fast move generation
- Zobrist hashing for position identification

### Search
- Alpha-Beta pruning
- Iterative Deepening
- Move ordering for improved cutoffs

### Evaluation
- Material evaluation
- Piece-square tables
- Basic pawn structure analysis

---

## Installation & Build

### Requirements
- C++17 or higher
- `cmake` ≥ 3.15
- A C++ compiler (GCC, Clang or MSVC)

### Build

```bash
git clone https://github.com/your-username/thoth-chess.git
cd thoth-chess
mkdir build && cd build
cmake ..
make
```

The compiled binary will be located at `build/thoth`.

---

## Usage (UCI)

Thoth uses the **Universal Chess Interface (UCI)** protocol and can be used with any UCI-compatible GUI such as [Arena](http://www.playwitharena.de/), [Cute Chess](https://cutechess.com/), or [Lucas Chess](https://lucaschess.pythonanywhere.com/).

### Run directly

```bash
./thoth
```

### Basic UCI commands

```
uci                  # Initialize UCI mode, engine responds with id and options
isready              # Check if engine is ready
position startpos    # Set starting position
position fen <FEN>   # Set position from FEN string
go movetime 3000     # Search for 3 seconds and return best move
quit                 # Exit the engine
```

### Example session

```
> uci
id name Thoth
id author Moritz Dwulit
uciok

> isready
readyok

> position startpos moves e2e4 e7e5
> go movetime 2000
bestmove g1f3
```

---

## Project Status

Thoth is currently under active development. Contributions, bug reports, and suggestions are welcome via [Issues](https://github.com/your-username/thoth-chess/issues).

---

## License

MIT License — see [LICENSE](LICENSE) for details.