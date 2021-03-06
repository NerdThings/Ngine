/**********************************************************************************************
*
*   Ngine - The 2D game engine.
*
*   Copyright (C) 2019 NerdThings
*
*   LICENSE: Apache License 2.0
*   View: https://github.com/NerdThings/Ngine/blob/master/LICENSE
*
**********************************************************************************************/

#include "TilesetRenderer.h"

#include <cmath>

namespace NerdThings::Ngine::Graphics {
    // Public Constructor(s)

    TilesetRenderer::TilesetRenderer(const Tileset &tileset_, int width_, int height_)
            : _Tileset(tileset_), _Width(width_), _Height(height_),
              _Tiles(width_ * height_) {
    }

    TilesetRenderer::TilesetRenderer(const Tileset &tileset_, int width_, int height_, std::vector<int> tiles_)
            : _Tileset(tileset_), _Width(width_), _Height(height_) {
        if (tiles_.size() != width_ * height_) {
            throw std::runtime_error("Tile data does not match dimensions.");
        }
        _Tiles = tiles_;
    }

    // Public Methods

    void TilesetRenderer::Draw(Vector2 pos_, float scale_, float rotation_, Vector2 origin_) {
        auto tileWidth = _Tileset.GetTileWidth();
        auto tileHeight = _Tileset.GetTileHeight();

        for (auto x = 0; x < _Width; x++) {
            for (auto y = 0; y < _Height; y++) {
                // Get tile position
                Vector2 pos = {(pos_.X + x * tileWidth) * scale_, (pos_.Y + y * tileHeight) * scale_};

                // Don't know why this needs doubled, but sure
                auto tilePos = pos_ - Vector2(-origin_.X * tileWidth * 2, -origin_.Y * tileHeight * 2);
                Vector2 tileOrigin = {-pos.X, -pos.Y};

                // Cheaty: Putting the wrong values in each argument for this to work
                _Tileset.DrawTile(tilePos, GetTileAt({(float)x, (float)y}), scale_, rotation_, tileOrigin);
            }
        }
    }

    void TilesetRenderer::Draw(Vector2 pos_, Vector2 renderFrom_, Vector2 renderTo_, float scale_) {
        // TODO: How do we rotate this stuff??

        // Get tile sizes
        auto tileWidth = _Tileset.GetTileWidth();
        auto tileHeight = _Tileset.GetTileHeight();

        // Tileset size
        auto tilesetWidth = _Width * scale_;
        auto tilesetHeight = _Height * scale_;

        // Correct coordinates
        if (renderFrom_.X < pos_.X) renderFrom_.X = pos_.X;
        if (renderFrom_.Y < pos_.Y) renderFrom_.Y = pos_.Y;
        if (renderFrom_.X > pos_.X + tilesetWidth * tileWidth) renderFrom_.X = pos_.X  + tilesetWidth * tileWidth;
        if (renderFrom_.Y > pos_.Y + tilesetHeight * tileHeight) renderFrom_.Y = pos_.Y + tilesetHeight * tileHeight;

        if (renderTo_.X < pos_.X) renderTo_.X = pos_.X;
        if (renderTo_.Y < pos_.Y) renderTo_.Y = pos_.Y;
        if (renderTo_.X > pos_.X + tilesetWidth * tileWidth) renderTo_.X = pos_.X + tilesetWidth * tileWidth;
        if (renderTo_.Y > pos_.Y + tilesetHeight * tileHeight) renderTo_.Y = pos_.Y + tilesetHeight * tileHeight;

        // Get coordinates
        auto tXFrom = (int)floorf((renderFrom_.X - pos_.X) / tileWidth / scale_);
        auto tYFrom = (int)floorf((renderFrom_.Y - pos_.Y) / tileHeight / scale_);
        auto tXTo = (int)ceilf((renderTo_.X - pos_.X) / tileWidth / scale_);
        auto tYTo = (int)ceilf((renderTo_.Y - pos_.Y) / tileHeight / scale_);

        for (auto x = tXFrom; x < tXTo; x++) {
            for (auto y = tYFrom; y < tYTo; y++) {
                // Get tile position
                Vector2 pos = {(pos_.X + x * tileWidth) * scale_, (pos_.Y + y * tileHeight) * scale_};
                _Tileset.DrawTile(pos, GetTileAt({(float)x, (float)y}), scale_);
            }
        }
    }

