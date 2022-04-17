#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

// to make vscode errors go away
#include <random>


int W= 1440, H=920;
const float PI = 3.14159f;

struct node {
    float x, y, velocity, angle;
    int type;
    // type == 1 point constrained inside the window
    // type == 0 outside window and static
};

std::vector<node> nodesData;
std::vector<std::tuple<int, int, int> > trianglesData;

bool intercepts_with_lines(int loc1, int loc2, std::vector<std::pair<int, int> >* lineAdress) {
    using namespace std;
    vector<pair<int, int> > linesData = *lineAdress;
    const float WW = 0.001f; // precision

    //shortening the names just so its easier to work with
    float x1 = nodesData[loc1].x;
    float y1 = nodesData[loc1].y;
    float x2 = nodesData[loc2].x;
    float y2 = nodesData[loc2].y;

    for(int i = 0; i < (*lineAdress).size(); i++){
        int p3 = (*lineAdress)[i].first;
        int p4 = (*lineAdress)[i].second;

        //interception check exception
        if (loc1 == p3 || loc1 == p4 || loc2 == p3 || loc2 == p4) continue;

        float x3 = nodesData[p3].x;
        float y3 = nodesData[p3].y;
        float x4 = nodesData[p4].x;
        float y4 = nodesData[p4].y;

        //y = ax + b
        //L - Location line, T - Target line
        float aL = (y2-y1) / (x2-x1);
        float bL = y1 - aL*x1;
        float aT = (y4-y3) / (x4-x3);
        float bT = y3 - aT*x3;

        //special case check
        if (abs(x1-x2) < WW){
            float y = aT*x1+bT;
            if (y > min(y3,y4) && y < max(y3,y4)) return true;
        }
        if (abs(x3-x4) < WW){
            float y = aL*x3+bL;
            if (y > min(y1, y2) && y < max(y1, y2)) return true;
        }
        
        //general case check
        float x = (bT-bL) / (aL-aT);
        if (x > min(x1, x2) && x > min (x3, x4) && x < max(x1, x2) && x < max(x3, x4)) return true;
    }

    return false;
}

