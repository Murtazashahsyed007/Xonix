#include <SFML/Graphics.hpp>
#include<time.h>
#include<sstream>
#include<fstream>

using namespace sf;
using namespace std;

// Game constants
const int M = 25;    // Number of rows 
const int N = 40;    // Number of columns 
int grid[M][N] = {0};// 2D array representing the game grid (0 = empty, 1 = claimed, 3= trail)
int ts = 18;         // Tile size in pixels
const int MAX_ENEMIES = 20; // Maximum number of enemies

//variables
int scoreP1 = 0;   // p1 score
int scoreP2 = 0;      // Current player scores
int highScore = 0;    // Highest score achieved
float areaPercentage = 0; // Percentage of area claimed
bool areaChanged = false; // Flag indicating if area changed
int moveCountP1 = 0;  // Movement counter for Player 1
int moveCountP2 = 0;  // Movement counter for Player 2
bool controlsP1 = true; // means k player 1 k controls enable ha and same for 2 
bool controlsP2 = true;  
bool p1Active = true;    // mean k player 1 active ha initially and same for player 2
bool p2Active = true;   
// ye neeche speed ko increase krne ki logic lgayi thi jisse code dump hojata hai ajeeb
/*const float SPEED_INCREASE_INTERVAL = 20.0f;  // Time between speed increases (20 seconds)
const float SPEED_INCREASE_AMOUNT = 0.2f;    // How much to increase speed each time (20%)
const float MAX_SPEED_MULTIPLIER = 3.0f;     // Maximum speed multiplier
float enemySpeedMultiplier = 1.0f;  // Current speed multiplier
float enemySpeedTimer = 0.0f;      // Timer for speed increases
*/// Enemy structure definition
struct Enemy
{
    int x, y;     // Current position
    int dx, dy;   // Movement direction
    
    //enemy at random position with random direction
    Enemy()
    {
        x = y = 300;  // Starting position
        dx = 4 - rand() % 8;  // Random x direction
        dy = 4 - rand() % 8;  // Random y direction
    }

    // Movement function
    void move()
    {
        x += dx;//*enemySpeedMultiplier;  // Move in x direction
        
        // Check for collision with walls (grid value 1)
        if (grid[y / ts][x / ts] == 1)
        {
            dx = -dx;  // Reverse x direction
            x += dx;//*enemySpeedMultiplier;   // Move back
        }
        
        y += dy;//*enemySpeedMultiplier;  // Move in y direction
        
        // Check for collision with walls
        if (grid[y / ts][x / ts] == 1)
        {
            dy = -dy;  // Reverse y direction
            y += dy;//*enemySpeedMultiplier;   // Move back
        }
    }
};

// Bubble sort for score and also sort their time too
void bubbleSortScores(int scores[], float times[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j] < scores[j + 1]) {
                // maine inbuild use kiye hai tu inko 
                // conventional way mai krlena ok
                swap(scores[j], scores[j + 1]);
                swap(times[j], times[j + 1]);
            }
        }
    }
}

// Recursive function to mark unclaimed areas
void drop(int y, int x)
{
    // Mark current cell if unclaimed
    if (grid[y][x] == 0)
        grid[y][x] = -1;
    
    // Recursively check and mark adjacent cells
    if (grid[y - 1][x] == 0) drop(y - 1, x);  // Up
    if (grid[y + 1][x] == 0) drop(y + 1, x);  // Down
    if (grid[y][x - 1] == 0) drop(y, x - 1);  // Left
    if (grid[y][x + 1] == 0) drop(y, x + 1);  // Right
}

