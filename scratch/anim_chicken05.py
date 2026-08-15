import os
import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_chicken05_anim():
    input_path = 'assets/enemy/chicken05.png'
    output_path = 'assets/enemy/chicken05_anim.png'
    num_frames = 24
    target_size = 100

    print(f"Loading {input_path}...")
    img = Image.open(input_path).convert("RGBA")
    
    # Analyze the bounding box of the original image
    arr_orig = np.array(img)
    alpha_orig = arr_orig[:, :, 3]
    y_idx, x_idx = np.where(alpha_orig > 0)
    
    # Calculate pivots based on actual geometry
    min_x, max_x = np.min(x_idx), np.max(x_idx)
    min_y, max_y = np.min(y_idx), np.max(y_idx)
    width = max_x - min_x
    center_x = min_x + width // 2
    
    # Resize to target
    img_resized = img.resize((target_size, target_size), Image.LANCZOS)
    arr = np.array(img_resized) / 255.0
    h, w, c = arr.shape
    
    # Translate original pivots to resized scale
    scale_x = target_size / img.width
    scale_y = target_size / img.height
    
    # For chicken05, the wings are distinct. We set pivot points to 1/3 and 2/3 of the bounding box.
    left_pivot = (min_x + width * 0.3) * scale_x
    right_pivot = (min_x + width * 0.7) * scale_x
    leg_y = (max_y - (max_y - min_y) * 0.25) * scale_y
    center_scaled_x = center_x * scale_x

    print(f"Calculated Pivots -> Left: {left_pivot:.1f}, Right: {right_pivot:.1f}, Leg Y: {leg_y:.1f}")

    cols = np.linspace(0, w, 15)
    rows = np.linspace(0, h, 15)

    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)

    frames = []

    for i in range(num_frames):
        phase = i * (2 * math.pi) / num_frames
        dip_factor = math.sin(phase)

        body_dy = 4.0 * dip_factor

        dst_points = []
        for x, y in src_points:
            dx = 0
            dy = body_dy

            if x <= left_pivot:
                amp = ((left_pivot - x) / left_pivot) * 15.0
                dy += amp * dip_factor
            elif x >= right_pivot:
                amp = ((x - right_pivot) / (w - right_pivot)) * 15.0
                dy += amp * dip_factor

            if y >= leg_y:
                leg_y_weight = (y - leg_y) / (h - leg_y)
                if x < center_scaled_x:
                    dx = -6.0 * dip_factor * leg_y_weight
                elif x > center_scaled_x:
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
    generate_chicken05_anim()
