#include "ui/FlowPanel.h"
#include "platform/PlatformSDL.h"
#include <SDL.h>
#include <algorithm>
#include <vector>

namespace WO3K {

FlowPanel::FlowPanel() {
}

void FlowPanel::Initialize() {
    Panel::Initialize();
    // initial flow layout
    ApplyFlowLayout();
}

void FlowPanel::Update(double deltaTime) {
    Panel::Update(deltaTime);
}

void FlowPanel::Draw() {
    Panel::Draw();
}

void FlowPanel::ApplyFlowLayout() {
    // Arrange children horizontally or vertically inside Bounds
    int contentX = Bounds.X + Padding;
    int contentY = Bounds.Y + Padding;
    int contentW = std::max(0, Bounds.Width - Padding * 2);
    int contentH = std::max(0, Bounds.Height - Padding * 2);

    if (FlowDirection == Direction::Horizontal) {
        // Build lines (each line is a vector of child pointers)
        std::vector<std::vector<std::shared_ptr<Control>>> lines;
        std::vector<int> lineHeights;

        int curX = 0; // offset within content
        int curLineH = 0;
        std::vector<std::shared_ptr<Control>> curLine;

        for (auto &c : children) {
            if (!c) continue;
            int w = c->Bounds.Width > 0 ? c->Bounds.Width : 80;
            int h = c->Bounds.Height > 0 ? c->Bounds.Height : (contentH > 0 ? contentH : 28);
            int itemTotalW = w + c->Margin.Left + c->Margin.Right;
            if (curLine.empty()) {
                // first item always fits on line
                curLine.push_back(c);
                curX = itemTotalW;
                curLineH = h + c->Margin.Top + c->Margin.Bottom;
            } else if (Wrap && contentW > 0 && (curX + Spacing + itemTotalW) > contentW) {
                // wrap to next line
                lines.push_back(curLine);
                lineHeights.push_back(curLineH);
                curLine.clear();
                curLine.push_back(c);
                curX = itemTotalW;
                curLineH = h + c->Margin.Top + c->Margin.Bottom;
            } else {
                // append to current line
                curX += Spacing + itemTotalW;
                curLine.push_back(c);
                curLineH = std::max(curLineH, h + c->Margin.Top + c->Margin.Bottom);
            }
        }
        if (!curLine.empty()) {
            lines.push_back(curLine);
            lineHeights.push_back(curLineH);
        }

        // Position lines with alignment
        int y = contentY;
        for (size_t li = 0; li < lines.size(); ++li) {
            auto &line = lines[li];
            int lineH = lineHeights[li];

            // compute total width of this line
            int totalW = 0;
            for (size_t i = 0; i < line.size(); ++i) {
                auto &c = line[i];
                int w = c->Bounds.Width > 0 ? c->Bounds.Width : 80;
                totalW += w + c->Margin.Left + c->Margin.Right;
            }
            if (!line.empty()) totalW += static_cast<int>(line.size() - 1) * Spacing;

            int offsetX = 0;
            if (Align == Alignment::Center && contentW > totalW) {
                offsetX = (contentW - totalW) / 2;
            } else if (Align == Alignment::End && contentW > totalW) {
                offsetX = contentW - totalW;
            }

            int x = contentX + offsetX;
            for (auto &c : line) {
                int w = c->Bounds.Width > 0 ? c->Bounds.Width : 80;
                int h = c->Bounds.Height > 0 ? c->Bounds.Height : (contentH > 0 ? contentH : 28);
                // if child is wider than content, clamp to available width
                int maxChildW = std::max(2, contentW - (c->Margin.Left + c->Margin.Right));
                if (w > maxChildW) w = maxChildW;

                c->Bounds.X = x + c->Margin.Left;
                // vertically align inside the line: center if Align==Center, end if Align==End
                int innerLineH = lineH - (c->Margin.Top + c->Margin.Bottom);
                if (innerLineH < 0) innerLineH = 0;
                if (h < innerLineH) {
                    if (Align == Alignment::Center) {
                        c->Bounds.Y = y + c->Margin.Top + (innerLineH - h) / 2;
                    } else if (Align == Alignment::End) {
                        c->Bounds.Y = y + c->Margin.Top + (innerLineH - h);
                    } else {
                        c->Bounds.Y = y + c->Margin.Top;
                    }
                } else {
                    c->Bounds.Y = y + c->Margin.Top;
                }

                c->Bounds.Width = w;
                c->Bounds.Height = h;
                x += w + c->Margin.Left + c->Margin.Right + Spacing;
            }

            y += lineH + Spacing;
        }

    } else {
        // Vertical flow: build columns when wrapping
        std::vector<std::vector<std::shared_ptr<Control>>> cols;
        std::vector<int> colWidths;

        int curY = 0;
        int curColW = 0;
        std::vector<std::shared_ptr<Control>> curCol;

        for (auto &c : children) {
            if (!c) continue;
            int w = c->Bounds.Width > 0 ? c->Bounds.Width : (contentW > 0 ? contentW : 80);
            int h = c->Bounds.Height > 0 ? c->Bounds.Height : 28;
            int itemTotalH = h + c->Margin.Top + c->Margin.Bottom;
            if (curCol.empty()) {
                curCol.push_back(c);
                curY = itemTotalH;
                curColW = w + c->Margin.Left + c->Margin.Right;
            } else if (Wrap && contentH > 0 && (curY + Spacing + itemTotalH) > contentH) {
                cols.push_back(curCol);
                colWidths.push_back(curColW);
                curCol.clear();
                curCol.push_back(c);
                curY = itemTotalH;
                curColW = w + c->Margin.Left + c->Margin.Right;
            } else {
                curY += Spacing + itemTotalH;
                curCol.push_back(c);
                curColW = std::max(curColW, w + c->Margin.Left + c->Margin.Right);
            }
        }
        if (!curCol.empty()) {
            cols.push_back(curCol);
            colWidths.push_back(curColW);
        }

        // Position columns with alignment
        int x = contentX;
        for (size_t ci = 0; ci < cols.size(); ++ci) {
            auto &col = cols[ci];
            int colW = colWidths[ci];

            // compute total height of this column
            int totalH = 0;
            for (size_t i = 0; i < col.size(); ++i) {
                auto &c = col[i];
                int h = c->Bounds.Height > 0 ? c->Bounds.Height : 28;
                totalH += h + c->Margin.Top + c->Margin.Bottom;
            }
            if (!col.empty()) totalH += static_cast<int>(col.size() - 1) * Spacing;

            int offsetY = 0;
            if (Align == Alignment::Center && contentH > totalH) {
                offsetY = (contentH - totalH) / 2;
            } else if (Align == Alignment::End && contentH > totalH) {
                offsetY = contentH - totalH;
            }

            int y = contentY + offsetY;
            for (auto &c : col) {
                int w = c->Bounds.Width > 0 ? c->Bounds.Width : (contentW > 0 ? contentW : 80);
                int h = c->Bounds.Height > 0 ? c->Bounds.Height : 28;
                // horizontal align inside column (left)
                int childX = x + c->Margin.Left;
                if (Align == Alignment::Center) {
                    childX = x + (colW - (w + c->Margin.Left + c->Margin.Right)) / 2 + c->Margin.Left;
                } else if (Align == Alignment::End) {
                    childX = x + colW - (w + c->Margin.Right);
                }
                c->Bounds.X = childX;
                c->Bounds.Y = y + c->Margin.Top;
                c->Bounds.Width = w;
                c->Bounds.Height = h;
                y += h + c->Margin.Top + c->Margin.Bottom + Spacing;
            }

            x += colW + Spacing;
        }
    }
}

} // namespace WO3K
