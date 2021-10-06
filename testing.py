import os, requests
from time import sleep
url = 'http://127.0.0.1:8000/upload'
path_img = "C:\\Users\\kelvin\\Desktop\\tugas semester 5\\advance control system\\UAS line keeper\\line-keeper-advanced-consys\\img_reg_lurus.jpg"

send_count = 1

def sendImage():
    with open(path_img, 'rb') as img:
        name_img= os.path.basename(path_img)
        
        files= {'file': (name_img,img,'multipart/form-data',{'Expires': '0'}) }
        with requests.Session() as s:
            r = s.post(url,files=files)
            print(r.status_code)

for i in range(send_count):
    sendImage()
    sleep(0.1)