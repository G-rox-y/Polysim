#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

// to make vscode errors go away
#include <random>

struct node {
    float x, y, velocity, angle;
};
struct line {
    int p1, p2;
    int age;
};

int W= 1920, H=1080;
const float PI = 3.14159f;
std::vector<node> nodesData;
std::vector<std::pair<int, int> > linesData;

//check if the line defined by 2 argument points interceprs with any other lines
bool intercepts_with_lines(int loc1, int loc2) {
    using namespace std;
    const float WW = 0.1f; // precision

    //shortening the names just so its easier to work with
    float x1 = nodesData[loc1].x;
    float y1 = nodesData[loc1].y;
    float x2 = nodesData[loc2].x;
    float y2 = nodesData[loc2].y;

    for(int i = 0; i < linesData.size(); i++){
        int p3 = linesData[i].first;
        int p4 = linesData[i].second;

        //interception check exception
        if (loc1 == p3 || loc1 == p4 || loc2 == p3 || loc2 == p4) continue;

        float x3 = nodesData[p3].x;
        float y3 = nodesData[p3].y;
        float x4 = nodesData[p4].x;
        float y4 = nodesData[p4].y;

        //y = ax + b
        //L - Location line, T - Target line
        float aL = (y2-y1) / (x2-x1);
        float bL = (isinf(aL)) ? x1 : y1 - aL*x1;
        float aT = (y4-y3) / (x4-x3);
        float bT = (isinf(aT)) ? x3 : y3 - aT*x3;

        float x = (bT-bL) / (aL-aT);
        float yT = (isinf(aT)) ? bT : aT*x+bT;
        float yL = (isinf(aL)) ? bL : aL*x+bT;

        //interception checks
        if (abs(x1-x2)<WW && min(x3,x4)<min(x1,x2) && max(x3,x4)>max(x1,x2) && yT>min(y1,y2) && yT<max(y1,y2)) return true;
        if (abs(x3-x4)<WW && min(x1,x2)<min(x3,x4) && max(x1,x2)>max(x3,x4) && yL>min(y3,y4) && yL<max(y3,y4)) return true;
        if (x-min(x1,x2)>WW && x-min(x3,x4)>WW && max(x1,x2)-x>WW && max(x3,x4)-x>WW) return true;
    }

    return false;
}

void update_nodes_data() {
    using namespace std;
    //position update
    for(int i = 0; i < nodesData.size(); i++){
        float yChange = sin(nodesData[i].angle*2*PI/360) * nodesData[i].velocity;
        float xChange = cos(nodesData[i].angle*2*PI/360) * nodesData[i].velocity;

        if (yChange + nodesData[i].y < 0){
            nodesData[i].angle *= -1;
            nodesData[i].y = abs(yChange+nodesData[i].y);
        }
        else if (yChange + nodesData[i].y > H){
            nodesData[i].angle *= -1;
            nodesData[i].y = 2*H-(yChange+nodesData[i].y);
        }
        else nodesData[i].y += yChange;

        if (xChange + nodesData[i].x < 0){
            nodesData[i].angle = 180.0f - nodesData[i].angle;
            nodesData[i].x = abs(xChange+nodesData[i].x);
        }
        else if (xChange + nodesData[i].x > W){
            nodesData[i].angle = 180.0f - nodesData[i].angle;
            nodesData[i].x = 2*W-(xChange+nodesData[i].x);
        }
        else nodesData[i].x += xChange;
    }
    linesData.clear();

    //connects nodes
    struct lineSpecial {
        int p1, p2;
        float length;
    };

    vector<lineSpecial> v;
    for (int i = 0; i < nodesData.size(); i++){
        for (int j = i+1; j < nodesData.size(); j++){
            lineSpecial l;
            l.p1 = i; l.p2 = j;
            l.length = sqrt(pow(nodesData[i].x-nodesData[j].x, 2) + pow(nodesData[i].y-nodesData[j].y, 2));
            v.emplace_back(l);
        }
    }
    sort(v.begin(), v.end(), [](lineSpecial& b1, lineSpecial& b2) -> bool {return b1.length < b2.length;} );

    for(int i = 0; i < v.size(); i++)
        if (!intercepts_with_lines(v[i].p1, v[i].p2))
            linesData.emplace_back(make_pair(v[i].p1, v[i].p2));

    return;
}

// generates random nodes
void node_spawner(int nodeAmmount){
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

    //called so that nodes are connected from the start
    update_nodes_data();

    return;
}

void window_setup() {
    using namespace sf;

    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow window(VideoMode(W, H, desktop.bitsPerPixel), "Polysim");
    View view(Vector2f(W/2, H/2), Vector2f(W, H));
    window.setFramerateLimit(12);
    while(window.isOpen()){

        Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed) window.close();
        }

        window.clear();
        window.setView(view);
        // CircleShape dot(2.0f);
        // for(int i = 0; i < nodesData.size(); i++){
        //     dot.setPosition(nodesData[i].x-1, nodesData[i].y-1);
        //     window.draw(dot);
        // }

        for(int i = 0; i < linesData.size(); i++){
            float x1 = nodesData[linesData[i].first].x;
            float y1 = nodesData[linesData[i].first].y;
            float x2 = nodesData[linesData[i].second].x;
            float y2 = nodesData[linesData[i].second].y;
            Vertex line[] = {Vertex(Vector2f(x1, y1)), Vertex(Vector2f(x2, y2))};
            window.draw(line, 2, Lines);
        }

        update_nodes_data();
        window.display();
    }
}

int main() {
    node_spawner(100);
    window_setup();
    return 0;
}
