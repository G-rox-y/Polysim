#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
// to make vscode errors go away
#include <random>

struct node {
    float x, y, velocity, angle;
};
int W= 1920, H=1080;
const float PI = 3.14159f;
std::vector<node> nodesData;

// generates random nodes
void nodeSpawner (int nodeAmmount)
{
    using namespace std;

    random_device dev;
    mt19937 rng(dev());
    uniform_real_distribution<> randomX(0.0f, (float)W);
    uniform_real_distribution<> randomY(0.0f, (float)H);
    uniform_real_distribution<> randomVel(0.2f, 0.8f);
    uniform_real_distribution<> randomAng(0.0f, 360.0f);

    for(int i=0; i<nodeAmmount; i++){
        node n;
        n.x = randomX(rng);
        n.y = randomY(rng);
        n.velocity = randomVel(rng);
        n.angle = randomAng(rng);
        nodesData.emplace_back(n);
    }

    return;
}

void update_nodes_data (){
    for(int i = 0; i < nodesData.size(); i++){
        nodesData[i].y += sin(nodesData[i].angle/(2*PI)) * nodesData[i].velocity;
        nodesData[i].x += cos(nodesData[i].angle/(2*PI)) * nodesData[i].velocity;
        if (nodesData[i].x < 0 || nodesData[i].x > W) nodesData[i].angle = 180.0f - nodesData[i].angle;
        if (nodesData[i].y < 0 || nodesData[i].y > H) nodesData[i].angle *= -1;
    }

    

    return;
}

void windowSetup() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(W, H, desktop.bitsPerPixel), "Polysim");
    sf::View view(sf::Vector2f(W/2, H/2), sf::Vector2f(W, H));
    window.setFramerateLimit(60);
    while(window.isOpen()){

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.setView(view);
        sf::CircleShape dot(2.0f);
        for(int i = 0; i < nodesData.size(); i++){
            dot.setPosition(nodesData[i].x, nodesData[i].y);
            window.draw(dot);
        }
        update_nodes_data();
        window.display();
    }
}

int main()
{
    nodeSpawner(250);
    windowSetup();
    return 0;
}