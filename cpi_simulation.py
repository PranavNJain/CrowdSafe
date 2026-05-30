"""
CrowdSafe — CPI Algorithm Simulation (Python)
Use this to test and visualize the Crowd Pressure Index algorithm
without physical hardware.
"""

import random
import time
import math

W_DENSITY  = 0.45
W_MOVEMENT = 0.35
W_SHOCK    = 0.20

SHOCK_THRESHOLD = 1.8


def compute_density_score(distances: list[float], max_dist: float = 80.0) -> float:
    avg = sum(distances) / len(distances)
    score = (1 - min(avg, max_dist) / max_dist) * 100
    return round(max(0, min(score, 100)), 2)


def compute_movement_score(current: list[float], previous: list[float]) -> float:
    delta = sum(abs(c - p) for c, p in zip(current, previous))
    return round(min(delta * 5.0, 100), 2)


def compute_shock_score(accel_magnitude: float) -> float:
    if accel_magnitude < SHOCK_THRESHOLD:
        return 0.0
    return round(min((accel_magnitude - SHOCK_THRESHOLD) * 60.0, 100), 2)


def compute_cpi(density: float, movement: float, shock: float) -> float:
    cpi = (W_DENSITY * density) + (W_MOVEMENT * movement) + (W_SHOCK * shock)
    return round(min(max(cpi, 0), 100), 2)


def get_alert_level(cpi: float) -> str:
    if cpi >= 70:
        return "DANGER ⚠️"
    elif cpi >= 40:
        return "WARNING 🟡"
    return "NORMAL  ✅"


def simulate():
    """Simulate a crowd scenario escalating from normal → warning → danger."""
    print("=" * 55)
    print("  CrowdSafe — CPI Simulation")
    print("=" * 55)
    print(f"{'Time':>5}  {'D-Score':>8} {'M-Score':>8} {'S-Score':>8} {'CPI':>6}  {'Level'}")
    print("-" * 55)

    prev_distances = [80.0, 80.0, 80.0]
    t = 0

    for phase, cycles in [("normal", 5), ("warning", 5), ("danger", 5)]:
        for _ in range(cycles):
            # Simulate distances shrinking as crowd grows
            if phase == "normal":
                distances = [random.uniform(60, 80) for _ in range(3)]
                accel     = random.uniform(1.0, 1.5)
            elif phase == "warning":
                distances = [random.uniform(30, 60) for _ in range(3)]
                accel     = random.uniform(1.4, 1.9)
            else:
                distances = [random.uniform(5, 30) for _ in range(3)]
                accel     = random.uniform(1.8, 3.0)

            d_score = compute_density_score(distances)
            m_score = compute_movement_score(distances, prev_distances)
            s_score = compute_shock_score(accel)
            cpi     = compute_cpi(d_score, m_score, s_score)
            level   = get_alert_level(cpi)

            print(f"{t:>5}s  {d_score:>8.1f} {m_score:>8.1f} {s_score:>8.1f} {cpi:>6.1f}  {level}")

            prev_distances = distances
            t += 1
            time.sleep(0.3)

    print("=" * 55)
    print("Simulation complete.")


if __name__ == "__main__":
    simulate()
