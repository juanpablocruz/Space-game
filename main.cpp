#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"

const int WIDTH = 800;
const int HEIGHT = 400;
const int NUM_BULLETS = 5;
const int NUM_COMETS = 10;
const int NUM_BOSSESS = 5;
int currentBoss = 0;
int Highest_pts = 0;
int current_level = 1;
int Game_level = 1;
enum Keys{UP, DOWN, LEFT, RIGHT, SPACE};

ALLEGRO_FONT *font18 = NULL;
ALLEGRO_FONT *font20 = NULL;
ALLEGRO_FONT *font25 = NULL;

double levelTimer;
bool keys[5] = {false, false, false, false, false};
bool newLevel = false;
bool paused;

void InitShip(SpaceShip &ship);
void DrawShip(SpaceShip &ship);
void MoveShipUp(SpaceShip &ship);
void MoveShipDown(SpaceShip &ship);
void MoveShipLeft(SpaceShip &ship);
void MoveShipRight(SpaceShip &ship);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip &ship);
void UpdateBullet(Bullet bullet[], int size);
void CollideBullet(Bullet bullet[], int size, Comet comets[], int cSize, SpaceShip &ship,
					Boss bosses[], int currentBoss);

void InitComet(Comet comets[], int size);
void IncreaseCometSpeed(Comet comets[], int size);
void DrawComet(Comet comets[], int size);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size);
void CollideComet(Comet comets[], int size, SpaceShip &ship);
void DestroyComets(Comet comets[], int size);

void InitBoss(Boss bosses[], int size);
void SpawnBoss(Boss bosses[], int i);
void DrawBoss(Boss bosses[], int currentBoss);
void MovetBoss(Boss bosses[], int currentBoss);
void UpdateBoss(Boss bosses[],int i);
void CollideBoss(Boss bosses[], int i, SpaceShip &ship);

void printMenu(int NG_R,int NG_B, int TG_R,int TG_B, int SG_R,int SG_B, int OP1_R, int OP1_B, int PT_R, int PT_B,ALLEGRO_BITMAP *title);
void printScore(SpaceShip &ship, int Highest_pts);
void printPause();
void printEndGame();
void changeMenu(int current);
void printDificultMenu(int ND_R,int ND_B, int MD_R,int MD_B, int HD_R,int HD_B, int EX_R,ALLEGRO_BITMAP *title);

void Restart(SpaceShip &ship,Comet (&comets)[NUM_COMETS],Boss &boss,int &current_level,
				bool &isGameOver,int &currentBoss);
void changeDificultMenu(int dificult_current_menu);
void SetDificult(int Game_Dificult);
void printRanking(ALLEGRO_BITMAP *background);

