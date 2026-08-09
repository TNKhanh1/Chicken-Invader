import os
import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_warped_sprite_sheet(input_path, output_path, num_frames=24, target_size=100):
    """
    Generate animation sprite sheet that is CONSISTENT with chicken01.
    All outputs are target_size x target_size per frame.
    The warp math dynamically scales the pivot points based on target_size.
    """
    try:
        img = Image.open(input_path).convert("RGBA")
    except FileNotFoundError:
        print(f"Skipping {input_path} (File not found)")
        return

    # Step 1: Resize source image to target_size x target_size
    img_resized = img.resize((target_size, target_size), Image.LANCZOS)
    arr = np.array(img_resized) / 255.0
    h, w, c = arr.shape  # h = w = target_size

    # Step 2: Build 11x11 control grid
    cols = np.linspace(0, w, 11)
    rows = np.linspace(0, h, 11)

    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)

    frames = []

    # Step 3: Scale constants based on target_size (baseline 100x100)
    scale = target_size / 100.0
    left_pivot = 30.0 * scale
    right_pivot = 70.0 * scale
    wing_amp = 15.0 * scale
    leg_start = 65.0 * scale
    leg_amp = 6.0 * scale
    base_body_dy = 4.0 * scale

    for i in range(num_frames):
        phase = i * (2 * math.pi) / num_frames
        dip_factor = math.sin(phase)

        body_dy = base_body_dy * dip_factor

        dst_points = []
        for x, y in src_points:
            dx = 0
            dy = body_dy

            # Wing movement
            if x <= left_pivot:
                amp = ((left_pivot - x) / left_pivot) * wing_amp
                dy += amp * dip_factor
            elif x >= right_pivot:
                amp = ((x - right_pivot) / (w - right_pivot)) * wing_amp
                dy += amp * dip_factor

            # Leg movement
            if y >= leg_start:
                leg_y_weight = (y - leg_start) / (h - leg_start)
                mid_x = 50.0 * scale
                if x < mid_x:
                    dx = -leg_amp * dip_factor * leg_y_weight
                elif x > mid_x:
                    dx = leg_amp * dip_factor * leg_y_weight

            dst_points.append((x + dx, y + dy))

        dst_points = np.array(dst_points)
        tform = PiecewiseAffineTransform()
        # Suppress deprecation warning by using from_estimate if available, else estimate
        if hasattr(tform, 'from_estimate'):
            tform = PiecewiseAffineTransform.from_estimate(src_points, dst_points)
            out = warp(arr, tform.inverse, output_shape=(h, w))
        else:
            tform.estimate(dst_points, src_points)
            out = warp(arr, tform, output_shape=(h, w))
        
        frames.append((out * 255).astype(np.uint8))

    sheet_arr = np.concatenate(frames, axis=1)
    sheet_img = Image.fromarray(sheet_arr, "RGBA")
    sheet_img.save(output_path)
    print(f"Generated: {output_path} ({w*num_frames}x{h})")

if __name__ == "__main__":
    generate_warped_sprite_sheet("assets/enemy/chicken11.png", "assets/enemy/chicken11_anim.png", target_size=200)
