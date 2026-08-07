import os

def update_enemy_cpp():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    enemy_cpp = os.path.join(base_dir, "src", "Enemy.cpp")

    with open(enemy_cpp, "r", encoding="utf-8") as f:
        content = f.read()

    # Update constructor
    old_ctor = """Enemy::Enemy(Vector2 pos, float hp, float dmg, float arm, float spd, int points)
    : Character(pos, hp, dmg, arm, spd), pointValue(points) {"""
    
    new_ctor = """Enemy::Enemy(int vId, EnemyRole r, const EnemyStats& s, Vector2 pos)
    : Character(pos, s.hp, s.damage, s.armor, s.speed), pointValue(s.score) {
    visualId = vId;
    role = r;
    stats = s;"""
    
    content = content.replace(old_ctor, new_ctor)
    
    # Update baseSizeForType
    old_base_size = """float Enemy::baseSizeForType() const {
    if (enemyType == 1) return 50.0f;  // SWARM
    if (enemyType == 2) return 130.0f; // TANK
    return 80.0f;                      // NORMAL
}"""
    
    new_base_size = """float Enemy::baseSizeForType() const {
    if (role == EnemyRole::SWARM) return 50.0f;
    if (role == EnemyRole::TANK) return 130.0f;
    if (role == EnemyRole::BOSS) return 250.0f;
    return 80.0f; // NORMAL
}"""
    
    content = content.replace(old_base_size, new_base_size)
    
    # Update Draw() check for asteroid
    # old: if (enemyType == 4) { // ASTEROID
    # new: if (role == EnemyRole::ASTEROID) {
    content = content.replace("if (enemyType == 4) { // ASTEROID", "if (role == EnemyRole::ASTEROID) {")
    content = content.replace("if (enemyType == 4)", "if (role == EnemyRole::ASTEROID)")

    with open(enemy_cpp, "w", encoding="utf-8") as f:
        f.write(content)

if __name__ == "__main__":
    update_enemy_cpp()
