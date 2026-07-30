import os
import re

content = """#ifndef MOVEMENT_STRATEGY_H
#define MOVEMENT_STRATEGY_H

#include "raylib.h"
#include <cmath>

class IMovementBehavior {
public:
    virtual ~IMovementBehavior() = default;
    virtual void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) = 0;
};

class StraightMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        position.y += moveSpeed * deltaTime;
    }
};

class HorizontalSweepMovement : public IMovementBehavior {
private:
    float direction;
public:
    HorizontalSweepMovement(float startDir) : direction(startDir) {}
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        position.x += direction * moveSpeed * deltaTime;
        if (direction > 0 && position.x > screenWidth + 100) {
            position.x = -100;
        } else if (direction < 0 && position.x < -100) {
            position.x = screenWidth + 100;
        }
    }
};

class VerticalZigzagMovement : public IMovementBehavior {
private:
    float startX;
    bool initialized = false;
    float dx = 1.0f;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            startX = position.x;
            initialized = true;
        }
        position.y += moveSpeed * 0.3f * deltaTime;
        position.x += dx * moveSpeed * 0.5f * deltaTime;
        if (position.x > startX + 250.0f) {
            dx = -1.0f;
        } else if (position.x < startX - 250.0f) {
            dx = 1.0f;
        }
        if (position.y > screenHeight + 100) {
            position.y = -100;
        }
    }
};

class HorizontalBounceMovement : public IMovementBehavior {
private:
    float startX;
    float drift;
    float direction;
    float targetY;
    bool initialized = false;
public:
    HorizontalBounceMovement(float targetY, float drift = 300.0f, float startDir = 1.0f) 
        : targetY(targetY), drift(drift), direction(startDir) {}

    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            startX = position.x;
            initialized = true;
        }
        if (position.y < targetY) {
            position.y += moveSpeed * deltaTime;
        } else {
            position.x += direction * moveSpeed * deltaTime;
            if (direction > 0 && position.x > startX + drift) {
                direction = -1.0f;
            } else if (direction < 0 && position.x < startX - drift) {
                direction = 1.0f;
            }
        }
    }
};

class MeteorDiveMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        position.y += moveSpeed * 3.0f * deltaTime;
    }
};

class SineZigzagMovement : public IMovementBehavior {
private:
    float startX;
    float time = 0.0f;
    float amplitude = 150.0f;
    float frequency = 3.0f;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            startX = position.x;
            initialized = true;
        }
        time += deltaTime;
        position.y += moveSpeed * 0.5f * deltaTime;
        position.x = startX + math.sin(time * frequency) * amplitude;
        if (position.y > screenHeight + 100) {
            position.y = -100;
        }
    }
};

class SpiralMovement : public IMovementBehavior {
private:
    float time = 0.0f;
    float radius = 0.0f;
    float maxRadius = 250.0f;
    float rotationSpeed = 3.0f;
    Vector2 center;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            center = position;
            initialized = true;
        }
        time += deltaTime;
        center.y += moveSpeed * 0.4f * deltaTime;
        if (radius < maxRadius) {
            radius += 50.0f * deltaTime;
        }
        position.x = center.x + math.cos(time * rotationSpeed) * radius;
        position.y = center.y + math.sin(time * rotationSpeed) * radius;
    }
};

#endif // MOVEMENT_STRATEGY_H
"""

import re
import os

out_dir_inc = 'include'
out_dir_src = 'src'

