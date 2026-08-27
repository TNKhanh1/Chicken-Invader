import os
import numpy as np
from PIL import Image
from skimage import transform

def generate_chicken12_anim():
    print("Loading chicken12.png...")
    img = Image.open('assets/enemy/chicken12.png').convert('RGBA')

    # Resize maintaining aspect ratio to fit inside an 80x80 box
    target_size = 80
    aspect = img.width / img.height
    if aspect > 1:
        new_w = target_size
        new_h = int(target_size / aspect)
    else:
        new_h = target_size
        new_w = int(target_size * aspect)

    resized = img.resize((new_w, new_h), Image.Resampling.LANCZOS)
    canvas = Image.new('RGBA', (100, 100), (0, 0, 0, 0))
    offset_x = (100 - new_w) // 2
    offset_y = (100 - new_h) // 2
    canvas.paste(resized, (offset_x, offset_y), resized)

    src_img = np.array(canvas)
    frames = []

    print("Generating 24 frames using mesh warp...")
    for i in range(24):
        phase = i * (2 * np.pi / 24)
        dip_factor = np.sin(phase)
        
        grid_size = 11
        src_cols = np.linspace(0, 100, grid_size)
        src_rows = np.linspace(0, 100, grid_size)
        src_grid_col, src_grid_row = np.meshgrid(src_cols, src_rows)
        
        dst_grid_col = np.copy(src_grid_col)
        dst_grid_row = np.copy(src_grid_row)
        
        for r in range(grid_size):
            for c in range(grid_size):
                x = src_grid_col[r, c]
                y = src_grid_row[r, c]
                
                # Wing flap: hinge at x=30 and x=70
                dy = 0
                if x < 30:
                    dy = ((30 - x) / 30.0) * 15.0 * dip_factor
                elif x > 70:
                    dy = ((x - 70) / 30.0) * 15.0 * dip_factor
                    
                # Body bob
                dy += 4.0 * dip_factor
                
                # Leg spread
                if y > 65:
                    leg_w = (y - 65) / 35.0
                    if x < 50:
                        dst_grid_col[r, c] -= 5.0 * dip_factor * leg_w
                    else:
                        dst_grid_col[r, c] += 5.0 * dip_factor * leg_w
                
                dst_grid_row[r, c] += dy
                
        src_pts = np.column_stack([src_grid_col.ravel(), src_grid_row.ravel()])
        dst_pts = np.column_stack([dst_grid_col.ravel(), dst_grid_row.ravel()])
        
        import warnings
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            if hasattr(transform.PiecewiseAffineTransform, 'from_estimate'):
                tform = transform.PiecewiseAffineTransform.from_estimate(dst_pts, src_pts)
            else:
                tform = transform.PiecewiseAffineTransform()
                tform.estimate(dst_pts, src_pts)
        
        warped = transform.warp(src_img, tform, output_shape=(100, 100), mode='constant', cval=0)
        warped_img = Image.fromarray((warped * 255).astype(np.uint8))
        frames.append(warped_img)

    spritesheet = Image.new('RGBA', (2400, 100), (0,0,0,0))
    for i, f in enumerate(frames):
        spritesheet.paste(f, (i*100, 0))

    out_path = 'assets/enemy/chicken12_anim.png'
    spritesheet.save(out_path)
    print(f"Saved spritesheet to {out_path}")

if __name__ == '__main__':
    generate_chicken12_anim()
