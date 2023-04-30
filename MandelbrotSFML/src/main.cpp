#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>

#include <ppl.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

const int SET_WIDTH = 800;
const int SET_HEIGHT = 800;

long double old_min_x = -99.l;
long double old_max_x = -99.l;
long double old_min_y = -99.l;
long double old_max_y = -99.l;

long double X_MIN = -1.7433419072021l; // 2.84 -2.0l
long double X_MAX = -1.7433419034621l; // 0.47l
long double Y_MIN = 0.0000907668789l;
long double Y_MAX = 0.0000907706189l;

long double factor = 1.0l;

const int MAX_ITERATIONS = 300;
const int MEDIUM_ITERATIONS = 200;
const int MIN_ITERATIONS = 100;
int MAXIMUM_ITERATIONS = MEDIUM_ITERATIONS;

typedef sf::Vector2<long double> Vector2d;

long double Map(
	long double value,
	long double in_min, long double in_max,
	long double out_min, long double out_max)
{
	long double slope = (out_max - out_min) / (in_max - in_min);
	return out_min + slope * (value - in_min);
}

// true, if 
bool SaveRange() {
	bool changed =
		old_min_x != X_MIN ||
		old_min_y != Y_MIN ||

		old_max_x != X_MAX ||
		old_max_y != Y_MAX;

	old_min_x = X_MIN;
	old_max_x = X_MAX;
	old_min_y = Y_MIN;
	old_max_y = Y_MAX;

	if (0) {
		std::cout << "x_min was " << old_min_x << " ; x_min is " << X_MIN << "\n";
		std::cout << "x_max was " << old_max_x << " ; x_max is " << X_MAX << "\n";
		std::cout << "y_min was " << old_min_y << " ; y_min is " << Y_MIN << "\n";
		std::cout << "y_max was " << old_max_y << " ; y_max is " << Y_MAX << "\n";
	}

	return changed;
}

Vector2d Map(sf::Vector2i& pos,
	long double in_min, long double in_max,
	long double out_min, long double out_max)
{
	long double pos_x = Map(pos.x, in_min, in_max, out_min, out_max);
	long double pos_y = Map(pos.y, in_min, in_max, out_min, out_max);

	return Vector2d(pos_x, pos_y);
}