# Split into classes (rough parsing)
classes = [
    ("IMovementBehavior", "", """class IMovementBehavior {
public:
    virtual ~IMovementBehavior() = default;
    virtual void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) = 0;
};"""),
    ("StraightMovement", "", """class StraightMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """void StraightMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    position.y += moveSpeed * deltaTime;
}"""),
    ("HorizontalSweepMovement", "float direction;", """class HorizontalSweepMovement : public IMovementBehavior {
private:
    float direction;
public:
    HorizontalSweepMovement(float startDir);
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """HorizontalSweepMovement::HorizontalSweepMovement(float startDir) : direction(startDir) {}

void HorizontalSweepMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    position.x += direction * moveSpeed * deltaTime;
    if (direction > 0 && position.x > screenWidth + 100) {
        position.x = -100;
    } else if (direction < 0 && position.x < -100) {
        position.x = screenWidth + 100;
    }
}"""),
    ("VerticalZigzagMovement", "", """class VerticalZigzagMovement : public IMovementBehavior {
private:
    float startX;
    bool initialized = false;
    float dx = 1.0f;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """void VerticalZigzagMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    position.y += moveSpeed * 0.3f * deltaTime;
    position.x += dx * moveSpeed * 0.5f * deltaTime;
    if (position.x > startX + 250.0f) {
        dx = -1.0f;
    } else if (position.x < startX - 250.0f) {
        dx = 1.0f;
    }
    if (position.y > screenHeight + 100) {
        position.y = -100;
    }
}"""),
    ("HorizontalBounceMovement", "", """class HorizontalBounceMovement : public IMovementBehavior {
private:
    float startX;
    float drift;
    float direction;
    float targetY;
    bool initialized = false;
public:
    HorizontalBounceMovement(float targetY, float drift = 300.0f, float startDir = 1.0f);
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """HorizontalBounceMovement::HorizontalBounceMovement(float targetY, float drift, float startDir) 
    : targetY(targetY), drift(drift), direction(startDir) {}

void HorizontalBounceMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    if (position.y < targetY) {
        position.y += moveSpeed * deltaTime;
    } else {
        position.x += direction * moveSpeed * deltaTime;
        if (direction > 0 && position.x > startX + drift) {
            direction = -1.0f;
        } else if (direction < 0 && position.x < startX - drift) {
            direction = 1.0f;
        }
    }
}"""),
    ("MeteorDiveMovement", "", """class MeteorDiveMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """void MeteorDiveMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    position.y += moveSpeed * 3.0f * deltaTime;
}"""),
    ("SineZigzagMovement", "", """class SineZigzagMovement : public IMovementBehavior {
private:
    float startX;
    float time = 0.0f;
    float amplitude = 150.0f;
    float frequency = 3.0f;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """void SineZigzagMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    time += deltaTime;
    position.y += moveSpeed * 0.5f * deltaTime;
    position.x = startX + std::sin(time * frequency) * amplitude;
    if (position.y > screenHeight + 100) {
        position.y = -100;
    }
}"""),
    ("SpiralMovement", "", """class SpiralMovement : public IMovementBehavior {
private:
    float time = 0.0f;
    float radius = 0.0f;
    float maxRadius = 250.0f;
    float rotationSpeed = 3.0f;
    Vector2 center;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};""", """void SpiralMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        center = position;
        initialized = true;
    }
    time += deltaTime;
    center.y += moveSpeed * 0.4f * deltaTime;
    if (radius < maxRadius) {
        radius += 50.0f * deltaTime;
    }
    position.x = center.x + std::cos(time * rotationSpeed) * radius;
    position.y = center.y + std::sin(time * rotationSpeed) * radius;
}""")
]

# Write interface
with open(os.path.join(out_dir_inc, "IMovementBehavior.h"), 'w') as f:
    f.write(f"""#ifndef IMOVEMENTBEHAVIOR_H
#define IMOVEMENTBEHAVIOR_H

#include "raylib.h"

{classes[0][2]}

#endif
""")

for cls in classes[1:]:
    name, _, hdr, src = cls
    with open(os.path.join(out_dir_inc, f"{name}.h"), 'w') as f:
        f.write(f"""#ifndef {name.upper()}_H
#define {name.upper()}_H

#include "IMovementBehavior.h"

{hdr}

#endif
""")
    with open(os.path.join(out_dir_src, f"{name}.cpp"), 'w') as f:
        f.write(f"""#include "{name}.h"
#include <cmath>

{src}
""")
