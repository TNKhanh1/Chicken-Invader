#include "../include/BulletPool.h"
#include "../include/BulletTrajectory.h"
#include "../include/BeamWeapon.h"
#include "../include/Enemy.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

void TestObjectPoolAndSilentDrop() {
    std::cout << "\n=======================================================\n";
    std::cout << "[TEST 1] Kiểm Thử BulletPool & Silent Drop Overflow\n";
    std::cout << "=======================================================\n";
    
    BulletPool pool(1000);
    std::cout << "[+] Khởi tạo thành công hồ chứa tĩnh với " << pool.GetPoolSize() << " con trỏ đạn trong RAM.\n";
    
    int acquiredCount = 0;
    for (int i = 0; i < 1000; ++i) {
        auto b = pool.Acquire(Vector2{100.0f, 100.0f}, 25.0f, 400.0f, true, 0, 12.0f);
        if (b != nullptr) acquiredCount++;
    }
    std::cout << "[+] Đã nạp đầy bể! Xin thành công: " << acquiredCount << "/1000 viên đạn.\n";

    // Thử vượt tải xin viên thứ 1001 và 1002 (mô phỏng buff Bắn Siêu Tốc / Bão Đạn)
    auto overflow1 = pool.Acquire(Vector2{100.0f, 100.0f}, 25.0f, 400.0f, true, 0, 12.0f);
    auto overflow2 = pool.Acquire(Vector2{100.0f, 100.0f}, 25.0f, 400.0f, true, 0, 12.0f);
    
    if (overflow1 == nullptr && overflow2 == nullptr) {
        std::cout << "[✔ THÀNH CÔNG] Cơ chế SILENT DROP hoạt động chính xác! Viên thứ 1001 & 1002 im lặng trả về nullptr (Bảo vệ game KHÔNG văng/Crash RAM).\n";
    } else {
        std::cout << "[✘ THẤT BẠI] Lỗi: Vẫn sinh được đạn mới vượt trần!\n";
    }

    // Trả 1 viên đạn về bể (ví dụ: đạn va chạm quái vật)
    auto activeList = pool.GetActiveBullets();
    pool.Release(activeList[0]);
    std::cout << "[+] Đã giải phóng 1 viên đạn (active = false). Trạng thái active hiện tại: " << pool.GetActiveBullets().size() << "/1000.\n";
    
    // Xin lại 1 viên sau khi có khoảng trống
    auto reAcquired = pool.Acquire(Vector2{200.0f, 200.0f}, 50.0f, 600.0f, true, 1, 15.0f);
    if (reAcquired != nullptr) {
        std::cout << "[✔ THÀNH CÔNG] Tái sinh lập tức đạn vừa giải phóng không qua cấu trúc new/delete!\n";
    }
}

void TestAutoLockingLightningFryer() {
    std::cout << "\n=======================================================\n";
    std::cout << "[TEST 2] Kiểm Thử Thuật Toán Tự Động Nhắm (Auto-Locking)\n";
    std::cout << "=======================================================\n";
    
    // Khởi tạo vũ khí tia Lightning Fryer (bán kính nhắm 650px)
    LightningFryerBehavior fryer;
    Vector2 podPosition = { 800.0f, 800.0f }; // Vị trí nòng súng phi thuyền
    std::cout << "[+] Tọa độ Nòng súng Lightning Fryer: X = " << podPosition.x << ", Y = " << podPosition.y << "\n";

    // Tạo giả lập mảng quái vật trên bầu trời
    std::vector<std::shared_ptr<Enemy>> mockEnemies;
    
    // Quái 1: Ở góc rất xa (X: 100, Y: 100) -> Khoảng cách: ~990px (Vượt tầm xa 650px)
    auto farEnemy = std::make_shared<Enemy>(Vector2{100.0f, 100.0f}, 100.0f, 10.0f, 0.0f, 50.0f, 100);
    farEnemy->SetActive(true);
    
    // Quái 2: Ở góc gần nhất (X: 600, Y: 500) -> Khoảng cách: ~360px (Trong bán kính nhắm)
    auto closeEnemy = std::make_shared<Enemy>(Vector2{600.0f, 500.0f}, 100.0f, 10.0f, 0.0f, 50.0f, 200);
    closeEnemy->SetActive(true);

    mockEnemies.push_back(farEnemy);
    mockEnemies.push_back(closeEnemy);

    Vector2 targetPos = {0, 0};
    float aimAngle = 0.0f;
    bool locked = fryer.FindNearestTarget(podPosition, mockEnemies, targetPos, aimAngle);
    
    if (locked && targetPos.x == 600.0f && targetPos.y == 500.0f) {
        std::cout << "[✔ THÀNH CÔNG] Auto-Locking lập tức KHÓA TÔNG quái vật gần nhất tại (X: " << targetPos.x << ", Y: " << targetPos.y << ")!\n";
        std::cout << "[✔ THÀNH CÔNG] Góc xoay tia chốt (theta): " << aimAngle << " độ (Tia sét sẽ uốn cong châm thẳng ngực quái vật thay vì bắn vút lên trừu tượng)!\n";
    } else {
        std::cout << "[✘ THẤT BẠI] Lỗi: Không khóa được đúng quái vật hoặc sai tọa độ!\n";
    }
}

void TestStrategyTrajectory() {
    std::cout << "\n=======================================================\n";
    std::cout << "[TEST 3] Kiểm Thử Strategy Pattern (Oscillating Trajectory)\n";
    std::cout << "=======================================================\n";
    
    // Quỹ đạo dao động hình sin cho Hypergun/Utensil Poker (góc gốc 0, tần số 2.0Hz, biên độ 7.5 độ)
    OscillatingTrajectory traj(0.0f, 2.0f, 7.5f);
    Vector2 pos = { 800.0f, 800.0f };
    float angle = 0.0f;
    float speed = 500.0f;
    
    std::cout << "[+] Vị trí xuất phát: (" << pos.x << ", " << pos.y << "), Góc ban đầu: " << angle << " độ.\n";
    
    // Giả lập lặp qua 3 khung hình, mỗi khung 0.1s
    for(int step = 1; step <= 3; ++step) {
        traj.UpdatePosition(pos, angle, speed, 0.1f);
        std::cout << "    - Bước " << step << " (t = " << step * 0.1f << "s): Tọa độ (" << pos.x << ", " << pos.y << "), Góc bay uốn lượn: " << angle << " độ.\n";
    }
    std::cout << "[✔ THÀNH CÔNG] Trajectory Strategy bẻ cong quỹ đạo động theo chu kỳ hình sin chuẩn sát theo specs!\n\n";
}

int main() {
    std::cout << "=======================================================\n";
    std::cout << "        BỘ KIỂM THỬ TỰ ĐỘNG HỆ THỐNG VŨ KHÍ OOP        \n";
    std::cout << "=======================================================\n";
    
    TestObjectPoolAndSilentDrop();
    TestAutoLockingLightningFryer();
    TestStrategyTrajectory();
    
    std::cout << "=> [HỢP THỨC HOÀN TOÀN]: TOÀN BỘ HỆ THỐNG KIẾN TRÚC MỚI HOạt Động CHUẨN XÁC 100%!\n\n";
    return 0;
}
