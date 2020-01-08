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

#include "UIControlSized.hpp"

#include "UIPanel.hpp"

namespace NerdThings::Ngine::UI {
    // Private Methods

    void UIControlSized::CheckConstraints() {
        // Min and max constraints
        if (_ConstraintsDefined && _Height > _MaxHeight)
            _Height = _MaxHeight;

        if (_Height < _MinHeight)
            _Height = _MinHeight;

        if ( _ConstraintsDefined && _Width > _MaxWidth)
            _Width = _MaxWidth;

        if (_Width < _MinWidth)
            _Width = _MinWidth;

        // Panel parent constraints
        auto panel = GetParent<UIPanel>();
        if (_ConstrainToPanel && panel != nullptr) {
            auto style = GetStyle();

            auto panelStyle = panel->GetStyle();
            auto rect = panelStyle.GetContentRect(panel->GetRenderRectangle());
            if (_Height > rect.Height)
                _Height = rect.Height - style.BorderThickness * 2;

            if (_Width > rect.Width)
                _Width = rect.Width - style.BorderThickness * 2;
        }
    }

    // Public Methods

    bool UIControlSized::GetConstrainToPanel() {
        return _ConstrainToPanel;
    }

    float UIControlSized::GetMaxHeight() {
        return _MaxHeight;
    }

    float UIControlSized::GetMaxWidth() {
        return _MaxWidth;
    }

    float UIControlSized::GetMinHeight() {
        return _MinHeight;
    }

    float UIControlSized::GetMinWidth() {
        return _MinWidth;
    }

    float UIControlSized::GetHeight() {
        CheckConstraints();
        return _Height;
    }

    float UIControlSized::GetWidth() {
        CheckConstraints();
        return _Width;
    }

    void UIControlSized::SetConstrainToPanel(bool constrain_) {
        _ConstrainToPanel = constrain_;
        CheckConstraints();
    }

    void UIControlSized::SetMaxHeight(float height_) {
        _MaxHeight = height_;
        _ConstraintsDefined = true;
        CheckConstraints();
    }

    void UIControlSized::SetMaxWidth(float width_) {
        _MaxWidth = width_;
        _ConstraintsDefined = true;
        CheckConstraints();
    }

    void UIControlSized::SetMinHeight(float height_) {
        _MinHeight = height_;
        CheckConstraints();
    }

    void UIControlSized::SetMinWidth(float width_) {
        _MinWidth = width_;
        CheckConstraints();
    }

    void UIControlSized::SetHeight(float height_) {
        _Height = height_;
        CheckConstraints();
    }

    void UIControlSized::SetWidth(float width_) {
        _Width = width_;
        CheckConstraints();
    }
}