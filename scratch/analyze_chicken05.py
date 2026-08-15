import os
import numpy as np
from PIL import Image

def analyze_chicken(img_path):
    print(f"Analyzing {img_path}...")
    if not os.path.exists(img_path):
        print("File not found!")
        return

    img = Image.open(img_path).convert('RGBA')
    arr = np.array(img)
    alpha = arr[:, :, 3]
    y_idx, x_idx = np.where(alpha > 0)
    
    if len(y_idx) == 0:
        print("Empty image.")
        return
        
    min_x, max_x = np.min(x_idx), np.max(x_idx)
    min_y, max_y = np.min(y_idx), np.max(y_idx)
    
    print(f"Image size: {img.size}")
    print(f"Bounding Box: X({min_x} - {max_x}), Y({min_y} - {max_y})")
    width = max_x - min_x + 1
    height = max_y - min_y + 1
    print(f"Content Size: {width}x{height}")
    
    # Calculate mass center for pivots
    center_x = (min_x + max_x) // 2
    print(f"Center X: {center_x}")
    print(f"Suggested Left Pivot: {center_x - width // 4}, Right Pivot: {center_x + width // 4}")

if __name__ == '__main__':
    analyze_chicken('assets/enemy/chicken05.png')
