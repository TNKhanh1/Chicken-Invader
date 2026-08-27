import numpy as np
import imageio.v3 as iio
from skimage.transform import PiecewiseAffineTransform, warp
from PIL import Image
import os

def create_chicken12_anim():
    img_path = 'assets/enemy/chicken12.png'
    if not os.path.exists(img_path):
        print(f"Error: {img_path} not found")
        return

    # Load image
    img = iio.imread(img_path)
    h, w = img.shape[:2]
    
    # Pad image to allow movement
    pad = 20
    padded_h, padded_w = h + pad*2, w + pad*2
    padded_img = np.zeros((padded_h, padded_w, 4), dtype=np.uint8)
    padded_img[pad:pad+h, pad:pad+w] = img

    # Define base points
    center_x, center_y = 105 + pad, 99 + pad
    left_wing_x, left_wing_y = 63 + pad, 99 + pad
    right_wing_x, right_wing_y = 147 + pad, 99 + pad
    head_x, head_y = 105 + pad, 40 + pad
    leg_x, leg_y = 105 + pad, 160 + pad

    # Corners and edges to keep stable
    base_points = [
        (0, 0), (padded_w//2, 0), (padded_w-1, 0),
        (0, padded_h//2), (padded_w-1, padded_h//2),
        (0, padded_h-1), (padded_w//2, padded_h-1), (padded_w-1, padded_h-1),
        (center_x, center_y) # Body stays
    ]

    # Source points (moving parts)
    src_moving = [
        (left_wing_x, left_wing_y),
        (right_wing_x, right_wing_y),
        (head_x, head_y),
        (leg_x, leg_y)
    ]
    
    src = np.array(base_points + src_moving)
    
    frames = []
    num_frames = 10
    
    for i in range(num_frames):
        progress = i / float(num_frames)
        # Sine wave for wing flapping
        wing_offset_y = np.sin(progress * np.pi * 2) * 20.0
        wing_offset_x = np.sin(progress * np.pi * 2) * 5.0
        
        # Head bobs slightly
        head_offset = np.cos(progress * np.pi * 2) * 5.0
        
        # Legs move opposite to head
        leg_offset = -np.cos(progress * np.pi * 2) * 3.0
        
        dst_moving = [
            (left_wing_x + wing_offset_x, left_wing_y + wing_offset_y),
            (right_wing_x - wing_offset_x, right_wing_y + wing_offset_y),
            (head_x, head_y + head_offset),
            (leg_x, leg_y + leg_offset)
        ]
        
        dst = np.array(base_points + dst_moving)
        
        tform = PiecewiseAffineTransform()
        tform.estimate(dst, src)
        
        warped = warp(padded_img, tform, output_shape=(padded_h, padded_w), preserve_range=True)
        warped = warped.astype(np.uint8)
        frames.append(warped)
        
    # Concatenate horizontally
    sprite_sheet = np.concatenate(frames, axis=1)
    
    out_path = 'assets/enemy/chicken12_anim.png'
    iio.imwrite(out_path, sprite_sheet)
    print(f"Generated {out_path} with shape {sprite_sheet.shape}")

if __name__ == '__main__':
    create_chicken12_anim()
