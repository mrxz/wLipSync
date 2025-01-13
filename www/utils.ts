export function smoothDamp(current: number, target: number, currentVelocity: number, smoothness: number, deltaTime: number) {
    let omega = 2 / smoothness;
    let x = omega * deltaTime;
    let exp = 1 / (1 + x + 0.48 * x * x + 0.235 * x * x * x);
    let change = current - target;
    let temp = (currentVelocity + omega*change) * deltaTime;
    currentVelocity = (currentVelocity - omega*temp) * exp;
    return [target + (change + temp) * exp, currentVelocity];
}
