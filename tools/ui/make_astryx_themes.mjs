// Builds the Astryx Light and Astryx Dark themes from Lichii Dark.
//
// Lichii Dark (dist/qt_themes/lichii) is the source of truth for layout: every rule of its
// stylesheet and every icon is copied, and only its colors and corner radii are swapped for
// values taken from Meta's Astryx design system (github.com/facebook/astryx, MIT,
// packages/core/src/theme/tokens.stylex.ts). Re-run this after changing Lichii Dark, then render
// the icons:
//   node tools/ui/make_astryx_themes.mjs
//   npm i @resvg/resvg-js && node tools/ui/render_lichii_icons.mjs astryx_light astryx_dark
import { readdirSync, readFileSync, writeFileSync, mkdirSync, rmSync } from "node:fs";

const themes = new URL("../../dist/qt_themes/", import.meta.url);
const lichii = new URL("lichii/", themes);

// --- Colors -------------------------------------------------------------------------------

const parse = (c) => {
    const m = /^#([0-9a-f]{6})([0-9a-f]{2})?$/i.exec(c);
    if (!m) throw new Error(`bad color ${c}`);
    const n = parseInt(m[1], 16);
    return { r: n >> 16, g: (n >> 8) & 255, b: n & 255, a: m[2] ? parseInt(m[2], 16) / 255 : 1 };
};
const hex = ({ r, g, b }) =>
    "#" + [r, g, b].map((v) => Math.round(v).toString(16).padStart(2, "0")).join("");
// Paints a translucent color over an opaque one, as the browser would.
const over = (top, under) => {
    const t = parse(top);
    const u = parse(under);
    const mix = (k) => t[k] * t.a + u[k] * (1 - t.a);
    return hex({ r: mix("r"), g: mix("g"), b: mix("b") });
};
// Astryx tints hover and pressed states with black (light) or white (dark) via color-mix.
const tint = (color, with_, amount) => over(with_ + Math.round(amount * 255).toString(16).padStart(2, "0"), color);
const rgba = (color, alpha) => {
    const { r, g, b } = parse(color);
    return `rgba(${r}, ${g}, ${b}, ${alpha})`;
};

// Astryx default theme, [light, dark] pairs of `light-dark()` in tokens.stylex.ts.
const astryx = {
    light: {
        body: "#F1F4F7", surface: "#FFFFFF", popover: "#FFFFFF", inverted: "#0A1317",
        onInverted: "#FFFFFF",
        text: "#0A1317", secondary: "#4E606F", disabled: "#A4B0BC",
        accent: "#0064E0", textAccent: "#0064E0", onAccent: "#FFFFFF", accentMuted: "#0082FB33",
        neutral: "#0536591A", overlayHover: "#0536590C", overlayPressed: "#05365919",
        border: "#05365919", borderEmphasized: "#CCD3DB", track: "#CCD3DB", tintHover: "#000000",
        borderGray: "#647685",
        chips: {
            yellow: ["#E2A400", "#753F07"], blue: ["#0171E3", "#042F97"], purple: ["#7952FF", "#3E0697"],
            cyan: ["#03A7D7", "#014975"], green: ["#24BB5E", "#09441F"], orange: ["#F27902", "#6B2203"],
            pink: ["#E638B3", "#650053"],
        },
    },
    dark: {
        body: "#111112", surface: "#1F1F22", popover: "#28292C", inverted: "#FFFFFF",
        onInverted: "#0A1317",
        text: "#DFE2E5", secondary: "#AAAFB5", disabled: "#6F747C",
        accent: "#0171E3", textAccent: "#3E9EFB", onAccent: "#FFFFFF", accentMuted: "#0082FB3F",
        neutral: "#DFE2E533", overlayHover: "#FFFFFF0C", overlayPressed: "#FFFFFF19",
        border: "#F2F4F619", borderEmphasized: "#494D53", track: "#5A5E66", tintHover: "#FFFFFF",
        borderGray: "#748695",
        chips: {
            yellow: ["#E2A400", "#FBCE03"], blue: ["#0171E3", "#AFD7FF"], purple: ["#7952FF", "#B3B0FE"],
            cyan: ["#03A7D7", "#A1EEF9"], green: ["#24BB5E", "#A5F690"], orange: ["#F27902", "#FDB876"],
            pink: ["#E638B3", "#FEADE3"],
        },
    },
};

