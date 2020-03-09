/**********************************************************************************************
*
*   Ngine - A 2D game engine.
*
*   Copyright 2020 NerdThings (Reece Mackie)
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
**********************************************************************************************/

#include <ngine/Ngine.hpp>

using namespace ngine;
using namespace ngine::audio;
using namespace ngine::components;
using namespace ngine::filesystem;
using namespace ngine::graphics;
using namespace ngine::input;
using namespace ngine::physics;

class TestEntity : public Entity {
    PhysicsFixture *m_fixture = nullptr;
public:
    TestEntity() {}

    void initialize(Scene *scene) override {
        // Call to base first.
        Entity::initialize(scene);

        // If physics is enabled, create body
        if (scene->isPhysicsEnabled()) {
            // Create body
            auto world = scene->getPhysicsWorld();
            auto body = world->createBody(PhysicsBody::Type::Dynamic, {0, 0});

            // Add a fixture
            //CirclePhysicsShape shape(scene->getPhysicsContext(), 32);
            //m_fixture = body->createFixture(&shape, 1);
            PolygonPhysicsShape shape(scene->getPhysicsContext(), 32, 32);
            m_fixture = body->createFixture(&shape, 1);

            // Set body
            setPhysicsBody(body);
        }

        // Set defaults
        setPosition({50, 0});
        setRotation(0);
        setScale({2, 2});

        // Set name
        setName("TestEntity");

        // Add component.
        auto sprite = new Sprite(getResourceManager()->getTexture("test_spritesheet"), 16, 16, 32, 32);
        addComponent("Sprite", new SpriteComponent(sprite));
    }

    void update() override {
        Entity::update();

        auto keyboard = getKeyboard();
        Vector2 vel;

        auto ms = 5;

        if (keyboard->isKeyDown('w')) {
            vel.Y += ms;
        }

        if (keyboard->isKeyDown('s')) {
            vel.Y -= ms;
        }

        if (keyboard->isKeyDown('a')) {
            vel.X -= ms;
        }

        if (keyboard->isKeyDown('d')) {
            vel.X += ms;
        }

        auto rot = getRotation();

        if (keyboard->isKeyDown(Key::Left)) {
            rot = rot - 5;
        }

        if (keyboard->isKeyDown(Key::Right)) {
            rot = rot + 5;
        }

        auto scale = getScale();

        if (keyboard->isKeyPressed(Key::Up)) {
            scale.X += 0.25;
            scale.Y += 0.25;
        }

        if (keyboard->isKeyPressed(Key::Down)) {
            scale.X -= 0.25;
            scale.Y -= 0.25;
        }

        // Move
//        if (getScene()->isPhysicsEnabled()) {
//            // * 60 because normally we're doing this once a frame but velocities are once per second
//            getPhysicsBody()->setLinearVelocity(vel * 60);
//        } else {
            auto pos = getPosition();
            setPosition(pos + vel);
//        }

        setRotation(rot);
        //setScale(scale);

        if (keyboard->isKeyPressed(' ')) {
            setPosition({0, 0});
        }

        if (keyboard->isKeyPressed('0')) {
            setRotation(0);
        }
    }
};

class ParentEntity : public Entity {
public:
    void initialize(Scene *scene) override {
        Entity::initialize(scene);
        setPosition({150, 150});
        setRotation(45);
    }

    void draw(Renderer *renderer, Matrix modelView) override {
        Entity::draw(renderer, modelView);
        ShapeRenderer::drawCircle(renderer, {0, 0}, 15, Color::Blue);
    }
};

// TESTING
class NewTestScene : public Scene {
public:
    NewTestScene() : Scene({0, 0.0f}, 16) {}

    void initialize(Game *game) override {
        // Always call base first so that m_game is set!
        Scene::initialize(game);
//        auto par = new ParentEntity();
//        par->addChild(new TestEntity());
//        addChild(par);
        addChild(new TestEntity());
        getPhysicsWorld()->enableDebugDraw();
        getPhysicsWorld()->setDebugDrawFlags(DRAW_SHAPE | DRAW_AABB | DRAW_CENTER_OF_MASS);
    }

