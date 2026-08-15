import numpy as np
from PIL import Image
import math
from skimage.transform import PiecewiseAffineTransform, warp
import os

src_path = "assets/enemy/chicken06.png"
if not os.path.exists(src_path):
    print(f"Error: Could not find {src_path}")
    exit(1)

print(f"Loading {src_path}...")
img = Image.open(src_path).convert("RGBA")
w, h = img.size
src_arr = np.array(img)

alpha = src_arr[:, :, 3]
col_sums = np.sum(alpha > 0, axis=0)
non_zero_cols = np.where(col_sums > 0)[0]

min_x = non_zero_cols[0]
max_x = non_zero_cols[-1]
actual_width = max_x - min_x

row_sums = np.sum(alpha > 0, axis=1)
non_zero_rows = np.where(row_sums > 0)[0]
min_y = non_zero_rows[0]
max_y = non_zero_rows[-1]
actual_height = max_y - min_y

# Tinh chỉnh lại ranh giới thân và cánh
# Thân gà thường hẹp hơn phần cánh vươn ra.
left_pivot = min_x + actual_width * 0.30
right_pivot = max_x - actual_width * 0.30

# Chân gà thường chiếm khoảng 25-30% chiều cao dưới cùng đối với các mẫu gà đặc biệt
leg_y = max_y - actual_height * 0.25

print(f"Calculated Pivots -> Left: {left_pivot:.1f}, Right: {right_pivot:.1f}, Leg Y: {leg_y:.1f}")

frames = 24
output_w = 100
output_h = 100
spritesheet = Image.new("RGBA", (output_w * frames, output_h), (0, 0, 0, 0))

# Tăng cường độ phân giải của mesh grid để làm mượt chuyển động uốn éo
cols = 16
rows = 16
src_points = []
for r in range(rows + 1):
    for c in range(cols + 1):
        x = c * (w / cols)
        y = r * (h / rows)
        src_points.append([x, y])
src_points = np.array(src_points)

center_x = w / 2.0

for f in range(frames):
    progress = f / frames
    cycle = math.sin(progress * math.pi * 2) 
    
    # Tăng biên độ chuyển động cánh (wing_dy) để cánh đập mạnh hơn
    max_wing_dy = 28.0 * cycle
    # Chân đưa lên thụt xuống
    max_leg_dy = -8.0 * cycle  
    # Chân khép vào trong (inward)
    max_leg_dx = 10.0 * cycle
    
    dst_points = np.copy(src_points)
    
    for i, (x, y) in enumerate(src_points):
        # 1. Chuyển động cánh
        if x < left_pivot:
            factor = (left_pivot - x) / left_pivot
            # Càng ra xa cánh càng đập mạnh
            dst_points[i, 1] += factor * factor * max_wing_dy
        elif x > right_pivot:
            factor = (x - right_pivot) / (w - right_pivot)
            dst_points[i, 1] += factor * factor * max_wing_dy
            
        # 2. Chuyển động chân (Khép vào trong)
        if y > leg_y:
            factor = (y - leg_y) / (h - leg_y)
            # Chuyển động theo trục y
            dst_points[i, 1] += factor * max_leg_dy
            
            # Khép chân vào trong (hướng về center_x)
            if x < center_x:
                dst_points[i, 0] += factor * max_leg_dx  # Chân trái dịch sang phải
            else:
                dst_points[i, 0] -= factor * max_leg_dx  # Chân phải dịch sang trái

    tform = PiecewiseAffineTransform()
    tform.estimate(dst_points, src_points)
    
    warped_arr = warp(src_arr, tform, output_shape=(h, w), mode='constant', cval=0, preserve_range=True)
    warped_arr = warped_arr.astype(np.uint8)
    warped_img = Image.fromarray(warped_arr, 'RGBA')
    
    # Canh giữa khung hình
    w_alpha = warped_arr[:, :, 3]
    w_row_sums = np.sum(w_alpha > 0, axis=1)
    w_col_sums = np.sum(w_alpha > 0, axis=0)
    
    if np.sum(w_alpha) > 0:
        w_nz_rows = np.where(w_row_sums > 0)[0]
        w_nz_cols = np.where(w_col_sums > 0)[0]
        bbox_min_x, bbox_max_x = w_nz_cols[0], w_nz_cols[-1]
        bbox_min_y, bbox_max_y = w_nz_rows[0], w_nz_rows[-1]
        
        cropped = warped_img.crop((bbox_min_x, bbox_min_y, bbox_max_x + 1, bbox_max_y + 1))
        
        cw, ch = cropped.size
        # Giữ max_size là 96 để tránh bị cắt lẹm lông
        scale = min(96 / cw, 96 / ch)
        new_cw, new_ch = int(cw * scale), int(ch * scale)
        cropped = cropped.resize((new_cw, new_ch), Image.Resampling.LANCZOS)
        
        frame_canvas = Image.new("RGBA", (100, 100), (0, 0, 0, 0))
        paste_x = (100 - new_cw) // 2
        paste_y = (100 - new_ch) // 2
        frame_canvas.paste(cropped, (paste_x, paste_y), cropped)
    else:
        frame_canvas = Image.new("RGBA", (100, 100), (0, 0, 0, 0))

    spritesheet.paste(frame_canvas, (f * 100, 0))

out_path = "assets/enemy/chicken06_anim.png"
spritesheet.save(out_path)
print(f"Generated: {out_path} ({spritesheet.width}x{spritesheet.height})")