void update_nodes_data() {
    using namespace std;

    //position update
    for(int i = 0; i < nodesData.size(); i++){
        if (!nodesData[i].type) continue;
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

    //creating node connections
    struct lineSpecial {
        int p1, p2;
        float length;
    };
    vector<lineSpecial> v;
    for (int i = 0; i < nodesData.size(); i++){
        for (int j = i+1; j < nodesData.size(); j++){
            if (!nodesData[i].type && !nodesData[j].type) continue;
            lineSpecial l;
            l.p1 = i; l.p2 = j;
            l.length = sqrt(pow(nodesData[i].x-nodesData[j].x, 2) + pow(nodesData[i].y-nodesData[j].y, 2));
            v.emplace_back(l);
        }
    }

    //filtering the node connections
    std::vector<std::pair<int, int> > linesData;
    sort(v.begin(), v.end(), [](lineSpecial& b1, lineSpecial& b2) -> bool {
        return b1.length < b2.length;
    });
    for(int i = 0; i < v.size(); i++)
        if (!intercepts_with_lines(v[i].p1, v[i].p2, &linesData))
            linesData.emplace_back(make_pair(v[i].p1, v[i].p2));

    //create triangles
    trianglesData.clear();
    sort(linesData.begin(), linesData.end());
    for(int i = 0; i < linesData.size(); i++){
        for(int j = i+1; j < linesData.size(); j++){
            if (linesData[j].first!=linesData[i].first) break;
            
            int sus = 0;
            int m = min(linesData[i].second, linesData[j].second);
            int M = max(linesData[i].second, linesData[j].second);
            for(int b = linesData.size()-1; b > 0; b/=2)
                while(b+sus < linesData.size() && (linesData[b+sus].first < m || (linesData[b+sus].second <= M && linesData[b+sus].first == m)))
                    sus += b;

            if (linesData[sus].first == m && linesData[sus].second == M)
                trianglesData.emplace_back(make_tuple(linesData[i].first, m, M));
        }
    }
    return;
}

void node_spawner(int insideNodeAmmount){
    using namespace std;

    //rng setup
    random_device dev;
    mt19937 rng(dev());
    uniform_real_distribution<> randomX(0.0f, (float)W);
    uniform_real_distribution<> randomY(0.0f, (float)H);
    uniform_real_distribution<> randomVel(0.2f, 0.7f);
    uniform_real_distribution<> randomAng(0.0f, 360.0f);

    //generate inner nodes (type 1)
    for(int i=0; i< insideNodeAmmount; i++){
        node n;
        n.x = randomX(rng);
        n.y = randomY(rng);
        n.velocity = randomVel(rng);
        n.angle = randomAng(rng);
        n.type = 1;
        nodesData.emplace_back(n);
    }

    //generate outer nodes (type 0)
    for(int i = 0; i <= 5; i++){
        node n1, n2, n3, n4;

        n1.x = W*i/10;
        n1.y = -10;
        n1.type = 0;
        nodesData.emplace_back(n1);
        
        n2.x = W*i/10;
        n2.y = H+10;
        n2.type = 0;
        nodesData.emplace_back(n2);
        
        n3.y = H*i/10;
        n3.x = -10;
        n3.type = 0;
        nodesData.emplace_back(n3);

        n4.y = H*i/10;
        n4.x = W+10;
        n4.type = 0;
        nodesData.emplace_back(n4);
    }

    return;
}

void window_setup() {
    using namespace sf;
    
    //window setup
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow window(VideoMode(W, H, desktop.bitsPerPixel), "Polysim", Style::Default, settings);
    window.setFramerateLimit(24);

    //important wariables
    View view(Vector2f(W/2, H/2), Vector2f(W*2/3, H*2/3));
    Font font; font.loadFromFile("OpenSans-Light.ttf");
    Clock clock;
    
    //window
    while(window.isOpen()){

        Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed) window.close();
        }

        update_nodes_data();

        window.clear();
        window.setView(view);

        // //draw nodes
        // CircleShape dot(2.0f);
        // for(int i = 0; i < nodesData.size(); i++){
        //     dot.setPosition(nodesData[i].x-1, nodesData[i].y-1);
        //     window.draw(dot);
        // }

        //draw triangles
        ConvexShape trokut;
        trokut.setPointCount(3);
        for(int i = 0; i < trianglesData.size(); i++){
            float x1 = nodesData[std::get<0>(trianglesData[i])].x;
            float y1 = nodesData[std::get<0>(trianglesData[i])].y;
            trokut.setPoint(0, Vector2f(x1, y1));
            float x2 = nodesData[std::get<1>(trianglesData[i])].x;
            float y2 = nodesData[std::get<1>(trianglesData[i])].y;
            trokut.setPoint(1, Vector2f(x2, y2));
            float x3 = nodesData[std::get<2>(trianglesData[i])].x;
            float y3 = nodesData[std::get<2>(trianglesData[i])].y;
            trokut.setPoint(2, Vector2f(x3, y3));

            int red = 60, green = 255, blue = 40;
            float colorFactor = (y1+y2+y3)/3/H;
            trokut.setFillColor(Color(colorFactor*red, colorFactor*green, colorFactor*blue));
            window.draw(trokut);
        }

        //fps counter
        int time = 1.0f/clock.getElapsedTime().asSeconds();
        clock.restart();
        Text text("FPS: " + std::to_string(time), font);
        text.setFillColor(Color::White);
        text.setCharacterSize(24);
        text.setPosition(Vector2f(W/6, H/6));
        window.draw(text);

        window.display();

    }
}

int main() {
    node_spawner(100);
    window_setup();
    return 0;
}


// todo
// optimise algorithmical complexity
//  - maybe update lines every so often (not with every frame)
// error handling
// handle resizing and add some interactivity or sth
// when 2 vertical lines are one above the other they cause a bug (fix it)