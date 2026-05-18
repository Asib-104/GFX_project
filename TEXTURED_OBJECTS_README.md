# Textured Objects Feature Documentation

## Overview
This project now includes multiple textured objects with different rendering modes, including curvy surfaces (Sphere and Cone).

## New Classes Added

### 1. Sphere Class (`sphere.h`)
- Generates a smooth sphere with proper normals and texture coordinates
- Configurable sector and stack count for detail control
- Supports all Phong lighting components

### 2. Cone Class (`cone.h`)
- Generates a cone with base and curved surface
- Proper normal calculation for realistic lighting
- Texture coordinates mapped correctly

## Texture Rendering Modes

### Mode 1: Simple Texture Without Surface Color
**Object**: Left floating sphere (white)
- Pure texture display without color tinting
- Diffuse and ambient set to white (1.0, 1.0, 1.0)
- Shows texture in its original colors
- **Key Feature**: Demonstrates texture-only rendering

### Mode 2: Blended Texture With Surface Color
**Object**: Right floating sphere (blue tint)
- Texture blended with surface color
- Diffuse set to blue (0.3, 0.4, 0.8)
- Creates colored texture effect
- **Key Feature**: Demonstrates texture + material color blending

### Mode 3: Color Computed on Vertex
**Object**: Left stage cone (red-orange)
- Warm color palette (red-orange tones)
- Lighting computed at vertices and interpolated
- Faster but less accurate lighting
- **Key Feature**: Demonstrates Gouraud shading (vertex-level)

### Mode 4: Color Computed on Fragment
**Object**: Right stage cone (green)
- Cool color palette (green tones)
- Lighting computed per-fragment
- More accurate and detailed lighting
- **Key Feature**: Demonstrates Phong shading (fragment-level)

## Keyboard Controls

### Texture & Object Controls
- **8**: Toggle all textured objects ON/OFF
- **9**: Cycle through texture modes (0-4)
  - Mode 0: Show all objects
  - Mode 1: Simple texture only
  - Mode 2: Blended texture only
  - Mode 3: Vertex color only
  - Mode 4: Fragment color only
- **0**: Reset to show all textured objects

### Existing Controls
- **1-3**: Toggle different light types
- **5-7**: Toggle light components (ambient, diffuse, specular)
- **4**: Cycle viewport modes
- **W/A/S/D**: Camera movement
- **Q/E**: Camera up/down
- **Arrow keys**: Camera rotation
- **ESC**: Exit

## Object Positions in Scene

1. **Left Floating Sphere** (Simple Texture)
   - Position: (-3.0, 2.0, -3.0)
   - Rotates slowly (0.5 rad/s)
   - Scale: 0.8

2. **Right Floating Sphere** (Blended Texture)
   - Position: (3.0, 2.0, -3.0)
   - Rotates slowly opposite direction (-0.5 rad/s)
   - Scale: 0.8

3. **Left Stage Cone** (Vertex Color)
   - Position: (-2.0, 0.3, -4.0)
   - On the stage
   - Scale: (0.5, 0.6, 0.5)

4. **Right Stage Cone** (Fragment Color)
   - Position: (2.0, 0.3, -4.0)
   - On the stage
   - Scale: (0.5, 0.6, 0.5)

## Technical Details

### Vertex Attributes
All curvy surface objects use the following vertex format:
- **Position**: 3 floats (x, y, z)
- **Normal**: 3 floats (nx, ny, nz)
- **Texture Coordinates**: 2 floats (u, v)
- **Total**: 8 floats per vertex

### Sphere Generation
- Uses parametric sphere equations
- Configurable sectors (longitude divisions)
- Configurable stacks (latitude divisions)
- Default: 36 sectors × 18 stacks

### Cone Generation
- Parametric cone generation
- Includes base circle
- Proper normal calculation for smooth shading
- Height: 2.0, Base radius: 1.0

## Shader Requirements

The implementation uses existing Phong shading shaders:
- **Vertex Shader**: `vertexShaderForPhongShadingWithTexture.vs`
- **Fragment Shader**: `fragmentShaderForPhongShadingWithTexture.fs`

Both shaders support:
- Multiple light types (directional, point, spot)
- Texture mapping
- Material properties (ambient, diffuse, specular, shininess)

## Lighting Computation

### Vertex Shader (Gouraud Shading)
- Lighting computed once per vertex
- Results interpolated across fragments
- Faster performance
- Less accurate highlights

### Fragment Shader (Phong Shading)
- Lighting computed per fragment
- More accurate lighting
- Better specular highlights
- Higher computational cost

## Color vs Texture

### Pure Texture (White Material)
```cpp
ambient = vec3(1.0, 1.0, 1.0);
diffuse = vec3(1.0, 1.0, 1.0);
```
Result: Texture colors are shown without modification

### Blended Texture (Colored Material)
```cpp
ambient = vec3(0.2, 0.3, 0.6);  // Blue tint
diffuse = vec3(0.3, 0.4, 0.8);  // Blue tint
```
Result: Texture colors are multiplied with material colors

## Usage Example

To add more textured objects:

```cpp
// Create sphere with custom properties
Sphere mySphere = Sphere(
    textureID,                    // Texture
    glm::vec3(1.0f, 1.0f, 1.0f), // Ambient
    glm::vec3(1.0f, 1.0f, 1.0f), // Diffuse
    glm::vec3(0.5f, 0.5f, 0.5f), // Specular
    32.0f,                        // Shininess
    36,                           // Sectors
    18                            // Stacks
);

// Draw the sphere
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0));
mySphere.draw(shader, model);
```

## Performance Considerations

- Sphere (36×18): ~666 vertices, ~1,296 triangles
- Cone (36×18): ~666 vertices, ~1,296 triangles
- Total new geometry: ~2,592 triangles per frame
- Rotating objects updated every frame

## Future Enhancements

Possible additions:
1. More curvy surfaces (torus, ellipsoid, etc.)
2. Normal mapping for enhanced detail
3. Multiple textures per object
4. Procedural textures
5. Texture animation
6. Environment mapping

## Troubleshooting

### Objects Not Visible
- Check if `showTexturedObjects` is true (press 8)
- Verify texture files exist at specified paths
- Check camera position and orientation

### Lighting Issues
- Ensure lights are enabled (keys 1-3)
- Verify light components are on (keys 5-7)
- Check material properties are set correctly

### Performance Issues
- Reduce sector/stack count in sphere/cone constructors
- Disable objects not needed (press 8)
- Reduce number of light sources
