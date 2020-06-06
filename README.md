# rbggamemanager

This is a game manager that can be used to play games written in regular board games language as described in
[rbgParser](https://github.com/uicus/rbgParser).

## Example

### Running the server

In order to run the server, first make the code by running:

```bash
./scripts/make.sh
```

Then to run the server (that is serving chess), run:

```bash
./build/start_server ./rbgGames/games/chess.rbg 8000
```

This will make the server wait for clients to join. The server is running on the specified port.
You can add a random player by running: (in a separate process)

```bash
./build/start_random_client localhost 8000
```

The server should respond immediately with `Got client number 0`. You can initiate a random game, by starting another client with the same game. Both the clients and the server should then play a game and close afterwards.

### Running a game

In order to see an example game being played run:

```bash
./build/explain ./rbgGames/games/breakthrough.rbg 
```

There are also various modes to play the games yourself (`--interactive help`) and to specify the moves tourself (as defined in the protocol below) (`--interactive full`).

## Protocol

The exchange between server and clients happends over TCP.
A message is a **null terminated string**. (A sequence of characters ending with `\null`.)

The first message is sent by manager to the player. It contains the **low level RBG language game**.

The server then waits for some time (`time_for_player_compilation` flag)
Afterwards the server sends the deadline as a **real number** (possibly containing a '.') 
representing the number of seconds a player has to make a move.

The second message contains a number that represents which player is the client. 
(1 means the first player that appears in `#player` macro)

Now the moves are being sent. The client who controls the player who has the 
current turn sends a message of the form:

```
vertex_1 modifier_index_1 vertex_2 modifier_index_2 ...
```

The vertices are numbered starting from 1 as they appear in `#board` macro. 
The modifier indices are indices of modifier actions in the game rules.
After the game is played, the `reset` message is sent and another game can be played. (If the client disconnects then no further games are played and the server shuts down.)
The next game starts without the initial message. 
(The game, deadline and the player assignment is not sent the second time.)

## Server logging

Server can log the moves done in each of the games in a file. 
This is controlled with the `log_moves` flag.
The result file contains one move as specified in the [protocol](#protocol). It contains the resets as well.


Also, the results of each game can be logged. 
This is done with `log_results` flag.
The format is:

```
<time of the game> <depth> <available_moves> <score_a> <score_b>
```

## Writing players

For writing new players, please refer to an example player in `./src/start_random_client.cc`.
If you want to create a more efficient player, see [rbg2cpp](https://github.com/uicus/rbg2cpp) 
or for python see [rbgcpp2py](https://github.com/shrumo/rbgcpp2py). Those allow compilation of
the rbg code.

## Visualizer

You can visualize how the resulting game [NFA](https://en.wikipedia.org/wiki/Nondeterministic_finite_automaton) 
looks like by doing: (this requires tkinter)

```bash
./scripts/visualize_with_tk.sh ./rbgGames/games/breakthrough.rbg 
```

![visualizer](https://raw.githubusercontent.com/shrumo/rbggamemanager/master/example_visualizer.png)

## Tests

To test the code run `test.sh` from `scripts/`.

## Dependencies

* asio for tcp communication (downloaded as a submodule)
* CMake for building the thing
* pybind11 for python bindings (downloaded as a submodule)
* tk for visualizing (with apt: `sudo apt install python3-tk`, used only when visualizing graphs)


