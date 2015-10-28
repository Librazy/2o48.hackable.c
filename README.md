# 2o48.hackable.c
An hackable 2048 game written in C
Used ncurses.h to control the input and output


## USAGE

### Welcome:
	Enter the size of board that you want to play.
	The size must be 1~9,
	The version number of game executable will be displayed at center of screen;
	
	
### Play:
	Use arrow left/down/up/right or key hjkl to move
	Type : to input commands;
	
	
### Command:
	
	#### Stash in memory:
		##### s | save  [n]
			Save current board and continuing current game in a new board
			n (optional): board number to jump to;
		##### s! | saveto  [n]
			Save current board to a new board and continuing current game in a current board
			n (optional): board number to save to;
		##### r | read  [n]
			Read from a board saved in memory
			n (optional):board number to read from;
			
	#### Save in file:
		The file name will be 2048[.%d].%X.save,
		where %d is the board number and %X is version number in hexadecimal;
		##### w | write
			Write current board to file;
		##### wb | writeboard  [n]
			Write board specfied to file
			n :board number to save;
		##### o | open  [n]
			Open a board saved in file
			n (optional): board number to read;
	
	####Gaming option
		##### q | quit | (ctrl+c)
			Quit the game with a confirming;
		##### q! | quit!
			Quit the game without confirming;
		##### wq | writequit
			Write current board to file and quit without confirming
			
## Lisense
The MIT License (MIT)

Copyright (c) 2015 Librazy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.