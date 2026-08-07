import os
import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_warped_sprite_sheet(input_path, output_path, num_frames=24, target_size=100):
    """
    Generate animation sprite sheet that is CONSISTENT with chicken01.
    All outputs are target_size x target_size per frame (default 100x100).
    The warp math uses absolute pixel values calibrated to 100x100 scale,
    matching warp_anim_v2.py exactly.
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

    # Step 2: Build 11x11 control grid (same as warp_anim_v2.py)
    cols = np.linspace(0, w, 11)
    rows = np.linspace(0, h, 11)

    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)

    frames = []

    # Step 3: Use SAME absolute pivot values as chicken01 (warp_anim_v2.py)
    left_pivot = 30.0
    right_pivot = 70.0

    for i in range(num_frames):
        phase = i * (2 * math.pi) / num_frames
        dip_factor = math.sin(phase)

        # Same body dip as chicken01: 4.0 pixels
        body_dy = 4.0 * dip_factor

        dst_points = []
        for x, y in src_points:
            dx = 0
            dy = body_dy

            # Wing movement: same 15.0 pixel amplitude as chicken01
            if x <= left_pivot:
                amp = ((left_pivot - x) / left_pivot) * 15.0
                dy += amp * dip_factor
            elif x >= right_pivot:
                amp = ((x - right_pivot) / (w - right_pivot)) * 15.0
                dy += amp * dip_factor

            # Leg movement: same 6.0 pixel amplitude as chicken01
            if y >= 65:
                leg_y_weight = (y - 65) / (h - 65)
                if x < 50:
                    dx = -6.0 * dip_factor * leg_y_weight
                elif x > 50:
                    dx = 6.0 * dip_factor * leg_y_weight

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
    base_dir = r"e:\ChickenInvader_local\ChickenInvader\assets\enemy"
    # Regenerate chicken02 through chicken10 with synchronized animation
    for i in range(2, 11):
        filename = f"chicken{i:02d}.png"
        in_path = os.path.join(base_dir, filename)
        out_path = os.path.join(base_dir, f"chicken{i:02d}_anim.png")
        generate_warped_sprite_sheet(in_path, out_path)
