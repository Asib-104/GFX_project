# ?? Quick Reference - Textured Objects Controls

## ?? New Keyboard Controls

| Key | Action |
|:---:|:-------|
| **8** | ?? Toggle Textured Objects ON/OFF |
| **9** | ?? Cycle Texture Modes (0?1?2?3?4?0) |
| **0** | ?? Reset to Show All Objects |

## ?? Texture Modes

### Mode 0: **All Objects** (Default)
Shows all 4 textured objects simultaneously

### Mode 1: **Simple Texture Only**
?? **Left Floating Sphere**
- Pure white material
- Texture shown without color modification
- Demonstrates: Raw texture mapping

### Mode 2: **Blended Texture Only**  
?? **Right Floating Sphere**
- Blue-tinted material
- Texture multiplied with surface color
- Demonstrates: Texture + color blending

### Mode 3: **Vertex Color Only**
?? **Left Stage Cone**
- Red-orange warm colors
- Gouraud shading (vertex-level lighting)
- Demonstrates: Vertex shader lighting

### Mode 4: **Fragment Color Only**
?? **Right Stage Cone**
- Green cool colors
- Phong shading (fragment-level lighting)
- Demonstrates: Fragment shader lighting

## ?? Object Locations

```
          [Sphere 1]           [Sphere 2]
         (White/Simple)      (Blue/Blended)
              |                    |
              v                    v
         (-3, 2, -3)           (3, 2, -3)
         [Rotating ?]        [Rotating ?]


         ???????????????????????????
                  STAGE
         ???????????????????????????
           [Cone 1]      [Cone 2]
          (Red/Vertex)  (Green/Frag)
              ?              ?
         (-2, 0.3, -4)  (2, 0.3, -4)
```

## ?? Viewing Tips

### Best Angles for Each Mode

**Simple Texture (Mode 1)**
- View from: Front (Z+)
- Light: All lights ON
- Look for: Pure texture colors

**Blended Texture (Mode 2)**
- View from: Any angle, move around
- Light: Point lights emphasized
- Look for: Blue tint mixed with texture

**Vertex Color (Mode 3)**
- View from: Side angle
- Light: Directional light
- Look for: Smooth but faceted appearance

**Fragment Color (Mode 4)**
- View from: 45° angle
- Light: Spot lights
- Look for: Sharp specular highlights

## ?? Animation Details

| Object | Motion | Speed |
|--------|--------|-------|
| Sphere 1 | Rotate Y-axis | 0.5 rad/s (clockwise) |
| Sphere 2 | Rotate Y-axis | -0.5 rad/s (counter-clockwise) |
| Cone 1 | Static | N/A |
| Cone 2 | Static | N/A |

## ?? Troubleshooting

### Objects Not Visible?
1. Press **8** to ensure objects are enabled
2. Press **0** to reset to default mode
3. Check camera position (press **Q/E** to adjust height)

### Can't See Specific Object?
1. Press **9** to cycle modes
2. Check current mode matches desired object
3. Verify lights are enabled (keys **1-3**)

### Lighting Looks Wrong?
1. Enable all light components (keys **5-7**)
2. Turn on all light types (keys **1-3**)
3. Move camera to different angle

### Performance Issues?
- Objects use ~2,600 triangles total
- This is very lightweight for modern GPUs
- If issues persist, press **8** to disable

## ?? Performance Info

```
Sphere (36×18):  ~666 vertices, ~1,296 triangles
Cone (36×18):    ~666 vertices, ~1,296 triangles
Total per frame:  ~1,332 vertices, ~2,592 triangles
```

## ?? Color Schemes

### Sphere 1 (Simple)
```
Ambient:  RGB(1.0, 1.0, 1.0) - White
Diffuse:  RGB(1.0, 1.0, 1.0) - White
Specular: RGB(0.5, 0.5, 0.5) - Gray
```

### Sphere 2 (Blended)
```
Ambient:  RGB(0.2, 0.3, 0.6) - Dark Blue
Diffuse:  RGB(0.3, 0.4, 0.8) - Blue
Specular: RGB(1.0, 1.0, 1.0) - White
```

### Cone 1 (Vertex)
```
Ambient:  RGB(0.8, 0.3, 0.2) - Red-Orange
Diffuse:  RGB(0.9, 0.4, 0.3) - Red-Orange
Specular: RGB(0.9, 0.9, 0.9) - Near-White
```

### Cone 2 (Fragment)
```
Ambient:  RGB(0.2, 0.6, 0.4) - Green
Diffuse:  RGB(0.3, 0.8, 0.5) - Bright Green
Specular: RGB(1.0, 1.0, 1.0) - White
```

## ?? Quick Commands

### See Everything
```
Press: 0 (reset)
Press: 8 (if objects hidden)
```

### Compare Shading Methods
```
Press: 9 until Mode 3 (Vertex/Gouraud)
Observe the shading
Press: 9 (switch to Mode 4 - Fragment/Phong)
Compare the difference
```

### See Pure Texture vs Blended
```
Press: 9 until Mode 1 (Simple)
Note the colors
Press: 9 (switch to Mode 2 - Blended)
See how blue tint affects texture
```

## ?? Status Indicators

When you press a key, the console will show:

- **8**: `"Textured Objects Display: ON/OFF"`
- **9**: `"Texture Mode: [Mode Name]"`
- **0**: `"Reset: Showing all textured objects"`

---

**?? Pro Tip**: Try disabling different light types (keys 1-3) while viewing different modes to see how each lighting component affects the appearance!

**?? Learning Path**:
1. Start with Mode 0 (see all objects)
2. Try Mode 1 & 2 (compare texture methods)
3. Try Mode 3 & 4 (compare shading methods)
4. Experiment with lighting (keys 1-7)

**?? For More Info**: See `TEXTURED_OBJECTS_README.md`
