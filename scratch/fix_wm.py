import os
with open(r'e:\ChickenInvader_local\ChickenInvader\src\WaveManager.cpp', 'r', encoding='utf-8') as f:
    c = f.read()
if '#include "Spaceship.h"' not in c:
    c = c.replace('#include "GameManager.h"', '#include "GameManager.h"\n#include "Spaceship.h"')
    with open(r'e:\ChickenInvader_local\ChickenInvader\src\WaveManager.cpp', 'w', encoding='utf-8') as f:
        f.write(c)