// The roles Lichii Dark's stylesheet uses, filled from Astryx tokens.
const roles = (t, mode) => {
    const raised = over(t.neutral, t.body);
    const header = over(t.overlayHover, t.body);
    const borderSubtle = over(t.border, t.body);
    const selection = over(t.accentMuted, t.body);
    const muted = parse(t.accentMuted).a;
    return {
        window: t.body,
        chrome: t.surface,
        sunken: t.surface,
        header,
        surface: t.surface,
        popover: t.popover,
        raised,
        hover: over(t.overlayHover, raised),
        pressed: over(t.overlayPressed, raised),
        border: t.borderEmphasized,
        borderHover: mode === "light" ? t.disabled : t.track,
        borderSubtle,
        divider: borderSubtle,
        text: t.text,
        // Text on hovered and selected rows; Lichii Dark brightens it to white.
        strong: mode === "light" ? t.text : "#FFFFFF",
        secondary: t.secondary,
        disabled: t.disabled,
        accent: t.accent,
        accentHover: tint(t.accent, t.tintHover, 0.1),
        accentPressed: tint(t.accent, t.tintHover, 0.2),
        accentSoft: rgba(t.accentMuted.slice(0, 7), +muted.toFixed(2)),
        textAccent: t.textAccent,
        onAccent: t.onAccent,
        selection,
        selectionHover: over(t.accentMuted, selection),
        accentDisabled: over(rgba8(t.accent, 0.35), t.body),
        itemHover: header,
        track: t.track,
        trackHover: t.disabled,
        inverted: t.inverted,
        onInverted: t.onInverted,
        borderGray: t.borderGray,
        chips: t.chips,
        tokens: t,
    };
};
const astryxOf = (r) => r.tokens;
const rgba8 = (color, alpha) => color + Math.round(alpha * 255).toString(16).padStart(2, "0");

// --- Stylesheet ---------------------------------------------------------------------------

// Lichii Dark color → role. Colors not listed (the console-style keyboard and dialog applets)
// stay as they are in every theme.
const qssColors = {
    "#17181b": "window", "#131416": "chrome", "#141518": "sunken", "#1b1c20": "header",
    "#202125": "surface", "#232429": "raised", "#2a2c31": "hover", "#303238": "pressed",
    "#34363c": "border", "#44464d": "borderHover", "#2a2b30": "borderSubtle", "#212226": "divider",
    "#2f3137": "borderSubtle",
    "#ececef": "text", "#d6d7db": "text", "#dcdde1": "text", "#c4c5ca": "secondary",
    "#9b9da4": "secondary", "#8b8d94": "secondary", "#6f7178": "secondary", "#55575d": "disabled",
    "#6b6d73": "disabled",
    "#f0386b": "accent", "#f45a85": "accentHover", "#d42c5a": "accentPressed",
    "#3a1d28": "selection", "#2c1a21": "selection", "#3a2230": "selectionHover",
    "#5a2f3d": "accentDisabled", "#1f2024": "itemHover", "#1d1e22": "header",
    "#4a4c53": "track", "#6a6c73": "trackHover",
    // Controller settings panels, from the older dark theme.
    "#232629": "raised", "#31363b": "sunken", "#76797c": "borderGray", "#54575b": "border",
};
const chipColors = {
    "f5b942": "yellow", "60a5fa": "blue", "a78bfa": "purple", "22d3ee": "cyan", "a3d95e": "green",
    "ff8a4c": "orange", "e879f9": "pink",
};
const chipRgb = Object.fromEntries(
    Object.entries(chipColors).map(([h, name]) => {
        const { r, g, b } = parse("#" + h);
        return [`${r}, ${g}, ${b}`, name];
    }),
);
// Lichii Dark radius → Astryx radius (inner 4, element 8, container 12).
const radii = { 3: 4, 5: 4, 6: 8, 7: 8, 10: 12 };

