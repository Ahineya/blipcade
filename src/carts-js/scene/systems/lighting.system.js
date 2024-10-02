class LightingSystem {
    init() {
        const light = ECS.createEntity();
        ECS.addComponent(light, "Light", {
            color: 0xF5F5B2,
            intensity: 1,
        });

        Lighting.addLightEffect("sunlight", {
            tintColor: 0xF5F5B2, // Warm yellow
            opacity: 0.5,
            maskImagePath: "resources/trapezoid.png"
        });
    }

    update() {
        ECS.forEachEntity(["Light"], (_, light) => {
            light.intensity = (0.5 + 0.5 * Math.sin(Date.now() / 1000)) * 0.5;
            Lighting.changeLightOpacity("sunlight", light.intensity);
        });
    }
}

export const lightingSystem = new LightingSystem();