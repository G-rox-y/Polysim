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

struct triangle {
    int p1, p2, p3;
    float center;
};
std::vector<triangle> trianglesData;


bool intercepts_with_lines(int loc1, int loc2, std::vector<std::pair<int, int> >* lineAdress) {
    using namespace std;
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
            if (abs(x3-x4) < WW && (max(min(y1, y2), min(y3, y4)) <= min(max(y1, y2), max(y3, y4)))) return true;
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

void update_nodes_position() {
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
    return;
}

void update_nodes_data() {
    using namespace std;

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

    //add border lines
    int endpt = nodesData.size();
    while(!nodesData[endpt-1].type) endpt--;
    linesData.emplace_back(make_pair(endpt, nodesData.size()-1));
    for(int i = endpt+1; i < nodesData.size(); i++)
        linesData.emplace_back(make_pair(i-1, i));
    
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

            if (linesData[sus].first == m && linesData[sus].second == M){
                triangle t;
                t.p1 = linesData[i].first; t.p2 = m; t.p3 = M;
                t.center = (nodesData[t.p1].y + nodesData[t.p2].y + nodesData[t.p3].y)/3;
                trianglesData.emplace_back(t);
            }
        }
    }
    return;
}

void node_spawner(int nodeAmm){
    using namespace std;

    //rng setup
    random_device dev;
    mt19937 rng(dev());
    uniform_real_distribution<> randomX(0.0f, (float)W);
    uniform_real_distribution<> randomY(0.0f, (float)H);
    uniform_real_distribution<> randomVel(0.2f, 0.7f);
    uniform_real_distribution<> randomAng(0.0f, 360.0f);

    //determine node distribution
    int outerAmm = max((W+H)/400, 1);
    int insideAmm = nodeAmm-4*outerAmm;

    // generate inner nodes (type 1)
    for(int i=0; i< insideAmm; i++){
        node n;
        n.x = randomX(rng);
        n.y = randomY(rng);
        n.velocity = randomVel(rng);
        n.angle = randomAng(rng);
        n.type = 1;
        nodesData.emplace_back(n);
    }

    //generate outer nodes (type 0)
    for(int i = 0; i < outerAmm; i++){
        node n;
        n.x = W*(float)i/outerAmm;
        n.y = 0;
        n.type = 0;
        nodesData.emplace_back(n);
    }
    for(int i = 0; i < outerAmm; i++){
        node n;
        n.x = W;
        n.y = H*(float)i/outerAmm;
        n.type = 0;
        nodesData.emplace_back(n);
    }
    for(int i = 0; i < outerAmm; i++){
        node n;
        n.x = W*(1-(float)i/outerAmm);
        n.y = H;
        n.type = 0;
        nodesData.emplace_back(n);
    }
    for(int i = 0; i < outerAmm; i++){
        node n;
        n.x = 0;
        n.y = H*(1-(float)i/outerAmm);
        n.type = 0;
        nodesData.emplace_back(n);
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
    window.setFramerateLimit(60);

    //important wariables
    View view(Vector2f(W/2, H/2), Vector2f(W, H));
    Font font; font.loadFromFile("OpenSans-Light.ttf");
    bool nodesOn = false, isPaused = false;

    // fps variables
    std::deque<int> fpsQueue;
    int fpsQuery = 0;

    //color variables
    Clock clock, colorReductor;
    std::vector<int> colors = {0, 255, 0};
    std::pair<int, bool> currentColor = {1, true};
    // ^ first value is the elemet index, and second is if its being reduced or not

    //window
    while(window.isOpen()){

        Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed) window.close();
            if(event.type == Event::KeyReleased){
                if (event.key.code == Keyboard::N) nodesOn = !nodesOn;
                if (event.key.code == Keyboard::P) isPaused = !isPaused;
            }
        }

        //changes between frames
        if (!isPaused){
            // node changes
            update_nodes_position();
            update_nodes_data();

            //update triangle colors
            if (colorReductor.getElapsedTime().asMilliseconds() > 100){
                colorReductor.restart();
                if (colors[currentColor.first] == 255) currentColor.second = true;
                else if (colors[currentColor.first] <= colors[(currentColor.first+1)%3]){
                    currentColor.first = (currentColor.first+1)%3;
                    currentColor.second = false;
                }
                if (currentColor.second){
                    colors[(currentColor.first+1)%3]++;
                    colors[currentColor.first]--;
                }
                else{
                    colors[(currentColor.first+2)%3]--;
                    colors[currentColor.first]++;
                }
            }
        }

        window.clear();
        window.setView(view);

        //draw triangles
        ConvexShape trokut;
        trokut.setPointCount(3);
        for(int i = 0; i < trianglesData.size(); i++){
            trokut.setPoint(0, Vector2f(nodesData[trianglesData[i].p1].x, nodesData[trianglesData[i].p1].y));
            trokut.setPoint(1, Vector2f(nodesData[trianglesData[i].p2].x, nodesData[trianglesData[i].p2].y));
            trokut.setPoint(2, Vector2f(nodesData[trianglesData[i].p3].x, nodesData[trianglesData[i].p3].y));

            float colorFactor = trianglesData[i].center/H;
            trokut.setFillColor(Color(colorFactor*colors[0], colorFactor*colors[1], colorFactor*colors[2]));
            window.draw(trokut);
        }

    	//draw nodes(this is for easier debugging)
        if (nodesOn){
            CircleShape dot(4.0f);
            dot.setFillColor(Color(255-colors[0], 255-colors[1], 255-colors[2]));
            for(int i = 0; i < nodesData.size(); i++){
                dot.setPosition(nodesData[i].x-1, nodesData[i].y-1);
                window.draw(dot);
            }
        }

        //fps counter
        Text text("", font);
        text.setFillColor(Color::White);
        text.setCharacterSize(24);
        text.setPosition(Vector2f(5, 5));
        int time = 1.0f/clock.getElapsedTime().asSeconds();
        clock.restart();
        if (!isPaused){
            fpsQueue.push_back(time);
            fpsQuery += time;
            if (fpsQueue.size() == 10){
                fpsQuery -= fpsQueue.front();
                fpsQueue.pop_front();
                text.setString("FPS: " + std::to_string(fpsQuery/10));
            }
            else text.setString("FPS: ...");
        }
        else text.setString("Paused");
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
// optimise algorithmical complexity and performance
//  - maybe update lines every so often (not with every frame)
//  - you could try implementing multithreading or async
// try to figure why sometimes triangles glitch for a splitsecond
// left click = add node, right click = remove node
// add forces? (to reduce empty spaces)
// update readme
// make a slider that controls color/node speed