const header = (name, r) => `/*
* SPDX-FileCopyrightText: Copyright 2026 Lichii Project
* SPDX-License-Identifier: GPL-3.0-or-later
*/

/* ${name}. Generated from Lichii Dark by tools/ui/make_astryx_themes.mjs; edit that script or
   Lichii Dark, not this file. Colors and radii are Meta's Astryx default theme tokens
   (github.com/facebook/astryx, MIT, packages/core/src/theme/tokens.stylex.ts):
   window ${r.window} (background-body)  chrome/sunken/surface ${r.surface} (background-surface)
   popover ${r.popover}  raised ${r.raised} (neutral)  hover ${r.hover}  pressed ${r.pressed}
   border ${r.border} (border-emphasized)  border-subtle ${r.borderSubtle} (border)
   text ${r.text}  secondary ${r.secondary}  disabled ${r.disabled}
   accent ${r.accent}  accent-hover ${r.accentHover}  accent-pressed ${r.accentPressed}  accent-soft ${r.accentSoft}
   selection ${r.selection}  tooltip ${r.inverted} (background-inverted)
   radius 4 / 8 / 12 (inner / element / container) */
`;

const makeQss = (name, folder, r) => {
    let qss = readFileSync(new URL("style.qss", lichii), "utf8");
    // Drop Lichii Dark's own header comment.
    qss = qss.replace(/^\/\*[\s\S]*?\*\/\s*\/\*[\s\S]*?\*\/\s*/, "");
    // The on-screen keyboard and applet dialogs mimic the console and keep their look.
    const cut = qss.indexOf("QStackedWidget#topOSK");
    let themed = qss.slice(0, cut);
    const fixed = qss.slice(cut);

    // Rule by rule, so white can be read in context.
    themed = themed.replace(/([^{}]*)\{([^{}]*)\}/g, (_, selector, body) => {
        // White text on an accent fill stays white; elsewhere it is the strongest text color.
        const onAccent = /QPushButton:default/.test(selector);
        body = body.replace(/^(\s*)([\w-]+)(\s*:\s*)([^;]*);/gm, (line, indent, prop, colon, value) => {
            value = value.replace(/#ffffff\b/gi, () => {
                if (/background/.test(prop)) return "#ffffff";
                return onAccent ? r.onAccent : r.strong;
            });
            value = value.replace(/#[0-9a-f]{6}\b/gi, (c) => {
                const key = c.toLowerCase();
                if (key === "#ffffff") return c;
                if (qssColors[key]) return r[qssColors[key]];
                if (chipColors[key.slice(1)]) return r.chips[chipColors[key.slice(1)]][1];
                return c;
            });
            value = value.replace(/rgba\(\s*(\d+),\s*(\d+),\s*(\d+),\s*([\d.]+)\)/g, (c, R, G, B, a) => {
                const rgb = `${R}, ${G}, ${B}`;
                if (rgb === "240, 56, 107") {
                    // Lichii Dark's accent-soft is 0.16; other alphas tint borders and selections.
                    return +a <= 0.16 ? r.accentSoft : rgba(r.accent, a);
                }
                if (chipRgb[rgb]) return rgba(r.chips[chipRgb[rgb]][0], 0.2);
                return c;
            });
            if (prop === "border-radius") {
                value = value.replace(/^(\d+)px$/, (m, px) => `${radii[px] ?? px}px`);
            }
            return `${indent}${prop}${colon}${value};`;
        });
        return `${selector}{${body}}`;
    });
    // Astryx tooltips are inverted: dark on light pages, light on dark ones.
    themed = themed.replace(/QToolTip \{[^}]*\}/, `QToolTip {
    color: ${r.onInverted};
    background-color: ${r.inverted};
    border: 1px solid ${r.inverted};
    border-radius: 4px;
    padding: 6px 8px;
}`);
    // Menus and dropdown lists float on the popover surface.
    themed = themed.replace(/(QMenu \{\s*background-color: )[^;]+;/, `$1${r.popover};`);
    themed = themed.replace(/(QComboBox QAbstractItemView \{\s*background-color: )[^;]+;/, `$1${r.popover};`);
    // Astryx highlights a menu row with the hover overlay, not the accent.
    themed = themed.replace(
        /(QMenu::item:selected \{\s*background-color: )[^;]+;/,
        `$1${over(astryxOf(r).overlayHover, r.popover)};`,
    );
    // A white slider knob needs an outline to show on a light page.
    if (r.strong !== "#FFFFFF") {
        themed = themed.replace(
            /(QSlider::handle:(?:horizontal|vertical) \{[^}]*background-color: #ffffff;)/g,
            `$1\n    border: 1px solid ${r.border};`,
        );
    }
    // The renderer chip is an accent chip.
    themed = themed.replace(/(QPushButton#RendererStatusBarButton \{[^}]*color: )[^;]+;/, `$1${r.textAccent};`);

    const out = header(name, r) + "\n" + themed + fixed;
    return out.replaceAll(":/lichii/icons/", `:/${folder}/icons/`);
};

// --- Icons --------------------------------------------------------------------------------

// Lichii Dark icon color → Astryx color, per role.
const svgColors = (r, t, mode) => ({
    "#f0386b": r.accent,
    "#ffffff": "#ffffff",
    "#b4b6bc": t.secondary, // menu icons (icon-secondary)
    "#9b9da4": t.secondary, // settings page icons
    "#7d7f86": t.secondary, // spin box chevrons
    "#4a4c53": t.disabled, // unchecked box and radio outline, disabled chevrons, hovered switch track
    "#6a6c73": r.borderGray, // hovered box and radio outline
    "#3a3c42": t.track, // switch track
    "#33353a": over(t.border, t.surface), // disabled outline
    "#26272b": over(t.overlayHover, mode === "light" ? t.track : t.surface), // disabled switch track
    "#6b6d73": mode === "light" ? "#FFFFFF" : t.track, // disabled switch knob
    "#3a2230": over(t.accentMuted, t.surface), // disabled checked fill
    "#5a2f3d": over(rgba8(t.accent, 0.35), t.surface),
    "#8a5f6c": over(rgba8(t.accent, 0.55), t.surface),
});

const makeTheme = (folder, name, mode) => {
    const t = astryx[mode];
    const r = roles(t, mode);
    const dir = new URL(`${folder}/`, themes);
    rmSync(new URL("svg/", dir), { recursive: true, force: true });
    mkdirSync(new URL("svg/", dir), { recursive: true });
    mkdirSync(new URL("icons/", dir), { recursive: true });

    writeFileSync(new URL("style.qss", dir), makeQss(name, folder, r));

    const map = svgColors(r, t, mode);
    const src = new URL("svg/", lichii);
    for (const file of readdirSync(src).filter((f) => f.endsWith(".svg"))) {
        const svg = readFileSync(new URL(file, src), "utf8").replace(/#[0-9a-f]{6}\b/gi, (c) => {
            const to = map[c.toLowerCase()];
            if (!to) throw new Error(`${file}: no Astryx color for ${c}`);
            return to.toLowerCase();
        });
        writeFileSync(new URL(`svg/${file}`, dir), svg);
    }

    writeFileSync(
        new URL("index.theme", dir),
        `[Icon Theme]\nName=${folder}\nComment=${name}\nInherits=${mode === "light" ? "colorful" : "colorful_dark"}\nDirectories=16x16\n\n[16x16]\nSize=16\n`,
    );

    // The icon list is filled in by render_lichii_icons.mjs.
    const qrc = readFileSync(new URL("style.qrc", lichii), "utf8")
        .replace(`prefix="icons/lichii"`, `prefix="icons/${folder}"`)
        .replace(`prefix="lichii"`, `prefix="${folder}"`)
        .replace(/<!-- The PNGs[^>]*-->/, `<!-- Generated by tools/ui/make_astryx_themes.mjs; the PNGs by tools/ui/render_lichii_icons.mjs. -->`);
    writeFileSync(new URL("style.qrc", dir), qrc);
};

makeTheme("astryx_light", "Astryx Light", "light");
makeTheme("astryx_dark", "Astryx Dark", "dark");
