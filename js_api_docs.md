# Blipcade JavaScript API Documentation

## Table of Contents

- [Namespace: global](#namespace-global)
   - [Function: log](#function-log)
   - [Function: text](#function-text)
- [Namespace: Graphics](#namespace-graphics)
   - [Function: setTransparentColor](#function-settransparentcolor)
   - [Function: fillScreen](#function-fillscreen)
   - [Function: drawSprite](#function-drawsprite)
   - [Function: drawSpriteEx](#function-drawspriteex)
   - [Function: putPixel](#function-putpixel)
   - [Function: drawLine](#function-drawline)
   - [Function: drawFilledCircle](#function-drawfilledcircle)
   - [Function: drawFilledRect](#function-drawfilledrect)
   - [Function: drawCircle](#function-drawcircle)
- [Namespace: Lighting](#namespace-lighting)
   - [Function: addLightEffect](#function-addlighteffect)
   - [Function: removeLightEffect](#function-removelighteffect)
   - [Function: updateLightEffect](#function-updatelighteffect)
   - [Function: changeLightOpacity](#function-changelightopacity)
   - [Function: changeLightTintColor](#function-changelighttintcolor)
- [Namespace: Input](#namespace-input)
   - [Function: isKeyPressed](#function-iskeypressed)
   - [Function: getMousePos](#function-getmousepos)
   - [Function: isMouseButtonPressed](#function-ismousebuttonpressed)
- [Namespace: ECS](#namespace-ecs)
   - [Function: createEntity](#function-createentity)
   - [Function: destroyEntity](#function-destroyentity)
   - [Function: addComponent](#function-addcomponent)
   - [Function: removeComponent](#function-removecomponent)
   - [Function: getComponent](#function-getcomponent)
   - [Function: forEachEntity](#function-foreachentity)
- [Namespace: Collision](#namespace-collision)
   - [Function: getCollider](#function-getcollider)
   - [Function: checkCollisionPoint](#function-checkcollisionpoint)
- [Namespace: Pathfinding](#namespace-pathfinding)
   - [Function: findPath](#function-findpath)
   - [Function: getNavMesh](#function-getnavmesh)
- [Namespace: Sound](#namespace-sound)
   - [Function: loadSound](#function-loadsound)
   - [Function: playSound](#function-playsound)
   - [Function: stopSound](#function-stopsound)
   - [Function: setSoundVolume](#function-setsoundvolume)

---

Namespace: `global`
---
### These are global functions that can be called from the JavaScript code.



#### Function: `log`
**Description:** Logs a message to the console.

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `message` | `string` | `N/A` | The message to log. |

---
#### Function: `text`
**Description:** Draws text on the canvas.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `text` | `string` | The text to draw. |

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `x` | `number` | `0` | The x-coordinate of the text. |
| `y` | `number` | `0` | The y-coordinate of the text. |
| `color` | `number` | `0xfe` | The color of the text (default is 0xfe). |

**Returns:** {void}

**Example:**

```javascript
text("Hello, Blipcade!", 10, 20, 0xff);
```

---
Namespace: `Graphics`
---
### Provides graphics-related functionalities.



#### Function: `setTransparentColor`
**Description:** Sets the color to use as transparent when drawing. 

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `color` | `number` | `0xff` | The color to use as transparent. |

**Example:**

```javascript
Graphics.setTransparentColor(0x00); // Black color will be transparent
```

---
#### Function: `fillScreen`
**Description:** Fills the screen with a color.

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `color` | `number` | `0x00` | The color to fill the screen with. |

**Example:**

```javascript
Graphics.fillScreen(0xff); // Fills the screen with white color.
```

---
#### Function: `drawSprite`
**Description:**    Draws a sprite on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `x` | `number` | The x-coordinate of the sprite. |
| `y` | `number` | The y-coordinate of the sprite. |
| `spriteIndex` | `number` | The index of the sprite to draw. |

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `spriteSheetIndex` | `number` | `0` | The index of the sprite sheet to use. |
| `flipX` | `boolean` | `false` | Whether to flip the sprite horizontally. |
| `flipY` | `boolean` | `false` | Whether to flip the sprite vertically. |

**Example:**

```javascript
Graphics.drawSprite(100, 100, 0); // Draws the first sprite from the first spritesheet at (100, 100).
```

---
#### Function: `drawSpriteEx`
**Description:**      Draws a sprite on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `x` | `number` | The x-coordinate of the sprite. |
| `y` | `number` | The y-coordinate of the sprite. |
| `spriteIndex` | `number` | The index of the sprite to draw. |

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `spriteSheetIndex` | `number` | `0` | The index of the sprite sheet to use. |
| `flipX` | `boolean` | `false` | Whether to flip the sprite horizontally. |
| `flipY` | `boolean` | `false` | Whether to flip the sprite vertically. |
| `scale` | `number` | `1.0` | The scale of the sprite. |
| `originX` | `number` | `0.5` | The x origin of the sprite, from 0 to 1, where 0 is the left and 1 is the right. |
| `originY` | `number` | `0.5` | The y origin of the sprite, from 0 to 1, where 0 is the top and 1 is the bottom. |

**Example:**

```javascript
Graphics.drawSpriteEx(100, 100, 0, 0, false, false, 1.0, 1.0); // Draws the first sprite from the first spritesheet at (100, 100).
```

---
#### Function: `putPixel`
**Description:**   Puts a pixel on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `x` | `number` | The x-coordinate of the pixel. |
| `y` | `number` | The y-coordinate of the pixel. |
| `color` | `number` | The color of the pixel. |

**Example:**

```javascript
Graphics.putPixel(100, 100, 0xfe); // Puts a pixel at (100, 100) with white color.
```

---
#### Function: `drawLine`
**Description:**   Draws a line on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `x1` | `number` | The x-coordinate of the starting point of the line. |
| `y1` | `number` | The y-coordinate of the starting point of the line. |
| `x2` | `number` | The x-coordinate of the ending point of the line. |
| `y2` | `number` | The y-coordinate of the ending point of the line. |
| `color` | `number` | The color of the line. |

**Example:**

```javascript
Graphics.drawLine(0, 0, 100, 100, 0xfe); // Draws a line from (0, 0) to (100, 100) with white color.
```

---
#### Function: `drawFilledCircle`
**Description:**   Draws a filled circle on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `center_x` | `number` | The x-coordinate of the center of the circle. |
| `center_y` | `number` | The y-coordinate of the center of the circle. |
| `radius` | `number` | The radius of the circle. |
| `color` | `number` | The color of the circle. |

**Example:**

```javascript
Graphics.drawFilledCircle(100, 100, 50, 0x50); // Draws a filled circle with a radius of 50 at (100, 100).
```

---
#### Function: `drawFilledRect`
**Description:**    Draws a filled rectangle on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `x` | `number` | The x-coordinate of the top-left corner of the rectangle. |
| `y` | `number` | The y-coordinate of the top-left corner of the rectangle. |
| `width` | `number` | The width of the rectangle. |
| `height` | `number` | The height of the rectangle. |
| `color` | `number` | The color of the rectangle. |

**Example:**

```javascript
Graphics.drawFilledRect(100, 100, 50, 50, 0x50); // Draws a filled rectangle with a width and height of 50 at (100, 100) with color 0x50.
```

---
#### Function: `drawCircle`
**Description:**   Draws a circle on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `center_x` | `number` | The x-coordinate of the center of the circle. |
| `center_y` | `number` | The y-coordinate of the center of the circle. |
| `radius` | `number` | The radius of the circle. |
| `color` | `number` | The color of the circle. |
| `params` | `object` | Additional parameters for the circle. |

**Example:**

```javascript
Graphics.drawCircle(100, 100, 50, 0x50); // Draws a filled circle with a radius of 50 at (100, 100).
```

---
Namespace: `Lighting`
---
### Provides lighting-related functionalities.



#### Function: `addLightEffect`
**Description:** Adds a new light effect.  Lighting.addLightEffect("sunlight", { tintColor: 0xffe066, opacity: 0.5, maskImagePath: "assets/masks/trapezoid.png" });

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `name` | `string` | Unique name for the light effect. |
| `params` | `object` | Parameters for the light effect. |
| `params.tintColor` | `number` | The tint color in hexadecimal (e.g., 0xff0000 for red). |
| `params.opacity` | `number` | Opacity of the tint (0.0 to 1.0). |
| `params.maskImagePath` | `string` | Path to the mask image. |

**Example:**

```javascript

```

---
#### Function: `removeLightEffect`
**Description:** Removes an existing light effect.  Lighting.removeLightEffect("sunlight");

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `name` | `string` | The name of the light effect to remove. |

**Example:**

```javascript

```

---
#### Function: `updateLightEffect`
**Description:** Updates an existing light effect.  Lighting.updateLightEffect("sunlight", { tintColor: 0xffffff, opacity: 0.7 });

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `name` | `string` | The name of the light effect to update. |
| `params` | `object` | Updated parameters for the light effect. |

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `params.tintColor` | `number` | `N/A` | The new tint color in hexadecimal. |
| `params.opacity` | `number` | `N/A` | New opacity of the tint. |
| `params.maskImagePath` | `string` | `N/A` | New path to the mask image. |

**Example:**

```javascript

```

---
#### Function: `changeLightOpacity`
**Description:**   Updates the opacity of an existing light effect. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `name` | `string` | The name of the light effect to update. |
| `opacity` | `number` | New opacity of the tint. |

**Example:**

```javascript
Lighting.changeLightOpacity("sunlight", 0.7);
```

---
#### Function: `changeLightTintColor`
**Description:**   Updates the tint color of an existing light effect. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `name` | `string` | The name of the light effect to update. |
| `tintColor` | `number` | The new tint color in hexadecimal. |

**Example:**

```javascript
Lighting.changeLightTintColor("sunlight", 0xff0000); // Changes the tint color to red.
```

---
Namespace: `Input`
---
### Provides input-related functionalities.



#### Function: `isKeyPressed`
**Description:**  Checks if a key is currently pressed. A key codes are defined in the `Key` enum.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `key` | `number` | The key code to check. |

**Returns:** {boolean} - `true` if the key is pressed, `false` otherwise.

**Example:**

```javascript
Input.isKeyPressed(Key.ArrowUp); // Returns true if the up arrow key is pressed.
```

---
#### Function: `getMousePos`
**Description:**  Gets the current mouse position.  

**Returns:** {object} - An object with `x` and `y` properties representing the mouse position.

**Example:**

```javascript
Input.getMousePos(); // Returns an object with `x` and `y` properties representing the mouse position.
```

---
#### Function: `isMouseButtonPressed`
**Description:**   

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `button` | `number` | The button code to check. 1 for left, 2 for right, 3 for middle. |

**Returns:** {boolean} - `true` if the button is pressed, `false` otherwise.

**Example:**

```javascript
Input.isMouseButtonPressed(1); // Returns true if the left mouse button is pressed.
```

---
Namespace: `ECS`
---
### Provides Entity-Component-System functionalities.



#### Function: `createEntity`
**Description:** Creates a new entity. 

**Returns:** {number} - The ID of the created entity.

**Example:**

```javascript
const entity = ECS.createEntity(); // Creates a new entity.
```

---
#### Function: `destroyEntity`
**Description:** Destroys an entity. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `entity` | `number` | The ID of the entity to destroy. |

**Example:**

```javascript
ECS.destroyEntity(entity); // Destroys the entity with the given ID.
```

---
#### Function: `addComponent`
**Description:** Adds a component to an entity. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `entity` | `number` | The ID of the entity to add the component to. |
| `typeName` | `string` | The name of the component type. |
| `component` | `object` | The component to add. |

**Example:**

```javascript
ECS.addComponent(entity, "Position", { x: 10, y: 20 }); // Adds a Position component to the entity.
```

---
#### Function: `removeComponent`
**Description:** Removes a component from an entity. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `entity` | `number` | The ID of the entity to remove the component from. |
| `typeName` | `string` | The name of the component type. |

**Example:**

```javascript
ECS.removeComponent(entity, "Position"); // Removes the Position component from the entity.
```

---
#### Function: `getComponent`
**Description:** Gets a component from an entity. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `entity` | `number` | The ID of the entity to get the component from. |
| `typeName` | `string` | The name of the component type. |

**Returns:** {object} - The component.

**Example:**

```javascript
const position = ECS.getComponent(entity, "Position"); // Gets the Position component from the entity.
```

---
#### Function: `forEachEntity`
**Description:** Iterates over entities that have all the specified components. Iteration order is guaranteed to be the same as the order in which entities were created. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `componentTypes` | `Array` | An array of string component types to filter entities by. |
| `callback` | `function` | The callback function to call for each entity. The first argument is the entity ID. Subsequent arguments are the components in the order specified in the `componentTypes` array. |

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `reverse` | `boolean` | `false` | Whether to iterate over entities in reverse order. |

**Example:**

```javascript
ECS.forEachEntity(["Position", "Velocity"], (entity, position, velocity) => { ... }
```

---
Namespace: `Collision`
---
### Provides collision-related functionalities.



#### Function: `getCollider`
**Description:**   Gets the collider object at the specified index. It has ['type', 'vertices', 'triangles'] properties.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `index` | `number` | The index of the collider to get. |

**Returns:** {object} - The collider object. It has ['type', 'vertices', 'triangles'] properties.

**Example:**

```javascript
const collider = Collision.getCollider(0); // Gets the collider object at index 0.
```

---
#### Function: `checkCollisionPoint`
**Description:**   Checks if a point collides with a collider.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `x` | `number` | The x-coordinate of the point to check. |
| `y` | `number` | The y-coordinate of the point to check. |
| `colliderIndex` | `number` | The index of the collider to check. |

**Returns:** {boolean} - `true` if the point collides with the collider, `false` otherwise.

**Example:**

```javascript
Collision.checkCollisionPoint(100, 100, 0); // Checks if the point (100, 100) collides with the collider at index 0.
```

---
Namespace: `Pathfinding`
---
### 

Provides pathfinding-related functionalities.

#### Function: `findPath`
**Description:**   Finds a path from the starting point to the ending point using the specified navigation mesh.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `startX` | `number` | The x-coordinate of the starting point. |
| `startY` | `number` | The y-coordinate of the starting point. |
| `endX` | `number` | The x-coordinate of the ending point. |
| `endY` | `number` | The y-coordinate of the ending point. |
| `navigationMeshId` | `number` | The ID of the navigation mesh to use. |

**Returns:** {Array} - An array of points representing the path. Each point is an object with `x` and `y` properties.

**Example:**

```javascript
Pathfinding.findPath(0, 0, 100, 100, 0); // Finds a path from (0, 0) to (100, 100) using the navigation mesh with ID 0.
```

---
#### Function: `getNavMesh`
**Description:**   Gets the navigation mesh with the specified ID.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `navMeshId` | `number` | The ID of the navigation mesh to get. |

**Returns:** {Array} - An array of regions in the navigation mesh. Each region is an object with a `vertices` property containing an array of points.

**Example:**

```javascript
Pathfinding.getNavMesh(0); // Gets the navigation mesh with ID 0.
```

---
Namespace: `Sound`
---
### 

Provides sound-related functionalities.

#### Function: `loadSound`
**Description:**   Loads a sound file.  

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `path` | `string` | The path to the sound file. |

**Returns:** {number} - The ID of the loaded sound.

**Example:**

```javascript
const sound = Sound.loadSound("assets/sounds/jump.wav"); // Loads the sound file.
```

---
#### Function: `playSound`
**Description:**   Plays a sound. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `soundId` | `number` | The ID of the sound to play. |

**Example:**

```javascript
Sound.playSound(soundId); // Plays the sound with the given ID.
```

---
#### Function: `stopSound`
**Description:**   Stops a sound. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `soundId` | `number` | The ID of the sound to stop. |

**Example:**

```javascript
Sound.stopSound(soundId); // Stops the sound with the given ID.
```

---
#### Function: `setSoundVolume`
**Description:**   Sets the volume of a sound. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `soundId` | `number` | The ID of the sound to set the volume for. |
| `volume` | `number` | The volume to set (0.0 to 1.0). |

**Example:**

```javascript
Sound.setSoundVolume(soundId, 0.5); // Sets the volume of the sound with the given ID to 0.5.
```

---