int main(void){
	bool done = false;
	bool gameStarted = false;
	bool redraw = true;
	bool dificultMenu = false;
	const int FPS = 60;
	bool isGameOver = false;
	int lifewidth = 0;
	int lifeheight = 0;

	levelTimer = 0;
	paused = false;

	SpaceShip ship;
	Bullet bullets[NUM_BULLETS];
	Comet comets[NUM_COMETS];
	Boss bosses[NUM_BOSSESS];

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *life = NULL;
	ALLEGRO_BITMAP *background = NULL;
	ALLEGRO_BITMAP *title = NULL;
	
	if(!al_init())
		return -1;
	display = al_create_display(WIDTH,HEIGHT);
	al_set_window_title(display,"Ender's Game");

	if(!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	srand(time(NULL));
	InitShip(ship);
	InitBullet(bullets, NUM_BULLETS);
	InitComet(comets, NUM_COMETS);
	InitBoss(bosses, NUM_BOSSESS);
	
	font18 = al_load_font("arial.ttf", 18, 0);
	font20 = al_load_font("arial.ttf", 20, 0);
	font25 = al_load_font("arial.ttf", 25, 0);

	life = al_load_bitmap("life.png");
	background = al_load_bitmap("stars.jpg");
	title = al_load_bitmap("titulo.png");

	lifewidth = al_get_bitmap_width(life);
	lifeheight = al_get_bitmap_height(life);
	

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_start_timer(timer);
	while(!done){
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER){
			redraw = true;
			if(keys[UP])
				MoveShipUp(ship);
			if(keys[DOWN])
				MoveShipDown(ship);
			if(keys[LEFT])
				MoveShipLeft(ship);
			if(keys[RIGHT])
				MoveShipRight(ship);

			if(!isGameOver&!newLevel&gameStarted){
				UpdateBullet(bullets, NUM_BULLETS);
				if(!bosses[currentBoss].live)
					StartComet(comets, NUM_COMETS);
				else
					CollideBoss(bosses, currentBoss, ship);
				UpdateComet(comets, NUM_COMETS);
				CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS, ship, bosses, currentBoss);
				CollideComet(comets, NUM_COMETS, ship);
				UpdateBoss(bosses, currentBoss);

				if(ship.lives <= 0){
					isGameOver = true;
					if(ship.score > Highest_pts)Highest_pts=ship.score;
				}
			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			done = true;
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
			switch(ev.keyboard.keycode){
			case ALLEGRO_KEY_ESCAPE:
				if(gameStarted)
					gameStarted = false;
				else
					done = true;
				break;
			case ALLEGRO_KEY_UP:
				if(gameStarted)
					keys[UP] = true;
				else{
					if(displayMenu){
						if(curr_menu==0)
							curr_menu=4;
						else
							curr_menu--;
					}
					else if(dificultMenu){
						if(dificult_current_menu == 0)
							dificult_current_menu = 3;
						else
							dificult_current_menu--;
					}
				}
				break;
			case ALLEGRO_KEY_DOWN:
				if(gameStarted)
					keys[DOWN] = true;
				else{
					if(displayMenu){
						if(curr_menu==4)
							curr_menu=0;
						else
							curr_menu++;
					}
					else if(dificultMenu){
						if(dificult_current_menu == 3)
							dificult_current_menu = 0;
						else
							dificult_current_menu++;
					}
				}
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				FireBullet(bullets, NUM_BULLETS, ship);
				break;
			case ALLEGRO_KEY_P:
				paused = !paused;
				break;
			case ALLEGRO_KEY_R:
				if(paused){
					Restart(ship,comets,bosses[currentBoss],current_level,isGameOver,currentBoss);
					paused = false;
					if(GAME_MODE == TIMED)remaining_time=120;
				}
			case ALLEGRO_KEY_ENTER:
				if(gameStarted){
					if(isGameOver){
						Restart(ship,comets,bosses[currentBoss],current_level,isGameOver,currentBoss);
						if(GAME_MODE == TIMED)remaining_time=120;
					}
				}
				else{
					if(displayMenu){
						switch(curr_menu){
						case 0:
							gameStarted=true;
							Restart(ship,comets,bosses[currentBoss],current_level,isGameOver,currentBoss);
							break;
						case 1:
							Restart(ship,comets,bosses[currentBoss],current_level,isGameOver,currentBoss);
							gameStarted=true;
							RemainingTimer = al_get_time();
							remaining_time=120;
							break;
						case 2:
							Restart(ship,comets,bosses[currentBoss],current_level,isGameOver,currentBoss);
							gameStarted=true;
							break;
						case 3:
							dificultMenu = true;
							displayMenu = false;
							break;
						case 4:
							RankingMenu = true;
							displayMenu = false;
							break;
						default:
							break;
						}
					}
					else if(dificultMenu){
						switch(dificult_current_menu){
						case 0:
							dificultMenu = false;
							displayMenu = true;
							SetDificult(1);
							break;
						case 1:
							dificultMenu = false;
							displayMenu = true;
							SetDificult(2);
							break;
						case 2:
							dificultMenu = false;
							displayMenu = true;
							SetDificult(4);
							break;
						case 3:
							dificultMenu = false;
							displayMenu = true;
							dificult_current_menu = 0;
							break;
						}
					}
					else if(RankingMenu){
						RankingMenu = false;
						displayMenu = true;
						break;
					}
				}
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP){
			switch(ev.keyboard.keycode){
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				break;
			}
		}
		if(redraw && al_is_event_queue_empty(event_queue)){
			redraw = false;
			if(gameStarted){
				if(!paused){
					if(!isGameOver && !newLevel){
						al_draw_scaled_bitmap(background,0,0,al_get_bitmap_width(background),al_get_bitmap_height(background),
							0,0,WIDTH,HEIGHT,0);
						DrawShip(ship);
						DrawBullet(bullets, NUM_BULLETS);
						DrawComet(comets, NUM_COMETS);
						DrawBoss(bosses, currentBoss);

						for(int i=0; i!=ship.lives;i++){
							al_draw_scaled_bitmap(life, 0,0, lifewidth, lifeheight, 105+(i*25), 5, lifewidth*0.2 , lifeheight*0.2, 0);
						}
						al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "Player lives: ");
						al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 25, 0, "Score: %i", ship.score);

						if(GAME_MODE == TIMED){			
							al_draw_textf(font18, al_map_rgb(255, 0, 255), WIDTH/2, 5, 0, "Remaining Time %i",remaining_time);
							  if(1 == ((int)RemainingTimer)%2){
								  remaining_time--;
								  RemainingTimer = al_get_time();
							  }
							  if(remaining_time==0)isGameOver=true;
						}
					}
					else if(isGameOver && !newLevel){
						printScore(ship,Highest_pts);
					}
					else{
						printEndGame();
					}
				}
				else{
					printPause();
				}
			}
			else{
				if(displayMenu){
					changeMenu(curr_menu);
					printMenu(NG_R,NG_B,TG_R,TG_B,SG_R,SG_B,OP1_R,OP1_B,PT_R,PT_B,title);
				}
				else if(dificultMenu){
					changeDificultMenu(dificult_current_menu);
					printDificultMenu(ND_R,ND_B,MD_R,MD_B,HD_R,HD_B,EX_R,title);
				}
				else if(RankingMenu){
					printRanking(background);
				}
			}
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}
	al_destroy_bitmap(life);
	al_destroy_bitmap(background);
	al_destroy_bitmap(title);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(font18);
	al_destroy_font(font20);
	al_destroy_font(font25);
	al_destroy_display(display);						//destroy our display object

	return 0;
}

