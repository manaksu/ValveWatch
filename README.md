# League Time

A minimal Pebble watchface for the Pebble Time Steel (Basalt) using League Gothic.

## Layout

- `HH:MM` in League Gothic 60px, anchored at `(4, 15)`, top-left
- `am`/`pm` superscript in League Gothic 18px, dynamically positioned after the time string

## CloudPebble Setup

1. Create a new project → Platform: Basalt, Type: Watchface
2. Replace the generated `package.json` with the one in this repo
3. Upload `resources/fonts/LeagueGothic-Regular-VariableFont_wdth.ttf` as a font resource
4. Replace `src/main.c` with the one in this repo
5. Build & install

## Structure

```
league-time/
├── src/
│   └── main.c
├── resources/
│   └── fonts/
│       └── LeagueGothic-Regular-VariableFont_wdth.ttf
├── package.json
└── .gitignore
```
