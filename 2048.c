/*!
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
    #include <pthread.h>
#else 
    #include "ncurses.h"
    #include "pthread.h"
#endif
typedef pthread_mutex_t Mut;
typedef pthread_cond_t Cond;
typedef pthread_attr_t Attr;
typedef pthread_t Thrd;
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#ifndef LOCAL_NCURSES
    #include "./include/dyad.h"
#else 
    #include "dyad.h"
#endif

const int NA=126;                   /**< Stand for invalid grid. */
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
#define MENU_POSITION_Y 0
#define MENU_POSITION_X 0
#define WARNING_POSITION_Y 0
#define WARNING_POSITION_X 0
/*! The password when generating the checksum */
const char cs_pwd[PWD_LEN+1]=PWD; 
/*! The mutex lock of the board */
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
/*! The buffer of the message to print */
char sinfo[512];
/*! If the message is a warning */
bool iswarn;
/*! If the game is in networking mode */
bool isnetworking;

bool connecting=false;

dyad_Stream *SClient;
dyad_Stream *SServ;
dyad_Stream *STmp;
dyad_Stream *SGaming;

/*! The handle of t_NetworkPlay thread */
Thrd TNetworkPlay;
/*! The handle of t_NetworkShow thread */
Thrd TNetworkShow;
/*! The handle of t_NetworkSend thread */
Thrd TNetworkSend;
/*! The handle of t_Show thread */
Thrd TShow;
/*! The handle of t_Info thread */
Thrd TInfo;
/*! The mutex of Board */
Mut MBoard;
/*! The mutex of Info */
Mut MInfo;
/*! The mutex of Screen */
Mut MScr;
/*! The mutex of Network connecting */
Mut MNetC;
/*! The mutex of Networking */
Mut MNet;
/*! The condition of board */
Cond CBoard;
/*! The condition of info */
Cond CInfo;
/*! The condition of network connecting  */
Cond CNet;
/*! The attr of threads */
Attr AThread;
/*! The window to display board */
WINDOW *BoardWin;
/*! The window to display info */
WINDOW *MenuWin;

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
    strcpy(display[20],"boom");
    for(int i=1;i<20;++i){
        eat[i][20][1]=i-1;
        eat[20][i][1]=i-1;
        eat[i][20][0]=i-2;
        eat[20][i][0]=i-2;
    }
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

void* t_Show(void* arg);
void* t_Info(void* arg);
void* t_Updater(void* arg);

