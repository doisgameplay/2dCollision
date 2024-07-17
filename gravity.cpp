//g++ -o test test.cpp -lsfml-graphics -lsfml-window -lsfml-system



#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

int width = 1000;
int height = 700;
int num_particles = 1000;
float gravity = 9.8f;
int T = 10;
float t = T*0.016f;
int frame = 0;


struct Particle{

    float x_velocity;
    float y_velocity;
    float x;
    float y;
    float max_vy;
    int radius;
    sf::Color color;
    bool touch = false;

    Particle(float x_, float y_, float x_velocity_, float y_velocity_, int radius_, int c1_, int c2_, int c3_, int c4_){ //we need to create an constructor to pass the color arguments
        sf::Color temp(c1_, c2_, c3_, c4_);
        y = y_ + 100;
        x_velocity = x_velocity_ ;
        y_velocity = y_velocity_ ;
        radius = radius_;
        x = x_;
        color = temp;

    }    
};


int main(){

    sf::RenderWindow window(sf::VideoMode(width, height), "p2"); //we first need to start the window
    window.setFramerateLimit(60); //we then set an framerate

    std::random_device rd; //we create an random number device generator
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(2,20);
    std::uniform_int_distribution<> orienation(0,1);
    std::uniform_int_distribution<> r(5,20);
    std::uniform_int_distribution<> c(0,255);
    std::uniform_int_distribution<> py(0,height - 300);
    //creating an vector that will hold all the particles:

    std::vector<Particle> particles;

    for(int i = 0; i < num_particles; i++){
        if(orienation(gen) % 2 ==0){ //just creating an50% probability
            Particle particle(width/2,py(gen),distr(gen), distr(gen), r(gen), c(gen), c(gen), c(gen), 255); //we then create an Particle object with all this parameters
            particles.push_back(particle);
            //std::cout<<particle.y<<std::endl;
            }else{
            Particle particle(width/2, py(gen), -distr(gen), -distr(gen), r(gen), c(gen), c(gen), c(gen), 255);
            particles.push_back(particle);
            //std::cout<<particle.y<<std::endl;
            }
    }

    //creating an vector that will hold all the circle shapes:
    std::vector<sf::CircleShape> circles;

    for(int i = 0; i < particles.size(); i++){
        sf::CircleShape circle(particles[i].radius); //we now will assing all the particle information to an circle 
        circle.setFillColor(particles[i].color); //we set the color of the circle here
        circles.push_back(circle);
    }


    while(window.isOpen()){ //while the window is open

        sf::Event event; //we create an Event type

        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear(sf::Color::Black); //We clear the screen 

        for(int i = 0; i < circles.size(); i++){
            frame ++;
            auto& p = particles[i];
            auto& c = circles[i];
            c.setPosition(p.x,p.y);
            window.draw(c);
            p.x += p.x_velocity * t;
            p.y += p.y_velocity * t;
            if(p.y_velocity + gravity*t <= p.max_vy && p.touch){p.y_velocity += gravity*t;}else if(p.touch){p.y_velocity = p.max_vy;}
            if(p.x < 0 ){
                p.x = 0;
                p.x_velocity = -p.x_velocity; 
                }else if(p.x > width - 2 * p.radius){
                    p.x = width - 2 * p.radius;
                    p.x_velocity = -p.x_velocity;
                }
            if(p.y < 0 ){
                p.touch = true;
                p.y = 1;
                p.y_velocity = -p.y_velocity; 
                }else if(p.y > height - 2 * p.radius ){
                    p.touch = true;
                    p.y = height - 2 * p.radius - 1;
                    p.y_velocity = -p.y_velocity ;
                }            
            if(!p.touch){p.max_vy = p.y_velocity; p.y_velocity+=gravity*t;}
        }

        window.display(); //we pass to the next frame

    }
    return 0;
}
