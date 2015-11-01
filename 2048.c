jwan yande /*!
 *  The password for the save file and represent the version of the game
 *  Should and only be changed when the saved file isn't/shouldn't compatible with others
 */ 
#define PWD ":2048"
/*!
 *  The length of PWD
 */ 
#define PWD_LEN 5
#ifndef LOCAL_NCURSES
#include <ncurses.h>
#else 
#include "./ncurses.h"
#endif
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
const int NA=127;                   /**< Stand for invalid grid. */
/*!
 *  The maxium board num (for saving in game)
 */ 
#define MAX_BOARD_NUM 16
/*!
 *  The maxium board size
 */ 
#define MAX_BOARD_SIZE 16
int P_RANDNUM=30;                   /**< The probability of an empty grid becoming filled */
int MAX_RANDNUM=2;                  /**< The maxium level of filling an grid. */

//Shortcuts for params when calling Eat()
/*!
  \def EUP
  Eat up
  \def EDOWN
  Eat down
  \def ELEFT
  Eat left
  \def ERIGHT
  Rat right
*/
#define EUP false,1
#define EDOWN false,-1
#define ELEFT true,1
#define ERIGHT true,-1

/*!
  \def MENU_POSITION_Y
  the Y position to print menu
  \def MENU_POSITION_X
  the X position to print menu
  \def WARNING_POSITION_Y
  the Y position to print warning
  \def WARNING_POSITION_X
  the X position to print warning
*/
#define MENU_POSITION_Y row-1
#define MENU_POSITION_X 0
#define WARNING_POSITION_Y row-2
#define WARNING_POSITION_X 0
/*! The password when generating the checksum */
const char cs_pwd[PWD_LEN+1]=PWD;   
/*! The boards to storage game progress */
char board[MAX_BOARD_NUM][MAX_BOARD_SIZE][MAX_BOARD_SIZE];
/*! The output string */
char boardstr[MAX_BOARD_SIZE][MAX_BOARD_SIZE][5];
/*! The random seed of the boards */
int boardseed[MAX_BOARD_NUM];
/*! Current board */
unsigned char curs=0;
//! Eat table(TODO:use eat array in CheckEat) 
/*! Will set a=eat[a][b][0]
*   and b=eat[a][b][1]
*   when eating a and b
*/
char eat[256][256][2];
/*! Display table */
/*! Will display a as string display[a]
*/
char display[256][16];
//! Point table(TODO:use eat array in CheckEat) 
/*! Will count a by point[a] when adding up the points
*/
int point[256];
/*! The size of the board */
char N=5;

/*! The size of the screen */
int row,col;

