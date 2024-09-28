# Blipcade JavaScript API Documentation

## Table of Contents

- [Namespace: Global](#namespace-global)
   - [Function: log](#function-log)
   - [Function: text](#function-text)
- [Namespace: User](#namespace-user)
   - [Property: name](#property-name)
   - [Function: greet](#function-greet)

---

Namespace: `Global`
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
| `x` | `number` | The x-coordinate of the text. |

**Parameters (Optional):**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `y` | `number` | `0` | The y-coordinate of the text. |
| `color` | `number` | `0xfe` | The color of the text (default is 0xfe). |

**Returns:** {void}

**Example:**

```javascript
text("Hello, Blipcade!", 10, 20, 0xff);
```

---
Namespace: `User`
---
### Provides user-related functionalities.



**Properties:**

| Name | Type | Description |
|------|------|-------------|
| `name` | `string` | The name of the user. |

#### Function: `greet`
**Description:** Greets the user with their name. 

**Example:**

```javascript
User.greet();
```

---