static void s_Accept(dyad_Event *e);
static void s_Init(dyad_Event *e);
static void n_Data(dyad_Event *e);
static void g_Error(dyad_Event *e);
static void n_Init(dyad_Event *e);
static void g_Data(dyad_Event *e);



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
    if(*a==20){
        *a=*b-1;
        *b=*b-2;
        return *a;
    }
    if(*a==*b){
        if(*a>=1&&*b>=1&&*a<=18&&*b<=18){
            *a=*a+1;*b=NA;return *a;
        }else{
            *a=eat[(int)*a][(int)*b][0];
            *b=eat[(int)*a][(int)*b][1];
            return eat[(int)*a][(int)*b][0];
        }
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
bool die();
bool play();
void showBoard(WINDOW* win,int offy,int offx);
void settings();
void welcome();

void c_boom(int y,int x);
int  c_checksum();
void c_currentStr(bool show);
void c_forceQuit();
void c_loadStr(int iptN,FILE* fp,bool info);
void c_readBoard(int from);
void c_readFromDisk(int boards,bool info);
void c_saveBoard(int to,bool jmp);
void c_tryQuit();
int  c_version();
void c_warning(const char* msg);
void c_info(char* msg);
bool c_writeBoardToDisk(char board,bool info);

/// \brief  Show and handle commands inputed by :
/// \return void
void command(){
    char cmd[MAX_BOARD_SIZE*MAX_BOARD_SIZE*4];
    echo();
    //curs_set(1);
    move(0,0);
    clrtoeol();
    attron(A_STANDOUT);
    mvprintw(0,MENU_POSITION_X,":");
    attroff(A_STANDOUT);
    int arg=NA,arg2=NA;
    scanw("%s %d %d",cmd,&arg,&arg2);cmd[15]='\0';
    noecho();
    //curs_set(0);
    if(strcmp(cmd,"r")==0||strcmp(cmd,"read")==0){
        c_readBoard(arg);
    }else if(strcmp(cmd,"s")==0||strcmp(cmd,"save")==0){
        c_saveBoard(arg,true);
    }else if(strcmp(cmd,"s!")==0||strcmp(cmd,"saveto")==0){
        c_saveBoard(arg,false);
    }else if(strcmp(cmd,"q")==0||strcmp(cmd,"quit")==0){
        c_tryQuit();
    }else if(strcmp(cmd,"w")==0||strcmp(cmd,"write")==0){
        c_writeBoardToDisk(NA,true);
    }else if(strcmp(cmd,"wb")==0||strcmp(cmd,"writeboard")==0){
        if(NA==arg){
            c_warning("Librazy don't know which board should be saved");
        }else{
            c_writeBoardToDisk(arg,true);
        }
    }else if(strcmp(cmd,"wq")==0||strcmp(cmd,"writequit")==0){
        if(c_writeBoardToDisk(NA,true))c_forceQuit();
    }else if(strcmp(cmd,"q!")==0||strcmp(cmd,"quit!")==0){
        c_forceQuit();
    }else if(strcmp(cmd,"o")==0||strcmp(cmd,"open")==0){
        c_readFromDisk(arg,true);
    }else if(strcmp(cmd,"boom")==0){
        c_boom(arg,arg2);
    }else{
        char str[1000];
        sprintf(str,"Librazy don't know how to %s,\nresume game now",cmd);
        str[99]='\0';
        c_warning(str);
    }
}
/// \brief  Handle when no empty grid present
/// \return If restart
bool die(){
    c_warning("You are dead!\nPress q to quit or r to restart");
    int ch;
    while((ch=getch())){
        switch(ch){
            case 'r':case 'R':
                return 1;
                break;
            case 'q':case 'Q':
                c_forceQuit();
                break;
        }
    }
    return 0;
}
/// \brief  Handle for main game
/// \return If restart
bool play(){
    if(boardseed[curs]==NA){
        boardseed[curs]=Rando(RAND_MAX);
    }
    srand(boardseed[curs]);
    Clrboard(curs);
    clear();
    pthread_create (&TShow, &AThread, t_Show, NULL);
    keypad(stdscr, TRUE);
    int ch=0;
    int res=NA,lastres=NA;GetRandNums();
    pthread_cond_signal(&CBoard);
    while((ch = getch()) != KEY_F(1)){
        move(0,0);
        clrtoeol();
        pthread_mutex_lock(&MInfo);
        pthread_mutex_lock(&MScr);
        wclear(MenuWin);
        pthread_mutex_unlock(&MScr);
        memset(sinfo,0,sizeof(sinfo));
        pthread_mutex_unlock(&MInfo);
        pthread_cond_signal(&CInfo);
        lastres=res;
        pthread_mutex_lock(&MBoard);
        switch(ch)
        {
            case KEY_LEFT:case 'H':case 'h':
                res=Eat(ELEFT);GetRandNums();
                break;
            case KEY_RIGHT:case 'L':case 'l':
                res=Eat(ERIGHT);GetRandNums();
                break;
            case KEY_UP:case 'K':case 'k':
                res=Eat(EUP);GetRandNums();
                break;
            case KEY_DOWN:case 'J':case 'j':
                res=Eat(EDOWN);GetRandNums();
                break;
            case ':':
                pthread_mutex_unlock(&MBoard);
                command();
                pthread_mutex_lock(&MBoard);
                break;
            case 3:
                pthread_mutex_unlock(&MBoard);
                c_tryQuit();
                pthread_mutex_lock(&MBoard);
                break;
        }
        pthread_mutex_unlock(&MBoard);
        pthread_cond_signal (&CBoard);
        if(res==lastres&&res==0){
            bool c=die();
            pthread_cancel(TShow);
            pthread_cancel(TInfo);
            pthread_cond_signal (&CInfo);
            pthread_join(TShow, NULL);
            pthread_join(TInfo, NULL);
            wclear(BoardWin);
            wclear(MenuWin);
            delwin(BoardWin);
            delwin(MenuWin);
            BoardWin=NULL;
            MenuWin=NULL;
            return c;
        }
    }
    return false;
}

/// \brief  Print the board to screen
/// \param  win The window to draw on
/// \param  offy The y position for the left-up corner
/// \param  offx The x position for the left-up corner
/// \return void
void showBoard(WINDOW* win,int offy,int offx){
    for(int j=0;j!=N;++j){
        for(int i=0;i!=N;++i){
            mvwprintw(win,offy+j*2,offx+i*10,"----------");
            mvwprintw(win,offy+j*2+1,offx+i*10,"|");
            mvwprintw(win,offy+j*2+1,offx+i*10+1,"         ");
            wattron(win,COLOR_PAIR(board[curs][j][i]));
            mvwprintw(win,offy+j*2+1,offx+i*10+5-strlen(Display(board[curs][j][i]))/2,Display(board[curs][j][i]));
            wattroff(win,COLOR_PAIR(board[curs][j][i]));
        }
        mvwprintw(win,offy+j*2,offx+N*10,"-");
        mvwprintw(win,offy+j*2+1,offx+N*10,"|");
    }
    for(int i=0;i!=N;++i){
        mvwprintw(win,offy+N*2,offx+i*10,"----------");
    }
    mvwprintw(win,offy+N*2,offx+N*10,"-");
    wrefresh(win);
}
/// \brief  Print the info to screen
/// \param  win The window to draw on
/// \return void
void showInfo(WINDOW* win){
    if(!iswarn){
        mvwprintw(win,MENU_POSITION_Y,MENU_POSITION_X,sinfo);
    }else{
        wattron(win,COLOR_PAIR(10));
        mvwprintw(win,WARNING_POSITION_Y,WARNING_POSITION_X,sinfo);
        wattroff(win,COLOR_PAIR(10));
    }
    wrefresh(win);
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
    while(1){
        inpN=getch();
        if(inpN>'0'&&inpN<='9')break;
        if(inpN=='n'||inpN=='N'){
            mvprintw(row-3,0,"Networking mode now,enter ip you want to connect\n");clrtoeol();move(row-2,0);echo();
            char ipstr[130]={0};
            while(scanw("%127s",ipstr)!=1){mvprintw(row-3,0,"Networking mode now,enter ip you want to connect!\n");clrtoeol();move(row-2,0);}
            noecho();
            SClient = dyad_newStream();
            printw(ipstr);
            printw(",Enter a number<=9 that you want the board be:");
            while(1){
                inpN=getch();
                if(inpN>'0'&&inpN<='9'){printw("%d",inpN);
                    N=(char)(inpN-'0');break;
                }
            }
            pthread_create (&TInfo, &AThread, t_Info, NULL);
			pthread_mutex_lock(&MNetC);
            connecting=true;
            dyad_addListener(SClient, DYAD_EVENT_DATA, n_Data, NULL);
            dyad_addListener(SClient, DYAD_EVENT_ERROR, g_Error, NULL);
            dyad_connect(SClient,ipstr, 2048);
            return;
        }
        if(inpN=='s'||inpN=='S'){
            move(row-3,0);clrtoeol();
            mvprintw(row-3,0,"Serving mode now,listening port 2048\n");clrtoeol();move(row-2,0);
            refresh();
            SServ = dyad_newStream();
            pthread_create (&TInfo, &AThread, t_Info, NULL);
			pthread_mutex_lock(&MNetC);
            connecting=true;
            dyad_addListener(SServ, DYAD_EVENT_ERROR, g_Error, NULL);
            dyad_addListener(SServ, DYAD_EVENT_ACCEPT, s_Accept, NULL);
            dyad_listenEx(SServ, "0.0.0.0",2048,1);//Force IPv4
            return;
        }
    }
    N=(char)(inpN-'0');
    printw("%d\nThe board will be %d.Press any key and rock on!",N,N);
    getch();
    pthread_create (&TInfo, &AThread, t_Info, NULL);
}
/// \brief  Make (y,x) a 'boom'
/// \return void
void c_boom(int y,int x){
    if(y==NA){y=Rando(N);}
    if(x==NA){x=Rando(N);}
    board[curs][y%N][x%N]=20;
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
        rowsum|=(r<<5)+r;
        rowsum^=r;
    }
    for(int j=0;j!=N;++j){
        c=0;
        for(int i=0;i!=N;++i){
            c+=((int)board[curs][i][j])^170;
            c^=cs_pwd[(i*N+j)%PWD_LEN];
        }
        colsum+=(c<<4)+c;
        colsum^=c;
    }
    checksum=((rowsum+colsum)+((colsum^rowsum)<<4))^(((rowsum+colsum)<<4)+(colsum^rowsum));
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
    dyad_shutdown();
    clear();
    endwin();
    exit(0);
}
/// \brief  Load the string representing saved board
/// \param  iptN The N in the saved game
/// \param  fp The file stream to read from
/// \return void
void c_loadStr(int iptN,FILE* fp,bool info){
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
    }
    if(c_checksum()==checksum){
        if(info)c_info("Game progress loaded successful!");
    }else{
        Clrboard(curs);
        c_warning("Librazy found you are cheating!");
    }
}
/// \brief  Read the saved board
/// \param  from The number of board to read from
/// \return void
void c_readBoard(int from){
    if(from==NA){
        from=abs((16+curs-1)%MAX_BOARD_NUM);
    }
    from%=MAX_BOARD_NUM;
    boardseed[curs]=Rando(RAND_MAX);
    char str[1000];
    if(boardseed[from]==NA){
        sprintf(str,"Librazy found board#%d empty",from);
        c_warning(str);
        return;
    }
    curs=from;
    srand(boardseed[from]);
    sprintf(str,"Load board#%d",curs);
    c_info(str);
}
/// \brief  Read the saved file
/// \param  boards The number of the saved board.NA for not to use
/// \return void
void c_readFromDisk(int boards,bool info){
    char name[20];
    int ver=c_version();
    if(boards!=NA){
        boards=abs(boards%MAX_BOARD_NUM);
        sprintf(name,"2048.%d.%X.save",boards,ver);
    }else{
        sprintf(name,"2048.%X.save",ver);
        boards=curs;
    }
    boards=abs(boards%MAX_BOARD_NUM);
    FILE *fp;
    if((fp=fopen(name,"r"))) {
        int iptN;
        int iptVer=0;
        fscanf(fp,"%X",&iptVer);
        char w[1024];
        if(iptVer!=ver){
            sprintf(w,"Librazy found that the game's version dosen't match!\nYour version:%X, saved:%X",ver,iptVer);
            c_warning(w);
            return ;
        }
        sprintf(w,"Opening %s",name);
        if(info)c_info(w);
        fscanf(fp,"%d",&iptN);
        c_loadStr(iptN%MAX_BOARD_SIZE,fp,info);
    }else{
        char str[1000];
        sprintf(str,"Librazy don't know how to open %s,\n read file failed",name);
        c_warning(str);
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
    to=abs(to%MAX_BOARD_NUM);
    boardseed[to]=boardseed[curs]=Rando(RAND_MAX);
    srand(boardseed[curs]);
    memcpy(board[to],board[curs],sizeof(char)*MAX_BOARD_SIZE*MAX_BOARD_SIZE);
    move(MENU_POSITION_Y,MENU_POSITION_X);
    clrtoeol();
    char str[1000];
    if(jmp){
        sprintf(str,"Save board#%d, current#%d",curs,to);
        c_info(str);
        curs=to;
    }else{
        sprintf(str,"Save board#%d to #%d",curs,to);
        c_info(str);
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
    c_warning("You game progress won't be saved!\nPress q to quit, others to resume");
    int ch=0;
    ch=getch();
    if(ch=='q'||ch=='Q'){
        c_forceQuit();
    }
    c_info("");
}
/// \brief  Write the board to disk
/// \param  boards The number of board to save
/// \return Whether the file is saved successfully
bool c_writeBoardToDisk(char boards,bool info){
    char name[20];
    int ver=c_version();
    if(boards!=NA){
        boards=abs(boards%MAX_BOARD_NUM);
        sprintf(name,"2048.%d.%X.save",boards,ver);
    }else{
        sprintf(name,"2048.%X.save",ver);
        boards=curs;
    }
    boards=abs(boards%MAX_BOARD_NUM);
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
        char str[1000];
        sprintf(str,"Saved board#%d at %s",boards,name);
        if(info)c_info(str);
        fclose(fp);
        return true;
    }else{
        char str[1000];
        sprintf(str,"Librazy don't know how to open %s,\nsave failed",name);
        c_warning(str);
        fclose(fp);
        return false;
    }
}
/// \brief  To print a warning to screen
/// 
///          The function storage the message in a buffer and call up the TInfo thread to print it
/// \param  msg The string to print
/// \return void
void c_warning(const char* msg){
    pthread_mutex_lock(&MInfo);
    memset(sinfo,0,sizeof(sinfo));
    strcpy(sinfo,msg);
    iswarn=true;
    pthread_mutex_unlock(&MInfo);
    pthread_cond_signal (&CInfo);
}
/// \brief  To print a info to screen
/// 
///          The function storage the message in a buffer and call up the TInfo thread to print it
/// \param  msg The string to print
/// \return void
void c_info(char* msg){
    pthread_mutex_lock(&MInfo);
    memset(sinfo,0,sizeof(sinfo));
    strcpy(sinfo,msg);
    iswarn=false;
    pthread_mutex_unlock(&MInfo);
    pthread_cond_signal (&CInfo);
}
/// \brief  The thread to print infos to screen
/// \param  arg Void
/// \return void* NULL
void* t_Info(void* arg){
    if(MenuWin!=NULL)delwin(MenuWin);
    MenuWin = newwin(2, col,row-2, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS , NULL);
    int oldtype;
    while(1){;
        pthread_mutex_lock(&MInfo);
        pthread_cond_wait (&CInfo, &MInfo);
        pthread_testcancel();
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
        pthread_mutex_lock(&MScr);
        wclear(MenuWin);
        showInfo(MenuWin);
        pthread_mutex_unlock(&MScr);
        pthread_mutex_unlock(&MInfo);
        pthread_setcanceltype(oldtype, NULL);
    }
    return NULL;
}
/// \brief  The thread to print Board to screen
/// \param  arg Void
/// \return void* NULL
void* t_Show(void* arg){
    if(BoardWin!=NULL)delwin(BoardWin);
    BoardWin = newwin(row-6, col, 1, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS , NULL);
    int oldtype;
    while(1){
        pthread_testcancel();
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
        pthread_mutex_lock(&MBoard);
        pthread_testcancel();
        pthread_mutex_lock(&MScr);
        wclear(BoardWin);
        showBoard(BoardWin,0,5);
        pthread_mutex_unlock(&MScr);
        pthread_cond_wait (&CBoard, &MBoard);
        pthread_mutex_unlock(&MBoard);
        pthread_setcanceltype(oldtype, NULL);
    }
    return NULL;
}
/// \brief  The thread to print Board to screen
/// \param  arg Void
/// \return void* NULL
void* t_NetworkShow(void* arg){
    if(BoardWin!=NULL)delwin(BoardWin);
    BoardWin = newwin(row-6, col, 1, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS , NULL);
    int oldtype;
    while(1){
        pthread_testcancel();
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
        pthread_mutex_lock(&MBoard);
        pthread_testcancel();
        pthread_mutex_lock(&MScr);
        wclear(BoardWin);
        curs=0;
        showBoard(BoardWin,0,5);
        curs=1;
        showBoard(BoardWin,0,col/2);
        pthread_mutex_unlock(&MScr);
        pthread_cond_wait (&CBoard, &MBoard);
        pthread_mutex_unlock(&MBoard);
        pthread_setcanceltype(oldtype, NULL);
    }
    return NULL;
}
/// \brief  The thread to print Board to screen
/// \param  arg Void
/// \return void* NULL
void* t_NetworkSend(void* arg){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS , NULL);
    int oldtype;
    while(1){
        pthread_testcancel();
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
        pthread_mutex_lock(&MNet);
		pthread_cond_wait (&CNet, &MNet);
        pthread_testcancel();
        pthread_mutex_lock(&MBoard);
		
        c_writeBoardToDisk(0,false);
        FILE *fp;
        char name[20];
        int ver=c_version();
        sprintf(name,"2048.0.%X.save",ver);
        if((fp=fopen(name,"r"))) {
            int c;
            int count = 500;
            while (count--) {
                if ((c = fgetc(fp)) != EOF) {
                    dyad_write(SGaming, &c, 1);
                } else {
                    dyad_update();
                    break;
                }
            }
			c_warning("Send");
        }else{
            dyad_writef(SGaming, "ERR");
        }
        
        pthread_mutex_unlock(&MBoard);
        pthread_mutex_unlock(&MNet);
        pthread_setcanceltype(oldtype, NULL);
    }
    return NULL;
}
/// \brief  The thread to print Board to screen
/// \param  arg Void
/// \return void* NULL
void* t_NetworkPlay(void* arg){
    pthread_mutex_lock(&MNetC);
    pthread_mutex_lock(&MBoard);
    curs=0;
    if(boardseed[curs]==NA){
        boardseed[curs]=Rando(RAND_MAX);
    }
    srand(boardseed[curs]);
    Clrboard(curs);
    clear();
    pthread_create (&TNetworkShow, &AThread, t_NetworkShow , NULL);
    pthread_create (&TNetworkSend, &AThread, t_NetworkSend , NULL);
    keypad(stdscr, TRUE);
    int ch=0;
    int res=NA,lastres=NA;GetRandNums();
    pthread_mutex_unlock(&MBoard);
    pthread_cond_signal(&CBoard);
    while((ch = getch()) != KEY_F(1)){
        move(0,0);
        clrtoeol();
        pthread_mutex_lock(&MInfo);
        pthread_mutex_lock(&MScr);
        wclear(MenuWin);
        pthread_mutex_unlock(&MScr);
        memset(sinfo,0,sizeof(sinfo));
        pthread_mutex_unlock(&MInfo);
        pthread_cond_signal(&CInfo);
        lastres=res;
        pthread_mutex_lock(&MBoard);
        curs=0;
        switch(ch)
        {
            case KEY_LEFT:case 'H':case 'h':
                res=Eat(ELEFT);GetRandNums();
                break;
            case KEY_RIGHT:case 'L':case 'l':
                res=Eat(ERIGHT);GetRandNums();
                break;
            case KEY_UP:case 'K':case 'k':
                res=Eat(EUP);GetRandNums();
                break;
            case KEY_DOWN:case 'J':case 'j':
                res=Eat(EDOWN);GetRandNums();
                break;
            case 3:
                pthread_mutex_unlock(&MBoard);
                c_tryQuit();
                pthread_mutex_lock(&MBoard);
                break;
        }
        pthread_mutex_unlock(&MBoard);
        pthread_cond_signal (&CBoard);
        pthread_cond_signal (&CNet);
        if(res==lastres&&res==0){
			c_warning("You are dead!");
            pthread_cancel(TShow);
            pthread_cancel(TInfo);
            pthread_cond_signal (&CInfo);
            pthread_join(TShow, NULL);
            pthread_join(TInfo, NULL);
            wclear(BoardWin);
            wclear(MenuWin);
            delwin(BoardWin);
            delwin(MenuWin);
            BoardWin=NULL;
            MenuWin=NULL;
            return NULL;
        }
    }
	pthread_mutex_unlock(&MNetC);
    return NULL;
}

