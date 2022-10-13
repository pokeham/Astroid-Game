
#include <SFML/Graphics.hpp>
#include <time.h>
#include <SFML/Audio.hpp>
#include <list>
#include <cmath>
#include <iostream>
#include <chrono>

using namespace sf;
using namespace std;

const int W = 1300;//width for window
const int H = 800;//height for window

float DEGTORAD = 0.017453f;//degree to radians conversion factor
///animation class///
class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
    std::vector<IntRect> frames;

	Animation(){}

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)//animation constructor
	{
	    Frame = 0;//initialize frame count to 0
        speed = Speed;//initialize speed to speed passed thru

		for (int i=0;i<count;i++) {
            frames.push_back(IntRect(x + i * w, y, w, h));
        }
		sprite.setTexture(t);//set texture to passed t
		sprite.setOrigin(w/2,h/2);//set origin to half od passed width and height
        sprite.setTextureRect(frames[0]);
	}


	void update()
	{
    	Frame += speed;//increment frames
		int n = frames.size();
		if (Frame >= n)
        {
            Frame -= n;
        }
		if (n>0) {
            sprite.setTextureRect( frames[int(Frame)] );
        }
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size();//check if animation is ended
	}

};

///entity class///
class Entity
{
    //members for all entities
public:
float x,y,dx,dy,R,angle;
bool life;
std::string name;
Animation anim;

Entity()
{
  life=1;//when called initialize life to 1
}
//settings for all entities
//used to modify entities animation x coordinate y coordinate its angle and radius
void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
{
  anim = a;
  x=X; y=Y;
  angle = Angle;
  R = radius;
}

virtual void update(){};
//render function
void draw(RenderWindow &app)
{
  anim.sprite.setPosition(x,y);
  anim.sprite.setRotation(angle+90);
  app.draw(anim.sprite);//draw predefined sprite to screen

  CircleShape circle(R);//render hitboxes for entities
  circle.setFillColor(Color(255,0,0,170));
  circle.setPosition(x,y);
  circle.setOrigin(R,R);
}

virtual ~Entity(){};//destructor
};
///Enemy class///
class ENEMY: public Entity
{
public:
    bool Rectangle=false, V = false;//formation members
    static bool Frozen;//frozen checker
    ENEMY()//constructor
    {
        dx = 0;// x speed is intialized to 0
        dy = 0;// y speed is intialized to 0
        name = "ENEMY";
    }
    void update()
    {
        if(Frozen == true)//frezze if frozen member is true
        {
            dx = 0;
            dy = 0;
        }
        else if (Rectangle == true)//if rectangle formation is true use these speeds
        {
            dx = 0;
            dy = 2;
        }
        else if(V == true )//if V formation is true use these speeds
        {
            dx =0;
            dy = 2;
        }

        if(Frozen != true)
        {
            x+=dx;
            y+=dy;
        }

    }


};
///UFO class///
class UFO: public Entity
{
public:
    static int UFOcount;//counter of UFO's on screen
    UFO()//UFO constructor
    {
        dx = 3;//intital UFO speed
        dy = 0;
        name = "UFO";
        UFOcount++;//increment count for UFO on screen if consturctor is called
    }
    ~UFO(){
        UFOcount--;//decrement count if deconstructor is called
    }
   static int get_UFO(){return UFOcount;};//getter for UFOcount
    void update()
    {
        x+=dx;//incremeent x by dx
        if (x>W || x<0 || y>H || y<0){//if it hits edge of screen kill it
            life=0;
        }
    }

};
///asteroid class///
class asteroid: public Entity
{
public:
    static int asteroid_num;//number of astroids on screen
  asteroid()
  {
    dx=rand()%8-4;//random speeds in x and y
    dy=rand()%8-4;
    name="asteroid";
    asteroid_num++;//increment counter variable when the constructor is called
  }
  ~asteroid(){
      asteroid_num--;//decrement counter if destructor is called
  }

void  update()
  {
   x+=dx;//increment x and y by dx and dy respectfully
   y+=dy;
      //if it goes off-screen put on the opposite side of screen
   if (x>W) {
       x=0;
   }
   if (x<0) {
       x=W;
   }
   if (y>H) {
       y=0;
   }
   if (y<0) {
       y=H;
   }
  }

};


