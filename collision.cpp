

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

int width = 1000;
int height = 400;
int num_particles = 100;
float gravity = 0.5;
float t = 1;
float restituition = 0.9;
int time_elapsed = 0;

struct Particle{

    float x_velocity;
    float y_velocity;
    int x;
    int y;
    int vy0;
    int radius;
    sf::Color color;
    int collision_time = 0;
    Particle(int x_, int y_, float x_velocity_, float y_velocity_, int radius_, int c1_, int c2_, int c3_, int c4_){ //we need to create an constructor to pass the color arguments
        sf::Color temp(c1_, c2_, c3_, c4_);
        x = x_;
        y = y_;
        x_velocity = x_velocity_ * t;
        y_velocity = y_velocity_ * t;
        vy0 = y_velocity_ * t;
        radius = radius_;
        color = temp;
    }    
};


struct Vector{

    int x;
    int y;
    float modulo;

    Vector(int x_, int y_){
        x = x_;
        y = y_;
        modulo = std::sqrt(x*x + y*y);
    }
};


Vector sum(Vector a, Vector b);
Vector multiply(float c, Vector a);


int main(){
    

    sf::RenderWindow window(sf::VideoMode(width, height), "p2"); //we first need to start the window
    window.setFramerateLimit(60); //we then set an framerate

    std::random_device rd; //we create an random number device generator
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(2,10);
    std::uniform_int_distribution<> orienation(0,1);
    std::uniform_int_distribution<> r(5,20);
    std::uniform_int_distribution<> c(0,255);
    //creating an vector that will hold all the particles:

    std::vector<Particle> particles;

    for(int i = 0; i < num_particles; i++){
        if(orienation(gen) % 2 ==0){ //just creating an50% probability
            Particle particle(width/2,0,distr(gen), distr(gen), r(gen), c(gen), c(gen), c(gen), 255); //we then create an Particle object with all this parameters
            particles.push_back(particle);
            }else{
            Particle particle(width/2, 0, -distr(gen), -distr(gen), r(gen), c(gen), c(gen), c(gen), 255);
            particles.push_back(particle);}


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
            circles[i].setPosition(particles[i].x, particles[i].y); //we set the position of the circle based on the x and y values of the particle            
            if(particles[i].x < 0 || particles[i].x > width - 2*particles[i].radius){particles[i].x_velocity = -restituition*particles[i].x_velocity;} //we then do the border verifier, if it touches, then the velocity is multiplied by -1
            if(particles[i].y < 0 || particles[i].y > height - 2*particles[i].radius){particles[i].y_velocity = -restituition*particles[i].y_velocity;}
            window.draw(circles[i]); //here we are drawing the circle
            particles[i].x += particles[i].x_velocity*t; //we are updating the x velocity here
            particles[i].y += particles[i].y_velocity*t; //we are updating the y velocity here
            particles[i].y_velocity += gravity; //we are updating the gravity
            

            //cheking if two balls are coliding:
            
        }

        window.display(); //we pass to the next frame

    }
    return 0;
}


Vector sum(Vector a, Vector b){
    Vector answer(a.x + b.x, a.y + b.y);

    return answer;
}

Vector multiply(float c, Vector a){
    Vector answer(a.x*c, a.y*c);

    return answer;
}



