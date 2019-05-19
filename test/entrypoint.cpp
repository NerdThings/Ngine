#include "ngine.h"

#include "Core/Game.h"
#include "Input/Mouse.h"

using namespace NGINE_NS;
using namespace NGINE_NS::Core;
using namespace NGINE_NS::Graphics;
using namespace NGINE_NS::Input;

class TestGame : public Game {
    TFont def;
    TTexture2D tex;
public:
    TestGame(int width_, int height_, int DrawFPS_, int UpdateFPS_, std::string title_) : Game(
        width_, height_, DrawFPS_, UpdateFPS_, title_, MSAA_4X) {
        // Only works on windows for obvious reasons, this is just for me to test while adding the abstraction layer
        def = TFont::LoadFont("C:\\windows\\Fonts\\Arial.ttf");
        //def = TFont::GetDefaultFont();

        tex = TTexture2D::LoadTexture("A:\\Pictures\\Video Projects\\Rev3_399.png");

        OnDraw.Bind(this, &TestGame::Draw);
    }

    void Draw(EventArgs &e_) {
        Drawing::DrawTriangle({250, 0}, {100, 200}, {400, 200}, TColor::Yellow);

        Drawing::DrawText(def, "Hello World", {10, 10}, 48, 2, TColor::White);

        Drawing::DrawFPS({10, 100});

        auto pos = Mouse::GetMousePosition();

        auto color = Mouse::IsButtonDown(LEFT);

        Drawing::DrawTexture(tex, pos, 300, 200, color ? TColor::Orange : TColor::White);
    }
};

#ifdef _WIN32
#include <Windows.h>
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
#else
int main() {
#endif
    auto game = new TestGame(800, 600, 120, 60, "Hi");
    game->Run();
    delete game;
    return 0;
}