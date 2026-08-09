import os
import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_warped_sprite_sheet(input_path, output_path, num_frames=24):
    try:
        img = Image.open(input_path).convert("RGBA")
    except FileNotFoundError:
        print(f"Skipping {input_path} (File not found)")
        return
        
    arr = np.array(img) / 255.0
    h, w, c = arr.shape
    
    cols = np.linspace(0, w, 11)
    rows = np.linspace(0, h, 11)
    
    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)
    
    frames = []
    left_pivot = w * 0.3
    right_pivot = w * 0.7
    
    for i in range(num_frames):
        phase = i * (2 * math.pi) / num_frames
        dip_factor = math.sin(phase)
        body_dy = (h * 0.04) * dip_factor 
        
        dst_points = []
        for x, y in src_points:
            dx = 0
            dy = body_dy
            
            if x <= left_pivot:
                amp = ((left_pivot - x) / left_pivot) * (h * 0.15)
                dy += amp * dip_factor
            elif x >= right_pivot:
                amp = ((x - right_pivot) / (w - right_pivot)) * (h * 0.15)
                dy += amp * dip_factor
                
            leg_threshold = h * 0.65
            if y >= leg_threshold:
                leg_y_weight = (y - leg_threshold) / (h - leg_threshold) 
                
                if x < w * 0.5:
                    dx = -(w * 0.06) * dip_factor * leg_y_weight
                elif x > w * 0.5:
                    dx = (w * 0.06) * dip_factor * leg_y_weight

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
    in_path = r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken11.png"
    out_path = r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken11_anim.png"
    generate_warped_sprite_sheet(in_path, out_path)
