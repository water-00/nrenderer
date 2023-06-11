#pragma once
#ifndef __NR_UI_CONTEXT_HPP__
#define __NR_UI_CONTEXT_HPP__

#include "geometry/vec.hpp"

namespace NRenderer
{
    struct UIContext
    {
        enum class State
        {
            HOVER_COMPONENT_PROGRESS,
            NORMAL
        };
        State state;

        enum class PreviewMode {
            PREVIEW_NONE,
            PREVIEW_MODEL,
            PREVIEW_NODE,
            PREVIEW_LIGHT
        };

        PreviewMode previewMode;

        Index previewModel;
        Index previewNode;
        Index previewLight;

        UIContext()
            : state (State::NORMAL)
            , previewNode(-1)
            , previewLight(-1)
            , previewModel(-1)
            , previewMode(PreviewMode::PREVIEW_NONE)
        {}
        inline
        State getState() const {
            return state;
        }
    };
} // namespace NRenderer


#endif