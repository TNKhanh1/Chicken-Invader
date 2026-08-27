import cv2
import numpy as np

img = cv2.imread('assets/enemy/chicken12.png', cv2.IMREAD_UNCHANGED)
if img is not None:
    print(f'Shape: {img.shape}')
    # find bounding box of alpha channel
    alpha = img[:, :, 3]
    y_indices, x_indices = np.where(alpha > 0)
    if len(y_indices) > 0:
        top = np.min(y_indices)
        bottom = np.max(y_indices)
        left = np.min(x_indices)
        right = np.max(x_indices)
        print(f'BBox: L={left}, R={right}, T={top}, B={bottom}')
        width = right - left
        height = bottom - top
        print(f'Center: ({left + width/2}, {top + height/2})')
        # Wings usually take up the left and right 20%
        print(f'Left Wing Pivot Approx: X={left + width*0.3}, Y={top + height*0.5}')
        print(f'Right Wing Pivot Approx: X={left + width*0.7}, Y={top + height*0.5}')
        # Legs usually at bottom 20%
        print(f'Leg Y Approx: {bottom - height*0.2}')
        # Head usually at top 20%
        print(f'Head Y Approx: {top + height*0.2}')
else:
    print('Failed to load image')
