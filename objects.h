//Object IDS
enum IDS{PLAYER, BULLET, ENEMY, BOSS};
enum MODES{NORMAL, TIMED, SURVIVAL};

//Our Player
struct SpaceShip
{
	int ID;
	int x;
	int y;
	int lives;
	int speed;
	int boundx;
	int boundy;
	int score;
};

struct Bullet
{
	int ID;
	int x;
	int y;
	bool live;
	int speed;
};

struct Comet
{
	int ID;
	int x;
	int y;
	bool live;
	int speed;
	int boundx;
	int boundy;
};

struct Boss
{
	int ID;
	int x;
	int y;
	int hp;
	bool live;
	int speed;
	int boundx;
	int boundy;
};

int NG_R= 0;
int NG_B= 255; 
int TG_R= 0;
int TG_B= 255;
int SG_R= 0;
int SG_B= 255;
int OP1_R = 0;
int OP1_B=255; 
int PT_R = 0; 
int PT_B = 255;
int curr_menu = 0;
int dificult_current_menu = 0;
int GAME_MODE = 0;
int ND_R= 0;
int ND_B= 255; 
int MD_R= 0;
int MD_B= 255;
int HD_R= 0;
int HD_B= 255;
int EX_R = 0;
int remaining_time = 120;
bool displayMenu = true;
bool RankingMenu = false;
double RemainingTimer=0;