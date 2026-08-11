# Implementation Plan: Stage 2 Wave 10 - Kẻ Hành Quyết Sinh Đôi (Dual Eggsecutioner Boss)

## Mục Tiêu
- **Kẻ thù chủ đạo:** 2 Boss `EggsecutionerBoss` (`visual_id: 11`).
- **Cơ chế đặc trưng:** Hai con Boss sẽ phối hợp bay lượn trên toàn bản đồ với chu kỳ di chuyển **5 giây/lần**.
- **Yêu cầu kỹ thuật:** Áp dụng hệ thống **"Traffic Light Yielding" (Nhường đường tại Giao lộ)** sử dụng thuật toán quét va chạm tương lai (Swept Collision Prediction) thay vì đẩy nhau thô bạo.

---

## 1. Cơ chế phối hợp Tấn công (Tag-team)
Thay vì cả 2 con Boss cùng lao vào phi thuyền một lúc gây cảm giác lộn xộn, chúng được trang bị AI để phân chia nhiệm vụ theo chu kỳ 5 giây:
- **Xác định mục tiêu (Alternating Target):** Dựa vào biến đếm toàn cục `battleTime`, cứ mỗi 5 giây, hệ thống sẽ chỉ định ngẫu nhiên 1 trong 2 con Boss làm **"Người đi săn"**.
- **Người đi săn (The Hunter):** Con Boss được chọn sẽ ưu tiên khóa mục tiêu bay tới vùng lân cận của phi thuyền người chơi (nhỉnh lên trên một chút để không gây chết oan ức), ép người chơi phải di chuyển và né đạn liên tục.
- **Kẻ bọc lót (The Flanker):** Con Boss còn lại sẽ ngay lập tức bay ra xa khỏi phi thuyền (nếu phi thuyền ở bên trái, nó bay sang bên phải). Điều này tạo ra đội hình chiến thuật hình chữ V, bủa vây người chơi từ 2 hướng. Và quan trọng nhất, nó giúp hạn chế tối đa việc 2 con Boss cản đường nhau!

---

## 2. Hệ thống Nhường đường (Swept Collision AI)

Mặc dù đã cố gắng tránh nhau bằng chiến thuật bọc lót, nhưng đôi khi quỹ đạo của chúng vẫn sẽ giao nhau. AI sẽ giải quyết theo các bước sau:

**Bước 2.1: Quét Tương Lai (Collision Prediction)**
Trong mỗi khung hình, mỗi con Boss tự mô phỏng đường bay của chính nó và đường bay của Boss kia **lên tới 3 giây trong tương lai**. Nếu khoảng cách mô phỏng của chúng ở bất kỳ thời điểm nào nhỏ hơn `280px` (đủ rộng để bao phủ toàn bộ cánh, đầu, chân), hệ thống sẽ phát tín hiệu cảnh báo Xung Đột.

**Bước 2.2: Phanh Gấp Nhường Đường (Yielding)**
- Khi phát hiện xung đột, luật giao thông sẽ ưu tiên con Boss có địa chỉ bộ nhớ nhỏ hơn (`this < otherBoss`) được quyền bay tiếp.
- Con Boss bị mất quyền ưu tiên sẽ lập tức bật đèn đỏ (`isYielding = true`), **Phanh cháy đường và đứng bất động trên không trung**. Động cơ đẩy bị ngắt hoàn toàn.
- Tuy bị dừng bay, nhưng các ụ súng trên thân Boss vẫn hoạt động 100% công suất! Nó sẽ trở thành một lô cốt bắn tỉa trong khi nhường đường cho đồng đội bay qua. 

**Bước 2.3: Bẻ Lái Khẩn Cấp (Emergency Rerouting)**
Trong lúc con Boss A đang đứng yên nhường đường, nếu đường bay của con Boss B lại vô tình đâm thẳng vào Boss A, Boss B sẽ lập tức phát hiện "Vật cản tĩnh" và tự động hủy đường bay, chọn một tọa độ mới an toàn hơn.

**Bước 2.4: Trường Lực Từ Tính (Hard Repulsion)**
Lớp bảo vệ cuối cùng! Nếu vì bất kỳ lỗi gì mà chúng tiến sát nhau dưới mức `260px`, một trường lực từ tính sẽ ngay lập tức được kích hoạt, đẩy bật cả 2 ra xa nhau (Push Force). Điều này đảm bảo tuyệt đối hai lớp Sprite của chúng không bao giờ đè lên nhau, giữ vững trải nghiệm thị giác.

---

## 3. Khởi tạo Không đồng bộ (Desynchronization)
Để tránh hiện tượng 2 con Boss "múa" đồng điệu như robot lúc mới sinh ra:
- **Thời gian khai hỏa (`attackTimer`)**: Mỗi con được bù một độ trễ bắn đạn ngẫu nhiên từ `0.0s` đến `2.0s`. Chùm tia Laser đỏ và Bom dội sẽ không bao giờ được xả ra cùng một lúc.
- **Quỹ đạo khởi đầu**: Ngay khi vào màn, mỗi con sẽ lao tới một góc ngẫu nhiên thay vì lao thẳng xuống dưới, tạo ra một trận chiến hỗn loạn ngay từ giây phút đầu tiên.

## User Review Required
> [!IMPORTANT]
> - Thiết kế này đã được hiện thực hóa 100% vào trong code và loại bỏ hoàn toàn các lỗi như "Tốc biến" (Dashing) hay "Đứng yên lờ đờ" (State Hovering cũ).
> - File này đã được cập nhật thành cấu hình chuẩn của hệ thống để lưu trữ cho các bản cập nhật sau.
