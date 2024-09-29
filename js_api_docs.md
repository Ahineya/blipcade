# Blipcade JavaScript API Documentation

## Table of Contents

- [Namespace: global](#namespace-global)
   - [Function: log](#function-log)
   - [Function: text](#function-text)
- [Namespace: Graphics](#namespace-graphics)
   - [Function: setTransparentColor](#function-settransparentcolor)
   - [Function: fillScreen](#function-fillscreen)
   - [Function: putPixel](#function-putpixel)
   - [Function: drawLine](#function-drawline)
   - [Function: drawFilledCircle](#function-drawfilledcircle)
   - [Function: drawCircle](#function-drawcircle)
- [Namespace: Input](#namespace-input)
   - [Function: isKeyPressed](#function-iskeypressed)

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
#### Function: `drawCircle`
**Description:**   Draws a circle on the canvas. 

**Parameters (Required):**

| Name | Type | Description |
|------|------|-------------|
| `center_x` | `number` | The x-coordinate of the center of the circle. |
| `center_y` | `number` | The y-coordinate of the center of the circle. |
| `radius` | `number` | The radius of the circle. |
| `color` | `number` | The color of the circle. |

**Example:**

```javascript
Graphics.drawCircle(100, 100, 50, 0x50); // Draws a filled circle with a radius of 50 at (100, 100).
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