/// \brief   Set the global settings
///
///           TODO:use a ini instead?
/// \return void
void settings(){
    for(int i=0;i!=MAX_BOARD_NUM;++i){
        boardseed[i]=NA;
    }
    point[0]=0;
    point[1]=0;
    point[2]=0;
    point[3]=10;
    point[4]=20;
    point[5]=40;
    point[6]=80;
    point[7]=320;
    point[8]=640;
    point[9]=2560;
    point[10]=5120;
    point[11]=20480;
    point[12]=40960;
    strcpy(display[0]," ");
    strcpy(display[1],"2");
    strcpy(display[2],"4");
    strcpy(display[3],"8");
    strcpy(display[4],"16");
    strcpy(display[5],"32");
    strcpy(display[6],"64");
    strcpy(display[7],"128");
    strcpy(display[8],"256");
    strcpy(display[9],"512");
    strcpy(display[10],"1024");
    strcpy(display[11],"2048");
    strcpy(display[12],"whuang");
    strcpy(display[13],"whuang1");
    strcpy(display[14],"whuang2");
    strcpy(display[15],"whuang3");
    strcpy(display[16],"whuang4");
    strcpy(display[17],"whuang5");
    strcpy(display[18],"whuang6");
    strcpy(display[19],"whuang7");
    strcpy(display[20],"whuang8");
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_GREEN, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_WHITE);
    init_pair(8, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(9, COLOR_YELLOW, COLOR_WHITE);
    init_pair(10, COLOR_RED, COLOR_WHITE);
    init_pair(11, COLOR_CYAN, COLOR_MAGENTA);
    init_pair(12, COLOR_CYAN, COLOR_RED);
    init_pair(13, COLOR_RED, COLOR_CYAN);
    init_pair(14, COLOR_MAGENTA, COLOR_CYAN);
    init_pair(15, COLOR_BLUE, COLOR_CYAN);
    init_pair(16, COLOR_WHITE, COLOR_CYAN);
    init_pair(17, COLOR_BLUE, COLOR_MAGENTA);
    init_pair(18, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(19, COLOR_RED, COLOR_MAGENTA);
    init_pair(20, COLOR_YELLOW, COLOR_MAGENTA);
}

char  AlignCol (int curline,int direction);
char  AlignLine(int curline,int direction);
char  CheckEat (char *a,char *b);
void  Clrboard (int boardToClr);
char* Display  (char in);
char  Eat      (bool isH,int direction);
char  EatCol   (int curline,int direction);
char  EatLine  (int curline,int direction);
int   GetRandNums();
unsigned int Rando(int N);

/// \brief  Align the vertical direction
/// \param  curcol Current column to align
/// \param  direction The direction to align to
///
///          positive for up and negative for down
/// \return The number of blank grid in the column
char AlignCol(int curcol,int direction){
    int begin=(direction<=0)?N-1:0;
    int end=(direction<=0)?-1:N;
    int step=(direction<=0)?-1:1; 
    int curn=begin+step;
    for(int i=begin;i!=end;i+=step){
        if(step<0&&curn>(i+step))curn=i+step;else if(step>0&&curn<(i+step))curn=i+step;
        if(board[curs][i][curcol]==NA||board[curs][i][curcol]==0){
            while(curn!=end&&(board[curs][curn][curcol]==NA||board[curs][curn][curcol]==0))curn+=step;
            if(curn==end){break;}else{
                board[curs][i][curcol]=board[curs][curn][curcol];
                board[curs][curn++][curcol]=0;
            }
        }
    }
    char blank=0;
    for(int i=end-step;i!=begin-step;i-=step){
        if(board[curs][i][curcol]==NA||board[curs][i][curcol]==0){
            board[curs][i][curcol]=0;
            blank++;
        }else break;
    }
    return blank;
}
/// \brief  Align the horizontal direction
/// \param  curline Current line to align
/// \param  direction The direction to align to
///
///          positive for left and negative for right
/// \return The number of blank grid in the column
char AlignLine(int curline,int direction){
    int begin=(direction<=0)?N-1:0;
    int end=(direction<=0)?-1:N;
    int step=(direction<=0)?-1:1; 
    int curn=begin+step;
    for(int i=begin;i!=end;i+=step){
        if(step<0&&curn>(i+step))curn=i+step;else if(step>0&&curn<(i+step))curn=i+step;
        if(board[curs][curline][i]==NA||board[curs][curline][i]==0){
            while(curn!=end&&(board[curs][curline][curn]==NA||board[curs][curline][curn]==0))curn+=step;
            if(curn==end){break;}else{
                board[curs][curline][i]=board[curs][curline][curn];
                board[curs][curline][curn++]=0;
            }
        }
    }
    char blank=0;
    for(int i=end-step;i!=begin-step;i-=step){
        if(board[curs][curline][i]==NA||board[curs][curline][i]==0){
            board[curs][curline][i]=0;
            blank++;
        }else break;
    }
    return blank;
}
/// \brief  Check while the two number can be eated
/// \param  a Value a
/// \param  b Value b
/// \return The final value of a
char CheckEat(char* a,char* b){
    if(*a==0||*b==0||*a==NA||*b==NA)return NA;//If empty
    if(*a>=0&&*b>=0&&*a<=19&&*b<=19){
        if(*a==*b){*a=*a+1;*b=NA;return *a;}
    }
    return NA;
}
/// \brief  Empty the board specified
/// \param  boardToClr The board to empty
/// \return void
void Clrboard(int boardToClr){
    memset(board[boardToClr],0,sizeof(char)*MAX_BOARD_SIZE*MAX_BOARD_SIZE);
}
/// \brief  Get the grid's display string
/// \param  in The grid's value
/// \return The string to display
char* Display(char in){
    display[(unsigned char)in][15]='\0';
    return display[(unsigned char)in];
}
/// \brief  Eat the board at the direction specified
/// \param  isH Is horizontal
///
///          true for horizontal and false for vertical
/// \param  direction The direction to eat to
///
///          positive for left/up and negative for right/down
/// \return The number of empty grids
char Eat(bool isH,int direction){
    char blank=0;
    if(isH){
        for(int i=0;i!=N;++i){
            AlignLine(i,direction);
            blank+=EatLine(i,direction);
        }
    }else{
        for(int i=0;i!=N;++i){
            AlignCol(i,direction);
            blank+=EatCol(i,direction);
        }
    }
    return blank;
}
/// \brief  Eat the vertical direction
/// \param  curcol Current column to eat
/// \param  direction The direction to eat
///
///          positive for up and negative for down
/// \return The number of blank grid in the column
char EatCol(int curcol,int direction){
    int begin=(direction<=0)?N-1:0;
    int end=(direction<=0)?-1-direction:N-direction;
    int step=(direction<=0)?-1:1;
    for(int i=begin;i!=end;i+=step){
        CheckEat(&board[curs][i][curcol],&board[curs][i+direction][curcol]);
    }
    return AlignCol(curcol,direction);
}
/// \brief  Eat the horizontal direction
/// \param  curline Current line to eat
/// \param  direction The direction to eat
///
///          positive for left and negative for right
/// \return The number of blank grid in the column
char EatLine(const int curline,int direction){
    int begin=(direction<=0)?N-1:0;
    int end=(direction<=0)?-1-direction:N-direction;
    int step=(direction<=0)?-1:1;
    for(int i=begin;i!=end;i+=step){
        CheckEat(&board[curs][curline][i],&board[curs][curline][i+direction]);
    }
    return AlignLine(curline,direction);
}
/// \brief  Get random grid ranged fron 0 to MAX_RANDNUM on board
/// \return The number of random grid generated
int GetRandNums(){
    int count=0;
    for(int i=0;i!=N;++i){
        for(int j=0;j!=N;++j){
            if(board[curs][i][j]==0&&Rando(100)<P_RANDNUM){board[curs][i][j]=Rando(MAX_RANDNUM)+1;count++;}
        }
    }
    return count;
}
/// \brief  Generate random num ranged fron 0 to N-1
/// \param  N The upper bound of the random number
/// \return The random number
unsigned int Rando(int N){
    unsigned int t=RAND_MAX/N;if(t<1)t=1;
    unsigned int ra;
    while((ra=rand())>=t*N);
    return ra%N;
}

void command();
void die();
void play();
void showBoard(int offy,int offx);
void settings();
void welcome();

int  c_checksum();
void c_currentStr(bool show);
void c_forceQuit();
void c_loadStr();
void c_readBoard(int from);
void c_readFromDisk(int boards);
void c_saveBoard(int to,bool jmp);
void c_tryQuit();
int  c_version();
void c_warning(char* warn);
bool c_writeBoardToDisk(char board);

/// \brief  Show and handle commands inputed by :
/// \return void
void command(){
    char cmd[MAX_BOARD_SIZE*MAX_BOARD_SIZE*4];
    echo();
    curs_set(1);
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    attron(A_STANDOUT);
    mvprintw(MENU_POSITION_Y,MENU_POSITION_X,":");
    attroff(A_STANDOUT);
    int arg=NA;
    scanw("%s %d",cmd,&arg);cmd[15]='\0';
    noecho();
    curs_set(0);
    if(strcmp(cmd,"r")==0||strcmp(cmd,"read")==0){
        c_readBoard(arg);
    }else if(strcmp(cmd,"s")==0||strcmp(cmd,"save")==0){
        c_saveBoard(arg,true);
    }else if(strcmp(cmd,"s!")==0||strcmp(cmd,"saveto")==0){
        c_saveBoard(arg,false);
    }else if(strcmp(cmd,"q")==0||strcmp(cmd,"quit")==0){
        c_tryQuit();
    }else if(strcmp(cmd,"w")==0||strcmp(cmd,"write")==0){
        c_writeBoardToDisk(NA);
    }else if(strcmp(cmd,"wb")==0||strcmp(cmd,"writeboard")==0){
        c_writeBoardToDisk(arg);
    }else if(strcmp(cmd,"wq")==0||strcmp(cmd,"writequit")==0){
        if(c_writeBoardToDisk(NA))c_forceQuit();
    }else if(strcmp(cmd,"q!")==0||strcmp(cmd,"quit!")==0){
        c_forceQuit();
    }else if(strcmp(cmd,"o")==0||strcmp(cmd,"open")==0){
        c_readFromDisk(arg);
    }else if(strcmp(cmd,"c")==0||strcmp(cmd,"current")==0){
        c_currentStr(true);
    }else{
        char str[100];
        sprintf(str,"Librazy don't know how to %s,\nresume game now",cmd);
        str[99]='\0';
        c_warning(str);
    }
}
/// \brief  Handle when no empty grid present
/// \return void
void die(){
    c_warning("You are dead!\nPress q to quit or r to restart");
    int ch,cho=1;
    while(cho&&cho-2&&(ch=getch())){
        switch(ch){
            case ':':
                cho=2;
                command();
                break;
            case 'q':case 'Q':
                c_forceQuit();
                break;
            case 'r':case 'R':
                cho=0;
        }
    }
    if(cho==2)return;
    welcome();
    play();
}
/// \brief  Handle for main game
/// \return void
void play(){
    clear();
    if(boardseed[curs]==NA){
        boardseed[curs]=Rando(RAND_MAX);
    }
    srand(boardseed[curs]);
    Clrboard(curs);
    showBoard(5,5);
    keypad(stdscr, TRUE);
    int ch=0;
    int res=NA,lastres=NA;GetRandNums();showBoard(5,5);
    while((ch = getch()) != KEY_F(1)){
        lastres=res;
        switch(ch)
        {
            case KEY_LEFT:case 'H':case 'h':
                res=Eat(ELEFT);GetRandNums();clear();showBoard(5,5);
                break;
            case KEY_RIGHT:case 'L':case 'l':
                res=Eat(ERIGHT);GetRandNums();clear();showBoard(5,5);
                break;
            case KEY_UP:case 'K':case 'k':
                res=Eat(EUP);GetRandNums();clear();showBoard(5,5);
                break;
            case KEY_DOWN:case 'J':case 'j':
                res=Eat(EDOWN);GetRandNums();clear();showBoard(5,5);
                break;
            case ':':
                command();
                break;
            case 3:
                c_tryQuit();
                break;
        }
        if(res==lastres&&res==0){die();}
    }
}
/// \brief  Print the board to screen
/// \param  offy The y position for the left-up corner
/// \param  offx The x position for the left-up corner
/// \return void
void showBoard(int offy,int offx){
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    move(1+WARNING_POSITION_Y,WARNING_POSITION_X);
    clrtoeol();
    move(WARNING_POSITION_Y,WARNING_POSITION_X);
    clrtoeol();
    for(int j=0;j!=N;++j){
        for(int i=0;i!=N;++i){
            mvprintw(offy+j*2,offx+i*10,"----------");
            mvprintw(offy+j*2+1,offx+i*10,"|");
            mvprintw(offy+j*2+1,offx+i*10+1,"         ");
            attron(COLOR_PAIR(board[curs][j][i]));
            mvprintw(offy+j*2+1,offx+i*10+5-strlen(Display(board[curs][j][i]))/2,Display(board[curs][j][i]));
            attroff(COLOR_PAIR(board[curs][j][i]));
        }
        mvprintw(offy+j*2,offx+N*10,"-");
        mvprintw(offy+j*2+1,offx+N*10,"|");
    }
    for(int i=0;i!=N;++i){
        mvprintw(offy+N*2,offx+i*10,"----------");
    }
    mvprintw(offy+N*2,offx+N*10,"-");
    refresh();
}
/// \brief  Print welcome message and input the size of the board
/// \return void
void welcome(){
    clear();
    char mesg[]=":2048";
    getmaxyx(stdscr,row,col);
    mvprintw(row/2,(col-strlen(mesg))/2,mesg);
    int ver=c_version();
    char verstr[100];
    sprintf(verstr,"version %X",ver);
    mvprintw(row/2+1,(col-strlen(verstr))/2,verstr);
    mvprintw(row-3,0,"Welcome to :2048 by Librazy\n");
    printw("Enter a number<=9 that you want the board be:");
    int inpN=4+'0';
    while(1){inpN=getch();if(inpN>'0'&&inpN<='9')break;}
    N=(char)(inpN-'0');
    printw("%d\nThe board will be %d.Press any key and rock on!",N,N);
    getch();
}
/// \brief  Calculate the checksum for saving
/// \return The checksum for current board
int c_checksum(){
    int checksum=0;
    int rowsum=85,colsum=85,r=0,c=0;
    for(int i=0;i!=N;++i){
        r=0;
        for(int j=0;j!=N;++j){
            r+=((int)board[curs][i][j])^170;
            r^=cs_pwd[(j*N+i)%PWD_LEN];
        }
        rowsum^=r;
    }
    for(int j=0;j!=N;++j){
        c=0;
        for(int i=0;i!=N;++i){
            c+=((int)board[curs][i][j])^170;
            c^=cs_pwd[(i*N+j)%PWD_LEN];
        }
        colsum^=c;
    }
    checksum=(rowsum<<4)+colsum;
    return checksum;
}
/// \brief  Genetate the string representing current board
/// \param  show If need to print the string to screen
/// \return void
void c_currentStr(bool show){
    for(int i=0;i!=N;++i){
        for(int j=0;j!=N;++j){
            if(board[curs][i][j]<=25){
                boardstr[i][j][0]=board[curs][i][j]+'A';
                boardstr[i][j][1]='\0';
                continue;
            }
            if(board[curs][i][j]<=51){
                boardstr[i][j][0]=board[curs][i][j]+'a';
                boardstr[i][j][1]='\0';
                continue;
            }
            sprintf(boardstr[i][j],"%03d",board[curs][i][j]);
            printw("%03d",board[curs][i][j]);
        }
    }
    if(show){
        move(row-N-1,0);
        for(int i=0;i!=N;++i){
            for(int j=0;j!=N;++j){
                printw("%s",boardstr[i][j]);
            }
            printw("\n");
        }
    }
}
/// \brief  Quit the game
/// \return void
void c_forceQuit(){
    clear();
    endwin();
    exit(0);
}
/// \brief  Load the string representing saved board
/// \param  iptN The N in the saved game
/// \param  fp The file stream to read from
/// \return void
void c_loadStr(int iptN,FILE* fp){
    N=iptN;
    Clrboard(curs);
    char str[256*3+2];
    fscanf(fp,"%s",str);
    int checksum;int seed;
    fscanf(fp,"%d",&seed);
    fscanf(fp,"%d",&checksum);
    boardseed[curs]=seed;
    srand(seed);
    checksum-=seed;
    int offset=0;
    for(int i=0;i!=N;++i){
        for(int j=0;j!=N;++j){
            if(('@'<=str[i*N+j+offset])&&('Z'>=str[i*N+j+offset])){
                board[curs][i][j]=str[i*N+j+offset]-'A';
                continue;
            }
            if(('a'<=str[i*N+j+offset])&&('z'>=str[i*N+j+offset])){
                board[curs][i][j]=str[i*N+j+offset]-'a';
                continue;
            }
            if(('0'<=str[i*N+j+offset])&&('9'>=str[i*N+j+offset])){
                board[curs][i][j]=(str[i*N+j+offset]-'0')*100+
                                        (str[i*N+j+offset+1]-'0')*10+
                                        (str[i*N+j+offset+2]-'0');
                offset+=2;
                continue;
            }
        }
        printw("\n");
    }
    showBoard(5,5);
    if(c_checksum()==checksum){
        move(MENU_POSITION_Y,MENU_POSITION_X);
        clrtoeol();
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Game progress loaded successful!");
    }else{
        Clrboard(curs);
        showBoard(5,5);
        c_warning("Librazy found you are cheating!");
    }
    refresh();
}
/// \brief  Read the saved board
/// \param  from The number of board to read from
/// \return void
void c_readBoard(int from){
    if(from==NA){
        from=abs((curs-1)%MAX_BOARD_NUM);
    }
    boardseed[curs]=Rando(RAND_MAX);
    if(boardseed[from]==NA){
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Librazy found board#%d empty",curs);
        return;
    }
    srand(boardseed[from]);
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    from%=MAX_BOARD_NUM;
    curs=abs(from);
    showBoard(5,5);
    mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Load board#%d",curs,boardseed[curs]);
}
/// \brief  Read the saved file
/// \param  boards The number of the saved board.NA for not to use
/// \return void
void c_readFromDisk(int boards){
    char name[20];
    int ver=c_version();
    if(boards!=NA){
        sprintf(name,"2048.%d.%X.save",boards,ver);
    }else{
        sprintf(name,"2048.%X.save",ver);
        boards=curs;
    }
    FILE *fp;
    if((fp=fopen(name,"r"))) {
        int iptN;
        int iptVer=0;
        fscanf(fp,"%X",&iptVer);
        if(iptVer!=ver){
            char w[1024];
            sprintf(w,"Librazy found that the game's version dosen't match!\nYour version:%X, saved:%X",ver,iptVer);
            c_warning(w);
            return ;
        }
        move(WARNING_POSITION_Y,WARNING_POSITION_X);
        clrtoeol();
        mvprintw(WARNING_POSITION_Y,WARNING_POSITION_X,"Opening %s",name);
        fscanf(fp,"%d",&iptN);
        c_loadStr(iptN%16,fp);
    }else{
        move(MENU_POSITION_Y,MENU_POSITION_X);
        clrtoeol();
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Librazy don't know how to open %s, read file failed",name);
    }
    fclose(fp);
}
/// \brief  Save the board in memory
/// \param  to The number of the board to save to.NA for auto find nnext
/// \param  jmp If should jump to new board
/// \return void
void c_saveBoard(int to,bool jmp){
    if(to==NA){
        to=abs((curs+1)%MAX_BOARD_NUM);
    }
    boardseed[to]=boardseed[curs]=Rando(RAND_MAX);
    srand(boardseed[curs]);
    memcpy(board[to],board[curs],sizeof(char)*MAX_BOARD_SIZE*MAX_BOARD_SIZE);
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Save board#%d, current#%d",curs,to,boardseed[curs]);
    if(jmp){
        curs=to;
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Save board#%d, current#%d",curs,to,boardseed[curs]);
    }else{
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Save board#%d to #%d",curs,to,boardseed[curs]);
    }
}
/// \brief  Calculate the game's version
/// \return Version
int c_version(){
    int ver=0;
    for(int i=0;i<100;++i){
        ver+=(cs_pwd[i%PWD_LEN]^cs_pwd[(i+1)%PWD_LEN])+cs_pwd[(i+2)%PWD_LEN];
    }
    return ver;
}
/// \brief  Ask player whether to quit
/// \return void
void c_tryQuit(){
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    move(WARNING_POSITION_Y,WARNING_POSITION_X);
    clrtoeol();
    move(1+WARNING_POSITION_Y,WARNING_POSITION_X);
    clrtoeol();
    c_warning("You game progress won't be saved!\nPress q to quit, others to resume");
    int ch=0;
    ch=getch();
    if(ch=='q'||ch=='Q'){
        c_forceQuit();
    }
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    move(WARNING_POSITION_Y,WARNING_POSITION_X);
    clrtoeol();
    move(1+WARNING_POSITION_Y,WARNING_POSITION_X);
    clrtoeol();
}
/// \brief  Write the board to disk
/// \param  boards The number of board to save
/// \return Whether the file is saved successfully
bool c_writeBoardToDisk(char boards){
    if(boards==NA){
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Librazy don't know which board should be saved");
        return false;
    }
    char name[20];
    int ver=c_version();
    if(boards!=NA){
        sprintf(name,"2048.%d.%X.save",boards,ver);
    }else{
        sprintf(name,"2048.%X.save",ver);
        boards=curs;
    }
    FILE *fp;
    if((fp=fopen(name,"w+"))) {
        fprintf(fp,"%X\n",ver);
        char tmp=curs;
        curs=boards;
        c_currentStr(false);
        fprintf(fp,"%d\n",N);
        for(int i=0;i!=N;++i){
            for(int j=0;j!=N;++j){
                fprintf(fp,"%s",boardstr[i][j]);
            }
        }
        boardseed[curs]=Rando(RAND_MAX);
        srand(boardseed[curs]);
        fprintf(fp,"\n%d\n",boardseed[curs]);
        int checksum=c_checksum();
        fprintf(fp,"%d\n",checksum+boardseed[curs]);
        curs=tmp;
        move(MENU_POSITION_Y,MENU_POSITION_X);
        clrtoeol();
        mvprintw(MENU_POSITION_Y,MENU_POSITION_X,"Saved board#%d at %s",boards,name);
        fclose(fp);
        return true;
    }else{
        move(WARNING_POSITION_Y,WARNING_POSITION_X);
        clrtoeol();
        move(1+WARNING_POSITION_Y,WARNING_POSITION_X);
        clrtoeol();
        char str[100];
        sprintf(str,"Librazy don't know how to open %s,\nsave failed",name);
        c_warning(str);
        fclose(fp);
        return false;
    }
    
    
}
/// \brief  Print a warning to screen
/// \param  warn The string to print
/// \return void
void c_warning(char* warn){
    attron(COLOR_PAIR(10)|A_BOLD);
    mvprintw(WARNING_POSITION_Y,WARNING_POSITION_X,warn);
    attroff(COLOR_PAIR(10)|A_BOLD);
    refresh();
}
/// \brief  Main executable
/// \return 0
int main()
{
    srand((int)time(0));
    initscr();
    clear();
    curs_set(0);
    noecho();
    raw();
    if(has_colors())start_color();
    settings();
    welcome();
    play();
    getch();
    clear();
    endwin();
    return 0;
}
