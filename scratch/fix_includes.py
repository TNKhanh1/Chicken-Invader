import os
import re

def fix_includes():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    wm_cpp_path = os.path.join(base_dir, "src", "WaveManager.cpp")

    with open(wm_cpp_path, "r", encoding="utf-8") as f:
        content = f.read()

    new_includes = """#include "WaveManager.h"
#include "HorizontalSweepMovement.h"
#include "HorizontalBounceMovement.h"
#include "VerticalZigzagMovement.h"
#include "SineZigzagMovement.h"
#include "MeteorDiveMovement.h"
#include "StraightMovement.h"
"""

    content = re.sub(r'#include\s+"WaveManager\.h"', new_includes, content)

    with open(wm_cpp_path, "w", encoding="utf-8") as f:
        f.write(content)

    print("Added includes to WaveManager.cpp")

if __name__ == "__main__":
    fix_includes()
