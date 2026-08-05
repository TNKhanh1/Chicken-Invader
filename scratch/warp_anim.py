import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp

def generate_warped_sprite_sheet(input_path, output_path, num_frames=12):
    img = Image.open(input_path).convert("RGBA")
    arr = np.array(img) / 255.0  # normalize for skimage

    h, w, c = arr.shape
    
    # Define a grid for the source image
    # Let's use a 5x5 grid (4x4 cells)
    cols = np.linspace(0, w, 5)
    rows = np.linspace(0, h, 5)
    
    src_points = []
    for r in rows:
        for c_val in cols:
            src_points.append((c_val, r))
    src_points = np.array(src_points)
    
    # We will generate 12 frames
    frames = []
    
    for i in range(num_frames):
        phase = i * (2 * math.pi) / num_frames
        
        dst_points = []
        for x, y in src_points:
            # We want to deform the wings (x=0, x=w) and legs (y=h, x=w/4, x=3w/4)
            dx = 0
            dy = 0
            
            # Wing tips (left)
            if x <= w/4:
                # amplitude increases towards x=0
                amp = (w/4 - x) / (w/4) * 20
                dy = math.sin(phase) * amp
                
            # Wing tips (right)
            elif x >= 3*w/4:
                # amplitude increases towards x=w
                amp = (x - 3*w/4) / (w/4) * 20
                dy = math.sin(phase) * amp
                
            # Legs
            if y >= 3*h/4:
                # Left leg
                if abs(x - w/4) <= w/8:
                    dy = -math.sin(phase + math.pi/2) * 5
                # Right leg
                elif abs(x - 3*w/4) <= w/8:
                    dy = math.sin(phase + math.pi/2) * 5
                    
            dst_points.append((x + dx, y + dy))
            
        dst_points = np.array(dst_points)
        
        tform = PiecewiseAffineTransform()
        tform.estimate(dst_points, src_points)
        
        # Warp the image. Note: output shape is (h, w)
        out = warp(arr, tform, output_shape=(h, w))
        frames.append((out * 255).astype(np.uint8))
        
    # Combine horizontally
    sheet_arr = np.concatenate(frames, axis=1)
    sheet_img = Image.fromarray(sheet_arr, "RGBA")
    sheet_img.save(output_path)
    print("Warped sprite sheet saved to", output_path)

if __name__ == "__main__":
    generate_warped_sprite_sheet(r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken01.png", r"e:\ChickenInvader_local\ChickenInvader\scratch\chicken01_anim_warped.png")
