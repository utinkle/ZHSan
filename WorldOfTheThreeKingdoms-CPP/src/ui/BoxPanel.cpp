#include "ui/BoxPanel.h"
#include "platform/PlatformSDL.h"
#include <SDL.h>
#include "ui/FontManager.h"
#include "ui/ShadowCache.h"
#include <algorithm>
#include <cmath>

namespace WO3K {

void BoxPanel::Draw() {
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;

    // draw drop shadow first (behind panel)
    if (DrawShadow) {
        // Use ShadowCache to draw a 3x3 shadow using corner/edge/center tiles.
        auto tiles = WO3K::ShadowCache::Get(std::max(4, RoundedRadius), renderer);
        int r = tiles.radius > 0 ? tiles.radius : 4;
        // if tiles available, draw nine-slice
        if (tiles.corner && tiles.edgeH && tiles.edgeV && tiles.center) {
            SDL_Texture* tCorner = tiles.corner->GetSDLTexture();
            SDL_Texture* tEdgeH = tiles.edgeH->GetSDLTexture();
            SDL_Texture* tEdgeV = tiles.edgeV->GetSDLTexture();
            SDL_Texture* tCenter = tiles.center->GetSDLTexture();

            // corners
            SDL_Rect dstTL{ Bounds.X - r, Bounds.Y - r, r, r };
            SDL_Rect dstTR{ Bounds.X + Bounds.Width, Bounds.Y - r, r, r };
            SDL_Rect dstBL{ Bounds.X - r, Bounds.Y + Bounds.Height, r, r };
            SDL_Rect dstBR{ Bounds.X + Bounds.Width, Bounds.Y + Bounds.Height, r, r };

            SDL_RenderCopy(renderer, tCorner, nullptr, &dstTL); // top-left
            SDL_RenderCopyEx(renderer, tCorner, nullptr, &dstTR, 0, nullptr, SDL_FLIP_HORIZONTAL); // top-right
            SDL_RenderCopyEx(renderer, tCorner, nullptr, &dstBL, 0, nullptr, SDL_FLIP_VERTICAL); // bottom-left
            SDL_RenderCopyEx(renderer, tCorner, nullptr, &dstBR, 0, nullptr, (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL)); // bottom-right

            // edges: top and bottom (stretch horizontally)
            SDL_Rect dstTop{ Bounds.X, Bounds.Y - r, Bounds.Width, r };
            SDL_Rect dstBottom{ Bounds.X, Bounds.Y + Bounds.Height, Bounds.Width, r };
            SDL_RenderCopy(renderer, tEdgeH, nullptr, &dstTop);
            SDL_RenderCopyEx(renderer, tEdgeH, nullptr, &dstBottom, 0, nullptr, SDL_FLIP_VERTICAL);

            // edges: left and right (stretch vertically)
            SDL_Rect dstLeft{ Bounds.X - r, Bounds.Y, r, Bounds.Height };
            SDL_Rect dstRight{ Bounds.X + Bounds.Width, Bounds.Y, r, Bounds.Height };
            SDL_RenderCopy(renderer, tEdgeV, nullptr, &dstLeft);
            SDL_RenderCopyEx(renderer, tEdgeV, nullptr, &dstRight, 0, nullptr, SDL_FLIP_HORIZONTAL);

            // center (fill interior)
            SDL_Rect dstCenter{ Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height };
            SDL_RenderCopy(renderer, tCenter, nullptr, &dstCenter);
        } else {
            // fallback simple soft rectangle shadow
            SDL_Rect sr{ Bounds.X + ShadowOffsetX, Bounds.Y + ShadowOffsetY, Bounds.Width, Bounds.Height };
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, ShadowR, ShadowG, ShadowB, ShadowA);
            SDL_RenderFillRect(renderer, &sr);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }

    // helper: filled circle for rounded corners
    auto fill_circle = [&](int cx, int cy, int radius) {
        for (int dy = -radius; dy <= radius; ++dy) {
            int yy = cy + dy;
            int dx = static_cast<int>(std::sqrt(static_cast<double>(radius*radius - dy*dy)));
            int x1 = cx - dx;
            int x2 = cx + dx;
            SDL_RenderDrawLine(renderer, x1, yy, x2, yy);
        }
    };

    // draw rounded background (or normal rect if radius == 0)
    int radius = std::max(0, RoundedRadius);
    if (radius <= 0) {
        SDL_Rect r{ Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height };
        SDL_SetRenderDrawColor(renderer, R, G, B, A);
        SDL_RenderFillRect(renderer, &r);
    } else {
        SDL_SetRenderDrawColor(renderer, R, G, B, A);
        // center rect
        SDL_Rect central{ Bounds.X + radius, Bounds.Y, Bounds.Width - 2*radius, Bounds.Height };
        SDL_RenderFillRect(renderer, &central);
        // left and right rects
        SDL_Rect left{ Bounds.X, Bounds.Y + radius, radius, Bounds.Height - 2*radius };
        SDL_Rect right{ Bounds.X + Bounds.Width - radius, Bounds.Y + radius, radius, Bounds.Height - 2*radius };
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
        // corner circles
        fill_circle(Bounds.X + radius, Bounds.Y + radius, radius);
        fill_circle(Bounds.X + Bounds.Width - radius - 1, Bounds.Y + radius, radius);
        fill_circle(Bounds.X + radius, Bounds.Y + Bounds.Height - radius - 1, radius);
        fill_circle(Bounds.X + Bounds.Width - radius - 1, Bounds.Y + Bounds.Height - radius - 1, radius);
    }

    // title bar (if provided)
    int titleH = std::max(0, TitleHeight);
    if (!Title.empty() && titleH > 0) {
        SDL_Rect tr{ Bounds.X, Bounds.Y, Bounds.Width, titleH };
        SDL_SetRenderDrawColor(renderer, TitleR, TitleG, TitleB, TitleA);
        SDL_RenderFillRect(renderer, &tr);

        // draw title text
        auto tex = FontManager::RenderText("Content/Font/SimHei.ttf:12", Title, 220, 220, 220);
        if (tex && tex->GetSDLTexture()) {
            int tw = tex->width();
            int th = tex->height();
            SDL_Rect d{ Bounds.X + 8, Bounds.Y + (titleH - th) / 2, tw, th };
            SDL_RenderCopy(renderer, tex->GetSDLTexture(), nullptr, &d);
        }

        // draw separation line
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderDrawLine(renderer, Bounds.X, Bounds.Y + titleH - 1, Bounds.X + Bounds.Width, Bounds.Y + titleH - 1);
    }

    // border (simple rect border)
    if (DrawBorder) {
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_Rect br{ Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height };
        SDL_RenderDrawRect(renderer, &br);
    }

    // Draw children
    Panel::Draw();

    // draw interactive handle in bottom-right corner (visual only)
    if (HandleVisible) {
        SDL_Rect hr{ Bounds.X + Bounds.Width - HandleInset - HandleSize, Bounds.Y + Bounds.Height - HandleInset - HandleSize, HandleSize, HandleSize };
        SDL_SetRenderDrawColor(renderer, 100, 140, 200, 220);
        SDL_RenderFillRect(renderer, &hr);
        SDL_SetRenderDrawColor(renderer, 60, 90, 140, 255);
        SDL_RenderDrawRect(renderer, &hr);
    }
}

} // namespace WO3K
