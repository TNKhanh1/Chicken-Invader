import os

with open('src/GameManager.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

content = content.replace('texEnemyAnim = {0};', 'for(int i=0; i<10; i++) texEnemyAnims[i] = {0};')

load_str = '    texEnemyAnim = LoadTexture("assets/enemy/chicken01_anim.png");'
new_load_str = '''    for (int i = 0; i < 10; i++) {
        char path[100];
        snprintf(path, sizeof(path), "assets/enemy/chicken%02d_anim.png", i + 1);
        texEnemyAnims[i] = LoadTexture(path);
    }'''
content = content.replace(load_str, new_load_str)

unload_str = '    UnloadTexture(texEnemyAnim);'
new_unload_str = '    for (int i = 0; i < 10; i++) UnloadTexture(texEnemyAnims[i]);'
content = content.replace(unload_str, new_unload_str)

with open('src/GameManager.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
