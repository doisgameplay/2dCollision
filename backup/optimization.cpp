#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>

int width = 1000;
int height = 500;
int num_particles = 0;
float gravity =0;//9.8f;
int T = 10;
float t = T*0.016f;
int frame = 0;
bool mouse_active = false;
float min_velocity = 0, max_velocity = 0;
float mouse_x_position = 0;
float mouse_y_position = 0;
int mouse_size = 10;
float ball_size = 5;
bool test = false;
bool pause = false;
bool passed = false;

struct Particle{

    float x_velocity;
    float y_velocity;
    float N_velocity;
    float T_velocity;
    float x;
    float y;
    float max_vy;
    int radius;
    float mass = 1;
    sf::Color color;
    bool touch = false;
    int id;
    Particle(float x_, float y_, float x_velocity_, float y_velocity_, int radius_, int c1_, int c2_, int c3_, int c4_, int id_){ //we need to create an constructor to pass the color arguments
        sf::Color temp(c1_, c2_, c3_, c4_);
        y = y_;
        x_velocity = x_velocity_ ;
        y_velocity = y_velocity_ ;
        radius = radius_;
        x = x_;
        color = temp;
        id = id_;
        //radius = 10;
    }    

};

struct Coordenate{

    float x;
    float y;

    Coordenate(float x_, float y_)
    :x(x_), y(y_){

    };
};

void check_borders(Particle& p);
void check_collision(std::vector<Particle> &particles);
bool is_touching(Particle p1, Particle p2);
float distance(Particle p1, Particle p2);
void separate(Particle &p1, Particle &p2);
void collide(Particle &p1, Particle &p2);
void add_ball( std::vector<Particle> &particles, std::vector<sf::CircleShape> &circles, sf::RenderWindow &window);
void update(std::vector<Particle> &particles, std::vector<sf::CircleShape> &circles, sf::RenderWindow &window);
void check_mouse_collision(std::vector<Particle> &particles, std::vector<sf::CircleShape> &circles, sf::RenderWindow &window);
void separate_mouse_particle(Particle mouse, Particle &p);
void collide_mouse_particle(Particle &mouse, Particle &p, float mouse_x_position, float mouse_y_position);
void show_grid(std::vector<Particle> particles, sf::RenderWindow &window);
void groupY(std::vector<Particle> ps,float w0 = 0, float h0 = 0, float wf = width, float hf = height);
void groupX(std::vector<Particle> ps,float w0 = 0, float h0 = 0, float wf = width, float hf = height);
bool CompareByY(Particle a, Particle b);
bool CompareByX(Particle a, Particle b);
void draw_line(sf::RenderWindow &window);


std::vector<std::vector<int>> idSets;
std::vector<std::vector<Coordenate>> connected_points;
std::vector<std::vector<Particle>> particles_to_check;

int main(){

    sf::RenderWindow window(sf::VideoMode(width, height), "p2"); //we first need to start the window
    window.setFramerateLimit(60); //we then set an framerate

    //creating an vector that will hold all the particles:
    //creating an vector that will hold all the circle shapes:
    std::vector<Particle> particles;
    std::vector<sf::CircleShape> circles;

    while(window.isOpen()){ //while the window is open

        sf::Event event; //we create an Event type

        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close(); 
            }

            if(event.type == sf::Event::KeyPressed){

                if(event.key.code == sf::Keyboard::A){      
                    add_ball(particles, circles, window);
                }
                if(event.key.code == sf::Keyboard::S){
                    std::cout<<"Activated"<<std::endl;
                    mouse_active = true;
                }
                if(event.key.code == sf::Keyboard::D){
                    std::cout<<"Deactivated"<<std::endl;
                    mouse_active = false;
                }
                if(event.key.code == sf::Keyboard::W){
                    std::cout<<"bigger"<<std::endl;
                    mouse_size++;
                }else if(event.key.code == sf::Keyboard::Q){
                    std::cout<<"smaller"<<std::endl;
                    mouse_size--;}
                if(event.key.code == sf::Keyboard::G){
                    gravity = 9.8f;
                }else if(event.key.code == sf::Keyboard::H){
                    gravity = 0;
                }
                if(event.key.code == sf::Keyboard::T){
                    test = true;
                }
                if(event.key.code == sf::Keyboard::U){
                    test = false;
                }
                if(event.key.code == sf::Keyboard::P){
                    if(pause){pause = false;}else{pause = true;}
                }
                if(event.key.code == sf::Keyboard::K){
                    passed = false;
                }
            }
        }
        if(!pause){
        window.clear(sf::Color::Black); //We clear the screen 

        update(particles, circles, window); //updating the information 

        window.display(); //we pass to the next frame
        }
    }
    return 0;
}

