export class GlitchAnimator {
    static PHASE_POOL = [1.5106780578114254, -3.5857690474322403, -0.2626735770838966, -0.860233192775377, 1.6951223830499789, 1.5524085906175014, 0.7440257681906406, -2.941921205354392, -1.85592274913712, -2.0567966870581866, -1.8363292233839008, 0.3396987386004575, 0.5683444656535208, -2.465715252183184, 3.9826148609714522, -2.0264721464184277, -0.3267395873909855, -0.13622742506631624, 1.4714123056414365, 0.7019045456493025, 1.0945983916335749, -0.4965531623917254, -3.910598927075993, 0.19179530772758113, 3.4204553721272504, -2.8361832558480167, -3.825596996154447, -0.903800712006511, 2.5975033850141447, -0.6792947266694451, -2.551940355284998, -3.4354442355378296]

    constructor(letterCount, initialIntensity = 7, decayRate = 0.95, frequency = 0.1, settleThreshold = 0.1) {
        this.letterOffsets = new Array(letterCount).fill(0);
        this.intensity = initialIntensity;
        this.decayRate = decayRate;
        this.frequency = frequency;
        this.settleThreshold = settleThreshold;
        this.frameCount = 0;
        this.phaseOffsets = this.generatePhaseOffsets(letterCount);
    }

    generatePhaseOffsets(count) {
        const offsets = [];
        for (let i = 0; i < count; i++) {
            offsets.push(GlitchAnimator.PHASE_POOL[i % GlitchAnimator.PHASE_POOL.length]);
        }
        return offsets;
    }

    update() {
        this.frameCount++;

        // Decay the intensity
        this.intensity *= this.decayRate;

        if (this.intensity < this.settleThreshold) {
            this.letterOffsets.fill(0);
            return this.letterOffsets;
        }

        for (let i = 0; i < this.letterOffsets.length; i++) {
            const sineValue = Math.sin(this.frameCount * this.frequency + this.phaseOffsets[i]);
            this.letterOffsets[i] = sineValue * this.intensity;
        }

        return this.letterOffsets;
    }

    isSettled() {
        return this.intensity < this.settleThreshold;
    }
}