// Pre-allocated result for the smoothDamp function
const smoothResult = { value: 0, velocity: 0 };

export function smoothDamp(current: number, target: number, currentVelocity: number, smoothness: number, deltaTime: number) {
    const omega = 2 / smoothness;
    const x = omega * deltaTime;
    const exp = 1 / (1 + x + 0.48 * x * x + 0.235 * x * x * x);
    const change = current - target;
    const temp = (currentVelocity + omega*change) * deltaTime;
    smoothResult.value = target + (change + temp) * exp;
    smoothResult.velocity = (currentVelocity - omega*temp) * exp;
    return smoothResult;
}
