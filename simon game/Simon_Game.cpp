
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <chrono>
using namespace std::chrono;


constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;
constexpr float BUTTON_SIZE = 100;

enum class ColorEnum { RED, GREEN, YELLOW, BLUE, COUNT };

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> colorDistribution(0, static_cast<int>(ColorEnum::COUNT) - 1);




class Game {
public:
	bool first_time = true;
	bool  time_flag = false;
	high_resolution_clock::time_point currentTime;

	sf::SoundBuffer soundBuffer1;
	sf::SoundBuffer soundBuffer2;
	sf::SoundBuffer soundBuffer3;
	sf::SoundBuffer soundBuffer4;
	sf::SoundBuffer soundBuffer5;
	sf::SoundBuffer soundBuffer6;
	sf::Sound red;
	sf::Sound green;
	sf::Sound blue;
	sf::Sound yellow;
	sf::Sound level_up;
	sf::Sound game_over_sound;
	int currentStep = 0;
	bool gameStarted = false;
	bool waitingForInput = false;
	std::vector<ColorEnum> sequence;
	high_resolution_clock::time_point lastButtonPressTime;

public:

	Game()
	{
		this->time_flag = false;
		//lastButtonPressTime= std::chrono::duration_cast<duration<double>>();
		soundBuffer1.loadFromFile("sounds/red.wav");
		red.setBuffer(soundBuffer1);
		soundBuffer2.loadFromFile("sounds/yellow.wav");
		yellow.setBuffer(soundBuffer2);
		soundBuffer3.loadFromFile("sounds/green.wav");
		green.setBuffer(soundBuffer3);
		soundBuffer4.loadFromFile("sounds/blue.wav");
		blue.setBuffer(soundBuffer4);
		soundBuffer5.loadFromFile("sounds/level_up.wav");
		level_up.setBuffer(soundBuffer5);
		soundBuffer6.loadFromFile("sounds/game_over.wav");
		game_over_sound.setBuffer(soundBuffer6);
	};

	void start_Button_onpressed(std::vector<tgui::Panel::Ptr>  colorButtons, sf::RenderWindow& window, tgui::GuiSFML& gui)
	{
		if (!gameStarted) {
			gameStarted = true;
			this->currentTime = steady_clock::now();
			this->time_flag = true;
			sequence.push_back(static_cast<ColorEnum>(colorDistribution(gen)));
			waitingForInput = true;
			std::cout << "New game started" << std::endl;
			std::cout << "Sequence: ";
			game_sound_sequence(colorButtons, window, gui);
		}
	}

	sf::Color getColor(ColorEnum color) {
		switch (color) {
		case ColorEnum::RED:
			return sf::Color::Red;
		case ColorEnum::GREEN:
			return sf::Color::Green;
		case ColorEnum::YELLOW:
			return sf::Color::Yellow;
		case ColorEnum::BLUE:
			return sf::Color::Blue;
		default:
			return sf::Color::White;
		}
	}

	void buttonClick(tgui::Panel::Ptr& b, tgui::GuiSFML& gui, sf::RenderWindow& window, sf::Sound sound) {
		auto color = b->getRenderer()->getBackgroundColor();
		b->getRenderer()->setBackgroundColor(sf::Color::Magenta);
		sound.play();
		window.clear();
		gui.draw();
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // wait for 500ms between button clicks
		b->getRenderer()->setBackgroundColor(color);
		window.clear();
		gui.draw();
		window.display();


	}

	sf::Sound checkUserInput(sf::Color color, sf::Sound red, sf::Sound green, sf::Sound blue, sf::Sound yellow)
	{
		if (color == sf::Color::Red) {
			return red;

		}
		else if (color == sf::Color::Green) {
			return green;
		}
		else if (color == sf::Color::Yellow) {
			return yellow;


		}
		else if (color == sf::Color::Blue) {
			return blue;


		}
		return sf::Sound::Sound();

	}

	void gameOver(sf::RenderWindow& window, tgui::GuiSFML& gui) {
		auto round = (sequence.size() - 1);
		game_over_sound.play();
		gameStarted = false;
		waitingForInput = false;
		currentStep = 0;
		sequence.clear();
		std::cout << "Game Over!" << std::endl;
		std::cout << "Sorry, wrong color. You made it ON round " << (sequence.size() - 1) << " of the game." << std::endl;


		tgui::ChildWindow::Ptr popupWindow = tgui::ChildWindow::create();
		popupWindow->setPosition(300, 150);
		popupWindow->setSize(250, 250);
		popupWindow->setTitle("Wrong Symphony");

		// Add contents to the pop-up window
		auto label = tgui::Label::create();
		std::string game_over_message = "you reached round: ";
		game_over_message += std::to_string(round);
		label->setText(game_over_message);
		label->setTextSize(12);
		popupWindow->add(label);

		auto RestartButton = tgui::Button::create("play again");
		RestartButton->setPosition(0, 30);
		RestartButton->onPress([&, popupWindow]() {gui.remove(popupWindow); });

		popupWindow->add(RestartButton);

		auto quitButton = tgui::Button::create("quit");
		quitButton->onClick([&]() {window.close(); });
		quitButton->setPosition(0, 60);
		popupWindow->add(quitButton);


		// Add the pop-up window to the GUI
		gui.add(popupWindow);
		this->time_flag = false;
		this->first_time = true;

	};