int main()
{
	std::cout << std::string(
		"R - reset everything\n") +
		"Z - zoom in, X - zoom out\n" +
		"Mouse pan or arrow keys to move around\n" +
		"B - minimum, N - medium, M - max number of iterations\n" +
		"numbers 1-9 for num*max number of iterations";

	bool useAntialising = 1;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8 * useAntialising;
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mandelbrot set SFML", sf::Style::Default, settings);

	sf::Event event;
	std::string filename = ".screenshots\\";
	sf::Texture texture;
	texture.create(SET_WIDTH, SET_HEIGHT);
	sf::Sprite sprite(texture);
	sf::Image image;
	image.create(SET_WIDTH, SET_HEIGHT, sf::Color::Black);

	int count = 0;
	int manual = 0;
	Vector2d mouseOldPos;

	bool saveScreenshots = 0;
	bool dynamicIterationsGrowth = 0;
	bool isPanningEnabled = 1;
	bool shouldRedraw = 0;

	bool isMouseDown = 0;
	bool isKeyDown = 0;
	bool isZoomingIn = 0;
	bool isZoomingOut = 0;

	long double redSlope = (255.0l - 0) / (6502.0l - 0);
	long double blueSlope = (255.0l - 0) / (15.968719422671311999070245176981l - 0);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
				isMouseDown = true;
				auto pos = sf::Mouse::getPosition();
				auto old_x = Map(pos.x, 0, WINDOW_WIDTH, X_MIN, X_MAX);
				auto old_y = Map(pos.y, 0, WINDOW_WIDTH, X_MIN, X_MAX);
				mouseOldPos = Vector2d(old_x, old_y);

				MAXIMUM_ITERATIONS = MIN_ITERATIONS;
			}

			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
				isMouseDown = false;
				MAXIMUM_ITERATIONS = MAX_ITERATIONS;
			}

			if (event.type == sf::Event::MouseMoved) {
				if (isMouseDown) {
					auto mousePos = sf::Mouse::getPosition();
					auto x = Map(mousePos.x, 0, WINDOW_WIDTH, X_MIN, X_MAX);
					auto y = Map(mousePos.y, 0, WINDOW_WIDTH, Y_MIN, Y_MAX);
					auto deltaPos = mouseOldPos - Vector2d(x, y);

					auto delta_x = deltaPos.x;
					auto delta_y = deltaPos.y;

					//std::cout << "delta_x: " << delta_x << " delta_y: " << delta_y << "\n";
					//std::cout << "X_MIN: " << X_MIN << "; X_MAX: " << X_MAX << "\n\n";

					X_MIN += delta_x;
					X_MAX += delta_x;

					Y_MIN += delta_y;
					Y_MAX += delta_y;
				}
			}

			if (event.type == sf::Event::KeyPressed)
			{
				//isKeyDown = true;
				//MAXIMUM_ITERATIONS = MIN_ITERATIONS;
				auto key = event.key.code;

				if (key == sf::Keyboard::Z) {
					auto factor2 = 0.1l * factor;
					X_MAX -= factor2;
					Y_MAX -= factor2;

					X_MIN += factor2;
					Y_MIN += factor2;
					//factor *= 0.9349l;
					factor *= 0.95l;
				}
				else if (key == sf::Keyboard::X) {
					auto factor2 = 0.1l * factor;
					X_MAX += factor2;
					Y_MAX += factor2;

					X_MIN -= factor2;
					Y_MIN -= factor2;
					factor /= 0.95l;
				}
				else if (key == sf::Keyboard::F) {
					texture.copyToImage().saveToFile(filename + "manual_" + std::to_string(manual++) + ".jpg");
				}

				else if (key >= sf::Keyboard::Num1 && key <= sf::Keyboard::Num9) {
					MAXIMUM_ITERATIONS = (key - 26) * MAX_ITERATIONS;
					std::cout << "maximum iterations: " << MAXIMUM_ITERATIONS << "\n";
				}
				else if (key == sf::Keyboard::Num0) {
					MAXIMUM_ITERATIONS = 5000;
					std::cout << "maximum iterations: " << MAXIMUM_ITERATIONS << "\n";
				}
				else if (key == sf::Keyboard::B) {
					MAXIMUM_ITERATIONS = MIN_ITERATIONS;
				} else if (key == sf::Keyboard::N) {
					MAXIMUM_ITERATIONS = MEDIUM_ITERATIONS;
				} else if (key == sf::Keyboard::M) {
					MAXIMUM_ITERATIONS = MAX_ITERATIONS;
				} else if (key == sf::Keyboard::E) {
					shouldRedraw = true;
				}

				const static long double pan_step = 0.05l;
				if (key == sf::Keyboard::Up) {
					auto diff = pan_step * (Y_MAX - Y_MIN);
					Y_MIN -= diff;
					Y_MAX -= diff;
				}
				else if (key == sf::Keyboard::Down) {
					auto diff = pan_step * (Y_MAX - Y_MIN);
					Y_MIN += diff;
					Y_MAX += diff;
				}
				else if (key == sf::Keyboard::Right) {
					auto diff = pan_step * (X_MAX - X_MIN);
					X_MIN += diff;
					X_MAX += diff;
				}
				else if (key == sf::Keyboard::Left) {
					auto diff = pan_step * (X_MAX - X_MIN);
					X_MIN -= diff;
					X_MAX -= diff;
				}
				else if (key == sf::Keyboard::R) {
					X_MIN = Y_MIN = -2.l;
					X_MAX = Y_MAX = 2.l;
					factor = 1.l;
				}
			}

			//if (event.type == sf::Event::KeyReleased) {
			//	isKeyDown = false;
			//	//MAXIMUM_ITERATIONS = MAX_ITERATIONS;
			//}
		}

		if (!SaveRange() && !shouldRedraw) {
			// std::cout << "skipping \n";
			continue;
		}
		else
			shouldRedraw = true;

		long double slope_y = (Y_MAX - Y_MIN) / (SET_HEIGHT - 0);
		long double slope_x = (X_MAX - X_MIN) / (SET_WIDTH - 0);

		// #pragma loop(hint_parallel(8))
		//for (int screen_y = 0; screen_y < SET_HEIGHT; screen_y++)

		concurrency::parallel_for(0, SET_HEIGHT, [&](int screen_y)
		{
			long double mapped_y = Y_MIN + slope_y * (screen_y - 0);

			for (int screen_x = 0; screen_x < SET_WIDTH; screen_x++)
			{
				long double mapped_x = X_MIN + slope_x * (screen_x - 0);

				long double x = 0.0l;
				long double y = 0.0l;

				long double x2 = 0.0l;
				long double y2 = 0.0l;

				int n = 0;
				while (n < MAXIMUM_ITERATIONS)
				{
					y = (x + x) * y + mapped_y; // 2*x*y = (x+x)*y = x*(y+y)
					x = x2 - y2 + mapped_x;
					x2 = x * x;
					y2 = y * y;
					if (x2 + y2 > 4) break;

					n++;
				}

				long double bright = 0;
				if (n != MAXIMUM_ITERATIONS)
				{
					bright = 255.0l / MAX_ITERATIONS * n;
				}

				int r = redSlope * bright * bright;
				int g = bright;
				int b = blueSlope * sqrt(bright);

				image.setPixel(screen_x, screen_y, sf::Color(r, g, b, 255));
			}
		});

		window.clear();

		texture.update(image);
		window.draw(sprite);

		if (isMouseDown)
		{
			sf::CircleShape circle;
			circle.setFillColor(sf::Color::White);
			circle.setRadius(2);
			circle.setPosition(WINDOW_WIDTH / 2 + 1, WINDOW_HEIGHT / 2 + 1);

			window.draw(circle);
		}
		window.display();

		if (saveScreenshots) {
			// std::cout << "saving screenshot " << std::to_string(count) << "\n";
			texture.copyToImage().saveToFile(filename + std::to_string(count++) + ".jpg");
		}
	}

	return 0;
}