///bullet class///
class bullet: public Entity
{
public:
  bullet()//constructor
  {
    name="bullet";
  }

void  update()
  {
   dx=cos(angle*DEGTORAD)*6;//angle of bullet is relative to angle of player
   dy=sin(angle*DEGTORAD)*6;
      //increment x and y by dx and dy
   x+=dx;
   y+=dy;

   if (x>W || x<0 || y>H || y<0) {// if it goes off-screen kill it
       life=0;
   }
  }

};
///player2 class///(used for invaders)
class player2: public Entity//player2 with constant speed
{
public:
    player2()//constructor
    {
        name = "player2";
    }
    void update()
    {

        if (x>W) {//if goes of x sides of screen it ends up on opppiste side if it hits y borders keep it there
            x=0;
        }
        if (x<0) {
            x=W;
        }
        if (y>H) {
            y=H;
        }
        if (y<0) {
            y=0;
        }

    }
};
///player class///(used for asteroids)
class player: public Entity
{
public:
   bool thrust;

   player()
   {
     name="player";
   }

   void update()
   {
     if (thrust)//if forward is pressed
      { dx+=cos(angle*DEGTORAD)*0.2; //increment dx and dy relative to the angle of the player
        dy+=sin(angle*DEGTORAD)*0.2; }
     else//if thrust isn't present
      { dx*=0.99;//slow own speeds in both x and y
        dy*=0.99; }

    int maxSpeed=15;//max speed
    float speed = sqrt(dx*dx+dy*dy);//speed is abosulte vale of x and y speeds
    if (speed>maxSpeed)
     { dx *= maxSpeed/speed;
       dy *= maxSpeed/speed; }//keep at max speed

    x+=dx;//increment speed
    y+=dy;

    if (x>W) {//pass thru borders of screen
        x=0;
    }
    if (x<0) {
        x=W;
    }
    if (y>H) {
        y=0;
    }
    if (y<0) {
        y=H;
    }

   }

};


bool isCollide(Entity *a,Entity *b)//check if two entities collide
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}

int UFO::UFOcount=0;
bool ENEMY::Frozen = false;
int asteroid::asteroid_num = 0;