void InitShip(SpaceShip &ship)
{
	ship.x = 20;
	ship.y = HEIGHT / 2;
	ship.ID = PLAYER;
	ship.speed = 5;
	ship.boundx = 6;
	ship.boundy = 7;
	ship.score = 0;
	
	switch(GAME_MODE){
	case SURVIVAL:
		ship.lives = 1;
		break;
	default:
		ship.lives = 3;

		break;
	}
}
void DrawShip(SpaceShip &ship)
{
	al_draw_filled_rectangle(ship.x, ship.y - 9, ship.x + 10, ship.y - 7, al_map_rgb(255, 0, 0));
	al_draw_filled_rectangle(ship.x, ship.y + 9, ship.x + 10, ship.y + 7, al_map_rgb(255, 0, 0));

	al_draw_filled_triangle(ship.x - 12, ship.y - 17, ship.x +12, ship.y, ship.x - 12, ship.y + 17, al_map_rgb(0, 255, 0));
	al_draw_filled_rectangle(ship.x - 12, ship.y - 2, ship.x +15, ship.y + 2, al_map_rgb(0, 0, 255));
}
void MoveShipUp(SpaceShip &ship)
{
	ship.y -= ship.speed;
	if(ship.y < 0)
		ship.y = 0;
}
void MoveShipDown(SpaceShip &ship)
{
	ship.y += ship.speed;
	if(ship.y > HEIGHT)
		ship.y = HEIGHT;
}
void MoveShipLeft(SpaceShip &ship)
{
	ship.x -= ship.speed;
	if(ship.x < 0)
		ship.x = 0;
}
void MoveShipRight(SpaceShip &ship)
{
	ship.x += ship.speed;
	if(ship.x > 300)
		ship.x = 300;
}

void InitBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}
void DrawBullet(Bullet bullet[], int size)
{
	for( int i = 0; i < size; i++)
	{
		if(bullet[i].live)
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));
	}
}
void FireBullet(Bullet bullet[], int size, SpaceShip &ship)
{
	for( int i = 0; i < size; i++)
	{
		if(!bullet[i].live)
		{
			bullet[i].x = ship.x + 17;
			bullet[i].y = ship.y;
			bullet[i].live = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(bullet[i].live)
		{
			bullet[i].x += bullet[i].speed;
			if(bullet[i].x > WIDTH)
				bullet[i].live = false;
		}
	}
}
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, SpaceShip &ship,
	Boss bosses[], int current)
{
	for(int i = 0; i < bSize; i++)
	{
		if(bullet[i].live)
		{
			for(int j =0; j < cSize; j++)
			{
				if(comets[j].live)
				{
					if(bullet[i].x > (comets[j].x - comets[j].boundx) &&
						bullet[i].x < (comets[j].x + comets[j].boundx) &&
						bullet[i].y > (comets[j].y - comets[j].boundy) &&
						bullet[i].y < (comets[j].y + comets[j].boundy))
					{
						bullet[i].live = false;
						comets[j].live = false;

						ship.score++;
						if(comets[j].speed != (Game_level+4))
							if(ship.score%15==0)IncreaseCometSpeed(comets,cSize);
						if(ship.score%15==0){
							if((current + 1)>= NUM_BOSSESS){
								current = 0;
							} 
							SpawnBoss(bosses, current);
						}
					}
				}
			}
			if(bosses[current].live){
				if(bullet[i].x > (bosses[current].x - bosses[current].boundx) &&
						bullet[i].x < (bosses[current].x + bosses[current].boundx) &&
						bullet[i].y > (bosses[current].y - bosses[current].boundy) &&
						bullet[i].y < (bosses[current].y + bosses[current].boundy))
					{
						bullet[i].live = false;
						if(--bosses[current].hp<=0){
							bosses[current].live = false;
							ship.score+=5;
							bosses[current].hp = 5;
							currentBoss++;
							DestroyComets(comets,cSize);
							levelTimer = al_get_time();
							newLevel = true;
							current_level++;
						}
					}
			}
		}
	}
}
void DestroyComets(Comet comets[], int size){
	for(int i = 0; i < size; i++)
	{
		comets[i].live = false;
	}	
}
void InitComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		comets[i].ID = ENEMY;
		comets[i].live = false;
		comets[i].speed = Game_level;
		comets[i].boundx = 18;
		comets[i].boundy = 18;
	}
}
void IncreaseCometSpeed(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		comets[i].speed++;
	}	
}
void DrawComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(comets[i].live)
		{
			al_draw_filled_circle(comets[i].x, comets[i].y, 20, al_map_rgb(255, 0, 0));
		}
	}
}
void StartComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(!comets[i].live)
		{
			if(rand() % 500 == 0)
			{
				comets[i].live = true;
				comets[i].x = WIDTH;
				comets[i].y = 30 + rand() % (HEIGHT - 60);

				break;
			}
		}
	}
}
void UpdateComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(comets[i].live)
		{
			comets[i].x -= comets[i].speed;
		}
	}
}
void CollideComet(Comet comets[], int cSize, SpaceShip &ship)
{
	for(int i = 0; i < cSize; i++)
	{
		if(comets[i].live)
		{
			if(comets[i].x - comets[i].boundx < ship.x + ship.boundx &&
				comets[i].x + comets[i].boundx > ship.x - ship.boundx &&
				comets[i].y - comets[i].boundy < ship.y + ship.boundy &&
				comets[i].y + comets[i].boundy > ship.y - ship.boundy)
			{
				ship.lives--;
				comets[i].live = false;
			}
			else if(comets[i].x < 0)
			{
				comets[i].live = false;
				ship.lives--;
			}
		}
	}
}
void InitBoss(Boss bosses[],int size){
	for(int i = 0; i < size; i++)
	{
		bosses[i].ID = BOSS;
		bosses[i].live = false;
		bosses[i].speed = 4;
		bosses[i].boundx = 38;
		bosses[i].boundy = 38;
		bosses[i].hp = 5;
	}
}
void SpawnBoss(Boss bosses[], int i){
	if(!bosses[i].live)
		{
			bosses[i].live = true;
			bosses[i].x = WIDTH;
			bosses[i].y = HEIGHT / 2;
		}
}
void DrawBoss(Boss bosses[], int currentBoss){
	if(bosses[currentBoss].live)
		al_draw_filled_circle(bosses[currentBoss].x, bosses[currentBoss].y, 40, al_map_rgb((155*bosses[currentBoss].x)%255, (255*bosses[currentBoss].x)%255, 200*(155*bosses[currentBoss].x)%255));
}
void UpdateBoss(Boss bosses[],int i){
	if(bosses[i].live)
		{
			bosses[i].x -= bosses[i].speed;
		}
}
void CollideBoss(Boss bosses[], int i, SpaceShip &ship){
	if(bosses[i].live)
		{
			if(bosses[i].x - bosses[i].boundx < ship.x + ship.boundx &&
				bosses[i].x + bosses[i].boundx > ship.x - ship.boundx &&
				bosses[i].y - bosses[i].boundy < ship.y + ship.boundy &&
				bosses[i].y + bosses[i].boundy > ship.y - ship.boundy)
			{
				ship.lives--;
				bosses[i].live = false;
			}
			else if(bosses[i].x < 0)
			{
				bosses[i].live = false;
				ship.lives--;
			}
		}	
}

