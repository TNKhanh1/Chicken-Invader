import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_warped_sprite_sheet(input_path, output_path, num_frames=12):
    img = Image.open(input_path).convert("RGBA")
    arr = np.array(img) / 255.0  # normalize for skimage

    h, w, c = arr.shape
    
    # 9x9 grid gives 8x8 cells for fine control
    cols = np.linspace(0, w, 11)
    rows = np.linspace(0, h, 11)
    
    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)
    
    frames = []
    
    # Pivot points for wings
    left_pivot = 30.0
    right_pivot = 70.0
    
    for i in range(num_frames):
        # phase goes from 0 to 2pi
        phase = i * (2 * math.pi) / num_frames
        
        # dip_factor: 1.0 = Max Out & Down, -1.0 = Max In & Up
        dip_factor = math.sin(phase)
        
        # Global body dip
        body_dy = 4.0 * dip_factor
        
        dst_points = []
        for x, y in src_points:
            dx = 0
            dy = body_dy # All points move with the body initially
            
            # Wing movement (Whole wing, hinging at pivots)
            if x <= left_pivot:
                # amplitude increases linearly from pivot to 0
                amp = ((left_pivot - x) / left_pivot) * 15.0
                dy += amp * dip_factor
            elif x >= right_pivot:
                # amplitude increases linearly from pivot to w
                amp = ((x - right_pivot) / (w - right_pivot)) * 15.0
                dy += amp * dip_factor
                
            # Leg movement (Symmetrical spreading)
            # Legs are roughly in the bottom quarter (y >= 75)
            # Left leg is around x=30 to x=45, Right leg around x=55 to x=70
            if y >= 65:
                # How much does this point belong to the legs?
                # Max leg movement at bottom (y=100)
                leg_y_weight = (y - 65) / (h - 65) 
                
                # Left leg region
                if x < 50:
                    # Move left (negative dx) when dip_factor is positive
                    dx = -6.0 * dip_factor * leg_y_weight
                # Right leg region
                elif x > 50:
                    # Move right (positive dx) when dip_factor is positive
                    dx = 6.0 * dip_factor * leg_y_weight

            dst_points.append((x + dx, y + dy))
            
        dst_points = np.array(dst_points)
        
        # Use PiecewiseAffineTransform.from_estimate instead of estimate (skimage warning fix)
        tform = PiecewiseAffineTransform()
        tform.estimate(dst_points, src_points)
        
        out = warp(arr, tform, output_shape=(h, w))
        frames.append((out * 255).astype(np.uint8))
        
    sheet_arr = np.concatenate(frames, axis=1)
    sheet_img = Image.fromarray(sheet_arr, "RGBA")
    sheet_img.save(output_path)
    print("Warped sprite sheet saved to", output_path)

if __name__ == "__main__":
    generate_warped_sprite_sheet(r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken01.png", r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken01_anim.png")