int main()
{
    RenderWindow menu(VideoMode(1200,800),"Main Menu!");//render main menu
    menu.clear(Color::Black);//clear window
    sf::Font font1;//loaf font from file
    font1.loadFromFile("fonts/ka1.ttf");//load in fonts//karmatic arcade font byvic fieger//https://www.dafont.com/karmatic-arcade.font//license, free for personal use
    Text menu_header;//menu text
    menu_header.setFont(font1);
    menu_header.setCharacterSize(70);
    menu_header.setString("GAME SELECT!");
    menu_header.setStyle(Text::Bold);
    menu_header.setStyle(Text::Italic);
    menu_header.setFillColor(Color::Black);
    menu_header.setOutlineColor(Color::White);
    menu_header.setOutlineThickness(20);
    menu_header.move(300,80);
    menu.draw(menu_header);//draw text
    Text Game1;//game one text
    Game1.setFont(font1);
    Game1.setCharacterSize(50);
    Game1.setString("Astroids!");
    Game1.setFillColor(Color::Red);
    Game1.setOutlineColor(Color::White);
    Game1.setOutlineThickness(15);
    Game1.move(100,500);
    menu.draw(Game1);
    Text Game2;//game two text
    Game2.setFont(font1);
    Game2.setCharacterSize(50);
    Game2.setString("Invaders!");
    Game2.setFillColor(Color::Blue);
    Game2.setOutlineColor(Color::White);
    Game2.setOutlineThickness(15);
    Game2.move(700,500);
    menu.draw(Game2);
    menu.display();

    Texture t1,t2,t3,t4,t5,t6,t7,t8;//initialize and load in all textures
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_red.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    t8.loadFromFile("images/alien2.png");//load in alien2.png free for use  at https://flyclipart.com/download-png#space-invaders-alien-transparent-background-png-png-arts-340337.png

    t1.setSmooth(true);//smooth the spaceship and background texture
    t2.setSmooth(true);
    t8.setSmooth(true);//smooth the enemy/ufo texture as well

    Sprite background(t2);//make the background texture a sprite

    //make every other texture an animation
    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);
    Animation sUFO(t8,0,0,50,50,1,0);

    bool breaker,game1,game2 = false;//initalize breaker and game1 and game2 to false
    srand(time(0));//start timer
    std::list<Entity*> entities;//initialize entities
    int kill_Count=0;
    auto Frozen_counter = std::chrono::system_clock::now();//intialize counter form frzen power up in game2
    while(menu.isOpen() && breaker == false)//waiting loop for menu
    {
        Event event;
        while (menu.pollEvent(event))//poll for events
        {
            if (event.type == Event::Closed) {
                menu.close();//if the app is closed close the app
            }
            if(Mouse::getPosition().x < 850 && Mouse::isButtonPressed(sf::Mouse::Left))//button for game1
            {
                //if game1 button is pressed goto the game1 if statement and close menu
                game1 = true;
                breaker = true;
                menu.close();

            }
            if(Mouse::getPosition().x > 850 && Mouse::isButtonPressed(sf::Mouse::Left))//button for game1
            {
                //if game2 button is pressed goto the game2 if statement and close menu
                game2 = true;
                breaker = true;
                menu.close();

            }

        }


    }

    if(game1 == true)//game1 if statement
    {
        sf::SoundBuffer buffer;
        buffer.loadFromFile("images/alien_sound.ogg");
        sf::Sound sound;
        sound.setBuffer(buffer);
        RenderWindow app(VideoMode(W, H), "Asteroids!");//render window to screen with W H and height and width
        app.setFramerateLimit(60);//set frame limit
        for(int i=0;i<15;i++)//spawn in new astroids before the game starts
        {
            asteroid *a = new asteroid();
            a->settings(sRock, rand()%W, rand()%H, rand()%360, 25);
            entities.push_back(a);//add to entities vector
        }

        player *p1 = new player();//spawn in a new player
        p1->settings(sPlayer,200,200,0,20);
        entities.push_back(p1);//add to entities vector
        while (app.isOpen()) //asteroids
        {
            Event event;
            while (app.pollEvent(event))
            {
                if (event.type == Event::Closed) {
                    app.close();//if the app is closed close the app
                }
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Space) {//if space is pressed spawn a new bullet and add it to the entities vector
                        bullet *b = new bullet();
                        b->settings(sBullet, p1->x, p1->y, p1->angle, 10);
                        entities.push_back(b);
                    }
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                p1->angle+=3;//if right is pressed rotate the player to the right
            }
            if (Keyboard::isKeyPressed(Keyboard::Left))  {
                p1->angle-=3;//if left is placed rotate the player to the left
            }
            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                p1->thrust=true;//if up is pressed set thrust to true
            }
            else {
                p1->thrust=false;//otherwise thrust is automatically false
            }


            //iterate thru every combination of astroids and bullets and id the
            //iscollide function returns true change the life of both to false
            //and spawn in a explosion animation and add the explosion animation to the
            //entities vector
            for(auto a:entities) {
                for (auto b: entities) {
                    if(a->name == "UFO" && b->name =="bullet")
                    {
                        if(isCollide(a,b))
                        {
                            a->life = false;
                            b->life = false;

                            Entity *e = new Entity();
                            e->settings(sExplosion, a->x, a->y);
                            e->name = "explosion";
                            entities.push_back(e);
                            sound.stop();

                        }
                    }
                    if (a->name == "asteroid" && b->name == "bullet")
                        if (isCollide(a, b)) {
                            a->life = false;
                            b->life = false;

                            Entity *e = new Entity();
                            e->settings(sExplosion, a->x, a->y);
                            e->name = "explosion";
                            entities.push_back(e);

                            //when an astroid is destroyed spawn in two smaller astroids  and add them to the entities vector
                            for (int i = 0; i < 2; i++) {
                                if (a->R == 15) continue;
                                Entity *e = new asteroid();
                                e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                                entities.push_back(e);
                            }

                        }
                    // if an astroid and a player collide spawn in an explosion for the point of collision and respawn player
                    if (a->name == "player" && b->name == "asteroid")
                        if (isCollide(a, b)) {
                            b->life = false;

                            Entity *e = new Entity();
                            e->settings(sExplosion_ship, a->x, a->y);
                            e->name = "explosion";
                            entities.push_back(e);

                            p1->settings(sPlayer, W / 2, H / 2, 0, 20);
                            p1->dx = 0;
                            p1->dy = 0;
                        }
                    if (a->name == "UFO" && b->name == "player")

                        if (isCollide(a, b))
                        {
                            a->life = false;

                            p1->settings(sPlayer, W / 2, H / 2, 0, 20);
                            p1->dx = 0;
                            p1->dy = 0;

                            Entity *e = new Entity();
                            e->settings(sExplosion_ship, a->x, a->y);
                            e->name = "explosion";
                            entities.push_back(e);
                            sound.stop();

                        }
                }
            }


            if (p1->thrust)//thrust animation only shows when thrusting
            {
                p1->anim = sPlayer_go;
            }
            else   {//otherwise it points to the other texture
                p1->anim = sPlayer;
            }

            //update life for explosions once they are completed
            for(auto e:entities)
            {
                if (e->name=="explosion"){
                    if (e->anim.isEnd()) {
                        e->life=0;
                    }
                }
            }

            ///spawn in astroids///
            if(asteroid::asteroid_num == 0){//spawn in new 15 astroids if none are present
                for(int i = 0; i < 15; i++){
                    asteroid * a = new asteroid();
                    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
                    entities.push_back(a);
                }
            }
            if (rand()%500==0)//spawin in astroid randomly
            {
                asteroid *a = new asteroid();
                a->settings(sRock, 0,rand()%H, rand()%360, 25);
                entities.push_back(a);
            }
            ///spawn in UFO///
            if(rand() % 250 == 0 && UFO::UFOcount == 0) {//spawn in UFO ranomdly
                UFO *u = new UFO();
                u->update();
                u->settings(sUFO, 1, rand() % H, -90, 25);
                entities.push_back(u);
                sound.play();
            }

            //increment each animation one step and check if the life is false if so delete the entity
            for(auto i=entities.begin();i!=entities.end();)
            {
                Entity *e = *i;

                e->update();
                e->anim.update();

                if (e->life==false) {
                    i=entities.erase(i);
                    delete e;
                }
                else {
                    i++;
                }
            }

            if(Keyboard::isKeyPressed(sf::Keyboard::Escape))//close if esc is pressed
            {
                app.close();
                return 0;
            }



            //////draw//////
            app.draw(background);//draw all objects to screen

            for(auto i:entities)
                i->draw(app);

            app.display();//display all objects to screen
        }
    }










    ////GAME TWO////
    if(game2 == true)
    {
        entities.clear();//clear entinties vector
        reset:;//goto statement for reset
        RenderWindow app(VideoMode(W, H), "Invaders!");//render window to screen with W H and height and width
        app.setFramerateLimit(60);//set frame limit


        ///spawn in initial Enemies///
        for(int i=0;i<15;i++)//spawn in first row of enemies
        {
            ENEMY *e = new ENEMY();
            e->settings(sUFO, 60*(i+1)+180, -150, 270, 25);//striaght line of Enemies
            entities.push_back(e);//add to entities vector
            e->Rectangle = true;
        }
        for(int i=0;i<15;i++)//spawn in second row of enemies
        {
            ENEMY *e = new ENEMY();
            e->settings(sUFO, 60*(i+1)+180, -75, 270, 25);//striaght line of Enemies
            entities.push_back(e);//add to entities vector
            e->Rectangle = true;
        }
        for(int i=0;i<15;i++)//spawn in third row of Enemies
        {
            ENEMY *e = new ENEMY();
            e->settings(sUFO, 60*(i+1)+180, 0, 270, 25);//striaght line of Enemies
            entities.push_back(e);//add to entities vector
            e->Rectangle = true;
        }

        ///define text's///
        Text Power_up;
        Power_up.setFont(font1);
        Power_up.setString("");
        Power_up.setFillColor(sf::Color::White);
        Power_up.setOutlineColor(sf::Color::Blue);
        Power_up.setOutlineThickness(10);
        Power_up.move(815,160);

        Text r_text;//final text
        r_text.setFont(font1);
        r_text.setString("");
        r_text.setCharacterSize(40);
        r_text.setFillColor(sf::Color::Black);
        r_text.setStyle(sf::Text::Bold);
        r_text.setOutlineColor(sf::Color::White);
        r_text.setOutlineThickness(10);
        r_text.move(W / 2 - 400, H / 2 );

        Text lives;
        lives.setFont(font1);
        lives.setCharacterSize(40);
        lives.setStyle(Text::Bold);
        lives.setFillColor(Color::Red);
        lives.setOutlineColor(Color::White);
        lives.setOutlineThickness(10);
        lives.move(1000, 60);

        ///spawn in a new player///
        player2 *p = new player2();
        p->settings(sPlayer,W/2,H/2,-90,20);//player spawn settings
        entities.push_back(p);//add to entities vector

        ///initialize variables///
        bool checker=true;//breaker for main game loop
        int Lives = 10;//Lives counter
        int numframes = 0;//frame counter
        auto Counter = std::chrono::system_clock::now();//clock set inorder to help timing of Enemies spawn
        font1.loadFromFile("fonts/ka1.ttf");//load in fonts//karmatic arcade font byvic fieger//https://www.dafont.com/karmatic-arcade.font//license, free for personal use

        ///main game loop///
        while(app.isOpen()&& checker == true) {
            Event event;
            while (app.pollEvent(event)) {//poll for events
                if (event.type == Event::Closed) {//check if app is closed
                    app.close();
                }
                if (event.type == Event::KeyPressed) {
                    if (event.key.code ==Keyboard::Space) {//if space is pressed spawn a new bullet and add it to the entities vector
                        bullet *b = new bullet();
                        b->settings(sBullet, p->x, p->y, p->angle, 10);
                        entities.push_back(b);
                    }
                }
            }
            ///movement of player///
            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                p->x += 8;//if right is pressed move the player to the right
            }
            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                p->x -= 8;//if left is pressed move the player to the left
            }
            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                p->y -= 8;//if up is pressed move the player up
            }
            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                p->y += 8;//if down is pressed move the player down
            }
            ///collision checker nested for loops///
            for (auto a: entities) {
                for (auto b: entities) {//iterate thru two combinations of entities to check for collisions
                    if (a->name == "ENEMY" && b->name == "bullet") {
                        if (isCollide(a, b)) {//if an Enemy and bullet collide
                            a->life = false;//kill the enemy
                            kill_Count++;//update kill count
                            Entity *ex = new Entity();//spawn in an explosion
                            ex->settings(sExplosion, a->x, a->y);
                            ex->name = "explosion";
                            entities.push_back(ex);//add it to the entities vector
                        }
                    }
                    if (a->name == "ENEMY" && b->name == "player2") {
                        if (isCollide(a, b)) {//if an enemy and a player collide
                            a->life = false;//kill the enemy

                            Entity *ex = new Entity();//spawn in an explosion
                            ex->settings(sExplosion, a->x, a->y);
                            ex->name = "explosion";
                            entities.push_back(ex);//add the explosion to the entities vector
                            b->settings(sPlayer, W / 2, 600, -90, 20);//move the player back into the center of the window(aka respawn it)
                        }
                    }


                }
                if (a->name == "ENEMY" && a->y == H) {//if an enemy passes the bottom of the screen
                    Lives--;//decrement the Lives counter
                    a->life = 0;//and kill the enemy
                }
            }



            ///spawn in waves of enemies///
            if (std::chrono::duration_cast<std::chrono::seconds>
                    (std::chrono::system_clock::now() - Counter).count() >12) {//if the difference between the clock and the timer set for spawning in new waves equals 12 spawn a new wave
                int number = rand() % 2 + 1;//randomly choose which kind of wave is spawned
                if (number == 1) {
                    Counter = std::chrono::system_clock::now();//reset spawn counter
                    ///spawn in new Enemies in rectangle formation///
                    for (int i = 0; i < 15; i++)
                    {
                        ENEMY *e = new ENEMY();
                        e->settings(sUFO, 60 * (i + 1) + 180, -150, 270, 25);
                        entities.push_back(e);//add to entities vector
                        e->Rectangle = true;
                    }
                    for (int i = 0; i < 15; i++)//spawn in new Enemies Row
                    {
                        ENEMY *e = new ENEMY();
                        e->settings(sUFO, 60 * (i + 1) + 180, -75, 270, 25);
                        entities.push_back(e);//add to entities vector
                        e->Rectangle = true;
                    }
                    for (int i = 0; i < 15; i++)//spawn in new Enemies row
                    {
                        ENEMY *e = new ENEMY();
                        e->settings(sUFO, 60 * (i + 1) + 180, 0, 270, 25);
                        entities.push_back(e);//add to entities vector
                        e->Rectangle = true;
                    }
                } else;//if 2 was choosen
                {
                    Counter = std::chrono::system_clock::now();//reset the counter for spawning in waves
                    ///spawn in the V formation///
                    for (int i = 0; i < 15; i++)
                    {
                        if (i < 8) {
                            ENEMY *e = new ENEMY();
                            e->settings(sUFO, 60 * (i + 1) + 180, 60 * (i + 1) - 850, 270, 25);
                            entities.push_back(e);//add to entities vector
                            e->V = true;
                        } else {
                            ENEMY *e = new ENEMY();
                            e->settings(sUFO, -60 * (i) + 1560, 60 * (i - 7) - 850, 270, 25);
                            entities.push_back(e);//add to entities vector
                            e->V = true;
                        }

                    }
                    for (int i = 0; i < 15; i++)//second rwo of V formation
                    {
                        if (i < 8) {
                            ENEMY *e = new ENEMY();
                            e->settings(sUFO, 60 * (i + 1) + 180, 60 * (i + 1) - 925, 270, 25);
                            entities.push_back(e);//add to entities vector
                            e->V = true;
                        } else {
                            ENEMY *e = new ENEMY();
                            e->settings(sUFO, -60 * (i) + 1560, 60 * (i - 7) - 925, 270, 25);
                            entities.push_back(e);//add to entities vector
                            e->V = true;
                        }

                    }
                }
            }

            for (auto ex: entities) {
                if (ex->name == "explosion") {
                    if (ex->anim.isEnd()) {//when explosions animation finish kill the explosions
                        ex->life = 0;
                    }
                }
            }
            if (Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                app.close();//close app if esc is pressed
            }
            if (Lives < 0) {
                Event event;
                while(true)
                {
                    app.clear();
                    lives.setString("Lives: 0");
                    r_text.setString("Press the R key to try again");
                    Power_up.setString("");
                    app.draw(lives);
                    app.draw(r_text);
                    app.draw(Power_up);
                    app.display();
                    if (Keyboard::isKeyPressed(Keyboard::R)) {

                        entities.clear();
                        kill_Count = 0;
                        Lives = 10;
                        goto reset;
                        break;
                    }
                }
            }
            if (kill_Count > 15 )// if kill count is greater than 15
            {
                Power_up.setString("Press F for Freeze!");//upddate text
                if(Keyboard::isKeyPressed(sf::Keyboard::F)) {//i ff is pressed
                    Power_up.setString("");//clear text
                    ENEMY::Frozen = true;//freeze enemies
                    Frozen_counter = std::chrono::system_clock::now();//reset the timer
                    kill_Count = 0;//reset the kill count
                }
            }
            if(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-Frozen_counter).count()>5)
            {
                ENEMY::Frozen = false;
            }
            for (auto i = entities.begin(); i != entities.end();) {//update all entities/animations every iteration(frame)
                Entity *e = *i;

                e->update();
                e->anim.update();

                if (e->life == false) {
                    i = entities.erase(i);
                    delete e;
                } else {
                    i++;
                }
            }
            app.draw(background);
            for (auto i: entities) {
                i->draw(app);
            }//draw each entity
            app.draw(r_text);
            app.draw(Power_up);
            if(Lives > 0){
                lives.setString("Lives: " + std::to_string(Lives));
            }
            app.draw(lives);//draw text
            app.display();
            numframes++;
        }
    }
    return 0;
}