// Calculate area claimed by a specific player
void calculateAreaClaimedByPlayer(int playerNum, int& playerScore, bool active)
{
    // If players are not active, don't update their score
    if (!active) {
        return;
    }
    
    int totalArea = (M - 2) * (N - 2); // Total playable area except borders 
    int claimedArea = 0;               // Counter for claimed area
    
    // Count all claimed cells 
    // ye logic already code mai thi bss usko replicate kia ha
    for (int i = 1; i < M - 1; i++)
        for (int j = 1; j < N - 1; j++)
            if (grid[i][j] == 1)
                claimedArea++;
    
    // Calculate new percentage and iski basis pr score calculate hoga 8.7 ka maqsad agar boundary nikalo tu andar total 870 indexes rhti hai tu iss wajah se 8.7 se multiply kia 
    float newPercentage = ((claimedArea * 100.0f) / totalArea) * 8.7f;
    playerScore = static_cast<int>(newPercentage);
    
    // Update area percentage for display
    if (playerNum == 1 || playerNum == 0) {  // 0 means single player mode and 1 mean two player mode
        areaPercentage = newPercentage;
        areaChanged = true;
    }
}

// Reset player position function
/* logic bss aik function bnya aur jo varibles ki need thi //unko //as reference pass kia parameter mai aur unki jo original poistioning thi function k andar likh li*/
void resetPlayerPosition(int& x, int& y, int& dx, int& dy, bool isPlayer1)
{
    dx = dy = 0;   // Reset movement direction
    
    if (isPlayer1) {
        x = 0;     // Reset Player 1 to top-left corner
        y = 0;
    } else {
        x = N - 1; // Reset Player 2 to bottom-right corner
        y = M - 1;
    }
}

// Reset game state
// same jo reset position ki logic ha whi ha
void resetGame(int& x, int& y, int& dx, int& dy, 
               int& x2, int& y2, int& dx2, int& dy2, 
               Enemy a[], int enemyCount, bool& gameOver, 
               bool& showEndMenu, Clock& gameClock)
{
    // Reset player 1 position 
    x = y = 0;    // originally given
    dx = dy = 0;   // same as above
    
    // Reset player 2 position and direction 
    x2 = N - 1;    // bottom right 
    y2 = M - 1;    
    dx2 = dy2 = 0; // No movement
    
    // grid bnane ki logic given thi bss replicate kia 
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
    
    // Reset enemies
    for (int i = 0; i < enemyCount; i++)
    {
        a[i].x = a[i].y = 300;  // Reset position
        a[i].dx = 4 - rand() % 8; // 
        a[i].dy = 4 - rand() % 8; // 
    }
    
    // Reset game state flags
    gameOver = false;
    showEndMenu = false;
    gameClock.restart();  // Reset game timer inbuild use howa tha
    
    // Reset scoring and counters
    scoreP1 = 0;
    scoreP2 = 0;
    moveCountP1 = 0;
    moveCountP2 = 0;
    areaPercentage = 0;
    areaChanged = false;
    
    // Reset player controls and activity status
    controlsP1 = true;
    controlsP2 = true;
    p1Active = true;
    p2Active = true;
    // reset enemy speed variables 
   // speed_multipier=1;
    //speed_timer=0;
}