	void game_sound_sequence(std::vector<tgui::Panel::Ptr>  colorButtons, sf::RenderWindow& window, tgui::GuiSFML& gui)
	{
		for (auto color : sequence) {
			switch (static_cast<int>(color)) {
			case 0:
				std::cout << "Red \n";
				buttonClick(colorButtons[0], gui, window, red);
				break;

			case 1:
				std::cout << "Green\n";
				buttonClick(colorButtons[1], gui, window, green);
				break;

			case 2:
				std::cout << "Yellow\n";
				buttonClick(colorButtons[2], gui, window, yellow);
				break;

			case 3:
				std::cout << "Blue\n";
				buttonClick(colorButtons[3], gui, window, blue);
				break;

			default:
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // wait for 500ms between button clicks
		}
	}

	bool hasGameTimedOut(high_resolution_clock::time_point lastButtonPressTime, std::chrono::seconds gameTimeoutDuration)
	{
		auto currentTime = high_resolution_clock::now();
		auto elapsedDuration = currentTime - lastButtonPressTime;
		return elapsedDuration >= gameTimeoutDuration;
	}
};

int main() {
	Game game = Game();


	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Simon Game");

	window.setFramerateLimit(60);

	tgui::GuiSFML gui(window);

	auto startButton = tgui::Button::create("Start Game");
	startButton->setSize({ BUTTON_SIZE, BUTTON_SIZE });

	startButton->setPosition((WINDOW_WIDTH - BUTTON_SIZE) / 2, (WINDOW_HEIGHT - BUTTON_SIZE) / 2);
	gui.add(startButton);

	std::vector<tgui::Panel::Ptr> colorButtons(static_cast<int>(ColorEnum::COUNT));

	for (int i = 0; i < static_cast<int>(ColorEnum::COUNT); ++i)
	{
		colorButtons[i] = tgui::Panel::create();
		colorButtons[i]->setSize({ BUTTON_SIZE, BUTTON_SIZE });
		colorButtons[i]->getRenderer()->setBackgroundColor(game.getColor(static_cast<ColorEnum>(i)));
		gui.add(colorButtons[i]);
	}

	colorButtons[static_cast<int>(ColorEnum::RED)]->setPosition((WINDOW_WIDTH - BUTTON_SIZE) / 2 - BUTTON_SIZE - 20, (WINDOW_HEIGHT - BUTTON_SIZE) / 2);
	colorButtons[static_cast<int>(ColorEnum::GREEN)]->setPosition((WINDOW_WIDTH - BUTTON_SIZE) / 2 + BUTTON_SIZE + 20, (WINDOW_HEIGHT - BUTTON_SIZE) / 2);
	colorButtons[static_cast<int>(ColorEnum::YELLOW)]->setPosition((WINDOW_WIDTH - BUTTON_SIZE) / 2, (WINDOW_HEIGHT - BUTTON_SIZE) / 2 - BUTTON_SIZE - 20);
	colorButtons[static_cast<int>(ColorEnum::BLUE)]->setPosition((WINDOW_WIDTH - BUTTON_SIZE) / 2, (WINDOW_HEIGHT - BUTTON_SIZE) / 2 + BUTTON_SIZE + 20);

	startButton->onPress([&]() {


		game.start_Button_onpressed(colorButtons, window, gui); });


	game.lastButtonPressTime = high_resolution_clock::now();


	std::this_thread::sleep_for(std::chrono::milliseconds(500)); // wait for 500ms between button clicks

	for (int i = 0; i < static_cast<int>(ColorEnum::COUNT); ++i) {




		colorButtons[i]->onClick([&, i]() {


			sf::Color color = colorButtons[i]->getRenderer()->getBackgroundColor();

			sf::Sound sound_is = game.checkUserInput(color, game.red, game.green, game.blue, game.yellow);

			game.buttonClick(colorButtons[i], gui, window, sound_is);

			if (game.gameStarted && game.waitingForInput) {

				if (game.sequence[game.currentStep] == static_cast<ColorEnum>(i)) {

					game.currentStep++;
					if (game.currentStep >= static_cast<int>(game.sequence.size())) {
						game.sequence.push_back(static_cast<ColorEnum>(colorDistribution(gen)));
						game.currentStep = 0;
						game.level_up.play();
						std::cout << "Correct! New sequence length: " << game.sequence.size() << std::endl;
						std::chrono::seconds sleepDuration(1);
						std::this_thread::sleep_for(sleepDuration);

						game.game_sound_sequence(colorButtons, window, gui);
					}
				}
				else
				{
					game.gameOver(window, gui);
				}

			}
			game.lastButtonPressTime = steady_clock::now();
			});

	}
	auto startTime = std::chrono::system_clock::now();



	// auto elapsedDuration = currentTime - lastButtonPressTime;


	 // Start the game
	while (window.isOpen())
	{




		if (game.time_flag == true) {
			game.currentTime = steady_clock::now();
			if (game.first_time == true) {
				game.lastButtonPressTime = game.currentTime;
				game.first_time = false;

			}
			std::chrono::duration<double> elapsed = duration_cast<duration<double>>(game.currentTime - game.lastButtonPressTime);
			std::cout << elapsed.count() << "\n";

			if (elapsed.count() >= 5.0)
			{
				std::cout << elapsed.count();
				game.gameOver(window, gui);
				elapsed = game.currentTime - game.currentTime;


			}
		}

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			gui.handleEvent(event);
		}
		window.clear();
		gui.draw();
		window.display();

	}
}