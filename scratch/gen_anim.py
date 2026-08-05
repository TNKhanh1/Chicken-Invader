import os
from PIL import Image, ImageDraw
import numpy as np
import math

def generate_sprite_sheet(input_path, output_path, num_frames=12):
    img = Image.open(input_path).convert("RGBA")
    w, h = img.size
    
    # Define masks for different body parts (approximate based on typical 100x100 chicken)
    # These coordinates might need tweaking
    def create_mask(poly):
        mask = Image.new("L", (w, h), 0)
        draw = ImageDraw.Draw(mask)
        draw.polygon(poly, fill=255)
        return mask

    # Polygons (x, y)
    poly_left_wing = [(0, 35), (32, 35), (32, 65), (0, 65)]
    poly_right_wing = [(68, 35), (100, 35), (100, 65), (68, 65)]
    poly_left_leg = [(25, 75), (45, 75), (45, 100), (25, 100)]
    poly_right_leg = [(55, 75), (75, 75), (75, 100), (55, 100)]
    
    # Body is everything minus wings and legs
    mask_left_wing = create_mask(poly_left_wing)
    mask_right_wing = create_mask(poly_right_wing)
    mask_left_leg = create_mask(poly_left_leg)
    mask_right_leg = create_mask(poly_right_leg)
    
    # Create body mask by subtracting others from a full mask
    arr_img = np.array(img)
    alpha = arr_img[:, :, 3]
    
    m_lw = np.array(mask_left_wing)
    m_rw = np.array(mask_right_wing)
    m_ll = np.array(mask_left_leg)
    m_rl = np.array(mask_right_leg)
    
    m_body = np.where((m_lw > 0) | (m_rw > 0) | (m_ll > 0) | (m_rl > 0), 0, alpha)
    
    def extract_part(mask_arr):
        part = arr_img.copy()
        part[:, :, 3] = np.minimum(part[:, :, 3], mask_arr)
        return Image.fromarray(part)

    part_body = extract_part(m_body)
    part_lw = extract_part(m_lw)
    part_rw = extract_part(m_rw)
    part_ll = extract_part(m_ll)
    part_rl = extract_part(m_rl)
    
    # Create sprite sheet
    sheet = Image.new("RGBA", (w * num_frames, h), (0,0,0,0))
    
    for i in range(num_frames):
        frame = Image.new("RGBA", (w, h), (0,0,0,0))
        
        # Calculate animation angles (sin wave)
        # Wings flap up and down
        wing_angle = math.sin(i * (2 * math.pi) / num_frames) * 20 # +/- 20 degrees
        
        # Legs move forward/backward (opposite phase)
        leg_angle = math.sin(i * (2 * math.pi) / num_frames) * 15
        
        # Paste Body
        frame.paste(part_body, (0, 0), part_body)
        
        # Pivot points for rotation
        pivot_lw = (32, 50)
        pivot_rw = (68, 50)
        pivot_ll = (35, 75)
        pivot_rl = (65, 75)
        
        def paste_rotated(part, pivot, angle, offset=(0,0)):
            # Rotate image around pivot
            # Pillow rotate is around center by default. To rotate around pivot, we translate, rotate, translate back.
            # Simplified: just rotate and let it be for now, or use transform.
            # For exact pivot rotation, use numpy or affine transform.
            pass
            
        # Simplified Pillow rotation around center
        # Let's crop to tight bounding boxes for each part, rotate, and paste
        def paste_rotated_bbox(part_img, angle, center_x, center_y):
            bbox = part_img.getbbox()
            if not bbox: return
            cropped = part_img.crop(bbox)
            rotated = cropped.rotate(angle, resample=Image.BICUBIC, expand=True)
            # Paste it back centered at its original center
            # orig_center = ((bbox[0]+bbox[2])/2, (bbox[1]+bbox[3])/2)
            # new center is same, so top-left = center - (w/2, h/2)
            nw, nh = rotated.size
            top_left = (int(center_x - nw/2), int(center_y - nh/2))
            frame.paste(rotated, top_left, rotated)
            
        # Centers for rotation
        paste_rotated_bbox(part_lw, -wing_angle, 16, 50)
        paste_rotated_bbox(part_rw, wing_angle, 84, 50)
        paste_rotated_bbox(part_ll, -leg_angle, 35, 87)
        paste_rotated_bbox(part_rl, leg_angle, 65, 87)
        
        sheet.paste(frame, (i * w, 0))
        
    sheet.save(output_path)

if __name__ == "__main__":
    generate_sprite_sheet(r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken01.png", r"e:\ChickenInvader_local\ChickenInvader\scratch\chicken01_anim.png")
    print("Sprite sheet generated!")
