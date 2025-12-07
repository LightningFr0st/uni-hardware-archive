#include <SDL.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "com_port_reader.h"

static constexpr int NUM_LINKS{ 5 };

static constexpr char PORT_NAME[]{ "COM5" };

static inline double deg2rad(double d)
{
	return d * M_PI / 180.0;
}

int main(int argc, char* argv[])
{
	using namespace mpu_6050;

	const int WIN_W = 1280;
	const int WIN_H = 720;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow(
		"MPU Motion",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIN_W, WIN_H, SDL_WINDOW_SHOWN);

	if (!win) {
		fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(
		win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!ren) {
		fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	array<atomic<double>, NUM_LINKS> roll_deg;
	for (auto& a : roll_deg) a.store(0.0);

	ComPortReader reader;
	reader.set_callback([&](MpuPacket const& pkt) {
		if (pkt.mpu_addr >= 0 && pkt.mpu_addr <= 4)
		{
			roll_deg[pkt.mpu_addr].store(pkt.kx, memory_order_relaxed);
		}
		});

	if (!reader.open(PORT_NAME, 115200)) {
		fprintf(stderr, "Failed to open %s\n", PORT_NAME);
		SDL_DestroyRenderer(ren); SDL_DestroyWindow(win); SDL_Quit();
		return 1;
	}

	const double LEFT_MARGIN = 60.0;
	const double RIGHT_MARGIN = 60.0;
	const double TOP_MARGIN = 40.0;
	const double BOTTOM_MARGIN = 40.0;

	auto compute_lengths = [&](int w, int h) {
		const double baseX = LEFT_MARGIN;
		const double baseY = h * 0.5;

		const double maxH = (h - TOP_MARGIN - BOTTOM_MARGIN);
		const double maxW = (w - baseX - RIGHT_MARGIN);

		const double totalLen = 0.92 * min(maxH, maxW);

		const double r1 = 0.24, r2 = 0.22, r3 = 0.20, r4 = 0.16, r5 = 0.10;
		return array<double, NUM_LINKS>{
			totalLen* r1,
				totalLen* r2,
				totalLen* r3,
				totalLen* r4,
				totalLen* r5
		};
		};

	array<double, NUM_LINKS> L = compute_lengths(WIN_W, WIN_H);

	bool quit = false;
	while (!quit) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) quit = true;
			if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				int w = ev.window.data1, h = ev.window.data2;
				L = compute_lengths(w, h);
			}
		}

		int w, h; SDL_GetRendererOutputSize(ren, &w, &h);

		array<double, NUM_LINKS> a_deg;
		for (int i = 0; i < NUM_LINKS; ++i)
			a_deg[i] = roll_deg[i].load(memory_order_relaxed);

		array<double, NUM_LINKS> a_rad{};
		double acc = 0.0;
		for (int i = 0; i < NUM_LINKS; ++i) {
			acc += a_deg[i];
			a_rad[i] = deg2rad(acc);
		}

		const double baseX = LEFT_MARGIN;
		const double baseY = h * 0.5;

		struct Pt { double x, y; };
		array<Pt, NUM_LINKS + 1> P;

		P[0] = { baseX, baseY };
		for (int i = 0; i < NUM_LINKS; ++i) {
			const Pt& prev = P[i];
			P[i + 1] = {
				prev.x + L[i] * cos(a_rad[i]),
				prev.y - L[i] * sin(a_rad[i])
			};
		}
		auto toSDL = [](const Pt& q) { return SDL_Point{ (int)lround(q.x), (int)lround(q.y) }; };

		SDL_SetRenderDrawColor(ren, 12, 12, 14, 255);
		SDL_RenderClear(ren);

		SDL_SetRenderDrawColor(ren, 40, 40, 48, 255);
		const int step = 80;
		for (int y = (int)TOP_MARGIN; y < h - (int)BOTTOM_MARGIN; y += step)
			SDL_RenderDrawLine(ren, 0, y, w, y);

		SDL_SetRenderDrawColor(ren, 230, 230, 230, 255);
		for (int i = 0; i < NUM_LINKS; ++i) {
			SDL_Point a = toSDL(P[i]);
			SDL_Point b = toSDL(P[i + 1]);
			SDL_RenderDrawLine(ren, a.x, a.y, b.x, b.y);
		}

		SDL_SetRenderDrawColor(ren, 0, 170, 255, 255);
		for (auto& q : P) {
			SDL_Point s = toSDL(q);
			SDL_RenderDrawLine(ren, s.x - 3, s.y, s.x + 3, s.y);
			SDL_RenderDrawLine(ren, s.x, s.y - 3, s.x, s.y + 3);
		}
		SDL_Delay(10);
		SDL_RenderPresent(ren);
	}

	reader.close();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
