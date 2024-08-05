#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_set>

sf::Vector2f gravity(0.0f,1.0f);
float dt = 0.0001f;
int step = 10;
float k = 100.0f;
float zoom = 1;

class Cloth : public sf::Drawable, public sf::Transformable{
public:
    Cloth(int numPoints, sf::Vector2f initial) :
    initial(initial),
    numPoints(numPoints),
    m_vertices(sf::Points,numPoints),//(2*sqrt(numPoints) - 2)*sqrt(numPoints)
    lines(sf::Lines, 8*numPoints)
    {
        fillNodes();
        updateNodes();
    }

public:
    sf::VertexArray update(){
        updateNodes();
        
        return lines;

    }
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
        states.transform *= getTransform();

        states.texture = NULL;

        target.draw(m_vertices, states);
    }

private:
    struct node{
        sf::Vector2f position;
        sf::Vector2f force = sf::Vector2f(0,0);
        sf::Vector2f acceleration = sf::Vector2f(0,0);
        sf::Vector2f velocity = sf::Vector2f(0,0);
        std::unordered_set<int> connectedPoints;
        float mass = 1.0f;
    };

private:
    void fillNodes(){
        int lenght = sqrt(numPoints);
        for(int k = 0; k < lenght; k++){
            for(int i = 0; i < lenght; i++){
                sf::Vector2f move(i*step, k*step);
                node n;
                n.position = (initial + move)*zoom;
                int id = i + k*lenght;
                std::cout<<id<<" i : "<<i<<"; k : "<<k<<std::endl;
                if( i - 1 >= 0){n.connectedPoints.insert((i-1) + k*lenght);std::cout<<1<<" :: "<<(i-1) + k*lenght<<std::endl;}
                if( k - 1 >= 0){n.connectedPoints.insert(i + (k-1)*lenght);std::cout<<2<<" :: "<<(i) + (k-1)*lenght<<std::endl;}
                if(i + 1 < lenght){n.connectedPoints.insert((i + 1) + k*lenght);std::cout<<3<<" :: "<<(i+1) + k*lenght<<std::endl;}
                if(k + 1 < lenght){n.connectedPoints.insert(i + (k + 1)*lenght);std::cout<<4<<" :: "<<(i) + (k + 1)*lenght<<std::endl;}
                nodes.push_back(n);
                std::cout<<"------------"<<std::endl;
                        
                //n.connectedPoints.insert((i + dk) + (di + k)*lenght);
            }
                
        }
        

        
    }
    
    void updateNodes(){
        int lenght = sqrt(numPoints);
        forceBetween();
        for(int i = 0 ; i < numPoints; i++){
            if(i < lenght){
              //  lines[i].position = nodes[i].position;
              //  lines[i].color = sf::Color::Red; 
                m_vertices[i].position = nodes[i].position; 
                continue;
            }
            //lines[i].position = nodes[i].position;
            //    lines[i].color = sf::Color::Red;
            nodes[i].acceleration += gravity * dt;
            //std::cout<<nodes[i].acceleration.y<<std::endl;
            nodes[i].velocity += nodes[i].acceleration * dt;
            nodes[i].position += nodes[i].velocity * dt;
            //nodes[i].position/=zoom;
            //std::cout<<nodes[i].position.x<<std::endl;
            m_vertices[i].position = nodes[i].position; 
            m_vertices[i].color = sf::Color(200,50,200);
        }
        updateLines();
    }
        
    void updateLines() {
        int lineIndex = 0;  // To keep track of the current index in the lines array
        for (int i = 0; i < numPoints; ++i) {
            for (auto id : nodes[i].connectedPoints) {
                lines[lineIndex].position = nodes[i].position;
                lines[lineIndex + 1].position = nodes[id].position;
                lines[lineIndex].color = sf::Color(248, 189, 127);
                lines[lineIndex + 1].color = sf::Color(248, 189, 127);
                lineIndex += 2;
            }
        }
    }
    
    void forceBetween(){
        std::vector<node> nodesCopy = nodes; 
        int lenght = sqrt(numPoints);
        for(int i = 0; i < numPoints; i++){
            for(auto id : nodes[i].connectedPoints){
                if(i < lenght)continue;
                float dx = abs(nodes[i].position.x - nodesCopy[id].position.x);
                float dy = abs(nodes[i].position.y - nodesCopy[id].position.y);
                float distance = sqrtf( dx*dx + dy*dy );
                float deltaDistance = distance - step;
                float HookForce = k*deltaDistance;
                
                sf::Vector2f pointer = nodesCopy[id].position - nodes[i].position;
                float pointerSize = sqrtf(pointer.x*pointer.x + pointer.y*pointer.y);
                pointer /= pointerSize;
                //std::cout<<pointer.x<<"  "<<pointer.y<<std::endl;
                //std::cout<<pointer.x*pointer.x + pointer.y*pointer.y<<std::endl;
                //std::cout<<pointerSize<<std::endl;
                nodes[i].acceleration += (pointer*HookForce)/nodes[i].mass;
                float ax = nodes[i].acceleration.x;
                float ay = nodes[i].acceleration.y;
                float accelerationSize = sqrt(ax*ax + ay*ay);
                //std::cout<<<<std::endl;
                //nodes[i].acceleration /= accelerationSize;
                if(distance > step + 5){
                    nodes[i].position += pointer*((distance - step)/2);
                }
                //std::cout<<nodes[i].acceleration.y<<std::endl;
               // std::cout<<i<<" :: "<<id <<" :: "<<deltaDistance<<std::endl;  
            }
           // std::cout<<std::endl;
        }
    }

private:
    sf::VertexArray m_vertices;
    sf::VertexArray lines;
    std::vector<node> nodes;
    int numPoints;
    sf::Vector2f initial;
};


int main(){

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "cloth", sf::Style::Fullscreen);
    sf::VideoMode minimized(1000,700);
    //sf::RenderWindow window(minimized, "cloth");
    int lenght = 20;
    Cloth cloth(lenght * lenght, sf::Vector2f(120,250));
    //Cloth curtina2(lenght * lenght, sf::Vector2f(500,250));
    sf::VertexArray lines;
    sf::VertexArray lines2;
    
    sf::Clock clock;

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::N){
                    if(zoom - 0.1f>=0)zoom -= 0.1f;
                }else if(event.key.code == sf::Keyboard::M){
                    zoom += 0.1f;
                }

                if(event.key.code == sf::Keyboard::Right){
                    if(dt - 0.00001f>=0)dt -= 0.00001f;
                }else if(event.key.code == sf::Keyboard::Left){
                    dt += 0.00001f;
                }
            }
        }
 
        window.clear();
        sf::Time elapsedTime = clock.restart();
        float fps =  1.0f/elapsedTime.asSeconds();
        //std::cout<<"FPS : "<<fps<<std::endl;
        lines = cloth.update();
        //lines2 = curtina2.update();
        window.draw(cloth);
        //window.draw(curtina2);
        window.draw(lines);
        //window.draw(lines2);
        window.display();
    }

    return 0;
}
