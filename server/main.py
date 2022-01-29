from attr import Attribute
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

coef = [0.0264, 0.1405, 0.3331, 0.3331, 0.1405, 0.0264]
# coef = [1/6, 1/6, 1/6, 1/6, 1/6, 1/6]


timeout = 1
# coef = [0.0680, 0.8640, 0.0680]
order_filter = len(coef) - 1
saved = [0 for _ in range(order_filter)]

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
    print(angle, filtered)
    
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

for i in range(10):
    try:
        response = requests.get(get_url , timeout=timeout, verify=False)
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
    except requests.exceptions.ConnectionError:
        print("connection error")
        pass
    except requests.exceptions.ReadTimeout:
        print("read timeout")
        pass
    except OSError:
        print("saved error")
        pass

while 1:
    try:
        t = time.time()
        response = requests.get(get_url , timeout=timeout,  verify=False)
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
        x = requests.post(post_url, data=json.dumps({"angle": angle}), timeout=timeout, verify=False).json()
        # print(angle, time.time() - t)
        # time.sleep(0.25)
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
    except AttributeError:
        print("att error")
        pass
 