#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>


struct Coordenate{

    float x;
    float y;

    Coordenate(float x_, float y_)
    :x(x_), y(y_){

    };
};

sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
std::vector<float> xPositions;
std::vector<float> yPositions;
std::vector<float> radius;
std::vector<float> xVelocities;
std::vector<float> yVelocities;
std::vector<float> masses;
std::vector<sf::CircleShape> Circles;
std::vector<sf::Color> Colors;
std::vector<std::vector<Coordenate>> connected_points;
int numberOfBalls = 0;
float dt = 0.016f;
float gravity = 0;// 9.8f;


void addBall(float x, float y, float r = 10, float mass = 1);
void addBall(float x, float y, float r, float mass){
    //this function is used to add the balls to the box
    xPositions.push_back(x);
    yPositions.push_back(y);
    xVelocities.push_back(1);
    yVelocities.push_back(0);
    radius.push_back(r);
    masses.push_back(mass);
    sf::CircleShape circle(r);
    circle.setPosition(x, y);
    circle.setOrigin(sf::Vector2f(r, r));
    Circles.push_back(circle);
    std::cout<<xPositions.size()<<std::endl;
    numberOfBalls ++;
}



void updateVelocities();
void updateVelocities(){
    //This function will update all the velocites inside the box
    for(int i = 0 ; i < numberOfBalls; i++){
        yVelocities[i] += gravity * dt;
    }

}


void updatePositions();
void updatePositions(){
    //this function will update all the positions of the balls
    for(int i = 0; i < numberOfBalls; i++){
        xPositions[i] += xVelocities[i] * dt; 
        yPositions[i] += yVelocities[i] * dt;
        Circles[i].setPosition(xPositions[i], yPositions[i]);
    }
}


void checkBorderCollision();
void checkBorderCollision(){
    //this function will check if any of the balls is colliding eith an corner
    for(int i = 0; i < numberOfBalls; i++){
        float x = xPositions[i];
        float y = yPositions[i];
        float r = radius[i];
        
        if(x - r <= 0){
            xVelocities[i] *= -1;
            xPositions[i] = r;
            }else if(x + r >= desktop.width){
                xVelocities[i] *= -1;
                xPositions[i] = desktop.width - r; 
                }
        if(y - r <= 0){
            yVelocities[i] *= -1;
            yPositions[i] = r;
            }else if(y + r >= desktop.height){
                yVelocities[i] *= -1;
                yPositions[i] = desktop.height - r; 
                }
    }
}


void keyEvents(sf::Event event);
void keyEvents(sf::Event event){
    bool UpKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool DownKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool Wkey = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool Rightkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    bool Leftkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    if(Rightkey){
        gravity += 0.05;
    }else if(Leftkey){
        if(gravity - 0.05 >= 0 ){gravity-= 0.05;}else{gravity =0;}
    }

    if(Wkey) addBall(100,100,20);

    if(UpKey){
        dt += 0.001;
        }else if(DownKey){
            if(dt - 0.001 >= 0){dt -= 0.001;}else{ dt = 0;}
        }
}


void checkUserInputs(sf::RenderWindow &window);
void checkUserInputs(sf::RenderWindow &window){
    //this function will search for user inputs
    sf::Event event;
    while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed) window.close();
            if(event.type == sf::Event::KeyPressed) keyEvents(event);

        }
}

void checkCollision(int id1, int id2);
void checkCollision(int id1, int id2){
    //This function will se if the balls with id1 and id2 are colliding or not
    float dx = xPositions[id2] - xPositions[id1];
    float dy = yPositions[id2] - yPositions[id1];
    if(dx == 0 && dy == 0){ dx = 1; dy = 1;}
    float distance = sqrtf(dx*dx + dy*dy);
    float intersection = radius[id2] + radius[id1] - distance ;
    if(intersection > 0 && distance >0){
        xPositions[id1] -= dx/distance*intersection/2; 
        yPositions[id1] += dy/distance*intersection/2;
        xPositions[id2] += dx/distance*intersection/2; 
        yPositions[id2] -= dy/distance*intersection/2;    
        float m1 = masses[id1];
        float m2 = masses[id2];
        float v1x = xVelocities[id1];
        float v2x = xVelocities[id2];
        float v1y = yVelocities[id1];  
        float v2y = yVelocities[id2];
        float J = 2*((v1x - v2x)*dx/distance + (v1y - v2y)*dy/distance)/(1/m1 + 1/m2);

        xVelocities[id1] = v1x - J/m1 * dx/distance;
        yVelocities[id1] = v1y - J/m1 * dy/distance;
        xVelocities[id2] = v2x + J/m2 * dx/distance;
        yVelocities[id2] = v2y + J/m2 * dy/distance;
    }


}

