class LightingSystem {
    init() {
        const light = ECS.createEntity();

        const name = "sunlight";

        ECS.addComponent(light, "Light", {
            color: 0xF5F5B2,
            intensity: 1,
            name
        });

        Lighting.addLightEffect(name, {
            tintColor: 0xF5F5B2, // Warm yellow
            opacity: 0.5,
            maskImagePath: "resources/trapezoid.png" // TODO: move to res://
        });
    }

    update() {
        ECS.forEachEntity(["Light"], (_, light) => {
            light.intensity = (0.5 + 0.5 * Math.sin(Date.now() / 1000)) * 0.5;
            Lighting.changeLightOpacity(light.name, light.intensity);
        });
    }
}

export const lightingSystem = new LightingSystem();