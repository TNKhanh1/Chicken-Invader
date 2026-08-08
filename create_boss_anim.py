import os
import numpy as np
import math
from PIL import Image
from skimage.transform import PiecewiseAffineTransform, warp
from skimage.measure import label, regionprops

def generate_boss_sprite_sheet(input_path, output_path, num_frames=24):
    try:
        img_orig = Image.open(input_path).convert("RGBA")
    except FileNotFoundError:
        print(f"File not found: {input_path}")
        return
        
    # --- 1. ISOLATE BODY AND FEATHERS ---
    arr_orig = np.array(img_orig)
    w_orig, h_orig = img_orig.size
    
    mask = arr_orig[:, :, 3] > 0
    lbl = label(mask)
    props = regionprops(lbl)
    
    if not props:
        print("No regions found")
        return
        
    # Remove logo in bottom right corner
    for p in props:
        min_row, min_col, max_row, max_col = p.bbox
        if min_col > w_orig - 100 and min_row > h_orig - 100:
            logo_mask = (lbl == p.label)
            arr_orig[logo_mask] = 0
            mask[logo_mask] = False
            
    # Re-evaluate labels after removing logo
    lbl = label(mask)
    props = regionprops(lbl)
    
    largest_label = max(props, key=lambda p: p.area).label
    
    base_mask = (lbl == largest_label)
    base_arr = np.zeros_like(arr_orig)
    base_arr[base_mask] = arr_orig[base_mask]
    
    feathers_mask = (mask) & (lbl != largest_label)
    feathers_arr = np.zeros_like(arr_orig)
    feathers_arr[feathers_mask] = arr_orig[feathers_mask]
    
    base_img = Image.fromarray(base_arr, "RGBA")
    feathers_img = Image.fromarray(feathers_arr, "RGBA")
    
    # --- 2. PAD AND RESIZE TO 350x350 ---
    w_orig, h_orig = img_orig.size
    size = max(w_orig, h_orig)
    offset = ((size - w_orig) // 2, (size - h_orig) // 2)
    
    def pad_and_resize(img):
        padded = Image.new("RGBA", (size, size), (0,0,0,0))
        padded.paste(img, offset)
        return padded.resize((350, 350), Image.LANCZOS)
        
    base_350 = pad_and_resize(base_img)
    feathers_350 = pad_and_resize(feathers_img)
    
    # --- 3. PREPARE WARP PARAMS ---
    arr_base = np.array(base_350) / 255.0
    h, w, c = arr_base.shape
    
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
        # A. WARP MAIN BODY
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
        
        warped_base_arr = warp(arr_base, tform, output_shape=(h, w))
        warped_base_img = Image.fromarray((warped_base_arr * 255).astype(np.uint8), "RGBA")
        
        # B. ANIMATE FEATHERS (Falling & Fading)
        # Using 2 overlapping streams for a continuous loop
        comp_feathers = Image.new("RGBA", (w, h), (0,0,0,0))
        for offset_frame in [0, num_frames // 2]:
            frame_idx = (i + offset_frame) % num_frames
            phi = frame_idx / float(num_frames) # 0.0 to 1.0
            
            y_shift = int(phi * 25) # drop by 25 pixels
            alpha_mult = 1.0 - phi  # fade out
            
            f_arr = np.array(feathers_350).astype(np.float32)
            f_arr[:, :, 3] *= alpha_mult
            shifted_f = Image.fromarray(f_arr.astype(np.uint8), "RGBA")
            
            temp = Image.new("RGBA", (w, h), (0,0,0,0))
            temp.paste(shifted_f, (0, y_shift))
            comp_feathers = Image.alpha_composite(comp_feathers, temp)
            
        # C. COMPOSITE TOGETHER
        final_frame = Image.alpha_composite(warped_base_img, comp_feathers)
        frames.append(np.array(final_frame))
        
    # --- 4. SAVE ---
    sheet_arr = np.concatenate(frames, axis=1)
    sheet_img = Image.fromarray(sheet_arr, "RGBA")
    sheet_img.save(output_path)
    print(f"Generated: {output_path} ({w*num_frames}x{h})")

if __name__ == "__main__":
    in_path = r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken10.png"
    out_path = r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken10_anim.png"
    generate_boss_sprite_sheet(in_path, out_path)