static void s_Accept(dyad_Event *e) {
    connecting=false;
    dyad_writef(e->remote,"ACC\n");
    c_warning("C0");
    dyad_removeAllListeners(SServ, DYAD_EVENT_ACCEPT);
    dyad_addListener(e->remote, DYAD_EVENT_DATA, s_Init, NULL);
}
static void s_Init(dyad_Event *e) {
    int cliver=0,val=0,cliN;
    val=sscanf(e->data,"VERSION %X %d",&cliver,&cliN);
    if(cliver!=c_version()||val!=2){
        dyad_writef(e->stream,"INCOMP\n");
        c_warning("Incompatiable version!Quit.");
        dyad_update();
        dyad_end(SServ);
        getch();
        c_forceQuit();
    }else{
        SGaming=e->stream;
        N=cliN;
        isnetworking=true;
        dyad_writef(e->stream,"OK\n");dyad_update();
        dyad_removeAllListeners(e->stream, DYAD_EVENT_DATA);
        dyad_addListener(e->stream, DYAD_EVENT_DATA, g_Data, NULL);
		pthread_mutex_unlock(&MNetC);
    }
}
static void n_Data(dyad_Event *e) {
    connecting=false;
    c_warning("nD0");
    int ver=c_version();
    dyad_writef(e->stream, "VERSION %X %d\n",ver,N);dyad_update();
    c_warning("nD1");
    dyad_removeAllListeners(e->stream, DYAD_EVENT_CONNECT);
    c_warning("nD2");
    dyad_addListener(e->stream, DYAD_EVENT_DATA, n_Init, NULL);
    c_warning("nD3");
}
static void n_Init(dyad_Event *e) {
    if (!memcmp(e->data, "INCOMP", 6)) {
        c_warning("nI0");
        c_warning("Incompatiable version!Quit.");
        dyad_end(e->stream);
        getch();
        c_forceQuit();
    }else{
        c_warning("nI1");
        SGaming=e->stream;
        isnetworking=true;
        c_warning("nI2");
        dyad_removeAllListeners(e->stream, DYAD_EVENT_DATA);
        dyad_addListener(e->stream, DYAD_EVENT_DATA, g_Data, NULL);
		pthread_mutex_unlock(&MNetC);
    }
}
static void g_Data(dyad_Event *e) {
    c_warning("C3");
    FILE *fp;
	char name[20],v[10];
    int ver=c_version();
    sprintf(name,"2048.1.%X.save",ver);
    sprintf(v,"%X",ver);
	if(NULL!=strstr((char*)e->data,v)){
		if((fp=fopen(name,"w+"))) {
			fprintf(fp,e->data);
		}
		fclose(fp);
		pthread_mutex_lock(&MBoard);
		curs=1;
		c_readFromDisk(1,false);
		pthread_mutex_unlock(&MBoard);
	}
	pthread_cond_signal(&CBoard);
}
static void g_Error(dyad_Event *e) {
    connecting=false;
    c_warning(e->msg);
    dyad_end(e->remote);
    c_forceQuit();
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
    dyad_init();
    pthread_attr_init (&AThread);
    pthread_attr_setdetachstate (&AThread, PTHREAD_CREATE_JOINABLE);
    bool cho=true;
    while(cho){
        pthread_mutex_init (&MBoard, NULL);
        pthread_cond_init (&CBoard, NULL);
        pthread_mutex_init (&MInfo, NULL);
        pthread_cond_init (&CInfo, NULL);
        pthread_mutex_init (&MScr, NULL);
        pthread_mutex_init (&MNet, NULL);
        pthread_mutex_init (&MNetC, NULL);
        pthread_cond_init (&CNet, NULL);
        Clrboard(curs);
        welcome();
        if(connecting){
			dyad_setUpdateTimeout(0.05);
			pthread_create (&TNetworkPlay, &AThread, t_NetworkPlay, NULL);
            while (dyad_getStreamCount() > 0) {
				pthread_mutex_lock(&MNet);
                dyad_update();
				pthread_mutex_unlock(&MNet);
				usleep(10000);
            }
        }else{
            cho=play();
        }
        pthread_mutex_destroy (&MScr);
        pthread_mutex_destroy (&MInfo);
        pthread_cond_destroy (&CInfo);
        pthread_mutex_destroy (&MBoard);
        pthread_cond_destroy (&CBoard);
        pthread_mutex_destroy (&MNet);
        pthread_mutex_destroy (&MNetC);
        pthread_cond_destroy (&CNet);
        if(isnetworking){c_forceQuit();}
    }
    c_forceQuit();
    return 0;
}
