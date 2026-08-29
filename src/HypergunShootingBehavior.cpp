#include "HypergunShootingBehavior.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "GameManager.h"
#include "json.hpp"
#include <fstream>
#include <cmath>
#include <iostream>

using json = nlohmann::json;

HypergunShootingBehavior::HypergunShootingBehavior() 
    : shotCount(0), frontAngleIndex(0), rearTimer(0), wingTimer(0) {
    LoadWeaponPods("assets/spaceship/hypergun.json");
}

void HypergunShootingBehavior::LoadWeaponPods(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        try {
            json j;
            file >> j;
            if (j.contains("pods")) {
                for (auto& el : j["pods"].items()) {
                    weaponPods[el.key()] = {el.value()["x"].get<float>(), el.value()["y"].get<float>()};
                }
            }
        } catch (std::exception& e) {
            std::cerr << "Failed to parse " << filepath << ": " << e.what() << std::endl;
        }
    }
    
    // Default values if not loaded
    if (weaponPods.empty()) {
        weaponPods["tip"] = {0.0f, -50.0f};
        weaponPods["front_left"] = {-15.0f, -20.0f};
        weaponPods["front_right"] = {15.0f, -20.0f};
        weaponPods["rear_left"] = {-30.0f, 10.0f};
        weaponPods["rear_right"] = {30.0f, 10.0f};
        weaponPods["wing_left"] = {-45.0f, 20.0f};
        weaponPods["wing_right"] = {45.0f, 20.0f};
    }
}

void HypergunShootingBehavior::Shoot(Spaceship* ship) {
    if (!ship) return;
    
    int level = ship->GetLevel();
    float dmg = ship->GetDamage();
    Vector2 shipPos = ship->GetPosition();
    float speed = 600.0f; // Tốc độ đạn Hypergun khá nhanh

    float pattern[] = {0.0f, 1.875f, 3.75f, 1.875f, 0.0f, -1.875f, -3.75f, -1.875f};
    float currentFrontAngle = pattern[frontAngleIndex % 8];
    frontAngleIndex++;

    shotCount++;
    bool shootRear = (shotCount % 2 == 0); // 2x slower
    bool shootWing = (shotCount % 4 == 0); // 4x slower

    auto spawnBullet = [&](const std::string& podName, int type, float angleDeg) {
        if (weaponPods.find(podName) == weaponPods.end()) return;
        
        Vector2 offset = weaponPods[podName];
        Vector2 startPos = {shipPos.x + offset.x, shipPos.y + offset.y};
        
        float angleRad = (angleDeg - 90.0f) * (PI / 180.0f); // -90 vì hướng lên trên là -90 độ trong raylib/toán học 2D
        Vector2 velocity = {std::cos(angleRad) * speed, std::sin(angleRad) * speed};
        
        float finalDmg = type == 1 ? dmg * 1.5f : dmg * 0.5f; // Strong = 1.5x, Weak = 0.5x
        auto bullet = std::make_shared<Bullet>(startPos, finalDmg, speed, true, type);
        bullet->SetVelocity(velocity);
        bullet->SetShooter(ship);
        GameManager::GetInstance()->AddBullet(bullet);
    };

    auto gm = GameManager::GetInstance();
    
    switch (level) {
        case 0:
            spawnBullet("tip", 1, 0.0f);
            break;
        case 1:
            spawnBullet("tip", 2, 0.0f);
            spawnBullet("front_left", 2, -currentFrontAngle);
            spawnBullet("front_right", 2, currentFrontAngle);
            break;
        case 2:
            spawnBullet("tip", 1, 0.0f);
            spawnBullet("front_left", 2, -currentFrontAngle);
            spawnBullet("front_right", 2, currentFrontAngle);
            break;
        case 3:
            spawnBullet("tip", 2, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            break;
        case 4:
            spawnBullet("tip", 1, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            break;
        case 5:
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            spawnBullet("tip", 2, 0.0f);
            if (shootRear) {
                spawnBullet("rear_left", 2, -currentFrontAngle);
                spawnBullet("rear_right", 2, currentFrontAngle);
            }
            break;
        case 6:
            spawnBullet("tip", 1, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            if (shootRear) {
                spawnBullet("rear_left", 2, -currentFrontAngle);
                spawnBullet("rear_right", 2, currentFrontAngle);
            }
            break;
        case 7:
            spawnBullet("tip", 2, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            if (shootRear) {
                spawnBullet("rear_left", 1, -currentFrontAngle);
                spawnBullet("rear_right", 1, currentFrontAngle);
            }
            break;
        case 8:
            spawnBullet("tip", 1, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            if (shootRear) {
                spawnBullet("rear_left", 1, -currentFrontAngle);
                spawnBullet("rear_right", 1, currentFrontAngle);
            }
            break;
        case 9:
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            spawnBullet("tip", 2, 0.0f);
            if (shootRear) {
                spawnBullet("rear_left", 1, -currentFrontAngle);
                spawnBullet("rear_right", 1, currentFrontAngle);
            }
            if (shootWing) {
                spawnBullet("wing_left", 2, -currentFrontAngle);
                spawnBullet("wing_right", 2, currentFrontAngle);
            }
            break;
        case 10:
            spawnBullet("tip", 1, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            if (shootRear) {
                spawnBullet("rear_left", 1, -currentFrontAngle);
                spawnBullet("rear_right", 1, currentFrontAngle);
            }
            if (shootWing) {
                spawnBullet("wing_left", 2, -currentFrontAngle);
                spawnBullet("wing_right", 2, currentFrontAngle);
            }
            break;
        case 11:
        default:
            spawnBullet("tip", 1, 0.0f);
            spawnBullet("front_left", 1, -currentFrontAngle);
            spawnBullet("front_right", 1, currentFrontAngle);
            if (shootRear) {
                spawnBullet("rear_left", 1, -currentFrontAngle);
                spawnBullet("rear_right", 1, currentFrontAngle);
            }
            if (shootWing) {
                spawnBullet("wing_left", 1, -currentFrontAngle);
                spawnBullet("wing_right", 1, currentFrontAngle);
            }
            break;
    }
    
    gm->PlayShootSound();
}
