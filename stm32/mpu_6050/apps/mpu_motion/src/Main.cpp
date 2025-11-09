#include <SDL.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "com_port_reader.h"
#include "mpu_processor.h"

static inline double deg2rad(double d) { return d * M_PI / 180.0; }

static inline int index_of(int i2c, int addr)
{
	return (i2c & 1) * 2 + (addr & 1);
}

static const char PORT_NAME[] = "COM3";

int main(int argc, char* argv[])
{
	const int WIN_W = 1280;
	const int WIN_H = 720;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		::std::fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow(
		"MPU Motion",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIN_W, WIN_H, SDL_WINDOW_SHOWN);

	if (!win) {
		::std::fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(
		win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!ren) {
		::std::fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	::std::array<::std::atomic<double>, 4> roll_deg;
	for (auto& a : roll_deg) a.store(0.0);

	::mpu_6050::MpuProcessor mpu(false);
	mpu.set_callback([&](::mpu_6050::MpuOutput const& out) {
		const int idx = index_of(out.i2c, out.addr);
		if (idx >= 0 && idx < 4) {
			roll_deg[idx].store(out.KalmanAngleX, ::std::memory_order_relaxed);
		}
		});

	::mpu_6050::ComPortReader reader;
	reader.set_callback([&](::mpu_6050::MpuPacket const& pkt) {
		mpu.feed(pkt);
		});

	if (!reader.open(PORT_NAME)) {
		::std::fprintf(stderr, "Failed to open %s\n", PORT_NAME);
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

		const double totalLen = 0.92 * ::std::min(maxH, maxW);

		const double r1 = 0.30, r2 = 0.27, r3 = 0.23, r4 = 0.20;
		return ::std::array<double, 4>{
			totalLen* r1, totalLen* r2, totalLen* r3, totalLen* r4
		};
		};

	::std::array<double, 4> L = compute_lengths(WIN_W, WIN_H);

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

		::std::array<double, 4> a_deg;
		for (int i = 0; i < 4; ++i) a_deg[i] = roll_deg[i].load(::std::memory_order_relaxed);

		::std::array<double, 4> a_rad{};
		double acc = 0.0;
		for (int i = 0; i < 4; ++i) {
			acc += a_deg[i];
			a_rad[i] = deg2rad(acc);
		}

		const double baseX = LEFT_MARGIN;
		const double baseY = h * 0.5;

		struct Pt { double x, y; };
		::std::array<Pt, 5> P;
		P[0] = { baseX, baseY };
		P[1] = { P[0].x + L[0] * ::std::cos(a_rad[0]), P[0].y - L[0] * ::std::sin(a_rad[0]) };
		P[2] = { P[1].x + L[1] * ::std::cos(a_rad[1]), P[1].y - L[1] * ::std::sin(a_rad[1]) };
		P[3] = { P[2].x + L[2] * ::std::cos(a_rad[2]), P[2].y - L[2] * ::std::sin(a_rad[2]) };
		P[4] = { P[3].x + L[3] * ::std::cos(a_rad[3]), P[3].y - L[3] * ::std::sin(a_rad[3]) };

		auto toSDL = [](const Pt& q) { return SDL_Point{ (int)::std::lround(q.x), (int)::std::lround(q.y) }; };

		SDL_SetRenderDrawColor(ren, 12, 12, 14, 255);
		SDL_RenderClear(ren);

		SDL_SetRenderDrawColor(ren, 40, 40, 48, 255);
		const int step = 80;
		for (int y = (int)TOP_MARGIN; y < h - (int)BOTTOM_MARGIN; y += step)
			SDL_RenderDrawLine(ren, 0, y, w, y);

		SDL_SetRenderDrawColor(ren, 230, 230, 230, 255);
		for (int i = 0; i < 4; ++i) {
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

		SDL_RenderPresent(ren);
	}

	reader.close();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
