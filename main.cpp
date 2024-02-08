#define SFML_DEFINE_DISCRETE_GPU_PREFERENCE

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <thread>
#include <cmath>

//Distance formula
const float distance(const sf::Vector2f, const sf::Vector2f);

//Game reset after a score
void reset_game(sf::CircleShape& ball, sf::Vector2u& window_boundaries, float& game_speed,  sf::Int32& game_time,
                float& x_move , float& y_move);

void player2_movement(sf::RectangleShape& player2, sf::Vector2u& window_boundaries, float& game_speed,
                      bool& game_end, sf::CircleShape hit_points[]);

int main()
{
    std::vector<sf::VideoMode> video_Modes = sf::VideoMode::getFullscreenModes();
    sf::RenderWindow window(video_Modes.front(), "Pong", sf::Style::Fullscreen);

    window.setVerticalSyncEnabled(true); // Limit framerate for higher quality

    //Use this to get boundaries of the window space. Use for collisions!
    sf::Vector2u window_boundaries = window.getSize();

    //Pong ball
    sf::CircleShape ball;
    ball.setRadius(20);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(window_boundaries.x / 2 , window_boundaries.y / 2);
    
    //Players
    sf::RectangleShape player;
    player.setSize(sf::Vector2f{ 90 , 25 });
    player.setOrigin(sf::Vector2f{ 45 , 13 }); //Useful for keeping hit_points inside rectangle
    player.setFillColor(sf::Color::White);
    player.setRotation(90);
    player.setPosition(window_boundaries.x / 30, window_boundaries.y / 2);

    sf::RectangleShape player2;
    player2.setSize(sf::Vector2f{ 90 , 25 });
    player2.setOrigin(sf::Vector2f{ 45 , 13 });
    player2.setFillColor(sf::Color::White);
    player2.setRotation(90);
    player2.setPosition(window_boundaries.x - window_boundaries.x / 30, window_boundaries.y / 2);

    //Embedded circleshapes into players for hit detection
    sf::CircleShape hit_points[4];
    hit_points[0].setRadius(5);
    hit_points[0].setFillColor(sf::Color::White);
    hit_points[0].setPosition(player.getPosition().x, player.getPosition().y + 30);
    hit_points[1].setRadius(5);
    hit_points[1].setFillColor(sf::Color::White);
    hit_points[1].setPosition(player.getPosition().x, player.getPosition().y - 30);
    hit_points[2].setRadius(5);
    hit_points[2].setFillColor(sf::Color::White);
    hit_points[2].setPosition(window_boundaries.x - window_boundaries.x / 30 - 10, window_boundaries.y / 2);
    hit_points[3].setRadius(5);
    hit_points[3].setFillColor(sf::Color::White);
    hit_points[3].setPosition(window_boundaries.x - window_boundaries.x / 30 - 10, window_boundaries.y / 2 + 80);

    //window.getSize().y / 40 so we can create a rectangle length 20, then 20 pixels of space for the next one
    int rectangles = window_boundaries.y / 40;
    sf::RectangleShape* separator = new sf::RectangleShape[rectangles];


    //Set size, position, and orientation of the rectangles on screen
    for (int i = 0 , j = 0; i < rectangles; ++i) {
        separator[i].setSize(sf::Vector2f{ 20 , 10 });
        separator[i].setRotation(90);
        separator[i].setFillColor(sf::Color::White);
        separator[i].setPosition(window.getSize().x / 2, j);
        j += 40;
    }


    //Random library to randomly generate direction ball will start in
    
    std::random_device generator;
    std::uniform_int_distribution<int> dist(1, 9);
    
    //Game speed = player speed, x / y direction determines starting direction
    float game_speed = 5.0;
    bool x_direction =  dist(generator) % 2;
    bool y_direction = dist(generator) % 2;

    //x_move / y_move determine ball movement and speed.
    float x_move = game_speed , y_move = game_speed;
   
    sf::Clock clock;
    sf::Int32 time_passed = 5000; //in milliseconds

    //Thread for player2, ends execution once game_end
    bool game_end = false;
    std::thread t(player2_movement, std::ref(player2), std::ref(window_boundaries), std::ref(game_speed), 
                  std::ref(game_end), hit_points);

    short player1_score = 0 , player2_score = 0;

    //Game Loop
    while (window.isOpen())
    {
        sf::Event event;

        //Check for 'x' out of window
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        for (int i = 0; i < rectangles; i++) {
            window.draw(separator[i]);
        }
        
        for (int i = 0; i < 4; i++) {
            window.draw(hit_points[i]);
        }

        window.draw(ball);

        window.draw(player);

        window.draw(player2);

        //Players and ball collision
        if (distance(hit_points[0].getPosition(), ball.getPosition()) < 25 ||
            distance(hit_points[1].getPosition(), ball.getPosition()) < 25 ||
            distance(hit_points[2].getPosition(), ball.getPosition()) < 25 ||
            distance(hit_points[3].getPosition(), ball.getPosition()) < 25 
            ) {
            x_direction = !x_direction;
            x_move *= -1;
        }

        if (ball.getPosition().y + ball.getRadius() >= window.getSize().y ||
            ball.getPosition().y - ball.getRadius() <= window_boundaries.y - window_boundaries.y) {
            y_direction = !y_direction;
            y_move *= -1;
        }

      
        ball.move(x_move, y_move);

        //Player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player.getPosition().y > 0) {
            player.move(0, -game_speed);
            hit_points[0].move(0, -game_speed);
            hit_points[1].move(0, -game_speed);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && player.getPosition().y + player.getSize().y
                <= window_boundaries.y - 90) {
            player.move(0, game_speed);
            hit_points[1].move(0, game_speed);
            hit_points[0].move(0, game_speed);
        }

       if (game_speed < 15 && clock.getElapsedTime().asMilliseconds() > time_passed) {
            game_speed += 1;
            if (x_move < 0) {
                --x_move;
            }
            else {
                ++x_move;
            }
            if (y_move < 0) {
                --y_move;
            }
            else {
                ++y_move;
            }
            time_passed += 5000;
        }

       window.display();

       if (ball.getPosition().x + ball.getRadius() >= window_boundaries.x) {
           ++player1_score;
           std::cout << "Player1: " << player1_score << std::endl; /*Debug*/
           clock.restart();
           reset_game(ball, window_boundaries, game_speed, time_passed,
                      x_move , y_move);
           //Sleep(10);
       }
       else if (ball.getPosition().x - ball.getRadius() <= 0) {
           ++player2_score;
           std::cout << "Player2: " << player2_score << std::endl; /*Debug*/
           clock.restart();
           reset_game(ball, window_boundaries, game_speed, time_passed,
                      x_move, y_move);
           //Sleep(10);
       }

    }

    game_end = true;

    delete[] separator;

    return 0;
}

//Dist Formula: sqrt( (x2 - x1)^2 + (y2 - y1)^2 )
const float distance(sf::Vector2f p1, sf::Vector2f p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void reset_game(sf::CircleShape& ball, sf::Vector2u& window_boundaries, float& game_speed, sf::Int32& game_time,
                float& x_move, float& y_move) {
    ball.setPosition(window_boundaries.x / 2, window_boundaries.y / 2);
    game_speed = 5.0;
    x_move = game_speed;
    y_move = game_speed;
    game_time = 5000;
}

void player2_movement(sf::RectangleShape& player2, sf::Vector2u& window_boundaries, float& game_speed,
                      bool& game_end, sf::CircleShape hit_points[]) {
    while(game_end == false){
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && player2.getPosition().y > 0) {
            player2.move(0, -game_speed);
            hit_points[2].move(0, -game_speed);
            hit_points[3].move(0, -game_speed);

        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && player2.getPosition().y + player2.getSize().y
            <= window_boundaries.y - 90) {
            player2.move(0, game_speed);
            hit_points[2].move(0, game_speed);
            hit_points[3].move(0, game_speed);
         }
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }
}