// Main game function
int main()
{
    // random number generator ha 
    srand(time(0));
    
    // ise window generate hoti ha 
    RenderWindow window(VideoMode(N * ts, M * ts), "Xonix Game!");
    window.setFramerateLimit(60);  // and 60 ka matlb 60 fps ha

    // Load textures
    Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");   // Tile textures
    t3.loadFromFile("images/enemy.png");   // Enemy sprite

    // Create sprites
    Sprite sTile(t1), sEnemy(t3);
    sEnemy.setOrigin(20, 20);         // Set enemy sprite origin to center

    // Load font
    Font font;
    if (!font.loadFromFile("font/fonts.ttf")) {
        return EXIT_FAILURE;  // Exit if font fails to load
    }

    // ye sab text ha jo screen pe diplay krna hai 
    // unka ye syntax ha
    Text title("Xonix Game!", font, 50);
    title.setPosition(100, 50);
    title.setFillColor(Color::White);

    Text option1("1: Single Player", font, 30);
    option1.setPosition(100, 150);
    option1.setFillColor(Color::Green);

    Text option2("2: Two Players", font, 30);
    option2.setPosition(100, 200);
    option2.setFillColor(Color::Yellow);
    
    Text option3("3: Scoreboard", font, 30);
    option3.setPosition(100, 250);
    option3.setFillColor(Color::Cyan);
    
    Text diffText("Choose Mode: E M H S", font, 30);
    diffText.setPosition(100, 300);
    diffText.setFillColor(Color::Cyan);

    Text timeText("Time: 0", font, 20);
    timeText.setPosition(250, 10);
    timeText.setFillColor(Color::Green);

    Text scoreTextP1("Score P1: 0", font, 15);
    scoreTextP1.setPosition(10, 40);
    scoreTextP1.setFillColor(Color::Green);
    
    Text scoreTextP2("Score P2: 0", font, 15);
    scoreTextP2.setPosition(500, 40);
    scoreTextP2.setFillColor(Color::Yellow);
    
    // Movement counter 
    Text moveTextP1("P1 Moves: 0", font, 15);
    moveTextP1.setPosition(10, 70);
    moveTextP1.setFillColor(Color::Green);
    
    Text moveTextP2("P2 Moves: 0", font, 15);
    moveTextP2.setPosition(500, 70);
    moveTextP2.setFillColor(Color::Yellow);
    
    // Player status texts
    Text statusP1("P1: Active", font, 15);
    statusP1.setPosition(10, 100);
    statusP1.setFillColor(Color::Green);
    
    Text statusP2("P2: Active", font, 15);
    statusP2.setPosition(500, 100);
    statusP2.setFillColor(Color::Yellow);

    Text resultText("", font, 30);
    resultText.setPosition(190, 170);
    resultText.setFillColor(Color::White);

    Text finalScoreP1("Player 1 Score: 0", font, 30);
    finalScoreP1.setPosition(190, 80);
    finalScoreP1.setFillColor(Color::Green);
    
    Text finalScoreP2("Player 2 Score: 0", font, 30);
    finalScoreP2.setPosition(190, 120);
    finalScoreP2.setFillColor(Color::Yellow);

    Text RestartText("Press R to Restart", font, 30);
    RestartText.setPosition(190, 220);
    RestartText.setFillColor(Color::Red);
    
    Text MainText("Press M for Main Menu", font, 30);
    MainText.setPosition(190, 270);
    MainText.setFillColor(Color::Yellow);
    
    Text ExitText("Press E to Exit", font, 30);
    ExitText.setPosition(190, 320);
    ExitText.setFillColor(Color::Green);

    // Create scores file 
    // maam wali logic 
    {
        ifstream testFile("scores.txt");
        if (!testFile.good()) {
            ofstream createFile("scores.txt");
            createFile.close();
        }
        testFile.close();
    }

    // Game state variables
    bool inMenu = true;
    bool chooseDifficulty = false;
    bool isTwoPlayer = false;
    bool gameOver = false;
    bool showEndMenu = false;
    bool continuousMode = false;
    bool showScoreboard = false;
    float enemySpawnTimer = 0;
    
    // Player variables
    int x = 0, y = 0, dx = 0, dy = 0;
    int x2 = N - 1, y2 = M - 1, dx2 = 0, dy2 = 0;
    
    // Game timing
    float timer = 0;
    const float delay = 0.07f;
    Clock clock;
    Clock gameClock;

    // Enemy variables
    int enemyCount = 2; // initially two enemies ko rkha 
    Enemy a[MAX_ENEMIES]; // aik array bnayi enemy ki yahan 
    // int waghera islye nhi lagaya kyu k ye aik structure hai 
    // jo already define ha

    // is se borders bnte ha
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    // asal kaam yahan hoga
    while (window.isOpen())
    {
        float time = clock.restart().asSeconds();
        timer += time;
        float gameTime =    gameClock.getElapsedTime().asSeconds(); // ye float game k 
        // aage jo code line likhi ha wo inbuild ha 
        // ye time ko show krwati ha

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();
        }

        if (inMenu)
        {
            window.clear();
            window.draw(title);// ye title ko draw krrha 
            window.draw(option1);// draw option1 on screen
            window.draw(option2);// same as above
            window.draw(option3);
            if (chooseDifficulty)// ye simple si logic ha 
            // jo game flow ko represent krrhi ha 
            // iska voice note send kro ga
                window.draw(diffText);
            window.display();

            if (Keyboard::isKeyPressed(Keyboard::Num1))
            {
                isTwoPlayer = false;
                chooseDifficulty = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Num2))
            {
                isTwoPlayer = true;
                chooseDifficulty = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Num3))
            {
                showScoreboard = true;
                inMenu = false;
            }

            if (chooseDifficulty)
            {
                if (Keyboard::isKeyPressed(Keyboard::E))
                {
                    enemyCount = 2;
                    inMenu = false;
                    continuousMode = false;
                    resetGame(x, y, dx, dy, x2, y2, dx2, dy2, a, enemyCount, gameOver, showEndMenu, gameClock);
                }
                else if (Keyboard::isKeyPressed(Keyboard::M))
                {
                    enemyCount = 4;
                    inMenu = false;
                    continuousMode = false;
                    resetGame(x, y, dx, dy, x2, y2, dx2, dy2, a, enemyCount, gameOver, showEndMenu, gameClock);
                }
                else if (Keyboard::isKeyPressed(Keyboard::H))
                {
                    enemyCount = 6;
                    inMenu = false;
                    continuousMode = false;
                    resetGame(x, y, dx, dy, x2, y2, dx2, dy2, a, enemyCount, gameOver, showEndMenu, gameClock);
                }
                else if (Keyboard::isKeyPressed(Keyboard::S))
                {
                    enemyCount = 2;
                    inMenu = false;
                    continuousMode = true;
                    resetGame(x, y, dx, dy, x2, y2, dx2, dy2, a, enemyCount, gameOver, showEndMenu, gameClock);
                    enemySpawnTimer = 0;
                }
            }
            continue;
        }

        // Scoreboard display
        if (showScoreboard) {
            int scores[5] = {0}; // atleast 5 scores 
            float times[5] = {0}; // with their respective time
            int count = 0;// count must be less than 5
            
            // Read scores from file
            ifstream file("scores.txt");
            if (file.is_open()) {
                while (file >> scores[count] >> times[count] && count < 5) {
                    count++;
                }
                file.close();
            }
            
            
            RenderWindow scoreWindow(VideoMode(600, 500), "Scoreboard");
            // yahan score board kilye aik alag window bnayi 
            // opar se iski logic ko replcaye kia bss
            // khair sfml ki website se bhi pata lag gya tha
            
            Text title("Top Scores", font, 40);
            // same whi title text likha screen pr
            title.setPosition(200, 50);
            title.setFillColor(Color::Cyan);
            
            Text scoreTexts[5];
            // score ko diplay krne kilye loop lagaya 
            for (int i = 0; i < count; i++) {
                stringstream ss;
                ss << (i+1) << ". Score: " << scores[i] 
                   << " Time: " << static_cast<int>(times[i])/60 << "m " << static_cast<int>(times[i])%60 << "s";
              
                   
                scoreTexts[i].setString(ss.str());
                scoreTexts[i].setFont(font);
                scoreTexts[i].setCharacterSize(25);
                scoreTexts[i].setPosition(100, 120 + i * 60);
                scoreTexts[i].setFillColor(Color::White);
              
            }
            
            Text backText("Press B to go back", font, 30);
            backText.setPosition(200, 400);
            backText.setFillColor(Color::Yellow);
            // option appear hoga to exit scoreboard window
            
            while (scoreWindow.isOpen()) {
                Event e;
                while (scoreWindow.pollEvent(e)) {
                    if (e.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::B)) {
                        scoreWindow.close();
                        showScoreboard = false;
                        inMenu = true;
                    }
                }
                
                scoreWindow.clear();
                scoreWindow.draw(title);
                for (int i = 0; i < count; i++) {
                    scoreWindow.draw(scoreTexts[i]);
                }
                scoreWindow.draw(backText);
                scoreWindow.display();
            }
            continue;
        }
     // Update enemy speed timer
