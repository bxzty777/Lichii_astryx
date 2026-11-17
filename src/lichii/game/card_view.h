// SPDX-FileCopyrightText: Copyright 2026 Lichii Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QListView>

/**
 * Shared base for the list views that draw games as cards (grid and carousel).
 *
 * The card look is exposed as properties so a theme can set it from its style.qss, e.g.
 *
 *     GameCardView {
 *         qproperty-cardRadius: 12;
 *         qproperty-cardHoverBackground: #2a2c31;
 *     }
 *
 * Anything a theme leaves unset falls back to the view's palette, which keeps the look of
 * themes that don't know about these properties unchanged.
 */
class GameCardView : public QListView {
    Q_OBJECT

    Q_PROPERTY(int cardRadius MEMBER card_radius)
    Q_PROPERTY(int coverRadius MEMBER cover_radius)
    Q_PROPERTY(int cardBorderWidth MEMBER card_border_width)
    Q_PROPERTY(int cardSelectedBorderWidth MEMBER card_selected_border_width)
    Q_PROPERTY(int cardTitlePixelSize MEMBER card_title_pixel_size)
    Q_PROPERTY(int cardTitleWeight MEMBER card_title_weight)
    Q_PROPERTY(QColor cardBackground MEMBER card_background)
    Q_PROPERTY(QColor cardHoverBackground MEMBER card_hover_background)
    Q_PROPERTY(QColor cardSelectedBackground MEMBER card_selected_background)
    Q_PROPERTY(QColor cardBorder MEMBER card_border)
    Q_PROPERTY(QColor cardHoverBorder MEMBER card_hover_border)
    Q_PROPERTY(QColor cardSelectedBorder MEMBER card_selected_border)
    Q_PROPERTY(QColor cardTitleColor MEMBER card_title_color)
    Q_PROPERTY(QColor cardSelectedTitleColor MEMBER card_selected_title_color)

public:
    explicit GameCardView(QWidget* parent = nullptr);

    /// The resolved look of one card, with every palette fallback already applied.
    struct CardStyle {
        int radius;
        int cover_radius;
        int border_width;
        QColor background;
        QColor border;
        QColor title;
        int title_pixel_size; ///< 0 means "derive from the cover size"
        int title_weight;
    };

    /// `view` may be null (a card drawn outside a card view), which gives the palette-only look.
    static CardStyle ResolveCardStyle(const GameCardView* view, const QPalette& palette,
                                      bool selected, bool hovered);

    /// Style sheets never take back a qproperty they set, so clear what the previous theme set
    /// on every card view before a new style sheet is applied.
    static void ResetAllThemedProperties();

private:
    /// What a theme set: -1 / 0 / invalid colors mean "not set".
    struct Themed {
        int card_radius = -1;
        int cover_radius = -1;
        int card_border_width = -1;
        int card_selected_border_width = -1;
        int card_title_pixel_size = 0;
        int card_title_weight = -1;
        QColor card_background;
        QColor card_hover_background;
        QColor card_selected_background;
        QColor card_border;
        QColor card_hover_border;
        QColor card_selected_border;
        QColor card_title_color;
        QColor card_selected_title_color;
    };

    Themed Snapshot() const;
    void ResetThemedProperties();

    // Q_PROPERTY(MEMBER) needs plain members, so the Themed fields are mirrored here.
    int card_radius = -1;
    int cover_radius = -1;
    int card_border_width = -1;
    int card_selected_border_width = -1;
    int card_title_pixel_size = 0;
    int card_title_weight = -1;
    QColor card_background;
    QColor card_hover_background;
    QColor card_selected_background;
    QColor card_border;
    QColor card_hover_border;
    QColor card_selected_border;
    QColor card_title_color;
    QColor card_selected_title_color;
};
