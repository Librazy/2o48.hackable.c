win:2048.c ncurses.h libpdcurses.a
	gcc -O4 -Wall -std=gnu11 libpdcurses.a 2048.c -o 2048 -L. -lpdcurses
winx86:2048.c ncurses.h libpdcursesx86.a
	gcc -O4 -Wall -std=gnu11 libpdcursesx86.a 2048.c -o 2048x86 -L. -lpdcursesx86 -m32
gdi:2048.c ncurses.h libcursesgdi.a
	gcc -O4 -Wall -std=gnu11 -mwindows libcursesgdi.a 2048.c -o 2048gdi -L. -lcursesgdi -lgdi32 -lcomdlg32
gdix86:2048.c ncurses.h libcursesgdix86.a
	gcc -O4 -Wall -std=gnu11 -mwindows libcursesgdix86.a 2048.c -o 2048gdix86 -L. -lcursesgdix86 -lgdi32 -lcomdlg32 -m32
linux:2048.c
	gcc -O4 -Wall -std=gnu11 2048.c -o 2048 -lcurses
clean:
	del *.o
	del *.exe
	