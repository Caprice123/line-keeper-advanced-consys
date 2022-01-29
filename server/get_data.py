import requests
import time
import urllib.request
import numpy as np
import cv2
import json
import os
from tqdm  import tqdm
from threading import Thread
from ImageHandler.ImageHandler import ImageHandler

timeout = 0.5

order_filter = 5
saved = [0 for _ in range(order_filter)]
coef = [0.0264, 0.1405, 0.3331, 0.3331, 0.1405, 0.0264]
filter_coef = coef[1:]
filter_now = coef[0]

def filter_angle(angle):
    sum = filter_now * angle
    for i in range(order_filter):
        sum += filter_coef[i] * saved[i]
    return sum 
    
def update_filter(angle):
    
    if angle == -1:
        angle = saved[0]
        
    for i in range(order_filter-2, -1, -1):
        saved[i + 1] = saved[i]
    saved[0]= angle
    
    filtered = filter_angle(angle)
    return angle


# url = "http://192.168.100.114/"

url = "http://192.168.1.26/"
get_url = url + "cam-lo.jpg"
post_url = url + "upload"

img_dir = "images"
img_path = os.path.join(img_dir, f"img_reg_1.jpg")

# fps = 4
# delay_max = 1 / fps
# requests.post("http://192.168.100.123/upload", data=json.dumps({"angle": 20}), headers={"Keep-alive": "9999999999999", "Connection": "keep-alive"}).json()

print("sending")

res = []



angles = []
times = []

current_time = time.time()
for i in range(1000):
    try:
        t = time.time()
        response = requests.get(get_url , timeout=timeout)
        img = response.content
        with open(img_path, 'wb') as f:
            f.write(img)
        # imgResp=urllib.request.urlopen(url)
        # imgNp=np.array(bytearray(imgResp.read()),dtype=np.uint8)
        # img=cv2.imdecode(imgNp,-1)
        
        img = cv2.imread(img_path)
        app = ImageHandler(image = img)
        angle = app.get_steering_angle()
        angle = update_filter(angle)
        # print(angle)
        x = requests.post(post_url, data=json.dumps({"angle": angle}), timeout=timeout).json()
        print(angle, time.time() - t)
        
        # time.sleep(0.5)
        angles.append(angle)
        times.append(time.time() - current_time)
        # cv2.imwrite(os.path.join(img_dir, f"img_reg_1.jpg"), img)
        
        # delay_time = delay_max - (time.time() - t)
        
        # if (delay_time > 0):
        #     time.sleep(delay_time)
    except requests.exceptions.ConnectionError:
        print("connection error")
        pass
    except requests.exceptions.ReadTimeout:
        print("read timeout")
        pass
    except OSError:
        print("saved error")
        pass
  
import pandas as pd

import numpy as np

angles = np.array(angles)
times = np.array(times)
df = pd.DataFrame({'times': times, 'angles': angles})
df.to_excel("test2.xlsx", index=False)  
print(angles)
print(times)
# response = requests.Session()

    
# for i in tqdm(range(1000)):
#     try:
#         t = time.time()
#         response = requests.get(url , timeout=2)
#         img = response.content
#         with open(os.path.join(img_dir, f"img_reg_1.jpg"), 'wb') as f:
#             f.write(img)
#         # imgResp=urllib.request.urlopen(url)
#         # imgNp=np.array(bytearray(imgResp.read()),dtype=np.uint8)
#         # img=cv2.imdecode(imgNp,-1)
        
#         img = cv2.imread(os.path.join(img_dir, f"img_reg_1.jpg"))
#         # print(img)
#         app = ImageHandler(image = img)
#         angle = app.get_steering_angle()
#         angle = update_filter(angle)
        
#         # print(angle)
#         x = requests.post("http://192.168.1.15/upload", data=json.dumps({"angle": str(angle)}), timeout=2).json()
#         # print(angle, time.time() - t)
#         # cv2.imwrite(os.path.join(img_dir, f"img_reg_1.jpg"), img)
#         res.append(time.time() - t)
#         # delay_time = delay_max - (time.time() - t)
        
#         # if (delay_time > 0):
#         #     time.sleep(delay_time)
#     except requests.exceptions.ConnectionError:
#         print("connection error")
#         pass
#     except requests.exceptions.ReadTimeout:
#         print("read timeout")
#         pass
#     except OSError:
#         print("saved error")
#         pass
   
# print(sum(res) / len(res)) 