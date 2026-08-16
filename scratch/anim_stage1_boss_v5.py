import numpy as np
from PIL import Image
import math
from skimage.transform import PiecewiseAffineTransform, warp
import os

src_path = "scratch/chicken10_main.png"
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

# pivots
left_pivot = min_x + actual_width * 0.35
right_pivot = max_x - actual_width * 0.35
leg_y_threshold = min_y + actual_height * 0.65
head_y_threshold = min_y + actual_height * 0.25 # Khoảng y < 186

print(f"Calculated Pivots -> Left: {left_pivot:.1f}, Right: {right_pivot:.1f}, Leg Y: {leg_y_threshold:.1f}, Head Y: {head_y_threshold:.1f}")

frames = 24
output_w = 340
output_h = 340
spritesheet = Image.new("RGBA", (output_w * frames, output_h), (0, 0, 0, 0))

cols = 16
rows = 16
src_points = []
for r in range(rows + 1):
    for c in range(cols + 1):
        x = c * (w / cols)
        y = r * (h / rows)
        src_points.append([x, y])
src_points = np.array(src_points)

for f in range(frames):
    progress = f / frames
    cycle = math.sin(progress * math.pi * 2) 
    
    max_wing_dy = 120.0 * cycle
    
    dst_points = np.copy(src_points)
    
    for i, (x, y) in enumerate(src_points):
        # Wing animation
        if x < left_pivot:
            factor = (left_pivot - x) / left_pivot
            dst_points[i, 1] += factor * factor * max_wing_dy
        elif x > right_pivot:
            factor = (x - right_pivot) / (w - right_pivot)
            dst_points[i, 1] += factor * factor * max_wing_dy
            
        # Leg animation
        if y > leg_y_threshold:
            leg_factor = (y - leg_y_threshold) / (h - leg_y_threshold)
            if x < w/2:
                dst_points[i, 0] += math.sin(progress * math.pi * 2) * 15.0 * leg_factor
                dst_points[i, 1] -= math.cos(progress * math.pi * 2) * 8.0 * leg_factor
            else:
                dst_points[i, 0] -= math.sin(progress * math.pi * 2) * 15.0 * leg_factor
                dst_points[i, 1] -= math.cos(progress * math.pi * 2) * 8.0 * leg_factor

        # Head Flame sway animation
        if y < head_y_threshold and min_x + actual_width * 0.4 < x < min_x + actual_width * 0.6:
            # Ngọn lửa trên đầu
            flame_factor = (head_y_threshold - y) / (head_y_threshold - min_y)
            # Thay đổi theo chu kỳ vỗ cánh (math.pi * 2 thay vì pi * 4) và tăng biên độ (35.0 thay vì 20.0)
            flame_cycle = math.sin(progress * math.pi * 2) 
            dst_points[i, 0] += flame_cycle * 35.0 * flame_factor * flame_factor

    tform = PiecewiseAffineTransform()
    tform.estimate(dst_points, src_points)
    
    warped_arr = warp(src_arr, tform, output_shape=(h, w), mode='constant', cval=0, preserve_range=True)
    warped_arr = warped_arr.astype(np.uint8)
    warped_img = Image.fromarray(warped_arr, 'RGBA')
    
    w_alpha = warped_arr[:, :, 3]
    w_row_sums = np.sum(w_alpha > 0, axis=1)
    w_col_sums = np.sum(w_alpha > 0, axis=0)
    
    if np.sum(w_alpha) > 0:
        w_nz_rows = np.where(w_row_sums > 0)[0]
        w_nz_cols = np.where(w_col_sums > 0)[0]
        bbox_min_x, bbox_max_x = w_nz_cols[0], w_nz_cols[-1]
        bbox_min_y, bbox_max_y = w_nz_rows[0], w_nz_rows[-1]
        
        pad = 20
        c_min_x = max(0, bbox_min_x - pad)
        c_min_y = max(0, bbox_min_y - pad)
        c_max_x = min(w, bbox_max_x + pad)
        c_max_y = min(h, bbox_max_y + pad)
        
        cropped = warped_img.crop((c_min_x, c_min_y, c_max_x, c_max_y))
        
        cw, ch = cropped.size
        scale = min(320 / cw, 320 / ch)
        new_cw, new_ch = int(cw * scale), int(ch * scale)
        cropped = cropped.resize((new_cw, new_ch), Image.Resampling.LANCZOS)
        
        frame_canvas = Image.new("RGBA", (output_w, output_h), (0, 0, 0, 0))
        paste_x = (output_w - new_cw) // 2
        paste_y = (output_h - new_ch) // 2
        frame_canvas.paste(cropped, (paste_x, paste_y), cropped)
    else:
        frame_canvas = Image.new("RGBA", (output_w, output_h), (0, 0, 0, 0))

    spritesheet.paste(frame_canvas, (f * output_w, 0))

out_path = "assets/enemy/chicken10_anim.png"
spritesheet.save(out_path)
print(f"Generated: {out_path} ({spritesheet.width}x{spritesheet.height})")
