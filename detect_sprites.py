from PIL import Image
import os
import json

def get_bounding_boxes(image_path, num_frames_x, num_frames_y=1):
    img = Image.open(image_path).convert("RGBA")
    width, height = img.size
    
    # Simple heuristic to find the content
    # Find the min/max x,y of non-transparent pixels
    pixels = img.getdata()
    min_x, min_y = width, height
    max_x, max_y = 0, 0
    
    found = False
    for y in range(height):
        for x in range(width):
            _, _, _, a = pixels[y * width + x]
            if a > 0:
                min_x = min(min_x, x)
                max_x = max(max_x, x)
                min_y = min(min_y, y)
                max_y = max(max_y, y)
                found = True
                
    if not found:
        return None

    content_w = max_x - min_x + 1
    content_h = max_y - min_y + 1
    
    # Split into frames
    frame_w = content_w / num_frames_x
    frame_h = content_h / num_frames_y
    
    frames = []
    for fy in range(num_frames_y):
        for fx in range(num_frames_x):
            frames.append({
                "x": int(min_x + fx * frame_w),
                "y": int(min_y + fy * frame_h),
                "width": int(frame_w),
                "height": int(frame_h)
            })
    return frames

assets = {
    "kitty_idle.png": 4,
    "kitty_walk.png": 6,
    "kitty_sleep.png": 2,
    "kitty_react.png": 4
}

base_path = "d:/Pety/Assets/"
results = {}

for filename, count in assets.items():
    path = base_path + filename
    if os.path.exists(path):
        # react is likely 2x2 grid based on my previous prompt and preview
        if "react" in filename:
            frames = get_bounding_boxes(path, 2, 2)
        else:
            frames = get_bounding_boxes(path, count, 1)
        results[filename.replace("kitty_", "").replace(".png", "")] = frames

print(json.dumps(results, indent=4))