void draw_line(sf::RenderWindow &window);
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


void groupY(std::vector<int> ids,float w0 = 0, float h0 = 0, float wf = desktop.width, float hf = desktop.height);



void groupX(std::vector<int> ids,float w0 = 0, float h0 = 0, float wf = desktop.width, float hf = desktop.height);


void update(sf::RenderWindow &window);  
void update(sf::RenderWindow &window){
    //this function will update all the values of the box and inside too
    if(numberOfBalls > 0){
    for(int i = 0; i < numberOfBalls; i++){
        for(int k = 0; k < numberOfBalls; k++){
            if(i == k) continue;
            checkCollision(i, k);
        }
    }
    updateVelocities();
    updatePositions();
    checkBorderCollision();
    std::vector<int> ids;
    for(int i = 0; i < numberOfBalls; i++){
        ids.push_back(i);
    }
    connected_points.clear();
    groupX(ids);
    std::cout<<"\n\n\n";
    }
}  


void draw(sf::RenderWindow &window);
void draw(sf::RenderWindow &window){
    //this function is used to draw all the balls on the screen
    for( auto circle : Circles){
        window.draw(circle);
    }
    draw_line(window);  

}

int main(){

    sf::RenderWindow window(desktop, "Balls 2.0", sf::Style::Fullscreen);
    addBall(200, 200, 50);

    while(window.isOpen()){

        checkUserInputs(window);

        window.clear(sf::Color::Black);

        update(window);

        draw(window);

        window.display();

    }

    return 0;
}



void groupY(std::vector<int> ids,float w0, float h0, float wf, float hf){
       float yMed = 0;
    for(auto id : ids){
        yMed += yPositions[id];
    }
    yMed /= ids.size();

    std::vector<int> up; 
    std::vector<int> down;

    for(auto id : ids){
        if(abs(yPositions[id] - yMed) <= radius[id]){down.push_back(id), up.push_back(id);}
        if(yPositions[id] + radius[id] <= yMed) up.push_back(id);
        if(yPositions[id] - radius[id] >= yMed) down.push_back(id);
    }
    //std::cout<<xMed<<std::endl;

    std::cout<<"down : { ";
    for(auto a : down){
        std::cout<<a<<" ";
    }
    std::cout<<"}"<<std::endl;


    std::cout<<"up : { ";
    for(auto a : up){
        std::cout<<a<<" ";
    }
    std::cout<<"}"<<std::endl;

    Coordenate c1(wf,yMed);
    Coordenate c2(w0, yMed);
    std::vector<Coordenate> line;
    line.push_back(c1);
    line.push_back(c2);
    connected_points.push_back(line);
    if(up.size() == down.size() && down.size() == ids.size()) return;

   groupX(up, w0, h0, wf, yMed);
   groupX(down,w0, yMed, wf, hf); 


}


void groupX(std::vector<int> ids,float w0, float h0, float wf, float hf){
    float xMed = 0;
    for(auto id : ids){
        xMed += xPositions[id];
    }
    xMed /= ids.size();

    std::vector<int> right; 
    std::vector<int> left;

    for(auto id : ids){
        if(abs(xPositions[id] - xMed) <= radius[id]){left.push_back(id), right.push_back(id);}
        if(xPositions[id] + radius[id] <= xMed) left.push_back(id);
        if(xPositions[id] - radius[id] >= xMed) right.push_back(id);
    }
    //std::cout<<xMed<<std::endl;

    //std::cout<<"LEFT : { ";
    for(auto a : left){
   //     std::cout<<a<<" ";
    }
 //   std::cout<<"}"<<std::endl;


//    std::cout<<"RIGHT : { ";
    for(auto a : right){
  //      std::cout<<a<<" ";
    }
   // std::cout<<"}"<<std::endl;

    Coordenate c1(xMed,h0);
    Coordenate c2(xMed, hf);
    std::vector<Coordenate> line;
    line.push_back(c1);
    line.push_back(c2);
    connected_points.push_back(line);

    if(right.size() == left.size() && left.size() == ids.size()) return;
   groupY(right, w0, h0, xMed, hf);
   groupY(left, xMed, h0, wf, hf); 

}








