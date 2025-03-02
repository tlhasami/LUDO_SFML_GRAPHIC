#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

//------> Screen Resolution
const int screen_resolution_x = 1000;
const int screen_resolution_y = 650;
//-----------------------> Class Declaration
class Object
{
public:
    Texture image;
    Sprite image_sprite;

    Object(string player, Vector2f place, Vector2f scale = Vector2f(0.4f, 0.4f))
    {
        if (!image.loadFromFile("images/" + player))
        {
            cout << "Error loading image" << player << endl;
        }
        image_sprite.setTexture(image);
        image_sprite.setScale(scale);
        image_sprite.setPosition(place);
    }

    void draw(RenderWindow &window)
    {
        window.draw(image_sprite);
    }
    void change_position(Vector2f new_position)
    {
        image_sprite.setPosition(new_position);
    }
    void change_scale(Vector2f new_scale)
    {
        image_sprite.setScale(new_scale);
    }
    void change_color()
    {
        image_sprite.setColor(Color(111, 103, 103));
    }
};
class PLAYER : public Object
{
public:
    bool lost;
    int current_position;
    int position;

    PLAYER(string player, Vector2f place, Vector2f scale = Vector2f(0.4f, 0.4f), bool is = false, int cur = 0, int pos = 0)
        : Object(player, place, scale), lost(is), current_position(cur), position(pos) {}
};
class DICE : public Object
{
public:
    int value;
    // Constructor for DICE, passing parameters to the Object constructor
    DICE(string player, Vector2f place, Vector2f scale = Vector2f(0.4f, 0.4f), int dice_value = 0)
        : Object(player, place, scale), value(dice_value) {}
    void number_got(int number)
    {
        this->value = number;
        if (!image.loadFromFile("images/dice_" + to_string(number) + ".png"))
        {
            cout << "Error loading image Dice " << number << endl;
        }
    }
};
//-----------------------> TAP TO PLAY TRIGGER
bool Game_start = false;
bool logo = true;
//-----------------------> DICE ROLLED
bool all_dice_rolled_of_current_player = false;
int dice_counter = 0; // how much dice are rolled
// ----------------------> NUMBER GOT
int number = 0;
//-----------------------> TURN
int turn = 1;
//-----------------------> PIECE SELECTED
int piece = 0;
int checker = 0;
bool showing_worker = false;
//-----------------------> WINNER POSITION
int result = 1;
//--------------------> KILL
Music kill;
    
//-----------------------> Function Declarations
void Background_setter(Texture &, Sprite &);
void initial_board_setter(Texture &, Sprite &);
void starter_logo(Texture &, Sprite &);
void player_draw(RenderWindow &, PLAYER[], PLAYER[], PLAYER[], PLAYER[]);
void tile_draw(RenderWindow &, Object[], DICE[]);
void worker_draw(RenderWindow &, DICE[]);
void red_position(PLAYER[], int);
void blue_position(PLAYER[], int);
void green_position(PLAYER[], int);
void yellow_position(PLAYER[], int);
void destroyer(Event &, PLAYER[], PLAYER[], PLAYER[], PLAYER[], Object[], DICE[]);
bool play_breaker(PLAYER[], DICE[]);
void after_breaking_effect(DICE[], Object &, DICE[], int);
bool can_be_destroyed(PLAYER[], int);
bool ended(PLAYER[], DICE[]);
bool winner(const PLAYER[]);
void overlapping_setting(PLAYER [], int );
void friendly_overlapping(Event &, PLAYER []);