void update(std::vector<Particle> &particles, std::vector<sf::CircleShape> &circles, sf::RenderWindow &window ){
    for(int i = 0; i < circles.size(); i++){          
        auto& p = particles[i];
        auto& c = circles[i];
        c.setPosition(p.x,p.y);
        window.draw(c);
        p.x += p.x_velocity * t;
        p.y += p.y_velocity * t;
        check_borders(p);
        if(p.y_velocity + gravity*t <= p.max_vy && p.touch){p.y_velocity += gravity*t;}else if(p.touch){p.y_velocity = p.max_vy;}
        if(!p.touch){p.max_vy = p.y_velocity; p.y_velocity+=gravity*t;}
        check_collision(particles);
        if(mouse_active){
            check_mouse_collision(particles, circles ,window);}
        }
        if(test) show_grid(particles, window);
}

void show_grid(std::vector<Particle> particles, sf::RenderWindow &window){
    idSets.clear();
    groupY(particles);
    if(!passed){
    std::cout<<std::endl;
        for(auto set: idSets ){

            std::cout<<set.size()<<"-{  ";
            for(auto id: set){
                std::cout<<id<<" , ";
            }
            std::cout<<" }-";
        }
        passed = true;
    }
    draw_line(window);
    connected_points.clear();
    test = true;

}


void draw_line(sf::RenderWindow &window){
    for(auto pair : connected_points){
        sf::Vector2f p0(pair[0].x, pair[0].y);
        sf::Vector2f p1(pair[1].x, pair[1].y);
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = p0;
        line[0].color = sf::Color::Red;
        line[1]. position = p1;
        line[1].color = sf::Color::Red;
        window.draw(line);
    }
}


void groupY(std::vector<Particle> ps,float w0, float h0, float wf, float hf){
    std::sort(ps.begin(), ps.end(), CompareByY);
    float yMed = (ps[ps.size()/2 - 1].y + ps[ps.size()/2 ].y - ps[ps.size()/2 - 1].radius + ps[ps.size()/2 ].radius )/2;
    Coordenate c0(w0, yMed);
    Coordenate c1(wf, yMed);
    if(c0.x == c1.x && c0.y == c1.y ){return;}
    std::vector<Coordenate> pair {c0, c1};
    connected_points.push_back(pair);
    std::vector<Particle> up;
    std::vector<Particle> down;
    
    for(auto p : ps){
        if(p.y > yMed ){down.push_back(p);}else{up.push_back(p);}
    }

    std::vector<int> ids;
    
    

    if(up.size() > 2 ){
        groupX(up, w0, h0, wf, yMed);
        }else{
            for(auto p : up){
            ids.push_back(p.id);
            }
            idSets.push_back(ids);
        }

    if(down.size() > 2){ groupX(down, w0, yMed, wf, hf);}else{
        ids.clear();
    for(auto p : down){
        ids.push_back(p.id);
    }
    idSets.push_back(ids);
    }

}