void printMenu(int NG_R,int NG_B, int TG_R,int TG_B, int SG_R,int SG_B, int OP1_R, int OP1_B, int PT_R, int PT_B, ALLEGRO_BITMAP *title){
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_scaled_bitmap(title,0,0,al_get_bitmap_width(title),al_get_bitmap_height(title),
							 WIDTH / 2-(al_get_bitmap_width(title)),(HEIGHT / 2)-140,
							 al_get_bitmap_width(title)*2,al_get_bitmap_height(title)*2,0);

	al_draw_textf(font20, al_map_rgb(NG_R, 0, NG_B), WIDTH / 2, (HEIGHT / 2)-50, ALLEGRO_ALIGN_CENTRE, "Normal Game");
	al_draw_textf(font20, al_map_rgb(TG_R, 0, TG_B), WIDTH / 2, (HEIGHT / 2)-25, ALLEGRO_ALIGN_CENTRE, "Timed Game");
	al_draw_textf(font20, al_map_rgb(SG_R, 0, SG_B), WIDTH / 2, (HEIGHT / 2), ALLEGRO_ALIGN_CENTRE, "Survival Game");
	al_draw_textf(font20, al_map_rgb(OP1_R, 0, OP1_B), WIDTH / 2, (HEIGHT / 2)+25, ALLEGRO_ALIGN_CENTRE, "Change Dificult");
	al_draw_textf(font20, al_map_rgb(PT_R, 0, PT_B), WIDTH / 2, (HEIGHT / 2)+50, ALLEGRO_ALIGN_CENTRE, "Highest Scores");
}
void printScore(SpaceShip &ship, int Highest_pts){
	al_draw_textf(font25, al_map_rgb(255, 255, 0), WIDTH / 2, (HEIGHT / 2)-40, ALLEGRO_ALIGN_CENTRE, "HIGHEST SCORE %i.", Highest_pts);
	al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, (HEIGHT / 2)-4, ALLEGRO_ALIGN_CENTRE, "Game Over. Final Score %i.", ship.score);
	al_draw_textf(font18, al_map_rgb(0, 255, 255), WIDTH / 2, (HEIGHT / 2)+20, ALLEGRO_ALIGN_CENTRE, "Press ENTER to RETRY");	
}
void printPause(){
	al_draw_textf(font25, al_map_rgb(255, 255, 0), WIDTH / 2, (HEIGHT / 2)-40, ALLEGRO_ALIGN_CENTRE, "PAUSED");
	al_draw_textf(font18, al_map_rgb(255, 0, 0), WIDTH / 2, (HEIGHT / 2)+140, ALLEGRO_ALIGN_CENTRE, "Press R to Restart the Game");
}
void printEndGame(){
	al_draw_textf(font25, al_map_rgb(255, 255, 0), WIDTH / 2, (HEIGHT / 2)-40, ALLEGRO_ALIGN_CENTRE, "LEVEL %i.", current_level);
	double new_time = al_get_time();
    if((int)new_time == (int)levelTimer+2){
		newLevel=false;
    } 
}
void changeMenu(int current){
	switch(current){
	case 0:
		GAME_MODE = NORMAL;
		NG_R= 255;
		TG_R= 0;
		SG_R= 0;
		OP1_R = 0;
		PT_R = 0; 
		break;
	case 1:
		GAME_MODE = TIMED;
		NG_R= 0;
		TG_R= 255;
		SG_R= 0;
		OP1_R = 0;
		PT_R = 0; 
		break;
	case 2:
		GAME_MODE = SURVIVAL;
		NG_R= 0;
		TG_R= 0;
		SG_R= 255;
		OP1_R = 0;
		PT_R = 0; 
		break;
	case 3:
		NG_R= 0;
		TG_R= 0;
		SG_R= 0;
		OP1_R = 255;
		PT_R = 0; 
		break;
	case 4:
		NG_R= 0;
		TG_R= 0;
		SG_R= 0;
		OP1_R = 0;
		PT_R = 255; 
		break;
	}
}
void changeDificultMenu(int current){
	switch(current){
	case 0:
		ND_R= 255;
		MD_R= 0;
		HD_R= 0;
		EX_R = 0;
		break;
	case 1:
		ND_R= 0;
		MD_R= 255;
		HD_R= 0;
		EX_R = 0;
		break;
	case 2:
		ND_R= 0;
		MD_R= 0;
		HD_R= 255;
		EX_R = 0; 
		break;
	case 3:
		ND_R= 0;
		MD_R= 0;
		HD_R= 0;
		EX_R = 255;
		break;
	}
}
void Restart(SpaceShip &ship,Comet (&comets)[NUM_COMETS],Boss &boss,int &current_level,
				bool &isGameOver,int &currentBoss){

		isGameOver = false;
		InitShip(ship);
		InitComet(comets, NUM_COMETS);
		boss.live = false;
		currentBoss = 0;
		current_level = 1;	
}
void SetDificult(int Game_Dificult){
	Game_level = Game_Dificult;
}
void printDificultMenu(int ND_R,int ND_B, int MD_R,int MD_B, int HD_R,int HD_B,int EX_R, ALLEGRO_BITMAP *title){
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_scaled_bitmap(title,0,0,al_get_bitmap_width(title),al_get_bitmap_height(title),
							 WIDTH / 2-(al_get_bitmap_width(title)),(HEIGHT / 2)-140,
							 al_get_bitmap_width(title)*2,al_get_bitmap_height(title)*2,0);

	al_draw_textf(font25, al_map_rgb(255, 0, 255), WIDTH / 2, (HEIGHT / 2)-50, ALLEGRO_ALIGN_CENTRE, "SET DIFICULT");
	al_draw_textf(font20, al_map_rgb(ND_R, 0, ND_B), WIDTH / 2, (HEIGHT / 2)-25, ALLEGRO_ALIGN_CENTRE, "Normal");
	al_draw_textf(font20, al_map_rgb(MD_R, 0, MD_B), WIDTH / 2, (HEIGHT / 2), ALLEGRO_ALIGN_CENTRE, "Medium");
	al_draw_textf(font20, al_map_rgb(HD_R, 0, HD_B), WIDTH / 2, (HEIGHT / 2)+25, ALLEGRO_ALIGN_CENTRE, "Hardcore");
	al_draw_textf(font20, al_map_rgb(EX_R, 0, 255), WIDTH / 2, (HEIGHT / 2)+50, ALLEGRO_ALIGN_CENTRE, "Exit");
}
void printRanking(ALLEGRO_BITMAP *background){
	al_draw_scaled_bitmap(background,0,0,al_get_bitmap_width(background),al_get_bitmap_height(background),
							0,0,WIDTH,HEIGHT,0);
	al_draw_textf(font25, al_map_rgb(255, 255, 0), WIDTH / 2, (HEIGHT / 2)-50, ALLEGRO_ALIGN_CENTRE, "%i POINTS",Highest_pts);
}
	