//enemySpeedTimer += time;

// Increase speed every 20 seconds
/*if (enemySpeedTimer >= SPEED_INCREASE_INTERVAL) {
    enemySpeedMultiplier += SPEED_INCREASE_AMOUNT;
    enemySpeedTimer = 0; // Reset timer
    
    // Cap the maximum speed
    if (enemySpeedMultiplier > MAX_SPEED_MULTIPLIER) {
        enemySpeedMultiplier = MAX_SPEED_MULTIPLIER;
    }
}     
     */   if (isTwoPlayer) {
            stringstream ss;

            ss << "P1: ";
            if (p1Active) {
                ss << "Active";
            } else {
                ss << "Inactive";
            }
            statusP1.setString(ss.str());

            if (p1Active) {
                statusP1.setFillColor(Color::Green);
            } else {
                statusP1.setFillColor(Color::Red);
            }

            ss.str("");
            ss.clear(); // clear any flags

            ss << "P2: ";
            if (p2Active) {
                ss << "Active";
            } else {
                ss << "Inactive";
            }
            statusP2.setString(ss.str());

            if (p2Active) {
                statusP2.setFillColor(Color::Yellow);
            } else {
                statusP2.setFillColor(Color::Red);
            }
        }

        // Continuous mode enemy spawning
        if (continuousMode)
        {
            enemySpawnTimer += time;
            if (enemySpawnTimer >= 20.0f && enemyCount < MAX_ENEMIES)
            {
                a[enemyCount] = Enemy();
                a[enemyCount+1] = Enemy();
                enemyCount += 2;
                enemySpawnTimer = 0;
            }
        }

        // Calculate and update scores
        if (!isTwoPlayer) {
            calculateAreaClaimedByPlayer(0, scoreP1, true);  // Single player mode
            
            stringstream ss;
            ss << "Score: " << scoreP1;
            scoreTextP1.setString(ss.str());
            
            ss.str("");
            ss << "P1 Moves: " << moveCountP1;
            moveTextP1.setString(ss.str());
        } else {
            calculateAreaClaimedByPlayer(1, scoreP1, p1Active);  // Player 1
            calculateAreaClaimedByPlayer(2, scoreP2, p2Active);  // Player 2
            
            stringstream ss;
            ss << "Score P1: " << scoreP1;
            scoreTextP1.setString(ss.str());
            
            ss.str("");
            ss << "Score P2: " << scoreP2;
            scoreTextP2.setString(ss.str());
            
            ss.str("");
            ss << "P1 Moves: " << moveCountP1;
            moveTextP1.setString(ss.str());
            
            ss.str("");
            ss << "P2 Moves: " << moveCountP2;
            moveTextP2.setString(ss.str());
        }
        
        stringstream ss;
        ss << "Time: "<< static_cast<int>(gameTime)/60 << "min " << static_cast<int>(gameTime)%60 << "sec ";
        timeText.setString(ss.str());

        float rotationSpeed = 10 + (gameTime / 10) * 5;
        if (rotationSpeed > 30) rotationSpeed = 30;
        sEnemy.rotate(rotationSpeed);
  

        // Check if both players are inactive in two-player mode
        if (isTwoPlayer && !p1Active && !p2Active) {
            gameOver = true;
        }

        if (gameOver)
        {
            // update scoreboard when game gets over
            int scores[5] = {0};
            float times[5] = {0};
            int count = 0;
            
            // Read scores
            ifstream inFile("scores.txt");
            if (inFile.is_open()) {
                while (inFile >> scores[count] >> times[count]  && count < 5) {
                    count++;
                }
                inFile.close();
            }
            
                       string mode;
                   if (isTwoPlayer) {
                        mode = "Two Player";
                     } else {
                    if (continuousMode) {
                       mode = "Survival";
                     } else {
                       mode = "Single";
                    }
                 }

            // Add new scores if they meet the condition
            bool p1Added = false, p2Added = false;
            if (count < 5 || scoreP1 > scores[4]) {
                if (count < 5) count++;
                scores[count-1] = scoreP1;
                times[count-1] = gameTime;
                p1Added = true;
            }
            
            if (isTwoPlayer && (count < 5 || scoreP2 > scores[4])) {
                if (count < 5) count++;
                scores[count-1] = scoreP2;
                times[count-1] = gameTime;
                p2Added = true;
            }
            
            // Sort scores
            if (p1Added || p2Added) {
                bubbleSortScores(scores, times, count);
            }
            
            // Write to file top 5 scores only
            ofstream outFile("scores.txt", ios::trunc|ios::app);
            if (outFile.is_open()) {
                for (int i = 0; i < min(count, 5); i++) {
                    outFile << scores[i] << " " << times[i] << " " << "\n";
                }
                outFile.close();
            }
            
            showEndMenu = true;
            window.clear();
            
            // Update final score texts
            ss.str("");
            ss << "Player 1 Score: " << scoreP1;
            finalScoreP1.setString(ss.str());
            
            if (isTwoPlayer) {
                ss.str("");
                ss << "Player 2 Score: " << scoreP2;
                finalScoreP2.setString(ss.str());
                
                if (scoreP1 > scoreP2) {
                    resultText.setString("Player 1 Wins!");
                    resultText.setFillColor(Color::Green);
                } else if (scoreP2 > scoreP1) {
                    resultText.setString("Player 2 Wins!");
                    resultText.setFillColor(Color::Yellow);
                } else {
                    resultText.setString("It's a tie!");
                    resultText.setFillColor(Color::White);
                }
                
                window.draw(finalScoreP2);
            } else {
                resultText.setString("Game Over!");
                resultText.setFillColor(Color::White);
            }
            
            window.draw(finalScoreP1);
            window.draw(resultText);
            window.draw(RestartText);
            window.draw(MainText);
            window.draw(ExitText);
            window.display();
            
            if (Keyboard::isKeyPressed(Keyboard::R)) {
                resetGame(x, y, dx, dy, x2, y2, dx2, dy2, a, enemyCount, gameOver, showEndMenu, gameClock);
                if (continuousMode) {
                    enemyCount = 2;
                    enemySpawnTimer = 0;
                }
            }
            else if (Keyboard::isKeyPressed(Keyboard::M)) {
                inMenu = true;
                chooseDifficulty = false;
                gameOver = false;
                showEndMenu = false;
                continuousMode = false;
                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++)
                        grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::E)) {
                window.close();
            }
            continue;
        }

        // Player 1 controls
        if (controlsP1) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) { dx = -1; dy = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::Right)) { dx = 1; dy = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::Up)) { dx = 0; dy = -1; }
            else if (Keyboard::isKeyPressed(Keyboard::Down)) { dx = 0; dy = 1; }
        }

        // Player 2 controls in two-player 
        if (isTwoPlayer && controlsP2)
        {
            if (Keyboard::isKeyPressed(Keyboard::A)) { dx2 = -1; dy2 = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::D)) { dx2 = 1; dy2 = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::W)) { dx2 = 0; dy2 = -1; }
            else if (Keyboard::isKeyPressed(Keyboard::S)) { dx2 = 0; dy2 = 1; }
        }

      
      