void groupX(std::vector<Particle> ps, float w0, float h0, float wf, float hf){
    std::sort(ps.begin(), ps.end(), CompareByX);
    float xMed = (ps[ps.size()/2 - 1].x + ps[ps.size()/2 ].x - ps[ps.size()/2 - 1].radius + ps[ps.size()/2 ].radius )/2;
    Coordenate c0(xMed, h0);
    Coordenate c1(xMed, hf);

    if(c0.x == c1.x && c0.y == c1.y ){return;}
    std::vector<Coordenate> pair {c0, c1};
    connected_points.push_back(pair);

    std::vector<Particle> left;
    std::vector<Particle> right;

    std::vector<int> ids;
    
    
    for(auto p : ps){
        if(p.x < xMed){left.push_back(p);}else{right.push_back(p);}
    }

    if(left.size() > 2) {
        groupY(left, w0, h0, xMed, hf);
        }else{
            for(auto p : left){
            ids.push_back(p.id);
    }
    idSets.push_back(ids);
    }

    if(right.size() > 2){
        groupY(right, xMed, h0, wf, hf);
        }else{
            ids.clear();
            for(auto p : right){
                ids.push_back(p.id);
            }
            idSets.push_back(ids);

        }

}


bool CompareByX(Particle a, Particle b){
    return a.x < b.x;
    }
bool CompareByY(Particle a, Particle b){
    return a.y < b.y;
}


void check_mouse_collision(std::vector<Particle> &particles, std::vector<sf::CircleShape> &circles, sf::RenderWindow &window){
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    Particle particle_mouse(mouse.x, mouse.y,0, 0, mouse_size, 255, 255, 255, 255,num_particles + 1);
    //std::cout<<mouse_size - particle_mouse.radius<<std::endl;
    sf::CircleShape circle(mouse_size);
    circle.setPosition(mouse.x, mouse.y);
    circle.setFillColor(particle_mouse.color);
    window.draw(circle);
    for(auto& p : particles){
        if(is_touching(particle_mouse, p)){
          //  std::cout<<"mouse touching ball"<<std::endl;
            separate_mouse_particle(particle_mouse, p);
            collide_mouse_particle(particle_mouse, p, mouse_x_position, mouse_y_position);
        }
    }
    mouse_x_position = mouse.x;
    mouse_y_position = mouse.y;
}

void collide_mouse_particle(Particle &mouse, Particle &p, float mouse_x_position, float mouse_y_position){
    
    float D = distance(mouse,p);
    float cos = (mouse.x - p.x)/D;
    float sen = (mouse.y - p.y)/D;

    p.N_velocity = p.x_velocity * cos + p.y_velocity * sen;
    p.T_velocity = p.y_velocity * cos - p.x_velocity * sen;
    p.N_velocity = -p.N_velocity;
    p.x_velocity = p.N_velocity * cos - p.T_velocity * sen;
    p.y_velocity = p.N_velocity * sen + p.T_velocity * cos;

    
   
}


void separate_mouse_particle(Particle mouse, Particle &p){
    float D = distance(mouse,p);
    float d = mouse.radius + p.radius - D;
    float cos = (p.x - mouse.x)/D;
    float sen = (p.y - mouse.y)/D;
    p.x += (d)*cos;
    p.y += (d)*sen; 
}

void add_ball(std::vector<Particle> &particles, std::vector<sf::CircleShape> &circles, sf::RenderWindow &window){
    
    std::random_device rd; //we create an random number device generator
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min_velocity,max_velocity);
    std::uniform_int_distribution<> orienation(0,1);
    std::uniform_int_distribution<> r(5,20);
    std::uniform_int_distribution<> c(0,255);
    std::uniform_int_distribution<> py(0,height - 300);

    sf::Vector2i mouse = sf::Mouse::getPosition(window);

    if(orienation(gen) % 2 == 0){ //just creating an50% probability
            Particle particle(mouse.x,mouse.y,distr(gen), distr(gen), ball_size, c(gen), c(gen), c(gen), 255,num_particles); //we then create an Particle object with all this parameters
            particles.push_back(particle);
            //std::cout<<particle.y<<std::endl;
            sf::CircleShape circle(particle.radius);
            circle.setFillColor(particle.color);
            circles.push_back(circle);
            num_particles++;
            }else{
            Particle particle(mouse.x, mouse.y, -distr(gen), -distr(gen), ball_size, c(gen), c(gen), c(gen), 255,num_particles);
            particles.push_back(particle);
            //std::cout<<particle.y<<std::endl;
            sf::CircleShape circle(particle.radius);
            circle.setFillColor(particle.color);
            circles.push_back(circle);
            num_particles++;
            }
}


