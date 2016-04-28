help:
	@ echo "Run one of the following command lines:"
	@ echo "make allw     (to build the four Windows executables below)"
	@ echo "make win      (to build the Windows X64 executable)"
	@ echo "make winx86   (to build the Windows X86 executable)"
	@ echo "make gdi      (to build the Windows X64 executable using gdi)"
	@ echo "make gdix86   (to build the Windows X86 executable using gdi)"
	@ echo "make wino     (to build the Windows X64 executable with optimization for current CPU)"
	@ echo "make gdio      (to build the Windows X64 executable using gdi with optimization for current CPU)"
	@ echo "make linux    (to build the linux executables both w and w/o optimization)"
	@ echo "make gnu      (to build the linux executable)"
	@ echo "make gnuo     (to build the linux executable with optimization for current CPU)"
	@ echo "make wclean   (to clean up the working directory for Windows)"
	@ echo "make clean    (to clean up the working directory for Unix)"

all:allw linux

allw: wino winx86 gdio gdix86

win:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libpdcurses.a
	gcc -O4 -Wall -std=gnu11 -I./include/ ./lib/libpdcurses.a ./lib/libpthreadX64GC2.a ./include/dyad.c 2048.c -o 2048 -L./lib/ -lpdcurses -lpthreadX64GC2 -lws2_32 -DLOCAL_NCURSES

wind:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libpdcurses.a
	gcc -g3 -Wall -std=gnu11 -I./include/ ./lib/libpdcurses.a ./lib/libpthreadX64GC2.a ./include/dyad.c 2048.c -o 2048d -L./lib/ -lpdcurses -lpthreadX64GC2 -lws2_32 -DLOCAL_NCURSES

windo:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libpdcurses.a
	gcc -O4 -march=native -g3 -Wall -std=gnu11 -I./include/ ./lib/libpdcurses.a ./lib/libpthreadX64GC2.a ./include/dyad.c 2048.c -o 2048do -L./lib/ -lpdcurses -lpthreadX64GC2 -lws2_32 -DLOCAL_NCURSES

wino:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libpdcurses.a
	gcc -O4 -march=native -Wall -std=gnu11 -I./include/ ./lib/libpdcurses.a ./lib/libpthreadX64GC2.a ./include/dyad.c 2048.c -o 2048 -L./lib/ -lpdcurses -lpthreadX64GC2 -lws2_32 -DLOCAL_NCURSES
    
winx86:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libpdcursesx86.a
	gcc -O4 -Wall -std=gnu11 -I./include/ ./lib/libpdcursesx86.a ./lib/libpthreadX86GC2.a ./include/dyad.c 2048.c -o 2048x86 -L./lib/ -lpdcursesx86 -lpthreadX86GC2 -m32 -lws2_32 -DLOCAL_NCURSES
    
gdi:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libcursesgdi.a
	gcc -O4 -Wall -std=gnu11 -I./include/ -mwindows ./lib/libcursesgdi.a ./lib/libpthreadX64GC2.a ./include/dyad.c 2048.c -o 2048gdi -L./lib/ -lcursesgdi -lpthreadX64GC2 -lgdi32 -lcomdlg32 -lws2_32 -DLOCAL_NCURSES

gdio:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libcursesgdi.a
	gcc -O4 -march=native -Wall -std=gnu11 -I./include/ -mwindows ./lib/libcursesgdi.a ./lib/libpthreadX64GC2.a ./include/dyad.c 2048.c -o 2048gdi -L./lib/ -lcursesgdi -lpthreadX64GC2 -lgdi32 -lcomdlg32 -lws2_32 -DLOCAL_NCURSES
    
gdix86:./include/dyad.c 2048.c ./include/ncurses.h ./lib/libcursesgdix86.a
	gcc -O4 -Wall -std=gnu11 -I./include/ -mwindows ./lib/libcursesgdix86.a ./lib/libpthreadX86GC2.a ./include/dyad.c 2048.c -o 2048gdix86 -L./lib/ -lcursesgdix86 -lpthreadX86GC2 -lgdi32 -lcomdlg32 -m32 -lws2_32 -DLOCAL_NCURSES
    
linux:gnu gnuo
	
gnu:./include/dyad.c 2048.c
	gcc -O4 -Wall -std=gnu11 ./include/dyad.c 2048.c -o 2048 -lcurses -lpthread

gnuo:./include/dyad.c 2048.c
	gcc -O4 -march=native -Wall -std=gnu11 ./include/dyad.c 2048.c -o 2048 -lcurses -lpthread
    
wclean:
	if exist *.obj del *.obj
	if exist *.def del *.def
	if exist *.ilk del *.ilk
	if exist *.pdb del *.pdb
	if exist *.exp del *.exp
	if exist *.map del *.map
	if exist *.o del *.o
	if exist *.i del *.i
	if exist *.res del *.res
	if exist *.exe del *.exe
	if exist 2048 del 2048
	if exist *.save del *.save
    
clean:
	rm -f 2048 *.save *.exe *.pdb *.obj
