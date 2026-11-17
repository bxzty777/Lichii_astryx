// SPDX-FileCopyrightText: Copyright 2026 Lichii Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QFont>

#include "lichii/game/card_view.h"

namespace {
QColor Or(const QColor& themed, const QColor& fallback) {
    return themed.isValid() ? themed : fallback;
}

int Or(int themed, int fallback) {
    return themed >= 0 ? themed : fallback;
}

// These fallbacks reproduce the card look from before the properties existed.
constexpr int DefaultRadius = 10;
constexpr int DefaultBorderWidth = 1;
} // namespace

GameCardView::GameCardView(QWidget* parent) : QListView{parent} {}

GameCardView::Themed GameCardView::Snapshot() const {
    return {card_radius,          cover_radius,           card_border_width,
            card_selected_border_width, card_title_pixel_size, card_title_weight,
            card_background,      card_hover_background,  card_selected_background,
            card_border,          card_hover_border,      card_selected_border,
            card_title_color,     card_selected_title_color};
}

void GameCardView::ResetThemedProperties() {
    const Themed unset{};
    card_radius = unset.card_radius;
    cover_radius = unset.cover_radius;
    card_border_width = unset.card_border_width;
    card_selected_border_width = unset.card_selected_border_width;
    card_title_pixel_size = unset.card_title_pixel_size;
    card_title_weight = unset.card_title_weight;
    card_background = unset.card_background;
    card_hover_background = unset.card_hover_background;
    card_selected_background = unset.card_selected_background;
    card_border = unset.card_border;
    card_hover_border = unset.card_hover_border;
    card_selected_border = unset.card_selected_border;
    card_title_color = unset.card_title_color;
    card_selected_title_color = unset.card_selected_title_color;
}

void GameCardView::ResetAllThemedProperties() {
    for (QWidget* widget : QApplication::allWidgets()) {
        if (auto* view = qobject_cast<GameCardView*>(widget)) {
            view->ResetThemedProperties();
        }
    }
}

GameCardView::CardStyle GameCardView::ResolveCardStyle(const GameCardView* view,
                                                       const QPalette& palette, bool selected,
                                                       bool hovered) {
    const Themed t = view ? view->Snapshot() : Themed{};

    CardStyle style{};
    style.radius = Or(t.card_radius, DefaultRadius);
    style.cover_radius = Or(t.cover_radius, style.radius);
    style.title_pixel_size = t.card_title_pixel_size;
    style.title_weight = Or(t.card_title_weight, static_cast<int>(QFont::Bold));

    const int border_width = Or(t.card_border_width, DefaultBorderWidth);
    const QColor base_background = Or(t.card_background, palette.window().color());
    const QColor base_border = Or(t.card_border, palette.dark().color());
    const QColor base_title = Or(t.card_title_color, palette.text().color());

    if (selected) {
        style.background = Or(t.card_selected_background, palette.highlight().color());
        style.border = Or(t.card_selected_border, palette.highlight().color().lighter(150));
        style.border_width = Or(t.card_selected_border_width, border_width);
        style.title = Or(t.card_selected_title_color,
                         Or(t.card_title_color, palette.highlightedText().color()));
    } else if (hovered) {
        style.background = Or(t.card_hover_background, base_background.lighter(120));
        style.border = Or(t.card_hover_border, base_border);
        style.border_width = border_width;
        style.title = base_title;
    } else {
        style.background = base_background;
        style.border = base_border;
        style.border_width = border_width;
        style.title = base_title;
    }
    return style;
}