int main()
{
    srand(time(0));
    // turn=rand()%4+1;
    //  Create a video mode object for the window
    VideoMode videoMode(screen_resolution_x, screen_resolution_y);
    // Create a render window
    RenderWindow window(videoMode, "LUDO");
    // Set the frame rate limit
    window.setFramerateLimit(60);

    //-------------------->   BackGround Permanent
    Texture background_image, board, starting_logo;
    Sprite background_image_sprite, board_sprite, starting_logo_sprite;

    PLAYER red_player[4] = {
        PLAYER("Red_player.png", Vector2f(259, 83)),
        PLAYER("Red_player.png", Vector2f(340, 83)),
        PLAYER("Red_player.png", Vector2f(259, 165)),
        PLAYER("Red_player.png", Vector2f(340, 165))};

    PLAYER blue_player[4] = {
        PLAYER("Blue_player.png", Vector2f(259, 450), Vector2f(0.4f, 0.4f)),
        PLAYER("Blue_player.png", Vector2f(340, 450), Vector2f(0.4f, 0.4f)),
        PLAYER("Blue_player.png", Vector2f(259, 531), Vector2f(0.4f, 0.4f)),
        PLAYER("Blue_player.png", Vector2f(340, 531), Vector2f(0.4f, 0.4f))};

    PLAYER yellow_player[4] = {
        PLAYER("Yellow_player.png", Vector2f(625, 450)),
        PLAYER("Yellow_player.png", Vector2f(706, 450)),
        PLAYER("Yellow_player.png", Vector2f(625, 531)),
        PLAYER("Yellow_player.png", Vector2f(706, 531))};

    PLAYER green_player[4] = {
        PLAYER("Green_player.png", Vector2f(625, 83), Vector2f(0.4f, 0.4f)),
        PLAYER("Green_player.png", Vector2f(706, 83), Vector2f(0.4f, 0.4f)),
        PLAYER("Green_player.png", Vector2f(625, 165), Vector2f(0.4f, 0.4f)),
        PLAYER("Green_player.png", Vector2f(706, 165), Vector2f(0.4f, 0.4f))};

    Object tap_to_play("play_button.png", Vector2f(-1000, -1000), Vector2f(0.2f, 0.2f)); // Load at placeholder position

    Object tile[4] = {
        Object("tileRed.png", Vector2f(277, 347), Vector2f(0.98f, 0.95f)),
        Object("tileBlue.png", Vector2f(521, 509), Vector2f(0.98f, 0.95f)),
        Object("tileYellow.png", Vector2f(684, 266), Vector2f(0.98f, 0.95f)),
        Object("tileGreen.png", Vector2f(440, 103), Vector2f(0.98f, 0.95f))};

    DICE dice[3] = {
        DICE("dice_0.png", Vector2f(140, 70), Vector2f(1.f, 1.f)),
        DICE("dice_0.png", Vector2f(140, 120), Vector2f(1.f, 1.f)),
        DICE("dice_0.png", Vector2f(140, 170), Vector2f(1.f, 1.f))};
    for (int i = 0; i < 3; i++)
    {
        dice[i].change_color();
    }

    DICE worker[3] = {
        DICE("dice_0.png", Vector2f(-100, -100), Vector2f(1.f, 1.f)),
        DICE("dice_0.png", Vector2f(-100, -100), Vector2f(1.f, 1.f)),
        DICE("dice_0.png", Vector2f(-100, -100), Vector2f(1.f, 1.f))};
    for (int i = 0; i < 3; i++)
    {
        worker[i].change_color();
    }

    Object dice_roller("dice.png", Vector2f(Vector2f(70, 120)), Vector2f(0.25f, 0.25f));

    Background_setter(background_image, background_image_sprite);
    initial_board_setter(board, board_sprite);
    starter_logo(starting_logo, starting_logo_sprite);
    Music background_music;
    if (!background_music.openFromFile("Music/background_music.ogg"))
    {
        cout << "Error loading background_music file!" << endl;
        return -1;
    }

    background_music.setLoop(true); // Enable looping
    background_music.play();
    background_music.setVolume(30);
    Music dice_back;
    if (!dice_back.openFromFile("Music/dice_press.ogg"))
    {
        cout << "Error loading dice_back file!" << endl;
        return -1;
    }
    dice_back.setVolume(45);
    if (!kill.openFromFile("Music/kill.ogg"))
    {
        cout << "Error loading kill file!" << endl;
        return -1;
    }
    kill.setVolume(200);
    //-------> Clock
    Clock clock;
    // Game loop
    while (window.isOpen())
    {
        // Process events
        Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == Event::Closed)
                window.close();

            if (event.mouseButton.button == Mouse::Left)
            {

                if (logo)
                {
                    // Right mouse button was clicked
                    if (tap_to_play.image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        cout << "Tap to play image clicked" << endl;
                        tap_to_play.image_sprite.setPosition(-1000, -1000);
                        Game_start = true;
                    }
                }
                if (Game_start)
                {

                    // place checking if their is a click
                    for (int i = 0; i < 4; i++)
                    {
                        if (turn == 1)
                        {
                            if (red_player[i].position)
                            {
                                cout << "red position" << red_player[i].position << endl;
                                turn = 2;
                                dice_roller.change_position(Vector2f(870, 112));
                                dice[0].change_position(Vector2f(820, 70));
                                dice[1].change_position(Vector2f(820, 120));
                                dice[2].change_position(Vector2f(820, 170));
                                break;
                            }
                            if (all_dice_rolled_of_current_player)
                            {
                                if (event.type == Event::MouseButtonPressed)
                                {
                                    if (play_breaker(red_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 2);
                                        cout << "i" << endl;
                                        break;
                                    }

                                    if (ended(red_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 2);
                                        cout << "o" << endl;
                                        break;
                                    }

                                    if (red_player[i].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                    {
                                        piece = i;
                                        cout << "red " << i + 1 << " image clicked" << endl;
                                        worker[0].number_got(dice[0].value);
                                        worker[1].number_got(dice[1].value);
                                        worker[2].number_got(dice[2].value);
                                        if (!showing_worker)
                                        {
                                            worker[0].change_position(Vector2(red_player[i].image_sprite.getPosition().x - 50, red_player[i].image_sprite.getPosition().y - 50));
                                            worker[1].change_position(Vector2(red_player[i].image_sprite.getPosition().x, red_player[i].image_sprite.getPosition().y - 50));
                                            worker[2].change_position(Vector2(red_player[i].image_sprite.getPosition().x + 50, red_player[i].image_sprite.getPosition().y - 50));
                                            showing_worker = true;
                                        }
                                        else
                                        {
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                        }
                                    }
                                    for (int j = 0; j < 3; j++)
                                    {

                                        if (worker[j].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                        {

                                            cout << "Dice " << j + 1 << "clicked" << endl;
                                            if (red_player[piece].current_position == 0)
                                            {
                                                if (worker[j].value == 6)
                                                {
                                                    red_player[piece].current_position++;
                                                    red_position(red_player, piece);

                                                    overlapping_setting(red_player, piece);
                                                    worker[j].number_got(0);
                                                    dice[j].number_got(0);
                                                }
                                                else
                                                {
                                                    cout << "Cannot be played" << endl;
                                                }
                                            }
                                            else
                                            {
                                                red_player[piece].current_position += worker[j].value;
                                                red_position(red_player, piece);
                                                overlapping_setting(red_player, piece);
                                                if (can_be_destroyed(red_player, piece))
                                                {
                                                    destroyer(event, red_player, blue_player, yellow_player, green_player, tile, dice);
                                                }
                                                cout<<"Checking friendly overlap red"<<endl;
                                                    friendly_overlapping(event,red_player);
                                                
                                                worker[j].number_got(0);
                                                dice[j].number_got(0);
                                            }
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                            break;
                                        }
                                    }
                                    for (int l = 0; l < 3; l++)
                                    {
                                        if (dice[l].value == 0)
                                        {
                                            checker++;
                                        }
                                    }
                                    if (checker == 3)
                                    {
                                        all_dice_rolled_of_current_player = false;
                                        dice_roller.change_position(Vector2f(870, 112));
                                        dice[0].change_position(Vector2f(820, 70));
                                        dice[1].change_position(Vector2f(820, 120));
                                        dice[2].change_position(Vector2f(820, 170));
                                        dice_counter = 0;
                                        cout << "Checking winner" << endl;
                                        if (winner(red_player))
                                        {
                                            cout << "won" << endl;
                                            for (int i = 0; i < 4; i++)
                                            {
                                                red_player[i].position = result;
                                            }
                                            result++;
                                        }
                                        turn = 2;
                                    }
                                    checker = 0;
                                }
                            }
                        }
                        if (turn == 2)
                        {
                            if (green_player[i].position)
                            {
                                cout << "green position" << green_player[i].position << endl;
                                turn = 3;
                                dice_roller.change_position(Vector2f(870, 485));
                                dice[0].change_position(Vector2f(820, 440));
                                dice[1].change_position(Vector2f(820, 490));
                                dice[2].change_position(Vector2f(820, 540));
                                break;
                            }
                            if (all_dice_rolled_of_current_player)
                            {
                                if (event.type == Event::MouseButtonPressed)
                                {
                                    if (play_breaker(green_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 3);
                                        break;
                                    }
                                    if (ended(green_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 3);
                                        break;
                                    }
                                    if (green_player[i].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                    {
                                        piece = i;
                                        cout << "green " << i + 1 << " image clicked" << endl;
                                        worker[0].number_got(dice[0].value);
                                        worker[1].number_got(dice[1].value);
                                        worker[2].number_got(dice[2].value);
                                        if (!showing_worker)
                                        {
                                            worker[0].change_position(Vector2(green_player[i].image_sprite.getPosition().x - 50, green_player[i].image_sprite.getPosition().y - 50));
                                            worker[1].change_position(Vector2(green_player[i].image_sprite.getPosition().x, green_player[i].image_sprite.getPosition().y - 50));
                                            worker[2].change_position(Vector2(green_player[i].image_sprite.getPosition().x + 50, green_player[i].image_sprite.getPosition().y - 50));
                                            showing_worker = true;
                                        }
                                        else
                                        {
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                        }
                                    }
                                    for (int j = 0; j < 3; j++)
                                    {
                                        if (worker[j].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                        {
                                            cout << "Dice " << j + 1 << "clicked" << endl;
                                            if (green_player[piece].current_position == 0)
                                            {
                                                if (worker[j].value == 6)
                                                {
                                                    green_player[piece].current_position++;
                                                    green_position(green_player, piece);
                                                    overlapping_setting(green_player, piece);
                                                    dice[j].number_got(0);
                                                    worker[j].number_got(0);
                                                }
                                                else
                                                {
                                                    cout << "Cannot be played" << endl;
                                                }
                                            }
                                            else
                                            {
                                                green_player[piece].current_position += worker[j].value;
                                                green_position(green_player, piece);
                                                overlapping_setting(green_player, piece);
                                                if (can_be_destroyed(green_player, piece))
                                                {
                                                    destroyer(event, red_player, blue_player, yellow_player, green_player, tile, dice);
                                                }
                                                cout<<"Checking friendly overlap green"<<endl;
                                                    friendly_overlapping(event,green_player);
                                                
                                                dice[j].number_got(0);
                                                worker[j].number_got(0);
                                            }
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                            break;
                                        }
                                    }
                                    for (int l = 0; l < 3; l++)
                                    {
                                        if (dice[l].value == 0)
                                        {
                                            checker++;
                                        }
                                    }
                                    if (checker == 3)
                                    {
                                        all_dice_rolled_of_current_player = false;
                                        dice_roller.change_position(Vector2f(870, 485));
                                        dice[0].change_position(Vector2f(820, 440));
                                        dice[1].change_position(Vector2f(820, 490));
                                        dice[2].change_position(Vector2f(820, 540));
                                        dice_counter = 0;
                                        cout << "Checking winner" << endl;
                                        if (winner(green_player))
                                        {
                                            cout << "Won" << endl;
                                            for (int i = 0; i < 4; i++)
                                            {
                                                green_player[i].position = result;
                                            }
                                            result++;
                                        }
                                        turn = 3;
                                    }
                                    checker = 0;
                                }
                            }
                        }
                        if (turn == 3)
                        {
                            if (yellow_player[i].position)
                            {
                                cout << "yellow position" << yellow_player[i].position << endl;
                                turn = 4;
                                dice_roller.change_position(Vector2f(70, 490));
                                dice[0].change_position(Vector2f(140, 440));
                                dice[1].change_position(Vector2f(140, 490));
                                dice[2].change_position(Vector2f(140, 540));
                                break;
                            }
                            if (all_dice_rolled_of_current_player)
                            {
                                if (event.type == Event::MouseButtonPressed)
                                {
                                    if (play_breaker(yellow_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 4);
                                        break;
                                    }
                                    if (ended(yellow_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 4);
                                        break;
                                    }
                                    if (yellow_player[i].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                    {
                                        piece = i;
                                        cout << "yellow " << i + 1 << " image clicked" << endl;
                                        worker[0].number_got(dice[0].value);
                                        worker[1].number_got(dice[1].value);
                                        worker[2].number_got(dice[2].value);
                                        if (!showing_worker)
                                        {
                                            worker[0].change_position(Vector2(yellow_player[i].image_sprite.getPosition().x - 50, yellow_player[i].image_sprite.getPosition().y - 50));
                                            worker[1].change_position(Vector2(yellow_player[i].image_sprite.getPosition().x, yellow_player[i].image_sprite.getPosition().y - 50));
                                            worker[2].change_position(Vector2(yellow_player[i].image_sprite.getPosition().x + 50, yellow_player[i].image_sprite.getPosition().y - 50));
                                            showing_worker = true;
                                        }
                                        else
                                        {
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                        }
                                    }
                                    for (int j = 0; j < 3; j++)
                                    {
                                        if (worker[j].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                        {
                                            cout << "Dice " << j + 1 << "clicked" << endl;
                                            if (yellow_player[piece].current_position == 0)
                                            {
                                                if (worker[j].value == 6)
                                                {
                                                    yellow_player[piece].current_position++;
                                                    yellow_position(yellow_player, piece);
                                                    overlapping_setting(yellow_player, piece);
                                                    worker[j].number_got(0);
                                                    dice[j].number_got(0);
                                                }
                                                else
                                                {
                                                    cout << "Cannot be played" << endl;
                                                }
                                            }
                                            else
                                            {
                                                yellow_player[piece].current_position += worker[j].value;
                                                yellow_position(yellow_player, piece);
                                                overlapping_setting(yellow_player, piece);
                                                if (can_be_destroyed(yellow_player, piece))
                                                {
                                                    destroyer(event, red_player, blue_player, yellow_player, green_player, tile, dice);
                                                }
                                                cout<<"Checking friendly overlap yellow"<<endl;
                                                    friendly_overlapping(event,yellow_player);
                                                
                                                worker[j].number_got(0);
                                                dice[j].number_got(0);
                                            }

                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;

                                            break;
                                        }
                                    }
                                    for (int l = 0; l < 3; l++)
                                    {
                                        if (dice[l].value == 0)
                                        {
                                            checker++;
                                        }
                                    }
                                    if (checker == 3)
                                    {
                                        all_dice_rolled_of_current_player = false;
                                        dice_roller.change_position(Vector2f(70, 490));
                                        dice[0].change_position(Vector2f(140, 440));
                                        dice[1].change_position(Vector2f(140, 490));
                                        dice[2].change_position(Vector2f(140, 540));
                                        dice_counter = 0;
                                        cout << "Checking winner" << endl;
                                        if (winner(yellow_player))
                                        {
                                            cout << "Won" << endl;
                                            for (int i = 0; i < 4; i++)
                                            {
                                                yellow_player[i].position = result;
                                            }
                                            result++;
                                        }
                                        turn = 4;
                                    }
                                    checker = 0;
                                }
                            }
                        }
                        if (turn == 4)
                        {
                            if (blue_player[i].position)
                            {
                                cout << "blue position" << blue_player[i].position << endl;
                                turn = 1;
                                dice_roller.change_position(Vector2f(70, 120));
                                dice[0].change_position(Vector2f(140, 70));
                                dice[1].change_position(Vector2f(140, 120));
                                dice[2].change_position(Vector2f(140, 170));
                                break;
                            }
                            if (all_dice_rolled_of_current_player)
                            {
                                if (event.type == Event::MouseButtonPressed)
                                {
                                    if (play_breaker(blue_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 1);
                                        break;
                                    }
                                    if (ended(blue_player, dice))
                                    {
                                        after_breaking_effect(worker, dice_roller, dice, 1);
                                        break;
                                    }

                                    if (blue_player[i].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                    {
                                        cout << "blue " << i + 1 << " image clicked" << endl;
                                        piece = i;
                                        worker[0].number_got(dice[0].value);
                                        worker[1].number_got(dice[1].value);
                                        worker[2].number_got(dice[2].value);
                                        if (!showing_worker)
                                        {
                                            worker[0].change_position(Vector2(blue_player[i].image_sprite.getPosition().x - 50, blue_player[i].image_sprite.getPosition().y - 50));
                                            worker[1].change_position(Vector2(blue_player[i].image_sprite.getPosition().x, blue_player[i].image_sprite.getPosition().y - 50));
                                            worker[2].change_position(Vector2(blue_player[i].image_sprite.getPosition().x + 50, blue_player[i].image_sprite.getPosition().y - 50));
                                            showing_worker = true;
                                        }
                                        else
                                        {
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                        }
                                    }
                                    for (int j = 0; j < 3; j++)
                                    {
                                        if (worker[j].image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                                        {
                                            cout << "Dice " << j + 1 << "clicked" << endl;
                                            if (blue_player[piece].current_position == 0)
                                            {
                                                if (worker[j].value == 6)
                                                {
                                                    blue_player[piece].current_position++;
                                                    blue_position(blue_player, piece);
                                                    overlapping_setting(blue_player, piece);
                                                    worker[j].number_got(0);
                                                    dice[j].number_got(0);
                                                }
                                                else
                                                {
                                                    cout << "Cannot be played" << endl;
                                                }
                                            }
                                            else
                                            {
                                                blue_player[piece].current_position += worker[j].value;
                                                blue_position(blue_player, piece);
                                                overlapping_setting(blue_player, piece);
                                                if (can_be_destroyed(blue_player, piece))
                                                {
                                                    destroyer(event, red_player, blue_player, yellow_player, green_player, tile, dice);
                                                }
                                                    cout<<"Checking friendly overlap blue"<<endl;
                                                    friendly_overlapping(event,blue_player);
                                                
                                                worker[j].number_got(0);
                                                dice[j].number_got(0);
                                            }
                                            worker[0].change_position(Vector2f(-100, -100));
                                            worker[1].change_position(Vector2f(-100, -100));
                                            worker[2].change_position(Vector2f(-100, -100));
                                            showing_worker = false;
                                            break;
                                        }
                                    }
                                    for (int l = 0; l < 3; l++)
                                    {

                                        if (dice[l].value == 0)
                                        {
                                            checker++;
                                        }
                                    }
                                    if (checker == 3)
                                    {
                                        all_dice_rolled_of_current_player = false;
                                        dice_roller.change_position(Vector2f(70, 120));
                                        dice[0].change_position(Vector2f(140, 70));
                                        dice[1].change_position(Vector2f(140, 120));
                                        dice[2].change_position(Vector2f(140, 170));
                                        dice_counter = 0;
                                        cout << "Checking winner" << endl;
                                        if (winner(blue_player))
                                        {
                                            cout << "won" << endl;
                                            for (int i = 0; i < 4; i++)
                                            {
                                                blue_player[i].position = result;
                                            }
                                            result++;
                                        }
                                        turn = 1;
                                    }
                                    checker = 0;
                                }
                            }
                        }
                    }
                    if (event.type == Event::MouseButtonPressed)
                    {
                        if (dice_roller.image_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && !all_dice_rolled_of_current_player)
                        {
                            dice_back.play();
                            cout << "Dice" << endl;
                            number = rand() % 6 + 1;
                            dice_counter++;
                            if (number != 6 && dice_counter < 3)
                            {
                                dice[dice_counter - 1].number_got(number);
                                all_dice_rolled_of_current_player = true;
                            }
                            else if (dice_counter == 3)
                            {
                                if (number == 6)
                                {
                                    dice[0].number_got(0);
                                    dice[1].number_got(0);
                                    dice[2].number_got(0);
                                }
                                else
                                {
                                    dice[2].number_got(number);
                                }

                                all_dice_rolled_of_current_player = true;
                            }
                            else
                            {
                                dice[dice_counter - 1].number_got(number);
                            }
                        }
                    }
                }
            }
        }

        // Clear the window with black color
        window.clear(Color::Black);
        if (clock.getElapsedTime().asSeconds() < 0.f)
        {
            window.draw(starting_logo_sprite);
        }
        else if (Game_start)
        {
            logo = false;
            // Draw your graphics here
            window.draw(background_image_sprite);
            window.draw(board_sprite);
            tile_draw(window, tile, dice);
            if (turn == 1)
            {
                player_draw(window, red_player, blue_player, green_player, yellow_player);
            }
            else if (turn == 2)
            {
                player_draw(window, green_player, blue_player, red_player, yellow_player);
            }
            else if (turn == 3)
            {
                player_draw(window, yellow_player, blue_player, green_player, red_player);
            }
            else
            {
                player_draw(window, blue_player, red_player, green_player, yellow_player);
            }
            worker_draw(window, worker);
            window.draw(dice_roller.image_sprite);
        }
        else
        {
            tap_to_play.change_position(Vector2f(250, 280));
            window.draw(background_image_sprite);
            tap_to_play.draw(window);
        }
        // Display the contents of the window
        window.display();
    }

    return 0;
}
void Background_setter(Texture &background_image, Sprite &background_image_sprite)
{
    if (!background_image.loadFromFile("images/Background.png"))
    {
        cout << "Error opening Background image " << endl;
        exit(0);
    }
    background_image_sprite.setTexture(background_image);
    background_image_sprite.setScale(0.9f, 0.9f);
}
void initial_board_setter(Texture &board, Sprite &board_sprite)
{
    if (!board.loadFromFile("images/Board.jpg"))
    {
        cout << "Error opening Board image " << endl;
        exit(0);
    }
    board_sprite.setTexture(board);
    board_sprite.setScale(1.f, 1.f);
    board_sprite.setPosition(screen_resolution_x / 2 - board.getSize().x / 2, screen_resolution_y / 2 - board.getSize().y / 2);
}
void starter_logo(Texture &starter_logo, Sprite &starter_logo_sprite)
{
    if (!starter_logo.loadFromFile("images/Title.jpeg"))
    {
        cout << "Error Opening personal logo" << endl;
        exit(0);
    }
    starter_logo_sprite.setTexture(starter_logo);
    starter_logo_sprite.setPosition(screen_resolution_x / 2 - starter_logo.getSize().x / 2, screen_resolution_y / 2 - starter_logo.getSize().y / 2);
    starter_logo_sprite.scale(1.f, 1.f);
}
void tile_draw(RenderWindow &window, Object tile[], DICE dice[])
{
    for (int i = 0; i < 4; i++)
    {
        tile[i].draw(window);
        if (i != 3)
        {
            dice[i].draw(window);
        }
    }
}
void player_draw(RenderWindow &window, PLAYER red_player[], PLAYER blue_player[], PLAYER green_player[], PLAYER yellow_player[])
{
    for (int i = 0; i < 4; i++)
    {
        red_player[i].draw(window);
        blue_player[i].draw(window);
        green_player[i].draw(window);
        yellow_player[i].draw(window);
    }
}
void destroyer(Event &event, PLAYER red_player[], PLAYER blue_player[], PLAYER yellow_player[], PLAYER green_player[], Object tile[], DICE dice[])
{
    if (turn == 1)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (red_player[i].image_sprite.getGlobalBounds().intersects(green_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << " green" << endl;
                    if (j == 0)
                    {
                        cout << "1" << endl;
                        green_player[j].change_position(Vector2f(625, 83));
                        green_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        cout << "2" << endl;
                        green_player[j].change_position(Vector2f(706, 83));
                        green_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        cout << "3" << endl;
                        green_player[j].change_position(Vector2f(625, 165));
                        green_player[j].current_position = 0;
                    }
                    else
                    {
                        cout << "4" << endl;
                        green_player[j].change_position(Vector2f(706, 165));
                        green_player[j].current_position = 0;
                    }
                }
                if (red_player[i].image_sprite.getGlobalBounds().intersects(yellow_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        cout << "1" << endl;
                        yellow_player[j].change_position(Vector2f(625, 450));
                        yellow_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        cout << "2" << endl;
                        yellow_player[j].change_position(Vector2f(706, 450));
                        yellow_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        cout << "3" << endl;
                        yellow_player[j].change_position(Vector2f(625, 531));
                        yellow_player[j].current_position = 0;
                    }
                    else
                    {
                        cout << "4" << endl;
                        yellow_player[j].change_position(Vector2f(706, 531));
                        yellow_player[j].current_position = 0;
                    }
                }
                if (red_player[i].image_sprite.getGlobalBounds().intersects(blue_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        cout << "1" << endl;
                        blue_player[j].change_position(Vector2f(259, 450));
                        blue_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        cout << "2" << endl;
                        blue_player[j].change_position(Vector2f(340, 450));
                        blue_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        cout << "3" << endl;
                        blue_player[j].change_position(Vector2f(259, 531));
                        blue_player[j].current_position = 0;
                    }
                    else
                    {
                        cout << "4" << endl;
                        blue_player[j].change_position(Vector2f(340, 531));
                        blue_player[j].current_position = 0;
                    }
                }
            }
        }
    }
    else if (turn == 2)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (green_player[i].image_sprite.getGlobalBounds().intersects(yellow_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        yellow_player[j].change_position(Vector2f(625, 450));
                        yellow_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        yellow_player[j].change_position(Vector2f(706, 450));
                        yellow_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        yellow_player[j].change_position(Vector2f(625, 531));
                        yellow_player[j].current_position = 0;
                    }
                    else
                    {
                        yellow_player[j].change_position(Vector2f(706, 531));
                        yellow_player[j].current_position = 0;
                    }
                }
                if (green_player[i].image_sprite.getGlobalBounds().intersects(blue_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;

                    if (j == 0)
                    {
                        blue_player[j].change_position(Vector2f(259, 450));
                        blue_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        blue_player[j].change_position(Vector2f(340, 450));
                        blue_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        blue_player[j].change_position(Vector2f(259, 531));
                        blue_player[j].current_position = 0;
                    }
                    else
                    {
                        blue_player[j].change_position(Vector2f(340, 531));
                        blue_player[j].current_position = 0;
                    }
                }
                if (green_player[i].image_sprite.getGlobalBounds().intersects(red_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        red_player[j].change_position(Vector2f(259, 83));
                        red_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        red_player[j].change_position(Vector2f(340, 83));
                        red_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        red_player[j].change_position(Vector2f(259, 165));
                        red_player[j].current_position = 0;
                    }
                    else
                    {
                        red_player[j].change_position(Vector2f(340, 165));
                        red_player[j].current_position = 0;
                    }
                }
            }
        }
    }
    else if (turn == 3)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (yellow_player[i].image_sprite.getGlobalBounds().intersects(red_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        red_player[j].change_position(Vector2f(259, 83));
                        red_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        red_player[j].change_position(Vector2f(340, 83));
                        red_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        red_player[j].change_position(Vector2f(259, 165));
                        red_player[j].current_position = 0;
                    }
                    else
                    {
                        red_player[j].change_position(Vector2f(340, 165));
                        red_player[j].current_position = 0;
                    }
                }
                if (yellow_player[i].image_sprite.getGlobalBounds().intersects(green_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        green_player[j].change_position(Vector2f(625, 83));
                        green_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        green_player[j].change_position(Vector2f(706, 83));
                        green_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        green_player[j].change_position(Vector2f(625, 165));
                        green_player[j].current_position = 0;
                    }
                    else
                    {
                        green_player[j].change_position(Vector2f(706, 165));
                        green_player[j].current_position = 0;
                    }
                }
                if (yellow_player[i].image_sprite.getGlobalBounds().intersects(blue_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        blue_player[j].change_position(Vector2f(259, 450));
                        blue_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        blue_player[j].change_position(Vector2f(340, 450));
                        blue_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        blue_player[j].change_position(Vector2f(259, 531));
                        blue_player[j].current_position = 0;
                    }
                    else
                    {
                        blue_player[j].change_position(Vector2f(340, 531));
                        blue_player[j].current_position = 0;
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (blue_player[i].image_sprite.getGlobalBounds().intersects(red_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        red_player[j].change_position(Vector2f(259, 83));
                        red_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        red_player[j].change_position(Vector2f(340, 83));
                        red_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        red_player[j].change_position(Vector2f(259, 165));
                        red_player[j].current_position = 0;
                    }
                    else
                    {
                        red_player[j].change_position(Vector2f(340, 165));
                        red_player[j].current_position = 0;
                    }
                }
                if (blue_player[i].image_sprite.getGlobalBounds().intersects(yellow_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        yellow_player[j].change_position(Vector2f(625, 450));
                        yellow_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        yellow_player[j].change_position(Vector2f(706, 450));
                        yellow_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        yellow_player[j].change_position(Vector2f(625, 531));
                        yellow_player[j].current_position = 0;
                    }
                    else
                    {
                        yellow_player[j].change_position(Vector2f(706, 531));
                        yellow_player[j].current_position = 0;
                    }
                }
                if (blue_player[i].image_sprite.getGlobalBounds().intersects(green_player[j].image_sprite.getGlobalBounds()))
                {
                    kill.play();
                    cout << "their was overlap by player " << turn << " on " << endl;
                    if (j == 0)
                    {
                        green_player[j].change_position(Vector2f(625, 83));
                        green_player[j].current_position = 0;
                    }
                    else if (j == 1)
                    {
                        green_player[j].change_position(Vector2f(706, 83));
                        green_player[j].current_position = 0;
                    }
                    else if (j == 2)
                    {
                        green_player[j].change_position(Vector2f(625, 165));
                        green_player[j].current_position = 0;
                    }
                    else
                    {
                        green_player[j].change_position(Vector2f(706, 165));
                        green_player[j].current_position = 0;
                    }
                }
            }
        }
    }
}
void dice_position(Object &dice_roller, DICE dice[])
{
    if (turn == 1)
    {
        dice_roller.change_position(Vector2f(870, 112));
        dice[0].change_position(Vector2f(820, 70));
        dice[1].change_position(Vector2f(820, 120));
        dice[2].change_position(Vector2f(820, 170));
    }
    else if (turn == 2)
    {
        dice_roller.change_position(Vector2f(870, 485));
        dice[0].change_position(Vector2f(820, 440));
        dice[1].change_position(Vector2f(820, 490));
        dice[2].change_position(Vector2f(820, 540));
    }
    else if (turn == 3)
    {
        dice_roller.change_position(Vector2f(70, 490));
        dice[0].change_position(Vector2f(140, 440));
        dice[1].change_position(Vector2f(140, 490));
        dice[2].change_position(Vector2f(140, 540));
    }
    else
    {
        dice_roller.change_position(Vector2f(70, 120));
        dice[0].change_position(Vector2f(140, 70));
        dice[1].change_position(Vector2f(140, 120));
        dice[2].change_position(Vector2f(140, 170));
    }

    // -----> RED
    //     Vector2f(70,120)
    //     Object("dice_0.png", Vector2f(140, 70),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(140, 120),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(140, 170),Vector2f(1.f,1.f))};
    // -------> GREEN
    //     Vector2f(870,112)
    //     Object("dice_0.png", Vector2f(820, 70),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(820, 120),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(820, 170),Vector2f(1.f,1.f))};
    // ------> YELLOW
    //     Vector2f(870,485)
    //     Object("dice_0.png", Vector2f(820, 440),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(820, 490),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(820, 540),Vector2f(1.f,1.f))};
    // ------> BLUE
    //     Vector2f(70,490)
    //     Object("dice_0.png", Vector2f(140, 440),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(140, 490),Vector2f(1.f,1.f)),
    //     Object("dice_0.png", Vector2f(140, 540),Vector2f(1.f,1.f))};

    
}
void after_breaking_effect(DICE worker[], Object &dice_roller, DICE dice[], int next_player)
{
    worker[0].change_position(Vector2f(-100, -100));
    worker[1].change_position(Vector2f(-100, -100));
    worker[2].change_position(Vector2f(-100, -100));
    showing_worker = false;
    for (int di = 0; di < 3; di++)
    {
        dice[di].number_got(0);
        worker[di].number_got(0);
    }
    all_dice_rolled_of_current_player = false;
    dice_position(dice_roller, dice);
    dice_counter = 0;
    turn = next_player;
}
void starting_positions(PLAYER red_player[], PLAYER green_player[], PLAYER yellow_player[], PLAYER blue_player[])
{
    red_player[0].change_position(Vector2f(259, 83));
    red_player[1].change_position(Vector2f(340, 83));
    red_player[1].change_position(Vector2f(340, 83));
}
void worker_draw(RenderWindow &window, DICE worker[])
{
    for (int i = 0; i < 3; i++)
    {
        worker[i].draw(window);
    }
}
bool play_breaker(PLAYER player[], DICE dice[])
{
    if ((player[0].current_position == 0 || player[0].current_position == 57) && dice[0].value != 6 && dice[1].value != 6 && dice[2].value != 6 /*|| player[0].current_position== -1 */)
    {
        if ((player[1].current_position == 0 || player[1].current_position == 57) && dice[0].value != 6 && dice[1].value != 6 && dice[2].value != 6)
        {
            if ((player[2].current_position == 0 || player[2].current_position == 57) && dice[0].value != 6 && dice[1].value != 6 && dice[2].value != 6)
            {
                if ((player[3].current_position == 0 || player[3].current_position == 57) && dice[0].value != 6 && dice[1].value != 6 && dice[2].value != 6)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}
bool ended(PLAYER player[], DICE dice[])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {

            if (player[i].current_position + dice[j].value <= 57)
            {
                return false;
            }
        }
    }
    return true;
}
bool winner(const PLAYER player[])
{
    int win = 0;
    for (int i = 0; i < 4; i++)
    {
        if (player[i].current_position == 57)
        {
            win++;
        }
    }
    if (win == 4)
    {
        return true;
    }
    return false;
}
bool can_be_destroyed(PLAYER player[], int jack)
{
    if (player[jack].current_position == 1 ||
        player[jack].current_position == 9 ||
        player[jack].current_position == 14 ||
        player[jack].current_position == 22 ||
        player[jack].current_position == 27 ||
        player[jack].current_position == 35 ||
        player[jack].current_position == 40 ||
        player[jack].current_position == 48 ||
        player[jack].current_position == 52 ||
        player[jack].current_position == 53 ||
        player[jack].current_position == 54 ||
        player[jack].current_position == 55 ||
        player[jack].current_position == 56)
    {
        return 0;
    }
    return 1;
}
void overlapping_setting(PLAYER player[], int p)
{
    if (player[p].current_position == 1 ||
        player[p].current_position == 9 ||
        player[p].current_position == 14 ||
        player[p].current_position == 22 ||
        player[p].current_position == 27 ||
        player[p].current_position == 35 ||
        player[p].current_position == 40 ||
        player[p].current_position == 48 ||
        player[p].current_position == 52 ||
        player[p].current_position == 53 ||
        player[p].current_position == 54 ||
        player[p].current_position == 55 ||
        player[p].current_position == 56)
    {
        player[p].change_scale(Vector2f(0.2f, 0.2f));
        if (p == 0)
        {
            float x = (player[p].image_sprite.getPosition().x);
            float y = (player[p].image_sprite.getPosition().y);
            player[p].change_position(Vector2f(x, y));
        }
        else if (p == 1)
        {
            float x = (player[p].image_sprite.getPosition().x) + 21.3f;
            float y = (player[p].image_sprite.getPosition().y);
            player[p].change_position(Vector2f(x, y));
        }
        else if (p == 2)
        {
            float x = (player[p].image_sprite.getPosition().x);
            float y = (player[p].image_sprite.getPosition().y) + 21.3f;
            player[p].change_position(Vector2f(x, y));
        }
        else
        {
            float x = (player[p].image_sprite.getPosition().x) + 21.3f;
            float y = (player[p].image_sprite.getPosition().y) + 21.3f;
            player[p].change_position(Vector2f(x, y));
        }
    }
    else
    {
        player[p].change_scale(Vector2f(0.4f, 0.4f));
    }
}
void yellow_position(PLAYER yellow_player[], int i)
{
    Vector2f move_check;
    switch (yellow_player[i].current_position)
    {
    case 1:
        move_check = sf::Vector2f(725, 347);
        break; // Starting position
    case 2:
        move_check = sf::Vector2f(686, 347);
        break; // Move 1
    case 3:
        move_check = sf::Vector2f(645, 347);
        break; // Move 2
    case 4:
        move_check = sf::Vector2f(605, 347);
        break; // Move 3
    case 5:
        move_check = sf::Vector2f(564, 347);
        break; // Move 4
    case 6:
        move_check = sf::Vector2f(523, 388);
        break; // Move 4
    case 7:
        move_check = sf::Vector2f(523, 429);
        break; // +41 in y-axis
    case 8:
        move_check = sf::Vector2f(523, 470);
        break; // +41 in y-axis
    case 9:
        move_check = sf::Vector2f(523, 511);
        break; // +41 in y-axis
    case 10:
        move_check = sf::Vector2f(523, 552);
        break; // +41 in y-axis
    case 11:
        move_check = sf::Vector2f(523, 593);
        break; // +41 in y-axis
    case 12:
        move_check = sf::Vector2f(482, 593);
        break; // -41 in x-axis
    case 13:
        move_check = sf::Vector2f(441, 593);
        break;
    case 14:
        move_check = sf::Vector2f(441, 552);
        break;
    case 15:
        move_check = sf::Vector2f(441, 511);
        break;
    case 16:
        move_check = sf::Vector2f(441, 470);
        break;
    case 17:
        move_check = sf::Vector2f(441, 429);
        break;
    case 18:
        move_check = sf::Vector2f(441, 388);
        break;
    case 19:
        move_check = sf::Vector2f(400, 347);
        break; // Turn up
    case 20:
        move_check = sf::Vector2f(359, 347);
        break; // 400 - 41 = 359
    case 21:
        move_check = sf::Vector2f(318, 347);
        break; // 359 - 41 = 318
    case 22:
        move_check = sf::Vector2f(277, 347);
        break; // 318 - 41 = 277
    case 23:
        move_check = sf::Vector2f(236, 347);
        break; // 277 - 41 = 236
    case 24:
        move_check = sf::Vector2f(195, 347);
        break; // 236 - 41 = 195
    case 25:
        move_check = sf::Vector2f(195, 307);
        break; // Turn right
    case 26:
        move_check = sf::Vector2f(195, 266);
        break; // Move 26 (-41 in y-axis from 307)
    case 27:
        move_check = sf::Vector2f(236, 266);
        break; // Move 27 (+41 in x-axis from 195)
    case 28:
        move_check = sf::Vector2f(277, 266);
        break; // Move 28 (+41 in x-axis from 236)
    case 29:
        move_check = sf::Vector2f(318, 266);
        break; // Move 29 (+41 in x-axis from 277)
    case 30:
        move_check = sf::Vector2f(359, 266);
        break; // Move 30 (+41 in x-axis from 318)
    case 31:
        move_check = sf::Vector2f(400, 266);
        break; // Move 31 (+41 in x-axis from 359)
    case 32:
        move_check = sf::Vector2f(441, 225);
        break; // 400+41 = 441, 266-41 = 225
    case 33:
        move_check = sf::Vector2f(441, 184);
        break; // Move 33 (-41 in y-axis from 225)
    case 34:
        move_check = sf::Vector2f(441, 143);
        break; // Move 34 (-41 in y-axis from 184)
    case 35:
        move_check = sf::Vector2f(441, 102);
        break; // Move 35 (-41 in y-axis from 143)
    case 36:
        move_check = sf::Vector2f(441, 61);
        break; // Move 36 (-41 in y-axis from 102)
    case 37:
        move_check = sf::Vector2f(441, 22);
        break; // Move 37 (-41 in y-axis from 61)
    case 38:
        move_check = sf::Vector2f(482, 22);
        break; // Move 38 (+41 in x-axis from 441)
    case 39:
        move_check = sf::Vector2f(523, 22);
        break; // Move 39 (+41 in x-axis from 482)
    case 40:
        move_check = sf::Vector2f(523, 63);
        break; // Move 40 (+41 in y-axis from 19)
    case 41:
        move_check = sf::Vector2f(523, 103);
        break; // Move 41 (+41 in y-axis from 60)
    case 42:
        move_check = sf::Vector2f(523, 144);
        break; // Move 42 (+41 in y-axis from 101)
    case 43:
        move_check = sf::Vector2f(523, 185);
        break; // Move 43 (+41 in y-axis from 142)
    case 44:
        move_check = sf::Vector2f(523, 226);
        break; // Move 44 (+41 in y-axis from 183)
    case 45:
        move_check = sf::Vector2f(564, 266);
        break;
    case 46:
        move_check = sf::Vector2f(605, 266);
        break; // Move 46 (+41 in x-axis from 564)
    case 47:
        move_check = sf::Vector2f(646, 266);
        break; // Move 47 (+41 in x-axis from 605)
    case 48:
        move_check = sf::Vector2f(687, 266);
        break; // Move 48 (+41 in x-axis from 646)
    case 49:
        move_check = sf::Vector2f(728, 266);
        break; // Move 49 (+41 in x-axis from 687)
    case 50:
        move_check = sf::Vector2f(767, 266);
        break; // Move 50 (+41 in x-axis from 728)
    case 51:
        move_check = sf::Vector2f(767, 307);
        break; // Move 51 (Y +41 from 266)
    case 52:
        move_check = sf::Vector2f(726, 307);
        break; // Move 52 (-41 in x-axis from 767)
    case 53:
        move_check = sf::Vector2f(685, 307);
        break; // Move 53 (-41 in x-axis from 726)
    case 54:
        move_check = sf::Vector2f(644, 307);
        break; // Move 54 (-41 in x-axis from 685)
    case 55:
        move_check = sf::Vector2f(603, 307);
        break; // Move 55 (-41 in x-axis from 644)
    case 56:
        move_check = sf::Vector2f(562, 307);
        break; // Move 56 (-41 in x-axis from 603)
    }
    cout << "yellow player " << i + 1 << " Moved" << endl;
    yellow_player[i].change_position(move_check);
}
void blue_position(PLAYER blue_player[], int i)
{
    Vector2f move_check;
    switch (blue_player[i].current_position)
    {
    case 1:
        move_check = sf::Vector2f(441, 552);
        break;
    case 2:
        move_check = sf::Vector2f(441, 511);
        break;
    case 3:
        move_check = sf::Vector2f(441, 470);
        break;
    case 4:
        move_check = sf::Vector2f(441, 429);
        break;
    case 5:
        move_check = sf::Vector2f(441, 388);
        break;
    case 6:
        move_check = sf::Vector2f(400, 347);
        break; // Turn up
    case 7:
        move_check = sf::Vector2f(359, 347);
        break; // 400 - 41 = 359
    case 8:
        move_check = sf::Vector2f(318, 347);
        break; // 359 - 41 = 318
    case 9:
        move_check = sf::Vector2f(277, 347);
        break; // 318 - 41 = 277
    case 10:
        move_check = sf::Vector2f(236, 347);
        break; // 277 - 41 = 236
    case 11:
        move_check = sf::Vector2f(195, 347);
        break; // 236 - 41 = 195
    case 12:
        move_check = sf::Vector2f(195, 307);
        break; // Turn right
    case 13:
        move_check = sf::Vector2f(195, 266);
        break; // Move 26 (-41 in y-axis from 307)
    case 14:
        move_check = sf::Vector2f(237, 266);
        break; // Move 27 (+41 in x-axis from 195)
    case 15:
        move_check = sf::Vector2f(277, 266);
        break; // Move 28 (+41 in x-axis from 236)
    case 16:
        move_check = sf::Vector2f(318, 266);
        break; // Move 29 (+41 in x-axis from 277)
    case 17:
        move_check = sf::Vector2f(359, 266);
        break; // Move 30 (+41 in x-axis from 318)
    case 18:
        move_check = sf::Vector2f(400, 266);
        break; // Move 31 (+41 in x-axis from 359)
    case 19:
        move_check = sf::Vector2f(441, 225);
        break; // 400+41 = 441, 266-41 = 225
    case 20:
        move_check = sf::Vector2f(441, 184);
        break; // Move 33 (-41 in y-axis from 225)
    case 21:
        move_check = sf::Vector2f(441, 143);
        break; // Move 34 (-41 in y-axis from 184)
    case 22:
        move_check = sf::Vector2f(441, 102);
        break; // Move 35 (-41 in y-axis from 143)
    case 23:
        move_check = sf::Vector2f(441, 61);
        break; // Move 36 (-41 in y-axis from 102)
    case 24:
        move_check = sf::Vector2f(441, 22);
        break; // Move 37 (-41 in y-axis from 61)
    case 25:
        move_check = sf::Vector2f(482, 22);
        break; // Move 38 (+41 in x-axis from 441)
    case 26:
        move_check = sf::Vector2f(523, 22);
        break; // Move 39 (+41 in x-axis from 482)
    case 27:
        move_check = sf::Vector2f(523, 63);
        break; // Move 40 (+41 in y-axis from 19)
    case 28:
        move_check = sf::Vector2f(523, 103);
        break; // Move 41 (+41 in y-axis from 60)
    case 29:
        move_check = sf::Vector2f(523, 144);
        break; // Move 42 (+41 in y-axis from 101)
    case 30:
        move_check = sf::Vector2f(523, 185);
        break; // Move 43 (+41 in y-axis from 142)
    case 31:
        move_check = sf::Vector2f(523, 226);
        break; // Move 44 (+41 in y-axis from 183)
    case 32:
        move_check = sf::Vector2f(564, 266);
        break;
    case 33:
        move_check = sf::Vector2f(605, 266);
        break; // Move 46 (+41 in x-axis from 564)
    case 34:
        move_check = sf::Vector2f(646, 266);
        break; // Move 47 (+41 in x-axis from 605)
    case 35:
        move_check = sf::Vector2f(687, 266);
        break; // Move 48 (+41 in x-axis from 646)
    case 36:
        move_check = sf::Vector2f(728, 266);
        break; // Move 49 (+41 in x-axis from 687)
    case 37:
        move_check = sf::Vector2f(767, 266);
        break; // Move 50 (+41 in x-axis from 728)
    case 38:
        move_check = sf::Vector2f(767, 307);
        break; // Move 51 (Y +41 from 266)
    case 39:
        move_check = sf::Vector2f(767, 348);
        break;
    case 40:
        move_check = sf::Vector2f(726, 348);
        break; // 767 - 41 = 726
    case 41:
        move_check = sf::Vector2f(685, 348);
        break; // 726 - 41 = 685
    case 42:
        move_check = sf::Vector2f(644, 348);
        break; // 685 - 41 = 644
    case 43:
        move_check = sf::Vector2f(603, 348);
        break; // 644 - 41 = 603
    case 44:
        move_check = sf::Vector2f(562, 348);
        break; // 603 - 41 = 562
    case 45:
        move_check = sf::Vector2f(522, 389);
        break;
    case 46:
        move_check = sf::Vector2f(522, 430);
        break; // 389 + 41 = 430
    case 47:
        move_check = sf::Vector2f(522, 471);
        break; // 430 + 41 = 471
    case 48:
        move_check = sf::Vector2f(522, 512);
        break; // 471 + 41 = 512
    case 49:
        move_check = sf::Vector2f(522, 553);
        break; // 512 + 41 = 553
    case 50:
        move_check = sf::Vector2f(522, 592);
        break; // 553 + 41 = 594
    case 51:
        move_check = sf::Vector2f(482, 592);
        break;
    case 52:
        move_check = sf::Vector2f(482, 551);
        break; // 592 - 41 = 551
    case 53:
        move_check = sf::Vector2f(482, 510);
        break; // 551 - 41 = 510
    case 54:
        move_check = sf::Vector2f(482, 469);
        break; // 510 - 41 = 469
    case 55:
        move_check = sf::Vector2f(482, 428);
        break; // 469 - 41 = 428
    case 56:
        move_check = sf::Vector2f(482, 387);
        break; // 428 - 41 = 387
    }
    cout << "blue player " << i + 1 << "Moved" << endl;
    blue_player[i].change_position(move_check);
}
void red_position(PLAYER red_player[], int i)
{
    Vector2f move_check;
    switch (red_player[i].current_position)
    {
    case 1:
        move_check = sf::Vector2f(237, 266);
        break; // Move 27 (+41 in x-axis from 195)
    case 2:
        move_check = sf::Vector2f(278, 266);
        break; // Move 28 (+41 in x-axis from 236)
    case 3:
        move_check = sf::Vector2f(319, 266);
        break; // Move 29 (+41 in x-axis from 277)
    case 4:
        move_check = sf::Vector2f(360, 266);
        break; // Move 30 (+41 in x-axis from 318)
    case 5:
        move_check = sf::Vector2f(401, 266);
        break; // Move 31 (+41 in x-axis from 359)
    case 6:
        move_check = sf::Vector2f(441, 225);
        break; // 400+41 = 441, 266-41 = 225
    case 7:
        move_check = sf::Vector2f(441, 184);
        break; // Move 33 (-41 in y-axis from 225)
    case 8:
        move_check = sf::Vector2f(441, 143);
        break; // Move 34 (-41 in y-axis from 184)
    case 9:
        move_check = sf::Vector2f(441, 102);
        break; // Move 35 (-41 in y-axis from 143)
    case 10:
        move_check = sf::Vector2f(441, 61);
        break; // Move 36 (-41 in y-axis from 102)
    case 11:
        move_check = sf::Vector2f(441, 22);
        break; // Move 37 (-41 in y-axis from 61)
    case 12:
        move_check = sf::Vector2f(482, 22);
        break; // Move 38 (+41 in x-axis from 441)
    case 13:
        move_check = sf::Vector2f(523, 22);
        break; // Move 39 (+41 in x-axis from 482)
    case 14:
        move_check = sf::Vector2f(523, 63);
        break; // Move 40 (+41 in y-axis from 19)
    case 15:
        move_check = sf::Vector2f(523, 103);
        break; // Move 41 (+41 in y-axis from 60)
    case 16:
        move_check = sf::Vector2f(523, 144);
        break; // Move 42 (+41 in y-axis from 101)
    case 17:
        move_check = sf::Vector2f(523, 185);
        break; // Move 43 (+41 in y-axis from 142)
    case 18:
        move_check = sf::Vector2f(523, 226);
        break; // Move 44 (+41 in y-axis from 183)
    case 19:
        move_check = sf::Vector2f(564, 266);
        break;
    case 20:
        move_check = sf::Vector2f(605, 266);
        break; // Move 46 (+41 in x-axis from 564)
    case 21:
        move_check = sf::Vector2f(646, 266);
        break; // Move 47 (+41 in x-axis from 605)
    case 22:
        move_check = sf::Vector2f(687, 266);
        break; // Move 48 (+41 in x-axis from 646)
    case 23:
        move_check = sf::Vector2f(728, 266);
        break; // Move 49 (+41 in x-axis from 687)
    case 24:
        move_check = sf::Vector2f(767, 266);
        break; // Move 50 (+41 in x-axis from 728)
    case 25:
        move_check = sf::Vector2f(767, 307);
        break; // Move 51 (Y +41 from 266)
    case 26:
        move_check = sf::Vector2f(767, 348);
        break;
    case 27:
        move_check = sf::Vector2f(726, 348);
        break; // 767 - 41 = 726
    case 28:
        move_check = sf::Vector2f(685, 348);
        break; // 726 - 41 = 685
    case 29:
        move_check = sf::Vector2f(644, 348);
        break; // 685 - 41 = 644
    case 30:
        move_check = sf::Vector2f(603, 348);
        break; // 644 - 41 = 603
    case 31:
        move_check = sf::Vector2f(562, 348);
        break; // 603 - 41 = 562
    case 32:
        move_check = sf::Vector2f(522, 389);
        break;
    case 33:
        move_check = sf::Vector2f(522, 430);
        break; // 389 + 41 = 430
    case 34:
        move_check = sf::Vector2f(522, 471);
        break; // 430 + 41 = 471
    case 35:
        move_check = sf::Vector2f(522, 512);
        break; // 471 + 41 = 512
    case 36:
        move_check = sf::Vector2f(522, 553);
        break; // 512 + 41 = 553
    case 37:
        move_check = sf::Vector2f(522, 592);
        break; // 553 + 41 = 594
    case 38:
        move_check = sf::Vector2f(481, 592);
        break; // 522 - 41 = 481
    case 39:
        move_check = sf::Vector2f(441, 592);
        break; // 481 - 41 = 440
    case 40:
        move_check = sf::Vector2f(441, 551);
        break; // 592 - 41 = 551
    case 41:
        move_check = sf::Vector2f(441, 510);
        break; // 551 - 41 = 510
    case 42:
        move_check = sf::Vector2f(441, 469);
        break; // 510 - 41 = 469
    case 43:
        move_check = sf::Vector2f(441, 428);
        break; // 469 - 41 = 428
    case 44:
        move_check = sf::Vector2f(441, 387);
        break; // 428 - 41 = 387
    case 45:
        move_check = sf::Vector2f(400, 346);
        break;
    case 46:
        move_check = sf::Vector2f(359, 346);
        break; // 400 - 41 = 359
    case 47:
        move_check = sf::Vector2f(318, 346);
        break; // 359 - 41 = 318
    case 48:
        move_check = sf::Vector2f(277, 346);
        break; // 318 - 41 = 277
    case 49:
        move_check = sf::Vector2f(237, 346);
        break; // 277 - 41 = 236
    case 50:
        move_check = sf::Vector2f(196, 346);
        break; // 236 - 41 = 195
    case 51:
        move_check = sf::Vector2f(197, 306);
        break;
    case 52:
        move_check = sf::Vector2f(238, 306);
        break; // 197 + 41 = 238
    case 53:
        move_check = sf::Vector2f(279, 306);
        break; // 238 + 41 = 279
    case 54:
        move_check = sf::Vector2f(320, 306);
        break; // 279 + 41 = 320
    case 55:
        move_check = sf::Vector2f(361, 306);
        break; // 320 + 41 = 361
    case 56:
        move_check = sf::Vector2f(401, 306);
        break; // 361 + 41 = 402
    }
    cout << "red player " << i + 1 << "Moved" << endl;
    red_player[i].change_position(move_check);
}
void green_position(PLAYER green_player[], int i)
{
    Vector2f move_check;
    switch (green_player[i].current_position)
    {
    case 1:
        move_check = sf::Vector2f(523, 63);
        break; // Move 40 (+41 in y-axis from 19)
    case 2:
        move_check = sf::Vector2f(523, 103);
        break; // Move 41 (+41 in y-axis from 60)
    case 3:
        move_check = sf::Vector2f(523, 144);
        break; // Move 42 (+41 in y-axis from 101)
    case 4:
        move_check = sf::Vector2f(523, 185);
        break; // Move 43 (+41 in y-axis from 142)
    case 5:
        move_check = sf::Vector2f(523, 226);
        break; // Move 44 (+41 in y-axis from 183)
    case 6:
        move_check = sf::Vector2f(564, 266);
        break;
    case 7:
        move_check = sf::Vector2f(605, 266);
        break; // Move 46 (+41 in x-axis from 564)
    case 8:
        move_check = sf::Vector2f(646, 266);
        break; // Move 47 (+41 in x-axis from 605)
    case 9:
        move_check = sf::Vector2f(687, 266);
        break; // Move 48 (+41 in x-axis from 646)
    case 10:
        move_check = sf::Vector2f(728, 266);
        break; // Move 49 (+41 in x-axis from 687)
    case 11:
        move_check = sf::Vector2f(767, 266);
        break; // Move 50 (+41 in x-axis from 728)
    case 12:
        move_check = sf::Vector2f(767, 307);
        break; // Move 51 (Y +41 from 266)
    case 13:
        move_check = sf::Vector2f(767, 348);
        break;
    case 14:
        move_check = sf::Vector2f(726, 348);
        break; // 767 - 41 = 726
    case 15:
        move_check = sf::Vector2f(685, 348);
        break; // 726 - 41 = 685
    case 16:
        move_check = sf::Vector2f(644, 348);
        break; // 685 - 41 = 644
    case 17:
        move_check = sf::Vector2f(603, 348);
        break; // 644 - 41 = 603
    case 18:
        move_check = sf::Vector2f(562, 348);
        break; // 603 - 41 = 562
    case 19:
        move_check = sf::Vector2f(522, 389);
        break;
    case 20:
        move_check = sf::Vector2f(522, 430);
        break; // 389 + 41 = 430
    case 21:
        move_check = sf::Vector2f(522, 471);
        break; // 430 + 41 = 471
    case 22:
        move_check = sf::Vector2f(522, 512);
        break; // 471 + 41 = 512
    case 23:
        move_check = sf::Vector2f(522, 553);
        break; // 512 + 41 = 553
    case 24:
        move_check = sf::Vector2f(522, 592);
        break; // 553 + 41 = 594
    case 25:
        move_check = sf::Vector2f(481, 592);
        break; // 522 - 41 = 481
    case 26:
        move_check = sf::Vector2f(441, 592);
        break; // 481 - 41 = 440
    case 27:
        move_check = sf::Vector2f(441, 551);
        break; // 592 - 41 = 551
    case 28:
        move_check = sf::Vector2f(441, 510);
        break; // 551 - 41 = 510
    case 29:
        move_check = sf::Vector2f(441, 469);
        break; // 510 - 41 = 469
    case 30:
        move_check = sf::Vector2f(441, 428);
        break; // 469 - 41 = 428
    case 31:
        move_check = sf::Vector2f(441, 387);
        break; // 428 - 41 = 387
    case 32:
        move_check = sf::Vector2f(400, 346);
        break;
    case 33:
        move_check = sf::Vector2f(359, 346);
        break; // 400 - 41 = 359
    case 34:
        move_check = sf::Vector2f(318, 346);
        break; // 359 - 41 = 318
    case 35:
        move_check = sf::Vector2f(277, 346);
        break; // 318 - 41 = 277
    case 36:
        move_check = sf::Vector2f(237, 346);
        break; // 277 - 41 = 236
    case 37:
        move_check = sf::Vector2f(196, 346);
        break; // 236 - 41 = 195
    case 38:
        move_check = sf::Vector2f(197, 306);
        break;
    case 39:
        move_check = sf::Vector2f(195, 266);
        break; // Move 26 (-41 in y-axis from 307)
    case 40:
        move_check = sf::Vector2f(236, 266);
        break; // Move 27 (+41 in x-axis from 195)
    case 41:
        move_check = sf::Vector2f(277, 266);
        break; // Move 28 (+41 in x-axis from 236)
    case 42:
        move_check = sf::Vector2f(318, 266);
        break; // Move 29 (+41 in x-axis from 277)
    case 43:
        move_check = sf::Vector2f(359, 266);
        break; // Move 30 (+41 in x-axis from 318)
    case 44:
        move_check = sf::Vector2f(400, 266);
        break; // Move 31 (+41 in x-axis from 359)
    case 45:
        move_check = sf::Vector2f(441, 225);
        break; // 400 + 41 = 441, 266 - 41 = 225
    case 46:
        move_check = sf::Vector2f(441, 184);
        break; // Move 33 (-41 in y-axis from 225)
    case 47:
        move_check = sf::Vector2f(441, 143);
        break; // Move 34 (-41 in y-axis from 184)
    case 48:
        move_check = sf::Vector2f(441, 102);
        break; // Move 35 (-41 in y-axis from 143)
    case 49:
        move_check = sf::Vector2f(441, 61);
        break; // Move 36 (-41 in y-axis from 102)
    case 50:
        move_check = sf::Vector2f(441, 22);
        break; // Move 37 (-41 in y-axis from 61)
    case 51:
        move_check = sf::Vector2f(482, 22);
        break; // Move 38 (+41 in x-axis from 441)
    case 52:
        move_check = sf::Vector2f(482, 63);
        break; // 22 + 41 = 63
    case 53:
        move_check = sf::Vector2f(482, 104);
        break; // 63 + 41 = 104
    case 54:
        move_check = sf::Vector2f(482, 145);
        break; // 104 + 41 = 145
    case 55:
        move_check = sf::Vector2f(482, 186);
        break; // 145 + 41 = 186
    case 56:
        move_check = sf::Vector2f(482, 227);
        break; // 186 + 41 = 227
    }
    cout << "green player " << i + 1 << "Moved" << endl;
    green_player[i].change_position(move_check);
}

void friendly_overlapping(Event &event, PLAYER player[])
{

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(i!=j)
            if (player[i].image_sprite.getGlobalBounds().intersects(player[j].image_sprite.getGlobalBounds()))
            {
                cout << "their was overlap by player " << turn << " on " << " green" << endl;
                if (j == 0)
                {
                    cout << "1" << endl;
                    float x = (player[j].image_sprite.getPosition().x);
                    float y = (player[j].image_sprite.getPosition().y);
                    player[j].change_position(Vector2f(x, y));
                    
                }
                else if (j == 1)
                {
                    cout << "2" << endl;
                    float x = (player[j].image_sprite.getPosition().x) + 21.3f;
                    float y = (player[j].image_sprite.getPosition().y);
                    player[j].change_position(Vector2f(x, y));
                    
                }
                else if (j == 2)
                {
                    cout << "3" << endl;
                    float x = (player[j].image_sprite.getPosition().x);
                    float y = (player[j].image_sprite.getPosition().y) + 21.3f;
                    player[j].change_position(Vector2f(x, y));
                    
                }
                else
                {
                    float x = (player[j].image_sprite.getPosition().x) + 21.3f;
                    float y = (player[j].image_sprite.getPosition().y) + 21.3f;
                    player[j].change_position(Vector2f(x, y));
                    cout << "4" << endl;
                    
                }
                if (i == 0)
                {
                    cout << "1" << endl;
                    float x = (player[i].image_sprite.getPosition().x);
                    float y = (player[i].image_sprite.getPosition().y);
                    player[i].change_position(Vector2f(x, y));
                    
                }
                else if (i == 1)
                {
                    cout << "2" << endl;
                    float x = (player[i].image_sprite.getPosition().x) + 21.3f;
                    float y = (player[i].image_sprite.getPosition().y);
                    player[i].change_position(Vector2f(x, y));
                    
                }
                else if (i == 2)
                {
                    cout << "3" << endl;
                    float x = (player[i].image_sprite.getPosition().x);
                    float y = (player[i].image_sprite.getPosition().y) + 21.3f;
                    player[i].change_position(Vector2f(x, y));
                    
                }
                else
                {
                    float x = (player[i].image_sprite.getPosition().x) + 21.3f;
                    float y = (player[i].image_sprite.getPosition().y) + 21.3f;
                    player[i].change_position(Vector2f(x, y));
                    cout << "4" << endl;
                    
                }
                player[j].change_scale(Vector2f(0.2f, 0.2f));
                player[i].change_scale(Vector2f(0.2f, 0.2f));
            }
        }
    }
}