    std::vector<Physics::ICollisionShape *>
    TilesetRenderer::GetCollisionShapesFor(int tile_, Rectangle range_, Vector2 tilesetPosition_) {
        std::vector<Physics::ICollisionShape *> shapes;

        int sX = range_.X;
        int sY = range_.Y;
        int eX = range_.X + range_.Width;
        int eY = range_.Y + range_.Height;

        // Validate
        auto w = GetWidth();
        auto h = GetHeight();

        if (sX < 0) sX = 0;
        if (sY < 0) sY = 0;
        if (sX >= w) sX = w - 1;
        if (sY >= h) sX = h - 1;

        if (eX < 0) eX = 0;
        if (eY < 0) eY = 0;
        if (eX >= w) eX = w - 1;
        if (eY >= h) eY = h - 1;

        for (auto x = sX; x <= eX; x++) {
            for (auto y = sY; y <= eY; y++) {
                if (GetTileAt({(float) x, (float) y}) == tile_) {
                    // Get polygon with world coordinates
                    auto poly = Rectangle(x * _Tileset.GetTileWidth() + tilesetPosition_.X,
                                                 y * _Tileset.GetTileWidth() + tilesetPosition_.Y,
                                          _Tileset.GetTileWidth(), _Tileset.GetTileHeight()).ToPolygonPtr();
                    shapes.push_back(dynamic_cast<Physics::ICollisionShape *>(poly));
                }
            }
        }

        return shapes;
    }

    std::vector<Physics::ICollisionShape *>
    TilesetRenderer::GetCollisionShapesFor(std::vector<int> tiles_, Rectangle range_,
                                           Vector2 tilesetPosition_) {
        std::vector<Physics::ICollisionShape *> shapes;

        int sX = range_.X;
        int sY = range_.Y;
        int eX = range_.X + range_.Width;
        int eY = range_.Y + range_.Height;

        // Validate
        auto w = GetWidth() / _Tileset.GetTileWidth();
        auto h = GetHeight() / _Tileset.GetTileHeight();

        if (sX < 0) sX = 0;
        if (sY < 0) sY = 0;
        if (sX >= w) sX = w - 1;
        if (sY >= h) sX = h - 1;

        if (eX < 0) eX = 0;
        if (eY < 0) eY = 0;
        if (eX >= w) eX = w - 1;
        if (eY >= h) eY = h - 1;

        for (auto x = sX; x <= eX; x++) {
            for (auto y = sY; y <= eY; y++) {
                if (std::find(tiles_.begin(), tiles_.end(), GetTileAt({(float) x, (float) y})) != tiles_.end()) {
                    // Get polygon with world coordinates
                    auto poly = Rectangle(x * _Tileset.GetTileWidth() + tilesetPosition_.X,
                                                 y * _Tileset.GetTileWidth() + tilesetPosition_.Y,
                                          _Tileset.GetTileWidth(), _Tileset.GetTileHeight()).ToPolygonPtr();
                    shapes.push_back(dynamic_cast<Physics::ICollisionShape *>(poly));
                }
            }
        }

        return shapes;
    }

    std::vector<Physics::ICollisionShape *>
    TilesetRenderer::GetCollisionShapesFor(int min_, int max_, Rectangle range_, Vector2 tilesetPosition_) {
        std::vector<Physics::ICollisionShape *> shapes;

        int sX = range_.X;
        int sY = range_.Y;
        int eX = range_.X + range_.Width;
        int eY = range_.Y + range_.Height;

        // Validate
        auto w = GetWidth() / _Tileset.GetTileWidth();
        auto h = GetHeight() / _Tileset.GetTileHeight();

        if (sX < 0) sX = 0;
        if (sY < 0) sY = 0;
        if (sX >= w) sX = w - 1;
        if (sY >= h) sX = h - 1;

        if (eX < 0) eX = 0;
        if (eY < 0) eY = 0;
        if (eX >= w) eX = w - 1;
        if (eY >= h) eY = h - 1;

        for (auto x = sX; x <= eX; x++) {
            for (auto y = sY; y <= eY; y++) {
                auto t = GetTileAt({(float) x, (float) y});
                if (t >= min_ && t <= max_) {
                    // Get polygon with world coordinates
                    auto poly = Rectangle(x * _Tileset.GetTileWidth() + tilesetPosition_.X,
                                                 y * _Tileset.GetTileWidth() + tilesetPosition_.Y,
                                          _Tileset.GetTileWidth(), _Tileset.GetTileHeight()).ToPolygonPtr();
                    shapes.push_back(dynamic_cast<Physics::ICollisionShape *>(poly));
                }
            }
        }

        return shapes;
    }

    int TilesetRenderer::GetHeight() {
        return _Height;
    }

    int TilesetRenderer::GetTileAt(Vector2 pos_) {
        auto w = GetWidth();
        auto h = GetHeight();

        auto i = static_cast<int>(pos_.X) + w * static_cast<int>(pos_.Y);

        return _Tiles[i];
    }

    Tileset *TilesetRenderer::GetTileset() {
        return &_Tileset;
    }

    int TilesetRenderer::GetWidth() {
        return _Width;
    }

    void TilesetRenderer::SetTileAt(Vector2 pos_, int tile_) {
        auto w = GetWidth();
        auto h = GetHeight();

        auto i = static_cast<int>(pos_.X) + w * static_cast<int>(pos_.Y);

        _Tiles[i] = tile_;
    }

    void TilesetRenderer::SetTileData(std::vector<int> data_) {
        if (data_.size() != GetWidth() * GetHeight()) {
            throw std::runtime_error("Tile data does not match dimensions.");
        }
        _Tiles = data_;
    }
}