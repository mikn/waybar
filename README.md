# Waybar
**Proof of concept**

Highly customizable wayland bar for sway or wlroots based compositor.

![Waybar](https://raw.githubusercontent.com/alexays/waybar/master/preview-2.png)
![Waybar](https://raw.githubusercontent.com/alexays/waybar/master/preview.png)

**Current features**
- Configuration via [JSON file](./resources/config)
- Customization via [CSS file](./resources/style.css)
- Sway workspaces
- Local time
- Battery
- Network name
- Used memory percentage
- Cpu load percentage

**How to compile**

```bash
meson build
ninja -C build
./build/waybar
```

Contributions welcome! - have fun