    void cleanup() override {
        // Slightly big todo, enable removal of entities easily.
    }

    void renderScene(graphics::Renderer *renderer) override {
        Scene::renderScene(renderer);

        // Setup coordinate mode
        renderer->setCoordinateSystem(CoordinateSystem::Screen);

        // Render debug stuffs
        // ShapeRenderer::DrawTriangle(renderer_, {40, 40}, {90, 90}, {40, 90}, Color::Orange, 0, {});
        auto viewport = getGame()->getGameViewport();
        ShapeRenderer::drawRectangle(renderer,
                                     {viewport.Width / 2.0f, viewport.Height / 2.0f, viewport.Width, viewport.Height},
                                     Color::Blue, 0, {0.5f, 0.5f}, true, 5);

        ShapeRenderer::drawCircle(renderer, {150, 150}, 2, Color::Red);

        getResourceManager()->getFont("Upheaval")->drawString(renderer, "Hello world.\nHow be you?", {150, 150}, 36, 2, Color::Orange);
    }
};

class TestGame : public Game {
    NewTestScene *m_scene;
public:

    TestGame(const WindowConfig &windowConfig_, const GameConfig &config_) : Game(windowConfig_, config_) {}

    void initialize() override {
        // Hook events
        EventDispatcher::bind<Game::SuspendEvent>(std::bind(&TestGame::Save, this, std::placeholders::_1), this);
        EventDispatcher::bind<Game::ResumeEvent>(std::bind(&TestGame::Load, this, std::placeholders::_1), this);

        EventDispatcher::bind<Mouse::ButtonPressedEvent>([] (const Mouse::ButtonPressedEvent &e) {
            if (e.Button == MouseButton::Left) {
                Console::notice("DEBUG", "Left mouse button pressed.");
            } else {
                Console::notice("DEBUG", "The left didnt do it!");
            }
        }, getMouse());

        EventDispatcher::bind<Keyboard::KeyPressEvent>([] (const Keyboard::KeyPressEvent &e) {
            Console::notice("DEBUG", "A key was pressed.");

            if (e.Key == Key::Backspace && e.IsControl) {
                Console::notice("DEBUG", "Howdy!");
            }
        }, getKeyboard());

        // Set exit key
        getGameWindow()->setExitKey(Key::Escape);

        // Load all content (using default resource manager config).
        auto resMan = getResourceManager();
        resMan->loadResources();

        // Load arial as default font
        Font::setDefaultFont(resMan->getFont("Upheaval"));

        // Create and enable scene.
        m_scene = new NewTestScene();
        setScene(m_scene);
    }

    void Save(const Game::SuspendEvent &e) {
        Console::notice("TestGame", "The game would have saved here.");
    }

    void Load(const Game::ResumeEvent &e) {
        Console::notice("TestGame", "The game would have loaded here.");
    }
};

NGINE_GAME_ENTRY {
    // Load icon (must remain in scope until NGINE_RUN_GAME happens)
    graphics::Image icon(Path::getExecutableDirectory() / "content" / "test_icon.png");

    GameConfig gameConfig;
    gameConfig.TargetWidth = 1280;
    gameConfig.TargetHeight = 768;
    gameConfig.RunWhileHidden = false; // For testing suspension.
    gameConfig.FPSCap = 60;
    gameConfig.MaintainResolution = true;

    WindowConfig windowConfig;
    windowConfig.Resizable = true;
    windowConfig.MSAA_4X = true;
    windowConfig.VSync = true;
    windowConfig.InitialWidth = 1920 / 2;
    windowConfig.InitialHeight = 1080 / 2;
    windowConfig.Title = "Test Game";
    windowConfig.Icon = &icon;
    //windowConfig.NativeDebugConsole = true;

#if defined(_WIN32) && defined(PLATFORM_DESKTOP)
    windowConfig.TargetAPI = GraphicsAPI::OpenGL;
    windowConfig.TargetAPIMajorVersion = 4;
    windowConfig.TargetAPIMinorVersion = 6;
#endif

    // Create game
    auto game = TestGame(windowConfig, gameConfig);

    // Run game
    NGINE_RUN_GAME(game);

    return 0;
}