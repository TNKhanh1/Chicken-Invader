import os
import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp
import warnings

def generate_warped_sprite_sheet(input_path, output_path, num_frames=24, target_size=200):
    try:
        img = Image.open(input_path).convert("RGBA")
    except FileNotFoundError:
        print(f"Skipping {input_path} (File not found)")
        return

    # Resize source image to target_size x target_size
    img_resized = img.resize((target_size, target_size), Image.LANCZOS)
    arr = np.array(img_resized) / 255.0
    h, w, c = arr.shape

    # Build 11x11 control grid
    cols = np.linspace(0, w, 11)
    rows = np.linspace(0, h, 11)
    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)
    frames = []

    # Custom pivot points for chicken11 geometry
    # Body (head/torso) X: 80 to 120
    # Red wings start at 44 and 157, so pivots must be inwards to flap them!
    left_pivot = 80.0
    right_pivot = 120.0
    leg_start = 130.0
    
    # Scale amplitudes appropriately for 200px
    wing_amp = 30.0
    leg_amp = 16.0
    base_body_dy = 8.0

    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        for i in range(num_frames):
            phase = i * (2 * math.pi) / num_frames
            dip_factor = math.sin(phase)

            body_dy = base_body_dy * dip_factor

            dst_points = []
            for x, y in src_points:
                dx = 0
                dy = body_dy

                # Wing movement pivots exactly at the shoulders (44 and 157)
                if x <= left_pivot:
                    amp = ((left_pivot - x) / left_pivot) * wing_amp
                    dy += amp * dip_factor
                elif x >= right_pivot:
                    amp = ((x - right_pivot) / (w - right_pivot)) * wing_amp
                    dy += amp * dip_factor

                # Leg movement starts below the body (168)
                if y >= leg_start:
                    leg_y_weight = (y - leg_start) / (h - leg_start)
                    mid_x = (left_pivot + right_pivot) / 2.0  # exactly between shoulders
                    if x < mid_x:
                        dx = -leg_amp * dip_factor * leg_y_weight
                    elif x > mid_x:
                        dx = leg_amp * dip_factor * leg_y_weight

                dst_points.append((x + dx, y + dy))

            dst_points = np.array(dst_points)
            tform = PiecewiseAffineTransform()
            tform.estimate(dst_points, src_points)
            
            out = warp(arr, tform, output_shape=(h, w))
            frames.append((out * 255).astype(np.uint8))

    sheet_arr = np.concatenate(frames, axis=1)
    sheet_img = Image.fromarray(sheet_arr, "RGBA")
    sheet_img.save(output_path)
    print(f"Generated: {output_path} ({w*num_frames}x{h})")

if __name__ == "__main__":
    generate_warped_sprite_sheet("assets/enemy/chicken11.png", "assets/enemy/chicken11_anim.png", target_size=200)
