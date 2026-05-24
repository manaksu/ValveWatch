# Valve Watchface

Pebble Time Steel (Basalt) watchface inspired by the Valve logo.

## CloudPebble setup

1. Create a new project → **Pebble C SDK**
2. Settings → Source Control → link this repo (or import zip)
3. Add `TitlingGothicNarrow-Medium.ttf` under **Resources → fonts**
4. Compile & install

## Layout

```
┌─────────────────────────────┐  black screen
│ ┌───────────────────────┐   │
│ │ 07:26 PM  [====  ]    │   │  red rect, x=4 y=15 w=89 h=25
│ │ 24 MAY 26  SUN        │   │
│ └───────────────────────┘   │
│                   ╔══════╗  │
│                   ║VALV E║  │  watermark peeking from bottom-right
└───────────────────╚══════╝──┘
```

## Files

```
valve-watchface/
├── appinfo.json       ← CloudPebble manifest + font resources
├── src/
│   └── main.c         ← full watchface C code
└── resources/
    └── fonts/
        └── README.md  ← font instructions
```

## Font

Titling Gothic Narrow Medium — closest commercial match to the Valve logo.
Free substitute: Barlow Condensed Bold (rename the .ttf file).

---
Manaksu
