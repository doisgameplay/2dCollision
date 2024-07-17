#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

int width = 650;
int height =500;
int num_particles = 2;
float gravity = 0;
float t = 1;
float restituition = 1;
int time_elapsed = 0;

struct Vector{
    float x;
    float y;
    float modulo;
    Vector(int x_, int y_){
        x = x_;
        y = y_;
        modulo = std::sqrt(x*x + y*y);
    }
};

struct Particle{
    float x_velocity;
    float y_velocity;
    Vector velocity;
    float x;
    float y;
    int vy0;
    int radius;
    sf::Color color;
    int collision_time = 0;
    float mass = 1;
    Particle(int x_, int y_, float x_velocity_, float y_velocity_, int radius_, int c1_, int c2_, int c3_, int c4_)
        :velocity(x_velocity_, y_velocity_) 
    { //we need to create an constructor to pass the color arguments
        sf::Color temp(c1_, c2_, c3_, c4_);
        x = x_;
        y = y_;
        x_velocity = x_velocity_ * t;
        y_velocity = y_velocity_ * t;
        vy0 = y_velocity_ * t;
        //radius = radius_;
        radius = 50;
        color = temp;
    }    
};

Vector sum(Vector a, Vector b);
Vector multiply(float c, Vector a);
float mod(Vector a);

int main(){


    sf::RenderWindow window(sf::VideoMode(width, height), "p2"); //we first need to start the window
    window.setFramerateLimit(60); //we then set an framerate

    std::random_device rd; //we create an random number device generator
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(2,2);
    std::uniform_int_distribution<> orienation(0,1);
    std::uniform_int_distribution<> r(5,20);
    std::uniform_int_distribution<> c(0,255);
    //creating an vector that will hold all the particles:

    std::vector<Particle> particles;

    for(int i = 0; i < num_particles; i++){
        if(orienation(gen) % 2 ==0){ //just creating an50% probability
            Particle particle(width/2 + c(gen),0+c(gen),distr(gen), distr(gen), r(gen), c(gen), c(gen), c(gen), 255); //we then create an Particle object with all this parameters
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
            //std::cout<<"x: "<<particles[i].x_velocity<<std::endl;
            //std::cout<<"y: "<<particles[i].y_velocity;
                        
            //cheking if two balls are coliding:
            if(time_elapsed >= 60){
                particles[i].collision_time ++;
                for(int k = 0; k < circles.size(); k++){
                    if(i == k){continue;}
                    if(sqrt( ( particles[i].x - particles[k].x + particles[i].radius - particles[k].radius )*( particles[i].x - particles[k].x + particles[i].radius - particles[k].radius) + ( particles[i].y - particles[k].y + particles[i].radius - particles[k].radius)*( particles[i].y - particles[k].y + particles[i].radius - particles[k].radius) ) <= particles[i].radius + particles[k].radius && particles[i].collision_time >= 10 && particles[k].collision_time >= 30){
                        int x1 = particles[i].x, y1 = particles[i].y, x2 = particles[k].x, y2 = particles[k].y;
                        float dx = x2 - x1, dy = y2 - y1;
                        float m1 = particles[i].mass, m2 = particles[k].mass;
                        float cos = dx/sqrt(dx*dx + dy*dy), sin = dy/sqrt(dy*dy + dx*dx);
                        float v1x = particles[i].x_velocity, v2x = particles[k].x_velocity, v1y = particles[i].y_velocity, v2y = particles[k].y_velocity;
                        float v1n = v1x * cos + v1y * sin, v2n = v2x * cos + v2y * sin;
                        float V1n = (v1n*(m1 - m2) + 2*m2*v2n)/(m1 + m2), V2n = (v2n*(m2 - m1) + 2*m1*v1n)/(m1 + m2);
                        float V1t = v1x * sin + v1y * cos, V2t = v2x * sin + v2y * cos;
                        particles[i].x_velocity = V1n * cos + V1t * sin;
                        particles[i].y_velocity = V1n * sin + V1t * cos;


                        //if(v1x * v2x < 0){particles[i].x_velocity = -V2n * cos - V2t * sin;}else{particles[i].x_velocity = V2n * cos + V2t * sin;}
                        //if(v1y * v2y < 0){particles[i].y_velocity = V2n * sin + V2t * cos;}else{particles[i].y_velocity = -V2n * sin - V2t * cos;}

                        //if(v1x * v2x < 0){particles[k].x_velocity = V2n * cos + V2t * sin;}else{particles[k].x_velocity = -V2n * cos - V2t * sin;}
                        if(v1y * v2y < 0){particles[k].y_velocity = V2n * sin + V2t * cos;}else{
                            std::cout<<"y:"<<std::endl;
                            if(v1x * v2x > 0){
                                particles[k].y_velocity = -V2n * sin - V2t * cos;
                                }else{particles[k].y_velocity = V1n * sin + V1t * cos;
                                }    
                            }
                        //particles[k].x_velocity = V2n * cos + V2t * sin;
                        //particles[k].y_velocity = V2n * sin + V2t * cos;
                        std::cout<<"V1t: "<<V1t<<std::endl;
                        //std::cout<<"x: "<<particles[i].x_velocity<<std::endl;
                        //std::cout<<"y: "<<particles[i].y_velocity;
                        particles[i].collision_time = 0;
                        particles[k].collision_time = 0; 
                        //std::cout<<time_elapsed<<std::endl;
                        //std::cout<<particles[i].collision_time<<std::endl;
                    // int V1x = mod(V1n) * cos + mod(V1t) * sin; 
                    }
                }
            }else{time_elapsed++;}

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


float mod(Vector a){
    return(sqrt(a.x*a.x + a.y*a.y));
}
