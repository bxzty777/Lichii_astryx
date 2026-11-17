// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: 2016 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QSettings>
#include "common/fs/fs.h"
#include "common/fs/path_util.h"
#include "qt_common/config/uisettings.h"

#ifndef CANNOT_EXPLICITLY_INSTANTIATE
namespace Settings {
template class Setting<bool>;
template class Setting<std::string>;
template class Setting<u16, true>;
template class Setting<u32>;
template class Setting<u8, true>;
template class Setting<u8>;
template class Setting<unsigned long long>;
} // namespace Settings
#endif

namespace FS = Common::FS;

namespace UISettings {

// This shouldn't have anything except static initializers (no functions). So
// QKeySequence(...).toString() is NOT ALLOWED HERE.
// This must be in alphabetical order according to action name as it must have the same order as
// UISetting::values.shortcuts, which is alphabetically ordered.
// clang-format off
const std::array<Shortcut, 34> default_hotkeys{{
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Audio Mute/Unmute")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+M"),  std::string("Home+Dpad_Right"), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Audio Volume Down")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("-"),       std::string("Home+Dpad_Down"), Qt::ApplicationShortcut, true}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Audio Volume Up")).toStdString(),            QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("="),       std::string("Home+Dpad_Up"), Qt::ApplicationShortcut, true}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Capture Screenshot")).toStdString(),         QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+P"),  std::string("Screenshot"), Qt::WidgetWithChildrenShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Change Adapting Filter")).toStdString(),     QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F8"),      std::string("Home+L"), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Change Docked Mode")).toStdString(),         QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F10"),     std::string("Home+X"), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Change GPU Mode")).toStdString(),            QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F9"),      std::string("Home+R"), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Configure")).toStdString(),                  QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+,"),  std::string(""), Qt::WidgetWithChildrenShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Configure Current Game")).toStdString(),     QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+."),  std::string(""), Qt::WidgetWithChildrenShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Continue/Pause Emulation")).toStdString(),   QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F4"),      std::string("Home+Plus"), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Exit Fullscreen")).toStdString(),            QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Esc"),     std::string(""), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Exit Eglantine")).toStdString(),             QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+Q"),  std::string("Home+Minus"), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Fullscreen")).toStdString(),                 QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F11"),     std::string("Home+B"), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Load File")).toStdString(),                  QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+O"),  std::string(""), Qt::WidgetWithChildrenShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Load/Remove Amiibo")).toStdString(),         QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F2"),      std::string(""), Qt::WidgetWithChildrenShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Browse Public Game Lobby")).toStdString(),   QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+B"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Create Room")).toStdString(),                QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+N"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Direct Connect to Room")).toStdString(),     QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+C"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Leave Room")).toStdString(),                 QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+L"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Show Current Room")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+R"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Report a Bug")).toStdString(),               QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+F12"), std::string(""), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Restart Emulation")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F6"),      std::string("R+Plus+Minus"), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Stop Emulation")).toStdString(),             QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("F5"),      std::string("L+Plus+Minus"), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "TAS Record")).toStdString(),                 QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+F7"), std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "TAS Reset")).toStdString(),                  QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+F6"), std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "TAS Start/Stop")).toStdString(),             QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+F5"), std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Filter Bar")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+F"),  std::string(""), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Framerate Limit")).toStdString(),     QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+U"),  std::string("Home+Y"), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Turbo Speed")).toStdString(),         QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+Z"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Slow Speed")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+X"),  std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Mouse Panning")).toStdString(),       QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+F9"), std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Renderdoc Capture")).toStdString(),   QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string(""),        std::string(""), Qt::ApplicationShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Status Bar")).toStdString(),          QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+S"),  std::string(""), Qt::WindowShortcut, false}},
    {QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Toggle Performance Overlay")).toStdString(), QStringLiteral(QT_TRANSLATE_NOOP("Hotkeys", "Main Window")).toStdString(), {std::string("Ctrl+V"),  std::string(""), Qt::WindowShortcut, false}},
}};
// clang-format on

const Themes themes{{
    {"Default", "default"},
    {"Default Colorful", "colorful"},
    {"Dark", "qdarkstyle"},
    {"Dark Colorful", "colorful_dark"},
    {"Midnight Blue", "qdarkstyle_midnight_blue"},
    {"Midnight Blue Colorful", "colorful_midnight_blue"},
    {"Lichii Dark", "lichii"},
    {"Astryx Light", "astryx_light"},
    {"Astryx Dark", "astryx_dark"},
}};

bool IsDarkTheme() {
    const auto& theme = UISettings::values.theme;
    return theme == std::string("qdarkstyle") || theme == std::string("qdarkstyle_midnight_blue") ||
           theme == std::string("colorful_dark") ||
           theme == std::string("colorful_midnight_blue") || theme == std::string("lichii") ||
           theme == std::string("astryx_dark");
}

bool IsLichiiStyleTheme() {
    const auto& theme = UISettings::values.theme;
    return theme == std::string("lichii") || theme == std::string("astryx_light") ||
           theme == std::string("astryx_dark");
}

