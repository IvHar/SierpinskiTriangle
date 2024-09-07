#include <utility>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <thread>
#include <mutex>

using namespace std;
using namespace sf;

const int height = 1080;
const int width = 1080;
const int depth = 9;

vector<ConvexShape> triangles;
Vector2f a(width/2, 100), b(100, height-100), c(width-100, height-100);
mutex trMutex;

void createTriangle(Vector2f a, Vector2f b, Vector2f c, int depth) {
	if (depth > 0) {
		sleep(milliseconds(5));

		ConvexShape triangle;
		triangle.setPointCount(3);
		triangle.setPoint(0, a);
		triangle.setPoint(1, b);
		triangle.setPoint(2, c);

		int colorValue = 255 - (255 / depth * (depth - 1));
		triangle.setFillColor(Color(colorValue, colorValue, colorValue));

		{
			lock_guard<mutex> guard(trMutex);
			triangles.push_back(triangle);
		}

		Vector2f ab((a.x+b.x)/2, (a.y+b.y)/2);
		Vector2f bc((b.x+c.x)/2, (b.y+c.y)/2);
		Vector2f ac((c.x+a.x)/2, (c.y+a.y)/2);

		createTriangle(a, ab, ac, depth-1);
		createTriangle(ab, b, bc, depth-1);
		createTriangle(ac, bc, c, depth-1);

	}
}

int main() {
	RenderWindow window(VideoMode(width, height), "Sierpinski triangle");
	thread drawThread([&]() {
		createTriangle(a, b, c, depth);
	});

	while (window.isOpen()) {
		Event e;
		while (window.pollEvent(e))
			if (e.type == Event::Closed) window.close();
		{
			lock_guard<mutex> guard(trMutex);
			for (auto& t : triangles)
				window.draw(t);
		}
		window.display();

	}
	if (drawThread.joinable()) drawThread.join();
	return 0;
}