// COLLISION LOGICS
//===========================//
// Game state variables 
int rewardCounterP1 = 0;
int rewardCounterP2 = 0;
int tilesCapturedThisMoveP1 = 0;
int tilesCapturedThisMoveP2 = 0;

// Movement and scoring logic
if (timer > delay) {
    // Player 1 movement
    if (p1Active) {
        int prevX = x;
        int prevY = y;
        x += dx;
        y += dy;
        
        if ((x != prevX || y != prevY) && (dx != 0 || dy != 0)) {
            moveCountP1++;
            
            // Boundary  checks
            if (x < 0 || x >= N || y < 0 || y >= M) {
                if (isTwoPlayer) {
                    controlsP1 = false;
                    p1Active = false;
                    resetPlayerPosition(x, y, dx, dy, true);
                } else {
                    gameOver = true;
                }
                tilesCapturedThisMoveP1 = 0;
            }
            else if ( grid[y][x] == 2 || grid[y][x] == 3) {
                if (isTwoPlayer) {
                    controlsP1 = false;
                    p1Active = false;
                    resetPlayerPosition(x, y, dx, dy, true);
                } else {
                    gameOver = true;
                }
                tilesCapturedThisMoveP1 = 0;
            }
            else if (grid[y][x] == 0) {
                grid[y][x] = 2;
                tilesCapturedThisMoveP1++;
                
                // Progressive reward system
                if (rewardCounterP1 < 3) {
                    if (tilesCapturedThisMoveP1 >= 10) {
                        scoreP1 *= 2;
                        if (tilesCapturedThisMoveP1 == 10) {
                            rewardCounterP1++;
                        }
                    } else {
                        scoreP1 *= 1;
                    }
                }
                else if (rewardCounterP1 < 5) {
                    if (tilesCapturedThisMoveP1 >= 5) {
                        scoreP1 *= 2;
                        if (tilesCapturedThisMoveP1 == 5) {
                            rewardCounterP1++;
                        }
                    } else {
                        scoreP1 *= 1;
                    }
                }
                else {
                    if (tilesCapturedThisMoveP1 >= 5) {
                        scoreP1 += 4;
                        if (tilesCapturedThisMoveP1 == 5) {
                            rewardCounterP1++;
                        }
                    } else {
                        scoreP1 += 1;
                    }
                }
            }
        }
    }

    // Player 2 movement
    if (isTwoPlayer && p2Active) {
        int prevX2 = x2;
        int prevY2 = y2;
        x2 += dx2;
        y2 += dy2;
        
        if ((x2 != prevX2 || y2 != prevY2) && (dx2 != 0 || dy2 != 0)) {
            moveCountP2++;
            
            // Boundary checks
            if (x2 < 0 || x2 >= N || y2 < 0 || y2 >= M) {
                controlsP2 = false;
                p2Active = false;
                resetPlayerPosition(x2, y2, dx2, dy2, false);
                tilesCapturedThisMoveP2 = 0;
            }
            else if (grid[y2][x2] == 3 || grid[y2][x2] == 2) {
                controlsP2 = false;
                p2Active = false;
                resetPlayerPosition(x2, y2, dx2, dy2, false);
                tilesCapturedThisMoveP2 = 0;
            }
            else if (grid[y2][x2] == 0) {
                grid[y2][x2] = 3;
                tilesCapturedThisMoveP2++;
                
                //reward system
                if (rewardCounterP2 < 3) {
                    if (tilesCapturedThisMoveP2 >= 10) {
                        scoreP2 *= 2;
                        if (tilesCapturedThisMoveP2 == 10) {
                            rewardCounterP2++;
                        }
                    } else {
                        scoreP2 *= 1;
                    }
                }
                else if (rewardCounterP2 < 5) {
                    if (tilesCapturedThisMoveP2 >= 5) {
                        scoreP2 *= 2;
                        if (tilesCapturedThisMoveP2 == 5) {
                            rewardCounterP2++;
                        }
                    } else {
                        scoreP2 *= 1;
                    }
                }
                else {
                    if (tilesCapturedThisMoveP2 >= 5) {
                        scoreP2 += 4;
                        if (tilesCapturedThisMoveP2 == 5) {
                            rewardCounterP2++;
                        }
                    } else {
                        scoreP2 *= 1;
                    }
                }
            }
        }
    }

    // Reset move counters 
    if (dx == 0 && dy == 0) {
        tilesCapturedThisMoveP1 = 0;
    }
    if (dx2 == 0 && dy2 == 0) {
        tilesCapturedThisMoveP2 = 0;
    }

    timer = 0;
}
    
        for (int i = 0; i < enemyCount; i++)
            a[i].move();

        for (int i = 0; i < enemyCount; i++) {
            // Check enemy collides with player 1 
            if (p1Active && abs(a[i].x/ts - x/ts) < 1 && abs(a[i].y/ts - y/ts) < 1) {
                if (isTwoPlayer) {
                    // Disable player 1 controls and mark as inactive
                    controlsP1 = false;
                    p1Active = false;
                    resetPlayerPosition(x, y, dx, dy, true);
                } else {
                    gameOver = true;
                    break;
                }
            }
            
            if (isTwoPlayer && p2Active && abs(a[i].x/ts - x2/ts) < 1 && abs(a[i].y/ts - y2/ts) < 1) {
                // Disable player 2 controls and mark as inactive
                controlsP2 = false;
                p2Active = false;
                resetPlayerPosition(x2, y2, dx2, dy2, false);
            }
            
            // Check enemy collides with P1 trail
            if (p1Active && grid[a[i].y / ts][a[i].x / ts] == 2) {
                if (isTwoPlayer) {
                    // Disable player 1 controls and mark as inactive
                    controlsP1 = false;
                    p1Active = false;
                    resetPlayerPosition(x, y, dx, dy, true);
                } else {
                    gameOver = true;
                    break;
                }
            }
            
            // Check enemy collides with P2 trail
            if (isTwoPlayer && p2Active && grid[a[i].y / ts][a[i].x / ts] == 3) {
                // Disable player 2 controls and mark as inactive
                controlsP2 = false;
                p2Active = false;
                resetPlayerPosition(x2, y2, dx2, dy2, false);
            }
        }

        if (p1Active && grid[y][x] == 1)
        {
            dx = dy = 0;
            for (int i = 0; i < enemyCount; i++)
                drop(a[i].y / ts, a[i].x / ts); 
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    if(grid[i][j]==-1){
                      grid[i][j]=0;
                    }
                    else{
                      grid[i][j]=1;
                    }
        }
        
        if (isTwoPlayer && p2Active && grid[y2][x2] == 1)
        {
            dx2 = dy2 = 0; 
            // same as above
            for (int i = 0; i < enemyCount; i++)
                drop(a[i].y/ ts, a[i].x / ts);

    
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    if(grid[i][j]==-1){
                      grid[i][j]=0;
                    }
                    else{
                      grid[i][j]=1;
                    }
        }
        window.clear();
        
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0) continue;
                sTile.setTextureRect((grid[i][j] == 1) ? IntRect(0, 0, ts, ts) : IntRect(54, 0, ts, ts));
                sTile.setPosition(j * ts, i * ts);
                window.draw(sTile);
            }
        }

        // Draw Player 1 - only if active
        if (p1Active) {
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(x * ts, y * ts);
            window.draw(sTile);
        }

        // Draw Player 2 in two-player mode
        if (isTwoPlayer && p2Active)
        {
            sTile.setTextureRect(IntRect(72, 0, ts, ts));
            sTile.setPosition(x2 * ts, y2 * ts);
            window.draw(sTile);
        }

        // Draw enemies
        for (int i = 0; i < enemyCount; i++)
        {
            sEnemy.setPosition(a[i].x, a[i].y);
            window.draw(sEnemy);
        }

        // Draw UI elements
        window.draw(timeText);
        window.draw(scoreTextP1);
        window.draw(moveTextP1);
        
        if (isTwoPlayer) {
            window.draw(scoreTextP2);
            window.draw(moveTextP2);
            window.draw(statusP1);
            window.draw(statusP2);
        }
        
        window.display();
    }

    return 0;
}
