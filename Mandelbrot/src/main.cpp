#include <SDL.h>
#include <string>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const int SET_WIDTH = 800;
const int SET_HEIGHT = 800;

/*long double SET_MIN = -2.0l;
long double SET_MAX = 2.0l;*/

/*
long double X_MIN = -2.0l; // -2.0l
long double X_MAX = 2.0l; // 0.47l
long double Y_MIN = -2.0l; // -1.12l
long double Y_MAX = 2.0l; // 1.12l
*/

long double X_MIN = -2.84l; // -2.0l
long double X_MAX = 1.0l; // 0.47l
long double Y_MIN = -2.84l; // -1.12l
long double Y_MAX = 1.0l; // 1.12l

long double factor = 1.0l;
int MAX_ITERATIONS = 300;

long double Map(
	long double value,
	long double in_min, long double in_max,
	long double out_min, long double out_max)
{
	long double slope = (out_max - out_min) / (in_max - in_min);
	return out_min + slope * (value - in_min);
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event sdlEvent;

	//SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
	SDL_CreateWindowAndRenderer(SET_WIDTH, SET_HEIGHT, 0, &window, &renderer);
	SDL_RenderSetLogicalSize(renderer, SET_WIDTH, SET_HEIGHT);
	SDL_SetWindowTitle(window, "Mandelbrot set zoom");
	
	int count = 0;
	bool saveToScreenshot = 1;

	bool isRunning = 1;
	while (isRunning)
	{
		long double fac_max = 0.1l * factor;
		Y_MAX -= fac_max;
		X_MAX -= fac_max;

		long double fac_min = 0.15l * factor;
		Y_MIN += fac_min;
		X_MIN += fac_min;

		factor *= 0.9349l;
		MAX_ITERATIONS += 5;

		if (count > 30) MAX_ITERATIONS *= 1.02l;

		SDL_RenderPresent(renderer);
		if (SDL_PollEvent(&sdlEvent))
		{
			if (sdlEvent.type == SDL_QUIT)
				return 0;

			else if (sdlEvent.type == SDL_KEYDOWN)
			{
				switch (sdlEvent.key.keysym.sym)
				{
					case SDLK_q:
						return 0;
				}
			}
		}

		// avoid recalculation
		long double slope_y = (Y_MAX - Y_MIN) / (SET_HEIGHT - 0);
		long double slope_x = (X_MAX - X_MIN) / (SET_WIDTH - 0);

		long double redSlope = (255.0l - 0) / (6502.0l - 0);
		long double blueSlope = (255.0l - 0) / (15.968719422671311999070245176981l - 0);

		for (int screen_y = 0; screen_y < SET_HEIGHT; screen_y++)
		{
			// mapped_y = Map(y, 0, SET_WIDTH, SET_MIN, SET_MAX);
			long double mapped_y = Y_MIN + slope_y * (screen_y - 0);

			for (int screen_x = 0; screen_x < SET_WIDTH; screen_x++)
			{
				long double mapped_x = X_MIN + slope_x * (screen_x - 0);

				long double x = 0.0l;
				long double y = 0.0l;

				long double x2 = 0.0l;
				long double y2 = 0.0l;

				int n = 0;
				while (n < MAX_ITERATIONS)
				{
					y = (x + x) * y + mapped_y; // 2*x*y = (x+x)*y = x*(y+y)
					x = x2 - y2 + mapped_x;
					x2 = x * x;
					y2 = y * y;
					if (x2 + y2 > 4) break;

					/*long double xtemp = x * x - y * y + mapped_x;
					y = 2 * x * y + mapped_y;
					x = xtemp;
					if (x + y > 4) break;*/

					n++;
				}

				long double bright = 0;
				if (n != MAX_ITERATIONS)
				{
					bright = Map(n, 0, MAX_ITERATIONS, 0, 255);
				}

				int r = redSlope * bright * bright;
				int g = bright;
				int b = blueSlope * sqrt(bright);

				SDL_SetRenderDrawColor(renderer, r, g, b, 255);
				SDL_RenderDrawPoint(renderer, screen_x, screen_y);
			}
		}

		if (saveToScreenshot)
		{
			SDL_Surface* sshot = SDL_GetWindowSurface(window);
			SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
			std::string file = std::to_string(count) + ".bmp";
			SDL_SaveBMP(sshot, file.c_str());
			SDL_FreeSurface(sshot);
		}

		count++;
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}