import numpy as np
from PIL import Image
from skimage.measure import label, regionprops

def test_isolate():
    img = Image.open(r"e:\ChickenInvader_local\ChickenInvader\assets\enemy\chicken10.png").convert("RGBA")
    arr = np.array(img)
    
    # Alpha channel > 0
    mask = arr[:, :, 3] > 0
    
    # Label connected components
    lbl = label(mask)
    props = regionprops(lbl)
    
    if not props:
        print("No regions found")
        return
        
    # Find largest component
    largest_label = max(props, key=lambda p: p.area).label
    
    # Create base image (main body)
    base_mask = (lbl == largest_label)
    base_arr = np.zeros_like(arr)
    base_arr[base_mask] = arr[base_mask]
    
    # Create feathers image (everything else)
    feathers_mask = (mask) & (lbl != largest_label)
    feathers_arr = np.zeros_like(arr)
    feathers_arr[feathers_mask] = arr[feathers_mask]
    
    Image.fromarray(base_arr, "RGBA").save(r"e:\ChickenInvader_local\ChickenInvader\scratch\base_test.png")
    Image.fromarray(feathers_arr, "RGBA").save(r"e:\ChickenInvader_local\ChickenInvader\scratch\feathers_test.png")
    
    print(f"Total regions: {len(props)}")
    print(f"Largest region area: {max([p.area for p in props])}")
    print(f"Feathers region area: {np.sum(feathers_mask)}")

if __name__ == "__main__":
    import os
    os.makedirs(r"e:\ChickenInvader_local\ChickenInvader\scratch", exist_ok=True)
    test_isolate()