void check_collision(std::vector<Particle> &particles){
    for(int i = 0; i < num_particles; i++){
        auto& p1 = particles[i];
        for (int k = 0; k < num_particles; k++ ){
            auto& p2 = particles[k];
            if(p1.id == p2.id){continue;}
            if(is_touching(p1,p2)){
           // std::cout<<i<<" - touching"<<std::endl;
                separate(p1,p2);
                collide(p1,p2);
                }
        } 
    }

}


void collide(Particle &p1, Particle &p2){
    float D = distance(p1,p2);
    float cos = (p2.x - p1.x)/D;
    float sen = (p2.y - p1.y)/D;
    
    
    p1.N_velocity = p1.x_velocity*cos + p1.y_velocity*sen;
    p1.T_velocity = p1.y_velocity*cos - p1.x_velocity*sen;
    p2.N_velocity = p2.x_velocity*cos + p2.y_velocity*sen;
    p2.T_velocity = p2.y_velocity*cos - p2.x_velocity*sen;
    
    p1.N_velocity = ( p1.N_velocity*(p1.mass - p2.mass) + 2*p2.mass*p2.N_velocity )/(p1.mass + p2.mass);
    p2.N_velocity = ( p2.N_velocity*(p2.mass - p1.mass) + 2*p1.mass*p1.N_velocity )/(p2.mass + p1.mass);

    
    p1.x_velocity = p1.N_velocity * cos - p1.T_velocity * sen;
    p1.y_velocity = p1.N_velocity * sen + p1.T_velocity * cos;
    p2.x_velocity = p2.N_velocity * cos - p2.T_velocity * sen;
    p2.y_velocity = p2.N_velocity * sen + p2.T_velocity * cos;

}


void separate(Particle &p1, Particle &p2){
    float D = distance(p1,p2);
    float d = p1.radius + p2.radius - D;
    float cos = (p2.x - p1.x)/D;
    float sen = (p2.y - p1.y)/D;

    //std::cout<<D<<std::endl;
    //std::cout<<cos<<std::endl;
    p1.x -= (d/2)*cos;
    p1.y -= (d/2)*sen;
    p2.x += (d/2)*cos;
    p2.y += (d/2)*sen;    
}

float distance(Particle p1, Particle p2){
    return sqrt( (p1.x - p2.x + p1.radius - p2.radius)*(p1.x - p2.x+ p1.radius - p2.radius) + (p1.y - p2.y+ p1.radius - p2.radius)*(p1.y - p2.y+ p1.radius - p2.radius)) ;  
}


bool is_touching(Particle p1, Particle p2){
    if( distance(p1,p2) <= p1.radius + p2.radius)return true;
    return false;
}


void check_borders(Particle &p){
    if(p.x < 0 ){
            p.x = 0;
            p.x_velocity = -p.x_velocity; 
            }else if(p.x > width - 2 * p.radius){
                p.x = width - 2 * p.radius;
                p.x_velocity = -p.x_velocity;
            }
        if(p.y <= 0 ){
            //p.touch = true;
            p.y = 1;
            p.y_velocity = -p.y_velocity; 
            }else if(p.y > height - 2 * p.radius ){
                p.touch = true;
                p.y = height - 2 * p.radius - 1;
                p.y_velocity = -p.y_velocity ;
            }            
}





