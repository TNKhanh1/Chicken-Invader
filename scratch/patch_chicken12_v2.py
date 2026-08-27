import os
import numpy as np
from PIL import Image
from skimage import transform

def generate_chicken12_anim():
    print("Loading chicken12.png...")
    img = Image.open('assets/enemy/chicken12.png').convert('RGBA')

    # We will use a 250x250 canvas to retain high resolution
    CANVAS_SIZE = 250
    # Keep the image mostly its native size, but fit within 210 to have safe margins
    target_size = 210
    aspect = img.width / img.height
    if aspect > 1:
        new_w = target_size
        new_h = int(target_size / aspect)
    else:
        new_h = target_size
        new_w = int(target_size * aspect)

    resized = img.resize((new_w, new_h), Image.Resampling.LANCZOS)
    canvas = Image.new('RGBA', (CANVAS_SIZE, CANVAS_SIZE), (0, 0, 0, 0))
    offset_x = (CANVAS_SIZE - new_w) // 2
    offset_y = (CANVAS_SIZE - new_h) // 2
    canvas.paste(resized, (offset_x, offset_y), resized)

    src_img = np.array(canvas)
    frames = []

    scale = CANVAS_SIZE / 100.0  # which is 2.5

    print(f"Generating 24 frames using mesh warp at {CANVAS_SIZE}x{CANVAS_SIZE}...")
    for i in range(24):
        phase = i * (2 * np.pi / 24)
        dip_factor = np.sin(phase)
        
        grid_size = 11
        src_cols = np.linspace(0, CANVAS_SIZE, grid_size)
        src_rows = np.linspace(0, CANVAS_SIZE, grid_size)
        src_grid_col, src_grid_row = np.meshgrid(src_cols, src_rows)
        
        dst_grid_col = np.copy(src_grid_col)
        dst_grid_row = np.copy(src_grid_row)
        
        for r in range(grid_size):
            for c in range(grid_size):
                x = src_grid_col[r, c]
                y = src_grid_row[r, c]
                
                # Wing flap
                dy = 0
                left_hinge = 30 * scale
                right_hinge = 70 * scale
                max_wing_dy = 15.0 * scale
                
                if x < left_hinge:
                    dy = ((left_hinge - x) / left_hinge) * max_wing_dy * dip_factor
                elif x > right_hinge:
                    dy = ((x - right_hinge) / (CANVAS_SIZE - right_hinge)) * max_wing_dy * dip_factor
                    
                # Body bob
                dy += 4.0 * scale * dip_factor
                
                # Leg spread
                leg_y = 65 * scale
                if y > leg_y:
                    leg_w = (y - leg_y) / (CANVAS_SIZE - leg_y)
                    center_x = 50 * scale
                    max_leg_dx = 5.0 * scale
                    if x < center_x:
                        dst_grid_col[r, c] -= max_leg_dx * dip_factor * leg_w
                    else:
                        dst_grid_col[r, c] += max_leg_dx * dip_factor * leg_w
                
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
        
        warped = transform.warp(src_img, tform, output_shape=(CANVAS_SIZE, CANVAS_SIZE), mode='constant', cval=0)
        warped_img = Image.fromarray((warped * 255).astype(np.uint8))
        frames.append(warped_img)

    spritesheet = Image.new('RGBA', (CANVAS_SIZE * 24, CANVAS_SIZE), (0,0,0,0))
    for i, f in enumerate(frames):
        spritesheet.paste(f, (i*CANVAS_SIZE, 0))

    out_path = 'assets/enemy/chicken12_anim.png'
    spritesheet.save(out_path)
    print(f"Saved spritesheet to {out_path}")

if __name__ == '__main__':
    generate_chicken12_anim()
