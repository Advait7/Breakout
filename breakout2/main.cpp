#include<iostream>
#include<sstream>
#include<vector>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>

using namespace std;
using namespace sf;

constexpr int windowWidth{800}, windowHeight{600};
constexpr float ballRadius{10.f}, ballVelocity{4.f};
constexpr float paddleWidth{60.f}, paddleHeight{20.f}, paddleVelocity{6.f};

constexpr float brickWidth{60.f}, brickHeight{20.f};
constexpr int brickCountX{11}, brickCountY{4};
/*
int score = 0;

ostringstream ssScore;
ssScore<<"Score: "<<score;

Text scorelbl;
scorelbl.setCharacterSize(20);
scorelbl.setPosition({windowWidth - 10, windowHeigth - 10});
scorelbl.setString(ssScore);
*/
class Ball {
    public:

        CircleShape shape;
        Vector2f velocity{-ballVelocity, -ballVelocity};

        float x() {
                return shape.getPosition().x;
        }

        float y() {
                return shape.getPosition().y;
        }

        float left() {
                return x() - shape.getRadius();
        }

        float right() {
                return x() + shape.getRadius();
        }

        float top() {
                return y() - shape.getRadius();
        }

        float bottom() {
                return y() + shape.getRadius();
        }

        Ball(float sx, float sy) {
            shape.setPosition(sx, sy);
            shape.setRadius(ballRadius);
            shape.setFillColor(Color::White);
            shape.setOrigin(ballRadius, ballRadius);
        }

        void update() {

            shape.move(velocity);

            if(left() < 0) {
                velocity.x = ballVelocity;
            }
            else if(right() > windowWidth) {
                velocity.x = -ballVelocity;
            }

            if(top() < 0) {
                velocity.y = ballVelocity;
            }
            else if(bottom() > windowHeight) {
                velocity.y = -ballVelocity;
            }

        }

};

class Paddle {
    public:

        RectangleShape shape;

        Vector2f velocity{0.f,0.f};

        float x() {
                return shape.getPosition().x;
        }

        float y() {
                return shape.getPosition().y;
        }

        float left() {
                return x() - shape.getSize().x / 2;
        }

        float right() {
                return x() + shape.getSize().x / 2;
        }

        float top() {
                return y() - shape.getSize().y / 2;
        }

        float bottom() {
                return y() + shape.getSize().y / 2;
        }

        Paddle(float sx, float sy) {
            shape.setPosition(sx, sy);
            shape.setSize({paddleWidth, paddleHeight});
            shape.setFillColor(Color::Red);
            shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
        }

        void update() {
            shape.move(velocity);

            if((Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) && left() > 0) {
                velocity.x = -paddleVelocity;
            }
            else if((Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) && right() < windowWidth) {
                velocity.x = paddleVelocity;
            }
            else {
                velocity.x = 0;
            }
        }

};

class Brick {
    public:

        RectangleShape shape;

        float x() {
                return shape.getPosition().x;
        }

        float y() {
                return shape.getPosition().y;
        }

        float left() {
                return x() - shape.getSize().x / 2;
        }

        float right() {
                return x() + shape.getSize().x / 2;
        }

        float top() {
                return y() - shape.getSize().y / 2;
        }

        float bottom() {
                return y() + shape.getSize().y / 2;
        }

        bool destroyed{false};

        Brick(float sx, float sy) {
            shape.setPosition(sx, sy);
            shape.setSize({brickWidth, brickHeight}); // ({}) this is equivalent to (Vector2{})
            shape.setFillColor(Color::Blue);
            shape.setOrigin(brickWidth / 2.f, brickHeight / 2.f);
        }



};

template<class T1, class T2> bool isIntersecting(T1 &s1, T2 &s2) {

    return s1.right() >= s2.left() && s1.left() <= s2.right() && s1.bottom() >= s2.top() && s1.top() <= s2.bottom();

}

void testCollisionBP(Paddle &sPaddle, Ball &sBall) {
    if(!isIntersecting(sPaddle, sBall)) {
        return;
    }

    sBall.velocity.y = -ballVelocity;

    if(sBall.x() < sPaddle.x()) {
        sBall.velocity.x = -ballVelocity;
    }
    else {
        sBall.velocity.x = ballVelocity;
    }

}

void testCollisionBB(Brick &sBrick, Ball &sBall) {

    if(!isIntersecting(sBrick, sBall)) {
        return;
    }

    sBrick.destroyed = true;

    float overlapLeft{sBall.right() - sBrick.left()};
    float overlapRight{sBrick.right() - sBall.left()};
    float overlapTop{sBall.bottom() - sBrick.top()};
    float overlapBottom{sBall.right() - sBrick.left()};

    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

    float minOverlapX{ballFromLeft ? overlapLeft : overlapRight};
    float minOverlapY{ballFromTop ? overlapTop : overlapBottom};

    if(abs(minOverlapX) < abs(minOverlapY)) {
        sBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
    }
    else {
        sBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
    }

}

int main() {
    Ball ball(windowWidth / 2, windowHeight - ballRadius - 20);

    Paddle paddle(windowWidth / 2, windowHeight - paddleHeight);

    vector<Brick> bricks;

    for(int iX{0};iX < brickCountX;iX++) {
        for(int iY{0};iY < brickCountY;iY++) {
            bricks.emplace_back((iX + 1) * (brickWidth + 3) + 22, (iY + 2) * (brickHeight + 3));
        }
    }

    RenderWindow window;
    window.create(VideoMode(windowWidth, windowHeight), "Breakout");
    window.setFramerateLimit(60);

    while(window.isOpen()) {
        window.clear(Color::Black);
        Event event;
        while(window.pollEvent(event)) {
            if(Keyboard::isKeyPressed(Keyboard::Escape)) {
                event.type = Event::Closed;
            }
            if(event.type == Event::Closed) {
                window.close();
            }
        }

        ball.update();

        paddle.update();

        testCollisionBP(paddle, ball);

        for(auto &brick : bricks) testCollisionBB(brick, ball);

        //remove_if sorts the vector by placing the elements that do not satisfy the condition at the front and returns an
        //iterator to the point at the start of the vectors which satisfy the condition. Hence the erase(start, end) works
        //on the second half of the vector erasing those range of elements. remove_if(start, end, predicate i.e. logical condition)
        bricks.erase(remove_if(begin(bricks), end(bricks), [](const Brick &sBrick){return sBrick.destroyed;}), end(bricks));

        //window.draw(scorelbl);
        window.draw(ball.shape);
        window.draw(paddle.shape);

        //auto automatically deduces that brick is of type Brick(class) without compromising the efficiency
        for(auto &brick : bricks) window.draw(brick.shape);

        window.display();
    }
    return 0;
}
