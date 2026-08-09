import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_warped_sprite_sheet(input_path, output_path, num_frames=16):
    img = Image.open(input_path).convert("RGBA")
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
    
    for i in range(num_frames):
        phase = i * (2 * math.pi) / num_frames
        
        # Sine wave mapping for breathing
        dip_factor = math.sin(phase)
        
        dst_points = []
        for x, y in src_points:
            # Scale Y by 5% and Scale X by 3% based on sine wave
            # Center of scaling is the middle of the image (w/2, h/2)
            dx = (x - w/2) * (0.03 * dip_factor)
            dy = (y - h/2) * (0.05 * dip_factor)

            dst_points.append((x + dx, y + dy))
            
        dst_points = np.array(dst_points)
        
        tform = PiecewiseAffineTransform()
        tform.estimate(dst_points, src_points)
        
        out = warp(arr, tform, output_shape=(h, w))
        frames.append((out * 255).astype(np.uint8))
        
    sheet_arr = np.concatenate(frames, axis=1)
    sheet_img = Image.fromarray(sheet_arr, "RGBA")
    sheet_img.save(output_path)
    print("Warped sprite sheet saved to", output_path)

if __name__ == "__main__":
    generate_warped_sprite_sheet(r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken11.png", r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken11_anim.png")