// Keep in step with the token lists at the top of each theme's style.qss.
const StyleColors& CurrentStyleColors() {
    static constexpr StyleColors lichii_dark{
        .window = 0x17181b, .surface = 0x202125, .raised = 0x232429, .hover = 0x2a2c31,
        .border = 0x34363c, .border_subtle = 0x2a2b30,
        .text = 0xececef, .strong = 0xffffff, .secondary = 0xc4c5ca, .placeholder = 0x6f7178,
        .disabled = 0x55575d,
        .accent = 0xf0386b, .accent_hover = 0xf45a85, .accent_pressed = 0xd42c5a,
        .selection = 0x3a1d28, .on_accent = 0xffffff,
        .tooltip = 0x232429, .tooltip_text = 0xececef,
    };
    // Meta's Astryx default theme tokens (github.com/facebook/astryx), as mapped by
    // tools/ui/make_astryx_themes.mjs.
    static constexpr StyleColors astryx_light{
        .window = 0xf1f4f7, .surface = 0xffffff, .raised = 0xd9e1e7, .hover = 0xcfd9e0,
        .border = 0xccd3db, .border_subtle = 0xdae1e8,
        .text = 0x0a1317, .strong = 0x0a1317, .secondary = 0x4e606f, .placeholder = 0x4e606f,
        .disabled = 0xa4b0bc,
        .accent = 0x0064e0, .accent_hover = 0x005ac9, .accent_pressed = 0x0050b3,
        .selection = 0xc1ddf8, .on_accent = 0xffffff,
        .tooltip = 0x0a1317, .tooltip_text = 0xffffff,
    };
    static constexpr StyleColors astryx_dark{
        .window = 0x111112, .surface = 0x1f1f22, .raised = 0x3a3b3c, .hover = 0x434445,
        .border = 0x494d53, .border_subtle = 0x272728,
        .text = 0xdfe2e5, .strong = 0xffffff, .secondary = 0xaaafb5, .placeholder = 0xaaafb5,
        .disabled = 0x6f747c,
        .accent = 0x0171e3, .accent_hover = 0x1b7fe6, .accent_pressed = 0x348de9,
        .selection = 0x0d2d4c, .on_accent = 0xffffff,
        .tooltip = 0xffffff, .tooltip_text = 0x0a1317,
    };
    const auto& theme = UISettings::values.theme;
    if (theme == std::string("astryx_light")) {
        return astryx_light;
    }
    if (theme == std::string("astryx_dark")) {
        return astryx_dark;
    }
    return lichii_dark;
}

QString ThemeIconPath(const QString& file) {
    const auto& theme = UISettings::values.theme;
    const QString folder = theme == std::string("astryx_light") || theme == std::string("astryx_dark")
                               ? QString::fromStdString(theme)
                               : QStringLiteral("lichii");
    return QStringLiteral(":/%1/icons/%2").arg(folder, file);
}

Values values = {};

Setting<u32, true>& CurrentGameIconSize() {
    return values.game_list_mode.GetValue() == Settings::GameListMode::TreeView
               ? values.game_icon_size
               : values.game_cover_size;
}

u32 CalculateWidth(u32 height, Settings::AspectRatio ratio) {
    switch (ratio) {
    case Settings::AspectRatio::R4_3:
        return height * 4 / 3;
    case Settings::AspectRatio::R21_9:
        return height * 21 / 9;
    case Settings::AspectRatio::R16_10:
        return height * 16 / 10;
    case Settings::AspectRatio::R16_9:
    case Settings::AspectRatio::Stretch:
        // TODO: Move this function wherever appropriate to implement Stretched aspect
        break;
    }
    return height * 16 / 9;
}

void SaveWindowState() {
    const auto window_state_config_loc =
        FS::PathToUTF8String(FS::GetEdenPath(FS::EdenPath::ConfigDir) / "window_state.ini");

    void(FS::CreateParentDir(window_state_config_loc));
    QSettings config(QString::fromStdString(window_state_config_loc), QSettings::IniFormat);

    config.setValue(QStringLiteral("geometry"), values.geometry);
    config.setValue(QStringLiteral("state"), values.state);
    config.setValue(QStringLiteral("geometryRenderWindow"), values.renderwindow_geometry);
    config.setValue(QStringLiteral("gameListHeaderState"), values.gamelist_header_state);

    config.sync();
}

void RestoreWindowState(std::unique_ptr<QtConfig>& qtConfig) {
    const auto window_state_config_loc =
        FS::PathToUTF8String(FS::GetEdenPath(FS::EdenPath::ConfigDir) / "window_state.ini");

    // Migrate window state from old location
    if (!FS::Exists(window_state_config_loc) && qtConfig->Exists("UI", "UILayout\\geometry")) {
        const auto config_loc =
            FS::PathToUTF8String(FS::GetEdenPath(FS::EdenPath::ConfigDir) / "qt-config.ini");
        QSettings config(QString::fromStdString(config_loc), QSettings::IniFormat);

        config.beginGroup(QStringLiteral("UI"));
        config.beginGroup(QStringLiteral("UILayout"));
        values.geometry = config.value(QStringLiteral("geometry")).toByteArray();
        values.state = config.value(QStringLiteral("state")).toByteArray();
        values.renderwindow_geometry =
            config.value(QStringLiteral("geometryRenderWindow")).toByteArray();
        values.gamelist_header_state =
            config.value(QStringLiteral("gameListHeaderState")).toByteArray();
        config.endGroup();
        config.endGroup();
        return;
    }

    void(FS::CreateParentDir(window_state_config_loc));
    const QSettings config(QString::fromStdString(window_state_config_loc), QSettings::IniFormat);

    values.geometry = config.value(QStringLiteral("geometry")).toByteArray();
    values.state = config.value(QStringLiteral("state")).toByteArray();
    values.renderwindow_geometry =
        config.value(QStringLiteral("geometryRenderWindow")).toByteArray();
    values.gamelist_header_state =
        config.value(QStringLiteral("gameListHeaderState")).toByteArray();
}

} // namespace UISettings
