#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <cmath>
#include <vector>

int width = 1000;
int height = 500;
int num_particles = 10;

using namespace std;

vector<int> find_minimum_pair(int x, int y);



int main(){

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-100,100);
    int random_number1 = distr(gen);
    int random_number2 = distr(gen);
    std::cout<<random_number1<<" "<<random_number2<<std::endl;


    sf::RenderWindow window(sf::VideoMode(width,height), "Particle");
    window.setFramerateLimit(120);

    sf::VertexArray particles(sf::Points, num_particles); //creates an VertexArray that will hold the particles

    int x_position = width/2;
    int y_position = height/2;
    int x = x_position + random_number1 + random_number2;
    int y = y_position + random_number2;
    int i = 0;
    int k = 0;


    int dx = x_position - x;
    int dy = y_position - y;
    int steps ;
    if(dx > dy){
        steps = dx ;
    }else{
        steps = dy;
    }

    int x_increment = dx/steps;
    int y_increment = dy/steps;


    vector<int> numbers = find_minimum_pair(abs(x - x_position), abs(y - y_position));
    i = numbers[0];
    k = numbers[1]; 

    cout<<x_increment<<y_increment<<endl;

    particles[1].position = sf::Vector2f(x,y);
    particles[1].color = sf::Color::Green;

    while(window.isOpen()){

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear(sf::Color::Black);


        particles[0].position = sf::Vector2f(x_position,y_position);
        particles[0].color = sf::Color::White;

        window.draw(particles);


        window.display();
        if( x != x_position){
            if(x_position < x){ x_position += x_increment;} else {x_position -= x_increment;} ;
            
        }
        if( y != y_position){
            if(y_position < y){y_position+=y_increment;} else{ y_position -= y_increment;}
        }


    }

    return 0;
}


vector<int> find_minimum_pair(int x, int y){

    vector<int> numbers;

    for(int i = 1; i < x; i++){
        if( x % i == 0 &&  y % i == 0 ){
            x = x/i;
            y = y/i;
        }
    }

    numbers.push_back(x);
    numbers.push_back(y);

    return numbers;
} 


