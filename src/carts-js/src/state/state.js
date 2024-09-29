class State {
    sprites = [];
    spritesheet = [];
    activeSprite = 0;
    cart = null;

    zoom = 3;

    constructor(cart) {
        this.cart = cart;
    }

    setSpritesheet(spritesheet) {
        this.spritesheet = spritesheet;
    }

    getUpdatedCart() {
        return {
            ...this.cart,
            sprite_data: this.sprites.map(sprite => [sprite.x, sprite.y, sprite.width, sprite.height, sprite.flags]).flat(),
            spritesheet: this.spritesheet
        }
    }
    
    loadCart() {
        if (!this.cart) {
            log("No cart loaded");
            return;
        }

        this.setSpritesheet(this.cart.spritesheet);
        this.sprites = [];
        for (let i = 0; i < this.cart.sprite_data.length; i += 5) {
            this.sprites.push({
                x: this.cart.sprite_data[i],
                y: this.cart.sprite_data[i + 1],
                width: this.cart.sprite_data[i + 2],
                height: this.cart.sprite_data[i + 3],
                flags: this.cart.sprite_data[i + 4]
            });
        }

        this.map = [];

    }

    getActiveSprite() {
        return this.sprites[this.activeSprite];
    }

    incSpriteWidth() {
        const spritesheetSize = Math.ceil(Math.sqrt(this.spritesheet.length));
        this.sprites[this.activeSprite].width++;

        if (this.sprites[this.activeSprite].width + this.sprites[this.activeSprite].x > spritesheetSize) {
            this.sprites[this.activeSprite].width = spritesheetSize - this.sprites[this.activeSprite].x;
        }
    }

    decSpriteWidth() {
        this.sprites[this.activeSprite].width--;
        if (this.sprites[this.activeSprite].width < 1) {
            this.sprites[this.activeSprite].width = 1;
        }
    }

    incSpriteHeight() {
        const spritesheetSize = Math.ceil(Math.sqrt(this.spritesheet.length));
        this.sprites[this.activeSprite].height++;

        if (this.sprites[this.activeSprite].height + this.sprites[this.activeSprite].y > spritesheetSize) {
            this.sprites[this.activeSprite].height = spritesheetSize - this.sprites[this.activeSprite].y;
        }
    }

    decSpriteHeight() {
        this.sprites[this.activeSprite].height--;

        if (this.sprites[this.activeSprite].height < 1) {
            this.sprites[this.activeSprite].height = 1;
        }
    }

    incSpriteX() {
        const spritesheetSize = Math.ceil(Math.sqrt(this.spritesheet.length));
        this.sprites[this.activeSprite].x++;

        if (this.sprites[this.activeSprite].x + this.sprites[this.activeSprite].width > spritesheetSize) {
            this.sprites[this.activeSprite].x = spritesheetSize - this.sprites[this.activeSprite].width;
        }
    }

    decSpriteX() {
        this.sprites[this.activeSprite].x--;
        if (this.sprites[this.activeSprite].x < 0) {
            this.sprites[this.activeSprite].x = 0;
        }
    }

    incSpriteY() {
        const spritesheetSize = Math.ceil(Math.sqrt(this.spritesheet.length));
        this.sprites[this.activeSprite].y++;

        if (this.sprites[this.activeSprite].y + this.sprites[this.activeSprite].height > spritesheetSize) {
            this.sprites[this.activeSprite].y = spritesheetSize - this.sprites[this.activeSprite].height;
        }
    }

    decSpriteY() {
        this.sprites[this.activeSprite].y--;
        if (this.sprites[this.activeSprite].y < 0) {
            this.sprites[this.activeSprite].y = 0;
        }
    }

    nextSprite() {
        this.activeSprite = (this.activeSprite + 1) % this.sprites.length;
    }

    prevSprite() {
        this.activeSprite = (this.activeSprite - 1 + this.sprites.length) % this.sprites.length;
    }
}

export const state = new State();